/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: grpint.c,v 1.5 2004/02/17 00:22:29 fuzzel Exp $
 $Date: 2004/02/17 00:22:29 $
**************************************/

#include "ecmh.h"

struct grpintnode *grpint_create(const struct intnode *interface)
{
	struct grpintnode *grpintn = malloc(sizeof(*grpintn));

	if (!grpintn) return NULL;

	// Fill her in
	grpintn->interface = (struct intnode *)interface;

	// Setup the list
	grpintn->subscriptions = list_new();
	grpintn->subscriptions->del = (void(*)(void *))grpint_destroy;

	// All okay
	return grpintn;
}

void grpint_destroy(struct grpintnode *grpintn)
{
	if (!grpintn) return;

	D(dolog(LOG_DEBUG, "Destroying grpint for device %s\n", grpintn->interface->name);)

	// Empty the subscriber list
	list_delete_all_node(grpintn->subscriptions);

	// Free the node
	free(grpintn);
}

struct grpintnode *grpint_find(const struct list *list, const struct intnode *interface)
{
	struct grpintnode	*grpintn;
	struct listnode		*ln;

	LIST_LOOP(list, grpintn, ln)
	{
		if (grpintn->interface == interface) return grpintn;
	}
	return NULL;
}

// grpintn	= The GroupInterface node
// ipv6		= Source IPv6 address
//		  !ff3x::/96 : unspecified IPv6 address
//		  ff3x::/96  : The source IPv6 address that wants to receive this S<->G channel
// mode		= MLD_SSM_MODE_*
bool grpint_refresh(struct grpintnode *grpintn, const struct in6_addr *ipv6, int mode)
{
	struct subscrnode *subscrn;

	// Find our beloved group
	subscrn = subscr_find(grpintn->subscriptions, ipv6);

	if (!subscrn)
	{
		// Create the subscr node
		subscrn = subscr_create(ipv6, mode);

		// Add the group to the list
		if (subscrn)
		{
			listnode_add(grpintn->subscriptions, (void *)subscrn);
		}
	}

	if (!subscrn) return false;

	// Refresh it
	subscrn->refreshtime = time(NULL);

	// All Okay
	return true;
}
