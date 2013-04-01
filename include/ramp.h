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

/***************************************************************/
/*                                                             */
/*                          RAMP HEADER                        */
/*                                                             */
/* NOTE: Header defines literals, control blocks etc. used     */
/*       by RAMP Services software.  REF's to MPA = RAMP       */
/*                                                             */
/* FILE:   RAMP.H                                              */
/* DATE:   01/15/98                                            */
/* REV:       42 (update Revision History if change rev here)  */
/* AUTHOR: BILL KRAMER                                         */
/*                                                             */
/*                                                             */
/***************************************************************/

#define	RAMP_H_INCLUDED			/*Create Symbol indicating RAMP.H 	*/
					/*already included/compiled		*/

/********************************************************************************
 *                                                                              *
 *                               RAMP.H REVISION HISTORY                        *
 *                               =========================                      *
 *                                                                              *
 * REV	  DATE				                     COMMENTS           *
 * ===  ========        ======================================================= *
 *  42	01/15/98	1. Updated Revision to reflect changes in RAMPSRVC.C	*
 *  41	10/09/97	1. Added forward definintion to slot_struct to satisfy	*
 *			   LINUX build						*
 *			2. Add this Revision block to clarify state of changes	*
 *                                                                              *
 ********************************************************************************/


/********************************************************************************/
/*										*/
/*				SERVICES REVISION STATUS			*/
/*										*/
/* REV (MPA_REV)     DATE     **   RAMPSRVC.C REV   RAMPADMN.C REV   RAMP.H REV	*/
/* =============   ========   **   ==============   ==============   ========== */
/*      15         01/15/98   **        15		 06               42	*/
/********************************************************************************/
#define    MPA_REV     		15			/*Rev of MPA software services	*/
#define    MPA_ADMIN_DIAG	 6			/*Rev of Admin/Diag Poll service*/

/********************************************
 *                                          *
 *     COMPILE - SYSTEM DEPENDENT MACROS    *
 *                                          *
 ********************************************/

/*NOTE: The following symbols must be "defined" prior to inclusion of this file:*/
/*              mpa_global_s = Name of SSP64 Global register structure          */
/*              mpa_input_s  = Name of SSP64 Input register structure           */
/*              mpa_output_s = Name of SSP64 Output register structure          */
/*      Example:                                                                */
/*              #define  mpa_global_s      icp_gbl_struct                    */
/*              #define  mpa_input_s       icp_in_struct                           */
/*              #define  mpa_output_s      icp_out_struct                          */
/********************************************
 *                                          *
 *             DEFINED SYMBOLS              *
 *                                          *
 ********************************************/

#define    ENGS_MPA		4			/*Number of Engines per MPA	*/

#define    SLOT_ARRAY_SZ	16			/*Number members in Slot Array	*/
							/*or # slots/CHANS per MPA	*/

#define    SLOT_SCRATCH_SZ	8			/*Number bytes in Slot Control	*/
							/*Block Scratch areas		*/

#define    DEADMAN_DELAY        ((unsigned short int)(4*500))  /*Dead man time out      */
                                                               /*in SSP Frames:         */
                                                               /* ~4 Frames per millisec*/
                                                               /* SSP Frame = 260.4 usec*/

/********************************************
 *                                          *
 *   SERVICE REQUEST RETURN ERROR CODES     *
 *                                          *
 ********************************************/

/*ADAPTER SERVICE - Identify System Functions (mpa_srvc_id_functs)						*/
#define    ERR_ID_FN_REDUNT	  ((unsigned short int)0xFF)	/*Redundant call				*/
#define    ERR_ID_FN_ADDR_BAD	  ((unsigned short int)0xFC)	/*Bad Function Addr in Import Block		*/

/*ADAPTER SERVICE - Register SSP (mpa_srvc_reg_ssp)								*/
#define    ERR_REG_SSP_GREG	  ((unsigned short int)0xFF)	/*Caller's SSP Global Register ptr bad		*/
#define    ERR_REG_SSP_SSP_AREA	  ((unsigned short int)0xFE)	/*Caller's SSP Data Area ptr bad (area in use)	*/
#define    ERR_REG_SSP_NO_FNS	  ((unsigned short int)0xFD)	/*System Functions NOT Defined			*/
#define    ERR_REG_SSP_FN_ADR_BAD ((unsigned short int)0xFC)	/*Function Addresses inconsistent		*/

/*ADAPTER SERVICE - Register MPA (mpa_srvc_reg_mpa)								*/
#define    ERR_REG_MPA_SSP_AREA	  ((unsigned short int)0xFF)	/*Caller's SSP Data Area ptr bad (area in use)	*/
#define    ERR_REG_MPA_CHAN_NUM	  ((unsigned short int)0xFE)	/*Caller's Chan number bad			*/
#define    ERR_REG_MPA_SLOT_ARRY  ((unsigned short int)0xFD)	/*Caller's Slot Array poiter bad		*/
#define    ERR_REG_MPA_IN_REG	  ((unsigned short int)0xFC)	/*Caller's SSP Input Reg ptr bad		*/
#define    ERR_REG_MPA_ID_BAD	  ((unsigned short int)0xFB)	/*MPA ID not as expected (not 8, 9 or 0x0A)	*/
#define    ERR_REG_MPA_ENG_ALLOC  ((unsigned short int)0xFA)	/*MPA previously allocated			*/
#define    ERR_REG_MPA_ENG_OFL    ((unsigned short int)0xF9)	/*Ring Configuration Error, starting Eng # + 	*/
								/*# Engs overflow Ring (ie ofl SSP's MPA Array) */

/*ADAPTER SERVICE - De-Register MPA (mpa_srvc_dereg_mpa)							*/
#define    ERR_DREG_MPA_SSP_AREA  ((unsigned short int)0xFF)	/*Caller's SSP Data Area ptr bad (area in use)	*/
#define    ERR_DREG_MPA_CHAN_NUM  ((unsigned short int)0xFE)	/*Caller's Chan number bad			*/
#define    ERR_DREG_MPA_BAD_ENG	  ((unsigned short int)0xFD)	/*Caller's Chan # does not pt to Registered MPA	*/

