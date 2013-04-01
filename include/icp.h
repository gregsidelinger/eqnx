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

/* include file - icp.h */

#define uchar_t  unsigned char
#define ushort_t unsigned short
#define uint_t   unsigned int

/* icp global register structure - physically overlaps icp_out_struct */

struct icp_gbl_struct
{
  uchar_t	gicp_filler_0[24];
  uchar_t	gicp_bus_ctrl;		/* bus control */
  uchar_t	gicp_rev;		/* icp revision number */
  uchar_t	gicp_filler_1[2];
  uchar_t	gicp_initiate;		/* lmx control & icp enable */
  uchar_t	gicp_scan_spd;		/* lmx scan speeds */
  uchar_t	gicp_tmr_size;		/* interval timer scale preset */
  uchar_t	gicp_tmr_count;		/* interval timer count preset */
  uchar_t	gicp_filler_2[24];
  uchar_t	gicp_watchdog;		/* watchdog timer */
  uchar_t	gicp_filler_3[3];
  uchar_t	gicp_attn;		/* global status - poll for work */
  uchar_t	gicp_chnl;		/* current channel number */
  ushort_t	gicp_frame_ctr;		/* frame counter */
  uchar_t	gicp_filler_4[24];
  uchar_t	gicp_rcv_attn[8];	/* receive attention bits */
  uchar_t	gicp_filler_5[24];
  uchar_t	gicp_xmit_attn[8];	/* transmit attention bits */
};  /* icp_gbl_struct */

/* icp banked input register structure */

struct cin_bnk_struct
{
  ushort_t	bank_dma_ptr;		/* offset ptr to next input dma */
  uchar_t	bank_fifo_lvl;		/* number chars held in fifo */
  uchar_t	bank_tags_l;		/* input tags, low byte */
  ushort_t	bank_sigs;		/* channel input signals */
  uchar_t	bank_filler_0;
  uchar_t	bank_tags_h;		/* input tags, high byte */
  uchar_t	bank_fifo[8];		/* input 8 char fifo */
  ushort_t	bank_num_chars;		/* number of characters received */
  ushort_t	bank_events;		/* input events detected */
};  /* cin_bnk_struct */

/************************************************************
 *                                                          *
 *BANKED INPUT REGISTER DEFINITIONS                         *
 *                                                          *
 ************************************************************/

struct  ssp2_bank_s {
  ushort_t	nxt_dma;		/*Next Input DMA offset*/
  uchar_t	fifo_lvl;		/*COMMON- # of Char held in FIFO*/
  uchar_t	tags_l;			/*COMMON - Input Tags, low byte */
  ushort_t	signals;		/*Input signals */
  uchar_t	undef_6;
  uchar_t	tags_h;			/*COMMON - Input Tags, high byte*/
  uchar_t	fifo_0;			/*COMMON - "Oldest" character not DMA'd */
  uchar_t	fifo_1;			/*COMMON */
  uchar_t	fifo_2;			/*COMMON */
  uchar_t	fifo_3;			/*COMMON */
  uchar_t	fifo_4;			/*COMMON */
  uchar_t	fifo_5;			/*COMMON */
  uchar_t	fifo_6;			/*COMMON */
  uchar_t	fifo_7;			/*COMMON */
  ushort_t	num_chars;		/*Number of characters received*/
  ushort_t	events;			/*COMMON - Input Events	*/
};	

/************************************************************
 *                                                          *
 * SSP & SSP2 COMMON BANKED REGISTERS                       *
 *                                                          *
 ***********************************************************/

struct  common_bank_s  {
  ushort_t	unused_00;		/* NOT COMMON */		
  uchar_t	fifo_lvl;		
  uchar_t	tags_l;		
  ushort_t	unused_04;		/* NOT COMMON */
  uchar_t	undef_6;		
  uchar_t	tags_h;		
  uchar_t	fifo_0;		
  uchar_t	fifo_1;		
  uchar_t	fifo_2;	
  uchar_t	fifo_3;	
  uchar_t	fifo_4;
  uchar_t	fifo_5;
  uchar_t	fifo_6;
  uchar_t	fifo_7;
  ushort_t	num_chars;	
  ushort_t	events;	
};

