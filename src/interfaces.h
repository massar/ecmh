/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: interfaces.h,v 1.3 2004/02/15 19:51:06 fuzzel Exp $
 $Date: 2004/02/15 19:51:06 $
**************************************/

// The list of interfaces we do multicast on
// These are discovered on the fly, very handy ;)
struct intnode
{
	int		ifindex;		// Interface Index
	char		name[IFNAMSIZ];		// Name of the interface
	int		groupcount;		// Number of groups this interface joined
	int		mtu;			// The MTU of this interface

	int		mld_version;		// The MLD version this interface supports

	struct sockaddr	hwaddr;			// Hardware bytes
	struct in6_addr	linklocal;		// Link local address

	// Per interface statistics
	uint64_t		stat_packets_received;		// Number of packets received
	uint64_t		stat_packets_sent;		// Number of packets forwarded
	uint64_t		stat_bytes_received;		// Number of bytes received
	uint64_t		stat_bytes_sent;		// Number of bytes forwarded
};

/* Node functions */
void int_add(struct intnode *intn);
struct intnode *int_create(int ifindex);
void int_destroy(struct intnode *intn);

/* List functions */
struct intnode *int_find(int ifindex, bool resort);
