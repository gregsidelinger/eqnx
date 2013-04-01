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

/************************************************
 *                                    	   	*
 * INSTALLATION STATUS TABLE HEADER FILE   	*
 *                                    	   	*
 *                                    	   	*
 *          FILE:   IST.H        	   	*
 *          DATE:   06/20/94          	   	*
 *           REV:      09 (Update "IST_REV"	*
 *			   in Constant area)	*
 *        AUTHOR: BILL KRAMER         	   	*
 *        			      	   	*
 ************************************************/


/********************************************************************************
 * 				REVISION HISTORY				*
 *										*
 * REV #    DATE				COMMENTS			*
 * =====  ========	======================================================	*
 *  01	  04/04/94	Original header file created				*
 *  02	  04/06/94	Added #defines that defines various sizes of arrays,	*
 *			updated values for elements in structures and updated	*
 *			 following elements:					*
 *				o ctlr_struct, "ctlr_port" changed to "ctrl_loc"*
 *				o Removed "Topological area" in ctlr_struct	*
 *				  which included elements:			*
 *					- topo_max_ports			*
 *					- topo_start_major			*
 *					- topo_start_minor			*
 *				o ist_struct, added element that indicates	*
 *				  structures revision "struct_rev"		*
 *				o Replaced Driver Major/Minor Rev. Number vars.	*
 *				  with 16 byte Driver Version string		*
 *				o Deleted "drvr_rev_date", Driver date		*
 *				o Added Driver Object Type var. "drvr_obj_type"	*
 *  03	  04/14/94	Add literal "IST_REV" and modified documentation for	*
 *			"cnfg_addr_size" and "sys_isa_hole_status" table values	*
 *  04    04/15/94	Changed symbol "ver_number" to "drvr_ver" to clarify	*
 *			symbol's meaning					*
 *  05	  04/18/94	Added structure ("operator_parms") that defines 	*
 *			parameters specified by	by operator during Installation.*
 *			Modified values of Memory Hole Status table 		*
 *			"sys_isa_hole_status"					*
 *  06	  04/26/94	Modified comments concerning following variables:	*
 *				o Controller Configuration Structure's 		*
 *				  "cntrlr_bus_inface" indicated that 8 bit	*
 *				  interface & auto-sense values only valid when	*
 *				  controller's mode is paged			*
 *				o Operator Parameter Structure's "isa_def_specd"*
 *				  changed "default not spec'd from 0 to 1	*
 *				o Host Controller Structure's "cnfg_fail" 	*
 *				  changed validity from when "cnfg_state" != 0	*
 *				  to != 1.  Also removed value def's since 	*
 *				  values are driver specific			*
 *				o Installation Status Table Structure's 	*
 *				  "sys_isa_hole_status" array, added two new	*
 *				  states					*
 *				o Installation Status Table Structure's 	*
 *				  "ctlr_array" increased array size from 8 to	*
 *				  16 in order to accomidate NCR systems		*
 *  07    04/28/94	Updated to accomodate function's "determ_isa_cnfg" 	*
 *			return parameter block. Change includes:		*
 *				o Adding "isa_cnfg_parms" structure		*
 *				o Modifying "ctlr_struct" to include		*
 *				  "isa_cnfg_parms" structure			*
 *  08    04/28/94	Updated structure "isa_cnfg_parms" to include variable,	*
 *			"method" that describes how Configuration parms were 	*
 *			determined						*
 *  09	  06/20/94	Modified following:					*
 *				o Modified symbol MAX_ISA_ARRAY_SIZE to		*
 *				  accommodate addition of memory between 512k &	*
 *				  640k to Probe Table				*
 *				o Structure operator_parms, Memory Probe Table,	*
 *				  added new flag indicating whether or not to	*
 *				  perform "VGA 8-bit memory override" memory	*
 *				  hole probe check				*
 *				o Structure ist_struct, added new value to 	*
 *				  table indicating address (memory hole) 	*
 *				  allocated as a 16-bit hole even though it 	*
 *				  appears that area is occupied by an 8-bit 	*
 *				  device (VGA ROM).				*
 *										*
 ********************************************************************************/

/************************************************
 *						*
 *   	 CONSTANT/LITERAL DECLARATIONS		*
 *						*
 ************************************************/


#define	IST_REV			( (unsigned char)0x09)		/*Revision level of Installation Status Table structure	*/



/*Literals defining sizes related to Tables (arrays) of physical addresses (see "ist_struct")				*/

/*RV9*/
#define MAX_ISA_ARRAY_SIZE	( (unsigned int)(28) )		/*Max. # entries in 640k-1M ISA Addr Table (Hole tbl)	*/

