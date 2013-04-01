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
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <termios.h>

#include "brdtab.h"
#include "icp.h"
#include "eqnx_ioctl.h"
#include "ist.h"
#define false (0)
#define true (!false)

extern int din_num; 
int diag_num = 0;

/* syntax: mkn */
/* reads a file, updates /dev, writes new version of file. */

#define MAXBOARDS (8)

#define BOARDISGOODTOGO (1) /* the only good cnfg_state */

#define BOARDISISA  (1)
#define BOARDISEISA (2)
#define BOARDISMC   (3)
#define BOARDISPCI  (4)
#define TEMPDELETE (-2) /* used herein to temporarily 'delete' a board */
#define PERMDELETE (-1) /* used by humans to PERMANENTLY 'delete' a board */
#define BDIDNONE    (NOID)
#define BUSNONE	    (NOBUS)

#define DEVPERBOARD   (128)
#define DEVPERMIM     (16)
#define modebits      (S_IFCHR|0666)
#define tty_base     "/dev/ttyQ"   /* home for callin devices */
#define dataBaseName  "database"
#define logFileName   "logfile"
#define staticCfgName "staticcfg"
#define basedir       "/etc/eqnx"
#define slash         "/"
#define idstring      (40)
#define drvpath  "/dev/eqn"   /* backdoor */
#define drvpath0  "/dev/eqn0"   /* backdoor */
#define drvdata0  (1)
#define drvpath1  "/dev/eqn1"   /* backdoor */
#define drvdata1  (2)
static FILE * logFile=NULL;
static char logLine[256];
void remove_devices( int logical, int bdid, int physical);

struct _d
{
	int physical;
	int bus_type;
	int place;
	int bdid;
	int bdrev;
	int bdmaj;
	int used;
} 
_d;

struct _bdmap
{
	int number;
	struct _d _d[MAXBOARDS];
} 
_bdmap;

static struct _bdmap bdmap;
static struct _bdmap oldBDM;
struct ist_struct reported;
static int staticcfg;
static int logged = false;
void recreate_device( char *name, dev_t dev_id);
void xxx_process_config(void);
void ist_process_config(void);
void logOldConfiguration(void);
void logNewConfiguration(void);
void logLocationChange( int bdid, int bus, int newPlace, 
	int logical, int oldPlace);
void logTypeChange( int newBdid, int bus, int place, int logical, int oldBdid);
void logChange( int newBdid, int newBus, int newPlace, int logical, 
		int oldBdid, int oldBus, int oldPlace);
void logRecovered( int bdid, int bus, int place, int logical);
void logMissing( int bdid, int bus, int place, int logical);
void logAdded( int bdid, int bus, int place, int logical);
void logUnexpected( int bdid, int bus, int place);
void logDouble(void);
void logTime(void);
void issue_ioctl(void);
void retrieve_config( FILE* file);
void obtain_config( char* filename);
void store_config( FILE *file, struct _bdmap *bdmap);
void recreate_directory(char *name);
int found_the_file( char *fn);
char *placeString( int bus, int place);
char *bdstr( int bdid, int bus);

#include "brdtab.c"

/************************************************************************/

char *bdstr( int bdid, int bus)
{
	static char bdstrv[idstring];
	struct brdtab_t *board_def;

	board_def = find_board_def((unsigned short) bdid, bus);
	if (board_def)
		strcpy (bdstrv, board_def->name);
	else	strcpy (bdstrv, "unknown");

	return bdstrv;
}

char *placeString( int bus, int place)
{
	static char pstrv[idstring];

	switch (bus)
	{
	case BOARDISISA:
		{
			strcpy(pstrv, "at I/O port ");
			sprintf(pstrv+strlen(pstrv), "%x", place);
			break;
		}
	case BOARDISEISA:
		{
			strcpy(pstrv, "in EISA slot ");
			sprintf(pstrv+strlen(pstrv), "%d", place);
			break;
		}
	case BOARDISPCI:
		{
			strcpy(pstrv, "in PCI slot ");
			sprintf(pstrv+strlen(pstrv), "%d", place);
			break;
		}
	case BOARDISMC:
		{
			strcpy(pstrv, "in Microchannel slot ");
			sprintf(pstrv+strlen(pstrv), "%d", place);
			break;
		}
	}
	return pstrv;
}

