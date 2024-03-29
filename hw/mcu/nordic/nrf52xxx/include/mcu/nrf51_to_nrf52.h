/**
 * Copyright (c) 2015 Runtime Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Copyright (c) 2015, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of Nordic Semiconductor ASA nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef NRF51_TO_NRF52_H
#define NRF51_TO_NRF52_H

/*lint ++flb "Enter library region */

/* This file is given to prevent your SW from not compiling with the name changes between nRF51 and nRF52 devices. 
 * It redefines the old nRF51 names into the new ones as long as the functionality is still supported. If the 
 * functionality is gone, there old names are not define, so compilation will fail. Note that also includes macros 
 * from the nrf51_deprecated.h file. */

 
/* IRQ */
/* Several peripherals have been added to several indexes. Names of IRQ handlers and IRQ numbers have changed. */
#define UART0_IRQHandler        UARTE0_UART0_IRQHandler
#define SPI0_TWI0_IRQHandler    SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
#define SPI1_TWI1_IRQHandler    SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
#define ADC_IRQHandler          SAADC_IRQHandler
#define LPCOMP_IRQHandler       COMP_LPCOMP_IRQHandler
#define SWI0_IRQHandler         SWI0_EGU0_IRQHandler
#define SWI1_IRQHandler         SWI1_EGU1_IRQHandler
#define SWI2_IRQHandler         SWI2_EGU2_IRQHandler
#define SWI3_IRQHandler         SWI3_EGU3_IRQHandler
#define SWI4_IRQHandler         SWI4_EGU4_IRQHandler
#define SWI5_IRQHandler         SWI5_EGU5_IRQHandler

#define UART0_IRQn              UARTE0_UART0_IRQn
#define SPI0_TWI0_IRQn          SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn
#define SPI1_TWI1_IRQn          SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn
#define ADC_IRQn                SAADC_IRQn
#define LPCOMP_IRQn             COMP_LPCOMP_IRQn
#define SWI0_IRQn               SWI0_EGU0_IRQn
#define SWI1_IRQn               SWI1_EGU1_IRQn
#define SWI2_IRQn               SWI2_EGU2_IRQn
#define SWI3_IRQn               SWI3_EGU3_IRQn
#define SWI4_IRQn               SWI4_EGU4_IRQn
#define SWI5_IRQn               SWI5_EGU5_IRQn


/* UICR */
/* Register RBPCONF was renamed to APPROTECT. */
#define RBPCONF     APPROTECT

#define UICR_RBPCONF_PALL_Pos           UICR_APPROTECT_PALL_Pos
#define UICR_RBPCONF_PALL_Msk           UICR_APPROTECT_PALL_Msk
#define UICR_RBPCONF_PALL_Enabled       UICR_APPROTECT_PALL_Enabled
#define UICR_RBPCONF_PALL_Disabled      UICR_APPROTECT_PALL_Disabled


/* GPIO */
/* GPIO port was renamed to P0. */
#define NRF_GPIO        NRF_P0
#define NRF_GPIO_BASE   NRF_P0_BASE


/* SPIS */
/* The registers PSELSCK, PSELMISO, PSELMOSI, PSELCSN were restructured into a struct. */
#define PSELSCK       PSEL.SCK
#define PSELMISO      PSEL.MISO
#define PSELMOSI      PSEL.MOSI
#define PSELCSN       PSEL.CSN

/* The registers RXDPTR, MAXRX, AMOUNTRX were restructured into a struct */
#define RXDPTR        RXD.PTR
#define MAXRX         RXD.MAXCNT
#define AMOUNTRX      RXD.AMOUNT

#define SPIS_MAXRX_MAXRX_Pos        SPIS_RXD_MAXCNT_MAXCNT_Pos
#define SPIS_MAXRX_MAXRX_Msk        SPIS_RXD_MAXCNT_MAXCNT_Msk

#define SPIS_AMOUNTRX_AMOUNTRX_Pos  SPIS_RXD_AMOUNT_AMOUNT_Pos
#define SPIS_AMOUNTRX_AMOUNTRX_Msk  SPIS_RXD_AMOUNT_AMOUNT_Msk

/* The registers TXDPTR, MAXTX, AMOUNTTX were restructured into a struct */
#define TXDPTR        TXD.PTR
#define MAXTX         TXD.MAXCNT
#define AMOUNTTX      TXD.AMOUNT

#define SPIS_MAXTX_MAXTX_Pos        SPIS_TXD_MAXCNT_MAXCNT_Pos
#define SPIS_MAXTX_MAXTX_Msk        SPIS_TXD_MAXCNT_MAXCNT_Msk

#define SPIS_AMOUNTTX_AMOUNTTX_Pos  SPIS_TXD_AMOUNT_AMOUNT_Pos
#define SPIS_AMOUNTTX_AMOUNTTX_Msk  SPIS_TXD_AMOUNT_AMOUNT_Msk


/* MPU */
/* Part of MPU module was renamed BPROT, while the rest was eliminated. */
#define NRF_MPU     NRF_BPROT

/* Register DISABLEINDEBUG macros were affected. */
#define MPU_DISABLEINDEBUG_DISABLEINDEBUG_Pos       BPROT_DISABLEINDEBUG_DISABLEINDEBUG_Pos
#define MPU_DISABLEINDEBUG_DISABLEINDEBUG_Msk       BPROT_DISABLEINDEBUG_DISABLEINDEBUG_Msk
#define MPU_DISABLEINDEBUG_DISABLEINDEBUG_Enabled   BPROT_DISABLEINDEBUG_DISABLEINDEBUG_Enabled
#define MPU_DISABLEINDEBUG_DISABLEINDEBUG_Disabled  BPROT_DISABLEINDEBUG_DISABLEINDEBUG_Disabled

/* Registers PROTENSET0 and PROTENSET1 were affected and renamed as CONFIG0 and CONFIG1. */
#define PROTENSET0  CONFIG0
#define PROTENSET1  CONFIG1

#define MPU_PROTENSET1_PROTREG63_Pos        BPROT_CONFIG1_REGION63_Pos
#define MPU_PROTENSET1_PROTREG63_Msk        BPROT_CONFIG1_REGION63_Msk
#define MPU_PROTENSET1_PROTREG63_Disabled   BPROT_CONFIG1_REGION63_Disabled
#define MPU_PROTENSET1_PROTREG63_Enabled    BPROT_CONFIG1_REGION63_Enabled
#define MPU_PROTENSET1_PROTREG63_Set        BPROT_CONFIG1_REGION63_Enabled

#define MPU_PROTENSET1_PROTREG62_Pos        BPROT_CONFIG1_REGION62_Pos
#define MPU_PROTENSET1_PROTREG62_Msk        BPROT_CONFIG1_REGION62_Msk
#define MPU_PROTENSET1_PROTREG62_Disabled   BPROT_CONFIG1_REGION62_Disabled
#define MPU_PROTENSET1_PROTREG62_Enabled    BPROT_CONFIG1_REGION62_Enabled
#define MPU_PROTENSET1_PROTREG62_Set        BPROT_CONFIG1_REGION62_Enabled

#define MPU_PROTENSET1_PROTREG61_Pos        BPROT_CONFIG1_REGION61_Pos
#define MPU_PROTENSET1_PROTREG61_Msk        BPROT_CONFIG1_REGION61_Msk
#define MPU_PROTENSET1_PROTREG61_Disabled   BPROT_CONFIG1_REGION61_Disabled
#define MPU_PROTENSET1_PROTREG61_Enabled    BPROT_CONFIG1_REGION61_Enabled
#define MPU_PROTENSET1_PROTREG61_Set        BPROT_CONFIG1_REGION61_Enabled

#define MPU_PROTENSET1_PROTREG60_Pos        BPROT_CONFIG1_REGION60_Pos
#define MPU_PROTENSET1_PROTREG60_Msk        BPROT_CONFIG1_REGION60_Msk
#define MPU_PROTENSET1_PROTREG60_Disabled   BPROT_CONFIG1_REGION60_Disabled
#define MPU_PROTENSET1_PROTREG60_Enabled    BPROT_CONFIG1_REGION60_Enabled
#define MPU_PROTENSET1_PROTREG60_Set        BPROT_CONFIG1_REGION60_Enabled

#define MPU_PROTENSET1_PROTREG59_Pos        BPROT_CONFIG1_REGION59_Pos
#define MPU_PROTENSET1_PROTREG59_Msk        BPROT_CONFIG1_REGION59_Msk
#define MPU_PROTENSET1_PROTREG59_Disabled   BPROT_CONFIG1_REGION59_Disabled
#define MPU_PROTENSET1_PROTREG59_Enabled    BPROT_CONFIG1_REGION59_Enabled
#define MPU_PROTENSET1_PROTREG59_Set        BPROT_CONFIG1_REGION59_Enabled

