# /**************************************
#  ecmh - Easy Cast du Multi Hub
#  by Jeroen Massar <jeroen@unfix.org>
# ***************************************
# $Author: fuzzel $
# $Id: Makefile,v 1.2 2004/01/11 21:41:05 fuzzel Exp $
# $Date: 2004/01/11 21:41:05 $
# **************************************/
#
# Source Makefile for ecmh - Jeroen Massar <jeroen@unfix.org>
#
# ECMH_VERSION needs to be defined, gets done by toplevel Makefile

BINS	= ecmh
SRCS	= ecmh.c linklist.c common.c interfaces.c groups.c grpint.c subscr.c
INCS	= ecmh.h linklist.h common.h interfaces.h groups.h grpint.h subscr.h mld.h
OBJS	= ecmh.o linklist.o common.o interfaces.o groups.o grpint.o subscr.o
CFLAGS	= -W -Wall -Wno-unused -D_GNU_SOURCE -D'ECMH_VERSION="$(ECMH_VERSION)"' -O9
LDFLAGS	= 
CC      = gcc
RM      = rm

all:	$(BINS)

ecmh:	$(OBJS) ${INCS}
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS);
	strip $@

clean:
	$(RM) -f $(OBJS) $(BINS)

