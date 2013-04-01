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

/*#include <linux/tty.h>*/
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "twindow.h"
#include "cstream.h"
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

extern int baud;
extern int flow_ctrl;
extern int errno;
extern chtype SHADATT;
extern int Internal_loop;
int Vertical_sel;
int helping = 0;
static int lastvsel[] = {1,1,1,1,1,1,1};
static int insert_mode = 0;
static int hp = 0;
static int ch = 0;
static int hx = 19;
static int hy = 7;
FILE *helpfp = NULL;
char helpname[64];
static void getlineX();
void help();

static void light();
static int getvmn( SMENU *mn, int *hsel, int del);
static void vlight();
static char first_letter();
static void sclight();
static int read_field();
static void movbytes();
static int writebuf(CSTREAM *z);

int spaces( char *);
int port_set(CSTREAM *);
int wputacs( SWIN *, unsigned int);
static void disp_field(SWIN *, char *, char *);
static void addfield(SWIN *, struct field *);
static void data_value(SWIN *, struct field *);
static void right_justify(char *);
static void right_justify_zero_fill(char *);
static int endstroke(int);
static void status_line(char *);

static int helpkey = KEY_F1;

#define MAXHELPS 100
static struct helps {
	char hname [9];
	int h, w;
	int hptr;
} hps [MAXHELPS+1];

/* ---- window structure linked list head & tail ---- */
SWIN *listhead = NULL;
SWIN *listtail = NULL;

long ftell();
char *fgets();

char  *AreYouSure[] = {
	"  DO NOT eqnx_exit  ",
	"   eqnx_exit NOW  ",
	0
};
/* ------------- display & process a menu ----------- */
void executive( SMENU *mn)
{
	int hsel=1, qsel, vsel, frtn = FALSE;
	int i = 0;
	SWIN *t1;
	SMENU vmen;
	vmen.func=NULL;
	vmen.mselcs=AreYouSure;
	
	curs_set(0); 

	qsel = 0;
	standout();		
	move(1,0);
	printw("                                                        \
		        ");
	move(1,0);
	printw("     "); 
	while ((mn+i)->mname){
	 	printw(" %-14.14s ", (mn+i++)->mname); 
		qsel++;
	}
	standend();
	t1 = establish_window(0, 2, 21, 80, 0,0, ""); 
	status_line("");

	light(mn,hsel,OFF);
	light(mn, hsel, ON);
	wrefresh(stdscr);
	while(TRUE){ 
		while (!frtn && ((vsel = getvmn(mn, &hsel, OFF)) != 0))	{
			curs_set(1);
			frtn = (*(mn+hsel-1)->func [vsel-1])();
			light(mn, hsel, ON);
			curs_set(0); 
		}
		/* beep(); causes glitch on ncr console */
		/* this code prompts a window to confirm eqnx_exit */
		/* if(vscroll("eqnx_exiting", &vmen, 1, 33, 7) == 2)
			break;  */ 

		if(frtn == 1) 
			break; 
		/* 
		this code pops down quit menu when ESC hit at top
			level menu */
		/*	else {
			hsel = qsel;			
			light(mn, hsel, ON);
		} */
		vsel = lastvsel[hsel-1]; 
	}	
	curs_set(1); 
#ifdef	NOTHING
	delete_window(t2); 
#endif
	delete_window(t1); 
}


/* ---------pop down a vertical menu --------- */
static int getvmn( SMENU *mn, int *hsel, int del)
{
	int ht, wd, vx, sel = 0, vsel = 0, vs;
	static int last_sel = 0;
	char tpstr[50];
	char **selcs;
	static SWIN *twin = NULL;
	
	while (sel != ESC && sel != '\r')	{
		dimension((mn+*hsel-1)->mselcs, &ht, &wd);
		vx = 6+(*hsel-1)*16;
		if(twin == NULL){
			if(del)
				twin=establish_window(vx,2,ht+2,wd+2,0,0, "");
			else if(last_sel == 0 || last_sel != '\r')
				twin=establish_window(vx,2,ht+2, wd+2,0,0,"");
		}	
		selcs = (mn+*hsel-1)->mselcs;
		for (vsel = 1; *selcs; vsel++)	{
			wcursor(twin, 1, vsel-1);
			sprintf(tpstr, "%s", *selcs++);
			Wprintf(twin, tpstr);
		}
		/*set_help("menu    ", 19, 7);*/
		sel = 0;
		vsel = lastvsel[*hsel-1];
	   while (sel != ESC && sel != '\r' && sel != KEY_RIGHT && sel != KEY_LEFT&& sel != BS) {
		   curs_set(0); 
				
		   vlight(twin, mn, *hsel, vsel, ON); 
	       sel=enabled(mn, *hsel, vsel) ? get_char():(sel == KEY_UP ? KEY_UP : KEY_DOWN);
		   selcs = (mn+*hsel-1)->mselcs;
		   for (vs = 1; *selcs; vs++, selcs++)	{
		  if (enabled(mn,*hsel,vs) && tolower(first_letter(*selcs)) == tolower(sel)){
			/*vlight(twin, mn, *hsel, vsel, OFF);*/
			vsel = vs;
				last_sel = sel = '\r';
				break;
			}
		   }
			switch (sel)	{
				case KEY_UP:
					vlight(twin, mn, *hsel, vsel, OFF);
					if (vsel > 1)
						--vsel;
					else
						vsel = ht;
					vlight(twin, mn, *hsel, vsel, ON);
					break;
				case DN:
				case KEY_DOWN:
					vlight(twin, mn, *hsel, vsel, OFF);
					if (vsel < ht)
						vsel++;
					else
						vsel = 1;
					vlight(twin, mn, *hsel, vsel, ON);
					break;
				case '\r':
			if ((mn+*hsel-1)->func [vsel-1] == NULL){
#ifndef XENIX
				if (test_option(mn,*hsel,vsel))
					clear_option(mn,*hsel,vsel);
				else
					set_option(mn,*hsel,vsel);
#endif
					vlight(twin, mn, *hsel, vsel, ON);
						sel = 0;
				}
				else
					last_sel = sel;
					/* vlight(twin, mn, *hsel, vsel, OFF);*/
					if(del)
						delete_window(twin);
					break;
				case KEY_RIGHT:
					delete_window(twin);
					twin = NULL;
					light(mn, *hsel, OFF);
					if ((mn+*hsel)->mname)
						(*hsel)++;
					else
						*hsel = 1;
					light(mn, *hsel, ON);
					last_sel = 0;
					break;
				case BS:
				case KEY_LEFT:
					delete_window(twin);
					twin = NULL;
					light(mn, *hsel, OFF);
					if (*hsel == 1)
						while ((mn+*hsel)->mname)
							(*hsel)++;
					else
						--(*hsel);
					light(mn, *hsel, ON);
					last_sel = 0;
					break;
				case ' ':
				case ESC:
					/* light(mn, *hsel, OFF); 
					delete_window(twin);  */
					delete_window(twin);
					twin = NULL;
					light(mn, *hsel, OFF);
					while ((mn+*hsel)->mname)
						(*hsel)++;
					light(mn, *hsel, ON);
					last_sel = 0;
					/*pppppp */
					sel = ESC;
					break;
				default:
					beep();	
					break;
			}
	    }
	}
	curs_set(1); 
	last_sel = sel;
	return sel == ESC ? 0 : vsel; 
}


