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

static char Copyright[82] = 
"Copyright (c) 1989-2007 Equinox Systems Inc. All Rights Reserved.";

static char Version[82] =
"Equinox SuperSerial Technology Device Driver";

#define MAXSLOTS	8
#define MAXISAIOPORTS	16
#define MAXLMXMPC       16              /* max RS232/422 channels per lmx */

#define MAXCHNL_ANX	2

#define NSSPS		2		/* max SSP64s per adapter */
#define NSSP4S		4		/* max SSP4s per adapter */
#ifndef MAXBRDCHNL
#define MAXBRDCHNL	128		/* max channels per board */
#endif
#ifndef MAXICPCHNL
#define MAXICPCHNL	64
#endif
#ifndef NMEGAPORT
#define NMEGAPORT	8		/* maximum possible megaport boards */
#endif
#define MAXCHNL         (MAXBRDCHNL * NMEGAPORT) /* 1024 */

#define MPRAMPTIME0      ((HZ*40)/1000)	/* 40ms. - poll for eqn_ramp_admin() */
#define RAMP_FAST_TIME   ((HZ*20)/1000)	/* 20ms. - poll when doing call backs */
#define MPA_MAX_RETRY       7

#define REG_MODEM_INDEX          0
#define SET_LOOP_BACK_INDEX      1
#define CLR_LOOP_BACK_INDEX      2
#define START_BREAK_INDEX        3
#define STOP_BREAK_INDEX         4
#define INIT_MODEM_INDEX         5
#define MODIFY_SETTINGS_INDEX    6
#define HARD_RESET_INDEX         7
#define MAX_RAMP_INDEX           8

typedef unsigned long paddr_t;
#define MPUNCTL(dev)	(dev & 0x3ff)		/* strip control bits */
#define MPBOARD(i)	((i) / NCHAN_BRD)	/* minor device to board */
#define MPCHAN(i)	((i) % NCHAN_BRD)	/* minor device to channel */ 
#define ulong_t unsigned long
#define	uint_t	unsigned int
#define ushort_t unsigned short
#define uchar_t unsigned char
#ifndef CTSFLOW
#define CTSFLOW 0x2000
#define RTSFLOW 0x4000
#endif

/* PCI defines */

#define EQUINOX_PCI_ID                 0x113f /* Vendor ID */
#define PCI_CLASS_COMM_CONTROLLER      0x07  /* Comm. Controller Class */
#define PCI_SUBCLASS_SERIAL_CONTROLLER 0x00  /* Serial Controller Sub-Class */
#define PCI_BASE_ADDR_MASK             0xFFFFFFF0 /* Mask off bits 0-3 */

/* Device IDs */
#define SST128_PCI_DEV_ID 0x10
#define SST64_PCI_DEV_ID  0x08
#define SST8F_PCI_DEV_ID  0x90
#define SST8P_PCI_DEV_ID  0x94
#define SST4_PCI_DEV_ID   0x88
#define SST2_PCI_DEV_ID   0xA8

/* PCI Register Offsets */

#define PCI_DEVICE_ID_REG    0x02  /* Device ID Register offset */
#define PCI_COMMAND_REG      0x04  /* Command Register offset */
#define PCI_REV_ID_REG       0x08  /* Revision Register offset */
#define PCI_CC_PROGRAM_REG   0x09  /* Program Interface Class Code Reg offset */
#define PCI_CC_SUBCLASS_REG  0x0A  /* Sub-Class Class Code Register offset */
#define PCI_CC_CLASS_REG     0x0B  /* Class Class Code Register offset */
#define PCI_HEADER_TYPE_REG  0x0E  /* Header Type Register offset */
#define PCI_BIST_REG         0x0F  /* Built In Self Test Register offset */
#define PCI_BASE_ADDRESS_REG 0x10  /* Base Register offset */

/* Memory Space Control bit (bit 1 @ offset 0x04) in I/O Configuration 
   Space's Command Reg. */
#define PCI_MSC 0x02 

typedef struct megaport *mpaddr_t;
typedef volatile union  input_regs_u *icpiaddr_t;
typedef volatile union  output_regs_u *icpoaddr_t;
typedef volatile union  global_regs_u *icpgaddr_t;
typedef volatile union  bank_regs_u *icpbaddr_t;
typedef volatile union  queue_regs_u *icpqaddr_t;

struct ccb_struct *ccb;

struct megaport {					/* 16KB */
	volatile union  input_regs_u mp_icpi[64];		/* 8KB */
	volatile union  output_regs_u mp_icpo[64];		/* 8KB */
};

/* session control struct */

struct sess {
        char    fill_0[20];
        uchar_t col_cnt;                /* column counter */
        uchar_t sess_cntr;              /* session control reg */        
        ushort_t sess_char_cnt;         /* opost expension chars */   
        char    fill_1[8];
};

struct sessions {
        struct sess cout_sess[256];
};
                
/* defines gicp_bus_ctrl */

#define CPU_BUS_8	0
#define CPU_BUS_16	1
#define CPU_BUS_32	2
#define DRAM_24		0
#define DRAM_40		4
#define STERNG		8
#define SWAPNG		0x10
#define DMARQ_2		0x20
#define GTIMER_EN	0x40

