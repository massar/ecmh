/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: interfaces.c,v 1.8 2004/10/07 09:28:21 fuzzel Exp $
 $Date: 2004/10/07 09:28:21 $
**************************************/

#include "ecmh.h"

void int_add(struct intnode *intn)
{
	listnode_add(g_conf->ints, intn);

	dolog(LOG_DEBUG, "Added %s, link %u, hw %s/%u with an MTU of %d\n",
		intn->name, intn->ifindex,
#ifdef linux
		(intn->hwaddr.sa_family == ARPHRD_ETHER ? "ether" : 
		 (intn->hwaddr.sa_family == ARPHRD_SIT ? "sit" : "???")),
		intn->hwaddr.sa_family,
#else
		(intn->dlt == DLT_NULL ? "Null":
		 (intn->dlt == DLT_EN10MB ? "Ethernet" : "???")),
		intn->dlt,

#endif
		intn->mtu);
}

#ifdef ECMH_BPF
bool int_create_bpf(struct intnode *intn, bool tunnel)
{
	unsigned int	i;
	char		devname[IFNAMSIZ];
	struct ifreq	ifr;

	/*
	 * When we are:
	 * - not in tunnel mode,
	 * or
	 * - it is not a tunnel
	 *
	 * Allocate BPF etc.
	 */
	if (!g_conf->tunnelmode || !tunnel)
	{
		/* Open a BPF for this interface */
		i = 0;
		while (intn->socket < 0)
		{
			snprintf(devname, sizeof(devname), "/dev/bpf%u", i++);
			intn->socket = open(devname, O_RDWR);
			if (intn->socket >= 0 || errno != EBUSY) break;
		}
		if (intn->socket < 0)
		{
			dolog(LOG_ERR, "Couldn't open a new BPF device for %s\n", intn->name);
			return false;
		}

		dolog(LOG_INFO, "Opened %s as a BPF device for %s\n", devname, intn->name);

		/* Bind it to the interface */
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, intn->name, sizeof(ifr.ifr_name));

		if (ioctl(intn->socket, BIOCSETIF, &ifr))
		{
			dolog(LOG_ERR, "Could not bind BPF to %s: %s (%d)\n", intn->name, strerror(errno), errno);
			return false;
		}
		dolog(LOG_INFO, "Bound BPF %s to %s\n", devname, intn->name);

		if (ioctl(intn->socket, BIOCPROMISC))
		{
		    	dolog(LOG_ERR, "Could not set %s to promisc: %s (%d)\n", intn->name, strerror(errno), errno);
			return false;
		}
		dolog(LOG_INFO, "BPF interface for %s is now promiscious\n", intn->name);

		if (fcntl(intn->socket, F_SETFL, O_NONBLOCK) < 0)
		{
			dolog(LOG_ERR, "Could not set %s to non_blocking: %s (%d)\n", intn->name, strerror(errno), errno);
			return false;
		}

		i = 1;
		if (ioctl(intn->socket, BIOCIMMEDIATE, &i))
		{
			dolog(LOG_ERR, "Could not set %s to immediate: %s (%d)\n", intn->name, strerror(errno), errno);
			return false;
		}

		if (ioctl(intn->socket, BIOCGDLT, &intn->dlt))
		{
			dolog(LOG_ERR, "Could not get %s's DLT: %s (%d)\n", intn->name, strerror(errno), errno);
			return false;
		}
		if (intn->dlt != DLT_NULL && intn->dlt != DLT_EN10MB)
		{
		    	dolog(LOG_ERR, "Only NULL and EN10MB (Ethernet) as a DLT are supported, this DLT is %u\n", intn->dlt);
			return false;
		}
		dolog(LOG_INFO, "BPF's DLT is %s\n", intn->dlt == DLT_EN10MB ? "Ethernet" : (intn->dlt == DLT_NULL ? "Null" : "??"));

		if (ioctl(intn->socket, BIOCGBLEN, &intn->bufferlen))
		{
			dolog(LOG_ERR, "Could not get %s's BufferLen: %s (%d)\n", intn->name, strerror(errno), errno);
			return false;
		}
		dolog(LOG_INFO, "BPF's bufferLength is %u\n", intn->bufferlen);

		/* Is this buffer bigger than what we have allocated? -> Upgrade */
		if (intn->bufferlen > g_conf->bufferlen)
		{
			free(g_conf->buffer);
			g_conf->bufferlen = intn->bufferlen;
			g_conf->buffer = malloc(g_conf->bufferlen);
			if (!g_conf->buffer)
			{
				dolog(LOG_ERR, "Couldn't increase bufferlength to %u\n", g_conf->bufferlen);
				dolog(LOG_ERR, "Expecting a memory shortage, exiting\n");
				exit(-1);
			}
		}

		/* Add it to the select set */
		FD_SET(intn->socket, &g_conf->selectset);
		if (intn->socket > g_conf->hifd) g_conf->hifd = intn->socket;
	}
	/* It is a tunnel and we are in tunnel mode*/
	else
	{
		struct if_laddrreq	iflr;
		int			sock;

		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0)
		{
			dolog(LOG_WARNING, "Couldn't allocate a AF_INET socket for getting interface info\n");
			return false;
		}

		memset(&iflr, 0, sizeof(iflr));
		strncpy(iflr.iflr_name, intn->name, sizeof(iflr.iflr_name));
 
		/*
		 * This a tunnel, find out based on it's src ip
		 * to which master it belongs
		 */
		if (ioctl(sock, SIOCGLIFPHYADDR, &iflr))
		{
			dolog(LOG_ERR, "Could not get GIF Addresses of tunnel %s: %s (%d)\n",
				intn->name, strerror(errno), errno);
			close(sock);
			return false;
		}
		close(sock);

		/* Find the master */
		intn->master = int_find_ipv4(true, &((struct sockaddr_in *)&iflr.addr)->sin_addr, false);
		if (!intn->master)
		{
			dolog(LOG_ERR, "Couldn't find the master device for %s\n", intn->name);
			return false;
		}

		/* Store the addresses */
		memcpy(&intn->ipv4_local, &((struct sockaddr_in *)&iflr.addr)->sin_addr, sizeof(intn->ipv4_local));
		memcpy(&intn->ipv4_remote, &((struct sockaddr_in *)&iflr.dstaddr)->sin_addr, sizeof(intn->ipv4_remote));
	}
	return true;
}
#endif /* ECMH_BPF */

