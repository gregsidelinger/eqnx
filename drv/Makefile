# 
# Equinox SST driver for Linux.
#
# Copyright (C) 1989-2007 Equinox Systems Inc.
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

ifeq ($(KERNELRELEASE),)
export BASE=$(shell pwd)
endif

include $(BASE)/../version.include

INC = $(BASE)/../include
.EXPORT ALL VARIABLES:

HFILES = ../include/icp.h  ../include/eqnx.h \
	../include/eqnx_ioctl.h  ../include/ist.h  

INCLUDES = -I$(INC) -I./ 
MODOBJS = sst.o 
MCFLAGS = -D__KERNEL__ -Wall -Wstrict-prototypes -O2 -pipe \
	-DMODULE -DLINUX  -D_LOOSE_KERNEL_NAMES -Wno-incompatible-pointer-types -Wno-error=implicit-function-declaration
NO_OPT_MCFLAGS = -D__KERNEL__ -Wall -Wstrict-prototypes -pipe
MCFLAGS2 = 
CPOBJS = eqnx.o
DRV = eqnx

########################################################################
# Environment tests

# Figure out if kernel is 2.2/2.4 or 2.6+
KVER := $(shell uname -r | cut -b1-3)
K26 := 1
K41 := 1
ifeq (2.2,$(KVER))
   K26 := 0
endif
ifeq (2.4,$(KVER))
   K26 := 0
endif
ifeq (4.1,$(KVER))
	K41:=0
endif

# Kernel Search Path
# These are all the places where kernel source might exist
KSP :=  /usr/src/linux-$(shell uname -r) \
	/usr/src/kernels/$(shell uname -r) \
	/lib/modules/$(shell uname -r)/build \
	/usr/src/kernels/linux-$(shell uname -r) \
        /usr/src/linux-$(shell uname -r)-$(shell uname -m) \
        /usr/src/kernels/$(shell uname -r)-$(shell uname -m) \
        /usr/src/kernels/$(shell uname -r | sed 's/smp/-smp/')-$(shell uname -m) \
        /usr/src/linux-$(shell uname -r | sed 's/-.*//') \
        /usr/src/kernels/$(shell uname -r | sed 's/-.*//') \
        /usr/src/linux-$(shell uname -r | sed 's/\([0-9]*\.[0-9]*\)\..*/\1/') \
        /usr/src/kernels/$(shell uname -r | sed 's/\([0-9]*\.[0-9]*\)\..*/\1/') \
        /usr/src/linux-$(shell uname -r | sed 's/custom//') \
        /usr/src/kernels/$(shell uname -r | sed 's/custom//') \
        /usr/src/linux \
	/usr/src/kernels/linux
				  
# Only look at paths that exist and have an include/linux sub-directory
test_dir = $(shell [ -e $(dir)/include/linux ] && echo $(dir))
KSP := $(foreach dir, $(KSP), $(test_dir))
			   
# Kernel Source Location
# we will use this first valid entry in the search path
KSRC := $(firstword $(KSP))
KOBJ := $(KSRC)

#
# Extract KSRC and KOBJ if available via build process
ifeq (${KSRC}, /lib/modules/$(shell uname -r)/build)
	KOBJ := /lib/modules/$(shell uname -r)/build
	dir := /lib/modules/$(shell uname -r)/source
	KSRC = $(shell [ -e $(dir)/include/linux ] && echo $(dir))
	ifeq ($(origin KSRC), undefined)
		KSRC := $(KOBJ)
	endif
endif

#
# KERNELSRC environment variable overrides KSRC and KOBJ
ifneq ($(origin KERNELSRC), undefined)
	KSRC := $(KERNELSRC)
	KOBJ := $(KSRC)
endif
KINC = $(KSRC)/include

#
# KERNELOBJ environement variable, overrides KOBJ
ifneq ($(origin KERNELOBJ), undefined)
	KOBJ := $(KERNELOBJ)
else
	ifneq ($(KSRC), $(KOBJ))
		KERNELOBJ := $(KOBJ)
	endif
endif
KINC2 = $(KOBJ)/include

ifeq (,$(KSRC))
   $(error The Linux kernel source could not be found)
endif
   
# Locate version.h, config.h, utsrelease.h and autoconf.h files
ifneq (,$(wildcard $(KINC)/linux/version.h))
	VERSION_FILE := $(KINC)/linux/version.h
else
ifneq (,$(wildcard $(KINC)/generated/uapi/linux/version.h))
	VERSION_FILE := $(KINC)/generated/uapi/linux/version.h
else
	VERSION_FILE := $(KINC2)/linux/version.h
endif
endif



ifneq (,$(wildcard $(KINC)/linux/config.h))
	CONFIG_FILE := $(KINC)/linux/config.h
else
	CONFIG_FILE := $(KINC2)/linux/config.h
endif
ifneq (,$(wildcard $(KINC)/linux/utsrelease.h))
	UTSRELEASE_FILE := $(KINC)/linux/utsrelease.h
else
ifneq (,$(wildcard $(KINC)/generated/utsrelease.h))
	UTSRELEASE_FILE := $(KINC)/generated/utsrelease.h
else
	UTSRELEASE_FILE := $(KINC2)/linux/utsrelease.h
endif
endif

ifeq (,$(wildcard $(UTSRELEASE_FILE)))
	UTSRELEASE_FILE := $(VERSION_FILE)
endif
ifneq (,$(wildcard $(KINC)/linux/autoconf.h))
	AUTOCONF_FILE := $(KINC)/linux/autoconf.h