/* defines gicp_initiate */

#define ICP_2		1
#define RNG_CLK_ON	2
#define ICP_PRAM_WR	4
#define DMA_EN		8

/* defines gicp_scan_spd */

#define A_16_PORTS	0
#define A_8_PORTS	1
#define A_4_PORTS	2
#define A_2_PORTS	3

#define B_16_PORTS	0
#define B_8_PORTS	4
#define B_4_PORTS	8
#define B_2_PORTS	0xc

#define C_16_PORTS	0
#define C_8_PORTS	0x10
#define C_4_PORTS	0x20
#define C_2_PORTS	0x30

#define D_16_PORTS	0
#define D_8_PORTS	0x40
#define D_4_PORTS	0x80
#define D_2_PORTS	0xc0

/* gicp_tmr_size res 520.8 us when set to 0xff and 133.3 msec when set to 00 
   (256 - Desired tick size / 520.8 us) + 1 */

/* gicp_tmr_count (256 - Desired time / tick size) + 1; 
   520.8 usec <= Timer Interval <= 34.13 seconds */

/* gicp_watchdog */

#define WDOG_DIS	0
#define WDOG_1_SEC	0x20
#define WDOG_2_SEC	0x40
#define WDOG_4_SEC	0x60
#define WDOG_8_SEC	0x80
#define WDOG_16_SEC	0xa0
#define WDOG_32_SEC	0xc0

/* gicp_attn */

#define GATTN_RX	1
#define GATTN_TX	2
#define RNG_FAIL	4
#define WDOG_EXP	8

/* current chan */

#define CHAN_NUM	0xf
#define LMX_NUM		0x30

/* cin_bank_signals */

#define CIN_BREAK	1
#define CIN_DCD         2
#define CIN_CTS         4
#define CIN_DSR         8
#define CIN_RI          0x10
#define LMX_ONLN        0x20    
#define LMX_RMT         0x40    /* if 0 then local lmx or bridge, else mux */
#define LMX_PRESENT	0x80    /* channel's LMX present */
#define ICP_OUT_2	0x100
#define LMX_OUT_2	0x200
#define AMI_LNK_ON      0x400   /* if a bridge link to a mux */
#define XPRNCY_PEND	0x800   /* waiting for link with mux */
#define LMX_LCK		0x1000  /* ring locked */
#define AMI_CNFG	0x2000  /* for bridge and mux is 1, for panel is 0 */

/* if LMX_ONLN is 1 then we can use all other lmx bits for info */

/* cin_bank_events - status of events that changed */

#define EV_AUTO_BAUD	1
#define EV_DCD_CNG	2
#define EV_CTS_CNG	4
#define EV_DSR_CNG	8
#define EV_RI_CNG	0x10
#define EV_BREAK_CNG	0x20
#define EV_LMX_CNG	0x40
#define EV_PAR_ERR	0x80
#define EV_FRM_ERR	0x100
#define EV_CHAR_LOOKUP	0x200
#define EV_VTIME	0x400
#define EV_VMIN		0x800
#define EV_OVERRUN	0x1000
#define EV_DMA_FAIL	0x2000
#define EV_REG_UPDT	0x4000
#define CHAN_ATTN_SET	0x8000
 
/* cin_char_cntrl */

#define CS_5		0		/* not valid if one of four top baud */
#define CS_6		1
#define CS_7		2
#define CS_8		3
#define PARITY_ON	4
#define PAR_ODD		0
#define PAR_EVEN	8
#define PAR_MARK	0x10
#define PAR_SPACE	0x18
#define IGN_BAD_CHAR	0x20
#define IGN_BRK_NULL	0x40
#define EN_CHAR_LOOKUP	0x80
#define NO_CMP_ERR	0x100
#define EN_LITNXT	0x200
#define EN_IXANY	0x400
#define EN_DBL_FLW	0x800
#define EN_DNS_FLW	0x1000
#define EN_ISTRIP	0x2000
#define EN_XOFF		0x4000
#define EN_XON		0x8000
  
/* input locks */

#define DIS_BANK_A	1
#define DIS_BANK_B	2
#define DIS_IN_STAT	4
#define DIS_IBAND_FLW	8
#define DIS_VTIME_WR	0x10
#define DIS_AUTOBAUD	0x20
#define DIS_SLF_CLRFLG	0x40
#define DIS_DMA_WR	0x80
#define SEL_BANK_A	2
#define SEL_BANK_B	1
#define SEL_NO_BANK	3

/* input queue control */

#define Q_SIZE_256	0
#define Q_SIZE_512	1
#define Q_SIZE_1K	2
#define Q_SIZE_2K	3
#define Q_SIZE_4K	4
#define Q_SIZE_8K	5	
#define Q_SIZE_16K	6
#define Q_SIZE_32K	7
#define Q_SIZE_64K	8

#define TAIL_PTR_B	0x10		/* set tail ptr b wr/rd */
#define EN_IXOFF_SVC	0x20
#define ST_IN_TMR       0x40            /* toggle input char timer */
#define CIN_TMR_MODE	0x80

/* cin_attn_ena */