#define MAX_BIT_31_ARRAY_SIZE	( (unsigned int)(16) )		/*Max. number of elements in bit 31 ISA mem Hole Table	*/

#define	HOLE_TABLE_SIZE		( (unsigned int)(MAX_ISA_ARRAY_SIZE + MAX_BIT_31_ARRAY_SIZE + 1) ) /*Total # elms in Mem*/


#ifdef	ISA_ENAB

/************************************************
 *      CONTROLLER CONFIGURATION STRUCTURE	*
 *						*
 * NOTE: This structure defines configuration 	*
 *	 parameters for an ISA Controller.  	*
 *	 Structure is used Operator Parameter   *
 *	 structure defined below.		*
 *						*
 ************************************************/

struct	cntrl_cfig  {

	unsigned short int 	base_port ;			/*ISA Controller base I/O port				*/

	unsigned long int	cntrlr_base_addr ;		/*Controller's physical base memory address		*/

	unsigned char		cntrlr_mode ;			/*Size of Controller's mem area (Port 1 value):		*/
								/*	o 0x00 = page					*/
								/*	o 0x40 = flat					*/

	unsigned char		cntrlr_bus_inface ;		/*Size of Controller's Bus interface (port 1 value):	*/
								/*	o 0x00 =  8 bit bus interface (only if "paged")	*/
								/*	o 0x20 = 16 bit bus interface			*/
								/*	o 0xFF = Dynamic/auto-sense (only if "paged")	*/

} ;

/****************************************
 *					*
 *     OPERATOR PARAMETER STRUCTURE	*
 *					*
 * NOTE: This structure supplies all	*
 *	 parameters specified during	*
 *	 Driver Installation.		*
 *					*
 ****************************************/
struct	operator_parms	{
/********************************
 * 	GENERAL PARAMETERS	*
 ********************************/

	unsigned char		    sys_bus_type ;		/*System's Bus type:					*/
								/*	0x01 = ISA Controller				*/
								/*	0x02 = EISA Controller				*/
								/*	0x03 = MCA Controller				*/

	unsigned char		    default_baud ;		/*Default baud rate table				*/

	unsigned char		    quiet_mode ;		/*Quiet mode operation:					*/
								/*	0x01 = Issue all status messages		*/
								/*	0xFF = Issues minimum set of status messages	*/

	unsigned char		    num_cntrls ;		/*Maximum number of Host Controllers:			*/
								/*	Char Driver    = 1 - 8				*/
								/*	Streams Driver = 0 (null, not used)		*/

/***************************************************
 *   ISA SPECIFIC PARAMETERS - MEMORY PROBE TABLES *
 ***************************************************/

	unsigned short int	    isa_num_ents ;			/*# entries in ISA Search Table ("Hole" array)	*/
									/* (1 - MAX_ISA_ARRAY_SIZE)			*/

	unsigned long int	    isa_addr_tbl[MAX_ISA_ARRAY_SIZE] ;	/*Table of addresses (between 640 & 1M) to use	*/
									/*when probing where ISA Controllers can reside	*/

	unsigned char		    isa_bit_31_probe ;			/*Flag indicating whether to use "bit 31" table	*/
									/*when probing for an available 16k page:	*/
									/*	o 0x01 = Use bit 31 table		*/
									/*	o 0xFF = DO NOT use bit 31 table	*/

	unsigned short int	    isa_num_bit_31_ents ;		/*# entries in ISA bit 31 Search Table		*/
									/* (1 - MAX_BIT_31_ARRAY_SIZE)			*/

	unsigned long int	    isa_bit_31_tbl[MAX_BIT_31_ARRAY_SIZE] ; /*Table of addresses (w/ bit 31 set) to use */
									/*when probing where ISA Controllers can reside	*/

/*RV9*/	unsigned char		    vga_hole_override ;			/*Flag indicating if should perform "VGA 8-bit	*/
									/*memory override" memory hole probe check:	*/
									/*	o 0x01 = Perform override probing	*/
									/*	o 0xFF = DO NOT attempt override	*/
/***************************************************
 *   ISA SPECIFIC PARAMETERS - CONFIGURATION PARMS *
 ***************************************************/

	unsigned char		    isa_def_specd ;			/*Default Controller parameters specified:	*/
									/*	o 0x01 = Defaults not specified 	*/
									/*	o 0xFF = Defaults specified in 		*/
									/*		 structure "default_config"	*/