/************************************************************
 *                                                          *
 * SSP2 INPUT REGISTER DEFINITION                           *
 *                                                          *
 ************************************************************/

struct  ssp2_input_s  {
  ushort_t	char_ctrl;		/*COMMON - Input Character Control & Attributes*/
  uchar_t	locks;			/*COMMON - Locks*/
  uchar_t	undef_03;
  ushort_t	baud;			/*COMMON - Input baud rate*/
  uchar_t	q_cntrl;		/*Input Queue Control */
  uchar_t	lit_nxt;		/*COMMON - Literal Next Character */
  uchar_t	xon_1;			/*COMMON - XON-1 char */
  uchar_t	xon_2;			/*COMMON - XON-2 char (used if 2 char XON)*/
  uchar_t	xoff_1;			/*COMMON - XOFF-1 char */
  uchar_t	xoff_2;			/*COMMON - XOFF-2 char (used if 2 char XOFF)*/
  uchar_t	unused_0C [4];
  uchar_t	tmr_preset;		/*COMMON - Timer Preset*/
  uchar_t	tmr_count;		/*COMMON - Timer Counter*/
  ushort_t	attn_ena;		/*COMMON - Attention Enable*/
  ushort_t	over_load;		/*COMMON - Buffer Overload Level*/
  uchar_t	susp_off;		/*Stop/Suspend Output if "off" condition met*/
  uchar_t	susp_on;		/*Stop/Suspend Output if "on" condition met*/
  ushort_t	min_char;		/*COMMON - Minimum character event threshold*/
  uchar_t	unused_1A [6];
  uchar_t	lookup_tbl [32];	/*COMMON - Character Lookup Table*/
  ushort_t	tail_a;			/*COMMON - Tail Pointer A*/
  uchar_t	unused_42 [2];
  struct  ssp2_bank_s	bank_a;		/*COMMON - Bank A */
  ushort_t	intern_tdm_a;		/*TDM*/
  uchar_t	unused_5A [2];
  ushort_t	intern_baud_ctr;	/*Internal Baud Rate Counter	*/
  uchar_t	intern_char_asm;	/*Internal Character Assembly	*/
  uchar_t	intern_flags;		/*COMMON - Internal Flags */
  ushort_t	tail_b;			/*COMMON - Tail Pointer B*/
  uchar_t	unused_62 [2];
  struct  ssp2_bank_s	bank_b;		/*COMMON - Bank B */
  ushort_t	intern_tdm_b;		/*TDM */
  uchar_t	unused_7A [2];
  uchar_t	intern_tmr_p;		/*COMMON - Timer Scale */
  uchar_t	intern_tmr_c;		/*COMMON - Timer Counter */
  uchar_t	intern_prt_char;	/*COMMON - Partially Framed Character	*/
  uchar_t	in_band;		/*COMMON - Inband Flow Control	*/
} ;

/************************************************************
 *                                                          *
 * SSP & SSP2 COMMON INPUT REGISTERS                        *
 *                                                          *
 ************************************************************/