#define ENA_AUTO_BUAD	1
#define ENA_DCD_CNG	2
#define ENA_CTS_CNG	4
#define ENA_DSR_CNG	8
#define ENA_RI_CNG	0x10
#define ENA_BREAK_CNG	0x20
#define ENA_LMX_CNG	0x40
#define ENA_PAR_ERR	0x80
#define ENA_FRM_ERR	0x100
#define ENA_CHAR_LOOKUP	0x200
#define ENA_VTIME	0x400
#define ENA_VMIN	0x800
#define ENA_IXOFF_SVS	0x1000
#define ENA_DMA_FAIL	0x2000
#define ENA_REG_UPDT	0x4000
#define EN_REG_UPDT_EV	0x8000

/* input signal flow control - cin_susp_output_lmx */

#define LMX_NOT_CONN	1
#define DCD_OFF		2
#define CTS_OFF		4
#define DSR_OFF		8
#define RI_OFF		0x10
#define MUX_NOT_CONN	0x20
#define LMX_OFF_LINE	0x40
#define CHAN_DAT_LNBK	0x80            /* chan data line non broken */

/* input signal flow control - cin_susp_output_sig */

#define DCD_ON		2
#define CTS_ON		4
#define DSR_ON		8
#define RI_ON		0x10
#define CHAN_DAT_LBRK	0x80            /* chan data line broken */

/* self clearing input flags - read only by cpu */

#define GOD_FRM_RCV	1
#define XON_1_RCV	2
#define XOFF_1_RCV	4
#define PRV_CH_LNXT	8
#define NO_CMP_ON_ERR	0x10
#define DAT_OUT_SUSP	0x20
#define IN_BUF_OVFL	0x40
#define BANK_B_ACT	0x80

/* output defines */

/* data queue/buffer type & size */

#define QSZ_256         0
#define QSZ_512         1
#define QSZ_1K          2
#define QSZ_2K          3
#define QSZ_4K          4
#define QSZ_8K          5
#define QSZ_16K         6
#define QSZ_32K         7
#define EN_CIRC_Q       8
#define EN_TX_LOW       0x10
#define EN_TX_EMPTY     0x20

/* queue output state */

#define	CMDQ_64		0
#define CMDQ_128	1
#define CMDQ_256	2
#define CMDQ_512	3
#define CMDQ_1K		4
#define CMDQ_2K		5
#define CMDQ_4K		6
#define CMDQ_8K		7
#define CMDQ_HALT	8
#define CMDQ_SND	0x10
#define CMDQ_EXEC	0x20
#define CMDQ_WAIT       0x40
#define CMDQ_CONT_SND   0x80            /* cpu write use lock 6 */

/* session control register */

#define SCR_EN		1
#define SCR_TABX	2
#define SCR_NLCR	4
#define SCR_CRNL	8
#define SCR_NOCR	0x10
#define SCR_NLRET	0x20

/* output event register */

#define EV_CPU_REQ_DN	1
#define EV_Q0_MRKR	2
#define EV_Q0_HLT_CMD	4
#define EV_Q1_MRKR	8
#define EV_Q1_HLT_CMD	0x10
#define EV_FLC_TMO	0x20
#define EV_CMD_PRC_INH  0x40
#define EV_OUT_TMR_EXP	0x80
#define EV_ILG_REQ	0x100
#define EV_TX_EMPTY_Q0  0x200
#define EV_TX_LOW_Q0    0x400
#define EV_TX_EMPTY_Q1  0x800
#define EV_TX_LOW_Q1    0x1000
#define EV_REG_UPDT	0x4000
#define EV_ATTN_SET	0x8000

/* output status register */

#define TXSR_Q1_ACT	1
#define TXSR_SND_BRK	2
#define TXSR_UNBRK_SEQ	4
#define TXSR_OPOST_INH	8
#define TXSR_EV_B_ACT	0x10
#define TXSR_CMDPRC_INH	0x20
#define TXSR_SND_HALT   0x40            /* Transistion to halt from send data */

/* output char control */

#define TX_CS5		0
#define TX_CS6		1
#define TX_CS7		2
#define TX_CS8		3
#define TX_PARENB	4
#define TX_PAR_ODD	0
#define TX_PAR_EVEN	8
#define TX_PAR_MARK	0x10
#define TX_PAR_SPACE	0x18
#define TX_2STPB	0x20

/* xon/xoff config */

#define TX_XOFF_RDN	1
#define TX_XON_XOFF_EN	2
#define TX_XON_DBL	4
#define TX_SND_XON      8
#define TX_TGL_XON_XOFF 0x10
#define	TX_BREAK_ON	0x20
#define TX_XTRA_DMA     0x40
#define TX_SUSP         0x80

/* output control signals */

#define TX_DTR		1
#define TX_RTS		2
#define TX_CNT_2	4
#define TX_CNT_3	8
#define TX_HFC_DTR	0x10
#define TX_HFC_RTS	0x20
#define TX_HFC_2	0x40
#define TX_HFC_3	0x80

#define TX_LED		0x100
#define TX_SND_CTRL_TG	0x200
#define TX_TRGT_LMX_MUX	0x8000

/* lmx command register */

#define TX_INTR_LB	1
#define TX_XTRN_LB	2
#define TX_LMX_ONLN	8
#define TX_BRDG_XPRNT	0x10
#define TX_LCK_LMX	0x20
#define TX_LMX_CMD_EN	0x40
#define TX_TRGT_MUX	0x80