void validate_devices( int logical, int bdid, int bus, int physical)
{
	int number_of_devices; 
	dev_t rdev;
	char name[256];
	int device, minor_start, tty_major;
	unsigned short oldmask;
	int number_of_icps;
	char unit[3];
	struct brdtab_t *board_def;

	if(physical < 0) return;
	oldmask = umask(0);
	sprintf(unit,"%d", logical + 1);

	number_of_icps = 1;
	number_of_devices = 0;

	board_def = find_board_def((unsigned short) bdid, bus);
	if (board_def) {
		number_of_icps    = board_def->number_of_asics;
		number_of_devices = board_def->number_of_ports;
	}

	sprintf(name, "%s%s",tty_base,unit);
	minor_start = (physical % 2) * DEVPERBOARD;
	
	tty_major = bdmap._d[logical].bdmaj;
	for (device=minor_start; device<(minor_start + DEVPERBOARD); device++)
	{
		volatile unsigned int m = device;
		volatile int mux = (device - minor_start) / DEVPERMIM;
		volatile int jack = (device - minor_start) % DEVPERMIM;
		char devname[15];

		rdev = makedev(tty_major, m);
		if ((physical > PERMDELETE) && (device < 
			(minor_start + number_of_devices)) &&
		    (reported.ctlr_array[physical].cnfg_state == BOARDISGOODTOGO))
		{
			if (rdev!= 0){
				/* need to delete ... gaps from ssm12 or ssm 26 */
				sprintf(devname,"%s%c%d", name, 
					mux + 'a',jack + 1);
				if ((number_of_devices == 12) ||
				    (number_of_devices == 24)) { 
					if ((device > (minor_start + 11) && device < (minor_start + 16)) 
						|| device > (minor_start + 27) ) /* ssm 24*/
						unlink(devname);
					else
						recreate_device(devname,rdev);
				}
				else{
					recreate_device(devname,rdev);
				}
			}
			else
				unlink(name);
		}
		else
			unlink(name);
	}

	umask(oldmask);
}