/*SLOT SERVICE - Register Modem Board (mpa_srvc_reg_modem)							*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_REG_MODEM_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_REG_MODEM_CALL_BK  ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_REG_MODEM_REQ_OUT  ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_REG_MODEM_REG      ((unsigned short int)0xEC)	/*Modem Board previously registered		*/
#define    ERR_REG_MODEM_BAD_SSP  ((unsigned short int)0xBD)	/*Slot's Input Q size bad, CIN_Q_CNTRL > 8	*/
#define    ERR_REG_MODEM_IN_Q     ((unsigned short int)0xBC)	/*Slot's Input Q not empty			*/
#define    ERR_REG_MODEM_REMOVED  ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_REG_MODEM_DEADMAN  ((unsigned short int)0x81)	/*Dead man timer expired			*/
#define    ERR_REG_MODEM_INPUT_TO ((unsigned short int)0x82)	/*MPA I/O read watch dog time out		*/
#define    ERR_REG_MODEM_NO_UART  ((unsigned short int)0x83)	/*BOARD's SPR not functional			*/

/*SLOT SERVICE - Deregister Modem Board (mpa_srvc_dereg_modem)							*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_DEREG_MODEM_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_DEREG_MODEM_NOT    ((unsigned short int)0xEE)	/*Slot Empty, Modem Board not registered	*/

/*SLOT SERVICE - Slot Status (mpa_srvc_slot_status)								*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_SLOT_STATUS_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/

/*SLOT SERVICE - Initialize (open) UART/Modem (mpa_srvc_init_modem)						*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_INIT_MODEM_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_INIT_MODEM_CALL_BK ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_INIT_MODEM_REQ_OUT ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_INIT_MODEM_NOT_REG ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_INIT_MODEM_REMOVED ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_INIT_MODEM_DEADMAN     ERR_REG_MODEM_DEADMAN	/*Dead man timer expired			*/

/*SLOT SERVICE - Modify UART/Modem Settings (mpa_srvc_modify_settings)						*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_MOD_MODEM_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_MOD_MODEM_CALL_BK  ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_MOD_MODEM_REQ_OUT  ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_MOD_MODEM_NOT_REG  ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_MOD_MODEM_NOPN     ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/
#define    ERR_MOD_MODEM_REMOVED  ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_MOD_MODEM_DEADMAN      ERR_REG_MODEM_DEADMAN	/*Dead man timer expired			*/

/*SLOT SERVICE - Set Internal Loop Back (mpa_srvc_set_loop_back)						*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_SET_LOOP_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_SET_LOOP_CALL_BK   ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_SET_LOOP_REQ_OUT   ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_SET_LOOP_NOT_REG   ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_SET_LOOP_REMOVED   ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/

/*SLOT SERVICE - Clear Internal Loop Back (mpa_srvc_clr_loop_back)						*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_CLR_LOOP_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_CLR_LOOP_CALL_BK   ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_CLR_LOOP_REQ_OUT   ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_CLR_LOOP_NOT_REG   ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_CLR_LOOP_REMOVED   ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/

/*SLOT SERVICE - Close Modem Board (mpa_srvc_close_modem)							*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_CLOSE_MODEM_MARB	   ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_CLOSE_MODEM_NOT     ((unsigned short int)0xEE)	/*Slot not occupied, can not close Modem	*/
#define    ERR_CLOSE_MODEM_REQ_OUT ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_CLOSE_MODEM_NOPN    ((unsigned short int)0xEC)	/*Slot not opened/init'd			*/

/*SLOT SERVICE - Cancel Slot's Outstanding MARB (mpa_srvc_can_marb)						*/
#define    ERR_CAN_SSPCB          ((unsigned short int)0xFF)	/*SSP Data Area ptr bad				*/
#define    ERR_CAN_MPACB          ((unsigned short int)0xFE)	/*MPA/Eng Control Block (CHAN #) invalid	*/
#define    ERR_CAN_SLOTCB         ((unsigned short int)0xFD)	/*SLOT Control Block invalid			*/
#define	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    /*Status for oustanding MARB that was Canceled	*/

/*SLOT SERVICE - Flush MPA Slot Input/Output Buffers (mpa_srvc_flush)						*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_MPA_FLUSH_MARB      ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_MPA_FLUSH_CALL_BK   ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_MPA_FLUSH_REQ_OUT   ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_MPA_FLUSH_NOT_REG   ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_MPA_FLUSH_NOPN      ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/
#define    ERR_MPA_FLUSH_REMOVED   ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/

/*SLOT SERVICE - Hard Reset Modem Board (mpa_srvc_hard_reset)							*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_HRD_RST_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_HRD_RST_CALL_BK    ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_HRD_RST_REQ_OUT    ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_HRD_RST_NOT_REG    ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_HRD_RST_NOPN       ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/
#define    ERR_HRD_RST_REMOVED    ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_HRD_RST_DEADMAN     ERR_REG_MODEM_DEADMAN	/*Dead man timer expired			*/

/*EXCEPTION SERVICE - Input Error Handling (mpa_srvc_input_error)						*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
#define    ERR_SLOT_ERROR_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_SLOT_ERROR_NOT     ((unsigned short int)0xEE)	/*Slot not occupied				*/
#define    ERR_SLOT_ERROR_NOPN    ((unsigned short int)0xEC)	/*Slot not opened/init'd			*/
#define    ERR_SLOT_ERROR_STRNG   ((unsigned short int)0xEB)	/*2nd error chanracter not as expected		*/

/*EXCEPTION SERVICE - Start Break (mpa_srvc_start_break)							*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_START_BREAK_MARB	   ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_START_BREAK_CALL_BK ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_START_BREAK_REQ_OUT ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_START_BREAK_NOT_REG ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_START_BREAK_NOPN    ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/
#define    ERR_START_BREAK_REMOVED ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_START_BREAK_DEADMAN    ERR_REG_MODEM_DEADMAN	/*Dead man timer expired			*/

