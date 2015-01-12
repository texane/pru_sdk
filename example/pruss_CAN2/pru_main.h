/* 
 * File:   pru_main.h
 * Author: longqi
 *
 * Created on January 12, 2015, 10:44 AM
 */

#ifndef PRU_MAIN_H
#define	PRU_MAIN_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define DCAN_NO_INT_PENDING               (0x00000000u)
#define DCAN_IN_CLK                       (24000000u)
#define DCAN_ERROR_OCCURED                (0x8000u)
#define DCAN_BIT_RATE                     (1000000u)
#define CAN_TX_MSG_EXTD_ID                (0x1000u)
  //*******server part*******
#define CAN_TX_MSG_STD_ID                 (0x03u)
#define CAN_RX_MSG_STD_ID                 (0x02u)
  //**************

  //*******client part*******
#define CAN_TX_MSG_STD_ID                 (0x02u)
#define CAN_RX_MSG_STD_ID                 (0x03u)
  //**************


#define DCAN_ID_MASK           (0x0fu) // the low 4 bits of CAN_ID will be the identifier 

#define REG_Len                 (4)
#define MEM_SIZE                (20)


#ifdef	__cplusplus
}
#endif

#endif	/* PRU_MAIN_H */

