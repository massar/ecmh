# /**************************************
#  ecmh - Easy Cast du Multi Hub
#  by Jeroen Massar <jeroen@unfix.org>
# ***************************************
# $Author: fuzzel $
# $Id: Makefile,v 1.4 2004/02/17 19:05:13 fuzzel Exp $
# $Date: 2004/02/17 19:05:13 $
# **************************************/
#
# Source Makefile for ecmh - Jeroen Massar <jeroen@unfix.org>
#
# ECMH_VERSION and ECMH_OPTIONS need to be defined, gets done by toplevel Makefile
#
# One should make this using the main Makefile (thus one dir up)

BINS	= ecmh
SRCS	= ecmh.c linklist.c common.c interfaces.c groups.c grpint.c subscr.c
INCS	= ecmh.h linklist.h common.h interfaces.h groups.h grpint.h subscr.h mld.h
DEPS	= ../Makefile Makefile
OBJS	= ecmh.o linklist.o common.o interfaces.o groups.o grpint.o subscr.o
CFLAGS	= -W -Wall -Wno-unused -D_GNU_SOURCE -D'ECMH_VERSION="$(ECMH_VERSION)"' $(ECMH_OPTIONS)
LDFLAGS	= 
CC      = gcc
RM      = rm

all:	$(BINS)

ecmh:	$(OBJS) ${INCS} ${DEPS}
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS);
	strip $@

clean:
	$(RM) -f $(OBJS) $(BINS)

# Mark targets as phony
.PHONY : all clean