/*EXCEPTION SERVICE - Stop Break (mpa_srvc_stop_break)								*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_STOP_BREAK_MARB	   ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_STOP_BREAK_CALL_BK  ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_STOP_BREAK_REQ_OUT  ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_STOP_BREAK_NOT_REG  ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_STOP_BREAK_NOPN     ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/

/*EXCEPTION SERVICE - MPA<->UART Hardware Flow Control (mpa_srvc_mpa_flow)					*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_MPA_FLOW_MARB      ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_MPA_FLOW_CALL_BK   ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_MPA_FLOW_REQ_OUT   ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_MPA_FLOW_NOT_REG   ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_MPA_FLOW_NOPN      ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/
#define    ERR_MPA_FLOW_BAD_PARM  ((unsigned short int)0xEA)	/*Flow Control parm invalid (sup_uchar0)	*/
#define    ERR_MPA_FLOW_REMOVED   ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_MPA_FLOW_DEADMAN       ERR_REG_MODEM_DEADMAN	/*Dead man timer expired			*/

/*EXCEPTION SERVICE - Read UART Register (mpa_srvc_read_uart)							*/
/*         MARB_STATUS_SSPCB      ((unsigned short int)0xFF)	SSP Data Area ptr bad				*/
/*         MARB_STATUS_MPACB      ((unsigned short int)0xFE)	MPA/Eng Control Block invalid			*/
/*         MARB_STATUS_SLOTCB     ((unsigned short int)0xFD)	SLOT Control Block invalid			*/
/*         MARB_STATUS_CALL_BK    ((unsigned short int)0xFC)	Invalid MARB "Call Back"			*/
/*     	   MARB_OUT_CANCELED      ((unsigned short int)0xF0)    MARB Canceled					*/
#define    ERR_RD_UART_MARB	  ((unsigned short int)0xEF)	/*Caller's MARB bad				*/
#define    ERR_RD_UART_CALL_BK    ((unsigned short int)0xEE)	/*Incorrect "call back" MARB			*/
#define    ERR_RD_UART_REQ_OUT    ((unsigned short int)0xED)	/*Init MARB recv'd w/call back pending		*/
#define    ERR_RD_UART_NOT_REG    ((unsigned short int)0xEC)	/*Modem Board not registered			*/
#define    ERR_RD_UART_NOPN       ((unsigned short int)0xEB)	/*Slot not opened/init'd			*/
#define    ERR_RD_UART_REMOVED    ((unsigned short int)0x80)	/*Modem Board removed between "call backs"	*/
#define    ERR_RD_UART_DEADMAN    ((unsigned short int)0x81)	/*Dead man timer expired			*/
#define    ERR_RD_UART_INPUT_TO   ((unsigned short int)0x82)	/*MPA I/O read watch dog time out		*/

/*ENABLE/DISABLE Plug and PLAY Probing (mpa_srvc_pnp_probe_cntrl)						*/
#define    ERR_PNP_PROBE_SSPCB_BAD  ((unsigned short int)0xFF)	/*SSPCB Invalid					*/
#define    ERR_PNP_PROBE_REDUND	    ((unsigned short int)0xFE)	/*Redundant call, Probe state already set	*/

/*ADMIN. & DIAG. SERVICE - Poll (mpa_srvc_diag_poll)								*/
#define    ERR_DIAG_POLL_NO_FNS	  ((unsigned short int)0xFD)	/*System Functions NOT Defined			*/

/********************************************************
 *                                                      *
 * ASYNC SERVICE REQUEST HANDLER FUNCTION RETURN CODES 	*
 *		  async_req_hndlr ()			*
 *							*
 ********************************************************/

#define    MARB_STATUS_SSPCB    ((unsigned short int)0xFF)	/*SSP Data Area ptr bad				*/
#define    MARB_STATUS_MPACB    ((unsigned short int)0xFE)	/*MPA/Eng Control Block invalid			*/
#define    MARB_STATUS_SLOTCB   ((unsigned short int)0xFD)	/*SLOT Control Block invalid			*/
#define    MARB_STATUS_CALL_BK  ((unsigned short int)0xFC)	/*Invalid MARB "Call Back"			*/

/********************************************************
 *                                                      *
 * 		MARB ACTIVE SERVICE VALUES		*
 *							*
 * NOTE: See marb_struct.actv_srvc and                  *
 *           slot_struct.req_outstnd                    *
 *							*
 ********************************************************/

#define    MARB_ACTV_SRVC_REG_MODEM    ((unsigned char)0x01)	/*Reg. Modem	*/
#define    MARB_ACTV_SRVC_INIT_MODEM   ((unsigned char)0x02)	/*Init. Modem	*/
#define    MARB_ACTV_SRVC_START_BREAK  ((unsigned char)0x03)	/*Start Break   */
#define    MARB_ACTV_SRVC_STOP_BREAK   ((unsigned char)0x04)	/*Stop Break    */
#define    MARB_ACTV_SRVC_MPA_FLOW     ((unsigned char)0x05)	/*MPA Flow Cntrl*/
#define    MARB_ACTV_SRVC_FLUSH        ((unsigned char)0x06)	/*Flush/Reset	*/
#define    MARB_ACTV_SRVC_READ_UART    ((unsigned char)0x07)	/*Read UART reg	*/
#define    MARB_ACTV_SRVC_SET_LOOP     ((unsigned char)0x08)	/*Set Loop BK	*/
#define    MARB_ACTV_SRVC_CLR_LOOP     ((unsigned char)0x09)	/*Clr Loop BK	*/
#define    MARB_ACTV_SRVC_HARD_RESET   ((unsigned char)0x0A)	/*Hrd Rst Modem	*/
#define    MARB_ACTV_SRVC_MOD_SETS     ((unsigned char)0x0B)	/*Mod.Modem Setg*/

/********************************************************************************
 *                                                                              *
 *                 		FORWARD REFERENCES				*
 *                                                                              *
 ********************************************************************************/

struct slot_struct ;

