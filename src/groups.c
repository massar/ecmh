/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: groups.c,v 1.6 2004/10/07 09:28:21 fuzzel Exp $
 $Date: 2004/10/07 09:28:21 $
**************************************/

#include "ecmh.h"

/* Create a groupnode */
struct groupnode *group_create(const struct in6_addr *mca)
{
	struct groupnode *groupn = malloc(sizeof(struct groupnode));

	if (!groupn) return NULL;

	/* Fill her in */
	memcpy(&groupn->mca, mca, sizeof(*mca));

	/* Setup the list */
	groupn->interfaces = list_new();
	groupn->interfaces->del = (void(*)(void *))grpint_destroy;

D(
	{
		char mca_txt[INET6_ADDRSTRLEN];
		memset(mca_txt,0,sizeof(mca_txt));
		inet_ntop(AF_INET6, mca, mca_txt, sizeof(mca_txt));
		dolog(LOG_DEBUG, "Created group %s\n", mca_txt);
	}
)

	/* All okay */
	return groupn;
}

void group_destroy(struct groupnode *groupn)
{
	if (!groupn) return;

D(
	{
		char mca_txt[INET6_ADDRSTRLEN];
		memset(mca_txt,0,sizeof(mca_txt));
		inet_ntop(AF_INET6, &groupn->mca, mca_txt, sizeof(mca_txt));
		dolog(LOG_DEBUG, "Destroying group %s\n", mca_txt);
	}
)

	/* Empty the subscriber list */
	list_delete_all_node(groupn->interfaces);

	/* Free the node */
	free(groupn);
}

struct groupnode *group_find(const struct in6_addr *mca)
{
	struct groupnode	*groupn;
	struct listnode		*ln;

	LIST_LOOP(g_conf->groups, groupn, ln)
	{
		if (IN6_ARE_ADDR_EQUAL(mca, &groupn->mca)) return groupn;
	}
	return NULL;
}

/*
 * Find the groupint or create it
 * mca		= The IPv6 address of the Multicast group
 * interface	= the interface we received it on
 */
struct grpintnode *groupint_get(const struct in6_addr *mca, struct intnode *interface)
{
	struct groupnode	*groupn;
	struct grpintnode	*grpintn;
	struct intnode		*intn;
	struct listnode		*ln;
	bool			forward = false;

	/* Find our beloved group */
	groupn = group_find(mca);

	if (!groupn)
	{
		/* Create the group node */
		groupn = group_create(mca);

		/* Add the group to the list */
		if (groupn) listnode_add(g_conf->groups, (void *)groupn);
		
		forward = true;
	}

	/* Forward it if we haven't done so for quite some time */
	else if ((time(NULL) - groupn->lastforward) >= ECMH_SUBSCRIPTION_TIMEOUT)
	{
		dolog(LOG_DEBUG, "Last update was %d seconds ago -> resending\n", (int)(time(NULL) - groupn->lastforward));
		forward = true;
	}

	if (forward)
	{
		dolog(LOG_DEBUG, "Broadcasting group to other interfaces...\n");

		/* Broadcast that we want this new group */
		LIST_LOOP(g_conf->ints, intn, ln)
		{
			/* Skip the interface it came from */
			if (interface->ifindex == intn->ifindex) continue;

			/* Send the MLD Report */
			mld_send_report(intn, mca);
		}
		groupn->lastforward = time(NULL);
	}

	if (!groupn) return false;

	/* Find the interface in this group */
	grpintn = grpint_find(groupn->interfaces, interface);

	if (!grpintn)
	{
		/* Create the groupinterface node */
		grpintn = grpint_create(interface);

		/* Add the group to the list */
		if (grpintn) listnode_add(groupn->interfaces, (void *)grpintn);
	}
	return grpintn;
}