#define MPU_PROTENSET1_PROTREG58_Pos        BPROT_CONFIG1_REGION58_Pos
#define MPU_PROTENSET1_PROTREG58_Msk        BPROT_CONFIG1_REGION58_Msk
#define MPU_PROTENSET1_PROTREG58_Disabled   BPROT_CONFIG1_REGION58_Disabled
#define MPU_PROTENSET1_PROTREG58_Enabled    BPROT_CONFIG1_REGION58_Enabled
#define MPU_PROTENSET1_PROTREG58_Set        BPROT_CONFIG1_REGION58_Enabled

#define MPU_PROTENSET1_PROTREG57_Pos        BPROT_CONFIG1_REGION57_Pos
#define MPU_PROTENSET1_PROTREG57_Msk        BPROT_CONFIG1_REGION57_Msk
#define MPU_PROTENSET1_PROTREG57_Disabled   BPROT_CONFIG1_REGION57_Disabled
#define MPU_PROTENSET1_PROTREG57_Enabled    BPROT_CONFIG1_REGION57_Enabled
#define MPU_PROTENSET1_PROTREG57_Set        BPROT_CONFIG1_REGION57_Enabled

#define MPU_PROTENSET1_PROTREG56_Pos        BPROT_CONFIG1_REGION56_Pos
#define MPU_PROTENSET1_PROTREG56_Msk        BPROT_CONFIG1_REGION56_Msk
#define MPU_PROTENSET1_PROTREG56_Disabled   BPROT_CONFIG1_REGION56_Disabled
#define MPU_PROTENSET1_PROTREG56_Enabled    BPROT_CONFIG1_REGION56_Enabled
#define MPU_PROTENSET1_PROTREG56_Set        BPROT_CONFIG1_REGION56_Enabled

#define MPU_PROTENSET1_PROTREG55_Pos        BPROT_CONFIG1_REGION55_Pos
#define MPU_PROTENSET1_PROTREG55_Msk        BPROT_CONFIG1_REGION55_Msk
#define MPU_PROTENSET1_PROTREG55_Disabled   BPROT_CONFIG1_REGION55_Disabled
#define MPU_PROTENSET1_PROTREG55_Enabled    BPROT_CONFIG1_REGION55_Enabled
#define MPU_PROTENSET1_PROTREG55_Set        BPROT_CONFIG1_REGION55_Enabled

#define MPU_PROTENSET1_PROTREG54_Pos        BPROT_CONFIG1_REGION54_Pos
#define MPU_PROTENSET1_PROTREG54_Msk        BPROT_CONFIG1_REGION54_Msk
#define MPU_PROTENSET1_PROTREG54_Disabled   BPROT_CONFIG1_REGION54_Disabled
#define MPU_PROTENSET1_PROTREG54_Enabled    BPROT_CONFIG1_REGION54_Enabled
#define MPU_PROTENSET1_PROTREG54_Set        BPROT_CONFIG1_REGION54_Enabled

#define MPU_PROTENSET1_PROTREG53_Pos        BPROT_CONFIG1_REGION53_Pos
#define MPU_PROTENSET1_PROTREG53_Msk        BPROT_CONFIG1_REGION53_Msk
#define MPU_PROTENSET1_PROTREG53_Disabled   BPROT_CONFIG1_REGION53_Disabled
#define MPU_PROTENSET1_PROTREG53_Enabled    BPROT_CONFIG1_REGION53_Enabled
#define MPU_PROTENSET1_PROTREG53_Set        BPROT_CONFIG1_REGION53_Enabled

#define MPU_PROTENSET1_PROTREG52_Pos        BPROT_CONFIG1_REGION52_Pos
#define MPU_PROTENSET1_PROTREG52_Msk        BPROT_CONFIG1_REGION52_Msk
#define MPU_PROTENSET1_PROTREG52_Disabled   BPROT_CONFIG1_REGION52_Disabled
#define MPU_PROTENSET1_PROTREG52_Enabled    BPROT_CONFIG1_REGION52_Enabled
#define MPU_PROTENSET1_PROTREG52_Set        BPROT_CONFIG1_REGION52_Enabled

#define MPU_PROTENSET1_PROTREG51_Pos        BPROT_CONFIG1_REGION51_Pos
#define MPU_PROTENSET1_PROTREG51_Msk        BPROT_CONFIG1_REGION51_Msk
#define MPU_PROTENSET1_PROTREG51_Disabled   BPROT_CONFIG1_REGION51_Disabled
#define MPU_PROTENSET1_PROTREG51_Enabled    BPROT_CONFIG1_REGION51_Enabled
#define MPU_PROTENSET1_PROTREG51_Set        BPROT_CONFIG1_REGION51_Enabled

#define MPU_PROTENSET1_PROTREG50_Pos        BPROT_CONFIG1_REGION50_Pos
#define MPU_PROTENSET1_PROTREG50_Msk        BPROT_CONFIG1_REGION50_Msk
#define MPU_PROTENSET1_PROTREG50_Disabled   BPROT_CONFIG1_REGION50_Disabled
#define MPU_PROTENSET1_PROTREG50_Enabled    BPROT_CONFIG1_REGION50_Enabled
#define MPU_PROTENSET1_PROTREG50_Set        BPROT_CONFIG1_REGION50_Enabled

#define MPU_PROTENSET1_PROTREG49_Pos        BPROT_CONFIG1_REGION49_Pos
#define MPU_PROTENSET1_PROTREG49_Msk        BPROT_CONFIG1_REGION49_Msk
#define MPU_PROTENSET1_PROTREG49_Disabled   BPROT_CONFIG1_REGION49_Disabled
#define MPU_PROTENSET1_PROTREG49_Enabled    BPROT_CONFIG1_REGION49_Enabled
#define MPU_PROTENSET1_PROTREG49_Set        BPROT_CONFIG1_REGION49_Enabled

#define MPU_PROTENSET1_PROTREG48_Pos        BPROT_CONFIG1_REGION48_Pos
#define MPU_PROTENSET1_PROTREG48_Msk        BPROT_CONFIG1_REGION48_Msk
#define MPU_PROTENSET1_PROTREG48_Disabled   BPROT_CONFIG1_REGION48_Disabled
#define MPU_PROTENSET1_PROTREG48_Enabled    BPROT_CONFIG1_REGION48_Enabled
#define MPU_PROTENSET1_PROTREG48_Set        BPROT_CONFIG1_REGION48_Enabled

#define MPU_PROTENSET1_PROTREG47_Pos        BPROT_CONFIG1_REGION47_Pos
#define MPU_PROTENSET1_PROTREG47_Msk        BPROT_CONFIG1_REGION47_Msk
#define MPU_PROTENSET1_PROTREG47_Disabled   BPROT_CONFIG1_REGION47_Disabled
#define MPU_PROTENSET1_PROTREG47_Enabled    BPROT_CONFIG1_REGION47_Enabled
#define MPU_PROTENSET1_PROTREG47_Set        BPROT_CONFIG1_REGION47_Enabled

#define MPU_PROTENSET1_PROTREG46_Pos        BPROT_CONFIG1_REGION46_Pos
#define MPU_PROTENSET1_PROTREG46_Msk        BPROT_CONFIG1_REGION46_Msk
#define MPU_PROTENSET1_PROTREG46_Disabled   BPROT_CONFIG1_REGION46_Disabled
#define MPU_PROTENSET1_PROTREG46_Enabled    BPROT_CONFIG1_REGION46_Enabled
#define MPU_PROTENSET1_PROTREG46_Set        BPROT_CONFIG1_REGION46_Enabled

#define MPU_PROTENSET1_PROTREG45_Pos        BPROT_CONFIG1_REGION45_Pos
#define MPU_PROTENSET1_PROTREG45_Msk        BPROT_CONFIG1_REGION45_Msk
#define MPU_PROTENSET1_PROTREG45_Disabled   BPROT_CONFIG1_REGION45_Disabled
#define MPU_PROTENSET1_PROTREG45_Enabled    BPROT_CONFIG1_REGION45_Enabled
#define MPU_PROTENSET1_PROTREG45_Set        BPROT_CONFIG1_REGION45_Enabled

#define MPU_PROTENSET1_PROTREG44_Pos        BPROT_CONFIG1_REGION44_Pos
#define MPU_PROTENSET1_PROTREG44_Msk        BPROT_CONFIG1_REGION44_Msk
#define MPU_PROTENSET1_PROTREG44_Disabled   BPROT_CONFIG1_REGION44_Disabled
#define MPU_PROTENSET1_PROTREG44_Enabled    BPROT_CONFIG1_REGION44_Enabled
#define MPU_PROTENSET1_PROTREG44_Set        BPROT_CONFIG1_REGION44_Enabled