/********************************************************************************
 *                                                                              *
 *                 SYSTEM FUNCTIONS IMPORT BLOCK DEFINITION			*
 *                                                                              *
 * Purpose: To define the Block that is used to import the addresses of System	*
 *	    functions required by the Modem Pool Software Servcies.		*
 *                                                                              *
 ********************************************************************************/

struct  sys_functs_struct {

/******************************************
 *     INTERRUPT MANAGEMENT FUNCTIONS	  *
 ******************************************/

/*000*/ unsigned long int (* blk_ints_fn) 		/*Address of function to*/	/*000*/
			    (				/*Blk/Disable interrupts*/
				void			/*	1st ENTITY IN	*/
			    ) ;

/*004*/	void		  (* rstr_ints_fn) 		/*Address of function to*/	/*004*/
			    (				/*Restore Interrupts	*/
				unsigned long int
			    ) ;


/******************************************
 *        LOCK MANAGEMENT FUNCTIONS	  *
 ******************************************/

/*008*/	void      *     (* init_lock_fn)		/*Address of function to*/	/*008*/
			    (				/*Initialize Lock struct*/
			       unsigned long int      * /*	   Functions	*/
			    ) ;

/*00C*/	unsigned char	(* attmpt_lock_fn)		/*Address of function   */	/*00C*/
			    (				/*that attempts to Lock	*/
				void 		     *	/*  0x00 = No Function	*/
			    ) ;
/*010*/	void		(* unlock_fn)			/*Address of function to*/	/*010*/
			    (				/*UnLock an object	*/
				void 		     *
			    ) ;

/*014*/											/*016*/
};

/********************************************************************************
 *                                                                              *
 *              MPA ASYNC SERVICE REQUEST BLOCK (MARB) DEFINITION               *
 *                                                                              *
 * Purpose: To define the MPA Async Service Request Block (MARB) that is used   *
 *         by the following MPA Services:                                       *
 *             - Register Modem Board                                           *
 *             - Initialize/Attach Modem Board                                  *
 *             - Modify UART/Modem Port Settings				*
 *             - Start Break                                                    *
 *             - Stop Break                                                     *
 *             - Set MPA-to-UART/Modem Hardware Flow Control                    *
 *	       - Flush Modem Pool FIFO's					*
 *	       - Hard Reset Modem Board						*
 *	       - Read UART Register						*
 *	       - Set Internal Loop Back						*
 *	       - Clear Internal Loop Back					*
 *                                                                              *
 * Notes: - Unused MARB fields must be zero on first CALL to an MPA Service.    *
 *         MARB must not be modified when issuing an "continuation" calls.      *
 ********************************************************************************/

struct  marb_struct {
/******************************************
 *                HEADER AREA             *
 *                                        *
 * - Always initialize to 0               *
 * - When "in use", fields should not be  *
 *   modified (i.e. fields are read only) *
 ******************************************/
/*000*/ unsigned char               in_use ;            /*"In use" flag:        */	/*000*/
                                                        /*  0x00 = MARB not be'g*/
                                                        /*         used by a MPA*/
                                                        /*         Service Func.*/
                                                        /*  0xFF = MARB being   */
                                                        /*         used by a MPA*/
                                                        /*         Service Func.*/
                                                        /*         ("call back" */
                                                        /*          outstanding)*/

/*001*/ unsigned char               actv_srvc ;         /*If non-waited request */	/*001*/
                                                        /*outstndg (in_use=0xFF)*/
                                                        /*then ID of Outstand'g */
                                                        /*function:             */
                                                        /*  0x00 = None         */
                                                        /*  0x01 = Reg. Modem   */
                                                        /*  0x02 = Init UART/Mdm*/
                                                        /*  0x03 = Start Break  */
                                                        /*  0x04 = Stop Break   */
                                                        /*  0x05 = MPA-UART Flow*/
                                                        /*  0x06 = Flush bufs	*/
							/*  0x07 = Read UART reg*/
							/*  0x08 = Set Loop BK	*/
							/*  0x09 = Clr Loop BK	*/
							/*  0x0A = Hrd Rst Modem*/
/******************************************
 *      INPUT PARAMETERS (MANDITORY)      *
 *                                        *
 * - Always initialize                    *
 * - When "in use", fields should not be  *
 *   modified (i.e. fields are read only) *
 ******************************************/

/*002*/ struct ssp_struct           *sspcb ;            /*SSP Control Block ptr */	/*002*/

/*008*/ unsigned char               slot_chan ;         /*Slot's chan number    */	/*006*/

/*009*/ unsigned char               req_type ;          /*Request type:         */	/*007*/
                                                        /*  0x00 = Waited i.e.  */
                                                        /*         ret when done*/
                                                        /*  0xFF = Non-waited,  */
                                                        /*         async request*/
/******************************************
 *     INPUT PARAMETERS (SUPPLEMENTAL)    *
 *                                        *
 * - Initialization dependent on Service  *
 * - When "in use", fields should not be  *
 *   modified (i.e. fields are read only) *
 ******************************************/

/*00A*/ unsigned char               *scratch_area ;     /*Scratch area pointer  */	/*008*/

/*010*/ unsigned char               sup_uchar0 ;					/*00C*/

/*011*/ unsigned char               sup_uchar1 ;					/*00D*/

/*012*/ unsigned char               sup_uchar2 ;					/*00E*/

/*013*/ unsigned char               sup_uchar3 ;					/*00F*/

/*014*/ unsigned short int          sup_ushort ;					/*010*/
/******************************************
 *          RETURN PARAMETER AREA         *
 *                                        *
 * - Don't Care how initialized           *
 * - When "in use", fields should not be  *
 *   modified (i.e. fields are read only) *
 ******************************************/

/*016*/ unsigned char               srvc_status ;       /*Service status        */	/*012*/
                                                        /*NOTE: Field coupled   */
                                                        /*     with "srvc_state"*/
                                                        /*     and therefore no */
                                                        /*     fields should be */
                                                        /*     intserted between*/

/*017*/ unsigned char               srvc_state ;        /*Service state:        */	/*013*/
                                                        /*  0x00 = Done         */
                                                        /*  0xFF = call back    */
/******************************************
 *    MPA ASYNC SRVC REQ INTERNAL AREA    *
 *                                        *
 * - Always initialize to 0               *
 * - DO NOT USE                           *
 ******************************************/

/*018*/ struct marb_struct          *signature ;        /*Ptr to struct base if */	/*014*/
                                                        /*"in use"              */

/*01E*/ void                        (* cont_funct)      /*Ptr to Handler	*/	/*018*/
                                        (               /*function to call      */
                                          unsigned long int,
					  struct marb_struct              *,
	      			     	  struct ssp_struct               *,
	      			          struct slot_struct              *,
	      			          volatile struct mpa_global_s    *,
				          volatile struct mpa_input_s     *,
	      			          volatile struct mpa_output_s    *
				        ) ;

/*022*/ unsigned char               gate ;		/*1st pass "gate"	*/      /*01C*/
							/*  0x00 = 1st use	*/
							/*  0xFF = Subsequnt use*/

/*023*/	unsigned char		    spare ;		/*Force even boundry	*/	/*01D*/



/*024*/											/*01E*/
};

