/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@massar.ch>
**************************************/

/* The node used to hold the interfaces which a group joined */
struct grpintnode
{
	uint64_t		ifindex;		/* The interface */
	struct list		*subscriptions;		/* Subscriber list */
};

struct grpintnode *grpint_create(const struct intnode *interface);
void grpint_destroy(struct grpintnode *grpintn);
struct grpintnode *grpint_find(const struct list *list, const struct intnode *interface);
bool grpint_refresh(struct grpintnode *grpintn, const struct in6_addr *ipv6, unsigned int mode);