/* output attention enable */

#define ENA_CPU_REQ_DN	1
#define ENA_Q0_MRKR	2
#define ENA_Q0_HLT_CMD	4
#define ENA_Q1_MRKR	8
#define ENA_Q1_HLT_CMD	0x10
#define ENA_FLC_TMO	0x20
#define ENA_CMD_PRC_INH 0x40
#define ENA_OUT_TMR_EXP	0x80
#define ENA_ILG_REQ	0x100
#define ENA_TX_EMPTY_Q0 0x200
#define ENA_TX_LOW_Q0   0x400
#define ENA_TX_EMPTY_Q1 0x800
#define ENA_TX_LOW_Q1   0x1000
#define ENA_REG_UPDT	0x4000
#define EN_REG_UPDT_EV	0x8000

/* cpu request register */

#define CPU_HLT_REQ	1
#define CPU_START_REQ	2
#define CPU_SND_REQ	4
#define CPU_SWTCH_Q	8
#define CPU_TMR_TGL     0x10

/* output lock register */

#define LCK_EVT_A	1
#define LCK_EVT_B	2
#define LCK_FLW_TMR	4
#define LCK_MISC_WR	8
#define LCK_OUT_TMR	0x10
#define LCK_STATUS	0x20
#define LCK_Q_ACT	0x40
#define DIS_CMD_PROC	0x80

/* session control - opost */

#define MOPOST           1
#define MTABX            2
#define MONLCR           4
#define MOCRNL           8
#define MONOCR           0x10
#define MONLRET          0x20

#define NCHARS  blk_sz
#define LMASK   ((output_tags_sz -1) << 2)
#define UMASK   (((short)output_tags << 2) & ~LMASK)
#define ZMASK   (UMASK|LMASK)

/* globals defines */

#define gicp_bus	icpg->ssp.gicp_bus_ctrl
#define Gicp_rev	icpg->ssp.gicp_rev	/* COMMON */
#define Gicp_initiate	icpg->ssp.gicp_initiate
#define Gicp_scan_spd	icpg->ssp.gicp_scan_spd
#define Gicp_tmr_size	icpg->ssp.gicp_tmr_size
#define Gicp_tmr_count	icpg->ssp.gicp_tmr_count
#define Gicp_watchdog	icpg->ssp.gicp_watchdog
#define i_gicp_attn	icpg->ssp.gicp_attn
#define Gicp_chnl	icpg->ssp.gicp_chnl
#define Gicp_fram_ctr	icpg->ssp.gicp_frame_ctr
#define gicp_rx_attn	icpg->ssp.gicp_rcv_attn
#define gicp_tx_attn	icpg->ssp.gicp_xmit_attn


/* input struct defines */

#define rx_base		icpi->ssp.cin_dma_base
#define rx_baud		icpi->ssp.cin_baud	/* COMMON */
#define rx_locks	icpi->ssp.cin_locks	/* COMMON */
#define rx_xon		icpi->ssp.cin_xon_1	/* COMMON */
#define rx_xoff		icpi->ssp.cin_xoff_1	/* COMMON */
#define rx_param	icpi->ssp.cin_char_ctrl	/* COMMON */
#define rx_q_ctrl	icpi->ssp.cin_q_ctrl	/* COMMON */
#define rx_vtime_scale	icpi->ssp.cin_tmr_preset_size	/* COMMON */
#define rx_vtime	icpi->ssp.cin_tmr_preset_count	/* COMMON */
#define rx_int_vtime_scale icpi->ssp.cin_tmr_size	/* COMMON */
#define rx_int_vtime	icpi->ssp.cin_char_tmr_remain	/* COMMON */
#define rx_cie		icpi->ssp.cin_attn_ena	/* COMMON */
#define rx_hiwat	icpi->ssp.cin_overload_lvl	/* COMMON */
#define rx_suspo_lmx	icpi->ssp.cin_susp_output_lmx	/* COMMON */
#define rx_suspo_sig	icpi->ssp.cin_susp_output_sig	/* COMMON */
#define rx_vmin		icpi->ssp.cin_min_char_lvl	/* COMMON */
#define rx_lokup_tbl	icpi->ssp.cin_lookup_tbl	/* COMMON */
#define rx_csr		icpi->ssp.cin_int_flags	/* COMMON */
#define rx_ixoff_rcv	icpi->ssp.cin_inband_flow_ctrl	/* COMMON */
#define rx_tdm_early	icpi->ssp.cin_tdm_early

/* bank defines - needs to load bank ptr with current bank address */
#define rx_next		icpb->ssp.bank_dma_ptr
#define rx_fifo_cnt	icpb->ssp.bank_fifo_lvl
#define rx_ctrl_sig	icpb->ssp.bank_sigs
#define rx_tags_l	icpb->ssp.bank_tags_l
#define rx_tags_h	icpb->ssp.bank_tags_h
#define rx_rcv_cnt	icpb->ssp.bank_num_chars
#define rx_events	icpb->ssp.bank_events
#define rx_fifo		icpb->ssp.bank_fifo