struct  common_input_s  {				
  ushort_t	char_ctrl;		/*COMMON - Input Character Control*/
  uchar_t	locks;			/*COMMON - Locks*/
  uchar_t	undef_03;	
  ushort_t	baud;			/*COMMON - Baud Rate*/
  uchar_t	undef_06;
  uchar_t	lit_nxt;		/*COMMON - Literal Next*/
  uchar_t	xon_1;			/*COMMON - XON-1 */
  uchar_t	xon_2;			/*COMMON - XON-2 */
  uchar_t	xoff_1;			/*COMMON - XOFF-1 */
  uchar_t	xoff_2;			/*COMMON - XOFF-2 */
  uchar_t	unused_0C [4];	
  uchar_t	tmr_preset;		/*COMMON - Timer Preset */
  uchar_t	tmr_count;		/*COMMON - Timer Count */
  ushort_t	attn_ena;		/*COMMON - Attention Enable */
  ushort_t	over_load;		/*COMMON - Buffer Overload */
  uchar_t 	undef_16;	
  uchar_t 	undef_17;
  ushort_t	min_char;		/*COMMON - Minimum Character Event Threshold */
  uchar_t	unused_1A [6];	
  uchar_t	lookup_tbl [32];	/*COMMON - Lookup Table */
  ushort_t	tail_a;			/*COMMON - Tail A */
  uchar_t 	unused_42 [2];	
  struct  ssp2_bank_s	bank_a;		/*COMMON - Bank A */
  uchar_t	undef_58 [8];
  ushort_t	tail_b;			/*Common - Tail A */
  uchar_t	unused_62 [2];	
  struct  ssp2_bank_s	bank_b;		/*COMMON - Bank B */
  uchar_t	undef_78 [2];	
  uchar_t	unused_7A [2];
  uchar_t	intern_tmr_p;		/*COMMON - Internal Timer Scale */
  uchar_t	intern_tmr_c;		/*COMMON - Internal Timer Count */
  uchar_t	intern_prt_char;	/*COMMON - Partial Character */
  uchar_t	in_band;		/*COMMON - Inband Flow Control */
};

/************************************************************
 *                                                          *
 * OUTPUT QUEUE REGISTER DEFINITIONS                        *
 *                                                          *
 ************************************************************/

struct  ssp2_queue_s {
  uchar_t	undef_00 [4];
  ushort_t	data_ptr;		/*Queue Data Pointer*/
  uchar_t	undef_06;
  uchar_t	buff_type_sz;		/*Buffer type & Size */
  ushort_t	data_count;		/*Output Data Count  */
  uchar_t	low_threshold;		/*Low Water Threshold	*/
  uchar_t	send_data_state;	/*Send Data State   */
} ;
 
/************************************************************
 *                                                          *
 *OUTPUT SESSION REGISTER DEFINITIONS                       *
 *                                                          *
 ************************************************************/

struct  ssp2_session_s {
  uchar_t	col;			/*COMMON - Session column counter  */
  uchar_t	expnd_ena;		/*COMMON - OPOST Expansion Enable */
  ushort_t	expnd_ctr;		/*Expansion Character Counter  */
};

/************************************************************
 *                                                          *
 * SSP2 OUTPUT REGISTER DEFINITIONS                         *
 *                                                          *
 ************************************************************/

struct  ssp2_output_s  {
  struct  ssp2_queue_s	q0;		/*COMMON - Queue 0 */
  ushort_t	intern_tdm_a;		/*Output TDM 0 */
  ushort_t	intern_tdm_b;		/*Output TDM 1	*/
  uchar_t	fifo_0;			/*COMMON */
  uchar_t	fifo_1;			/*COMMON */
  uchar_t	fifo_2;			/*COMMON */
  uchar_t	fifo_3;			/*COMMON */
  struct  ssp2_session_s ses_a;		/*COMMON - Session A */
  ushort_t	frame_ctr;		/*Frame Counter	*/
  uchar_t	undef_1A [6];
  uchar_t	intern_flow;		/*COMMON - Internal Flow Cintrol State	*/
  uchar_t	intern_opost;		/*Internal OPOST State	*/
  uchar_t	intern_fifo_ptr;	/*COMMON - Internal FIFO Pointers */
  uchar_t	intern_tog;		/*Internal Saved Toggles */
  ushort_t	intern_tmr;		/*COMMON - Output Timer Counter	*/
  uchar_t	undef_26 [2];
  ushort_t	intern_tmr_g;		/*Global Timer Counter*/
  ushort_t	events_a;		/*Events register A */
  ushort_t	intern_status;		/*Status */
  ushort_t	events_b;		/*Events Register B */
  ushort_t	nxt_dma;		/*Next DMA Address */
  uchar_t	unused_0x31;
  uchar_t	intern_dma_sv;		/*Saved DMA Status */
  struct  ssp2_session_s ses_b;		/*COMMON - Session B	*/
  uchar_t	undef_38 [8];
  struct  ssp2_queue_s	q1;		/*COMMON - Queue 1 */
  ushort_t	intern_baud_ctr;	/*COMMON-Internal Baud Rate Counter */
  ushort_t	intern_state;		/*COMMON-Internal Output Character State */
  uchar_t	fifo_4;			/*COMMON */
  uchar_t	fifo_5;			/*COMMON */
  uchar_t	fifo_6;			/*COMMON */
  uchar_t	fifo_7;			/*COMMON */
  struct  ssp2_session_s ses_c;		/*COMMON - Session C */
  uchar_t	undef_58 [8];
  ushort_t	baud;			/*COMMON - Baud Rate */
  uchar_t	char_fmt;		/*COMMON - Character Format */
  uchar_t	flow_config;		/*XON/XOFF Control */
  ushort_t	cntrl_sigs;		/*Control Signals */
  uchar_t	loop_backs;		/*Loop Backs */
  uchar_t	unused_0x67;
  uchar_t	glb_tmr_preset; 	/*Global Timer scale preset */
  uchar_t	glb_tmr_count;		/*Global Timer Down Counter preset */
  ushort_t	attn_ena;		/*Attention Enable */
  uchar_t	xoff_1;			/*COMMON - XOFF-1 Character */
  uchar_t	xoff_2;			/*COMMON - XOFF-1 Character */
  uchar_t	xon_1;			/*COMMON - XON-1 Character */
  uchar_t	xon_2;			/*COMMON - XON-2 Character */
  uchar_t	tmr_preset;		/*COMMON - Timer Scale Preset	*/
  uchar_t	tmr_count;		/*COMMON - Timer Down Counter	*/
  uchar_t	request;		/*Request	*/
  uchar_t	locks;			/*Locks		*/
  struct  ssp2_session_s ses_d;		/*COMMON - Session D */
  uchar_t	undef_78 [8];
} ;	

