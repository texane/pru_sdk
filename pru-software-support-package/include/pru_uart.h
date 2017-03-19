
/* UART Register set */
typedef struct {

	/*
	 * RBR and THR register pair
	 * This is a unique register pair in that RBR and THR
	 * share the same address. RBR is read-only while THR is
	 * write-only.
	 *
	 * Additionally, RBR and THR share an address with DLL. To
	 * read/write RBR/THR write 0 to the DLAB bit in the LCR
	 * register. To modify DLL write a 1.
	 *
	 * DLL also has a dedicated
	 * address which does not require toggling the DLAB bit.
	 */
	union {
		/* PRU_UART_RBR register bit field */
		union {
			volatile uint32_t RBR;

			volatile struct{
				unsigned DATA : 8;		// 7:0
				unsigned rsvd8 : 24;		// 31:8
			} RBR_bit;
		} ;

		/* PRU_UART_THR register bit field */
		union {
			volatile uint32_t THR;

			volatile struct{
				unsigned DATA : 8;		// 7:0
				unsigned rsvd8 : 24;		// 31:8
			} THR_bit;
		} ;
	} ;	// 0x0


	/* PRU_UART_IER register bit field */
	/*
	 * IER shares an address with DLH. To modify IER write 0
	 * to the DLAB bit in the LCR register. To modify DLH write a 1.
	 *
	 * DLH also has a dedicated address which does not require
	 * toggling the DLAB bit.
	 */
	union {
		volatile uint32_t IER;

		volatile struct{
			unsigned ERBI : 1;		// 0
			unsigned ETBEI : 1;		// 1
			unsigned ELSI : 1;		// 2
			unsigned EDSSI : 1;		// 3
			unsigned rsvd4 : 28;		// 31:4
		} IER_bit;
	} ;	// 0x4


	/*
	 * IIR and FCR register pair
	 * This is a unique register pair in that IIR and FCR
	 * share the same address. IIR is read-only while FCR is
	 * write-only.
	 */
	union {
		/* PRU_UART_IIR register bit field */
		union {
			volatile uint32_t IIR;

			volatile struct{
				unsigned IPEND : 1;		// 0
				unsigned INTID : 3;		// 3:1
				unsigned rsvd4 : 2;		// 5:4
				unsigned FIFOEN : 2;		// 7:6
				unsigned rsvd8 : 24;		// 31:8
			} IIR_bit;
		} ;

		/* PRU_UART_FCR register bit field */
		union {
			volatile uint32_t FCR;

			volatile struct{
				unsigned FIFOEN : 1;		// 0
				unsigned RXCLR : 1;		// 1
				unsigned TXCLR : 1;		// 2
				unsigned DMAMODE1 : 1;		// 3
				unsigned rsvd4 : 2;		// 5:4
				unsigned RXFIFTL : 2;		// 7:6
				unsigned rsvd8 : 24;		// 31:8
			} FCR_bit;
		} ;
	} ;	// 0x8


	/* PRU_UART_LCR register bit field */
	union {
		volatile uint32_t LCR;

		volatile struct{
			unsigned WLS : 2;		// 1:0
			unsigned STB : 1;		// 2
			unsigned PEN : 1;		// 3
			unsigned EPS : 1;		// 4
			unsigned SP : 1;		// 5
			unsigned BC : 1;		// 6
			unsigned DLAB : 1;		// 7
			unsigned rsvd8 : 24;		// 31:8
		} LCR_bit;
	} ;	// 0xC


	/* PRU_UART_MCR register bit field */
	union {
		volatile uint32_t MCR;

		volatile struct{
			unsigned rsvd0 : 1;		// 0
			unsigned RTS : 1;		// 1
			unsigned OUT1 : 1;		// 2
			unsigned OUT2 : 1;		// 3
			unsigned LOOP : 1;		// 4
			unsigned AFE : 1;		// 5
			unsigned rsvd8 : 26;		// 31:6
		} MCR_bit;
	} ;	// 0x10


	/* PRU_UART_LSR register bit field */
	union {
		volatile uint32_t LSR;

		volatile struct{
			unsigned DR : 1;		// 0
			unsigned OE : 1;		// 1
			unsigned PE : 1;		// 2
			unsigned FE : 1;		// 3
			unsigned BI : 1;		// 4
			unsigned THRE : 1;		// 5
			unsigned TEMT : 1;		// 6
			unsigned RXFIFOE : 1;		// 7
			unsigned rsvd8 : 24;		// 31:8
		} LSR_bit;
	} ;	// 0x14


	/* PRU_UART_MSR register bit field */
	union {
		volatile uint32_t MSR;

		volatile struct{
			unsigned DCTS : 1;		// 0
			unsigned DDSR : 1;		// 1
			unsigned TERI : 1;		// 2
			unsigned DCD : 1;		// 3
			unsigned CTS : 1;		// 4
			unsigned DSR : 1;		// 5
			unsigned RI : 1;		// 6
			unsigned CD : 1;		// 7
			unsigned rsvd8 : 24;		// 31:8
		} MSR_bit;
	} ;	// 0x18


	/* PRU_UART_SCR register bit field */
	union {
		volatile uint32_t SCR;

		volatile struct{
			unsigned SCR : 8;		// 7:0
			unsigned rsvd8 : 24;		// 31:8
		} SCR_bit;
	} ;	// 0x1C


	/* PRU_UART_DLL register bit field */
	union {
		volatile uint32_t DLL;

		volatile struct{
			unsigned DLL : 8;		// 7:0
			unsigned rsvd8 : 24;		// 31:8
		} DLL_bit;
	} ;	// 0x20


	/* PRU_UART_DLH register bit field */
	union {
		volatile uint32_t DLH;

		volatile  struct{
			unsigned DLH : 8;		// 7:0
			unsigned rsvd8 : 24;		// 31:8
		} DLH_bit;
	} ;	// 0x24


	/* PRU_UART_REVID1 register bit field */
	union {
		volatile uint32_t REVID1;

		volatile struct{
			unsigned REVID1 : 32;		// 31:0
		} REVID1_bit;
	} ;	// 0x28


	/* PRU_UART_REVID2 register bit field */
	union {
		volatile uint32_t REVID2;

		volatile struct{
			unsigned REVID2 : 32;		// 31:0
		} REVID2_bit;
	} ;	// 0x2C


	/* PRU_UART_PWREMU_MGMT register bit field */
	union {
		volatile uint32_t PWREMU_MGMT;

		volatile struct{
			unsigned FREE : 1;		// 0
			unsigned rsvd1 : 12;		// 12:1
			unsigned URRST : 1;		// 13
			unsigned UTRST : 1;		// 14
			unsigned rsvd15 : 17;		// 31:15
		} PWREMU_MGMT_bit;
	} ;	// 0x30


	/* PRU_UART_MDR register bit field */
	union {
		volatile uint32_t MDR;

		volatile struct{
			unsigned OSM_SEL : 1;		// 0
			unsigned rsvd1 : 31;		// 31:1
		} MDR_bit;
	} ;	// 0x34

} pruUart;