/* output defines */
/************************************************************************/
/* output queue */
/*
icpg = (icpgaddr_t)ccb->ccb_output_regs;
icpo = (icpoaddr_t)ccb->ccb_output_regs;
*/
#define tx_csr		icpo->ssp.cout_status
#define tx_ses_col	icpo->ssp.cout_ses_col_a
#define tx_baud		icpo->ssp.cout_baud
#define tx_baud_ctr     icpo->ssp.cout_int_baud_ctr
#define tx_char_fmt	icpo->ssp.cout_char_fmt
#define tx_flow_config	icpo->ssp.cout_flow_cfg
#define tx_ctrl_sig	icpo->ssp.cout_ctrl_sigs
#define tx_lmx_cmd	icpo->ssp.cout_lmx_cmd
#define tx_cie		icpo->ssp.cout_attn_ena
#define tx_xoff		icpo->ssp.cout_xoff_1
#define tx_xon		icpo->ssp.cout_xon_1
#define tx_cpu_reg	icpo->ssp.cout_cpu_req
#define tx_svd_togls	icpo->ssp.cout_int_save_togl
#define tx_lck_ctrl	icpo->ssp.cout_lock_ctrl
#define tx_timer	icpo->ssp.cout_timer
#define tx_dma_ptr	icpo->ssp.cout_dma_ptr
#define tx_dma_base	icpo->ssp.cout_dma_base
#define tx_dma_stat     icpo->ssp.cout_dma_stat_save
#define tx_susp_timeo	icpo->ssp.cout_flow_timeout

/*
icpq = (tx_csr & TXSR_Q1_ACT) ? &icpo->ssp.cout_cout_q1 : &icpo->ssp.cout_q0;
*/
#define Q_cmd_ptr	icpq->ssp.q_cmd_ptr
#define Q_cmd_base	icpq->ssp.q_cmd_base
#define Q_data_ptr	icpq->ssp.q_data_ptr
#define Q_data_base	icpq->ssp.q_data_base
#define q_type          icpq->ssp.q_size
#define Q_data_count	icpq->ssp.q_data_count
#define Q_block_count	icpq->ssp.q_block_count
#define Q_out_state	icpq->ssp.q_out_state


#define get_tail() \
{ \
	if(icpi->ssp.cin_q_ctrl & TAIL_PTR_B) \
	  return (icpi->ssp.cin_tail_ptr_b); \
	else \
	  return (icpi->ssp.cin_tail_ptr_a); \
}


#define rx_tail(val) \
{ \
        if (icpi->ssp.cin_q_ctrl & TAIL_PTR_B) \
           icpi->ssp.cin_tail_ptr_a = (val); \
	else  \
	   icpi->ssp.cin_tail_ptr_b = (val); \
       	rx_q_ctrl ^= TAIL_PTR_B; \
}           


#define FREEZ_BANK(mpc) \
{ \
	ushort_t cie = CHAN_ATTN_SET | rx_cie; \
	int frztimeo = 0; \
unsigned  char lcks = 0; \
	rx_cie = 0; \
    if( (rx_locks & 1) == 1) { /* Bank A is locked */\
       icpb = &icpi->ssp.cin_bank_b; \
	lcks = 0x80; \
     }  \
     else  \
       icpb = &icpi->ssp.cin_bank_a; \
     if (!(icpb->ssp.bank_events & EV_REG_UPDT)) { \
        while ((icpi->ssp.cin_int_flags & 0x80) != lcks) \
           if (++frztimeo > 8000) break; \
     }  \
     mpc->mpc_icpb = icpb; \
     rx_locks ^= 3; /* Flip to the other bank */\
	cur_chnl_sync(mpc); \
	 mpc->mpc_cin_events |= rx_events; \
	 rx_events = 0; \
	 rx_cie = cie; \
} 


#define TX_EVENTS(x, mpc) \
{ \
	volatile uchar_t csr = tx_csr; \
	volatile ushort oie = tx_cie; \
       	tx_cie = 0; \
        if(csr & TXSR_EV_B_ACT) { \
             	 tx_lck_ctrl ^= 3; \
		cur_chnl_sync(mpc); \
             (x) |= icpo->ssp.cout_events_b; \
             icpo->ssp.cout_events_b = 0; \
        } else { \
        	 tx_lck_ctrl ^= 3; \
		cur_chnl_sync(mpc); \
             (x) |= icpo->ssp.cout_events_a; \
             icpo->ssp.cout_events_a = 0; \
        } \
        if((x) & EV_TX_EMPTY_Q0) \
        	oie &= ~ENA_TX_EMPTY_Q0;\
        if((x) & EV_TX_LOW_Q0) \
        	oie &= ~ENA_TX_LOW_Q0;\
	tx_cie = oie; \
}

#if (LINUX_VERSION_CODE < 132628)
/* kernels prior to 2.6.20 */
#define _termios termios
#else
/* 2.6.20+ kernels */
#define _termios ktermios
#endif

/*
 * per-megaport board configuration and status
 */

struct lmx_struct
{
	int	lmx_active;
	int	lmx_numbr;
	int	lmx_id;
	int	lmx_speed;
	int	lmx_chan;
	int	lmx_type;		/* b0: 0 = local, 1 = bridge */
	int	lmx_rmt_active;
	int	lmx_rmt_id;		/* mux id */
	int	lmx_rmt_type;		/* mux type */
	int	lmx_wait;
	int	lmx_good_count;
	struct mpchan *lmx_mpc;		/* first channel mpc */
};