/********************************************************************************
 *                                                                              *
 *                   SLOT CONTROL BLOCK (SLOTCB) DEFINITION                     *
 *                            (SLOT ARRAY MEMBER)				*
 *                                                                              *
 * Purpose: To define a Slot Control Block (or Slot Array member/element) in 	*
 *	   a MPA's Slot Array.                 					*
 *                                                                              *
 * Notes: - DRIVER'S MUST TREAT SLOTCB AS READ ONLY !!!!!			*
 *                                                                              *
 ********************************************************************************/

struct  slot_struct {


/******************************************
 *             Header Area                *
 ******************************************/
/*000*/ unsigned char             	in_use ;        /*"In use" flag:        */	/*000*/
                                                        /*  Always 0xFF         */

/*001*/ unsigned char        		alloc ;         /*Element initialized:  */	/*001*/
                                                        /*  0x00 = No           */
                                                        /*  0xFF = Yes, element */
                                                        /*         represents a */
                                                        /*         valid Slot   */

/*002*/ struct slot_struct       *	signature ;     /*Ptr to struct base.   */	/*002*/

/*008*/ volatile struct mpa_input_s     *input ;         /*Slot's Input reg ptr  */	/*006*/

/*00E*/ volatile struct mpa_output_s    *output ;        /*Slot's Output reg ptr */	/*00A*/

/*014*/ volatile struct icp_gbl_struct      * global ;	/*Slot's SSP Glogal regs*/	/*00E*/

/*01A*/	unsigned char		    chan_num ;		/*Slot's channel number	*/	/*012*/




/******************************************
 *          Slot SSP Save Area            *
 ******************************************/

/*01B*/ unsigned char               sv_cout_flow_config; /*Slot/chan's sv'd Flow*/	/*013*/
                                                         /*Cntrl Config (0x63)  */

/*01C*/ unsigned short int          sv_cout_cntrl_sig ; /*Slot/chan's saved     */	/*014*/
                                                        /*Control Sig reg (0x64)*/

/*01E*/ unsigned char               sv_cout_xon_1 ;     /*Slot/chan's saved XON */	/*016*/
                                                        /*reg (0x6E)            */
/******************************************
 * ASYNC/Non-Wait Request Management Area *
 ******************************************/

/*01F*/ unsigned char               req_outstnd ;       /*Driver non-waited req */	/*017*/
                                                        /*outstanding flag (from*/
                                                        /*marb_stuct.actv_srvc) */
                                                        /*  0x00 = No req outstd*/
                                                        /*  0x01 = Reg. Modem   */
                                                        /*  0x02 = Init UART/Mdm*/
                                                        /*  0x03 = Start Break  */
                                                        /*  0x04 = Stop Break   */
                                                        /*  0x05 = MPA-UART Flow*/
                                                        /*  0x06 = Flush bufs   */
							/*  0x07 = Read UART reg*/
							/*  0x08 = Set Loop BK	*/
							/*  0x09 = Clr Loop BK	*/
							/*  0x0A = Hrd Rst Modem*/
							/*  0x0B = Modfy Settngs*/

/*020*/ struct marb_struct          *actv_marb ;        /*Ptr "active" MARB     */	/*018*/

/*026*/ void                        (* cont_funct)	/*Ptr to next "call back"*/	/*01C*/
                                        (              	/*function. Note: Matches MARB*/
					  unsigned long int,
					  struct marb_struct              *,
	      			     	  struct ssp_struct               *,
	      			          struct slot_struct              *,
	      			          volatile struct mpa_global_s    *,
				          volatile struct mpa_input_s     *,
	      			          volatile struct mpa_output_s    *
				        ) ;

/*02A*/ void                        (* clean_up)                     /*Ptr to clean */  /*020*/
					(			     /*up function  */
                                          struct slot_struct      *, /*if Modem     */
                                          unsigned char		     /*removed. If  */
                                                                     /*req_outstnd =*/
                                                                     /*0, then no   */
                                                                     /*no clean up  */
					) ;

/*02E*/ unsigned short int          frame_ctr ;         /*Frame Counter used to */	/*024*/
                                                        /*calculate delay times */

/******************************************
 *          Slot & Board State Area       *
 ******************************************/

/*030*/	unsigned short int 	    power_state ;	/*Slot's Power State:	*/	/*026*/
							/*NOTE: VALID only when	*/
							/* 	mpa_hrdwre_rev	*/
							/*	(see mpa_struct)*/
							/*	GREATER THAN 0	*/
							/*  0x00 = Power OFF	*/
							/*  0xFF = Power ON	*/

/*032*/ unsigned char               slot_state ;        /*Slot's state:         */	/*028*/
                                                        /*  0x00 = Empty        */
                                                        /*  0x80 = Being reg'd  */
							/*  0xFE = Occupied by	*/
							/*	   Unknown brd	*/
                                                        /*  0xFF = Occupied by  */
							/*	   Modem/UART	*/

/*033*/ unsigned char               type ;              /*Board Type (valid only*/	/*029*/
							/*if slot_state = 0xFF)	*/
                                                        /*  0x00 = ISA Modem    */
                                                        /*  0x01 = Plug & Play  */

/*034*/ unsigned short int          io_base ;           /*Board base I/O Addr:	*/	/*02A*/
                                                        /*  0x2E8 (COM4)        */
                                                        /*  0x2F8 (COM2)        */
                                                        /*  0x3E8 (COM3)        */
                                                        /*  0x3F8 (COM1)        */

/*035*/ unsigned char               init_state ;        /*Board's UART Init     */	/*02C*/
                                                        /*(Open) state(valid 	*/
							/*if slot_state = 0xFF)	*/
                                                        /*  0x00 = Closed       */
                                                        /*  0xFF = Init'd/Opened*/

/*037*/	unsigned char		    read_toggle ;	/*Current setting of 	*/	/*02D*/
							/*slot's UART I/O Read	*/
							/*Toggle/strobe:	*/
							/*  0x00 or 0x08	*/

/******************************************
 *          UART/Modem Settings           *
 ******************************************/

/*038*/ unsigned char               flow_setting ;     	/*MPA<->UART/Modem Hrdwr*/	/*02E*/
                                                       	/*Flow Control setting. */
                                                       	/*Bit mapped as follows:*/
							/* BIT  DIRCTN  CTRL SIG*/
							/* ===  ======  ========*/
							/*  7     --    NOT USED*/
							/*  6     --    NOT USED*/
							/*  5     IN	  RI	*/
							/*  4     IN	  DSR	*/
							/*  3     IN      CTS	*/
							/*  2     IN      DCD	*/
							/*  1     OUT     RTS	*/
							/*  0     OUT     DTR	*/

/*039*/ unsigned char               num_bits ;          /*Brd's number bits (see*/	/*02F*/
                                                        /*UART LCR definition): */
                                                        /*  0x00 = 5            */
                                                        /*  0x01 = 6            */
                                                        /*  0x02 = 7            */
                                                        /*  0x03 = 8            */

/*03A*/ unsigned char               stop_bits ;         /*Brd's num Stop Bits   */	/*030*/
                                                        /*(See UART LCR def):   */
                                                        /*  0x00 = 1 stop bit   */
                                                        /*  0x04 = 2 stop bits  */

/*03B*/ unsigned char               parity ;            /*Brd's parity (see UART*/	/*031*/
                                                        /*LCR definition):      */
                                                        /*  0x00 = No parity    */
                                                        /*  0x08 = Odd parity   */
                                                        /*  0x18 = Even parity  */
                                                        /*  0x28 = Mark parity  */
                                                        /*  0x38 = Space parity */

/*03C*/ unsigned char               break_state ;       /*UART/Modem BREAK state*/	/*032*/
                                                        /*  0x00 = Break OFF    */
                                                        /*  0x40 = Break ON     */

/*03D*/ unsigned short int          baud ;              /*Brd's baud rate set'g */	/*033*/
                                                        /*  Value's as per UART */
                                                        /*  divisor latch       */

/*03F*/ unsigned char               loop_back ;         /*Internal Loop Back	*/      /*034*/
							/*State:		*/
							/*  0x00 = Loop back OFF*/
                                                        /*  0xFF = Loop back ON */

/******************************************
 *    DIAGNOSTIC/ADMINISTRATIVE AREA	  *
 ******************************************/

/*Plug and Play Probe area								  */
/*040*/	unsigned short int	    pnp_probe_state ;	/*Slot's PnP Probe state	  */ /*036*/
							/*  0x00 = PnP Probing not init'd */
							/*  0x80 = PnP Probing active	  */
							/*  0x8F = PnP Probe delay	  */
							/*  0xFF = PnP Board config'd	  */

/*042*/ void                        (* pnp_probe_funct)	/*Ptr to next PnP" Probe function */ /*038*/
                                        (              	/*Used only if slot_state=0(EMPTY)*/
							/*and pnp_probe_state != 0x00	  */
							/*(Probe init'd)		  */
	      			     	  struct ssp_struct *,
	      			          struct slot_struct *,
	      			          volatile struct mpa_global_s *,
				          volatile struct mpa_input_s *,
	      			          volatile struct mpa_output_s *
				        ) ;

/*046*/	unsigned int	    	    pnp_probe_parm ;	/*Variable used to send Initiation*/ /*03C*/
							/*Key sequence & PnP brd recogntn */
							/*delay				  */ 

/*04A*/	void                        (* pnp_clean_up)    /*Ptr to PnP clean up Function    */ /*040*/
					(
                                          struct slot_struct      *,  /*SLOTCB to clean up*/
                                          unsigned char		      /*Clean Up type:	  */
					) ;			      /*  0xFF = Abnormal */
							/*NOTE: Called only if slot_state */
							/*      EMPTY (0) and 		  */
							/*	pnp_probe_state	== 0x80	  */

/*04E*/ unsigned short int          pnp_frame_ctr ;     /*Svd Frame Ctr (used for delays) */ /*044*/

/*050*/ unsigned char               pnp_sv_cout_flow_config; /*Slot's sv'd Flow Ctrl Cnfig*/ /*046*/
                                                             /*(0x63)  		  	  */

/*051*/ unsigned char               pnp_sv_cout_xon_1 ;     /*Slot's saved XON (0x6E)	  */ /*047*/

/*052*/ unsigned short int          pnp_sv_cout_cntrl_sig ; /*Slot's svd Ctrl Sig (0x64)  */ /*048*/

/******************************************
 *           Driver Scratch Areas         *
 ******************************************/

/*054*/ unsigned char   rscratch[SLOT_SCRATCH_SZ] ;     /*Scratch Area defined  */	/*04A*/
                                                        /*by Driver when Modem  */
                                                        /*Board registered      */

/*05C*/ unsigned char   oscratch[SLOT_SCRATCH_SZ] ;    /*Scratch Area defined  */	/*052*/
                                                       /*by Driver when Modem  */
                                                       /*board Init'd/Attached */


/******************************************
 *           Driver MARB Area             *
 ******************************************/

/*064*/ struct marb_struct          marb ;              /*MPA Async Service Req */	/*05A*/
                                                        /*Block                 */

/*088*/											/*078*/
};