#ifndef ECMH_BPF
struct intnode *int_create(unsigned int ifindex)
#else
struct intnode *int_create(unsigned int ifindex, bool tunnel)
#endif
{
	struct intnode	*intn = malloc(sizeof(struct intnode));
	struct ifreq	ifreq;
	int		sock;

	if (!intn) return NULL;

#ifndef ECMH_BPF
	dolog(LOG_DEBUG, "Creating new interface %u\n", ifindex);
#else
	dolog(LOG_DEBUG, "Creating new interface %u (%s)\n", ifindex, tunnel ? "Tunnel" : "Native");
#endif

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		dolog(LOG_ERR, "Couldn't create tempory socket for ioctl's\n");
		return NULL;
	}

	memset(intn, 0, sizeof(*intn));

	intn->ifindex = ifindex;

	/* Default to 0, we discover this after the queries has been sent */
	intn->mld_version = 0;

#ifdef ECMH_BPF
	intn->socket = -1;
#endif

	/* Get the interface name (eth0/sit0/...) */
	/* Will be used for reports etc */
	memset(&ifreq, 0, sizeof(ifreq));
#ifdef linux
	ifreq.ifr_ifindex = ifindex;
	if (ioctl(sock, SIOCGIFNAME, &ifreq) != 0)
#else
        if (if_indextoname(ifindex, intn->name) == NULL)
#endif
	{
		dolog(LOG_ERR, "Couldn't determine interfacename of link %d : %s\n", intn->ifindex, strerror(errno));
		int_destroy(intn);
		close(sock);
		return NULL;
	}
#ifdef linux
	memcpy(&intn->name, &ifreq.ifr_name, sizeof(intn->name));
#else
	memcpy(&ifreq.ifr_name, &intn->name, sizeof(ifreq.ifr_name));
#endif

	/* Get the MTU size of this interface */
	/* We will use that for fragmentation */
	if (ioctl(sock, SIOCGIFMTU, &ifreq) != 0)
	{
		dolog(LOG_ERR, "Couldn't determine MTU size for %s, link %d : %s\n", intn->name, intn->ifindex, strerror(errno));
		perror("Error");
		int_destroy(intn);
		close(sock);
		return NULL;
	}
	intn->mtu = ifreq.ifr_mtu;

#ifdef linux
	/* Get hardware address + type */
	if (ioctl(sock, SIOCGIFHWADDR, &ifreq) != 0)
	{
		dolog(LOG_ERR, "Couldn't determine hardware address for %s, link %d : %s\n", intn->name, intn->ifindex, strerror(errno));
		perror("Error");
		int_destroy(intn);
		close(sock);
		return NULL;
	}
	memcpy(&intn->hwaddr, &ifreq.ifr_hwaddr, sizeof(intn->hwaddr));
#endif

	/* Cleanup the socket */
	close(sock);

#ifdef linux
	/* Ignore Loopback devices */
	if (intn->hwaddr.sa_family == ARPHRD_LOOPBACK)
	{
		int_destroy(intn);
		return NULL;
	}
#endif /* Linux */

#ifdef ECMH_BPF
	if (!int_create_bpf(intn, tunnel))
	{
		int_destroy(intn);
		return NULL;
	}
#endif

	/* All okay */
	return intn;
}

void int_destroy(struct intnode *intn)
{
	if (!intn) return;

D(	dolog(LOG_DEBUG, "Destroying interface %s\n", intn->name);)

#ifdef ECMH_BPF
	if (intn->socket != -1)
	{
		close(intn->socket);
		intn->socket = -1;
	}
#endif

	free(intn);
	return;
}