void xxx_process_config(void)
{
	int bd_info_used[MAXBOARDS];
	int bd_cnt; 
	int tmp;
	int i; 
	int p; 
	int all_used;

	bd_cnt = reported.ctlr_max_num;
	tmp = reported.ctlr_found;
	if ( bd_cnt > tmp ) bd_cnt = tmp;
	if ( bd_cnt > MAXBOARDS ) bd_cnt = MAXBOARDS;

	/* save old configuration for logging */
	oldBDM = bdmap;
	/* mark all detected boards as unmapped */
	for (p=0; p<bd_cnt; p++)
		bd_info_used[p] =false;
	/* expected configuration was marked unmapped when read */

	/* scan for (type and "location") matches */
	all_used =true;
	for (i=0; i<bdmap.number; i++){
		int board_found =false;
		int bdm_type = bdmap._d[i].bdid;
		/* scan for a particular (type and "location") pair */
		for (p=0; p<bd_cnt; p++){
			int bdi_type = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
            		bdi_type |= reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
			if ((bd_info_used[p] == false) &&
			    (bdi_type == bdm_type) &&
			    (reported.ctlr_array[p].ctlr_loc == bdmap._d[i].place)){
				board_found =true;
				break;
			}
		}
		if (board_found){
			bdmap._d[i].used =true;
			bd_info_used[p] =true;
			/* check previous status */
			if (bdmap._d[i].physical < PERMDELETE){
				/* log recovered board */
				logRecovered(bdmap._d[i].bdid,
				    bdmap._d[i].bus_type,
				    bdmap._d[i].place,
				    i+1);
			}
			/* update physical number */
			bdmap._d[i].physical = p+1;
			/* update revision */
			bdmap._d[i].bdrev = reported.ctlr_array[p].ctlr_rev;
			/* update major number */
			bdmap._d[i].bdmaj = reported.ctlr_array[p].ctlr_major;
			validate_devices(i, bdmap._d[i].bdid, 
					bdmap._d[i].bus_type, p);
		}
		else
			/* something missing? perhaps, maybe later we'll find it */
			all_used =false;
	}

	/* scan for type matches */
	if (!all_used && !staticcfg){
		/*printf("!all_used (type matches)\n");*/
		all_used =true;
		for (i=0; i<bdmap.number; i++){
			/* sooner or later an unmapped board will be found */
			if (bdmap._d[i].used == false){
				int board_found =false;
				int bdm_type = bdmap._d[i].bdid;
				int bdm_ISA = bdmap._d[i].bus_type == BOARDISISA;
				/* scan for a particular type */
				for (p=0; p<bd_cnt; p++){
					int bdi_ISA = reported.ctlr_array[p].ctlr_bus_type == BOARDISISA;
					int bdi_type = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                    			bdi_type |= 
						reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
					if ((bd_info_used[p] == false) &&
					    (bdi_type == bdm_type) &&
					    (bdm_ISA == bdi_ISA)){
						board_found =true;
						break;
					}
				}
				if (board_found){
					bdmap._d[i].used =true;
					bd_info_used[p] =true;
					/* log change in "location" */
                    			logLocationChange(
					reported.ctlr_array[p].ctlr_model |
				      	reported.ctlr_array[p].ctlr_model_ext<<8,
					    reported.ctlr_array[p].ctlr_bus_type,
					    reported.ctlr_array[p].ctlr_loc,
					    i+1,
					    bdmap._d[i].place);
					/* update physical number */
					bdmap._d[i].physical = p+1;
					/* update "location" */
					bdmap._d[i].place = reported.ctlr_array[p].ctlr_loc;
					/* update revision */
					bdmap._d[i].bdrev = reported.ctlr_array[p].ctlr_rev;
					/* update major number */
					bdmap._d[i].bdmaj = reported.ctlr_array[p].ctlr_major;
					validate_devices(i, bdmap._d[i].bdid, 
						bdmap._d[i].bus_type, p);
				}
				else
					/* something missing? perhaps, maybe upgrade/downgrade */
					all_used =false;
			}
		}
	}

	/* scan for "location" matches */
	if (!all_used && !staticcfg){
		all_used =true;
		for (i=0; i<bdmap.number; i++){
			/* sooner or later an unmapped board will be found */
			if (bdmap._d[i].used == false){
				int board_found =false;
				/* scan for a particular "location" */
				for (p=0; p<bd_cnt; p++){
					if ((bd_info_used[p] == false) &&
					    (reported.ctlr_array[p].ctlr_loc == bdmap._d[i].place)){
						board_found =true;
						break;
					}
				}
				if (board_found){
                    			int bdi_type = 
					reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                    			bdi_type |= 
					reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
					bdmap._d[i].used =true;
					bd_info_used[p] =true;
					/* log change in type */
					logTypeChange(bdi_type,
					    reported.ctlr_array[p].ctlr_bus_type,
					    reported.ctlr_array[p].ctlr_loc,
					    i+1,
					    bdmap._d[i].bdid);
					/* update physical number */
					bdmap._d[i].physical = p+1;
					/* update bus */
					bdmap._d[i].bus_type = reported.ctlr_array[p].ctlr_bus_type;
					/* update type */
					bdmap._d[i].bdid = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                    			bdmap._d[i].bdid |= reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
					/* update revision */
					bdmap._d[i].bdrev = reported.ctlr_array[p].ctlr_rev;
					/* update major number */
					bdmap._d[i].bdmaj = reported.ctlr_array[p].ctlr_major;
					validate_devices(i, bdmap._d[i].bdid, 
						bdmap._d[i].bus_type, p);
				}
				else
					/* something missing? perhaps, maybe upgrade/downgrade */
					all_used =false;
			}
		}
	}

	/* scan for unspoken for logical name thingys */
	if (!all_used && !staticcfg){
		all_used =true;
		for (i=0; i<bdmap.number; i++){
			/* sooner or later an unmapped board will be found */
			if (bdmap._d[i].used == false){
				int board_found =false;
				/* scan for an unclaimed board */
				for (p=0; p<bd_cnt; p++){
					if (bd_info_used[p] == false){
						board_found =true;
						break;
					}
				}
				if (board_found){
                    			int bdi_type = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                    			bdi_type |= reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
					bdmap._d[i].used =true;
					bd_info_used[p] =true;
					/* log change */
					logChange(bdi_type,
					    reported.ctlr_array[p].ctlr_bus_type,
					    reported.ctlr_array[p].ctlr_loc,
					    i+1,
					    bdmap._d[i].bdid,
					    bdmap._d[i].bus_type,
					    bdmap._d[i].place);
					/* update physical number */
					bdmap._d[i].physical = p+1;
					/* update bus */
					bdmap._d[i].bus_type = reported.ctlr_array[p].ctlr_bus_type;
					bdmap._d[i].place = reported.ctlr_array[p].ctlr_loc;
					/* update type */
					bdmap._d[i].bdid = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                    			bdmap._d[i].bdid |= reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
					bdmap._d[i].bdrev = reported.ctlr_array[p].ctlr_rev;
					bdmap._d[i].bdmaj = reported.ctlr_array[p].ctlr_major;
					validate_devices(i, bdmap._d[i].bdid, 
						bdmap._d[i].bus_type, p);
				}
				else
					/* something missing */
					all_used =false;
			}
		}
	}

	/* log missing boards and/or process new boards */
	if (!all_used)
	{
		/* log missing boards */
		for (i=0; i<bdmap.number; i++)
		{
			if (bdmap._d[i].used == false)
			{
				/* log missing board */
				logMissing(bdmap._d[i].bdid,
				    bdmap._d[i].bus_type,
				    bdmap._d[i].place,
				    i+1);
				/* update configuration */
				bdmap._d[i].used =true;
				if (bdmap._d[i].physical >= 0){
					remove_devices(i, bdmap._d[i].bdid, 
						bdmap._d[i].physical - 1);
					bdmap._d[i].physical = TEMPDELETE;
				}
			}
		}
	}
	else if (!staticcfg)
	{
		/* add new boards */
		for (p=0; p<bd_cnt; p++)
		{
			if (bd_info_used[p] == false)
			{
				/* create new entry */
				i = bdmap.number;
				bdmap.number++;
				/* physical number */
				bdmap._d[i].physical = p+1;
				bdmap._d[i].bus_type = reported.ctlr_array[p].ctlr_bus_type;
				bdmap._d[i].place = reported.ctlr_array[p].ctlr_loc;
				bdmap._d[i].bdid = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                		bdmap._d[i].bdid |= reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
				bdmap._d[i].bdrev = reported.ctlr_array[p].ctlr_rev;
				bdmap._d[i].bdmaj = reported.ctlr_array[p].ctlr_major;
				bdmap._d[i].used =true;
				bd_info_used[p] =true;
				validate_devices(i, bdmap._d[i].bdid, 
						bdmap._d[i].bus_type, p);
				/* log added board */
				logAdded(bdmap._d[i].bdid,
				    bdmap._d[i].bus_type,
				    bdmap._d[i].place,
				    i+1);
			}
		}
	}
	if (staticcfg){
		/* ignore new boards */
		for (p=0; p<bd_cnt; p++)
		{
			if (bd_info_used[p] == false)
			{
                    		int bdi_type = reported.ctlr_array[p].ctlr_model;
/* SSP4 fix Rel 3.20 */
                    		bdi_type |= reported.ctlr_array[p].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */
				/* ignore new entry */
				bd_info_used[p] =true;
				/* log ignored board */
				logUnexpected(bdi_type,
				    reported.ctlr_array[p].ctlr_bus_type,
				    reported.ctlr_array[p].ctlr_loc);
			}
		}
	}

	if (logged){
		/* unlink any leftover names */
		for (i=bdmap.number; i<reported.ctlr_max_num; i++)
			validate_devices(i, BDIDNONE, BUSNONE, TEMPDELETE);
	}

	recreate_device(drvpath0, makedev(diag_num, drvdata0));
	recreate_device(drvpath1, makedev(diag_num, drvdata1));

/* The following sections only do an 'ls' to list all devices after creating.
This is not necessary, and for large numbers of devs, screen info is lost. */
/*
#undef tcs
#define tcs "ls -laFR /dev/ttyQ*|sort"
	printf("\n%s\n",tcs);
	fflush(stdout);
	system(tcs);
	fflush(stdout);
#undef tcs
#define tcs "ls -laFR /dev/cuq*|sort"
	printf("\n%s\n",tcs);
	fflush(stdout);
	system(tcs);
	fflush(stdout);
#undef tcs
#define tcs "ls -laFR /dev/eqn*"
	printf("\n%s\n",tcs);
	fflush(stdout);
	system(tcs);
	fflush(stdout);
#undef tcs
*/
}
/************************************************************************************/
void ist_process_config(void)
{
	int bd_cnt = reported.ctlr_max_num;
	int tmp = reported.ctlr_found;
	int identical;
	if (bd_cnt>tmp) bd_cnt=tmp;
	if (bd_cnt>MAXBOARDS) bd_cnt=MAXBOARDS;
	identical = false;
	if (bd_cnt==bdmap.number){
		int i;
		identical = true;
		for (i=0; i<bd_cnt; i++){
			/* check all boards for identical */
			int bdm_type = bdmap._d[i].bdid;
			int bdi_type = reported.ctlr_array[i].ctlr_model;
/* SSP4 fix Rel 3.20 */
            		bdi_type |= reported.ctlr_array[i].ctlr_model_ext << 8;
/* SSP4 fix Rel 3.20 */ 
			if ((bdi_type != bdm_type) ||
			    ((bdmap._d[i].physical < PERMDELETE) &&
			    (reported.ctlr_array[i].cnfg_state == BOARDISGOODTOGO)) ||
			    (reported.ctlr_array[i].ctlr_loc != bdmap._d[i].place) ||
			    (reported.ctlr_array[i].ctlr_major != bdmap._d[i].bdmaj))
				identical = false;
		}
	}
	{
		int i; 
		int check = false; 
		int value = 0;
		for (i=0; i<reported.ctlr_found; i++){
			if (reported.ctlr_array[i].ctlr_bus_type == BOARDISISA)
				check = true;
		}
		if (check){
			check = false;
			if (reported.sys_isa_holes[0] != 0){
				for (i=0; reported.sys_isa_holes[i] != 0; i++){
					if (reported.sys_isa_hole_status[i] > 0xfc){
						value = reported.sys_isa_holes[i];
						break;
					}
				}
				if (value != 0)
					fprintf(logFile,"EQNX: ISA boards mapped at address: 0x%x\n",value);
				else{
					sprintf(logLine, "EQNX: WARNING: Unable to memory map ISA boards.\n");
					logDouble();
				}
			}
		}
	}
	if (identical)
		printf("EQNX: Device update not required\n");
	else{
		printf("EQNX: Updating devices. Please be patient\n");
		xxx_process_config();
	}
	logNewConfiguration();
}
int found_the_file( char *fn)
{
	struct stat buffer;
	return stat(fn, &buffer)==0;
}
void logOldConfiguration(void)
{
	if (logFile != NULL){
		/* if not previously logged, log configuration and inform others */
		if (!logged){
			if (oldBDM.number != 0){
				fprintf(logFile,
				    "EQNX: Previous Configuration\nLogical Physical Bus    Slot   I/O port Board Type/Revision\n"
				    );
				store_config(logFile, &oldBDM);
			}
			else
				fprintf(logFile,"EQNX: NO Previous Configuration\n");
			logged = true;
		}
	}
}

