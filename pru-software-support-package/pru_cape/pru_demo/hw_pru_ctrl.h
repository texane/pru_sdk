/*
 * hw_pru_ctrl.h
 *
 * This file contains the register definititions for the PRU_CTRL submodule.
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef HW_PRU_CTRL_H_
#define HW_PRU_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************************************
* Register Definitions 
****************************************************************************************************/

#define PRU_CTRL_CONTROL                                            (0x0U)
#define PRU_CTRL_STATUS                                             (0x4U)
#define PRU_CTRL_WAKEUP_EN                                          (0x8U)
#define PRU_CTRL_CYCLE                                              (0xcU)
#define PRU_CTRL_STALL                                              (0x10U)
#define PRU_CTRL_CTBIR0                                             (0x20U)
#define PRU_CTRL_CTBIR1                                             (0x24U)
#define PRU_CTRL_CTPPR0                                             (0x28U)
#define PRU_CTRL_CTPPR1                                             (0x2cU)

/****************************************************************************************************
* Field Definition Macros 
****************************************************************************************************/

#define PRU_CTRL_CONTROL_SOFT_RST_N_SHIFT                                               (0U)
#define PRU_CTRL_CONTROL_SOFT_RST_N_MASK                                                (0x00000001U)

#define PRU_CTRL_CONTROL_ENABLE_SHIFT                                                   (1U)
#define PRU_CTRL_CONTROL_ENABLE_MASK                                                    (0x00000002U)

#define PRU_CTRL_CONTROL_SLEEPING_SHIFT                                                 (2U)
#define PRU_CTRL_CONTROL_SLEEPING_MASK                                                  (0x00000004U)

#define PRU_CTRL_CONTROL_COUNTER_ENABLE_SHIFT                                           (3U)
#define PRU_CTRL_CONTROL_COUNTER_ENABLE_MASK                                            (0x00000008U)

#define PRU_CTRL_CONTROL_SINGLE_STEP_SHIFT                                              (8U)
#define PRU_CTRL_CONTROL_SINGLE_STEP_MASK                                               (0x00000100U)

#define PRU_CTRL_CONTROL_BIG_ENDIAN_SHIFT                                               (14U)
#define PRU_CTRL_CONTROL_BIG_ENDIAN_MASK                                                (0x00004000U)

#define PRU_CTRL_CONTROL_RUNSTATE_SHIFT                                                 (15U)
#define PRU_CTRL_CONTROL_RUNSTATE_MASK                                                  (0x00008000U)

#define PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_SHIFT                                         (16U)
#define PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_MASK                                          (0xffff0000U)

#define PRU_CTRL_STATUS_PCOUNTER_SHIFT                                                  (0U)
#define PRU_CTRL_STATUS_PCOUNTER_MASK                                                   (0x0000ffffU)

#define PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_SHIFT                                        (0U)
#define PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_MASK                                         (0xffffffffU)

#define PRU_CTRL_CYCLE_CYCLECOUNT_SHIFT                                                 (0U)
#define PRU_CTRL_CYCLE_CYCLECOUNT_MASK                                                  (0xffffffffU)

#define PRU_CTRL_STALL_STALLCOUNT_SHIFT                                                 (0U)
#define PRU_CTRL_STALL_STALLCOUNT_MASK                                                  (0xffffffffU)

#define PRU_CTRL_CTBIR0_C24_BLK_INDEX_SHIFT                                             (0U)
#define PRU_CTRL_CTBIR0_C24_BLK_INDEX_MASK                                              (0x000000ffU)

#define PRU_CTRL_CTBIR0_C25_BLK_INDEX_SHIFT                                             (16U)
#define PRU_CTRL_CTBIR0_C25_BLK_INDEX_MASK                                              (0x00ff0000U)

#define PRU_CTRL_CTBIR1_C26_BLK_INDEX_SHIFT                                             (0U)
#define PRU_CTRL_CTBIR1_C26_BLK_INDEX_MASK                                              (0x000000ffU)

#define PRU_CTRL_CTBIR1_C27_BLK_INDEX_SHIFT                                             (16U)
#define PRU_CTRL_CTBIR1_C27_BLK_INDEX_MASK                                              (0x00ff0000U)

#define PRU_CTRL_CTPPR0_C28_POINTER_SHIFT                                               (0U)
#define PRU_CTRL_CTPPR0_C28_POINTER_MASK                                                (0x0000ffffU)

#define PRU_CTRL_CTPPR0_C29_POINTER_SHIFT                                               (16U)
#define PRU_CTRL_CTPPR0_C29_POINTER_MASK                                                (0xffff0000U)

#define PRU_CTRL_CTPPR1_C30_POINTER_SHIFT                                               (0U)
#define PRU_CTRL_CTPPR1_C30_POINTER_MASK                                                (0x0000ffffU)

#define PRU_CTRL_CTPPR1_C31_POINTER_SHIFT                                               (16U)
#define PRU_CTRL_CTPPR1_C31_POINTER_MASK                                                (0xffff0000U)

#ifdef __cplusplus
}
#endif
#endif  /* HW_PRU_CTRL_H_ */