struct icp_struct
{
	uchar_t	icp_active;
	uchar_t	icp_rng_last;
	uchar_t	icp_rng_svcreq;
	uchar_t	icp_rng_state;
	int     icp_rng_wait;
	int	icp_rng_fail_count;
	int	icp_rng_good_count;
	paddr_t	icp_dram_start;
	paddr_t	icp_tags_start;
	paddr_t	icp_cmds_start;
	int	icp_dram_size;
	int	icp_tags_size;
	int	icp_cmd_size;
	mpaddr_t icp_regs_start;
	int	icp_minor_start;	/* start minor number for this icp */
	struct lmx_struct lmx[4];
	uchar_t	icp_poll_defer;         /* megapoll processing deferred */
	struct ssp_struct *sspcb;		/* Ramp control block */
	struct slot_struct *slot_ctrl_blk;	/* Ramp */
};

typedef struct icp_struct *icpaddr_t;

struct hwq_struct
{
  int hwq_size;
  uint_t hwq_bits;
  uint_t hwq_hiwat;
  uint_t hwq_lowat;
  uchar_t  hwq_rxwrap;
  uchar_t  hwq_txwrap;
  uchar_t  hwq_cmdsize;
  uint_t  hwq_minchar;
};  /* hwq_struct */

struct mpdev {			/* Adapter struct */
	uchar_t	mpd_alive;		/* board present */
	uchar_t mpd_mem_width;		/* memory access width */
	ushort	mpd_nchan;		/* number of channels used */
	ushort  mpd_id;			/* board type */
	uchar_t mpd_slot;		/* slot where the adapter was found */ 
	uchar_t	mpd_bus;		/* bus type machine */
	uchar_t mpd_nicps;		/* number of icps on board */
	uchar_t mpd_sspchan;		/* number of channels on ssp */
	uchar_t mpd_ccode;		/* PatH - Multimodem country code reg */
	uchar_t mpd_nwin;		/* size of win */
	ushort  mpd_io;			/* io port number */
	ushort	mpd_pgctrl;		/* io port for page control */
	struct	mpchan *mpd_mpc;	/* mpchan base address */
	paddr_t mpd_regs;		/* virtual address of icp regs. */
	paddr_t mpd_pmem;		/* physical memory address */
	int	mpd_addrsz;		/* physical memory size */
	paddr_t mpd_mem;		/* virtual address of on-board memory */
	paddr_t mpd_tags;		/* virtual addrs of tag memory dram */
	int	mpd_memsz;		/* buffer memory size */
	int	mpd_tagsz;
	int	mpd_minor;		/* start minor number for this board */
	struct hwq_struct *mpd_hwq;	/* queue param ptr */
	struct icp_struct icp[NSSP4S];
	struct brdtab_t *mpd_board_def;	/* board definition pointer */
	spinlock_t mpd_lock;		/* protection lock */
};

/*
 * software copy of icp circular queue
 */

typedef	struct {
	char *q_addr;			/* virtual base address */
	uint_t	q_ptr;			/* tail for rx; head for tx */
	ushort	q_begin;		/* start index */
	ushort	q_end;			/* end index */
	ushort	q_size;			/* queue size */
} queue_t;

/*
 * per-channel status information
 */

