# 
# Equinox SST driver for Linux.
# Top level makefile
#
# Copyright (C) 1999-2007 Equinox Systems Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

#
# Builds and installs the source for the Equinox SST "eqnx" software.
#
# The following file contains the version number which may need to be
# changed for a new version:
#	version.include
#
# Also, documentation to be updated is located at:
#	inst/INSTALL.TXT
#	inst/README.TXT
#	inst/RELEASE.TXT
#
# The following targets are available:
#    make clean 
#		this removes .o files from the source directories
# 
#    make clobber
#		this removes .o files from the source directories
#		and removes all object
# 
#    make build
#		this builds all object 
#
#    make install
#		creates the installation media with built object.
#		(note: internal only)
#
# With no targets a "make build install" will be done.

all:	build install

build:
	cd drv; make build
	cd utils; make build
	cd utils/ssdiag; make build
	cd inst; make build

install:
	cd drv; make install
	cd utils; make install
	cd utils/ssdiag; make install
	cd inst; make install

clean:
	cd drv; make clean
	cd utils; make clean
	cd utils/ssdiag; make clean
	cd inst; make clean

clobber:
	cd drv;  make clobber
	cd utils; make clobber
	cd utils/ssdiag; make clobber
	cd inst; make clobber