else
ifneq (,$(wildcard $(KINC)/generated/autoconf.h))
	AUTOCONF_FILE := $(KINC)/generated/autoconf.h
else
	AUTOCONF_FILE := $(KINC2)/linux/autoconf.h
endif
endif

ifeq (,$(wildcard $(UTSRELEASE_FILE)))
   $(error Linux kernel source not configured - missing $(UTSRELEASE_FILE))
endif

ifeq (,$(wildcard $(UTSRELEASE_FILE)))
	UTSRELEASE_FILE := $(VERSION_FILE)
endif

ifeq (,$(wildcard ($CONFIG_FILE)))
	CONFIG_FILE := $(AUTOCONF_FILE)
endif

ifeq (,$(wildcard $(CONFIG_FILE)))
   $(error Linux kernel source not configured - could not find $(CONFIG_FILE) or $(AUTOCONF_FILE))
endif
	 
# get the kernel version - we use this to find the correct install path
KVER := $(shell $(CC) $(CFLAGS) -D__KERNEL__ -E -dM -I$(KINC) -I$(KINC2) $(UTSRELEASE_FILE) | grep UTS_RELEASE | \
	awk '{ print $$3 }' | sed 's/\"//g')

ifneq ($(KVER),$(shell uname -r))
   $(warning ***)
   $(warning *** Warning: kernel source version ($(KVER)))
   $(warning *** does not match running kernel  ($(shell uname -r)).)
   $(warning *** Its likely that the Equinox SST driver cannot)
   $(warning *** be built or cannot be loaded.)
   $(warning ***)
endif

# pick the appropriate install path
ifneq (,$(wildcard /lib/modules/$(KVER)/kernel))
   MODDIR := $(OBJROOT)/lib/modules/$(KVER)/kernel/drivers/char
else
   MODDIR := $(OBJROOT)/lib/modules/$(KVER)/char
endif  

# check the SMP flag - first from config.h
SMP := $(shell $(CC) $(CFLAGS) -D__KERNEL__ -E -dM -I$(KINC) -I$(KINC2) $(CONFIG_FILE) | \
	grep 'CONFIG_SMP ' | awk '{ print $$3 }')
ifneq ($(SMP),1)
   SMP := 0
endif
			        
# check that running kernel matches SMP configuration from config.h
ifneq ($(SMP),$(shell uname -a | grep SMP > /dev/null 2>&1 && echo 1 || echo 0))
   $(warning ***)
ifeq ($(SMP),1)
      $(warning *** Warning: kernel source configuration (SMP))
      $(warning *** does not match running kernel (UP).)
else
      $(warning *** Warning: kernel source configuration (UP))
      $(warning *** does not match running kernel (SMP).)
endif
      $(warning *** This must be corrected before the Equinox SST)
      $(warning *** driver can be correctly used.)
      $(warning ***)
      $(error CONFIG_SMP mismatch in kernel source and running kernel)
endif          

ifeq ($(SMP), 1)
   MCFLAGS += -D__SMP__
endif

# check MODVERSIONS configuration from running kernel
ifeq (0,$(K26))
   MODV := $(shell cat /proc/ksyms | grep n_tty_ioctl | cut -d" " -f2)
   ifneq ($(MODV), n_tty_ioctl)
     MODV := 1
   else
     MODV := 0
   endif
endif

# check the MODVERSIONS flag from config.h
MODVER := $(shell $(CC) $(CFLAGS) -D__KERNEL__ -E -dM -I$(KINC) -I$(KINC2) $(CONFIG_FILE) | \
	grep CONFIG_MODVERSIONS | awk '{ print $$3 }')
ifneq ($(MODVER),1)
   MODVER := 0
endif
			        
# check that MODVERSIONS configuration matches
ifeq (0,$(K26))
ifneq ($(MODV), $(MODVER))
   $(warning ***)
ifeq ($(MODV),1)
   $(warning *** Warning: module versions configuration option: MODVERSIONS)
   $(warning *** [disabled] does not match running kernel [MODVERSIONS enabled].)
else
   $(warning *** Warning: module versions configuration option: MODVERSIONS)
   $(warning *** [enabled] does not match running kernel [MODVERSIONS disabled].)
endif
   $(warning *** Its likely that the Equinox SST driver cannot)
   $(warning *** be loaded.  Please check CONFIG_MODVERSIONS in kernel source.)
   $(warning ***)
endif        
endif

ifeq "$(findstring 86, $(shell uname -m))" "86"
ifeq "$(findstring x86_64, $(shell uname -m))" "x86_64"
   MCFLAGS += -DX86_64 -mcmodel=kernel -mno-red-zone
   NO_OPT_MCFLAGS += -DX86_64
else
   MCFLAGS += -DIX86 -DISA_ENAB -DMCA_ENAB 
   NO_OPT_MCFLAGS += -DIX86 -DISA_ENAB -DMCA_ENAB
endif
endif
ifeq "$(findstring ia64, $(shell uname -m))" "ia64"
   MCFLAGS += -DIA64 -mcmodel=kernel
   NO_OPT_MCFLAGS += -DIA64
endif
MCFLAGS += -DVERSNUM=\"$(REL)\"

# Changes for 2.6 kernel
ifeq (1,$(K26))
   CPOBJS = eqnx.ko
endif

ifeq (1,$(K41))
   CPOBJS = eqnx.ko
endif

# Completed Environment tests
########################################################################
ALL: build

install: build
	@mkdir -p $(MODDIR) 
	@install -o root -m 664 $(CPOBJS) $(MODDIR)

ifeq (0,$(K26))
include $(BASE)/makefile_2.4
else
include $(BASE)/makefile_2.6
endif