SCRLL *start = NULL;
SCRLL *last = NULL;


/* --------- vertical scroll selector --------- */
int vscroll(title, mselec, def, x, y)
char *title;
SMENU *mselec;
int def, x, y;
{
	int nsel, ht, wd, tp, sel = 0, vsel, vs, lsc;
	int last_sel = 0, frtn;
	int scrol= 0;
	char **selcs, **tptr, **selstr;
	SWIN *twin;

	selcs = selstr = mselec->mselcs;
	nsel=count_strings(selstr);
	dimension(selcs, &ht, &wd);
	wd = max( strlen(title) + 2 , wd );
	twin=establish_window(x,y,ht+2,wd+1, 0,0, title);

	if(def>nsel)
		def=ht;
	vsel=def;
	if(nsel > ht ) {
		for(tp=0; tp < def - 1 ; tp++) 
			scrol++;
		vsel=1;
		if(def> nsel - ht) { 
			  	scrol = nsel -ht;
				vsel = def - scrol;
		 } 
	}
	lsc=scrol+1;	
 	while (sel != ESC && sel != '\r') {
		curs_set(0); 
		tptr = mselec->mselcs;
		if(lsc != scrol){
			lsc=scrol;
			for (tp=1; tp < ht+1; tp++){
				sclight(twin, tptr + scrol , tp, OFF);
				}
			if(scrol > 0){
				/* put ACS_UARROW */
				put_arrow(twin, ACS_UARROW);
	
			} else put_arrow(twin, DEL_UARROW);
			if(scrol + ht + 1 <= nsel){
				/* put ASC_DARROW */
				put_arrow(twin, ACS_DARROW);
			} else put_arrow(twin, DEL_DARROW);
		}
		sclight(twin, tptr + scrol, vsel, ON);
	       	sel= get_char();
		if(nsel <= ht)
		selcs = tptr + scrol;
		for (vs = 1; *selcs; vs++, selcs++)	{
		  if (tolower(first_letter(*selcs)) == tolower(sel)){
			sclight(twin,tptr + scrol, vsel, OFF);
			vsel = vs;
			sclight(twin,tptr + scrol, vsel, ON);
				last_sel = sel = '\r';
				break;
			}
		   } 
			switch (sel)	{
				case KEY_NPAGE:
				sclight(twin,tptr + scrol, vsel, OFF);
				if(nsel > ht ) {
					if(vsel+scrol >= nsel - ht )
						vsel = ht;
					if(nsel -scrol -ht> ht) { 
						scrol = scrol +ht;
						if(scrol >= nsel - ht)
					 		scrol = nsel - ht;
					} else {
					     scrol = nsel - ht;
					}
						  
				} else {
					vsel = ht;
				}
				sclight(twin,tptr + scrol , vsel, ON);
				break;
				case KEY_PPAGE:
				sclight(twin, tptr + scrol,vsel, OFF);
				if(nsel > ht ) {
					if(vsel+scrol <= ht )
					    vsel = 1;
					if(scrol> ht) { 
					    scrol = scrol -ht;
					} else {
					     scrol = 0;
					}
				} else {
					vsel = 1;
				}
				sclight(twin, tptr + scrol,vsel, ON);
				break;
				case KEY_UP:
					sclight(twin, tptr + scrol,vsel, OFF);
					if(nsel > ht){
					     if (vsel > 1){
						vsel--;
					      } else {
						  if(scrol > 0)
						  	scrol--;
						  else {
						  	vsel=ht;
						  	scrol = nsel -ht;
						  }
					      }
					} else {
						if(vsel > 1)
							vsel--;
						else 
							vsel = ht;
					}
					sclight(twin, tptr + scrol,vsel, ON);
					break;
				case DN:
				case KEY_DOWN:
					sclight(twin, tptr + scrol,vsel, OFF);
					if(nsel > ht ) {
					     if(vsel < ht) { 
						vsel++;
					      } else {
						  if(vsel + scrol < nsel)	
						  	scrol++;
						   else { scrol = 0;
							  vsel = 1;
						   }
					       } 	
					} else {
						if(vsel < ht)
							vsel++;
						else {
							scrol = 0;
							vsel = 1;
						}
					}
					sclight(twin,tptr + scrol, vsel, ON);
					break;
				case '\r':
				last_sel = sel;
				Vertical_sel=vsel+scrol;
				if(mselec->func != NULL)
					if(mselec->func[vsel+scrol-1] != NULL){
						curs_set(1); 
					    frtn=(mselec->func[vsel+scrol-1])();
					    if(frtn){
						delete_window(twin);
						return(vsel + scrol);
					    }	
					    sel = 0;
					    break;
					}	
				sclight(twin,tptr + scrol, vsel, OFF);
				delete_window(twin);
				break;
				case ESC:
					delete_window(twin);
					break;
				default:
					beep();	
					break;
			}
		if(sel == ESC) 
			break;
	}
	
	curs_set(1); 
	return(vsel + scrol);
}

