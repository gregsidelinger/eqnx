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

/* include file - devdyn.h */

#define DEVSIGNATURE    0x7871 		/* "qx" - arbitrary sequence */

#define HA_WIN16        0x0001          /* supports one 16K window */
#define HA_FLAT         0x0002          /* supports flat mapping */

#define EQNID           DEVSIGNATURE	/* streams module id */
#define WATCHDOGID      (0x0c0 | 0x17)  /* 32 sec or'd with 5-bit key */
#define BUS_ISA		1
#define BUS_EISA	2
#define BUS_MCA		3

#define MPTIME0         4               /* poll interval for megapoll() */
#define MPSANETIME0	500		/* poll interval for eqxsane() */

/* values for "mpc_flags" field */
#define MPC_FLUSHIN 	0x01		/* flush input on next poll */
#define MPC_OUTCS_UP    0x02            /* outbound control sigs raised */
#define MPC_TXQBUSY	0x04		/* set by eqnoput */
#define MPC_V24CFG      0x08            /* v24 processing configured */
#define MPC_LPCFG       0x10            /* slave printer configured */
#define MPC_OPEN        0x20            /* channel open */
#define MPC_LPOPEN      0x40            /* slave printer opened */
#define MPC_LOOPBACK	0x80		/* channel in loopback mode */
#define MPC_BLOCK       0x0100          /* input flow controlled */
#define MPC_FLUSHOUT	0x0200		/* close flush output */
#define MPC_IXON	0x0800		/* don't honor -IXON */
#define MPC_HIGHBAUD	0x1000		/* alternate baudrate interpretation */
#define MPC_FLOWOFF     0x00800000      /* software flag following "STOPDMA" */
#define MPC_LPBUSY      0x01000000      /* lp output on - delays close */
#define MPC_VCLOSE		0x02000000	/* vpix close in progress */
#define MPC_HANGUP 		0x0400	/* vpix close in progress */


#define MPC_DEFAULT_MASK    (MPC_LPCFG | MPC_LPOPEN | MPC_V24CFG \
				| MPC_IXON | MPC_LOOPBACK | MPC_HIGHBAUD)

#define LP_BAND         'Q'             /* eqxlp msg priority band */
#define ESC_MAX_LOOK    8               /* search back this far in xmit data */
#define ESC_MAX_TIMEOUT 24              /* wait ticks after xmit escape */

#define CIN_DEF_ENABLES         0x3800  /* dma fail, input ovfl, min char rx */
#define COUT_DEF_ENABLES        0x8000

/******************************************************************************

        The following structures assume that each ldv_struct or rdv_struct
        element represents 16 minor devices.  Physically, this may not be
        the case.  However, 16 minor device numbers must be reserved for
        each structure.
        
        Additional assumptions:
        
          1) There must always be four ldv's per icp.
          2) For bridges, there is one rdv_struct per ldv_struct.
        
******************************************************************************/

/* to save processing time, the VIRTUAL pointers are computed at init time
  and saved in per channel structures - they could (at the expense of
  increased processing) be inferred from *mpc_icp structure members */

#if MP
typedef struct {
        dev_t l_devno;          /* chnl number useful for debugging */
        sv_t *l_sv_open0;       /* t_cc[3] */
        sv_t *l_sv_open1;       /* t_rdqp */
        sv_t *l_sv_open2;       /* v_state */
        sv_t *l_sv_close0;      /* t_oflag */
        sv_t *l_sv_close1;      /* t_oflag + 2*/
        lock_t *l_sleep;        /* used to pass to SV_WAIT_SIG */
        lock_t *l_chnl;         /* channel lock structure ptr */
        int l_context;          /* context of top-level channel lock */
        int l_ucount;           /* recursion depth for utility channel lock */
        ushort l_uheld;         /* utility lock actually held */
        ushort l_uspin;         /* count of utilities spinning for lock */
        pl_t l_oldspl;          /* spl level prior to acquiring lock */
} mpc_locks_t;
#endif