void logNewConfiguration(void)
{
	if (logFile != NULL){
		/* if old configuration logged, log new configuration */
		if (logged){
			fprintf(logFile,
			    "\nEQNX: Current Configuration\nLogical Physical Bus    Slot   I/O port Board Type/Revision\n"
			    );
			store_config(logFile, &bdmap);
			logged = false;
		}
		else
			fprintf(logFile,"EQNX: No alterations to configuration.\n");
	}
}

void logLocationChange( int bdid, int bus, int newPlace, int logical, int oldPlace)
{
	char nps[idstring]; 
	char ops[idstring];
	logOldConfiguration();
	strcpy(nps,placeString(bus,newPlace));
	strcpy(ops,placeString(bus,oldPlace));
	sprintf(logLine,
	    "EQNX: Board type %s found %s. Configured as board %d.\n      Expected %s.\n",
	    bdstr(bdid,bus), nps, logical, ops);
	logDouble();
}

void logTypeChange( int newBdid, int bus, int place, int logical, int oldBdid)
{
	char nbs[idstring]; 
	char obs[idstring];
	logOldConfiguration();
	strcpy(nbs,bdstr(newBdid,bus));
	strcpy(obs,bdstr(oldBdid,bus));
	sprintf(logLine,
	    "EQNX: Board type %s found %s. Configured as board %d.\n      Expected %s.\n",
	    nbs, placeString(bus,place), logical, obs);
	logDouble();
}

