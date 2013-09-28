# /**************************************
#  ecmh - Easy Cast du Multi Hub
#  by Jeroen Massar <jeroen@massar.ch>
# **************************************/
#
# Tools Makefile for ecmh - Jeroen Massar <jeroen@massar.ch>
#
# mtrace6 is taken from the KAME distribution, see http://www.kame.net
# Copyright etc is theirs, it is only included for convienience.

BINS	= mtrace6
SRCS	= mtrace6.c
INCS	= ../../src/trace.h
DEPS	= ../../Makefile ../Makefile Makefile
OBJS	= mtrace6.o
CFLAGS	= -W -Wall -Wno-unused -D_GNU_SOURCE -D'ECMH_VERSION="$(ECMH_VERSION)"' $(ECMH_OPTIONS)
LDFLAGS	= 
RM	= @rm
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

mtrace6: $(DEPS) $(OBJS) $(INCS)
	$(LINK) -o $@ $(OBJS) $(LDLIBS)
ifeq ($(shell echo $(ECMH_OPTIONS) | grep -c "DEBUG"),0)
	@strip $@
endif

clean:
	$(RM) -f $(OBJS) $(BINS)

# Mark targets as phony
.PHONY : all clean mtrace6