#define MPU_PROTENSET1_PROTREG43_Pos        BPROT_CONFIG1_REGION43_Pos
#define MPU_PROTENSET1_PROTREG43_Msk        BPROT_CONFIG1_REGION43_Msk
#define MPU_PROTENSET1_PROTREG43_Disabled   BPROT_CONFIG1_REGION43_Disabled
#define MPU_PROTENSET1_PROTREG43_Enabled    BPROT_CONFIG1_REGION43_Enabled
#define MPU_PROTENSET1_PROTREG43_Set        BPROT_CONFIG1_REGION43_Enabled

#define MPU_PROTENSET1_PROTREG42_Pos        BPROT_CONFIG1_REGION42_Pos
#define MPU_PROTENSET1_PROTREG42_Msk        BPROT_CONFIG1_REGION42_Msk
#define MPU_PROTENSET1_PROTREG42_Disabled   BPROT_CONFIG1_REGION42_Disabled
#define MPU_PROTENSET1_PROTREG42_Enabled    BPROT_CONFIG1_REGION42_Enabled
#define MPU_PROTENSET1_PROTREG42_Set        BPROT_CONFIG1_REGION42_Enabled

#define MPU_PROTENSET1_PROTREG41_Pos        BPROT_CONFIG1_REGION41_Pos
#define MPU_PROTENSET1_PROTREG41_Msk        BPROT_CONFIG1_REGION41_Msk
#define MPU_PROTENSET1_PROTREG41_Disabled   BPROT_CONFIG1_REGION41_Disabled
#define MPU_PROTENSET1_PROTREG41_Enabled    BPROT_CONFIG1_REGION41_Enabled
#define MPU_PROTENSET1_PROTREG41_Set        BPROT_CONFIG1_REGION41_Enabled

#define MPU_PROTENSET1_PROTREG40_Pos        BPROT_CONFIG1_REGION40_Pos
#define MPU_PROTENSET1_PROTREG40_Msk        BPROT_CONFIG1_REGION40_Msk
#define MPU_PROTENSET1_PROTREG40_Disabled   BPROT_CONFIG1_REGION40_Disabled
#define MPU_PROTENSET1_PROTREG40_Enabled    BPROT_CONFIG1_REGION40_Enabled
#define MPU_PROTENSET1_PROTREG40_Set        BPROT_CONFIG1_REGION40_Enabled

#define MPU_PROTENSET1_PROTREG39_Pos        BPROT_CONFIG1_REGION39_Pos
#define MPU_PROTENSET1_PROTREG39_Msk        BPROT_CONFIG1_REGION39_Msk
#define MPU_PROTENSET1_PROTREG39_Disabled   BPROT_CONFIG1_REGION39_Disabled
#define MPU_PROTENSET1_PROTREG39_Enabled    BPROT_CONFIG1_REGION39_Enabled
#define MPU_PROTENSET1_PROTREG39_Set        BPROT_CONFIG1_REGION39_Enabled

#define MPU_PROTENSET1_PROTREG38_Pos        BPROT_CONFIG1_REGION38_Pos
#define MPU_PROTENSET1_PROTREG38_Msk        BPROT_CONFIG1_REGION38_Msk
#define MPU_PROTENSET1_PROTREG38_Disabled   BPROT_CONFIG1_REGION38_Disabled
#define MPU_PROTENSET1_PROTREG38_Enabled    BPROT_CONFIG1_REGION38_Enabled
#define MPU_PROTENSET1_PROTREG38_Set        BPROT_CONFIG1_REGION38_Enabled

#define MPU_PROTENSET1_PROTREG37_Pos        BPROT_CONFIG1_REGION37_Pos
#define MPU_PROTENSET1_PROTREG37_Msk        BPROT_CONFIG1_REGION37_Msk
#define MPU_PROTENSET1_PROTREG37_Disabled   BPROT_CONFIG1_REGION37_Disabled
#define MPU_PROTENSET1_PROTREG37_Enabled    BPROT_CONFIG1_REGION37_Enabled
#define MPU_PROTENSET1_PROTREG37_Set        BPROT_CONFIG1_REGION37_Enabled

#define MPU_PROTENSET1_PROTREG36_Pos        BPROT_CONFIG1_REGION36_Pos
#define MPU_PROTENSET1_PROTREG36_Msk        BPROT_CONFIG1_REGION36_Msk
#define MPU_PROTENSET1_PROTREG36_Disabled   BPROT_CONFIG1_REGION36_Disabled
#define MPU_PROTENSET1_PROTREG36_Enabled    BPROT_CONFIG1_REGION36_Enabled
#define MPU_PROTENSET1_PROTREG36_Set        BPROT_CONFIG1_REGION36_Enabled

#define MPU_PROTENSET1_PROTREG35_Pos        BPROT_CONFIG1_REGION35_Pos
#define MPU_PROTENSET1_PROTREG35_Msk        BPROT_CONFIG1_REGION35_Msk
#define MPU_PROTENSET1_PROTREG35_Disabled   BPROT_CONFIG1_REGION35_Disabled
#define MPU_PROTENSET1_PROTREG35_Enabled    BPROT_CONFIG1_REGION35_Enabled
#define MPU_PROTENSET1_PROTREG35_Set        BPROT_CONFIG1_REGION35_Enabled

#define MPU_PROTENSET1_PROTREG34_Pos        BPROT_CONFIG1_REGION34_Pos
#define MPU_PROTENSET1_PROTREG34_Msk        BPROT_CONFIG1_REGION34_Msk
#define MPU_PROTENSET1_PROTREG34_Disabled   BPROT_CONFIG1_REGION34_Disabled
#define MPU_PROTENSET1_PROTREG34_Enabled    BPROT_CONFIG1_REGION34_Enabled
#define MPU_PROTENSET1_PROTREG34_Set        BPROT_CONFIG1_REGION34_Enabled

#define MPU_PROTENSET1_PROTREG33_Pos        BPROT_CONFIG1_REGION33_Pos
#define MPU_PROTENSET1_PROTREG33_Msk        BPROT_CONFIG1_REGION33_Msk
#define MPU_PROTENSET1_PROTREG33_Disabled   BPROT_CONFIG1_REGION33_Disabled
#define MPU_PROTENSET1_PROTREG33_Enabled    BPROT_CONFIG1_REGION33_Enabled
#define MPU_PROTENSET1_PROTREG33_Set        BPROT_CONFIG1_REGION33_Enabled

#define MPU_PROTENSET1_PROTREG32_Pos        BPROT_CONFIG1_REGION32_Pos
#define MPU_PROTENSET1_PROTREG32_Msk        BPROT_CONFIG1_REGION32_Msk
#define MPU_PROTENSET1_PROTREG32_Disabled   BPROT_CONFIG1_REGION32_Disabled
#define MPU_PROTENSET1_PROTREG32_Enabled    BPROT_CONFIG1_REGION32_Enabled
#define MPU_PROTENSET1_PROTREG32_Set        BPROT_CONFIG1_REGION32_Enabled

#define MPU_PROTENSET0_PROTREG31_Pos        BPROT_CONFIG0_REGION31_Pos
#define MPU_PROTENSET0_PROTREG31_Msk        BPROT_CONFIG0_REGION31_Msk
#define MPU_PROTENSET0_PROTREG31_Disabled   BPROT_CONFIG0_REGION31_Disabled
#define MPU_PROTENSET0_PROTREG31_Enabled    BPROT_CONFIG0_REGION31_Enabled
#define MPU_PROTENSET0_PROTREG31_Set        BPROT_CONFIG0_REGION31_Enabled

#define MPU_PROTENSET0_PROTREG30_Pos        BPROT_CONFIG0_REGION30_Pos
#define MPU_PROTENSET0_PROTREG30_Msk        BPROT_CONFIG0_REGION30_Msk
#define MPU_PROTENSET0_PROTREG30_Disabled   BPROT_CONFIG0_REGION30_Disabled
#define MPU_PROTENSET0_PROTREG30_Enabled    BPROT_CONFIG0_REGION30_Enabled
#define MPU_PROTENSET0_PROTREG30_Set        BPROT_CONFIG0_REGION30_Enabled

#define MPU_PROTENSET0_PROTREG29_Pos        BPROT_CONFIG0_REGION29_Pos
#define MPU_PROTENSET0_PROTREG29_Msk        BPROT_CONFIG0_REGION29_Msk
#define MPU_PROTENSET0_PROTREG29_Disabled   BPROT_CONFIG0_REGION29_Disabled
#define MPU_PROTENSET0_PROTREG29_Enabled    BPROT_CONFIG0_REGION29_Enabled
#define MPU_PROTENSET0_PROTREG29_Set        BPROT_CONFIG0_REGION29_Enabled