/************************************************************
 *                                                          *
 * SSP & SSP2 COMMON OUTPUT REGISTERS                       *
 *                                                          *
 ************************************************************/

struct  common_output_s  {
  uchar_t	usused_00 [sizeof(struct  ssp2_queue_s)];
  uchar_t	usused_0C [4];		
  uchar_t	fifo_0;			/*COMMON - FIFO 0 Character */
  uchar_t	fifo_1;			/*COMMON - FIFO 1 Character */
  uchar_t	fifo_2;			/*COMMON - FIFO 2 Character */
  uchar_t	fifo_3;			/*COMMON - FIFO 3 Characte */
  struct  ssp2_session_s ses_a;		/*COMMON - Session A	*/
  uchar_t	unused_18 [2];
  uchar_t	undef_1A  [6];
  uchar_t	intern_flow;		/*COMMON - Internal Flow Cintrol State	*/
  uchar_t	unused_21;	
  uchar_t	intern_fifo_ptr;	/*COMMON - Internal FIFO Pointers*/
  uchar_t	unused_23;	
  ushort_t	intern_tmr;		/*COMMON - Output Timer Counter	*/
  uchar_t	undef_26 [2];
  uchar_t	unused_28 [12];
  struct  ssp2_session_s ses_b;		/*COMMON - Session B	*/
  uchar_t	undef_38 [8];		
  struct  ssp2_queue_s	q1;		/*COMMON - Queue 1 */
  ushort_t	intern_baud_ctr;	/*COMMON - Internal Baud Rate Counter */
  ushort_t	intern_state;		/*COMMON - Internal Output Character State */
  uchar_t	fifo_4;			/*COMMON - FIFO 4 Character */
  uchar_t	fifo_5;			/*COMMON - FIFO 5 Character */
  uchar_t	fifo_6;			/*COMMON - FIFO 6 Character */
  uchar_t	fifo_7;			/*COMMON - FIFO 7 Character */
  struct  ssp2_session_s ses_c;		/*COMMON - Session C	*/
  uchar_t	undef_58 [8];	
  ushort_t	baud;			/*COMMON - Baud Rate */
  uchar_t	char_fmt;		/*COMMON - Character Format*/
  uchar_t	unused_63 [3];	
  uchar_t	loop_backs;		/*COMMON - Loop Back bits (0 & 1)*/
  uchar_t	unused_67 [5];	
  uchar_t	xoff_1;			/*COMMON - XOFF-1 Character */
  uchar_t	xoff_2;			/*COMMON - XOFF-1 Character */
  uchar_t	xon_1;			/*COMMON - XON-1 Character */
  uchar_t	xon_2;			/*COMMON - XON-2 Character */
  uchar_t	tmr_preset;		/*COMMON - Timer Scale Preset */
  uchar_t	tmr_count;		/*COMMON - Timer Down Counter*/
  uchar_t	unused_72 [2];	
  struct  ssp2_session_s ses_d;		/*COMMON - Session D	*/
  uchar_t	undef_78 [8];
};