int count_strings( char **strs)
{
   	int cnt = 0;
   	char **tst=strs;	

	while(*tst){
		(void) *tst++;
		cnt++;
	}
	return(cnt);
}

char first_letter( char *s)
{

	do{
		if(*s)
		   if(*s != ' ')
			return(*s);
	}while(*(s++));
	return(' ');
}


/* ---------- compute a menu's height & width --------- */
void dimension(sl, ht, wd)
char *sl[];
int *ht;
int *wd;
{
	*ht = *wd = 0;

	while (sl && sl [*ht])	{
		*wd = max(*wd, (unsigned) strlen(sl [*ht]) + 2);
		(*ht)++;
	}
	*ht=min(*ht, MAXSCR);

}

/* --------- accent a horizontal menu selection ---------- */
static void light( SMENU *mn, int hsel, int onoff)
{
 	if(onoff)
	/*	attrset(A_STANDOUT | GLATT);*/
		standend();
	else
		standout();
		/*attrset(~(A_STANDOUT | GLATT));*/
 	move(1, (hsel-1)*16+6);
	printw((mn+hsel-1)->mname);
		standend();
		/*attrset(~(A_STANDOUT | GLATT));*/

}

/* --------- accent a vertical menu selection ---------- */
static void vlight( SWIN *wnd, SMENU *mn, int hsel, int vsel, int onoff)
{
	char tmp[55];
	if(onoff)
		reverse_video(wnd);
		
 	wcursor(wnd, 0, vsel-1);
	
	sprintf(tmp, " %s           ", *((mn+hsel-1)->mselcs+vsel-1)); 
	set_help(*((mn+hsel-1)->mselcs+vsel-1), 19, 7);
	Wprintf(wnd, tmp); 
	lastvsel[hsel-1] = vsel;
	if(onoff)
		reverse_video(wnd);

}


/* --------- accent a vertical menu selection ---------- */
static void sclight( SWIN *wnd, char **mn, int vsel, int onoff)
{
	char tmp[55];
	if(onoff)
		reverse_video(wnd);
 	wcursor(wnd, 0, vsel-1);
	sprintf(tmp, " %s       ", *(mn+vsel-1));
	set_help(*(mn+vsel-1), 19, 7);
	Wprintf(wnd, tmp); 
	if(onoff)
		reverse_video(wnd);

}
void reverse_video( SWIN *wnd)
{
	chtype t;

	if(verify_wnd(wnd)) {
		t=WATT;
		WATT=HATT;
		HATT=t;
	}
}
void fatal(char *msg) /* -- print error message and terminate -- */
{
	fprintf(stderr, "ERROR: %s\n",msg);
	eqnx_exit(1);
}

void syserr(char *msg) /* -- print system call error message and terminate -- */
{
	fprintf(stderr, "ERROR: %s (%d", msg, errno);
	if (errno > 0)
		fprintf(stderr, "; %s)\n", strerror(errno));
	else
		fprintf(stderr, ")\n");
	eqnx_exit(1);
}

/* -------- initialize a template --------- */
void init_template( SWIN *wnd)
{
 	struct field *fld;
 	struct field *fl;

	fld = FHEAD;
	while (fld)	{
		fl = fld->fnxt;
		free(fld);
		fld = fl;
	}
	FHEAD = FTAIL = NULL;
}
/* ------ establish a field in a template ------- */
struct field *establish_field( SWIN *wnd, int cl, int rw, char *msk, char *bf, int ty)
{
	struct field *fld;

	if ( (fld = (struct field *) malloc(sizeof(field))) == NULL){
		printf("Error : malloc : funct; establish_field\n");
		eqnx_exit(1);
	}
	fld->fmask = msk;
	fld->frow = rw;
	fld->fcol = cl;
	fld->fbuff = bf;
	fld->ftype = ty;
	fld->fprot = 0;
	fld->fnxt = fld->fprv = NULL;
	fld->fvalid = NULL;
	fld->fhelp = NULL;
	fld->fhwin = NULL;
	fld->flx = fld->fly = 0;
	addfield(wnd, fld);
	return fld;
}

/* ----- add a field to the end of the list ------ */
static void addfield( SWIN *wnd, struct field *fld)
{
	if (FTAIL)	{
		fld->fprv = FTAIL;
		FTAIL->fnxt = fld;
	}
	FTAIL = fld;
	if (!FHEAD)
		FHEAD = fld;
}
void field_help( struct field *fld, void (*func)())
{
	fld->fhelp = func;
}
void field_validate( struct field *fld, int (*func)())
{
	fld->fvalid = func;
}

/* -------- display a data field ------ */
static void disp_field( SWIN *wnd, char *bf, char *msk)
{
	register char *savemsk = msk;
	char *savebf = bf;
	char savebuf[80];
	
	strcpy(savebuf, bf); /* save and clear bf to remove residuals*/
	while(*msk){
		if(*msk == FIELDCHAR)
			*bf++ = ' ';	
		msk++;
	}
	msk = savemsk;
	bf = savebf;	
	
	strcpy(bf, savebuf); /* restore it */
	while(*msk)	{
	  wputchar(wnd, (unsigned char)(*msk != FIELDCHAR ? *msk : *bf++)); 
		msk++;
	}
 	wrefresh(stdscr);
}

/* ------- display the data value in a field ------ */
static void data_value( SWIN *wnd, struct field *fld)
{
	wcursor(wnd, fld->fcol, fld->frow);
	disp_field(wnd, fld->fbuff, fld->fmask);
}

/* ------ display all the fields in a window ------- */
void field_tally(SWIN *wnd)
{
	register struct field *fld;

	fld = FHEAD;
	while (fld != NULL)	{
		data_value(wnd, fld);
		fld = fld->fnxt;
	}
}