#define MPU_PROTENSET0_PROTREG28_Pos        BPROT_CONFIG0_REGION28_Pos
#define MPU_PROTENSET0_PROTREG28_Msk        BPROT_CONFIG0_REGION28_Msk
#define MPU_PROTENSET0_PROTREG28_Disabled   BPROT_CONFIG0_REGION28_Disabled
#define MPU_PROTENSET0_PROTREG28_Enabled    BPROT_CONFIG0_REGION28_Enabled
#define MPU_PROTENSET0_PROTREG28_Set        BPROT_CONFIG0_REGION28_Enabled

#define MPU_PROTENSET0_PROTREG27_Pos        BPROT_CONFIG0_REGION27_Pos
#define MPU_PROTENSET0_PROTREG27_Msk        BPROT_CONFIG0_REGION27_Msk
#define MPU_PROTENSET0_PROTREG27_Disabled   BPROT_CONFIG0_REGION27_Disabled
#define MPU_PROTENSET0_PROTREG27_Enabled    BPROT_CONFIG0_REGION27_Enabled
#define MPU_PROTENSET0_PROTREG27_Set        BPROT_CONFIG0_REGION27_Enabled

#define MPU_PROTENSET0_PROTREG26_Pos        BPROT_CONFIG0_REGION26_Pos
#define MPU_PROTENSET0_PROTREG26_Msk        BPROT_CONFIG0_REGION26_Msk
#define MPU_PROTENSET0_PROTREG26_Disabled   BPROT_CONFIG0_REGION26_Disabled
#define MPU_PROTENSET0_PROTREG26_Enabled    BPROT_CONFIG0_REGION26_Enabled
#define MPU_PROTENSET0_PROTREG26_Set        BPROT_CONFIG0_REGION26_Enabled

#define MPU_PROTENSET0_PROTREG25_Pos        BPROT_CONFIG0_REGION25_Pos
#define MPU_PROTENSET0_PROTREG25_Msk        BPROT_CONFIG0_REGION25_Msk
#define MPU_PROTENSET0_PROTREG25_Disabled   BPROT_CONFIG0_REGION25_Disabled
#define MPU_PROTENSET0_PROTREG25_Enabled    BPROT_CONFIG0_REGION25_Enabled
#define MPU_PROTENSET0_PROTREG25_Set        BPROT_CONFIG0_REGION25_Enabled

#define MPU_PROTENSET0_PROTREG24_Pos        BPROT_CONFIG0_REGION24_Pos
#define MPU_PROTENSET0_PROTREG24_Msk        BPROT_CONFIG0_REGION24_Msk
#define MPU_PROTENSET0_PROTREG24_Disabled   BPROT_CONFIG0_REGION24_Disabled
#define MPU_PROTENSET0_PROTREG24_Enabled    BPROT_CONFIG0_REGION24_Enabled
#define MPU_PROTENSET0_PROTREG24_Set        BPROT_CONFIG0_REGION24_Enabled

#define MPU_PROTENSET0_PROTREG23_Pos        BPROT_CONFIG0_REGION23_Pos
#define MPU_PROTENSET0_PROTREG23_Msk        BPROT_CONFIG0_REGION23_Msk
#define MPU_PROTENSET0_PROTREG23_Disabled   BPROT_CONFIG0_REGION23_Disabled
#define MPU_PROTENSET0_PROTREG23_Enabled    BPROT_CONFIG0_REGION23_Enabled
#define MPU_PROTENSET0_PROTREG23_Set        BPROT_CONFIG0_REGION23_Enabled

#define MPU_PROTENSET0_PROTREG22_Pos        BPROT_CONFIG0_REGION22_Pos
#define MPU_PROTENSET0_PROTREG22_Msk        BPROT_CONFIG0_REGION22_Msk
#define MPU_PROTENSET0_PROTREG22_Disabled   BPROT_CONFIG0_REGION22_Disabled
#define MPU_PROTENSET0_PROTREG22_Enabled    BPROT_CONFIG0_REGION22_Enabled
#define MPU_PROTENSET0_PROTREG22_Set        BPROT_CONFIG0_REGION22_Enabled

#define MPU_PROTENSET0_PROTREG21_Pos        BPROT_CONFIG0_REGION21_Pos
#define MPU_PROTENSET0_PROTREG21_Msk        BPROT_CONFIG0_REGION21_Msk
#define MPU_PROTENSET0_PROTREG21_Disabled   BPROT_CONFIG0_REGION21_Disabled
#define MPU_PROTENSET0_PROTREG21_Enabled    BPROT_CONFIG0_REGION21_Enabled
#define MPU_PROTENSET0_PROTREG21_Set        BPROT_CONFIG0_REGION21_Enabled

#define MPU_PROTENSET0_PROTREG20_Pos        BPROT_CONFIG0_REGION20_Pos
#define MPU_PROTENSET0_PROTREG20_Msk        BPROT_CONFIG0_REGION20_Msk
#define MPU_PROTENSET0_PROTREG20_Disabled   BPROT_CONFIG0_REGION20_Disabled
#define MPU_PROTENSET0_PROTREG20_Enabled    BPROT_CONFIG0_REGION20_Enabled
#define MPU_PROTENSET0_PROTREG20_Set        BPROT_CONFIG0_REGION20_Enabled

#define MPU_PROTENSET0_PROTREG19_Pos        BPROT_CONFIG0_REGION19_Pos
#define MPU_PROTENSET0_PROTREG19_Msk        BPROT_CONFIG0_REGION19_Msk
#define MPU_PROTENSET0_PROTREG19_Disabled   BPROT_CONFIG0_REGION19_Disabled
#define MPU_PROTENSET0_PROTREG19_Enabled    BPROT_CONFIG0_REGION19_Enabled
#define MPU_PROTENSET0_PROTREG19_Set        BPROT_CONFIG0_REGION19_Enabled

#define MPU_PROTENSET0_PROTREG18_Pos        BPROT_CONFIG0_REGION18_Pos
#define MPU_PROTENSET0_PROTREG18_Msk        BPROT_CONFIG0_REGION18_Msk
#define MPU_PROTENSET0_PROTREG18_Disabled   BPROT_CONFIG0_REGION18_Disabled
#define MPU_PROTENSET0_PROTREG18_Enabled    BPROT_CONFIG0_REGION18_Enabled
#define MPU_PROTENSET0_PROTREG18_Set        BPROT_CONFIG0_REGION18_Enabled

#define MPU_PROTENSET0_PROTREG17_Pos        BPROT_CONFIG0_REGION17_Pos
#define MPU_PROTENSET0_PROTREG17_Msk        BPROT_CONFIG0_REGION17_Msk
#define MPU_PROTENSET0_PROTREG17_Disabled   BPROT_CONFIG0_REGION17_Disabled
#define MPU_PROTENSET0_PROTREG17_Enabled    BPROT_CONFIG0_REGION17_Enabled
#define MPU_PROTENSET0_PROTREG17_Set        BPROT_CONFIG0_REGION17_Enabled

#define MPU_PROTENSET0_PROTREG16_Pos        BPROT_CONFIG0_REGION16_Pos
#define MPU_PROTENSET0_PROTREG16_Msk        BPROT_CONFIG0_REGION16_Msk
#define MPU_PROTENSET0_PROTREG16_Disabled   BPROT_CONFIG0_REGION16_Disabled
#define MPU_PROTENSET0_PROTREG16_Enabled    BPROT_CONFIG0_REGION16_Enabled
#define MPU_PROTENSET0_PROTREG16_Set        BPROT_CONFIG0_REGION16_Enabled

#define MPU_PROTENSET0_PROTREG15_Pos        BPROT_CONFIG0_REGION15_Pos
#define MPU_PROTENSET0_PROTREG15_Msk        BPROT_CONFIG0_REGION15_Msk
#define MPU_PROTENSET0_PROTREG15_Disabled   BPROT_CONFIG0_REGION15_Disabled
#define MPU_PROTENSET0_PROTREG15_Enabled    BPROT_CONFIG0_REGION15_Enabled
#define MPU_PROTENSET0_PROTREG15_Set        BPROT_CONFIG0_REGION15_Enabled

#define MPU_PROTENSET0_PROTREG14_Pos        BPROT_CONFIG0_REGION14_Pos
#define MPU_PROTENSET0_PROTREG14_Msk        BPROT_CONFIG0_REGION14_Msk
#define MPU_PROTENSET0_PROTREG14_Disabled   BPROT_CONFIG0_REGION14_Disabled
#define MPU_PROTENSET0_PROTREG14_Enabled    BPROT_CONFIG0_REGION14_Enabled
#define MPU_PROTENSET0_PROTREG14_Set        BPROT_CONFIG0_REGION14_Enabled