/************************************************************
 *                                                          *
 * SSP2 GLOBAL REGISTER DEFINITIONS                         *
 *                                                          *
 ***********************************************************/

struct  ssp2_global_s  {
  uchar_t	bus_cntrl;		/*Global Control register*/
  uchar_t	rev;			/*Revision Level Number	*/
  uchar_t	on_line;		/*On-line	*/
  uchar_t	unused_3;
  uchar_t	chan_ctr;		/*Active Channel Number*/
  uchar_t	unused_5;
  uchar_t	chan_attn;		/*Channel Attention bits*/
  uchar_t	tmr_evnt;		/*Timer Event Bits	*/
  uchar_t	unused_8 [16];
  uchar_t	filler3;
  uchar_t	type;			/*COMMON	*/
  uchar_t	unused_1A [0x3C - 0x1A];
  uchar_t	attn_pend;		/*Channel Attention and/or   */
};

struct  common_global_s  {
  uchar_t	unused_8 [0x19 - 0x00];
  uchar_t	type;			/*SSP Type bit	*/
  uchar_t	unused_1A [0x3D - 0x1A];
};

/************************************************************/
/*                                                          */
/* UNION OF GLOBAL REGISTER STRUCTURES                      */
/*                                                          */
/************************************************************/

union	global_regs_u	/*Declare Union of SSP & SSP2 Global registers	*/
{
  struct  icp_gbl_struct ssp;		/*SSP Global registers structure */
  struct  ssp2_global_s ssp2;		/*SSP2 Global registers structure */
  struct  common_global_s com;		/*SSP/SSP2 Common Global regs struct */

};

/************************************************************
 *                                                          * 
 * SSP2 ADDRESS SPACE DEFINITION                            *
 * Place here to calculate sizes of registers               *
 ************************************************************/

struct  ssp2_addr_space_s  {
  struct  ssp2_input_s in_chan  [4];	/*Input Channel area */
  struct  ssp2_output_s	out_chan [4];	/*Output Channel area */
  union   global_regs_u	global;		/*Global area */
  uchar_t	empty_0[0x1000 - 0x400 - sizeof(union global_regs_u)];
  uchar_t	in_buf  [0x1000];	/*Input buffer area */
  uchar_t	out_buf [0x1000];	/*Output buffer area */
  uchar_t	tags    [0x0400];	/*Input tag area */
  uchar_t	empty_1[0x4000 - 0x3000 - 0x400];
};	

/************************************************************
 *                                                          *
 *UNION OF BANKED REGISTER STRUCTURES                       *
 *                                                          *
 ************************************************************/

union	bank_regs_u	/*Declare Union of SSP & SSP2 Banked registers	*/
{
  struct  cin_bnk_struct ssp;  		/*SSP Banked registers structure */
  struct  ssp2_bank_s ssp2; 		/*SSP2 Banked registers structure*/
  struct  common_bank_s	com;  		/*SSP/SSP2 Common Banked regs struct*/

};

/* icp input register structure */

