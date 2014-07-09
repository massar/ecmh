/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@massar.ch>
**************************************/

#include "ecmh.h"

/* Subscription Node */
struct subscrnode *subscr_create(const struct in6_addr *ipv6, int mode)
{
	struct subscrnode *subscrn = calloc(1, sizeof(*subscrn));

	if (!subscrn) return NULL;

	/* Fill her in */
	memcpy(&subscrn->ipv6, ipv6, sizeof(*ipv6));
	subscrn->mode = mode;
	subscrn->refreshtime = gettimes();

D(
	{
		char addr[INET6_ADDRSTRLEN];
		memset(addr,0,sizeof(addr));
		inet_ntop(AF_INET6, &subscrn->ipv6, addr, sizeof(addr));
		dolog(LOG_DEBUG, "Adding subscription %s (%s)\n", addr,
			subscrn->mode == MLD2_MODE_IS_INCLUDE ? "INCLUDE" : "EXCLUDE");
	}
)

	/* All okay */
	return subscrn;
}

void subscr_destroy(struct subscrnode *subscrn)
{
	if (!subscrn) return;

D(
	{
		char addr[INET6_ADDRSTRLEN];
		memset(addr,0,sizeof(addr));
		inet_ntop(AF_INET6, &subscrn->ipv6, addr, sizeof(addr));
		dolog(LOG_DEBUG, "Destroying subscription %s (%s)\n", addr,
			subscrn->mode == MLD2_MODE_IS_INCLUDE ? "INCLUDE" : "EXCLUDE");
	}
)

	/* Free the node */
	free(subscrn);
}

struct subscrnode *subscr_find(const struct list *list, const struct in6_addr *ipv6)
{
	struct subscrnode	*subscrn;
	struct listnode		*ln;

	LIST_LOOP(list, subscrn, ln)
	{
		if (IN6_ARE_ADDR_EQUAL(ipv6, &subscrn->ipv6)) return subscrn;
	}
	return NULL;
}

bool subscr_unsub(struct list *list, const struct in6_addr *ipv6)
{
	struct subscrnode	*subscrn;
	struct listnode		*ln;

	LIST_LOOP(list, subscrn, ln)
	{
		if (IN6_ARE_ADDR_EQUAL(ipv6, &subscrn->ipv6))
		{
			/* Delete the entry from the list */
			list_delete_node(list, ln);
			/* Destroy the item itself */
			subscr_destroy(subscrn);
			return true;
		}
	}
	return false;
}