#define MPU_PROTENSET0_PROTREG13_Pos        BPROT_CONFIG0_REGION13_Pos
#define MPU_PROTENSET0_PROTREG13_Msk        BPROT_CONFIG0_REGION13_Msk
#define MPU_PROTENSET0_PROTREG13_Disabled   BPROT_CONFIG0_REGION13_Disabled
#define MPU_PROTENSET0_PROTREG13_Enabled    BPROT_CONFIG0_REGION13_Enabled
#define MPU_PROTENSET0_PROTREG13_Set        BPROT_CONFIG0_REGION13_Enabled

#define MPU_PROTENSET0_PROTREG12_Pos        BPROT_CONFIG0_REGION12_Pos
#define MPU_PROTENSET0_PROTREG12_Msk        BPROT_CONFIG0_REGION12_Msk
#define MPU_PROTENSET0_PROTREG12_Disabled   BPROT_CONFIG0_REGION12_Disabled
#define MPU_PROTENSET0_PROTREG12_Enabled    BPROT_CONFIG0_REGION12_Enabled
#define MPU_PROTENSET0_PROTREG12_Set        BPROT_CONFIG0_REGION12_Enabled

#define MPU_PROTENSET0_PROTREG11_Pos        BPROT_CONFIG0_REGION11_Pos
#define MPU_PROTENSET0_PROTREG11_Msk        BPROT_CONFIG0_REGION11_Msk
#define MPU_PROTENSET0_PROTREG11_Disabled   BPROT_CONFIG0_REGION11_Disabled
#define MPU_PROTENSET0_PROTREG11_Enabled    BPROT_CONFIG0_REGION11_Enabled
#define MPU_PROTENSET0_PROTREG11_Set        BPROT_CONFIG0_REGION11_Enabled

#define MPU_PROTENSET0_PROTREG10_Pos        BPROT_CONFIG0_REGION10_Pos
#define MPU_PROTENSET0_PROTREG10_Msk        BPROT_CONFIG0_REGION10_Msk
#define MPU_PROTENSET0_PROTREG10_Disabled   BPROT_CONFIG0_REGION10_Disabled
#define MPU_PROTENSET0_PROTREG10_Enabled    BPROT_CONFIG0_REGION10_Enabled
#define MPU_PROTENSET0_PROTREG10_Set        BPROT_CONFIG0_REGION10_Enabled

#define MPU_PROTENSET0_PROTREG9_Pos        BPROT_CONFIG0_REGION9_Pos
#define MPU_PROTENSET0_PROTREG9_Msk        BPROT_CONFIG0_REGION9_Msk
#define MPU_PROTENSET0_PROTREG9_Disabled   BPROT_CONFIG0_REGION9_Disabled
#define MPU_PROTENSET0_PROTREG9_Enabled    BPROT_CONFIG0_REGION9_Enabled
#define MPU_PROTENSET0_PROTREG9_Set        BPROT_CONFIG0_REGION9_Enabled

#define MPU_PROTENSET0_PROTREG8_Pos        BPROT_CONFIG0_REGION8_Pos
#define MPU_PROTENSET0_PROTREG8_Msk        BPROT_CONFIG0_REGION8_Msk
#define MPU_PROTENSET0_PROTREG8_Disabled   BPROT_CONFIG0_REGION8_Disabled
#define MPU_PROTENSET0_PROTREG8_Enabled    BPROT_CONFIG0_REGION8_Enabled
#define MPU_PROTENSET0_PROTREG8_Set        BPROT_CONFIG0_REGION8_Enabled

#define MPU_PROTENSET0_PROTREG7_Pos        BPROT_CONFIG0_REGION7_Pos
#define MPU_PROTENSET0_PROTREG7_Msk        BPROT_CONFIG0_REGION7_Msk
#define MPU_PROTENSET0_PROTREG7_Disabled   BPROT_CONFIG0_REGION7_Disabled
#define MPU_PROTENSET0_PROTREG7_Enabled    BPROT_CONFIG0_REGION7_Enabled
#define MPU_PROTENSET0_PROTREG7_Set        BPROT_CONFIG0_REGION7_Enabled

#define MPU_PROTENSET0_PROTREG6_Pos        BPROT_CONFIG0_REGION6_Pos
#define MPU_PROTENSET0_PROTREG6_Msk        BPROT_CONFIG0_REGION6_Msk
#define MPU_PROTENSET0_PROTREG6_Disabled   BPROT_CONFIG0_REGION6_Disabled
#define MPU_PROTENSET0_PROTREG6_Enabled    BPROT_CONFIG0_REGION6_Enabled
#define MPU_PROTENSET0_PROTREG6_Set        BPROT_CONFIG0_REGION6_Enabled

#define MPU_PROTENSET0_PROTREG5_Pos        BPROT_CONFIG0_REGION5_Pos
#define MPU_PROTENSET0_PROTREG5_Msk        BPROT_CONFIG0_REGION5_Msk
#define MPU_PROTENSET0_PROTREG5_Disabled   BPROT_CONFIG0_REGION5_Disabled
#define MPU_PROTENSET0_PROTREG5_Enabled    BPROT_CONFIG0_REGION5_Enabled
#define MPU_PROTENSET0_PROTREG5_Set        BPROT_CONFIG0_REGION5_Enabled

#define MPU_PROTENSET0_PROTREG4_Pos        BPROT_CONFIG0_REGION4_Pos
#define MPU_PROTENSET0_PROTREG4_Msk        BPROT_CONFIG0_REGION4_Msk
#define MPU_PROTENSET0_PROTREG4_Disabled   BPROT_CONFIG0_REGION4_Disabled
#define MPU_PROTENSET0_PROTREG4_Enabled    BPROT_CONFIG0_REGION4_Enabled
#define MPU_PROTENSET0_PROTREG4_Set        BPROT_CONFIG0_REGION4_Enabled

#define MPU_PROTENSET0_PROTREG3_Pos        BPROT_CONFIG0_REGION3_Pos
#define MPU_PROTENSET0_PROTREG3_Msk        BPROT_CONFIG0_REGION3_Msk
#define MPU_PROTENSET0_PROTREG3_Disabled   BPROT_CONFIG0_REGION3_Disabled
#define MPU_PROTENSET0_PROTREG3_Enabled    BPROT_CONFIG0_REGION3_Enabled
#define MPU_PROTENSET0_PROTREG3_Set        BPROT_CONFIG0_REGION3_Enabled

#define MPU_PROTENSET0_PROTREG2_Pos        BPROT_CONFIG0_REGION2_Pos
#define MPU_PROTENSET0_PROTREG2_Msk        BPROT_CONFIG0_REGION2_Msk
#define MPU_PROTENSET0_PROTREG2_Disabled   BPROT_CONFIG0_REGION2_Disabled
#define MPU_PROTENSET0_PROTREG2_Enabled    BPROT_CONFIG0_REGION2_Enabled
#define MPU_PROTENSET0_PROTREG2_Set        BPROT_CONFIG0_REGION2_Enabled

#define MPU_PROTENSET0_PROTREG1_Pos        BPROT_CONFIG0_REGION1_Pos
#define MPU_PROTENSET0_PROTREG1_Msk        BPROT_CONFIG0_REGION1_Msk
#define MPU_PROTENSET0_PROTREG1_Disabled   BPROT_CONFIG0_REGION1_Disabled
#define MPU_PROTENSET0_PROTREG1_Enabled    BPROT_CONFIG0_REGION1_Enabled
#define MPU_PROTENSET0_PROTREG1_Set        BPROT_CONFIG0_REGION1_Enabled

#define MPU_PROTENSET0_PROTREG0_Pos        BPROT_CONFIG0_REGION0_Pos
#define MPU_PROTENSET0_PROTREG0_Msk        BPROT_CONFIG0_REGION0_Msk
#define MPU_PROTENSET0_PROTREG0_Disabled   BPROT_CONFIG0_REGION0_Disabled
#define MPU_PROTENSET0_PROTREG0_Enabled    BPROT_CONFIG0_REGION0_Enabled
#define MPU_PROTENSET0_PROTREG0_Set        BPROT_CONFIG0_REGION0_Enabled


/* From nrf51_deprecated.h */

/* NVMC */
/* The register ERASEPROTECTEDPAGE changed name to ERASEPCR0 in the documentation. */
#define ERASEPROTECTEDPAGE      ERASEPCR0


/* IRQ */
/* COMP module was eliminated. Adapted to nrf52 headers. */
#define LPCOMP_COMP_IRQHandler  COMP_LPCOMP_IRQHandler
#define LPCOMP_COMP_IRQn        COMP_LPCOMP_IRQn


/* RADIO */
/* The name of the field SKIPADDR was corrected. Old macros added for compatibility. */
#define RADIO_CRCCNF_SKIP_ADDR_Pos      RADIO_CRCCNF_SKIPADDR_Pos 
#define RADIO_CRCCNF_SKIP_ADDR_Msk      RADIO_CRCCNF_SKIPADDR_Msk 
#define RADIO_CRCCNF_SKIP_ADDR_Include  RADIO_CRCCNF_SKIPADDR_Include 
#define RADIO_CRCCNF_SKIP_ADDR_Skip     RADIO_CRCCNF_SKIPADDR_Skip 