struct hwq_struct
{
  int hwq_size;
  uint_t hwq_bits;
  uint_t hwq_hiwat;
  uint_t hwq_lowat;
  uchar_t  hwq_rxwrap;
  uchar_t  hwq_txwrap;
  uchar_t  hwq_cmdsize;
  uint_t hwq_minchar;
};  /* hwq_struct */

struct mpc_struct
{
  int mpc_flags;                        /* named bits */
  struct cin_struct *mpc_cin;           /* VIRTUAL- hw cin structure ptr */
  struct cout_struct *mpc_cout;         /* VIRTUAL- hw cout structure ptr */
  uchar_t *mpc_rxbuf;                   /* VIRTUAL- base of chnl rx buf */
  uchar_t *mpc_txbuf;                   /* VIRTUAL- base of chnl tx buf */
  ushort_t mpc_rxbuf_pg;
  ushort_t mpc_txbuf_pg;
  uchar_t *mpc_rxtags;                  /* VIRTUAL- base of chnl tag buf */
  uchar_t *mpc_txcmds;                  /* VIRTUAL- base of chnl cmd buf */
  ushort_t mpc_rxtags_pg;
  ushort_t mpc_txcmds_pg;
  struct strtty *mpc_stty;              /* pointer to tty structure */
  struct ldv_struct *mpc_ldv;           /* backward link or NULL */
  struct icp_struct *mpc_icp;           /* link to icp structure */
  struct cin_bank_struct mpc_rx_bank;   /* snapshot + events to process */
  ushort_t mpc_rx_bank_is_b;            /* identity of snapshot bank */
  ushort_t mpc_rx_last_events;          /* "leftover" rx events */
  ushort_t mpc_rx_last_ena;             /* last "cin_attn_ena" mask */
  ushort_t mpc_rx_add_ena;              /* add to mask and clear */
  ushort_t mpc_rx_remove_ena;           /* remove from mask and clear */
  ushort_t mpc_tx_events;               /* current tx events to process */
  ushort_t mpc_tx_last_events;          /* "leftover" tx events */
  ushort_t mpc_tx_last_ena;             /* last "cout_attn_ena" mask */
  ushort_t mpc_tx_add_ena;              /* add to mask and clear */
  ushort_t mpc_tx_remove_ena;           /* add to mask and clear */
  ushort_t mpc_tx_count;		/* count of data added to queue */
  ushort_t mpc_tx_last_invent;		/* last value of char invent count */
  uint_t   mpc_cin_qbase;               /* 16-bit ptr to base of queue */
  uint_t   mpc_cin_qmask;               /* mask to remove upper bits from 
                                           icp input queue pointer */
  ushort_t mpc_cin_qlast;               /* 16-bit ptr to last byte of queue */
  ushort_t mpc_cin_stat_mask;		/* input events to process */
  ushort_t mpc_cout_qbase;              /* 16-bit ptr to base of queue */
  ushort_t mpc_cout_qmask;              /* mask to remove upper bits from 
                                           icp output queue pointer */
  ushort_t mpc_cout_qnext;              /* 16-bit ptr to next free byte */
  ushort_t mpc_cout_qlast;              /* 16-bit ptr to last byte of queue */
  ushort_t mpc_hfc;			/* holds x_hflag from termiox */
  int mpc_to_txstrt;			/* timeout id for eqntxstrt() */
  int mpc_to_delay;			/* timeout id for eqndelay() */
  long mpc_esc_tstamp;                  /* eqxlp - actually clock_t */
  uchar_t mpc_esc_since;                /* eqxlp */
  uchar_t mpc_filler1[3];
#ifdef VPIX
  v86_t *mpc_v86;                       /* pointer to VP/ix info */
  uchar_t mpc_inttype;                  /* virtual device type */
  uchar_t mpc_lcr;                      /* i8250 line control register */
#else
  uchar_t mpc_filler2[6];               /* force non-vpix to same size */
#endif
#if MP
  mpc_locks_t *mpc_locks;               /* pointer to channel lock structure */
#else
  void *mpc_filler3;
#endif
};  /* mpc_struct */

