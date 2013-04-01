/* 
 * Equinox SST driver for Linux.
 *
 * Copyright (C) 1999-2007 Equinox Systems Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <string.h>
#include <termios.h>

#include "brdtab.h"
#include "icp.h"
#include "eqnx_ioctl.h"
#include "ist.h"
#define DEV_VIRGIN      0xff            /* device id is unknown */
#define DEV_BAD         0               /* device was known but now offline */
#define DEV_GOOD        1               /* device known and online */
#define DEV_INSANE      2               /* hardware error (unexpected) */
#define DEV_WAITING	3
char DEVNAME[] =  "/dev/";

/*extern int errno;*/

char	*whoami;
struct disp dp;
struct ist_struct isttab;
void usage(void);
void print_lmx_info(int brd);
void fill_board_struct( struct eqnbrdstatus *bsptr, int brd);
int open_devfile(char *, struct stat *);
int log_to_phys(int);

#include "brdtab.c" 

int main(int argc, char **argv)
{
	register char *p;
        register int fd, eqnfd;
	int arg;
	int rval;
	unsigned int param;
	char flag[32];
	struct stat stbuf;
	int	portfd;
	
	fd = fileno(stdin);

	if ((p = strrchr(*argv, '/')))
		whoami = p + 1;
	else
		whoami = *argv;
	if (argc == 1 || argc > 4)
		usage();

   /* Turn off RAMP trace */
	if (argv[1][0] == '-' && argv[1][1] == 'v' && argv[1][2] == 'm')
	{
		int rval, dummy;

	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed,errno=%d\n",errno);
			exit(1);
		}
	        if((rval = ioctl(eqnfd, EQNTRACEOFF, &dummy))){
			fprintf(stderr,"%s: command failed\n",whoami);
			fprintf(stderr,"rval %x errno %d\n",rval,errno);
			exit(1);
	        }
      exit(0);
   }

   /* Turn on RAMP trace */
	else if (argv[1][0] == '+' && argv[1][1] == 'v' && argv[1][2] == 'm')
	{
		int rval, dummy;

	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed,errno=%d\n",errno);
			exit(1);
		}
	        if((rval = ioctl(eqnfd, EQNTRACEON, &dummy))){
			fprintf(stderr,"%s: command failed\n",whoami);
			fprintf(stderr,"rval %x errno %d\n",rval,errno);
			exit(1);
	        }
		exit(0);
	}

   /* Turn off ixany or ixon */
	if (argv[1][0] == '-' && argv[1][1] == 'i' && argv[1][2] == 'x')
	{
		if(argv[1][3] == 'o')
		   arg = 2;
		else if(argv[1][3] == 'a')
		   arg = 4;
		else arg = 0;
		if (argc < 3){
			if(argv[1][3] == 'o')
			     fprintf(stderr, "Usage:  %s -ixon [1a1-8h16]\n", whoami);
			else fprintf(stderr, "Usage:  %s -ixany [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY, arg)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Reset Port */
	else if (argv[1][0] == '-' && argv[1][1] == 'r' && argv[1][2] == 'p')
	{
		if (argc < 3) {
			fprintf(stderr, "Usage:  %s -rp [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		arg = (int)(stbuf.st_rdev);
	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed,errno=%d\n",errno);
			exit(1);
		}
		if(ioctl(eqnfd, EQNCHRESET, (char *) &arg)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Turn on ixany or ixon */
	else if (argv[1][0] == '+' && argv[1][1] == 'i' && argv[1][2] == 'x')
	{
		if(argv[1][3] == 'o')
		   arg = 1;
		else if(argv[1][3] == 'a')
		   arg = 3;
		else arg = 0;
		if (argc < 3){
			if(argv[1][3] == 'o')
			     fprintf(stderr, "Usage:  %s +ixon [1a1-8h16]\n", whoami);
			else fprintf(stderr, "Usage:  %s +ixany [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY, arg)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Resume output */
	else if(argv[1][0] == '+' && argv[1][1] == 'r' && argv[1][2] == 's')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +rs [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		arg = (int)(stbuf.st_rdev);
	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
				fprintf(stderr,"open of /dev/eqn failed,errno=%d\n",errno);
				exit(1);
		}
		if(ioctl(eqnfd, EQNRESUME, (char *) &arg)) {
				fprintf(stderr,"%s: command failed\n",whoami);
				exit(1);
		}
		exit(0);
	}

   /* Turn on DTR */
	else if(argv[1][0] == '+' && argv[1][1] == 'd' && argv[1][2] == 't' &&
			argv[1][3] == 'r')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +dtr [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXDTR, 1)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Turn off DTR */
	else if(argv[1][0] == '-' && argv[1][1] == 'd' && argv[1][2] == 't' &&
			argv[1][3] == 'r')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s -dtr [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXDTR, 0)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Enable hardware flow control */
	else if(argv[1][0] == '+' && argv[1][1] == 'r' && argv[1][2] == 't')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +rts [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY, (char *) 17)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}
			
   /* Disable hardware flow control */
	else if(argv[1][0] == '-' && argv[1][1] == 'r' && argv[1][2] == 't')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +rts [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY,(char *) 18)) {
				fprintf(stderr,"%s: command failed\n",whoami);
				exit(1);
		}
		exit(0);
	}

   /* Loopback port */
	else if(argv[1][0] == '+' && argv[1][1] == 'l' && argv[1][2] == 'b')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +lb [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY,(char *) 21)) {
			fprintf(stderr,"%s: command failed %d\n",
					whoami, errno);
			exit(1);
		}
		exit(0);
	}
			
   /* Stop port loopback */
	else if(argv[1][0] == '-' && argv[1][1] == 'l' && argv[1][2] == 'b')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s -lb [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY,(char *) 22)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Discard xon/xoff */
	else if(argv[1][0] == '+' && argv[1][1] == 'x' && argv[1][2] == 'o')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +xon [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY, (char *) 23)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}
			
   /* Do not discard xon/xoff */
	else if(argv[1][0] == '-' && argv[1][1] == 'x' && argv[1][2] == 'o')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s -xon [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY, (char *) 24)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Suppress ring confirm messages */
	else if(argv[1][0] == '+' && argv[1][1] == 'q' && argv[1][2] == 't')
	{
		int rval, dummy = 1;
	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed\n");
			exit(1);
		}
	   	if((rval = ioctl(eqnfd, EQNQUIETON, &dummy))){
			fprintf(stderr,"%s: command failed\n",whoami);
			fprintf(stderr,"rval %x errno %d\n",rval,errno);
			exit(1);
	   	}
	   exit(0);
	}

   /* Enable ring confirm messages */
	else if(argv[1][0] == '-' && argv[1][1] == 'q' && argv[1][2] == 't')
	{
		int rval, dummy = 1;
	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed\n");
			exit(1);
		}
	   	if((rval = ioctl(eqnfd, EQNQUIETOFF, &dummy))){
			fprintf(stderr,"%s: command failed\n",whoami);
			fprintf(stderr,"rval %x errno %d\n",rval,errno);
			exit(1);
	   	}
	   exit(0);
	}
	
   /* Lock port settings */
	else if(argv[1][0] == '+' && argv[1][1] == 'l' && argv[1][2] == 'c')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s +lck [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
		if(ioctl(portfd, EQNXSTTY,(char *) 25)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
		}
		exit(0);
	}

   /* Unlock port settings */
	else if(argv[1][0] == '-' && argv[1][1] == 'l' && argv[1][2] == 'c')
	{
		if (argc < 3){
			fprintf(stderr, "Usage:  %s -lck [1a1-8h16]\n", whoami);
			exit(1);
		}
		portfd = open_devfile(argv[2], &stbuf);
	   	if(ioctl(portfd, EQNXSTTY,(char *) 26)) {
			fprintf(stderr,"%s: command failed\n",whoami);
			exit(1);
	   	}
	   	exit(0);
	}

	/* Quiet bus expansion/failure messages */
	else if (argv[1][0] == '-' && argv[1][1] == 'q')
	{
		int rval, dummy;

	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed,errno=%d\n",errno);
			exit(1);
		}
	        if((rval = ioctl(eqnfd, EQNQUIETON, &dummy))){
			fprintf(stderr,"%s: command failed\n",whoami);
			fprintf(stderr,"rval %x errno %d\n",rval,errno);
			exit(1);
	        }
		exit(0);
	}

	/* Enable bus expansion/failure messages */
	else if (argv[1][0] == '+' && argv[1][1] == 'q')
	{
		int rval, dummy;

	   	if ((eqnfd = open("/dev/eqn",O_RDONLY)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed,errno=%d\n",errno);
			exit(1);
		}
	        if((rval = ioctl(eqnfd, EQNQUIETOFF, &dummy))){
			fprintf(stderr,"%s: command failed\n",whoami);
			fprintf(stderr,"rval %x errno %d\n",rval,errno);
			exit(1);
	        }
		exit(0);
	}

	argv++;
	if (**argv == '-') {
		if (argv[0][2] != '\0')
			usage();
		switch (argv[0][1]) {

#ifdef	ISA_ENAB
		case 'i': {
			int i;

			eqnfd = open ("/dev/eqn",O_RDONLY);
			if (eqnfd < 0) {
				fprintf(stderr,"%s: open of /dev/eqn failed,errno=%d\n",whoami,errno);
				exit (1);
			}
			rval = ioctl (eqnfd,EQNISTCFG,&isttab);
			if (rval < 0) {
				fprintf(stderr,"%s: ioctl EQNISTCFG on /dev/eqn failed, errno=%d\n",whoami,errno);
				exit (1);
			}
			if (isttab.sys_isa_holes[0] != 0) { 
				printf("Only 16 bit holes will be used automatically.\n\n");
				printf("ISA Memory hole status\n");
				printf("----------------------\n");
			}
			else {
				printf("No ISA Memory hole status to report\n");
				break;
			}
		
			for (i=0; i< HOLE_TABLE_SIZE; i++) {
				if (isttab.sys_isa_holes[i] == 0) break;
				printf("hole 0x%x   status 0x%x  ",
					(unsigned int)isttab.sys_isa_holes[i],
					(unsigned int) isttab.sys_isa_hole_status[i]);
				switch ( isttab.sys_isa_hole_status[i] ) {
					case 1:
						printf("  Occupied by ROM\n");
						break;
					case 2:
						printf("  Occupied by RAM\n");
						break;
					case 3:
						printf(" Failed: Address is cached\n");
						break;
					case 4:
						printf(" Failed: Could not detect channel counter advance\n");
						break;
					case 0x80:
						printf(" Failed: Driver Map of address failed\n");
						break;
					case 0xf0:
						printf(" Available, bus size not determined\n");
						break;
					case 0xf1:
						printf(" Available, bus size of 8 bits\n");
                                                break;
                                        case 0xf2:
                                                printf(" Available, bus size of 16 bits (8 bit ROM present)\n");
                                                break;
                                        case 0xf3:
                                                printf(" Available, bus size of 16 bits \n");
                                                break;
					case 0xfd:
						printf(" Address in use by driver, bus size of 16 bits (8 bit ROM present)\n");
						break;
					case 0xfe:
/*
                                                printf(" Available, bus size of 8 bits\n");
*/
						printf(" Address in use by driver, bus size of 8 bits\n");

						break;
					case 0xff:
						printf(" Address in use by driver, bus size of 16 bits\n");
						break;
					default:
						printf(" Failed: Unknown reason\n");
						break;
				}
			}
			break;
		}
#endif	/* ISA_ENAB */

		case 'a':{
		unsigned char devname[16];
		struct SttyStatus sttystatus;

		if (argc < 3){
			fprintf(stderr, "Usage: %s -a [1a1-8h16]\n", whoami);
			exit(1);
		}
		
		if (strlen(argv[1]) > 4) {
			fprintf(stderr,
			"Device %s invalid: use names like 1a1 or 2b16\n",argv[1]);
			exit(1);
		}
		sprintf(devname,"/dev/ttyQ%s",argv[1]);

		if (stat(devname, &stbuf)){
			fprintf(stderr,"stat(2) of %s failed.\n", devname);
			exit(1);
		}
		arg = (int)(stbuf.st_rdev);
		sttystatus.mp_dev = arg;
	   	if ((eqnfd = open("/dev/eqn",O_RDWR)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed\n");
			exit(1);
		}
		if(ioctl(eqnfd, EQNSTATUS2, (caddr_t)&sttystatus)) {
			fprintf(stderr,"%s: get status failed\n",whoami);
			exit(1);
		}
		param = sttystatus.mpc_param;
			for (arg=0; arg < 17; arg++) {
				if((param >> arg) & 1)
					flag[arg] = '+';
				else
					flag[arg] = '-';
			}
			printf("\n%cixon %cixany %ccts %crts %clb %cxon %clck\n",flag[0],flag[1],flag[6],flag[7],flag[10],flag[11],flag[12]);
			break;
		}

		case 'c':
			eqnfd = open ("/dev/eqn",O_RDONLY);
			if (eqnfd < 0) {
				fprintf(stderr,"%s: open of /dev/eqn failed,errno=%d\n",whoami,errno);
				exit (1);
			}
			rval = ioctl (eqnfd,EQNISTCFG,&isttab);
			if (rval < 0) {
				fprintf(stderr,"%s: ioctl EQNISTCFG on /dev/eqn failed, errno=%d\n",whoami,errno);
				exit (1);
			}
			if ( isttab.ctlr_max_num < isttab.ctlr_found)
				rval = isttab.ctlr_max_num;
			else
				rval = isttab.ctlr_found;
			for (arg=0; arg < rval; arg++) {
				char initstate[40]; 
				int brd;
				struct brdtab_t *board_def;
				unsigned char devid, devid_ext;
				unsigned short id;
				if ( isttab.ctlr_array[arg].cnfg_state == 1 ) 
					initstate[0] = (char) 0; /* Empty null-terminated string */
				else
					sprintf(initstate,"Init failed, reason code %d",isttab.ctlr_array[arg].cnfg_fail); 

				brd = log_to_phys(arg+1);
				if (brd < 0)
					brd = arg+1;

				devid = isttab.ctlr_array[arg].ctlr_model;
				devid_ext = isttab.ctlr_array[arg].ctlr_model_ext;
				id = devid | (devid_ext << 8);

				board_def = find_board_def(id, 
					isttab.ctlr_array[arg].ctlr_bus_type);

				switch (isttab.ctlr_array[arg].ctlr_bus_type) {

					case PCI_BUS: 
						if (board_def) {
				     			printf("Board %d id 0x%x %s Slot %d %s\n",
									brd, devid, board_def->name,
									isttab.ctlr_array[arg].ctlr_loc,initstate);
							if (board_def->number_of_ports >= 24)
								print_lmx_info(arg);
		     				} else	printf("Board %d, ids 0x%x / 0x%x Slot %d unknown PCI id\n",
									brd, devid, devid_ext,
									isttab.ctlr_array[arg].ctlr_loc);
						break;

					case ISA_BUS:
						if (board_def) {
			     				printf("Board %d id 0x%x %s I/O 0x%x %s\n",
									brd, devid, board_def->name,
									isttab.ctlr_array[arg].ctlr_loc,initstate);
							if (board_def->number_of_ports >= 24)
								print_lmx_info(arg);
						} else	printf("Board %d id 0x%x I/O 0x%x ISA type unknown \n",
									brd, devid,
									isttab.ctlr_array[arg].ctlr_loc);
						break;

					case EISA_BUS: 
						if (board_def) {
			     				printf("Board %d id 0x%x %s I/O 0x%x %s\n",
									brd, devid, board_def->name,
									isttab.ctlr_array[arg].ctlr_loc,initstate);
							if (board_def->number_of_ports >= 24)
								print_lmx_info(arg);
						} else	printf("Board %d id 0x%x I/O 0x%x EISA type unknown \n",
									brd, devid,
									isttab.ctlr_array[arg].ctlr_loc);

						break;

					case MCA_BUS: 
						if (board_def) {
			     				printf("Board %d id 0x%x %s I/O 0x%x %s\n",
									brd, devid, board_def->name,
									isttab.ctlr_array[arg].ctlr_loc,initstate);
							if (board_def->number_of_ports >= 24)
								print_lmx_info(arg);
						} else	printf("Board %d id 0x%x I/O 0x%x Microchannel type unknown \n",
									brd, devid,
									isttab.ctlr_array[arg].ctlr_loc);

						break;
					default:
			     				printf("Board %d Slot %d id 0x%x bus type %d unknown\n",
									brd,
									isttab.ctlr_array[arg].ctlr_loc,
									isttab.ctlr_array[arg].ctlr_model,
									isttab.ctlr_array[arg].ctlr_bus_type); 
					} /* bus_type */
				} /*for*/
				if (!rval)
			  	fprintf(stderr,"No boards found\n");
			break;

		case 'm':{
		unsigned char devname[16];
		struct ModemStatus mdstatus;

		if (argc < 3){
			fprintf(stderr, "Usage: %s -m [1a1-8h16]\n", whoami);
			exit(1);
		}
		
		if (strlen(argv[1]) > 4) {
			fprintf(stderr,
			"Device %s invalid: use names like 1a1 or 2b16\n",argv[1]);
			exit(1);
		}
		sprintf(devname,"/dev/ttyQ%s",argv[1]);

		if (stat(devname, &stbuf)){
			fprintf(stderr,"stat(2) of %s failed.\n", devname);
			exit(1);
		}
		arg = (int)(stbuf.st_rdev);
		mdstatus.mp_dev = arg;
	   	if ((eqnfd = open("/dev/eqn",O_RDWR)) < 0) {
			fprintf(stderr,"open of /dev/eqn failed\n");
			exit(1);
		}
		if(ioctl(eqnfd, EQNSTATUS3, (caddr_t)&mdstatus)) {
			fprintf(stderr,"%s: get status failed\n",whoami);
			exit(1);
		}
		param = mdstatus.mpstatus;
/******************************************************************************
  
        Return the current set of input and output control signals.

        The value returned is "native" icp bit positions regardless of the 
        origin of the signals.  Namely,

                b0      dcd
                b1      cts
                b2      dsr
                b3      ri
                -----------
                b4      dtr
                b5      rts
                b6      0
                b7      0
  
        The returned bits are POSITIVE logic... 1 means signal is on.

******************************************************************************/
			for(arg=0; arg < 6; arg++)
	    		    flag[arg] = ' ';
			if(!(param & 0x1)) flag[0] = '-'; /* dcd */
			if(!(param & 0x2)) flag[1] = '-'; /* cts */
			if(!(param & 0x4)) flag[2] = '-'; /* dsr */
			if(!(param & 0x8)) flag[3] = '-'; /* ri */
			if(!(param & 0x10)) flag[4] = '-'; /* dtr */
			if(!(param & 0x20)) flag[5] = '-'; /* rts */
			printf("\n%cdtr  %crts  %cdcd  %ccts  %cdsr  %cri\n",flag[4],flag[5],flag[0],flag[1],flag[2],flag[3]);
			break;
		}

		case 's': {
		unsigned char devname[16];

		if (argc < 3){
			fprintf(stderr, "Usage: %s -s [1a1-8h1]\n", whoami);
			exit(1);
		}
		
		if (strlen(argv[1]) > 4) {
			fprintf(stderr,
			"Device %s invalid: use names like 1a1 or 2b16\n",argv[1]);
			exit(1);
		}
		sprintf(devname,"/dev/ttyQ%s",argv[1]);

		if (stat(devname, &stbuf)){
			fprintf(stderr,"stat(2) of %s failed.\n", devname);
			exit(1);
		}
		arg = (int)(stbuf.st_rdev);
		dp.mp_dev = arg;
		dp.ioctl_version = IOCTL_VERSION;
	   	if ((eqnfd = open("/dev/eqn",O_RDWR)) < 0) {
				fprintf(stderr,"open of /dev/eqn failed\n");
				exit(1);
			}
			if(ioctl(eqnfd, EQNSTATUS1, (caddr_t)&dp)) {
				fprintf(stderr,"%s: get status failed\n",whoami);
				exit(1);
			}
		printf("\nSTATUS of PORT \n===================\n");
		printf("state  = %4x  flags = %4x  g_attn = %2x  g_init = %2x  g_rx_attn0 = %x\n",dp.mp_state, dp.mp_flags,dp.mp_g_attn,dp.mp_g_init,dp.mp_g_rx_attn0);
		printf("cflag  = %4x  iflag = %4x   oflag   = %4x  lflag = %4x\n",dp.mp_cflag,dp.mp_iflag,dp.mp_oflag,dp.mp_lflag);
		printf("outqc  = %2x    rawqc = %2x     \n",
			(unsigned int) dp.mp_outqcc,
			(unsigned int) dp.mp_rawqcc);
		printf("ldisp  = %2d    param = %x\n",dp.mp_ldisp, (unsigned int) dp.mp_param);
		printf("mpa_stat = %x\n",dp.mp_mpa_stat);
		printf("+------------------------ ICP ------------------------------+\n");
		printf("txbase = %2x    tx_q_ptr = %3x    tx_q_cnt = %x   tx_cnt = %x\n",dp.mp_tx_base,dp.mp_tx_q_ptr,dp.mp_tx_q_cnt,dp.mp_tx_count);
		printf("rxbase = %2x    rx_next  = %3x    rx_tail  = %x\n",dp.mp_rx_base,dp.mp_rx_next,dp.mp_rx_tail);
		printf("rx_fmt = %2x    rxbaud   = %2x   in_ctl  = %x\n",dp.mp_rx_fmt,dp.mp_rxbaud,dp.mp_cin_ctrl);
		printf("tx_fmt = %2x    txbaud   = %2x   out_ctl = %x\n",dp.mp_tx_fmt,dp.mp_txbaud,dp.mp_cout_ctrl);
		printf("rx_csr = %4x  tx_csr   = %4x\n",dp.mp_rx_csr,dp.mp_tx_csr);
		printf("rx_lck = %2x    cie      = %4x   cis     = %4x\n",dp.mp_rx_locks,dp.mp_cie,dp.mp_cis);
		printf("rxtdm  = %2x    txtdm  = %2x       rcv_cnt = %d   fifo_cnt = %d\n",dp.mp_rxtdm,dp.mp_txtdm,dp.mp_rx_rcv_cnt,dp.mp_fifo_cnt & 0xf);
		printf("tx_flw_cfg = %2x  susp_lmx = %2x   susp_sig = %x  tx_cpu_reg = %x\n",dp.mp_tx_flow_cfg,dp.mp_susp_lmx,dp.mp_susp_sig,dp.mp_tx_cpu_reg);
		printf("lmx_cmd = %2x  tx_ev_ena = %4x  tx_evnts = %4x  int_flgs = %2x\n",dp.mp_lmx_cmd,dp.mp_cieo,dp.mp_tx_events,dp.mp_int_flgs);
		printf("+-----------------------------------------------------------+\n");
		printf("brdno  = %2d    icpno = %2d    lmxno = %2d    chan = %2d\n",dp.mp_brdno,dp.mp_icpno,dp.mp_lmxno,dp.mp_chan);
		printf("start  = %2d    stop   = %2d    input   = %x     output = %x\n",dp.mp_start,dp.mp_stop,(unsigned int) dp.mp_input,(unsigned int)dp.mp_output);
			break;
	}
               break;

#ifdef	ISA_ENAB
                case 'v':
                {
                        struct movebrd mb;
                        eqnfd = open ("/dev/eqn",O_RDONLY);
			if (eqnfd < 0) {
				fprintf(stderr,"%s: open of /dev/eqn failed,errno=%d\n",whoami,errno);
				exit (1);
			}
                        if (argc < 3 || argc > 3)
                        {
                            fprintf(stderr, "%s: usage:\n\n", whoami);
                            fprintf(stderr, "  %s -v 0xaddr      move isa board(s) to addr\n", whoami);
                            fprintf(stderr, "\n");
                            exit(1);
                        }


                        if ((strncmp("0x",argv[1],2))==0)
                            sscanf(argv[1],"%li", &(mb.addr));
                        else
                          sscanf(argv[1],"%lx",&(mb.addr));

                        if (!mb.addr)
                        {
                            fprintf(stderr, "%s: usage:\n\n", whoami);
                            fprintf(stderr, "  %s -v 0xaddr      move isa board(s) to addr\n", whoami);
                            fprintf(stderr, "\n");
                            exit(1);
                        }
                        rval = ioctl (eqnfd,EQNMOVEBRD,&mb);
			if (rval < 0) {
                                fprintf(stderr,"%s: ioctl EQNMOVEBRD on /dev/eqn failed, errno=%d\n",whoami,errno);
				exit (1);
			}
                }
#endif	/* ISA_ENAB */

                     break;

		default:
			usage();
		}
	} else
		usage();
	exit(0);
}

void usage(void)
{
	fprintf(stderr, "%s: usage:\n\n", whoami);
	fprintf(stderr, "  %s -c                   list Equinox SST configuration\n",
		whoami);
#ifdef	ISA_ENAB
	fprintf(stderr, "  %s -i                   list ISA memory hole status table\n",
		whoami);
        fprintf(stderr, "  %s -v        [0xaddr]   move isa board(s) to addr\n", whoami);
#endif	/* ISA_ENAB */

	fprintf(stderr, "  %s [+-]q                quiet messages about bus expansion/failure\n", whoami);

	fprintf(stderr, "  %s -a        [1a1-8h16] display sstty settings of port \n", whoami);

	fprintf(stderr, "  %s -m        [1a1-8h16] display modem signals of port\n", whoami);
	fprintf(stderr, "  %s -s        [1a1-8h16] display status of port\n", whoami);
	fprintf(stderr, "  %s [+-]dtr   [1a1-8h16] toggle dtr. for port\n", whoami);
	fprintf(stderr, "  %s [+-]ixany [1a1-8h16] force ignore ixany for port\n", whoami);
	fprintf(stderr, "  %s [+-]ixon  [1a1-8h16] force ixon flow control on port \n", whoami);
	fprintf(stderr, "  %s [+-]lb    [1a1-8h16] loopback port\n", whoami);
	fprintf(stderr, "  %s [+-]lck   [1a1-8h16] lock port settings\n",whoami);
	fprintf(stderr, "  %s +rs       [1a1-8h16] resume output on port\n", whoami);
	fprintf(stderr, "  %s [+-]rts   [1a1-8h16] set RTSCTSFLOW flow control on port\n", whoami);
	fprintf(stderr, "  %s [+-]xon   [1a1-8h16] discard xon/xoff\n", whoami);
	fprintf(stderr, "  %s [+-]vm               verbose RAMP messages\n", whoami);
	fprintf(stderr, "  %s -rp       [1a1-8h16] reset port\n", whoami);

	fprintf(stderr, "\n");
	exit(1);
}

void print_lmx_info(int brd)
{
	struct eqnbrdstatus brdstat;
	int nicps, lmx_no, pm_no;
#define EICPCOUNT (brdstat.nicps)
#define BRD_COUNT (brdstat.gbl_neqnbrds)
#define	S_LDV_ALIVE (brdstat.lmx[nicps][lmx_no].lmx_active)
#define	S_LDV_ID (brdstat.lmx[nicps][lmx_no].lmx_id)
#define	S_RDV_ALIVE (brdstat.lmx[nicps][lmx_no].lmx_rmt_active)
#define	S_RDV_ID (brdstat.lmx[nicps][lmx_no].lmx_rmt_id)

	fill_board_struct(&brdstat, brd);
	for(nicps = 0; nicps <  EICPCOUNT; nicps++){
		for(lmx_no = 0; lmx_no < 4; lmx_no++){
			pm_no = lmx_no + ( 4 * nicps) + 1;
			switch( S_LDV_ALIVE ){
				case DEV_GOOD:
					if(S_LDV_ID == 0x10){   /* GDC */
			     			printf("\t-Port Module %d GDC32\n",pm_no);
						break;
					}	
					if(S_LDV_ID == 0){
						/*LDVFOUND */
			     			printf("\t-Port Module %d PM16\n",pm_no);
						break;
					}	
					if(S_LDV_ID == 7){
						/*LDVFOUND422*/
			     			printf("\t-Port Module %d PM16-422\n",pm_no);
						break;
					}	
					if(S_LDV_ID == 1) {  /* bridge */
						if(S_RDV_ALIVE == DEV_GOOD 
							&& S_RDV_ID == 2) {	
							/*MUXFOUND*/
							if (lmx_no < 4)
			     					printf("\t-Port Module %d CMX16\n",pm_no);
							else
			     					printf("\t-Port Module %d HCMX16\n",pm_no);
						} else {
							/*RDVFOUND*/
			     					printf("\t-Port Module %d MIM1\n",pm_no);
						}	
						break;
					}
					if(S_LDV_ID == 4 ){
						/*LDV8FOUND*/
			     			printf("\t-Port Module %d PM8\n",pm_no);
						break;
					}	
					if(S_LDV_ID == 8 ){
						/*RAMP1FOUND*/
			     			printf("\t-Port Module %d MP16\n",pm_no);
						break;
					}	
					if(S_LDV_ID == 9 ){
						/*RAMP2FOUND*/
			     			printf("\t-Port Module %d MP2E\n",pm_no);
						break;
					}	
					if(S_LDV_ID == 0xB ){
						/*RAMP4FOUND*/
			     			printf("\t-Port Module %d MP4E\n",pm_no);
						break;
					}	
					break;
				/*case DEV_VIRGIN:	
					topo[tidx++] = NONE;
					break;
				case DEV_BAD:
					topo[tidx++] = NONE;
					break;*/
				default:
					break;
			}
		}
	}
}

void fill_board_struct( struct eqnbrdstatus *bsptr, int brd)
{
	int retval, pfd;

    if((pfd = open( "/dev/eqn", O_RDWR )) < 0){
		fprintf(stderr,"%s: open of /dev/eqn failed,errno=%d\n",
			whoami,errno);
		exit (1);
	}
	bsptr->brd_nbr = brd + 1;
	bsptr->ioctl_version = IOCTL_VERSION;
	if((retval = ioctl( pfd, EQNBRDSTATUS, bsptr))){
		fprintf(stderr,"%s: ioctl EQNBRDSTATUS on /dev/eqn failed, errno=%d\n",whoami,errno);
		exit (1);
	}
}

int
open_devfile(char *name, struct stat *stbuf)
{
	int	portfd;
	char	devname[25];

	if (strlen(name) > 4) {
		fprintf(stderr,
		"Device name %s invalid: use names like 1a1 or 2b16\n", name);
		exit(1);
	}
	sprintf(devname,"/dev/ttyQ%s", name);
	if (stat(devname, stbuf)){
		fprintf(stderr,"stat(2) of %s failed.\n", devname);
		exit(1);
	}
	if ((portfd = open(devname,O_RDONLY)) < 0) {
		fprintf(stderr,"open of %s failed, errno=%d\n", devname, errno);
		exit(1);
	}
	return (portfd);
}

#define	SSDBASE	"/etc/eqnx/database"

/* return physical board number */
int log_to_phys(logdev)
{
	FILE	*datab=NULL;
	char	datain[80];
	int	brd, log, phys, found = 0;

	datab = fopen(SSDBASE, "r");
	if (datab == NULL)
		return -1;

	brd = 0;
	while ((fgets(datain, 80, datab) != NULL)) {
		sscanf(datain ,"%d%d", &phys, &log);
		if (log == logdev) {
			found++;
			break;
		}
		brd++;
	}

	fclose(datab);

	if (found)
		return (brd+1);
	else	return (-1);
}

	