/* FICR */
/* The registers FICR.DEVICEID0 and FICR.DEVICEID1 were renamed into an array. */
#define DEVICEID0       DEVICEID[0]                     
#define DEVICEID1       DEVICEID[1]  

/* The registers FICR.ER0, FICR.ER1, FICR.ER2 and FICR.ER3 were renamed into an array. */
#define ER0             ER[0]                           
#define ER1             ER[1]                          
#define ER2             ER[2]                       
#define ER3             ER[3]                      

/* The registers FICR.IR0, FICR.IR1, FICR.IR2 and FICR.IR3 were renamed into an array. */
#define IR0             IR[0]                         
#define IR1             IR[1]                         
#define IR2             IR[2]                         
#define IR3             IR[3]                          

/* The registers FICR.DEVICEADDR0 and FICR.DEVICEADDR1 were renamed into an array. */
#define DEVICEADDR0     DEVICEADDR[0]                  
#define DEVICEADDR1     DEVICEADDR[1] 


/* PPI */
/* The tasks PPI.TASKS_CHGxEN and PPI.TASKS_CHGxDIS were renamed into an array of structs. */
#define TASKS_CHG0EN     TASKS_CHG[0].EN                    
#define TASKS_CHG0DIS    TASKS_CHG[0].DIS                  
#define TASKS_CHG1EN     TASKS_CHG[1].EN                    
#define TASKS_CHG1DIS    TASKS_CHG[1].DIS                  
#define TASKS_CHG2EN     TASKS_CHG[2].EN                   
#define TASKS_CHG2DIS    TASKS_CHG[2].DIS                  
#define TASKS_CHG3EN     TASKS_CHG[3].EN                    
#define TASKS_CHG3DIS    TASKS_CHG[3].DIS                  

/* The registers PPI.CHx_EEP and PPI.CHx_TEP were renamed into an array of structs. */
#define CH0_EEP          CH[0].EEP                          
#define CH0_TEP          CH[0].TEP                          
#define CH1_EEP          CH[1].EEP                         
#define CH1_TEP          CH[1].TEP                         
#define CH2_EEP          CH[2].EEP                          
#define CH2_TEP          CH[2].TEP                         
#define CH3_EEP          CH[3].EEP                          
#define CH3_TEP          CH[3].TEP                         
#define CH4_EEP          CH[4].EEP                         
#define CH4_TEP          CH[4].TEP                         
#define CH5_EEP          CH[5].EEP                          
#define CH5_TEP          CH[5].TEP                          
#define CH6_EEP          CH[6].EEP                          
#define CH6_TEP          CH[6].TEP                         
#define CH7_EEP          CH[7].EEP                          
#define CH7_TEP          CH[7].TEP                          
#define CH8_EEP          CH[8].EEP                         
#define CH8_TEP          CH[8].TEP                          
#define CH9_EEP          CH[9].EEP                          
#define CH9_TEP          CH[9].TEP                          
#define CH10_EEP         CH[10].EEP                         
#define CH10_TEP         CH[10].TEP                         
#define CH11_EEP         CH[11].EEP                         
#define CH11_TEP         CH[11].TEP                         
#define CH12_EEP         CH[12].EEP                         
#define CH12_TEP         CH[12].TEP                         
#define CH13_EEP         CH[13].EEP                         
#define CH13_TEP         CH[13].TEP                         
#define CH14_EEP         CH[14].EEP                         
#define CH14_TEP         CH[14].TEP                         
#define CH15_EEP         CH[15].EEP                         
#define CH15_TEP         CH[15].TEP                        

/* The registers PPI.CHG0, PPI.CHG1, PPI.CHG2 and PPI.CHG3 were renamed into an array. */
#define CHG0             CHG[0]                            
#define CHG1             CHG[1]                            
#define CHG2             CHG[2]                             
#define CHG3             CHG[3]                           

/* All bitfield macros for the CHGx registers therefore changed name. */
#define PPI_CHG0_CH15_Pos       PPI_CHG_CH15_Pos            
#define PPI_CHG0_CH15_Msk       PPI_CHG_CH15_Msk            
#define PPI_CHG0_CH15_Excluded  PPI_CHG_CH15_Excluded       
#define PPI_CHG0_CH15_Included  PPI_CHG_CH15_Included  
     
#define PPI_CHG0_CH14_Pos       PPI_CHG_CH14_Pos            
#define PPI_CHG0_CH14_Msk       PPI_CHG_CH14_Msk           
#define PPI_CHG0_CH14_Excluded  PPI_CHG_CH14_Excluded       
#define PPI_CHG0_CH14_Included  PPI_CHG_CH14_Included     
  
#define PPI_CHG0_CH13_Pos       PPI_CHG_CH13_Pos            
#define PPI_CHG0_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG0_CH13_Excluded  PPI_CHG_CH13_Excluded      
#define PPI_CHG0_CH13_Included  PPI_CHG_CH13_Included   
    
#define PPI_CHG0_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG0_CH12_Msk       PPI_CHG_CH12_Msk            
#define PPI_CHG0_CH12_Excluded  PPI_CHG_CH12_Excluded       
#define PPI_CHG0_CH12_Included  PPI_CHG_CH12_Included   
    
#define PPI_CHG0_CH11_Pos       PPI_CHG_CH11_Pos            
#define PPI_CHG0_CH11_Msk       PPI_CHG_CH11_Msk            
#define PPI_CHG0_CH11_Excluded  PPI_CHG_CH11_Excluded       
#define PPI_CHG0_CH11_Included  PPI_CHG_CH11_Included  
     
#define PPI_CHG0_CH10_Pos       PPI_CHG_CH10_Pos            
#define PPI_CHG0_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG0_CH10_Excluded  PPI_CHG_CH10_Excluded       
#define PPI_CHG0_CH10_Included  PPI_CHG_CH10_Included  
     
#define PPI_CHG0_CH9_Pos        PPI_CHG_CH9_Pos             
#define PPI_CHG0_CH9_Msk        PPI_CHG_CH9_Msk             
#define PPI_CHG0_CH9_Excluded   PPI_CHG_CH9_Excluded        
#define PPI_CHG0_CH9_Included   PPI_CHG_CH9_Included  
      
#define PPI_CHG0_CH8_Pos        PPI_CHG_CH8_Pos             
#define PPI_CHG0_CH8_Msk        PPI_CHG_CH8_Msk             
#define PPI_CHG0_CH8_Excluded   PPI_CHG_CH8_Excluded        
#define PPI_CHG0_CH8_Included   PPI_CHG_CH8_Included  
      
#define PPI_CHG0_CH7_Pos        PPI_CHG_CH7_Pos             
#define PPI_CHG0_CH7_Msk        PPI_CHG_CH7_Msk             
#define PPI_CHG0_CH7_Excluded   PPI_CHG_CH7_Excluded        
#define PPI_CHG0_CH7_Included   PPI_CHG_CH7_Included  
      
#define PPI_CHG0_CH6_Pos        PPI_CHG_CH6_Pos             
#define PPI_CHG0_CH6_Msk        PPI_CHG_CH6_Msk             
#define PPI_CHG0_CH6_Excluded   PPI_CHG_CH6_Excluded        
#define PPI_CHG0_CH6_Included   PPI_CHG_CH6_Included  
      
#define PPI_CHG0_CH5_Pos        PPI_CHG_CH5_Pos             
#define PPI_CHG0_CH5_Msk        PPI_CHG_CH5_Msk             
#define PPI_CHG0_CH5_Excluded   PPI_CHG_CH5_Excluded       
#define PPI_CHG0_CH5_Included   PPI_CHG_CH5_Included   
     
#define PPI_CHG0_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG0_CH4_Msk        PPI_CHG_CH4_Msk             
#define PPI_CHG0_CH4_Excluded   PPI_CHG_CH4_Excluded       
#define PPI_CHG0_CH4_Included   PPI_CHG_CH4_Included  
     
#define PPI_CHG0_CH3_Pos        PPI_CHG_CH3_Pos             
#define PPI_CHG0_CH3_Msk        PPI_CHG_CH3_Msk            
#define PPI_CHG0_CH3_Excluded   PPI_CHG_CH3_Excluded        
#define PPI_CHG0_CH3_Included   PPI_CHG_CH3_Included  
     
#define PPI_CHG0_CH2_Pos        PPI_CHG_CH2_Pos            
#define PPI_CHG0_CH2_Msk        PPI_CHG_CH2_Msk             
#define PPI_CHG0_CH2_Excluded   PPI_CHG_CH2_Excluded       
#define PPI_CHG0_CH2_Included   PPI_CHG_CH2_Included  
     
