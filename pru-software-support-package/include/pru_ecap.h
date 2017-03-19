
/* PRU_ECAP register set */
typedef struct{

	/* PRU_ECAP_TSCTR register bit field */
	union {
		volatile uint32_t TSCTR;

		volatile struct {
			unsigned  TSCTR : 32;
		} TSCTR_bit;
	} ;	// 0x0


	/* PRU_ECAP_CTRPHS register bit field */
	union {
		volatile uint32_t CTRPHS;

		volatile struct{
			unsigned CTRPHS : 32;
		} CTRPHS_bit;
	} ;	// 0x4


	/* PRU_ECAP_CAP1 register bit field */
	union {
		volatile uint32_t CAP1;

		volatile struct{
			unsigned CAP1 : 32;
		} CAP1_bit;
	} ;	// 0x8

        /* PRU_ECAP_CAP2 register bit field */
        union {
                volatile uint32_t CAP2;

                volatile struct{
                        unsigned CAP2 : 32;
                } CAP2_bit;
        } ;     // 0xC

        /* PRU_ECAP_CAP3 register bit field */
        union {
                volatile uint32_t CAP3;

                volatile struct{
                        unsigned CAP3 : 32;
                } CAP3_bit;
        } ;     // 0x10

        /* PRU_ECAP_CAP4 register bit field */
        union {
                volatile uint32_t CAP4;

                volatile struct{
                        unsigned CAP4 : 32;
                } CAP4_bit;
        } ;     // 0x14

        uint32_t rsvd18[4];     // 0x18 - 0x24

	/* PRU_ECAP_ECCTL1 register */
	volatile uint16_t ECCTL1;

	/* PRU_ECCTL2 register */
	volatile uint16_t ECCTL2;

	/* PRU_ECAP_ECEINT register */
	volatile uint16_t ECEINT;

        /* PRU_ECAP_ECFLG register */
	volatile uint16_t ECFLG;

	/* PRU_ECAP_ECCLR register */
	volatile uint16_t ECCLR;

        /* PRU_ECAP_ECFRC register */
	volatile uint16_t ECFRC;

	uint32_t rsvd34[10];	// 0x34 - 0x54

	/* PRU_ECAP_REVID register bit field */
	union {
		volatile uint32_t REVID;

		volatile struct{
			unsigned REV : 32;
		} REVID_bit;
	} ;	// 0x5C
} pruEcap;