void logChange( int newBdid, int newBus, int newPlace, int logical, int oldBdid, int oldBus, int oldPlace)
{
	char nbs[idstring]; 
	char nps[idstring];
	char obs[idstring]; 
	char ops[idstring];
	logOldConfiguration();
	strcpy(nbs,bdstr(newBdid,newBus));
	strcpy(nps,placeString(newBus,newPlace));
	strcpy(obs,bdstr(oldBdid,oldBus));
	strcpy(ops,placeString(oldBus,oldPlace));
	sprintf(logLine,
	    "EQNX: %s %s found %s. Configured as board %d.\n      Expected %s %s.\n", 
	    "Board Type", nbs, nps, logical, obs, ops);
	logDouble();
}

void logRecovered( int bdid, int bus, int place, int logical)
{
	logOldConfiguration();
	sprintf(logLine,
	    "EQNX: Board type %s FOUND %s.\n      %s %d, mapping recovered.\n",
	    bdstr(bdid,bus), placeString(bus,place),
	    "Previously configured as board", logical);
	logDouble();
}

void logMissing( int bdid, int bus, int place, int logical)
{
	logOldConfiguration();
	sprintf(logLine,
	    "EQNX: Board type %s NOT FOUND %s.\n      %s %d, mapping maintained.\n",
	    bdstr(bdid,bus), placeString(bus,place),
	    "Previously configured as board", logical);
	logDouble();
}