#define PPI_CHG0_CH1_Pos        PPI_CHG_CH1_Pos            
#define PPI_CHG0_CH1_Msk        PPI_CHG_CH1_Msk            
#define PPI_CHG0_CH1_Excluded   PPI_CHG_CH1_Excluded        
#define PPI_CHG0_CH1_Included   PPI_CHG_CH1_Included  
     
#define PPI_CHG0_CH0_Pos        PPI_CHG_CH0_Pos            
#define PPI_CHG0_CH0_Msk        PPI_CHG_CH0_Msk            
#define PPI_CHG0_CH0_Excluded   PPI_CHG_CH0_Excluded        
#define PPI_CHG0_CH0_Included   PPI_CHG_CH0_Included  
     
#define PPI_CHG1_CH15_Pos       PPI_CHG_CH15_Pos           
#define PPI_CHG1_CH15_Msk       PPI_CHG_CH15_Msk           
#define PPI_CHG1_CH15_Excluded  PPI_CHG_CH15_Excluded       
#define PPI_CHG1_CH15_Included  PPI_CHG_CH15_Included    
  
#define PPI_CHG1_CH14_Pos       PPI_CHG_CH14_Pos           
#define PPI_CHG1_CH14_Msk       PPI_CHG_CH14_Msk            
#define PPI_CHG1_CH14_Excluded  PPI_CHG_CH14_Excluded      
#define PPI_CHG1_CH14_Included  PPI_CHG_CH14_Included   
    
#define PPI_CHG1_CH13_Pos       PPI_CHG_CH13_Pos           
#define PPI_CHG1_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG1_CH13_Excluded  PPI_CHG_CH13_Excluded      
#define PPI_CHG1_CH13_Included  PPI_CHG_CH13_Included   
   
#define PPI_CHG1_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG1_CH12_Msk       PPI_CHG_CH12_Msk           
#define PPI_CHG1_CH12_Excluded  PPI_CHG_CH12_Excluded      
#define PPI_CHG1_CH12_Included  PPI_CHG_CH12_Included   
  
#define PPI_CHG1_CH11_Pos       PPI_CHG_CH11_Pos            
#define PPI_CHG1_CH11_Msk       PPI_CHG_CH11_Msk           
#define PPI_CHG1_CH11_Excluded  PPI_CHG_CH11_Excluded      
#define PPI_CHG1_CH11_Included  PPI_CHG_CH11_Included    
  
#define PPI_CHG1_CH10_Pos       PPI_CHG_CH10_Pos           
#define PPI_CHG1_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG1_CH10_Excluded  PPI_CHG_CH10_Excluded      
#define PPI_CHG1_CH10_Included  PPI_CHG_CH10_Included   
   
#define PPI_CHG1_CH9_Pos        PPI_CHG_CH9_Pos            
#define PPI_CHG1_CH9_Msk        PPI_CHG_CH9_Msk            
#define PPI_CHG1_CH9_Excluded   PPI_CHG_CH9_Excluded       
#define PPI_CHG1_CH9_Included   PPI_CHG_CH9_Included    
   
#define PPI_CHG1_CH8_Pos        PPI_CHG_CH8_Pos            
#define PPI_CHG1_CH8_Msk        PPI_CHG_CH8_Msk            
#define PPI_CHG1_CH8_Excluded   PPI_CHG_CH8_Excluded       
#define PPI_CHG1_CH8_Included   PPI_CHG_CH8_Included    
   
#define PPI_CHG1_CH7_Pos        PPI_CHG_CH7_Pos             
#define PPI_CHG1_CH7_Msk        PPI_CHG_CH7_Msk            
#define PPI_CHG1_CH7_Excluded   PPI_CHG_CH7_Excluded        
#define PPI_CHG1_CH7_Included   PPI_CHG_CH7_Included     
  
#define PPI_CHG1_CH6_Pos        PPI_CHG_CH6_Pos             
#define PPI_CHG1_CH6_Msk        PPI_CHG_CH6_Msk            
#define PPI_CHG1_CH6_Excluded   PPI_CHG_CH6_Excluded       
#define PPI_CHG1_CH6_Included   PPI_CHG_CH6_Included    
   
#define PPI_CHG1_CH5_Pos        PPI_CHG_CH5_Pos             
#define PPI_CHG1_CH5_Msk        PPI_CHG_CH5_Msk             
#define PPI_CHG1_CH5_Excluded   PPI_CHG_CH5_Excluded       
#define PPI_CHG1_CH5_Included   PPI_CHG_CH5_Included   
    
#define PPI_CHG1_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG1_CH4_Msk        PPI_CHG_CH4_Msk             
#define PPI_CHG1_CH4_Excluded   PPI_CHG_CH4_Excluded        
#define PPI_CHG1_CH4_Included   PPI_CHG_CH4_Included    
    
#define PPI_CHG1_CH3_Pos        PPI_CHG_CH3_Pos            
#define PPI_CHG1_CH3_Msk        PPI_CHG_CH3_Msk             
#define PPI_CHG1_CH3_Excluded   PPI_CHG_CH3_Excluded        
#define PPI_CHG1_CH3_Included   PPI_CHG_CH3_Included   
    
#define PPI_CHG1_CH2_Pos        PPI_CHG_CH2_Pos            
#define PPI_CHG1_CH2_Msk        PPI_CHG_CH2_Msk             
#define PPI_CHG1_CH2_Excluded   PPI_CHG_CH2_Excluded        
#define PPI_CHG1_CH2_Included   PPI_CHG_CH2_Included    
    
#define PPI_CHG1_CH1_Pos        PPI_CHG_CH1_Pos             
#define PPI_CHG1_CH1_Msk        PPI_CHG_CH1_Msk            
#define PPI_CHG1_CH1_Excluded   PPI_CHG_CH1_Excluded        
#define PPI_CHG1_CH1_Included   PPI_CHG_CH1_Included   
    
#define PPI_CHG1_CH0_Pos        PPI_CHG_CH0_Pos             
#define PPI_CHG1_CH0_Msk        PPI_CHG_CH0_Msk            
#define PPI_CHG1_CH0_Excluded   PPI_CHG_CH0_Excluded       
#define PPI_CHG1_CH0_Included   PPI_CHG_CH0_Included   
    
#define PPI_CHG2_CH15_Pos       PPI_CHG_CH15_Pos           
#define PPI_CHG2_CH15_Msk       PPI_CHG_CH15_Msk            
#define PPI_CHG2_CH15_Excluded  PPI_CHG_CH15_Excluded      
#define PPI_CHG2_CH15_Included  PPI_CHG_CH15_Included   
   
#define PPI_CHG2_CH14_Pos       PPI_CHG_CH14_Pos           
#define PPI_CHG2_CH14_Msk       PPI_CHG_CH14_Msk           
#define PPI_CHG2_CH14_Excluded  PPI_CHG_CH14_Excluded       
#define PPI_CHG2_CH14_Included  PPI_CHG_CH14_Included    
  
#define PPI_CHG2_CH13_Pos       PPI_CHG_CH13_Pos           
#define PPI_CHG2_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG2_CH13_Excluded  PPI_CHG_CH13_Excluded       
#define PPI_CHG2_CH13_Included  PPI_CHG_CH13_Included  
    
#define PPI_CHG2_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG2_CH12_Msk       PPI_CHG_CH12_Msk            
#define PPI_CHG2_CH12_Excluded  PPI_CHG_CH12_Excluded      
#define PPI_CHG2_CH12_Included  PPI_CHG_CH12_Included     
  
#define PPI_CHG2_CH11_Pos       PPI_CHG_CH11_Pos           
#define PPI_CHG2_CH11_Msk       PPI_CHG_CH11_Msk           
#define PPI_CHG2_CH11_Excluded  PPI_CHG_CH11_Excluded       
#define PPI_CHG2_CH11_Included  PPI_CHG_CH11_Included   
    
#define PPI_CHG2_CH10_Pos       PPI_CHG_CH10_Pos            
#define PPI_CHG2_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG2_CH10_Excluded  PPI_CHG_CH10_Excluded      
#define PPI_CHG2_CH10_Included  PPI_CHG_CH10_Included   
   
#define PPI_CHG2_CH9_Pos        PPI_CHG_CH9_Pos            
#define PPI_CHG2_CH9_Msk        PPI_CHG_CH9_Msk            
#define PPI_CHG2_CH9_Excluded   PPI_CHG_CH9_Excluded        
#define PPI_CHG2_CH9_Included   PPI_CHG_CH9_Included   
    
#define PPI_CHG2_CH8_Pos        PPI_CHG_CH8_Pos            
#define PPI_CHG2_CH8_Msk        PPI_CHG_CH8_Msk            
#define PPI_CHG2_CH8_Excluded   PPI_CHG_CH8_Excluded       
#define PPI_CHG2_CH8_Included   PPI_CHG_CH8_Included  
      
