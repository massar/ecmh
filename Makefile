# Easy Cast du Multi Hub
# Toplevel Makefile allowing easy distribution ;)
#
# Jeroen Massar <jeroen@unfix.org>

# The name of the application
ECMH=ecmh
# The version of this release
ECMH_VERSION=2004.02.17
export ECMH_VERSION

# Change this if you want to install into another dirtree
# Required for eg the Debian Package builder
DESTDIR=

# Get the source dir, needed for eg debsrc
SOURCEDIR := $(shell pwd)

# Misc bins
RM=rm -f

# Paths
sbindir=/usr/sbin/
srcdir=src/

all:	${srcdir}
	$(MAKE) -C src all

install: all
	mkdir -p $(DESTDIR)${sbindir}
	cp src/$(ECMH) $(DESTDIR)${sbindir}

# Clean all the output files etc
distclean: clean

clean: debclean
	$(MAKE) -C src clean

# Generate Distribution files
dist:	tar bz2 deb debsrc rpm rpmsrc

# tar.gz
tar:	clean
	-${RM} ../ecmh_${ECMH_VERSION}.tar.gz
	tar -zclof ../ecmh_${ECMH_VERSION}.tar.gz *

# tar.bz2
bz2:	clean
	-${RM} ../ecmh_${ECMH_VERSION}.tar.bz2
	tar -jclof ../ecmh_${ECMH_VERSION}.tar.bz2 *

# .deb
deb:	clean
	# Copy the changelog
	cp doc/changelog debian/changelog
	debian/rules binary
	${MAKE} clean

# Source .deb
debsrc: clean
	# Copy the changelog
	cp doc/changelog debian/changelog
	cd ..; dpkg-source -b ${SOURCEDIR}; cd ${SOURCEDIR}
	${MAKE} clean

# Cleanup after debian
debclean:
	-rm -rf build-stamp configure-stamp debian/changelog debian/ecmh/ debian/ecmh.postinst.debhelper debian/ecmh.postrm.debhelper debian/ecmh.prerm.debhelper debian/ecmh.substvars

# RPM
rpm:	clean
	# TODO ;)
	${MAKE} clean

rpmsrc:	clean
	# TODO ;)

# Mark targets as phony
.PHONY : all install clean dist tar bz2 deb debsrc debclean rpm rpmsrc

