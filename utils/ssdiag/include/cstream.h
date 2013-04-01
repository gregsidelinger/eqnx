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

/* --------------CSTREAM package header package ------------------ */
#define SENOMEM 1001
#define SEINVAL 1002
#define BUFSIZE 8192


typedef struct	{
	struct termios sterm; /* save termio struct */
	struct termios cterm; /* clobber termio */
	unsigned int xm; /* transfer xmit count*/
	unsigned int rc; /* transfer recieve count*/
	unsigned int err; /* error count*/
	int fd;		/* file descripter */
	char dir;	/* direction: r or w */
	int total;	/* total chars in buf */
	int next;	/* next char in buf */
	unsigned char patt;
	int pnum;	/* port number */
	unsigned char buf[BUFSIZE];	/* buffer */
	char fname[35];
	int highbaud;
	int loopback;
}CSTREAM;
typedef struct {
	int fd[2];
}CPIPE;

CSTREAM *Sopen(char *path, char *dir); /* -- open stream -- */
int Sgets();
int Sgetc();
int  Sclose(CSTREAM *z);	/* -- close a stream -- */
int Sputc( CSTREAM *z, unsigned char c);

