/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: fuzzel $
 $Id: ecmh.h,v 1.6 2004/02/17 00:22:29 fuzzel Exp $
 $Date: 2004/02/17 00:22:29 $
**************************************/

#define _XOPEN_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <pwd.h>
#include <getopt.h>

#include <net/if.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <sys/ioctl.h>
#include "mld.h"

#define PIDFILE "/var/run/ecmh.pid"
#define ECMH_DUMPFILE "/var/run/ecmh.dump"

// Defines
#undef DEBUG
#undef ECMH_SUPPORT_IPV4
#undef ECMH_SUPPORT_MLD2

#ifdef DEBUG
#define D(x) x
#else
#define D(x) {}
#endif

// The timeout for queries
// as per MLDv2 "9.2.  Query Interval"
#define ECMH_SUBSCRIPTION_TIMEOUT 125

#include "linklist.h"
#include "common.h"

// Booleans
#define false	0
#define true	(!false)
#define bool	int

#include "interfaces.h"
#include "groups.h"
#include "grpint.h"
#include "subscr.h"

// Our configuration structure
struct conf
{
	int			maxgroups;
	struct list		*ints;				// The interfaces we are watching *
	struct list		*groups;			// The groups we are joined to

	bool			daemonize;			// To Daemonize or to not to Daemonize

	int			rawsocket;			// RAW socket for sending and receiving

	FILE			*stat_file;			// The file handle of ourdump file
	time_t			stat_starttime;			// When did we start
	uint64_t		stat_packets_received;		// Number of packets received
	uint64_t		stat_packets_sent;		// Number of packets forwarded
	uint64_t		stat_bytes_received;		// Number of bytes received
	uint64_t		stat_bytes_sent;		// Number of bytes forwarded
	uint64_t		stat_icmp_received;		// Number of ICMP's received
	uint64_t		stat_icmp_sent;			// Number of ICMP's sent
};

// Global Stuff
extern struct conf *g_conf;

void mld_send_report(struct intnode *intn, const struct in6_addr *mca);