/* ----- set a field's help window ------- */
void field_window( struct field *fld, char *hwin, int x, int y)
{
	fld->fhwin=hwin;
	fld->flx = x;
	fld->fly = y;
}
/*page*/
/* ------- clear a template to all blanks ------ */
void clear_template( SWIN *wnd)
{
	struct field *fld;
	char *bf, *msk;

	fld = FHEAD;
	while (fld != NULL)	{
		bf = fld->fbuff;
		msk = fld->fmask;
		while (*msk)	{
			if (*msk == FIELDCHAR)
				*bf++ = ' ';
			msk++;
		}
		fld = fld->fnxt;
	}
	field_tally(wnd);
}

/* ---------- set insert/exchange cursor shape ----------- */
/*page*/
/* ------- read a field from the keyboard ------------- */
static int read_field( SWIN *wnd, struct field *fld)
{
	char *mask = fld->fmask, *buff = fld->fbuff;
	int  done = FALSE, c, column;

	column = fld->fcol;
	while (*mask != FIELDCHAR)	{
		column++;
		mask++;
	}
	buff[strlen(mask)] = 0x00;
	while (TRUE)	{
		wmove(stdscr, fld->frow + ROW +1, column + COL + 1); 
		wrefresh(stdscr);
		c = get_char();
		/* force a call to helper routine */
		if( fld->ftype == 'H'){
			if (c == '\r')
				return(KEY_F1);
		    if(endstroke(c))
				return(c);
			else
				continue;	
		}
		/* -------------------------------*/
		if (fld->ftype == 'A')
			c = toupper(c);
		clear_message();
		switch (c)	{
			case BS:
			case KEY_BACKSPACE:
			case KEY_LEFT:
				if (buff == fld->fbuff)	{
					done = c = BS; /* ----fix---*/
					break;
				}
				--buff;
				do	{
					--mask;
					--column;
				} while (*mask != FIELDCHAR);
				/*if (c == BS) */
					break;
			case KEY_BREAK:
				strncpy(buff+1, buff, strlen(buff)); /*fix*/
				*(buff+strlen(buff)) = ' ';
				wcursor(wnd, column, fld->frow);
				disp_field(wnd, buff, mask);
				break; 
			case KEY_RIGHT:
			case RARROW:
				do	{
					column++;
					mask++;
				} while (*mask && *mask != FIELDCHAR);
				buff++;
				break;
		/*	case INS:
				insert_mode ^= TRUE;
				insert_status();
				break; */
			case '.':
				if (fld->ftype == 'C')	{
					if (*mask++ && *buff == ' ')	{
						*buff++ = '0';
						if (*mask++ && *buff == ' ')
							*buff++ = '0';
					}
					right_justify(fld->fbuff);
					wcursor(wnd, fld->fcol, fld->frow);
					disp_field(wnd, fld->fbuff, fld->fmask);
					column = fld->fcol+strlen(fld->fmask)-2;
					mask = fld->fmask+strlen(fld->fmask)-2;
					buff = fld->fbuff+strlen(fld->fbuff)-2;
					break;
				}
			default:
				if (endstroke(c))	{
					done = TRUE;
					break;
				}
				if (toupper(fld->ftype)!='A'&&!isdigit(c)){
					error_message("Numbers only !");
					break;
				}
				if (insert_mode)	{
					strncpy(buff, buff+1, strlen(buff)-1);
					disp_field(wnd, buff, mask);
					wcursor(wnd, column, fld->frow);
				}
				*buff++ = c;

				mvwaddch(stdscr,SCROLL+1,column+COL+1,c | WATT);
				WCURS++;
				wrefresh(stdscr);

				do	{
					column++;
					mask++;
				} while (*mask && *mask != FIELDCHAR);
                if(*buff == ' ')
                   *buff = '\0';
				if (!*mask)
					c = KEY_RIGHT;
				break;
		}
		if (!*mask) 
			done = TRUE;
		if(done)	
			break;
	}
	if (c != ESC && toupper(fld->ftype) != 'A')	{
		if (fld->ftype == 'C')	{
			if (*mask++ && *buff == ' ')	{
				*buff++ = '0';
				if (*mask++ && *buff == ' ')
					*buff++ = '0';
			}
		}
		if (fld->ftype == 'Z' || fld->ftype == 'D')
			right_justify_zero_fill(fld->fbuff);
		else
			right_justify(fld->fbuff);
		wcursor(wnd, fld->fcol, fld->frow);
		disp_field(wnd, fld->fbuff, fld->fmask);
	}
	return c;
}
/* ---------- test c for an ending keystroke ----------- */
static int endstroke( int c)
{
	switch (c)	{
		case 15:
		case KEY_F2:
		case KEY_F3:
		case KEY_F4:
		case KEY_F5:
		case KEY_F6:
		case KEY_F7:
		case KEY_F8:
		case KEY_F9:
		case KEY_F10:
		case KEY_UP:
		case KEY_DOWN:
		case '\r':
		case '\n':
		case '\t':
		case ESC:
			return TRUE;
		default:
			return FALSE;
	}
}
/* ------- right justify, space fill -------- */
static void right_justify(char *s)
{
	int len;

	len = strlen(s);
	while ((*s == ' ' || *s == '0') && len)	{
		len--;
		*s++ = ' ';
	}
	if (len)
		while (*(s+(len-1)) == ' '){
			movbytes(s +1 , s, len-1);
			*s = ' ';
		}
}