	struct cntrl_cfig    	    isa_def_config ;			/*Default parameters for ISA Controllers not	*/
									/*specified in Explicit Config Table below i.e.	*/
									/*not defined in "explct_cnfg_tbl"		*/
									/*NOTE: o Structure only valid if 		*/
									/*	  "deflt_specified" = 0xFF		*/
									/*	o Element "base_port" in structure is	*/
									/*	  ignored and therefore is assumed 0	*/

	unsigned char		    isa_explct_ents ;			/*Number of entries in Explicit Config Table	*/
									/*	o   0   = No explicitly specified ISA	*/
									/*		  Controllers			*/
									/*	o 1 - 8 = Number valid/specified entries*/

	struct cntrl_cfig    	    isa_explct_tbl[8] ;			/*Table of explicit Configuration parameters for */
									/*particular ISA Controllers			*/

} ;

/************************************************
 *						*
 *   determ_isa_cnfg RETURN PARAMETER BLOCK	*
 *						*
 * NOTE: This structure is integrated into	*
 *	 the Host Controller structure 		*
 *	 (ctlr_struct) defined below.		*
 *	 This structure is built by the		*
 *	 "Determine ISA Controller 		*
 *	 Configuration" function.		*
 *						*
 ************************************************/

struct isa_cnfg_parms {

		unsigned char			status ;	/*Status of Configuration attempt (functn's ret status)	*/

		unsigned char			method ;	/*How Configuration parms were determined:		*/
								/*	0x02 = Dynamic					*/
								/*	0x03 = Explicit					*/

		struct	cntrl_cfig		cntrlr_parms ;	/*ISA Controller Configuration params (valid only if	*/
								/*status = successful					*/

} ;
#endif	/* ISA_ENAB */


/************************************************
 *						*
 *   	     HOST CONTROLLER STRUCTURE		*
 *						*
 * NOTE: This structure is integrated into the	*
 *	 Installation Status Table structure	*
 *	 (ist_struct) defined following this 	*
 *	 structure				*
 *						*
 ************************************************/

struct	ctlr_struct	{

/****************************************
 * 	 CONTROLLER INFORMATION		*
 ****************************************/

	unsigned char 		ctlr_major ;		/*Controller major number 					*/
	unsigned char 		ctlr_model ;		/*Controller's type (Board ID):					*/
							/*	0x08 = SS64						*/
							/*	0x10 = SS128						*/
							/*	0x18 = SS8						*/
	unsigned char		ctlr_model_ext;	/* Controller's extent (Qualifier ID): */
							/* 
								ISA/EISA SSP-64 based:
								0x00

								ISA/EISA SSP-4 based:
								0x00 = 1 ASIC,   1 Port
								0x24 = 1 ASIC,   2 Ports
								0x09 = 1 ASIC,   4 Ports
								0x10 = 2 ASICs,  2 Ports
								0x12 = 2 ASICs,  4 Ports
								0x1B = 2 ASICs,  8 Ports(Full control signal support)
								0x19 = 2 ASICs,  8 Ports(Partial control signal support)
								0x1A = 2 ASICs,  8 Ports(No control signal support)
								0x18 = 4 ASICs,  4 Ports
								0x20 = 4 ASICs,  8 Ports
								0x08 = 4 ASICs, 16 Ports

								PCI SSP-64 based:
								0x10 = 2 ASICs, 128 Ports
								0x08 = 1 ASIC,   64 Ports

								PCI SSP-4 based:
								0x90 = 2 ASICs
								0x88 = 1 ASIC
							*/

	unsigned char		ctlr_rev ;		/*Controller's revision level (0-7)				*/

	unsigned char		ctlr_bus_type ;		/*Controller's Bus interface type:				*/
							/*	0x01 = ISA Controller					*/
							/*	0x02 = EISA Controller					*/
							/*	0x03 = MCA Controller					*/

	unsigned short int	ctlr_loc ;		/*Controller's location.  Value is used to uniquely identify	*/
							/*Controller.  Value can be used with Set Up Utility's data base*/
							/*to determine a Port's logical name prefix.  Values are as	*/
							/*follows:							*/
							/*	ISA  Controller = Base I/O port (0x0200 - 0x03E0)	*/
							/*	EISA Controller = EISA Slot # (0x01 - 0x0F)		*/
							/*	MCA  Controller = MCA/POS Slot # (0x00 - 0x0F)		*/
							/*		NOTE: MCA Slot 0 is physc'ly labeled 1 therefore*/
							/*		      value = labeled slot # -1			*/

	unsigned short int	ctlr_mca_port ;		/*If MCA Controller, base I/O port i.e. "normal" port		*/
							/*(0x3000, 0x3080, 0x3400, 0x3480, 0x3800, 0x3880,		*/
							/* 0x3C00, 0x3C80, 0x7000, 0x7080, 0x7400, 0x7480,		*/
							/* 0x7800, 0x7880, 0x7C00, 0x7C80)				*/
/****************************************
 *     CONFIGURATION INFORMATION	*
 ****************************************/