struct rdv_struct
{
  int rdv_id;
  int rdv_alive;
  int rdv_mpc_count;
  int rdv_found_count;
  struct mpc_struct *rdv_mpc;           /* forward link - always 16 elements */
  struct ldv_struct *rdv_ldv;           /* backward link */
  int rdv_to_rdv_wait;
};  /* rdv_struct */

struct ldv_struct
{
  int ldv_id;                           /* -1 if never known, else last id */
  int ldv_alive;                        /* false if offline */
  int ldv_lmx_speed;                    /* internal speed 0..3 */
  int ldv_mpc_count;                    /* zero if a bridge */
  struct mpc_struct *ldv_mpc;           /* always 16 elements or NULL */
  struct rdv_struct *ldv_rdv;           /* always 1 element or NULL */
  struct icp_struct *ldv_icp;           /* backward link */
};  /* ldv_struct */

struct ring_struct
{
  int rng_state;
  uchar_t rng_last;
  uchar_t rng_svcreq;
  int rng_good_count;
  int rng_fail_count;
  int rng_fail_lbolt;
  int rng_to_rng_wait;
};  /* ring_struct */

struct icp_struct
{
  short icp_alive;
  short icp_idx;		      /* icp index on host adapter */
  dev_t icp_org_minor;
  uchar_t icp_id;
  struct ldv_struct *icp_ldv_ara;     /* forward link - always 4 elements */
  struct ha_struct *icp_ha;           /* backward link */
  struct icp_global_struct *icp_glo;  /* VIRTUAL- ptr to global regs */
  struct cin_struct *icp_cin;         /* VIRTUAL- element 0 of array[0..63] */
  struct cout_struct *icp_cout;       /* VIRTUAL- element 0 of array[0..63] */
  uchar_t *icp_buf;                   /* VIRTUAL- base of buffer or pg frame */
  uchar_t *icp_tags;                  /* VIRTUAL- base of tagbuf or pg frame */
  uchar_t *icp_cmds;                  /* VIRTUAL- base of cmdbuf or pg frame */
  struct ring_struct icp_ring;
  uint_t icp_sw_rx0;
  uint_t icp_sw_rx1;
  uint_t icp_sw_tx0;
  uint_t icp_sw_tx1;
  uchar_t icp_v24[MAXICPCHNL];	      /* one per chnl - on if v24 */
};  /* icp_struct */

struct ha_struct
{
  int ha_valid;
  int ha_alive;
  short ha_icp_count;
  short ha_slot;		/* -1 for isa cards */
  short ha_mem_strat;
  short ha_mem_width;
  struct hwq_struct *ha_hwqparms;
  int ha_mem_size;
  paddr_t ha_paddr;		/* physical address of base of card */
  uint_t ha_paddr_size;		/* byte length of card in memory map */
  unsigned ha_pgctrl;		/* io address to select memory page */
  caddr_t ha_addr;		/* VIRTUAL address of base of card */
  struct icp_struct *ha_icp_ara;
  ushort_t ha_id;
  ushort_t ha_bus;
};  /* ha_struct */

typedef struct chanvec
{
  struct ha_struct *hba;
  struct icp_struct *icp;
  struct ldv_struct *ldv;
  struct rdv_struct *rdv;
  struct mpc_struct *mpc;
  struct strtty *tp;
} chanvec_t;

#define RNG_BAD         0
#define RNG_GOOD        1
#define RNG_FAIL        14
#define RNG_RECOVER	15

#define DEV_VIRGIN      -1              /* device id is unknown */
#define DEV_BAD         0               /* device was known but now offline */
#define DEV_GOOD        1               /* device known and online */
#define DEV_WAITING	2		/* waiting for mux link to settle */
#define DEV_NOSUPPORT	15              /* unsupported board type */
#define DEV_INSANE      16              /* hardware error (unexpected) */

