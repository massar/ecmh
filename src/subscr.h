/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: subscr.h,v 1.2 2004/01/11 21:41:05 fuzzel Exp $
 $Date: 2004/01/11 21:41:05 $
**************************************/

// MLDv2 Source Specific Multicast Support
struct subscrnode
{
	struct in6_addr	ipv6;			// The address that wants packets matching this S<->G
	time_t		refreshtime;		// The time we last received a join for this S<->G on this interface
};

struct subscrnode *subscr_create(const struct in6_addr *ipv6);
void subscr_destroy(struct subscrnode *subscrn);
struct subscrnode *subscr_find(const struct list *list, const struct in6_addr *ipv6);
bool subscr_unsub(struct list *list, const struct in6_addr *ipv6);