struct	mpchan {
	int	mpc_flags;
#define MPC_WSEC	0x1		/* wakeup second session */
#define MPC_MODEM	0x2		/* modem control lines in use */
#define MPC_DIALOUT	0x4		/* line used as dial-out */
#define MPC_SOFTCAR	0x8		/* disable modem control */
#define MPC_SLAVE	0x10		/* slave printer configured */
#define MPC_OPEN	0x20		/* channel open */
#define MPC_SPOPEN	0x40		/* slave printer opened */
#define MPC_BUSY	0x80		/* output active */
#define MPC_BLOCK	0x100		/* input flow controlled */
#define MPC_RXFLUSH	0x200		/* input queue flushed */
#define MPC_BREAKIN	0x400		/* received break start */
#define MPC_HIGHBAUD	0x800		/* use high speed table */
#define MPC_SCRN	0x1000		/* multi-screen */
#define MPC_DIALIN	0x2000		/* disconnect on carrier drop */
#define MPC_CTS		0x4000		/* control in used as CTSFLOW */
#define MPC_WBREAK	0x8000		/* wait for end spaceing line */
#define MPC_CHAN_LD	0x10000		/* LD port */
#define MPC_SANE_LD	0x20000		/* open with space, need recovery */
#define MPC_SCANCODE	0x40000		/* scan code mode */
#define MPC_LOOPBACK	0x80000		/* Port in Internal Loopback */
#define MPC_DSCOPER	0x100000	/* Port in Datascope Mode for reads */
#define MPC_DSCOPEW	0x200000	/* Port in Datascope Mode for writes*/
#define MPC_PFREEZE     0x400000        /* freeze in Poll */
#define MPC_DEFER       0x800000        /* Poll processing deferred */
	uchar_t	mpc_rxmask;		/* receive character mask */
	uchar_t	mpc_chan;		/* channel number on this device */
	uchar_t	mpc_wmode;		/* write mode (see below) */
	uchar_t	mpc_rmode;		/* read mode (see below) */
#define LINEDISCIPLINE	0		/* normal line discipline */
#define LD0SUBSET	1		/* use subset of ld 0 */
#define BLOCKCOPY	2		/* block copy */
	ushort	mpc_last_rcv_cnt;	/* previous icp_rxnext value */
	ushort	mpc_ptimer;		/* printer timer */
	ushort	mpc_btimer;		/* break on timer */
	uint_t	mpc_param;		/* overwrite parameters */
#define	IXONSET	1			/* force ixon */
#define	IXANYIG	2			/* ignore ixany setup */
#define IOCTCTS 0x40
#define IOCTRTS 0x80
#define IOCTLLB 0x400
#define IOCTXON 0x800
#define IOCTLCK 0x1000			/* force lock port settings */
	icpiaddr_t mpc_icpi;		/* icp input channel reg's ptr */
#define mpc_cin mpc_icpi
	icpoaddr_t mpc_icpo;		/* icp output channel reg's ptr */
#define mpc_cout mpc_icpo
	icpbaddr_t mpc_icpb;		/* pointer to freezed bank */
	icpaddr_t mpc_icp;		/* pointer to icp_struct */	
	ushort	mpc_cin_events;		/* pending input events */
	ushort	mpc_cout_events;	/* pending output events */
	ushort	mpc_cin_ena;		/* input events enabled */
	ushort	mpc_cout_ena;		/* output events enabled */
	ushort	mpc_count;		/* pending output */
	ushort	mpc_rcv_cnt;		/* last rcv count */
	ushort	mpc_tx_last_invent;	/* chars invented by icp in opost */
	ushort	mpc_cin_stat_mask;
	ushort	mpc_lastrxnext;
	ushort	mpc_rxbase;		/* base tail index */
	ushort	mpc_txbase;		/* base  index */
	queue_t	mpc_rxq;		/* receive queue */
	queue_t	mpc_txq;		/* transmit queue */
	struct	mpdev *mpc_mpd;		/* pointer to megaport board info */
	struct	tty_struct *mpc_tty;		/* pointer to tty structure */
	struct	tty_struct *mpc_sptty;		/* slave printer tty */
	struct	megaslave *mpc_slave;	/* slave printer control strings */
	uint_t mpc_tags;
	uint_t mpc_input;		/* counter total chars rcv */
	uint_t mpc_output;		/* counter total chars send */
	struct	buf *mpc_bp;		/* buffer for allocation heap */
	unsigned char *mpc_rxcp;	/* pointer into mpc_rxbuf */
#ifdef _INKERNEL
	v86_t	*mpc_v86;		/* pointer to VP/ix info */
#else
	void    *mpc_v86;
#endif
	uchar_t	mpc_inttype;		/* virtual device type */
	uchar_t	mpc_lcr;		/* i8250 line control register */
	uchar_t	mpc_start;		/* start output */
	uchar_t	mpc_stop;		/* stop output */
	struct  tty_struct *mpc_ptty;		/* pointer to port tty */
	struct  tty_struct *mpc_sctty;		/* pointer to multi screen tty */
	uchar_t mpc_brdno;		/* board number */
	uchar_t mpc_icpno;		/* icp number on the adapter */
	uchar_t	mpc_lmxno;		/* lmx number from the icp */
	uchar_t mpc_tail_end;
	ushort  mpc_last_esc;		/* last addr esc was detected */
	ushort  mpc_esc_timo;		/* actual esc time waited */
	uchar_t mpc_scrn_no;		/* active screen on this port */
	uchar_t mpc_scrn_count;		/* number screens open */
	struct proc *mpc_selrd;		/* select read pointer */
	struct proc *mpc_selwr;		/* select write pointer */
	uchar_t mpc_selflags;		/* select flags */
	uchar_t mpc_rxpg;		/* select page rx buffer */
	uchar_t mpc_txpg;		/* select page tx buffer */
	uchar_t mpc_tgpg;		/* select page tags buffer */
	int	flags;			/* Linux specific serial flags */

#if	(LINUX_VERSION_CODE < 132096)
	/* kernels before 2.4 */
	struct	wait_queue *open_wait;
	struct	wait_queue *close_wait;
	struct	wait_queue *raw_wait;
#else
	/* 2.4 kernels and after */
	wait_queue_head_t open_wait;
	wait_queue_head_t close_wait;
	wait_queue_head_t raw_wait;
#endif
	int	open_wait_wait;

	int	refcount;
	struct _termios *normaltermios;
	struct _termios *callouttermios;
	pid_t	session;
	pid_t	pgrp;
	int 	openwaitcnt;
	int	close_delay;
	int	closing_wait;
	int	custom_divisor;
	int	baud_base;
#if	(LINUX_VERSION_CODE < 132608)
	/* kernels before 2.6 */
	struct tq_struct	tqhangup;
#else
	/* 2.6 kernels and after */
	struct work_struct	tqhangup;
#endif