/********************************************************************************
 *                                                                              *
 *                   MPA/ENGINE CONTROL BLOCK (MPACB) DEFINITION                *
 *                                                                              *
 * Purpose: To define an MPA (engine) Control Block (member) in SSP Control 	*
 *	   Block's MPA/Engine Array.						*
 *                                                                              *
 * Notes: - DRIVER'S MUST TREAT MPACB AS READ ONLY !!!!!			*
 *                                                                              *
 ********************************************************************************/

struct  mpa_struct  {

/**********************************************
 *                  Header                    *
 *                                            *
 * Common for all Engines within a single MPA *
 *                                            *
 **********************************************/
/*000*/ unsigned char               base_chan ;         /*MPA's Ring Position	*/	/*000*/
                                                        /*(LMX Channel #):      */
                                                        /*  0, 16, 32, 48, or   */
                                                        /*  0xFF = MPA Eng not  */
                                                        /*         exists at    */
                                                        /*         position on  */
                                                        /*         Ring i.e.    */
                                                        /*         element not  */
                                                        /*         allocated    */

/*001*/ unsigned char               mpa_num_engs ;      /*MPA's Number of Engs: */	/*001*/
                                                        /*  1 = 1 Eng./16 slots */
                                                        /*  2 = 2 Engs/8 slots  */
                                                        /*  4 = 4 Engs/4 slots */
                                                        /*Note: Defines number  */
                                                        /*      of Slot elements*/
                                                        /*      in Slot array   */
                                                        /*      used by Engine  */

/*002*/ volatile struct mpa_input_s *input_base ;       /*Pointer to MPA's 1st  */	/*002*/
                                                        /*set of Input registers*/

/*008*/ struct slot_struct          *slot_array_hd ;    /*Ptr to head of Slot   */	/*006*/
                                                        /*array associated with */
                                                        /*MPA                   */

/*00E*/ unsigned char               mpa_hrdwre_rev ;    /*MPA Hardware Rev LVL	*/	/*00A*/
							/*  0x00 = 1st Release	*/
							/*  0x01 = Hot Swap via	*/
							/*	   PWR SWTCH Rel*/
/**********************************************
 *                                            *
 *           Engine Information               *
 *                                            *
 **********************************************/

/*00F*/ unsigned char               eng_num ;           /*Engine number:        */	/*00B*/
                                                        /*  00 = 1st MPA Engine */
                                                        /*  01 = 2nd MPA Engine */
                                                        /*  02 = 3rd MPA Engine */
                                                        /*  03 = 4th MPA Engine */

/*010*/ struct slot_struct          *eng_head ;         /*Ptr to Engine's 1st   */	/*00C*/
                                                        /*Slot element in Slot  */
                                                        /*array                 */

/*016*/											/*010*/
};

