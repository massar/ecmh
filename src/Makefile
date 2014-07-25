# /**************************************
#  ecmh - Easy Cast du Multi Hub
#  by Jeroen Massar <jeroen@massar.ch>
# **************************************/
#
# Source Makefile for ecmh - Jeroen Massar <jeroen@massar.ch>
#
# ECMH_VERSION and ECMH_OPTIONS need to be defined, gets done by toplevel Makefile
#
# One should make this using the main Makefile (thus one dir up)
#
# FreeBSD people should uncomment these if they don't have GNU make and
# then run 'make' from this directory
# On FreeBSD4 install the "libgnugetopt" port to get getopt working
#ECMH_OPTIONS=-DECMH_BPF -DECMH_SUPPORT_MLD2 -DECMH_GETIFADDR -I/usr/local/include/
#ECMH_VERSION=2013.09.28
#ECMH_LDFREEBSD=-L/usr/local/lib/ -lgnugetopt

# Below here nothing should have to be changed
BINS	= ecmh
SRCS	= ecmh.c linklist.c common.c interfaces.c groups.c grpint.c subscr.c
INCS	= ecmh.h linklist.h common.h interfaces.h groups.h grpint.h subscr.h mld.h
DEPS	= ../Makefile Makefile
OBJS	= ecmh.o linklist.o common.o interfaces.o groups.o grpint.o subscr.o

# Standard Warnings
WARNS	+=	-Wall -Wextra

ifeq ($(CC_TYPE),gcc)
WARNS	+=	-Werror
endif

# Extended warnings
WARNS	+=	-Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings
WARNS	+=	-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes
WARNS	+=	-Wmissing-declarations -Wredundant-decls -Wnested-externs
WARNS	+=	-Winline -Wbad-function-cast -fstrict-aliasing
WARNS	+=	-fno-common -Wno-packed -Wswitch-default
WARNS	+=	-Wformat=2 -Wformat-security
WARNS	+=	-Wmissing-format-attribute
WARNS	+=	-D_REENTRANT -D_THREAD_SAFE -pipe -Wunused -Winit-self
WARNS	+=	-Wextra -Wno-long-long -Wmissing-include-dirs
WARNS	+=	-Wno-variadic-macros
WARNS	+=	-std=c99
WARNS	+=	-pedantic

# clang does not know the -ansi option
ifneq ($(CC_TYPE),clang)
WARNS	+=	-ansi
endif

ifeq ($(shell echo $(ECMH_OPTIONS) | grep -c "DEBUG"),1)
EXTRA	= -g -O0
else
EXTRA	= -O3
endif

CFLAGS	= $(WARNS) $(EXTRA) -D_GNU_SOURCE -D'ECMH_VERSION="$(ECMH_VERSION)"' -D'ECMH_GITHASH="$(ECMH_GITHASH)"' $(ECMH_OPTIONS)
LDFLAGS	= $(ECMH_LDFREEBSD)
LINK	= @echo "* Linking $@"; $(CC) $(CFLAGS) $(LDFLAGS)

-include $(OBJS:.o=.d)

all:	$(BINS)

depend: clean
	@echo "* Making dependencies"
	@$(MAKE) -s $(OBJS)
	@echo "* Making dependencies - done"

%.o: %.c $(DEPS)
	@echo "* Compiling $@";
	@$(CC) -c $(CFLAGS) $*.c -o $*.o
	@$(CC) -MM $(CFLAGS) $*.c > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

ecmh:	$(DEPS) $(OBJS)
	$(LINK) -o $@ $(OBJS) $(LDLIBS)
ifeq ($(shell echo $(ECMH_OPTIONS) | grep -c "DEBUG"),0)
	@strip $@
endif

clean:
	$(RM) $(OBJS) $(BINS)

# Mark targets as phony
.PHONY : all clean