	unsigned char		cnfg_init ;		/*Controller initialization method:				*/
							/*	0x01 = CMOS info. (i.e. EISA or MCA Controller set up 	*/
							/*	       by BOOT/BIOS using CMOS info. established by	*/
							/*	       Bus configuration software)			*/
							/*	0x02 = Dynamic (i.e. ISA Controller init'd via	Driver	*/
							/*	       dynamically finding memory hole etc.)		*/
							/*	0x03 = Explicit (i.e. ISA Controller init'd via	Driver	*/
							/*	       using operator supplied parameters established	*/
							/*	       during Installation)				*/

	unsigned char		cnfg_state ;		/*Controller initialization state (coupled with "cnfg_fail"):	*/
							/*	0x01 = Controller initialized successfully		*/
							/*	0x02 = Controller init. failed during Drver Init.	*/
							/*	       (see "cnfg_fail" below for failure reason)	*/
							/*	0x03 = Controller init. failed during Driver Post Init.	*/
							/*	       (see "cnfg_fail" below for failure reason)	*/

	unsigned char		cnfg_fail ;		/*Controller initialization failure reason			*/
							/*NOTE: 1. Variable valid only if "cnfg_state" != 1		*/
							/*	2. Values are Driver specific				*/

	unsigned long int	cnfg_physc_base ;	/*Controller's physical base address				*/
							/*NOTE: Field not valid if "cnfg_state" indicates error (not 1)	*/
							/*	and "cnfg_fail" = 0x01 (memory hole search failed)	*/

	unsigned char		cnfg_mode ;		/*Controller's memory mode:					*/
							/*	0x01 = Paged						*/
							/*	0x02 = Flat						*/

	unsigned long int	cnfg_addr_size ;	/*Controller's address space size (in bytes)			*/
							/*	0x004000 = 16k (typical paged Controller)		*/
							/*	0x100000 = 1MB (typical flat SS64 type Controller)	*/
							/*	0x200000 = 2MB (typical flat SS128 type Controller)	*/

	unsigned char		cnfg_isa_bus_sz ;	/*ISA Controller bus interface size				*/
							/*	0x08 = 8 bit						*/
							/*	0x10 = 16 bit						*/

#ifdef	ISA_ENAB
	struct isa_cnfg_parms	cnfg_determ_reslt ;	/*Return paramaters from "determ_isa_cnfg" function that	*/
#endif	/* ISA_ENAB */
							/*describe Controller's memory adresss, pg/flat, bus interface	*/

} ;							/*End of Host Controller Structure				*/


/************************************************
 *						*
 * INSTALLATION STATUS TABLE (IST) DEFINITION	*
 *						*
 ************************************************/

struct	ist_struct	{

	unsigned char		struct_rev ;		/*Structure's Revision Number					*/
							/*NOTE: Should match Header's Rev. Number			*/


/****************************************
 *     	   DRIVER  INFORMATION		*
 ****************************************/

	unsigned char		drvr_ver[16] ;		/*Driver Version Number string with following format:		*/
							/*	         FIELD		     SIZE (bytes)		*/
							/*	=========================    ============		*/
							/*	Major Version Number		 1 - 2			*/
							/*	"." (literal .)			   1			*/
							/*	Minor Version Number		   2			*/
							/*	Qualifier String		 0 - 10			*/
							/*	  o "ALPHA"						*/
							/*	  o "BETA"						*/
							/*	0 = Null String Terminator	   1			*/

	unsigned int	 	drvr_type ;		/*Driver type:							*/
							/*	0x01 = Character Driver					*/
							/*	0x02 = Streams Driver					*/

	unsigned int		drvr_bind_type ;	/*Driver/Kernel binding type:					*/
							/*	0x01 = Driver linked with Kernel			*/
							/*	0x02 = Loadable	Driver					*/

	unsigned char		drvr_obj_type ;		/*Driver Object Type:						*/
							/*	0x01 = SCO Xenix					*/
							/*	0x02 = SCO Unix						*/
							/*	0x03 = AT&T Unix R3.2					*/
							/*	0x04 = UNISYS SVR4 (loadable)				*/
							/*	0x05 = HP/UX						*/
							/*	0x06 = SOLARIS x86					*/
							/*	0x07 = SOLARIS SPARC					*/
							/*	0x08 = SVR4 UNIVEL (loadable)				*/
							/*	0x09 = SVR4 (static)					*/
							/*	0x0A = NCR/UP SVR4 (static)				*/
							/*	0x0B = NCR/MP SVR4 (static)				*/

/****************************************
 *           SYSTEM INFORMATION		*
 ****************************************/