/* ---------- right justify, zero fill --------------- */
static void right_justify_zero_fill(char *s)
{
	int len;

	if (spaces(s))
		return;
	len = strlen(s);
	while (*(s + len - 1) == ' ')	{
		movbytes(s +1 , s, len-1);
		*s = '0';
	}
}
static void movbytes( char *des, char *src, int cnt)
{

	char *buff;
	register int f;

        buff = (char *)malloc(cnt+5);
	
	for(f=0; f < cnt; f++) {
		*(buff + f) = *(src + f);
	}

	for(f=0; f < cnt; f++) {
		*(des + f) = *(buff + f);
	}
	
}

	
/* ----------- test for spaces -------- */
int spaces( char *c)
{
	while (*c == ' ')
		c++;
	return !*c;
}
/* ----- Process data entry for a screen template. ---- */
int data_entry( SWIN *wnd)
{
	struct field *fld;
	int eqnx_exitcode=0, isvalid, done=FALSE, oldhelpkey=helpkey;
	field_tally(wnd);
	fld = FHEAD;
	/* ---- collect data from keyboard into screen ---- */
	while (fld != NULL && done == FALSE)	{
		set_help(fld->fhwin, fld->flx, fld->fly); 
		/* allow for field help and verification */
		/* helpkey = (fld->fhelp) ? 0 : oldhelpkey; */
		if( fld->ftype == 'H')
			 curs_set(0); 
		else{
		 	curs_set(1);
		}
		wrefresh(stdscr);

		if (fld->fprot == FALSE){
			reverse_video(wnd);
			data_value(wnd, fld);
			eqnx_exitcode = read_field(wnd, fld);
			reverse_video(wnd);
			isvalid = (eqnx_exitcode != ESC && fld->fvalid) ?
					(*(fld->fvalid))(fld->fbuff) : OK;
		}
		else	{
			eqnx_exitcode = KEY_RIGHT;
			isvalid = OK;
		}
		if (isvalid == OK)	{
			data_value(wnd, fld);
			switch (eqnx_exitcode)	{	/* passed edit */
				case KEY_F1: if (fld->fhelp)	{
								(*(fld->fhelp))(fld->fbuff);
								data_value(wnd, fld);
								}
							break;
				case 15:
				case DOWN:
				case KEY_ENTER:
				case KEY_DOWN:
				case '\r':
				case '\t':
				case KEY_RIGHT:	fld = fld->fnxt;
						  if (fld == NULL) {
						    if(SPASS)
							done=TRUE;
						    else	
							fld = FHEAD;
						}
						break;
				case UP:
				case KEY_UP:
				case BS:
				case KEY_BACKSPACE:
				case KEY_LEFT:	fld = fld->fprv;
							if (fld == NULL)
								fld = FTAIL;
							break;
				default:	done = endstroke(eqnx_exitcode);
							break;
			}
		}
	}
	helpkey = oldhelpkey;
	curs_set(0); 
	wrefresh(stdscr);
	return (eqnx_exitcode);
}
void field_pass_mode( SWIN *wnd, int ps)
{
	wnd->single_pass=ps;
	return;
}
/* --------- display a window prompt -------- */
void wprompt( SWIN *wnd, int x,int y, char *s)
{
	wcursor(wnd, x, y);
	Wprintf(wnd, s);
}

/* ------------- window-oriented printf ---------------- */
void Wprintf( SWIN *wnd, char *ln)
{
	if (verify_wnd(wnd))		{
		while (*ln)
			wputchar(wnd, *ln++);
	}
	
	wrefresh(stdscr);
}
/* ------------ write a character to the window ---------- */
void wputchar( SWIN *wnd, unsigned char c)
{

	if (!verify_wnd(wnd))
		return;

	switch (c)	{
		case '\n':
			if (SCROLL == HEIGHT-3)
				; /* wscroll(wnd);*/
			else {
				SCROLL++;
				wmove(stdscr, SCROLL + COL +1, ROW +1);
			}
			WCURS=ROW + 1;
			break;
		case '\t':
			do mvwaddch(stdscr,SCROLL+1,(WCURS++)+3,' ' | WATT);
			     while ((WCURS%TABS) && (WCURS+1) < COL + WIDTH-2);
			break;
		default:
			if(c == '\0')
				c=' ';
			if ((WCURS+1) < COL + WIDTH-1)	{
			 	mvwaddch(stdscr,SCROLL+1,WCURS+1,c | WATT);
				WCURS++;
			}
			break;
	}
}

/* ------------ write an extended character to the window ---------- */
int wputacs( SWIN *wnd, unsigned int c)
{

	if (!verify_wnd(wnd))
		return(1);

	if ((WCURS+1) < COL + WIDTH-1)	{
	 	mvwaddch(stdscr,SCROLL+1,WCURS+1,c | WATT);
		WCURS++;
	}
	return (0);
}
int put_arrow( SWIN *wnd, unsigned int c)
{

	if(c == ACS_UARROW ) 
    	mvwaddch( stdscr , ROW + 1, COL + WIDTH-1,ACS_UARROW | WATT );
	else if(c ==  DEL_UARROW )
    	mvwaddch( stdscr , ROW + 1, COL + WIDTH-1,ACS_VLINE | WATT );
	else if( c == ACS_DARROW )
    	mvwaddch(stdscr,ROW + HEIGHT - 2,COL + WIDTH-1,ACS_DARROW | WATT );
	else if(c == DEL_DARROW )
    	mvwaddch(stdscr,ROW + HEIGHT - 2,COL + WIDTH-1,ACS_VLINE | WATT );
	return(0);

}
/* --- write char after moveing cursor-------------------------- */
void wmputchar( SWIN *wnd, int x, int y, unsigned char c)
{
	if (!verify_wnd(wnd))
		return;
	if(x < (COL+WIDTH-1) && y < HEIGHT - (BTYPE ? 1 : 2)){
		WCURS = COL + x;
		SCROLL = ROW + y; 
/* DEBUG 	wmove(stdscr, ROW+y, COL+x); */
	}


	switch (c)	{
		case '\n':
			if (SCROLL == HEIGHT-3)
				; /* wscroll(wnd);*/
			else {
				SCROLL++;
				wmove(stdscr, SCROLL + COL +1, ROW +1);
			}
			WCURS=ROW + 1;
			break;
		case '\t':
			do mvwaddch(stdscr,SCROLL+1,(WCURS++)+3,' ' | WATT);
			     while ((WCURS%TABS) && (WCURS+1) < COL + WIDTH-2);
			break;
		default:
			if(c == '\0')
				c=' ';
			if ((WCURS+1) < COL + WIDTH-1)	{
			 	mvwaddch(stdscr,SCROLL+1,WCURS+1,c | WATT);
				WCURS++;
			}
			break;
	}
}


/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */

/* ------- scroll a window's contents up or down --------- */
void wscroll( SWIN *wnd)
{
     int x1 = COL;
     int y1 = ROW; 
     unsigned int c;

     if (verify_wnd(wnd))
         for(y1 = ROW + 1; (y1 < (ROW + HEIGHT-2)); y1++) 
	    for(x1 = COL + 1; (x1 < (COL + WIDTH-1)); x1++) {
                c = (unsigned int) mvwinch(stdscr, y1+1, x1);
        	mvwaddch(stdscr,y1, x1,c | WATT);
		}
	/* -------- Clear last line ------- */
	    for(x1 = COL + 1; (x1 < (COL + WIDTH-1)); x1++) 
        	mvwaddch(stdscr,y1, x1,' ' | WATT);

	wrefresh(stdscr);
}


    
/* ----------- establish a new window -------------- */
SWIN *establish_window( int x, int y, int h, int w, int t,
	int rev,char *title)
{
        SWIN *wnd;
        
	wnd = (SWIN *) malloc(sizeof (SWIN));
        if(!wnd) {
                printf("Failure allocating swindow in funct. establish_win");
                eqnx_exit(1);
                }
	/* ------- adjust for out-of bounds parameters ------- */
	WTITLE = title;
	HEIGHT = min(h, SCRNHT);
	WIDTH = min(w, SCRNWIDTH);
	COL = max(0, min(x, SCRNWIDTH-WIDTH));
	ROW = max(0, min(y, SCRNHT-HEIGHT));
	WCURS = 0;
	SCROLL = 0;
	SELECT = 1;
	BTYPE = t;
	VISIBLE = HIDDEN = 0;
	PREV = NEXT = NULL;
	FHEAD = FTAIL = NULL;
        SAV = NULL;

#ifndef	XENIX
	if(has_colors() == TRUE)
		WATT = COLOR_PAIR(1);
	else {
		 WATT = rev ? A_STANDOUT : A_NORMAL;
	}	
	if(has_colors() == TRUE)
	/* 	HATT = COLOR_PAIR(3); */
		
		HATT = rev ? A_NORMAL : A_STANDOUT;
	else {	
		HATT = rev ? A_NORMAL : A_STANDOUT;
	}	
#else
		WATT	 = rev ? A_STANDOUT : A_NORMAL;
		HATT = rev ? A_NORMAL : A_STANDOUT; 
#endif

	REVV = rev;
	wnd->single_pass=0;
	wcursor(wnd, COL, ROW );
        SAV = (chtype *)malloc(sizeof(long) * (WIDTH+1) * (HEIGHT+1));
        if (!SAV){
                printf("Failure allocating image buffer in estab. win");
                eqnx_exit(1);
                }
        display_window(wnd);
	return(wnd);
}

/* -----------------------------------------*/
int get_char(void)
{
	int cch;

	while(1) {
		cch=getch();
		if(cch == helpkey) {
		   if(!helping) {
			helping=1;
			help();
			helping=0;
			continue;
			}
	      }
	 break;

	}
	return(cch);
}

/* ----------- load the HELP! definition file ------------ */
void load_help( char *hn)
{
	void (*helpfunc)();
	char lineh [80];


	if (strcmp(helpname, hn) == 0)
		return;
	helpfunc = help;
	helpkey = KEY_F1;
	hp = 0;
	strcpy(helpname, hn);
	if ((helpfp = fopen(helpname, "r")) == NULL)
		return;
	getlineX(lineh);
	while (1)	{
		if (hp == MAXHELPS)
			break;
		if (strncmp(lineh, "<end>", 5) == 0)
			break;
		if (*lineh != '<')
			continue;
		hps[hp].h = 3;
		hps[hp].w = 18;
		strncpy(hps[hp].hname, lineh+1, 8);
		hps[hp].hptr = ftell(helpfp);
		getlineX(lineh);
		while (*lineh != '<')	{
			hps[hp].h++;
			hps[hp].w = max(hps[hp].w, strlen(lineh)+2);
			getlineX(lineh);
		}
		hp++;
	}
}
/* -------- get a line of text from the help file -------- */
static void getlineX( char *lineh)
{
	if (fgets(lineh, 80, helpfp) == NULL)
		strcpy(lineh, "<end>");
}
/* -------- set the current active help screen ----------- */
void set_help( char *s, int x, int y)
{
	for (ch = 0; ch < hp; ch++)
		if (strncmp(s, hps[ch].hname, min( 8, strlen(s))) == 0)
			break;
	hx = x;
	hy = y;
}
/* ---------- display the current help window ----------- */
void help()
{
	char ln [80];
	int i, xx, yy;
	SWIN *wnd;

	getyx(stdscr, yy, xx);

	if (hp && ch != hp)	{
		/*wnd = establish_window(hx, hy, hps[ch].h + 2, hps[ch].w,0,0," Help ");*/

		wnd = establish_window((SCRNWIDTH - hps[ch].w - 1) / 2 ,
			(SCRNHT - hps[ch].h -1 ) / 2,
			hps[ch].h + 2, hps[ch].w,0,0," Help ");

		fseek(helpfp, hps[ch].hptr, 0);
		for (i = 0; i < hps[ch].h-3; i++)	{
			getlineX(ln);
			wcursor(wnd, 1, i);
			Wprintf(wnd, ln);
		}
		wcursor(wnd, 1, i+1);
		Wprintf(wnd, " ESC to return");

		while (get_char() != ESC)
			beep();
		delete_window(wnd);
	}
	move(yy,xx);
	wrefresh(stdscr);
}


/*-------------------------------------------------------*/

/* ------------ display an established window ------------ */
void display_window( SWIN *wnd)
{
	if (verify_wnd(wnd) && !VISIBLE)	{
		VISIBLE = 1;
		vsave(wnd);
		mclear_window(wnd); 
		wframe(wnd);
		}
}

/* ------------ remove a window ------------------ */
void delete_window( SWIN *wnd)
{
	if (verify_wnd(wnd))		{
		vrstr(wnd);
		init_template(wnd); 
		free(SAV);
		free(wnd);
		wnd=NULL;
	}
}


/* ----------- clear the window area -------------- */
void mclear_window( SWIN *wnd)
{
	register int x1 = COL;
	int y1 = ROW; 

	if (verify_wnd(wnd))
		for (y1++; (y1 < (ROW + HEIGHT-1)); y1++) 
		     for (x1 = COL + 1; (x1 < (COL + WIDTH-1)); x1++) 
                        mvwaddch(stdscr, y1, x1, ' ' | WATT ); 
	wrefresh(stdscr);
}