void logUnexpected( int bdid, int bus, int place)
{
	logOldConfiguration();
	sprintf(logLine,
	    "EQNX: %s %s NOT EXPECTED %s.\n      This board is being ignored!\n",
	    "Board type", bdstr(bdid,bus), placeString(bus,place));
	logDouble();
}


void logAdded( int bdid, int bus, int place, int logical)
{
	logOldConfiguration();
	sprintf(logLine,
	    "EQNX: %s %s found %s. Configured as board %d.\n      This is a new board.\n",
	    "Board type", bdstr(bdid,bus), placeString(bus,place), logical);
	logDouble();
}

void logDouble(void)
{
	if (logFile)
		fprintf(logFile,logLine);
	printf("\r");
	printf(logLine);
}

void logTime(void)
{
	time_t value = time(0); 
	char string[idstring];
	strcpy(string, ctime(&value));
	sprintf(logLine,"\n\nEQNX: Logged at %s", string);
	logDouble();
}

void issue_ioctl(void)
{
	int fi, rv;

	fi = open( drvpath, O_RDONLY );
	if ( fi <= 0 ) {
		sprintf(logLine,"EQNX: mkn--open of %s failed\n", drvpath);
		logDouble();
		exit(1);
	}

	rv = ioctl( fi, EQNISTCFG, &reported );

	if ( rv != 0 ) {
		sprintf(logLine,"EQNX: mkn--ioctl rv==%d(0x%x), errno==%d(0x%x)\n", rv,rv, errno,errno);
		logDouble();
		exit(2);
	}
}

void retrieve_config( FILE* file)
{
	char line[256];
	bdmap.number=0;
	while (fgets(line, sizeof(line), file) != NULL){
		int logical; 
		int physical; 
		int place; 
		int bdid; 
		int bdrev; 
		int bdmaj; 
		int bus;
		char slot[5]; 
		char ioaddr[5]; 
		sscanf(line, "%d %d %d %s %s %x %d %d", &logical, &physical, &bus, slot, ioaddr, &bdid, &bdrev, &bdmaj);
		if (!strcmp(slot,"xxx"))
			sscanf(ioaddr, "%x", &place);
		else
			sscanf(slot, "%d", &place);
		if ((logical-1) == bdmap.number){
			int i = bdmap.number;
			bdmap.number++;
			bdmap._d[i].physical = physical;
			bdmap._d[i].bus_type = bus;
			bdmap._d[i].place = place;
			bdmap._d[i].bdid = bdid;
			bdmap._d[i].bdrev = bdrev;
			bdmap._d[i].bdmaj = bdmaj;
			bdmap._d[i].used = false;
		}
		else{
			printf("EQNX: -- configuration out of order -- \n");
			exit(1);
		}
	}
}

void obtain_config( char* filename)
{
	FILE *in;
	if ((in = fopen(filename, "r")) != NULL)
		retrieve_config(in);
	else
		bdmap.number = 0;
}