	unsigned char		sys_bus_type ;		/*System's Bus Type (as determined by Install Script):		*/
							/*	0x01 = ISA Bus						*/
							/*	0x02 = EISA Bus						*/
							/*	0x03 = MCA Bus						*/

	unsigned long int	sys_isa_holes[HOLE_TABLE_SIZE]; /*If ISA/EISA Bus, table of physical memory addresses	*/
							/*that Driver used for 16k page mem. hole availability probing.	*/
							/*NOTES: 1. Range of addresses in table includes 640k-1M and	*/
							/*	    addresses above 0x80000000				*/
							/*	 2. Number of entries in array is variable therefore	*/
							/*	    array is null (0) terminated.  Max. number of 	*/
							/*	    entries is 20 possible ISA addresses, 16 addresses	*/
							/*	    with bit 31 set & 1 entry for null terminator	*/
							/*	 3. To determine if an entry in this table has been	*/
							/*	    checked by Driver, use address's index to index	*/
							/*	    into memory hole status table below.		*/

	unsigned char		sys_isa_hole_status[HOLE_TABLE_SIZE]; /*If ISA/EISA Bus, mem hole status table.  Each	*/
							/*entry maps to corresponding element in table of physical mem	*/
							/*addresses above.  Table null terminated.  An entry in table	*/
							/*can have following values:					*/
							/*  o 0x00 = end of table					*/
							/*  o 0x01 = addr checked & failed due to memory occupied by ROM*/
							/*  o 0x02 = addr checked & failed due to memory occupied by RAM*/
							/*  o 0x03 = addr checked & failed due to memory cached		*/
							/*  o 0x04 = addr checked & failed due to conflict including	*/
							/*	     ICP's Channel Counter not "ticking"		*/
							/*  o 0x80 = address failed due to memory mapping failure	*/
							/*  o 0xF0 = address available, bus interface size unknown	*/
							/*  o 0xF1 = address available using 8 bit bus interface	*/

/*RV9*/							/*  o 0xFD = address in use by Driver (16 bit bus & page mode)	*/
							/*	     even though memory appears to be in a region	*/
							/*	     occupied by an 8 bit device (VGA ROM)		*/
							/*		NOTE: 1. ISA Controller(s) mapped to this addr	*/
							/*		      2. Only one element in array will have	*/
							/*		      	 this value				*/
							/*  o 0xFE = address in use by Driver (8 bit bus & page mode)	*/
							/*		NOTE: 1. ISA Controller(s) mapped to this addr	*/
							/*		      2. Only one element in array will have	*/
							/*		      	 this value				*/
							/*  o 0xFF = address in use by Driver (16 bit bus & page mode)	*/
							/*		NOTE: 1. ISA Controller(s) mapped to this addr	*/
							/*		      2. Only one element in array will have	*/
							/*		      	 this value				*/
/****************************************
 * CONTROLLER CONFIGURATION INFORMATION	*
 ****************************************/

	unsigned char		ctlr_max_num ;		/*If Character Driver, max. Number of Host Controllers Driver 	*/
							/*can manage (1-8).  Value established during Installation and 	*/
							/*was used to acquire Major numbers.  Field is 8 if streams drvr*/

	unsigned char		ctlr_found ;		/*Number of Host Controllers found by Driver.  Also, describes	*/
							/*Number of valid elements (structures) in array of Host 	*/
							/*Controllers (see "ctlr_array" below).				*/
							/*	0x00 	    = Driver DID NOT FIND ANY Controllers	*/
							/*	0x01 - 0x08 = Number Controllers found by Driver	*/

	unsigned char		ctlr_init ;		/*Number of Host Controllers initialized/in use by Driver(0-8).	*/
							/*NOTE:	Difference between "ctlr_found" and this field is the	*/
							/*	number of Controllers not initialized due to error	*/

	struct ctlr_struct	ctlr_array[16] ;	/*Array of found Host Controllers.  One structure per found	*/
							/*Host Controller (see "ctlr_found" above).			*/
							/*NOTE: 1. Array in Minor Number order				*/
							/*	2. Number of valid structures = "ctlr_found"		*/



/****************************************
 *  DECLARE "EXTRA" SPACE FOR EXPANSION *
 ****************************************/

	unsigned char		expansion[25] ; 	/*Set structure size = 720 (0x2D0) bytes (if no "bounding")	*/

} ;							/*End of Initialization Status Table Structure			*/

