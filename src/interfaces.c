/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: interfaces.c,v 1.6 2004/02/17 00:22:29 fuzzel Exp $
 $Date: 2004/02/17 00:22:29 $
**************************************/

#include "ecmh.h"

void int_add(struct intnode *intn)
{
	char ll[INET6_ADDRSTRLEN];
	listnode_add(g_conf->ints, intn);

	inet_ntop(AF_INET6, &intn->linklocal, ll, sizeof(ll));

	dolog(LOG_DEBUG, "Added %s, link %d, hw %s/%d with an MTU of %d, linklocal: %s\n",
		intn->name, intn->ifindex,
		(intn->hwaddr.sa_family == ARPHRD_ETHER ? "ether" : 
		 (intn->hwaddr.sa_family == ARPHRD_SIT ? "sit" : "???")),
		intn->hwaddr.sa_family,
		intn->mtu,
		ll);
}

struct intnode *int_create(int ifindex)
{
	struct intnode	*intn = malloc(sizeof(struct intnode));
	struct ifreq	ifreq;
	int		i;

	if (!intn) return NULL;
	memset(intn, 0, sizeof(*intn));

	intn->ifindex = ifindex;

	// Default to 0, we discover this after the queries has been sent
	intn->mld_version = 0;

	// Get the interface name (eth0/sit0/...)
	// Will be used for reports etc
	memset(&ifreq, 0, sizeof(ifreq));
	ifreq.ifr_ifindex = ifindex;
	if (ioctl(g_conf->rawsocket, SIOCGIFNAME, &ifreq) != 0)
	{
		dolog(LOG_ERR, "Couldn't determine interfacename of link %d : %s\n", intn->ifindex, strerror(errno));
		int_destroy(intn);
		return NULL;
	}
	memcpy(&intn->name, &ifreq.ifr_name, sizeof(intn->name));

	// Get the MTU size of this interface
	// We will use that for fragmentation
	if (ioctl(g_conf->rawsocket, SIOCGIFMTU, &ifreq) != 0)
	{
		dolog(LOG_ERR, "Couldn't determine MTU size for %s, link %d : %s\n", intn->name, intn->ifindex, strerror(errno));
		perror("Error");
		int_destroy(intn);
		return NULL;
	}
	intn->mtu = ifreq.ifr_mtu;

	// Get hardware address
	if (ioctl(g_conf->rawsocket, SIOCGIFHWADDR, &ifreq) != 0)
	{
		dolog(LOG_ERR, "Couldn't determine hardware address for %s, link %d : %s\n", intn->name, intn->ifindex, strerror(errno));
		perror("Error");
		int_destroy(intn);
		return NULL;
	}
	memcpy(&intn->hwaddr, &ifreq.ifr_hwaddr, sizeof(intn->hwaddr));

	// All okay	
	return intn;
}

void int_destroy(struct intnode *intn)
{
	if (!intn) return;

D(	dolog(LOG_DEBUG, "Destroying interface %s\n", intn->name);)
	free(intn);
	return;
}

struct intnode *int_find(int ifindex, bool resort)
{
	struct intnode	*intn;
	struct listnode	*ln;

	LIST_LOOP(g_conf->ints, intn, ln)
	{
		if (ifindex == intn->ifindex)
		{
			// Move the node to the beginning
			// This way it will be in the front
			// the next time we try to find it
			if (resort) list_movefront_node(g_conf->ints, ln);
			return intn;
		}
	}
	return NULL;
}

// Store the version of MLD if it is lower than the old one or the old one was 0 ;)
// We only respond MLDv1's this way when there is a MLDv1 router on the link.
// but we respond MLDv2's when there are only MLDv2 router on the link
// Even if we build without MLDv2 support we detect the version which
// could be used to determine if a router can't fallback to v1 for instance
// later on in diagnostics. When built without MLDv2 support we always do MLDv1 though.
// Everthing higher as MLDv2 is reported as MLDv2 as we don't know about it (yet :)
void int_set_mld_version(struct intnode *intn, int newversion)
{
	if (newversion == 1)
	{
		// Only reset the version number
		// if it wasn't set and not v1 yet
		if (	intn->mld_version == 0 &&
			intn->mld_version != 1)
		{
			D(
			if (intn->mld_version == 2) dolog(LOG_DEBUG, "MLDv1 Query detected on %5s, downgrading from MLDv2 to MLDv1\n", intn->name);
			else dolog(LOG_DEBUG, "MLDv1 detected on %s, setting it to MLDv1\n", intn->name);
			)
			intn->mld_version = 1;
		}
	}
	else
	{
		// Only reset the version number
		// if it wasn't set and not v1 yet and not v2 yet
		if (	intn->mld_version == 0 &&
			intn->mld_version != 1 &&
			intn->mld_version != 2)
		{
			D(dolog(LOG_DEBUG, "MLDv2 detected on %s, setting it to MLDv2\n", intn->name);)
			intn->mld_version = 2;
		}
	}
}