/********************************************************************************
 *                                                                              *
 *                      SSP CONTROL BLOCK (SSPCB) DEFINITION                    *
 *                                                                              *
 * Purpose: To define Control Block initialized when "Register SSP" function is *
 *         is called.  Control Block is "Head" of Data for all MPAs associated  *
 *         associated with a SSP.  There is an instance of this Control Block   *
 *         for each registered SSP                                   		*
 *                                                                              *
 * Notes: - DRIVER'S MUST TREAT SSPCB AS READ ONLY !!!!!			*
 *                                                                              *
 ********************************************************************************/

struct  ssp_struct  {

/***********************
 *       Header        *
 ***********************/
/*000*/ unsigned char                  in_use ;         /*"In use" flag:        */	/*000*/
                                                        /*  Always 0xFF         */

/*001*/ unsigned char                  rev ;            /*MPA Software rev #   	*/	/*001*/
                                                        /*Binary value          */

/*002*/ unsigned char                  admin_rev;  	/*RAMP Admin/Diag 	*/	/*002*/
							/*Facilities software	*/
							/*Rev - Binary value	*/

/*003*/ unsigned char                  hdr_dummy ;  	/*Dummy, force word bndry*/	/*003*/

/*004*/ struct ssp_struct              *signature ;     /*Ptr to struct base.   */	/*004*/
/***********************
 * ADMIN & DIAG AREA   *
 ***********************/

/*00A*/ struct ssp_struct              *sspcb_link ;	/*Link to nxt Registered*/	/*008*/
							/*SSPCB:		*/
							/*  0x00 = End of Chain	*/

/*010*/	void     		       *(* map_fn)	/*Pointer to Driver FN	*/	/*00C*/
					   (		/*to MAP SSP Regs:	*/	/*R32*/
							/*  0x00 = No MAP FN,	*/
					     void      *
					   );

/*014*/	void     		       *map_parm ;	/*MAP SSP Regs FN Parm	*/	/*010*/

/*01A*/	void 		               (* unmap_fn)	/*Pointer to Driver FN	*/	/*014*/
					   (		/*to UNMAP SSP Regs:	*/
							/*  0x00 = No UNMAP FN,	*/
					     void      *
					   );

/*01E*/	unsigned short int	       pnp_probe_ena ;	/*SSP's Plug and Play	*/	/*018*/
							/*Probe Enabled Flag:	*/
							/*  0x00 = NO		*/
							/*  0xFF = YES		*/
/***********************
 * SSP Global reg area *
 ***********************/

/*020*/ volatile struct mpa_global_s    *global ;        /*SSP's Global reg      */	/*01A*/

/*026*/ unsigned char			sv_sp_mul_msk ;	/*Sv'd SSP Speed mult msk*/	/*01E*/

/*027*/	unsigned char			sv_sp_mul_set ;	/*Sv'd speed mult set'g	*/	/*01F*/



/***********************
 *      MPA ARRAY      *
 ***********************/

/*028*/ struct mpa_struct              mpa[ENGS_MPA] ;  /*MPA/Eng Array, defines*/	/*020*/
                                                        /*MPAs and Engs on SSP's*/
                                                        /*Ring.  Array ordered  */
                                                        /*by LMX position       */

/*080*/											/*060*/

};