void store_config( FILE *file, struct _bdmap *bdmap)
{
	int i;
	for (i=0; i<bdmap->number; i++)
	{
		int place; 
		int bus; 
		int rev;
		int maj;
		char slot[5]; 
		char ioaddr[5]; 
		char revs[4];

		rev = bdmap->_d[i].bdrev;
		maj = bdmap->_d[i].bdmaj;
		revs[0]=0;
		if (rev){
			revs[0]='/';
			sprintf(revs+1, "%d", rev);
		}
		strcpy(slot,"xxx"); 
		strcpy(ioaddr,"xxx");
		place = bdmap->_d[i].place;
		if ((bus = bdmap->_d[i].bus_type) != BOARDISISA)
			sprintf(slot, "%d", place);
		else
			sprintf(ioaddr, "%x", place);
		fprintf(file, "%d\t%d\t%d\t%s\t%s\t%02X\t%d\t%d\t%s%s\n", i+1,
		    bdmap->_d[i].physical, bus,
		    slot, ioaddr, bdmap->_d[i].bdid, rev,
		    maj,bdstr(bdmap->_d[i].bdid,bus),revs);
	}
}
void recreate_device( char *name, dev_t dev_id)
{
	struct stat buffer;
	int lost = stat(name, &buffer);

	if (!lost && (buffer.st_rdev == dev_id)) {
		;
	} else{
		if (!lost){
			unlink(name);
		}
		if (mknod(name, modebits, dev_id))
			printf("EQNX: mknod posted errno==%d for dev==%X (name==%s)\n", errno, (unsigned int) dev_id, name);
	}
}

void recreate_directory(char *name)
{
	struct stat buffer;
	int lost = stat(name, &buffer);
	if (lost)
		mkdir(name, 0777);
}

int main (void)
{
	char lfn[idstring]; 
	char dbn[idstring]; 
	char scf[idstring];
	char pline[80], diag_str[4];
	FILE *d_scan;

	if ((d_scan = fopen("/proc/devices","r")) == NULL)
		printf("ssmkn:Error opening /proc/devices\n");
	while (fgets(pline, sizeof(pline), d_scan) != NULL)
		if (strstr(pline, "eqnxdiag")) {
			strncpy(diag_str, pline, 4);
			diag_num = atoi(diag_str);
			break;
		}
	fclose(d_scan);
	if(!diag_num) {
		printf("ssmkn: Driver not configured.\n");
		return(-1);
	}
	recreate_device(drvpath, makedev(diag_num, 0));
	/* flesh out names and directory structure for files used */
	recreate_directory(basedir);
	strcpy(lfn, basedir);
	strcat(lfn, slash);
	strcat(lfn, logFileName);
	strcpy(dbn, basedir);
	strcat(dbn, slash);
	strcat(dbn, dataBaseName);
	strcpy(scf, basedir);
	strcat(scf, slash);
	strcat(scf, staticCfgName);
	/* open log file */
	logFile = fopen(lfn, "a");
	/* get previous configuration */
	obtain_config(dbn);
	/* get current configuration */
	issue_ioctl();
	/* process previous and current configurations */
	logTime();
	printf("EQNX: running ssmkn\n");
	staticcfg = found_the_file(scf);
	ist_process_config();
	if (!staticcfg)
	{
		FILE *out = fopen(dbn, "w");
		store_config(out, &bdmap);
		fclose(out);
	}
	printf(
	    "\rEQNX: ssmkn complete                                                           \n");

	return(0);
}
void remove_devices( int logical, int bdid, int physical)
{
	char name[256];
	int device, minor_start;
	char unit[3];

	sprintf(unit,"%d", logical + 1);
	printf("remove_devices(%d,%02X,%d)\n",logical,bdid,physical);

	sprintf(name, "%s%s",tty_base,unit);
	minor_start = (physical % 2) * DEVPERBOARD;
	for (device=minor_start; device<(minor_start + DEVPERBOARD); device++)
	{
		char devname[15];
		volatile int mux = (device - minor_start) / DEVPERMIM;
		volatile int jack = (device - minor_start) % DEVPERMIM;

		sprintf(devname,"%s%c%d", name, mux + 'a',jack + 1);
		unlink(devname);
	}

}
