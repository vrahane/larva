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

/*----------------------------------------------------------------------------
 * Copyright (c) 1999-2009 KEIL, 2009-2013 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

        .file   "HAL_CM4.S"
        .syntax unified

/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

        .thumb

        .section ".text"
        .align  2

/*--------------------------- os_set_env ------------------------------------*/
#       void os_set_env (void);
        /* Switch to Unprivileged/Privileged Thread mode, use PSP. */

        .thumb_func
        .type   os_set_env, %function
        .global os_set_env
os_set_env:
        .fnstart
        .cantunwind

        MOV     R0,SP                   /* PSP = MSP */
        MSR     PSP,R0
        LDR     R0,=os_flags
        LDRB    R0,[R0]
        LSLS    R0,#31
        ITE     NE
        MOVNE   R0,#0x02                /* Privileged Thread mode, use PSP */
        MOVEQ   R0,#0x03                /* Unprivileged Thread mode, use PSP */
        MSR     CONTROL,R0
        ISB
        BX      LR

        .fnend
        .size   os_set_env, .-os_set_env
/*--------------------------- os_set_env ------------------------------------*/


/*--------------------------- os_arch_init_task_stack------------------------*/
#       void os_arch_init_task_stack(os_stack_t *sf);
# NOTE: This function only stores R4 through R11 on stack. The reason we do 
# this is that the application may have stored some values in some of the
# registers and we want to preserve those values (position independent code
# is a good example). The other registers are handled in the C startup code.
        .thumb_func
        .type   os_arch_init_task_stack, %function
        .global os_arch_init_task_stack
os_arch_init_task_stack:
        .fnstart

        STMIA   R0,{R4-R11}
        BX      LR

        .fnend
        .size   os_arch_init_task_stack, .-os_arch_init_task_stack
/*--------------------------- os_set_env ------------------------------------*/


/*-------------------------- SVC_Handler ------------------------------------*/

#       void SVC_Handler (void);

        .thumb_func
        .type   SVC_Handler, %function
        .global SVC_Handler
SVC_Handler:
        .fnstart
        .cantunwind

        MRS     R0,PSP                  /* Read PSP */
        LDR     R1,[R0,#24]             /* Read Saved PC from Stack */
        LDRB    R1,[R1,#-2]             /* Load SVC Number */
        CBNZ    R1,SVC_User

        LDM     R0,{R0-R3,R12}          /* Read R0-R3,R12 from stack */
        PUSH    {R4,LR}                 /* Save EXC_RETURN */
        BLX     R12                     /* Call SVC Function */
        POP     {R4,LR}                 /* Restore EXC_RETURN */

        MRS     R12,PSP                 /* Read PSP */
        STM     R12,{R0-R2}             /* Store return values */
        BX      LR                      /* Return from interrupt */

        /*------------------- User SVC ------------------------------*/
SVC_User:
        PUSH    {R4,LR}                 /* Save EXC_RETURN */
        LDR     R2,=SVC_Count
        LDR     R2,[R2]
        CMP     R1,R2
        BHI     SVC_Done                /* Overflow */

        LDR     R4,=SVC_Table-4
        LDR     R4,[R4,R1,LSL #2]       /* Load SVC Function Address */

        LDM     R0,{R0-R3,R12}          /* Read R0-R3,R12 from stack */
        BLX     R4                      /* Call SVC Function */

        MRS     R12,PSP
        STM     R12,{R0-R3}             /* Function return values */
SVC_Done:
        POP     {R4,LR}                 /* Restore EXC_RETURN */
        BX      LR                      /* Return from interrupt */

        .fnend
        .size   SVC_Handler, .-SVC_Handler


/*-------------------------- PendSV_Handler ---------------------------------*/

#       void PendSV_Handler (void);

        .thumb_func
        .type   PendSV_Handler, %function
        .global PendSV_Handler
PendSV_Handler:
        .fnstart
        .cantunwind

        LDR     R3,=g_os_run_list       /* Get highest priority task ready to run */
        LDR     R2,[R3]                 /* Store in R2 */
        LDR     R3,=g_current_task      /* Get current task */
        LDR     R1,[R3]                 /* Current task in R1 */
        CMP     R1,R2
        IT      EQ
        BXEQ    LR                      /* RETI, no task switch */

        MRS     R12,PSP                 /* Read PSP */
        STMDB   R12!,{R4-R11}           /* Save Old context */
        STR     R12,[R1,#0]             /* Update stack pointer in current task */
        STR     R2,[R3]                 /* g_current_task = highest ready */

        LDR     R12,[R2,#0]             /* get stack pointer of task we will start */
        LDMIA   R12!,{R4-R11}           /* Restore New Context */
        MSR     PSP,R12                 /* Write PSP */
        BX      LR                      /* Return to Thread Mode */

        .fnend
        .size   PendSV_Handler, .-PendSV_Handler


/*-------------------------- SysTick_Handler --------------------------------*/

#       void SysTick_Handler (void);
        .thumb_func
        .type   SysTick_Handler, %function
        .global SysTick_Handler
SysTick_Handler:
        .fnstart
        .cantunwind

        PUSH    {R4,LR}                 /* Save EXC_RETURN */
        BL      timer_handler
        POP     {R4,LR}                 /* Restore EXC_RETURN */
        BX      LR

        .fnend
        .size   SysTick_Handler, .-SysTick_Handler

        .end

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
