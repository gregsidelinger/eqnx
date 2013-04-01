Summary:	Device driver/utilities for Equinox SST SuperSerial family
Name:		eqnx
Version:	4.12d
Release:	1
License:	GPL
Group:		Applications/Communications
URL:		http://www.equinox.com
Vendor:		Avocent, Connectivity and Control Division
Packager:	Mike Straub <michael.straub@avocent.com>
ExclusiveOS:	Linux
BuildRoot:	/var/tmp/%{name}-build
Source:		eqnx-4.12d-1.tar.gz

%description
Provides device driver and diagnostic utilities for Equinox SuperSerial
multiport and expandable host controller boards.  This driver supports
ISA, EISA and PCI versions of the 2, 4, 8 and 16 port boards as well 
as the expandable 64 and 128 port boards and the 4RJ and 8RJ boards.
ISA and EISA are not available on Itanium systems.

Brief description of installation (for more, see installation notes):
1) install the source RPM:
	rpm -i eqnx-4.12d-1.src.rpm		(wherever RPM is located)
2) build the binary RPM:
	rpmbuild (or rpm) -bb <rpm directory>/SPECS/eqnx.spec
	(note: the RPM directory will vary based on linux distribution)
3) install binary RPM:
	rpm -i <rpm directory>/RPMS/<ARCH>/eqnx-4.12d-1.<ARCH>.rpm
4) load driver and configure device files:
	/usr/sbin/rc.eqnx start
5) make driver restarted at boot time:
      	chkconfig --add eqnx			(varies based on distribution)
	-or-
	a) copy /usr/sbin/rc.eqnx to /etc/rc.d/eqnx and
	   add links at /etc/rc.d/rcN.d/S90eqnx
	b) copy /usr/sbin/rc.eqnx to /etc/init.d/eqnx and
	   add links at /etc/rcN.d/S90eqnx
	c) invoke rc.eqnx from rc.local file


%prep
%setup -n %{name}-%{version}-%{release}
rm -rf $RPM_BUILD_ROOT/src
mkdir -m 755 -p $RPM_BUILD_ROOT/src
cp -R $RPM_BUILD_DIR/%{name}-%{version}-%{release}/* $RPM_BUILD_ROOT/src

%build
rm -rf $RPM_BUILD_ROOT/src
mkdir -m 755 -p $RPM_BUILD_ROOT/src
cp -R $RPM_BUILD_DIR/%{name}-%{version}-%{release}/* $RPM_BUILD_ROOT/src
make all OBJROOT=$RPM_BUILD_ROOT

%install
make install OBJROOT=$RPM_BUILD_ROOT
if [ -f $RPM_BUILD_ROOT/src/eqnx.spec ]
then
	mv $RPM_BUILD_ROOT/src/eqnx.spec $RPM_BUILD_ROOT
fi
if [ -f ./eqnx.spec ]
then
        mv ./eqnx.spec $RPM_BUILD_ROOT
fi
rm -rf $RPM_BUILD_ROOT/src
rm -f $RPM_BUILD_ROOT/README.ssdiag

%clean
make clobber OBJROOT=$RPM_BUILD_ROOT

%preun
# check driver usage.  If ports in use, disallow the remove
if [ -e /sbin/chkconfig ]
then
	/sbin/chkconfig eqnx
	if [ $? != 0 ]
	then
		exit 0
	fi
fi

if [ -f /usr/sbin/rc.eqnx ]
then
	/usr/sbin/rc.eqnx status > /dev/null 2>&1
	if [ $? != 0 ]
	then
		echo "eqnx driver in use, unable to unload"
		echo "Please stop all processes running on SST ports"
		exit 1
	fi
fi

# stop the driver + remove device files
if [ -f /usr/sbin/rc.eqnx ]
then
	/usr/sbin/rc.eqnx stop
fi

# remove eqnx from startup scripts
if [ -e /usr/sbin/eqnx-installrc ]
then
	/usr/sbin/eqnx-installrc -u
fi

%post
echo "To complete installation of the Equinox SST product:"
echo "   1. /usr/sbin/rc.eqnx start"
echo "   2. ensure this script is invoked at boot-time"
echo "	    (such as chkconfig --add eqnx, for redhat, etc.)"
echo "   Refer to installation notes for more information".

%files
%defattr(-,root,root)
%doc inst/INSTALL.TXT
%doc inst/RELEASE.TXT
%doc utils/ssdiag/README.ssdiag
/lib/modules/*/*/*
/usr/bin/sscode
/usr/bin/ssdiag
/usr/bin/ssmkn
/usr/bin/ssrm
/usr/bin/sstty
/usr/sbin/rc.eqnx
/usr/sbin/rpmvar
/usr/sbin/eqnx-cfg
/usr/sbin/eqnx-installrc
/usr/lib/sst/ss.hlp
/eqnx.spec

%changelog
* Thu Jun 06 2007 Mike Straub (michael.straub@avocent.com)
- changes for release 4.11.

* Tue Nov 22 2005 Mike Straub (michael.straub@avocent.com)
- changes for release 4.10.

* Sun May 31 2005 Mike Straub (michael.straub@avocent.com)
- changes for release 4.09: add redhat ES 4 support, add x86_64 support,
  add 2.6.10+ fixes.
  
* Tue Oct 26 2004 Mike Straub (michael.straub@avocent.com)
- changes for release 4.08: modification to driver start-up procedure.
  Thanks to Joel Soete (soete.joel@tiscali.be).

* Fri Dec 19 2003 Mike Straub (mstraub@avocent.com)
- changes for release 4.07: add support for linux 2.6 kernels

* Tue Jul 8 2003 Mike Straub (mstraub@equinox.com)
- changes for release 4.06: comply with redhat 9 (rpm version 4.2)

* Thu Oct 24 2002 Mike Straub (mstraub@equinox.com)
- changes for release 4.05: multithreading and device tables
- improved installation checks.

* Tue Oct 22 2002 Mike Straub (mstraub@equinox.com)
- changed to adhere to redhat 8.0 (rpm version 4.1)

* Thu Feb 07 2002 Mike Straub (mstraub@equinox.com)
- added SST-4P/ULP board; fixed driver oops

* Fri Dec 28 2001 Mike Straub (mstraub@equinox.com)
- compliance with rpmlint version 0.32
- additional fixes for release 4.03

* Mon Nov 19 2001 Mike Straub (mstraub@equinox.com)
- conversion of existing RPM to comply with "good practices".
