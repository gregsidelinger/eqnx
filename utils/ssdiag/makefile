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

include ../../version.include
  
HERE=$(shell pwd)
INC = $(HERE)/../../include
.EXPORT_ALL_VARIABLES: 

INCLUDES = -I./include -I$(INC) 
MCFLAGS = -Wall -O2 -pipe -DLINUX 
MCFLAGS2 = -DVERSNUM=\"$(REL)\"
LIBS = -lncurses

build:  ssdiag sscode

install: ssdiag sscode
	@strip ssdiag sscode
	@mkdir -p $(OBJROOT)/usr/bin
	@install -o root -m 755 ssdiag $(OBJROOT)/usr/bin 
	@install -o root -m 755 sscode $(OBJROOT)/usr/bin 
	@install -o root -m 444 README.ssdiag $(OBJROOT)/.
	@mkdir -p $(OBJROOT)/usr/lib/sst
	@install -o root -m 444 ss.hlp $(OBJROOT)/usr/lib/sst


ssdiag: u4.o ports.o screen.o 
	$(CC) $(MCFLAGS) $(LIBS) u4.o screen.o ports.o -ossdiag

sscode: ccode.o screen.o 
	$(CC) $(MCFLAGS) $(LIBS) ccode.o screen.o -osscode

u4.o: u4.c
	$(CC) $(MCFLAGS) $(MCFLAGS2) $(INCLUDES) -c u4.c

screen.o: screen.c
	$(CC) $(MCFLAGS) $(INCLUDES) -c screen.c

ports.o: ports.c
	$(CC) $(MCFLAGS) $(INCLUDES) -c ports.c

ccode.o: ccode.c
	$(CC) $(MCFLAGS) $(INCLUDES) -c ccode.c

clean:
	@rm -f *.o
	@rm -f ssdiag sscode 

clobber: clean
	@rm -f $(OBJROOT)/usr/bin/ssdiag
	@rm -f $(OBJROOT)/usr/bin/sscode
	@rm -f $(OBJROOT)/README.ssdiag
	@rm -f $(OBJROOT)/usr/lib/sst/ss.hlp