struct icp_in_struct
{
  ushort_t	cin_char_ctrl;		/* input char control & attributes */
  uchar_t	cin_locks;		/* input locks */
  uchar_t	cin_dma_base;		/* input dma base - bits b23 - b16 */
  ushort_t	cin_baud;		/* input baud */
  uchar_t	cin_q_ctrl;		/* input queue control reg */
  uchar_t	cin_lit_nxt_char;	/* literal next char */
  uchar_t	cin_xon_1;		/* xon-1 character */
  uchar_t	cin_xon_2;		/* xon-2 character */
  uchar_t	cin_xoff_1;		/* xoff-1 character */
  uchar_t	cin_xoff_2;		/* xoff-2 character */
  uint_t	cin_filler_0;
  uchar_t	cin_tmr_preset_size;	/* character timer scale preset */
  uchar_t	cin_tmr_preset_count;	/* character tick count preset */
  ushort_t	cin_attn_ena;		/* input attention mask */
  ushort_t	cin_overload_lvl;	/* input queue/buffer overload level */
  uchar_t	cin_susp_output_lmx;	/* enable channel output suspension */
  uchar_t	cin_susp_output_sig;	/* enable channel output suspension */
  ushort_t	cin_min_char_lvl;	/* minimum character level */
  ushort_t	cin_filler_1[3];
  uchar_t	cin_lookup_tbl[32];	/* char lookup attn bits */
  ushort_t	cin_tail_ptr_a;		/* tail pointer a */
  ushort_t	cin_filler_2;
  union		bank_regs_u cin_bank_a;
  ushort_t	cin_tdm_early;		/* input tdm early */
  ushort_t	cin_filler_3;
  ushort_t	cin_baud_ctr;		/* autobaud ctr & ctr bit sample */
  uchar_t	cin_input_state;	/* input state */
  uchar_t	cin_int_flags;		/* self-clearing flags */
  ushort_t	cin_tail_ptr_b;		/* tail pointer b */
  ushort_t	cin_filler_4;
  union		bank_regs_u cin_bank_b;
  ushort_t	cin_tdm_late;		/* input tdm late */
  ushort_t	cin_filler_5;
  uchar_t	cin_tmr_size;		/* character timer scale */
  uchar_t	cin_char_tmr_remain;	/* (VTIME) number of remaining ticks */
  uchar_t	cin_partial_char;	/* partial incoming character */
  uchar_t	cin_inband_flow_ctrl;	/* inband flow control */
};   /* icp_in_struct */

/* icp output queue structure -
     this structure is "owned" by cpu until a command is given to
     execute commands (after which it is owned by icp */

struct cout_que_struct
{
  ushort_t	q_cmd_ptr;		/* queue cmd ptr - lower 16 bits 
					   must be left-shifted by 2 when
					   written and right-shifted by 2
					   when read */
  uchar_t	q_cmd_base;		/* queue cmd base - bits 23..16 */
  uchar_t	q_cmd_save;		/* queue command save register */
  ushort_t	q_data_ptr;		/* queue data ptr - low 16 bits */
  uchar_t	q_data_base;		/* queue data base - bits 23..16 */
  uchar_t	q_size;			/* circular queue size/wrap reg(type) */
  ushort_t	q_data_count;		/* queue data count register */
  uchar_t	q_block_count;		/* queue block count register */
  uchar_t	q_out_state;		/* queue output state register */
};   /* cout_que_struct */

/************************************************************
 *                                                          *
 * UNION OF QUEUE REGISTER STRUCTURES                       *
 *                                                          *
 ************************************************************/

union	queue_regs_u	/*Declare Union of SSP & SSP2 Queue registers	*/
{
  struct  cout_que_struct	ssp;	/*SSP Queue registers structure	*/
  struct  ssp2_queue_s		ssp2;	/*SSP2 Queue registers structure */
} ;

/* icp output register structure */

struct  icp_out_struct
{
  union		queue_regs_u cout_q0;	/* queue zero */
  ushort_t	cout_tdm_0;		/* output tdm 0 */
  ushort_t	cout_tdm_1;		/* output tdm 1 */
  uint_t	cout_tx_fifo_0;		/* next four chars to tx */
  uchar_t	cout_ses_col_a;		/* session a column counter */
  uchar_t	cout_ses_ctrl_a;	/* session a control register */
  ushort_t	cout_ses_invent_a;	/* session a count chars invented */
  uchar_t	cout_filler_0[8];
  uchar_t	cout_int_flow_ctrl;	/* cpu flow control register */
  uchar_t	cout_int_opost;		/* internal opost & flow control */
  uchar_t	cout_int_fifo_ptr;	/* internal fifo pointers */
  uchar_t	cout_int_save_togl;	/* internal request and state mgmt */
  ushort_t	cout_timer;		/* (24h) output timer */
  ushort_t	cout_flow_count;	/* output flow control counter */
  ushort_t	cout_cmd_dly_timer;	/* command delay timer */
  ushort_t	cout_events_a;		/* (2ah) output events register a */
  ushort_t	cout_status;		/* (2ch) output status register */
  ushort_t	cout_events_b;		/* (2eh) output events register b */