	int mpc_block;
	int carr_state;
	unsigned char 		*xmit_buf;
	int			xmit_head;
	int			xmit_tail;
	int			xmit_cnt;
	unsigned int mpc_vtime;		/* extra VTIME for speed multiplier */
	unsigned int mpc_mpa_stat;	/* modem pool status word */
	/* MPA status falue */
#define MPA_SLOT_EMPTY			0x00000001
#define MPA_LMX_CHANGE			0x00000002
#define MPA_INITG_UART    		0x00000004
#define MPA_INIT_ERROR    		0x00000008
#define MPA_LOOP_ERROR    		0x00000010
#define MPA_BREAK_ERROR    		0x00000020
#define MPA_UART_CFG_DONE 		0x00000040
#define MPA_INITG_SET_LOOP_BACK		0x00000080
#define MPA_SET_LOOP_BACK_DONE		0x00000100
#define MPA_INITG_CLR_LOOP_BACK		0x00000200
#define MPA_SET_LOOP_BACK_ERROR		0x00000400
#define MPA_CLR_LOOP_BACK_ERROR		0x00000800
#define MPA_CALL_BACK_ERROR		0x00001000

#define MPA_INITG_START_BREAK		0x00002000
#define MPA_START_BREAK_DONE		0x00004000
#define MPA_INITG_STOP_BREAK		0x00008000
#define MPA_INITG_HARD_RESET		0x00010000
#define MPA_HARD_RESET_ERROR		0x00020000
#define MPA_INITG_INIT_MODEM		0x00040000
#define MPA_INITG_MODIFY_SETTINGS	0x00080000
#define MPA_INIT_MODEM_DONE		0x00100000
#define MPA_INIT_MODEM_ERROR		0x00200000

#if	(LINUX_VERSION_CODE < 132096)
	/* kernels before 2.4 */
	struct	wait_queue *mpc_mpa_slb; /* set loopback wait queue */
	struct	wait_queue *mpc_mpa_clb; /* clear loopback wait queue */
	struct	wait_queue *mpc_mpa_rst; /* reset modem wait queue */
	struct	wait_queue *mpc_mpa_call_back; /* writer's sleep queue */
#else
	/* 2.4 kernels and after */
	wait_queue_head_t mpc_mpa_slb; /* set loopback wait queue */
	wait_queue_head_t mpc_mpa_clb; /* clear loopback wait queue */
	wait_queue_head_t mpc_mpa_rst; /* reset modem wait queue */
	wait_queue_head_t mpc_mpa_call_back; /* writer's sleep queue */
#endif
	int	mpc_mpa_slb_wait;
	int	mpc_mpa_clb_wait;
	int	mpc_mpa_rst_wait;
	int	mpc_mpa_call_back_wait;

	uchar_t mpc_mpa_init_retry;	/* retry count */
	uchar_t mpc_mpa_reset_retry;	/* reset count */
	uchar_t mpc_mpa_delay[MAX_RAMP_INDEX];
	ushort_t	mpc_mpa_cout_ctrl_sigs;	/* output control signals */

	uint_t	mpc_parity_err_cnt;	/* count of parity errors */
	uint_t	mpc_framing_err_cnt;	/* count of framing errors */
	uint_t	mpc_break_cnt;		/* count of breaks */

	int	mpc_major;		/* device major number */
	int	mpc_minor;		/* device minor number */
};

#define mpa_global_s	icp_gbl_struct		/*Global regs*/
#define mpa_input_s	icp_in_struct		/*Input regs */
#define mpa_output_s	icp_out_struct		/*Output regs*/

#define PCI_CSH_LEN 0x40

#define vuchar_t  volatile unsigned char
#define vushort_t volatile unsigned short
#define vuint_t   volatile unsigned int

struct pci_csh00 { /* Type 00 Configuration Space Header */
	vushort_t	vend_id;
	vushort_t	dev_id;
	vushort_t	command;
	vushort_t	status;
	vuchar_t	rev_id;
	vuchar_t	prog_if;
	vuchar_t	sub_class;
	vuchar_t	class;
	vuchar_t	cache_line_size;
	vuchar_t	latency_timer;
	vuchar_t	header_type;
	vuchar_t	bist;		/* built in self timer */
	volatile paddr_t base_addr_reg0;
	volatile paddr_t base_addr_reg1;
	volatile paddr_t base_addr_reg2;
	volatile paddr_t base_addr_reg3;
	volatile paddr_t base_addr_reg4;
	volatile paddr_t base_addr_reg5;
	vuint_t		cisp;		/* Cardbus Info Struct pointer (PCMCIA) */
	vushort_t	sub_sys_vend_id;
	vushort_t	sub_sys_id;
	volatile paddr_t rom_base_addr;
	vuint_t		res1;
	vuint_t		res2;
	vuchar_t	interrupt_line;
	vuchar_t	interrupt_pin;
	vuchar_t	min_gnt;	/* len of burst period @ 33MHz */
	vuchar_t	max_lat;	/* freq. of PCI bus access */
} ;

struct pci_csh {  /* Generic Configuration Space Header */
	union {
		unsigned int     vl[PCI_CSH_LEN/sizeof(int)];
		unsigned short   vs[PCI_CSH_LEN/sizeof(short)];
		unsigned char    vc[PCI_CSH_LEN];
		struct pci_csh00 csh00;
	} cs;
	unsigned char bus;
	unsigned char slot;
} ;