/*
 * Both int_find() & int_find_ipv4() move the node to the beginning
 * This way it will be in the front the next time we try to find it.
 */
struct intnode *int_find(unsigned int ifindex, bool resort)
{
	struct intnode	*intn;
	struct listnode	*ln;

	LIST_LOOP(g_conf->ints, intn, ln)
	{
		if (ifindex == intn->ifindex)
		{
			if (resort) list_movefront_node(g_conf->ints, ln);
			return intn;
		}
	}
	return NULL;
}

#ifdef ECMH_BPF
struct intnode *int_find_ipv4(bool local, struct in_addr *ipv4, bool resort)
{
	struct intnode	*intn;
	struct listnode	*ln;

	LIST_LOOP(g_conf->ints, intn, ln)
	{
		if (memcmp(local ? &intn->ipv4_local : &intn->ipv4_remote, ipv4, sizeof(ipv4)) == 0)
		{
			if (resort) list_movefront_node(g_conf->ints, ln);
			return intn;
		}
	}
	return NULL;
}
#endif /* ECMH_BPF */

/*
 * Store the version of MLD if it is lower than the old one or the old one was 0 ;)
 * We only respond MLDv1's this way when there is a MLDv1 router on the link.
 * but we respond MLDv2's when there are only MLDv2 router on the link
 * Even if we build without MLDv2 support we detect the version which
 * could be used to determine if a router can't fallback to v1 for instance
 * later on in diagnostics. When built without MLDv2 support we always do MLDv1 though.
 * Everthing higher as MLDv2 is reported as MLDv2 as we don't know about it (yet :)
 */
void int_set_mld_version(struct intnode *intn, unsigned int newversion)
{
	if (newversion == 1)
	{
		/*
		 * Only reset the version number
		 * if it wasn't set and not v1 yet
		 */
		if (	intn->mld_version == 0 &&
			intn->mld_version != 1)
		{
			if (intn->mld_version > 1) dolog(LOG_DEBUG, "MLDv1 Query detected on %s, downgrading from MLDv%u to MLDv1\n", intn->mld_version, intn->name);
			else dolog(LOG_DEBUG, "MLDv1 detected on %s, setting it to MLDv1\n", intn->name);
			intn->mld_version = 1;
			intn->mld_last_v1 = time(NULL);
		}
	}
	else
	{
		/*
		 * Current version = 1 and haven't seen a v1 for a while?
		 * Reset the counter
		 */
		if (	intn->mld_last_v1 != 0 &&
			(time(NULL) > (intn->mld_last_v1 + ECMH_SUBSCRIPTION_TIMEOUT)))
		{
			dolog(LOG_DEBUG, "MLDv1 has not been seen for %u seconds on %s, allowing upgrades\n", 
				(intn->mld_last_v1 + ECMH_SUBSCRIPTION_TIMEOUT), intn->name);
			/* Reset the version */
			intn->mld_version = 0;
			intn->mld_last_v1 = 0;
		}

		/*
		 * Only reset the version number
		 * if it wasn't set and not v1 yet and not v2 yet
		 */
		if (	intn->mld_version == 0 &&
			intn->mld_version != 1 &&
			intn->mld_version != 2)
		{
			dolog(LOG_DEBUG, "MLDv%u detected on %s, setting it to MLDv%u\n",
				newversion, intn->name, newversion);
			intn->mld_version = newversion;
		}
	}
}

#ifdef ECMH_BPF

/* Add or update a local interface */
void local_update(struct intnode *intn)
{
	struct localnode	*localn;

	/* Try to find it first */
	localn = local_find(&intn->ipv4_local);
	/* Already there?  */
	if (localn) return;

	/* Allocate a piece of memory */
	localn = (struct localnode *)malloc(sizeof(*localn));
	if (!localn)
	{
		dolog(LOG_ERR, "Couldn't allocate memory for localnode\n");
		return;
	}

	/* Fill it in */
	localn->intn = intn;

	dolog(LOG_DEBUG, "Adding %s to local tunnel-intercepting-interfaces\n", intn->name);

	/* Add it to the list */
	listnode_add(g_conf->locals, localn);
}

struct localnode *local_find(struct in_addr *ipv4)
{
	struct localnode	*localn;
	struct listnode		*ln;

	LIST_LOOP(g_conf->locals, localn, ln)
	{
		if (memcmp(&localn->intn->ipv4_local, ipv4, sizeof(ipv4)) == 0)
		{
			return localn;
		}
	}
	return NULL;
}

void local_remove(struct intnode *intn)
{
	struct localnode *localn;
	localn = local_find(&intn->ipv4_local);
	if (!localn) return;

	dolog(LOG_DEBUG, "Remove %s from local tunnel-intercepting-interfaces\n", intn->name);

	/* Remove it from the list */
	listnode_delete(g_conf->locals, localn);
}

void local_destroy(struct localnode *localn)
{
	/* Free the memory */
	free(localn);
}

#endif /* ECMH_BPF */

