# /**************************************
#  ecmh - Easy Cast du Multi Hub
#  by Jeroen Massar <jeroen@unfix.org>
# ***************************************
# $Author: fuzzel $
# $Id: Makefile,v 1.7 2004/02/19 14:21:07 fuzzel Exp $
# $Date: 2004/02/19 14:21:07 $
# **************************************/
#
# Toplevel Makefile allowing easy distribution.
# Use this makefile for doing almost anything
# 'make help' shows the possibilities
#

# The name of the application
ECMH=ecmh

# The version of this release
ECMH_VERSION=2004.02.18
export ECMH_VERSION

# ECMH Compile Time Options
# Append one of the following option on wish to
# include certain features, -O3 is the default
#
# Optimize             : -O3
# Enable Debugging     : -DDEBUG
# Enable IPv4 Support  : -DECMH_SUPPORT_IPV4
# Enable MLDv2 Support : -DECMH_SUPPORT_MLD2
ECMH_OPTIONS=-O9
export ECMH_OPTIONS

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
toolsdir=tools/

all:	ecmh tools

ecmh:	$(srcdir)
	$(MAKE) -C src all

tools:	$(toolsdir)
	$(MAKE) -C tools all

help:
	@echo "ecmh - Easy Cast du Multi Hub"
	@echo "Website: http://unfix.org/projects/ecmh/"
	@echo "Author : Jeroen Massar <jeroen@unfix.org>"
	@echo
	@echo "Makefile targets:"
	@echo "all      : Build everything"
	@echo "ecmh	: Build only ecmh"
	@echo "tools	: Build only the tools"
	@echo "help     : This little text"
	@echo "install  : Build & Install"
	@echo "clean    : Clean the dirs to be pristine in bondage"
	@echo
	@echo "Distribution targets:"
	@echo "dist     : Make all distribution targets"
	@echo "tar      : Make source tarball (tar.gz)"
	@echo "bz2      : Make source tarball (tar.bz2)"
	@echo "deb      : Make Debian binary package (.deb)"
	@echo "debsrc   : Make Debian source packages"
	@echo "rpm      : Make RPM package (.rpm)"
	@echo "rpmsrc   : Make RPM source packages"

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
.PHONY : all ecmh tools install help clean dist tar bz2 deb debsrc debclean rpm rpmsrc