#define PPI_CHG2_CH7_Pos        PPI_CHG_CH7_Pos            
#define PPI_CHG2_CH7_Msk        PPI_CHG_CH7_Msk            
#define PPI_CHG2_CH7_Excluded   PPI_CHG_CH7_Excluded       
#define PPI_CHG2_CH7_Included   PPI_CHG_CH7_Included   
    
#define PPI_CHG2_CH6_Pos        PPI_CHG_CH6_Pos            
#define PPI_CHG2_CH6_Msk        PPI_CHG_CH6_Msk            
#define PPI_CHG2_CH6_Excluded   PPI_CHG_CH6_Excluded       
#define PPI_CHG2_CH6_Included   PPI_CHG_CH6_Included   
    
#define PPI_CHG2_CH5_Pos        PPI_CHG_CH5_Pos            
#define PPI_CHG2_CH5_Msk        PPI_CHG_CH5_Msk            
#define PPI_CHG2_CH5_Excluded   PPI_CHG_CH5_Excluded       
#define PPI_CHG2_CH5_Included   PPI_CHG_CH5_Included  
      
#define PPI_CHG2_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG2_CH4_Msk        PPI_CHG_CH4_Msk             
#define PPI_CHG2_CH4_Excluded   PPI_CHG_CH4_Excluded        
#define PPI_CHG2_CH4_Included   PPI_CHG_CH4_Included   
    
#define PPI_CHG2_CH3_Pos        PPI_CHG_CH3_Pos            
#define PPI_CHG2_CH3_Msk        PPI_CHG_CH3_Msk            
#define PPI_CHG2_CH3_Excluded   PPI_CHG_CH3_Excluded       
#define PPI_CHG2_CH3_Included   PPI_CHG_CH3_Included   
    
#define PPI_CHG2_CH2_Pos        PPI_CHG_CH2_Pos            
#define PPI_CHG2_CH2_Msk        PPI_CHG_CH2_Msk           
#define PPI_CHG2_CH2_Excluded   PPI_CHG_CH2_Excluded       
#define PPI_CHG2_CH2_Included   PPI_CHG_CH2_Included   
    
#define PPI_CHG2_CH1_Pos        PPI_CHG_CH1_Pos             
#define PPI_CHG2_CH1_Msk        PPI_CHG_CH1_Msk             
#define PPI_CHG2_CH1_Excluded   PPI_CHG_CH1_Excluded       
#define PPI_CHG2_CH1_Included   PPI_CHG_CH1_Included   
    
#define PPI_CHG2_CH0_Pos        PPI_CHG_CH0_Pos            
#define PPI_CHG2_CH0_Msk        PPI_CHG_CH0_Msk            
#define PPI_CHG2_CH0_Excluded   PPI_CHG_CH0_Excluded       
#define PPI_CHG2_CH0_Included   PPI_CHG_CH0_Included    
    
#define PPI_CHG3_CH15_Pos       PPI_CHG_CH15_Pos           
#define PPI_CHG3_CH15_Msk       PPI_CHG_CH15_Msk           
#define PPI_CHG3_CH15_Excluded  PPI_CHG_CH15_Excluded     
#define PPI_CHG3_CH15_Included  PPI_CHG_CH15_Included  
    
#define PPI_CHG3_CH14_Pos       PPI_CHG_CH14_Pos          
#define PPI_CHG3_CH14_Msk       PPI_CHG_CH14_Msk           
#define PPI_CHG3_CH14_Excluded  PPI_CHG_CH14_Excluded      
#define PPI_CHG3_CH14_Included  PPI_CHG_CH14_Included   
    
#define PPI_CHG3_CH13_Pos       PPI_CHG_CH13_Pos           
#define PPI_CHG3_CH13_Msk       PPI_CHG_CH13_Msk            
#define PPI_CHG3_CH13_Excluded  PPI_CHG_CH13_Excluded      
#define PPI_CHG3_CH13_Included  PPI_CHG_CH13_Included   
   
#define PPI_CHG3_CH12_Pos       PPI_CHG_CH12_Pos            
#define PPI_CHG3_CH12_Msk       PPI_CHG_CH12_Msk            
#define PPI_CHG3_CH12_Excluded  PPI_CHG_CH12_Excluded       
#define PPI_CHG3_CH12_Included  PPI_CHG_CH12_Included   
    
#define PPI_CHG3_CH11_Pos       PPI_CHG_CH11_Pos            
#define PPI_CHG3_CH11_Msk       PPI_CHG_CH11_Msk            
#define PPI_CHG3_CH11_Excluded  PPI_CHG_CH11_Excluded      
#define PPI_CHG3_CH11_Included  PPI_CHG_CH11_Included   
    
#define PPI_CHG3_CH10_Pos       PPI_CHG_CH10_Pos            
#define PPI_CHG3_CH10_Msk       PPI_CHG_CH10_Msk            
#define PPI_CHG3_CH10_Excluded  PPI_CHG_CH10_Excluded      
#define PPI_CHG3_CH10_Included  PPI_CHG_CH10_Included   
   
#define PPI_CHG3_CH9_Pos        PPI_CHG_CH9_Pos            
#define PPI_CHG3_CH9_Msk        PPI_CHG_CH9_Msk            
#define PPI_CHG3_CH9_Excluded   PPI_CHG_CH9_Excluded       
#define PPI_CHG3_CH9_Included   PPI_CHG_CH9_Included   
    
#define PPI_CHG3_CH8_Pos        PPI_CHG_CH8_Pos            
#define PPI_CHG3_CH8_Msk        PPI_CHG_CH8_Msk             
#define PPI_CHG3_CH8_Excluded   PPI_CHG_CH8_Excluded       
#define PPI_CHG3_CH8_Included   PPI_CHG_CH8_Included   
    
#define PPI_CHG3_CH7_Pos        PPI_CHG_CH7_Pos             
#define PPI_CHG3_CH7_Msk        PPI_CHG_CH7_Msk            
#define PPI_CHG3_CH7_Excluded   PPI_CHG_CH7_Excluded        
#define PPI_CHG3_CH7_Included   PPI_CHG_CH7_Included   
    
#define PPI_CHG3_CH6_Pos        PPI_CHG_CH6_Pos             
#define PPI_CHG3_CH6_Msk        PPI_CHG_CH6_Msk             
#define PPI_CHG3_CH6_Excluded   PPI_CHG_CH6_Excluded       
#define PPI_CHG3_CH6_Included   PPI_CHG_CH6_Included   
     
#define PPI_CHG3_CH5_Pos        PPI_CHG_CH5_Pos             
#define PPI_CHG3_CH5_Msk        PPI_CHG_CH5_Msk             
#define PPI_CHG3_CH5_Excluded   PPI_CHG_CH5_Excluded        
#define PPI_CHG3_CH5_Included   PPI_CHG_CH5_Included  
     
#define PPI_CHG3_CH4_Pos        PPI_CHG_CH4_Pos             
#define PPI_CHG3_CH4_Msk        PPI_CHG_CH4_Msk            
#define PPI_CHG3_CH4_Excluded   PPI_CHG_CH4_Excluded        
#define PPI_CHG3_CH4_Included   PPI_CHG_CH4_Included  
      
#define PPI_CHG3_CH3_Pos        PPI_CHG_CH3_Pos             
#define PPI_CHG3_CH3_Msk        PPI_CHG_CH3_Msk            
#define PPI_CHG3_CH3_Excluded   PPI_CHG_CH3_Excluded        
#define PPI_CHG3_CH3_Included   PPI_CHG_CH3_Included   
     
#define PPI_CHG3_CH2_Pos        PPI_CHG_CH2_Pos             
#define PPI_CHG3_CH2_Msk        PPI_CHG_CH2_Msk             
#define PPI_CHG3_CH2_Excluded   PPI_CHG_CH2_Excluded        
#define PPI_CHG3_CH2_Included   PPI_CHG_CH2_Included  
     
#define PPI_CHG3_CH1_Pos        PPI_CHG_CH1_Pos             
#define PPI_CHG3_CH1_Msk        PPI_CHG_CH1_Msk             
#define PPI_CHG3_CH1_Excluded   PPI_CHG_CH1_Excluded        
#define PPI_CHG3_CH1_Included   PPI_CHG_CH1_Included  
      
#define PPI_CHG3_CH0_Pos        PPI_CHG_CH0_Pos             
#define PPI_CHG3_CH0_Msk        PPI_CHG_CH0_Msk             
#define PPI_CHG3_CH0_Excluded   PPI_CHG_CH0_Excluded        
#define PPI_CHG3_CH0_Included   PPI_CHG_CH0_Included        




/*lint --flb "Leave library region" */

#endif /* NRF51_TO_NRF52_H */

