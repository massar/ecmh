/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: interfaces.h,v 1.2 2004/01/11 21:41:05 fuzzel Exp $
 $Date: 2004/01/11 21:41:05 $
**************************************/

// The list of interfaces we do multicast on
// These are discovered on the fly, very handy ;)
struct intnode
{
	int		ifindex;		// Interface Index
	char		name[IFNAMSIZ];		// Name of the interface
	int		groupcount;		// Number of groups this interface joined
	int		mtu;			// The MTU of this interface
	bool		removeme;		// Is this device to be removed?

	struct sockaddr	hwaddr;			// Hardware bytes
	struct in6_addr	linklocal;		// Link local address
};

/* Node functions */
void int_add(struct intnode *intn);
struct intnode *int_create(int ifindex);
void int_destroy(struct intnode *intn);

/* List functions */
struct intnode *int_find(int ifindex);
