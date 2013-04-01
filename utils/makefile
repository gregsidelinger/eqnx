 
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

include ../version.include

HERE=$(shell pwd)
INC = $(HERE)/../include
SRC = $(HERE)/../drv
.EXPORT_ALL_VARIABLES:

HFILES = ../include/icp.h ../include/eqnx_ioctl.h ../include/ist.h 

INCLUDES = -I$(INC) 
MCFLAGS = -Wall -Wstrict-prototypes -O2 -pipe -DLINUX

########################################################################
# Environment tests
 
ifeq "$(findstring 86, $(shell uname -m))" "86"
ifeq "$(findstring x86_64, $(shell uname -m))" "x86_64"
   MCFLAGS += -DX86_64
else
   MCFLAGS += -DIX86 -DISA_ENAB -DMCA_ENAB
endif
endif
ifeq "$(findstring ia64, $(shell uname -m))" "ia64"
   MCFLAGS += -DIA64 -Wno-format
endif

# Completed Environment tests
########################################################################

build: sstty ssmkn

install: sstty ssmkn
	@strip sstty ssmkn
	@mkdir -p $(OBJROOT)/usr/bin
	@install -o root -m 755 ssmkn $(OBJROOT)/usr/bin
	@install -o root -m 755 ssrm $(OBJROOT)/usr/bin
	@install -o root -m 755 sstty $(OBJROOT)/usr/bin
	@mkdir -p $(OBJROOT)/etc/rc.d/init.d
	@mkdir -p $(OBJROOT)/usr/sbin
	@install -o root -m 755 rc.eqnx $(OBJROOT)/usr/sbin

sstty: sstty.c $(HFILES)
	$(CC) $(MCFLAGS) $(INCLUDES) sstty.c -o sstty

ssmkn: ssmkn.c $(HFILES)
	$(CC) $(MCFLAGS) $(INCLUDES) ssmkn.c -o ssmkn

clean:
	@rm -f ssmkn.o sstty.o
	@rm -f sstty ssmkn

clobber: clean
	@rm -f $(OBJROOT)/usr/bin/ssmkn
	@rm -f $(OBJROOT)/usr/bin/ssrm
	@rm -f $(OBJROOT)/usr/bin/sstty
	@rm -f $(OBJROOT)/usr/sbin/rc.eqnx