/* -------- save video memory into the save buffer ---- */
void vsave( SWIN *wnd)
{
	register int x; 
	chtype *bf = SAV;
	int y;
	for (y = ROW; y < ROW + HEIGHT + 1 ; y++)
		for (x = COL; x < COL + WIDTH + 1; x++) 
			*bf++ = mvwinch(stdscr, y, x);
}
/* ----- restore video memory from the save buffer ----- */
void vrstr( SWIN *wnd)
{
	int x;
	chtype *bf = SAV;
	int y;
	
	for (y = ROW; y < ROW + HEIGHT + 1; y++) 
		for (x = COL; x < COL + WIDTH + 1; x++) {
                        mvwaddch(stdscr, y, x, *bf); 
			bf++;
		}
	wrefresh(stdscr);
}
void wframe( SWIN *wnd)
{
	register int x1=0, y1=0;

       	/* ---- Draw top line and title ---- */	
	/* if((mvinch( ROW, COL) & ACS_VLINE ) == ACS_VLINE )
	      mvwaddch( stdscr , ROW, COL, DTEE | WATT);
	else
	*/
	      mvwaddch( stdscr , ROW, COL, ACS_ULCORNER | WATT );
	dtitle(wnd);
	/* if((mvinch( ROW, COL + WIDTH -1) & LINE ) == LINE && ROW < 2)
        	mvwaddch( stdscr , ROW, COL + WIDTH-1, DTEE | WATT);
	else
	*/
        	mvwaddch( stdscr , ROW, COL + WIDTH-1, ACS_URCORNER | WATT );
	/* ---- window sides ---*/
	for (y1 = ROW + 1; y1 < ROW + HEIGHT-1; y1++)  {
		mvwaddch( stdscr , y1,COL,ACS_VLINE | WATT );
    		mvwaddch( stdscr , y1, COL + WIDTH-1,ACS_VLINE | WATT );
		mvwaddch( stdscr, y1, COL + WIDTH,
			mvinch(y1, COL + WIDTH) | SHADATT);
	}

	mvwaddch( stdscr, y1, COL + WIDTH,
	    mvinch(y1 , COL + WIDTH) | SHADATT);
        if(y1 <= 25) /* don't highlight rows beyond row 25 */
        {
	    mvwaddch( stdscr, y1 + 1, COL + WIDTH,
		mvinch(y1 + 1 , COL + WIDTH) | SHADATT);
        }
	/* ---- bottom of frame ---- */
	switch(BTYPE) {
		case 1:
			mvwaddch( stdscr , y1, COL,ACS_VLINE | WATT );
       	   	mvwaddch( stdscr ,y1,COL + WIDTH-1,ACS_VLINE | WATT );
	   		for(x1 = COL +1; x1 < COL + WIDTH-1;x1++) {
				mvwaddch( stdscr,y1,x1,' ' | WATT ); 
                                if(y1 <= 24)
                                {
				     mvwaddch( stdscr, y1 +1, x1,
					mvinch(y1 + 1, x1) | SHADATT);
                                 }
			}
			break;
		case 0:
		case 2:
			mvwaddch( stdscr , y1, COL,ACS_LLCORNER | WATT );
	   		for(x1 = COL +1; x1 < COL + WIDTH-1;x1++) {
				mvwaddch( stdscr,y1,x1,ACS_HLINE | WATT ); 
                                if(y1 <= 24)
                                {
				    mvwaddch( stdscr, y1 +1, x1,
					mvinch(y1 + 1, x1) | SHADATT);
                                }
			}
           		mvwaddch( stdscr ,y1,x1, ACS_LRCORNER | WATT );
			break;
		default:
			break;
		}
                if(y1 <= 24)
                {
		    mvwaddch( stdscr, y1 +1, x1,
			mvinch(y1 + 1, x1) | SHADATT);
                }
        wrefresh( stdscr );
}



/*--------------------------------------------*/
/* ------ Display the window title ------ */
void dtitle( SWIN *wnd)
{
	register int x1 =COL;
	int i, ln;
        char *s = WTITLE;     
   
	x1++;
	if(s) {
	    ln = strlen(s);
            if(ln > WIDTH-2)
	         i = 0;
            else
                 i = ((WIDTH -2 -ln) /2);
            if(i >0)
		 while(i--)  {
			mvwaddch( stdscr , ROW, x1, ACS_HLINE | WATT);
			x1++;
		}
	    while(*s && x1 < COL + WIDTH -1)   {
		if(BTYPE > 0 && *s == ' ')
			mvwaddch( stdscr , ROW, x1, ACS_HLINE | WATT);
		else		
			mvwaddch( stdscr , ROW, x1, *s | WATT);
		x1++; s++;
	    }
	}
       		while(x1 < COL + WIDTH -1)	{
            	mvwaddch( stdscr , ROW, x1, ACS_HLINE | WATT);
	    	x1++;
       	} 
}

void wcursor( SWIN *wnd, int x, int y)
{
   if(verify_wnd(wnd) && x < (COL+WIDTH-1) && y < HEIGHT - (BTYPE ? 1 : 2)){
		WCURS = COL + x;
		SCROLL = ROW + y; 
		wmove(stdscr, ROW+y, COL+x);
	}
}

int verify_wnd( SWIN *wnd)
{
      if(wnd == NULL)
          return(0);
      return(1);
}

SWIN *nwnd;
/* ------- error messages ------- */
void display_notice( char *s)
{

	nwnd = establish_window(35, 20, 3, max(10, strlen(s)+4),0, 1," Notice ");
	wcursor(nwnd, 1, 0);
	Wprintf(nwnd, s);
}

void clear_notice(void)
{
	if (nwnd)
		delete_window(nwnd);
	nwnd = NULL;
}

SWIN *ewnd;
/* ------- error messages ------- */
void error_message( char *s)
{

	ewnd = establish_window(35, 20, 3, max(10, strlen(s)+4),0, 1," Error ");
	wcursor(ewnd, 1, 0);
	Wprintf(ewnd, s);
	beep();
}