					/* these dma ptrs are copied from 
					   current cout_que_struct */
  ushort_t	cout_dma_ptr;		/* dma address ptr - low 16 bits */
  uchar_t	cout_dma_base;		/* dma base - upper 8 bits */

  uchar_t	cout_dma_stat_save;	/* dma status save */
  uchar_t	cout_ses_col_b;		/* session b column counter */
  uchar_t	cout_ses_ctrl_b;	/* session b control register */
  ushort_t	cout_ses_invent_b;	/* session b count chars invented */
  uchar_t	cout_filler_1[8];
  union		queue_regs_u cout_q1;	/* queue one */
  ushort_t	cout_int_baud_ctr;	/* internal baud counter */
  ushort_t	cout_int_state;		/* output state mach state */
  uint_t	cout_tx_fifo_1;		/* later four chars to tx */
  uchar_t	cout_ses_col_c;		/* session c column counter */
  uchar_t	cout_ses_ctrl_c;	/* session c control register */
  ushort_t	cout_ses_invent_c;	/* session c count chars invented */
  uchar_t	cout_filler_2[8];
  ushort_t	cout_baud;             	/* output baud rate register */
  uchar_t	cout_char_fmt;		/* output character format register */
  uchar_t	cout_flow_cfg;		/* cpu flow control register */
  ushort_t	cout_ctrl_sigs;		/* output control signals */
  uchar_t	cout_lmx_cmd;		/* lmx command register */
  uchar_t	cout_mux_ctrl;		/* mux control */
  ushort_t	cout_flow_timeout;	/* flow control timeout */
  ushort_t	cout_attn_ena;		/* (6ah) output attention mask */
  uchar_t	cout_xoff_1;		/* xoff-1 character */
  uchar_t	cout_xoff_2;		/* xoff-2 character */
  uchar_t	cout_xon_1;		/* xon-1 character */
  uchar_t	cout_xon_2;		/* xon-2 character */
  uchar_t	cout_tmr_size;		/* (70h) timer scale register */
  uchar_t	cout_tmr_count;		/* (71h) timer register */
  uchar_t	cout_cpu_req;		/* (72h) cpu request register */
  uchar_t	cout_lock_ctrl;		/* (73h) output lock and control reg */
  uchar_t	cout_ses_col_d;		/* session d column counter */
  uchar_t	cout_ses_ctrl_d;	/* session d control register */
  ushort_t	cout_ses_invent_d;	/* session d count chars invented */
  uchar_t	cout_filler_3[8];
};  /* icp_out_struct */

/************************************************************
 *                                                          *
 * UNION OF INPUT REGISTER STRUCTURES	                    *
 *                                                          *
 ************************************************************/

union	input_regs_u		/*Declare Union of SSP & SSP2 Input registers */
{
  struct  icp_in_struct ssp;	/*SSP Input registers structure	*/
  struct  ssp2_input_s	ssp2;	/*SSP2 Input registers structure */
  struct  common_input_s com;	/*SSP & SSP2 Common Input registers structure	*/

} ;

/************************************************************
 *                                                          *
 * UNION OF OUTPUT REGISTER STRUCTURES                      *
 *                                                          *
 ************************************************************/

union	output_regs_u	/*Declare Union of SSP & SSP2 Output registers	*/
{
  struct  icp_out_struct ssp;	/*SSP Output registers structure*/
  struct  ssp2_output_s ssp2;	/*SSP2 Output registers structure*/
  struct  common_output_s com;	/*SSP & SSP2 Common Output registers structure	*/

} ;

