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

/* ------------------- twindow.h ----------------------- */
#include <curses.h>
#include "keys.h"

/* #define eqnx_exit(val)	resetty();nl();echo();endwin();exit(val); */

#define eqnx_exit(val)		nl();echo();refresh();endwin();exit(val);
#define FASTWINDOWS
#define FIELDCHAR '_'

#define BORDER 0
#define TITLE 1
#define ACCENT 2
#define NORMAL 3
#define ALL 4
/* -------- local prototypes -------- */
#define ON  1
#define OFF 0
#define ERROR -1

#define TABS 4
#define SCRNHT 25
#define SCRNWIDTH 80
#define MAXSCR 14

#ifdef	NOTUSED
#ifndef	XENIX
static chtype GLATT=0L;
#endif
#endif

#define ERROR -1
/* #define OK 0 */
/*page*/
/* ------------- window controller structures ----------- */
struct field {	/* data entry field description	*/
	char *fmask;		/* field data entry mask		*/
	int fprot;			/* field protection	      */
	char *fbuff;		/* field buffer			*/
	int ftype;			/* field type			*/
	int frow;			/* field row			*/
	int fcol;			/* field column			*/
	/* void (*fhelp)(char *);  field help function		*/
	void (*fhelp)();  /* field help function		*/
	char *fhwin;		/* field help window			*/
	int flx, fly;		/* help window location			*/
 	/* int (*fvalid)(char *);  field validation function	*/
 	int (*fvalid)();  /* field validation function	*/
	struct field *fnxt;	/* next field on template 		*/
	struct field *fprv;	/* previous field on template 	*/
}field;
typedef struct sclent {
	char name[35];
	struct sclent *prv;		
	struct sclent *nex;
}SCRLL;
		
typedef struct wn{
	int _wv;		  /* true if window is visible     */
	int _hd;		  /* true if window was hidden     */
	chtype *_ws;	  /* points to window save block   */
	chtype _attru;
	chtype _hattru;
	short rev_vid;
	char *_tl;		  /* points to window title		   */
	int _wx;		  /* nw x coordinate			   */
	int _wy;		  /* nw y coordinate			   */
	int _ww;		  /* window width		   */
	int _wh;		  /* window height		       */
	int _wsp;		  /* scroll pointer		       */
	int _sp;		  /* selection pointer		       */
	int _cx;		  /* cursor x location		       */
	int btype;		  /* border type		   */
	int wcolor[4];	  /* colors for window 		   */
	int _pn;		  /* previous normal color	   */
	struct wn *_nx; /* points to next window         */
	struct wn *_pv; /* points to previous window     */
	struct field *_fh;		  /* points to 1st data entry fld  */	
	struct field *_ft;		  /* points to last data entry fld */
	int single_pass;		/* field single pass mode set */
        WINDOW *_cw;             /* curse window pointer */
	char last_vselc;
}SWIN;

typedef struct {
	char *mname;		 /* menu bar selection names	   */
	char **mselcs;		 /* the pop-down menu selections   */
	int (**func)();	 /* the functions to execute  	   */
} SMENU;

#define endname(m,h,v) \
	(&(m[h-1].mselcs[v-1][strlen(m[h-1].mselcs[v-1])]))
#define selector(m,h,v) (*(endname(m,h,v)-2))
#define enabler(m,h,v)  (*(endname(m,h,v)-1))

#define CHECK 0xfb
#define test_option(m,h,v)        ((unsigned char)selector(m,h,v)==CHECK)
#define set_option(m,h,v)         (selector(m,h,v)=CHECK)
#define clear_option(m,h,v)       (selector(m,h,v)=' ')
#define enable_selection(m,h,v)   (enabler(m,h,v)=' ')
#define disable_selection(m,h,v)  (enabler(m,h,v)='*')
#define enabled(m,h,v)  		  (enabler(m,h,v)!='*')



#define SPASS   (wnd->single_pass)
#define LASTS	(wnd->last_vselc)
#define CURSEW  (wnd->_cw)
#define WATT 	(wnd->_attru)
#define HATT 	(wnd->_hattru)
#define REVV 	(wnd->rev_vid)
#define SAV 	(wnd->_ws)
#define WTITLE  (wnd->_tl)
#define COL 	(wnd->_wx)
#define ROW 	(wnd->_wy)
#define WIDTH 	(wnd->_ww)
#define HEIGHT 	(wnd->_wh)
#define SCROLL  (wnd->_wsp)
#define SELECT  (wnd->_sp)
#define WCURS   (wnd->_cx)
#define WBORDER	(wnd->wcolor[BORDER])
#define WTITLEC	(wnd->wcolor[TITLE])
#define WACCENT	(wnd->wcolor[ACCENT])
#define WNORMAL	(wnd->wcolor[NORMAL])
#define PNORMAL (wnd->_pn)
#define BTYPE	(wnd->btype)
#define NEXT    (wnd->_nx)
#define PREV    (wnd->_pv)
#define WCOLOR  (wnd->wcolor)
#define VISIBLE (wnd->_wv)
#define HIDDEN  (wnd->_hd)
#define FHEAD	(wnd->_fh)
#define FTAIL   (wnd->_ft)
#define NW 218
#define NE 191

#define DEL_UARROW 1
#define DEL_DARROW 2

#define RTEE 191

#define DTEE 194
#define LINE 196
#define SIDE 179
#define SW 192
#define SE 217
#define MAX_WINDOWS 10
#define BELL 0x7

#define ESC 0x1B
#define HOME 0x1E
#define BS 0x08
#define UP 0x0B
#define DOWN 0x0A
#define RARROW 0x0C
#define MAXFIELDS 24


#define DN 0x0A
#define DEL 0x7F
#define FWD 0x0C
#define F1 0xEC
/*#define reverse_video(wnd) wnd->rev_vid=1
#define normal_video(wnd) wnd->rev_vid=0
*/
/* #define NE   	(wcs[wnd->btype].ne) */
/* #define NW   	(wcs[wnd->btype].nw) */
/* #define SE   	(wcs[wnd->btype].se) */
/* #define SW   	(wcs[wnd->btype].sw) */
/* #define SIDE 	(wcs[wnd->btype].side) */
/* #define LINE 	(wcs[wnd->btype].line) */

/* ----- help functions ----- */
void load_help();
int get_char();
void set_help();
/* ------- screen functions ---- */
void field_pass_mode();

void wscroll();
void wputchar( SWIN *wnd, unsigned char c);
void wmputchar( SWIN *wnd, int x, int y, unsigned char c);
int wputacs();
int put_arrow();
void Wprintf();
void dtitle();
void wframe();
void wcursor();
int verify_wnd();
void vsave();
void vrstr();
/* ----- data entry field ------- */
void wprompt();
void init_template();
void field_tally();
void clear_template();
struct field *establish_field();

void field_window();

/* ---------------- misc ----------------------- */
void error_message();
void clear_message();

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif


void executive();
int vscroll();

void syserr();
void fatal();

/* ----- window functions and macros ------- */
SWIN *establish_window();
void display_window();
void delete_window();
void mclear_window();
void hide_window();
void set_wattri();
void reverse_video();
void field_help(); 
void field_validate();
void dimension();
int count_strings();
/*typedef unsigned long paddr_t;*/