void clear_message(void)
{
	if (ewnd)
		delete_window(ewnd);
	ewnd = NULL;
}


#ifndef	max
/* -- return the maximum value -- */
int max( int a, int b)
{
     return((a > b)? a : b);
}
#endif

#ifndef	min
/* -- return the lower value -- */
int min( int a, int b)
{
     return((a < b)? a : b);
}
#endif

CSTREAM *Sopen(char *path, char *dir) /* -- open stream -- */
{
	CSTREAM *z;
	int fd, flags;

	switch(dir[0])	{
	/*case 'a':	flags =(O_RDWR |O_NDELAY);*/
	case 'a':	flags =(O_RDWR);
				break;
	case 'r':	flags = (O_RDONLY| O_NDELAY);
				break;
	case 'w':	flags = (O_WRONLY | O_CREAT | O_TRUNC);
				break;
	default:	errno = SEINVAL;
				return (NULL);
	}
	if ((fd = open(path, flags, 0666)) == -1){
		return(NULL);
	}
/* 	setmode(fd, O_BINARY); */
	/* if(dir[0] == 'r')
		if((tcflush(fd , TCIFLUSH)) == -1)
			syserr("ioctl, TCFLSH"); 
	else if(dir[0] == 'w')
		if((tcflush(fd , TCOFLUSH)) == -1)
			syserr("ioctl, TCFLSH"); 
	if(dir[0] == 'a')
		if((tcflush(fd , TCIOFLUSH)) == -1)
			syserr("ioctl, TCFLSH"); */
	if((z = (CSTREAM *)malloc(sizeof(CSTREAM))) == NULL)	{
		errno = SENOMEM;
		return(NULL);
	}

	z->fd = fd;
	z->dir = dir[0];
	z->patt = '\0';
	z->err = z->rc = z->xm = z->total = z->next = 0;
	z->loopback = z->highbaud = 0;
/* Call this routine to change stty for terminal devices */
	if(port_set(z)){
		close(z->fd);
		free(z);
		return(NULL);
	}
		
	return(z);
}

int  Sclose(CSTREAM *z)	/* -- close a stream -- */
{
	int fd;

	if(z->dir == 'w' && !writebuf(z))
		return(FALSE);
	/*ioctl(z->fd,TCSETAW,&(z->sterm));*/
	tcsetattr(z->fd, TCSANOW, (struct termios *) &(z->sterm));
	fd = z->fd;
	free(z);
	return(close(fd) != -1);
}
int Sputc( CSTREAM *z, unsigned char c)
{
	z->buf[z->next++] = c;
	if(z->next >= sizeof(z->buf))
		return(writebuf(z));
	return(TRUE);
}
static int writebuf(CSTREAM *z) /* -- flush buffer -- */
{
	int n, total;

	total = 0;
	while (total < z->next)	{
		if((n = write(z->fd, &z->buf[total], z->next - total)) == -1)
			return(FALSE);
			total += n;
	}
	z->xm += total;
	z->next = 0;
	return(TRUE);
}

int port_set(CSTREAM *t)
{
	int res;
	unsigned int speed = baud;
	unsigned char errst[50];

	/*if(Internal_loop != TRUE)
		flow_ctrl=CRTSCTS;*/
	/*if((res = ioctl(t->fd,TCGETA,&(t->sterm))) == 0)*/
	if((res = tcgetattr(t->fd, (struct termios *)&(t->sterm))) == 0)
	{
		if (baud > 15)
			speed = (baud - 15) | CBAUDEX;
		t->cterm = t->sterm;

		t->cterm.c_iflag = 0;
		t->cterm.c_cflag = 0;
		if (flow_ctrl == CRTSCTS){
			t->cterm.c_cflag |= (speed | CS8 | CREAD | CLOCAL 
				| flow_ctrl);
			//t->cterm.c_iflag |= (IGNBRK);
		}
		else{
			t->cterm.c_cflag |= (speed | CS8 | CREAD | CLOCAL);
			t->cterm.c_iflag |= (IGNBRK | flow_ctrl);
		}
		t->cterm.c_oflag = 0;
		t->cterm.c_lflag = 0;
		t->cterm.c_cc[VINTR] = '\277';
		t->cterm.c_cc[VQUIT] = '\277';
		t->cterm.c_cc[VERASE] = '\277';
		t->cterm.c_cc[VKILL] = '\277';
		t->cterm.c_cc[VMIN] = 0;
		t->cterm.c_cc[VTIME] = 1;
		/*if(res = ioctl(t->fd,TCSETAW,&(t->cterm))){*/
		if ((res = tcsetattr(t->fd, TCSANOW, (struct termios *) &(t->cterm)))){
			if(errno == EINVAL){
				sprintf(errst,"Baud rate not supported on this module.");
				error_message(errst); get_char(); clear_message();
				} else {
				sprintf(errst,"ERROR ioctl :%d errno:%d", res, errno);
				error_message(errst); get_char(); clear_message();
			}	
		}
	}
	else
	{
		sprintf(errst,"ERROR ioctl :%d errno:%d", res, errno);
		error_message(errst); get_char(); clear_message();


	}
	/* note You can check for error if desirable */
	return(res);
}


static void status_line( char *s)
{
	int  i, xx, yy,t;
	char statl[85];
	getyx(stdscr, yy, xx);
	
	sprintf(statl,"%s","Use Arrow Keys: F1 for help ");

	if(*s)
		for(i=0;*s && i<SCRNWIDTH-1; i++ )
			mvwaddch(stdscr,23,i,*s++ | A_STANDOUT);
	else
		for(i=0;i<52-1; i++)
			mvwaddch(stdscr,23,i,' '| A_STANDOUT);
	for(;i < 52; i++)
			mvwaddch(stdscr,23,i,' '| A_STANDOUT);
	for(t=0;i < (SCRNWIDTH) && (statl[t]); i++, t++)
			mvwaddch(stdscr,23,i, statl[t] | A_STANDOUT);
 
	move(yy,xx);
	wrefresh(stdscr);
}

