/*****************************************************
 ecmh - Easy Cast du Multi Hub - Common Functions
******************************************************
 $Author: fuzzel $
 $Id: common.c,v 1.3 2005/02/09 17:58:06 fuzzel Exp $
 $Date: 2005/02/09 17:58:06 $
*****************************************************/

#include "ecmh.h"

void dolog(int level, const char *fmt, ...)
{
	va_list ap;
	if (g_conf && !g_conf->verbose && level == LOG_DEBUG) return;
	va_start(ap, fmt);
	if (g_conf && g_conf->daemonize)
	{
		vsyslog(LOG_LOCAL7|level, fmt, ap);
	}
	else
	{
		if (g_conf && g_conf->verbose)
		{
			printf("[%6s] ",
				level == LOG_DEBUG ?	"debug" :
				(level == LOG_ERR ?	"error" :
				(level == LOG_WARNING ?	"warn" :
				(level == LOG_INFO ?	"info" : ""))));
		}
		vprintf(fmt, ap);
	}
	va_end(ap);
}

int huprunning()
{
	int pid;

	FILE *f = fopen(PIDFILE, "r");
	if (!f) return 0;
	fscanf(f, "%d", &pid);
	fclose(f);
	/* If we can HUP it, it still runs */
	return (kill(pid, SIGHUP) == 0 ? 1 : 0);
}

void savepid()
{
	FILE *f = fopen(PIDFILE, "w");
	if (!f) return;
	fprintf(f, "%d", getpid());
	fclose(f);

	dolog(LOG_INFO, "Running as PID %d\n", getpid());
}

void cleanpid(int i)
{
	dolog(LOG_INFO, "Trying to exit, got signal %d...\n", i);
	unlink(PIDFILE);

	if (g_conf) g_conf->quit = true;
}

uint64_t gettimes(void)
{
#ifdef __MACH__
	/* OS X does not have clock_gettime, use clock_get_time */
	clock_serv_t	cclock;
	mach_timespec_t	mts;

	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);

	return (mts.tv_sec);
#else
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);

	return (ts.tv_sec);
#endif
}