/********************************************************************************
 *                                                                              *
 *   		ADMINISTRATIVE & DIAGNOSTIC SERVICES HEADER DEFINITION		*
 *										*
 * Purpose: To define ADMINISTRATIVE & DIAGNOSTIC SERVICES HEADER that is used	*
 *	    to manage SSPCBs by Administrative and Diagnostic facilities	*
 *										*
 * Notes: - DRIVER'S MUST TREAT SSPCB AS READ ONLY !!!!!			*
 *                                                                              *
 ********************************************************************************/

struct	admin_diag_hdr_struct	{

/******************************
 *    SYSTEM FUNCTION AREA    *
 ******************************/

/*000*/	unsigned short int		init_flag ;	/*Flag indicating if 	*/	/*000*/
							/*System Function ptrs	*/
							/*have been estab'd	*/
							/*  0x0000 = No		*/
							/*  0xFFFF = Yes	*/

/*002*/	struct	sys_functs_struct	os_functs ;					/*002*/


/************************
 * FUNCTIONAL CHAINS	*
 ************************/

	/*SSPCB LIST HEAD AREA							*/

/*016*/	void     			*sspcb_lst_lck_hndl; /*SSPCB LIST's Lck Hndle*/	/*016*/

/*01C*/	unsigned long int		sspcb_lock ;	 /*Primative Lock	     */	/*01A*/

/*020*/	struct	ssp_struct     		*sspcb_link ;	/*Chain of Registered SSPs:  */	/*01E*/
							/*  0x00 = None		     */
/****************************
 * Plug and Play Probe area *
 ****************************/

/*026*/	unsigned short int		pnp_probe ;	/*Flag indicating that PnP   */	/*022*/
							/*Probe has been enable on at*/
							/*least one SSP:	     */
							/*  0x00 = NO		     */
							/*  0xFF = YES, PnP enabled  */
							/*	   on at least 1 SSP */

/*028*/	struct	ssp_struct     		*pnp_sspcb ;	/*Ptr to next SSPCB to do PNP*/	/*024*/
							/*Probing (valid only if     */
							/*pnp_probe = 0xFF)	     */

/*02E*/	unsigned int			pnp_eng_index ;	/*Index to next MPA/Engine to*/	/*028*/
							/*do PnP Probing (valid only */
							/*if pnp_probe = 0xFF	     */

/*032*/											/*02C*/
} ;

/**********************************************************
 *                                                        *
 *  MPA SERVICE REQUEST FUNCTION DECLARATIONS/PROTOTYPES  *
 *                                                        *
 **********************************************************/


/*MPA ADAPTER Services                                                         		*/

extern	unsigned short int	mpa_srvc_id_functs (
						    struct sys_functs_struct      *
						   ) ;

extern	unsigned short int	mpa_srvc_reg_ssp (
						  struct ssp_struct             *,
	                               	          volatile struct mpa_global_s           *,
						  void			        * (*)(void      *),
						  void			        *,
						  void			          (*)(void      *)
						 ) ;

extern	unsigned short int	mpa_srvc_reg_mpa (
						  struct ssp_struct             *,
                          			  unsigned char,
					  	  struct slot_struct            *,
					  	  volatile struct mpa_input_s   *
						 ) ;

extern unsigned short int	mpa_srvc_dereg_mpa (
						    struct ssp_struct           *,
                                	  	    unsigned char,
					  	    struct slot_struct          *        *
						   ) ;


/*MPA SLOT Services                                                            		*/

extern unsigned short int	mpa_srvc_reg_modem     (
							struct marb_struct        *
						       ) ;

extern unsigned short int	mpa_srvc_dereg_modem   (
							struct marb_struct        *
						       ) ;

extern unsigned short int	mpa_srvc_slot_status   (
							struct marb_struct        *,
						        struct slot_struct        *        * 
						       ) ;

extern unsigned short int	mpa_srvc_init_modem    (
							struct marb_struct        *
						       ) ;

extern unsigned short int	mpa_srvc_modify_settings (
							  struct marb_struct        *
							 ) ;

extern unsigned short int	mpa_srvc_set_loop_back (
							struct marb_struct        *
						       ) ;

extern unsigned short int	mpa_srvc_clr_loop_back (
							struct marb_struct        *
						       ) ;

extern unsigned short int	mpa_srvc_close_modem   (
							struct marb_struct        *
						       ) ;
extern unsigned short int	mpa_srvc_can_marb      (
							struct ssp_struct         *,
                                	  	        unsigned char,
						        struct marb_struct        *        * 
						       ) ;

extern unsigned short int	mpa_srvc_flush (
						struct marb_struct               *
					       ) ;

extern unsigned short int	mpa_srvc_hard_reset (
						     struct marb_struct          *
						    ) ;

extern unsigned short int	mpa_srvc_input_error (
						      struct marb_struct        *,
						      unsigned char             *
						     ) ;

extern unsigned short int	mpa_srvc_start_break (
						      struct marb_struct        *
						     ) ;

extern unsigned short int	mpa_srvc_stop_break (
						     struct marb_struct         *
						    ) ;

extern unsigned short int	mpa_srvc_mpa_flow (
						   struct marb_struct           *
						  ) ;

extern unsigned short int	mpa_srvc_read_uart (
						    struct marb_struct           *
						   ) ;





/*MPA ADMINISTRATIVE & DIAGNOSTIC Services					*/

extern unsigned short int	mpa_srvc_diag_poll (
						    void
						   ) ;

extern unsigned short int	mpa_srvc_pnp_probe_cntrl (
							  struct ssp_struct      *,
	                               	          	  unsigned char		  
							 ) ;

