# 
# Equinox SST driver for Linux.
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
include ../version.include

HERE=$(shell pwd)
BASE = $(HERE)/..
TOPDIR = $(HERE)

.EXPORT_ALL_VARIABLES:   

build:	

install:
	@mkdir -p $(OBJROOT)/usr/sbin
	@install -o root -m 755 eqnx-cfg $(OBJROOT)/usr/sbin
	@install -o root -m 755 rpmvar $(OBJROOT)/usr/sbin
	@install -o root -m 755 eqnx-installrc $(OBJROOT)/usr/sbin

clean:

clobber:
	@rm -f $(OBJROOT)/usr/sbin/eqnx-cfg
	@rm -f $(OBJROOT)/usr/sbin/rpmvar
	@rm -f $(OBJROOT)/usr/sbin/eqnx-installrc

