/*
    FreeRTOS V6.0.2 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS eBook                                  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the PPC405 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include "xtime_l.h"
#include "xintc.h"
#include "xintc_i.h"

/*-----------------------------------------------------------*/

/* Definitions to set the initial MSR of each task. */
#define portCRITICAL_INTERRUPT_ENABLE (1UL << 17UL)
#define portEXTERNAL_INTERRUPT_ENABLE (1UL << 15UL)
#define portMACHINE_CHECK_ENABLE (1UL << 12UL)

#if configUSE_FPU == 1
#define portAPU_PRESENT (1UL << 25UL)
#define portFCM_FPU_PRESENT (1UL << 13UL)
#else
#define portAPU_PRESENT (0UL)
#define portFCM_FPU_PRESENT (0UL)
#endif

#define portINITIAL_MSR \
    (portCRITICAL_INTERRUPT_ENABLE | portEXTERNAL_INTERRUPT_ENABLE | portMACHINE_CHECK_ENABLE | portAPU_PRESENT | \
        portFCM_FPU_PRESENT)

extern const unsigned _SDA_BASE_;
extern const unsigned _SDA2_BASE_;

/*-----------------------------------------------------------*/

/*
 * Setup the system timer to generate the tick interrupt.
 */
static void prvSetupTimerInterrupt(void);

/*
 * The handler for the tick interrupt - defined in portasm.s.
 */
extern void vPortTickISR(void);

/*
 * The handler for the yield function - defined in portasm.s.
 */
extern void vPortYield(void);

/*
 * Function to start the scheduler running by starting the highest
 * priority task that has thus far been created.
 */
extern void vPortStartFirstTask(void);

/*-----------------------------------------------------------*/

/* Structure used to hold the state of the interrupt controller. */
static XIntc xInterruptController;

/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if the task had been
 * interrupted.
 *
 * See the header file portable.h.
 */
portSTACK_TYPE* pxPortInitialiseStack(portSTACK_TYPE* pxTopOfStack, pdTASK_CODE pxCode, void* pvParameters)
{
    /* Place a known value at the bottom of the stack for debugging. */
    *pxTopOfStack = 0xDEADBEEF;
    pxTopOfStack--;

    /* EABI stack frame. */
    pxTopOfStack -= 20; /* Previous backchain and LR, R31 to R4 inclusive. */

    /* Parameters in R13. */
    *pxTopOfStack = (portSTACK_TYPE)&_SDA_BASE_; /* address of the first small data area */
    pxTopOfStack -= 10;

    /* Parameters in R3. */
    *pxTopOfStack = (portSTACK_TYPE)pvParameters;
    pxTopOfStack--;

    /* Parameters in R2. */
    *pxTopOfStack = (portSTACK_TYPE)&_SDA2_BASE_; /* address of the second small data area */
    pxTopOfStack--;

    /* R1 is the stack pointer so is omitted. */

    *pxTopOfStack = 0x10000001UL;
    ; /* R0. */
    pxTopOfStack--;
    *pxTopOfStack = 0x00000000UL; /* USPRG0. */
    pxTopOfStack--;
    *pxTopOfStack = 0x00000000UL; /* CR. */
    pxTopOfStack--;
    *pxTopOfStack = 0x00000000UL; /* XER. */
    pxTopOfStack--;
    *pxTopOfStack = 0x00000000UL; /* CTR. */
    pxTopOfStack--;
    *pxTopOfStack = (portSTACK_TYPE)vPortEndScheduler; /* LR. */
    pxTopOfStack--;
    *pxTopOfStack = (portSTACK_TYPE)pxCode; /* SRR0. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_MSR; /* SRR1. */
    pxTopOfStack--;
    *pxTopOfStack = (portSTACK_TYPE)vPortEndScheduler; /* Next LR. */
    pxTopOfStack--;
    *pxTopOfStack = 0x00000000UL; /* Backchain. */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler(void)
{
    prvSetupTimerInterrupt();
    XExc_RegisterHandler(XEXC_ID_SYSTEM_CALL, (XExceptionHandler)vPortYield, (void*)0);
    vPortStartFirstTask();

    /* Should not get here as the tasks are now running! */
    return pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
    /* Not implemented. */
    for(;;)
        ;
}
/*-----------------------------------------------------------*/

/*
 * Hardware initialisation to generate the RTOS tick.
 */
static void prvSetupTimerInterrupt(void)
{
    const unsigned long ulInterval = ((configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1UL);

    XTime_PITClearInterrupt();
    XTime_FITClearInterrupt();
    XTime_WDTClearInterrupt();
    XTime_WDTDisableInterrupt();
    XTime_FITDisableInterrupt();

    XExc_RegisterHandler(XEXC_ID_PIT_INT, (XExceptionHandler)vPortTickISR, (void*)0);

    XTime_PITEnableAutoReload();
    XTime_PITSetInterval(ulInterval);
    XTime_PITEnableInterrupt();
}
/*-----------------------------------------------------------*/

void vPortISRHandler(void* pvNullDoNotUse)
{
    unsigned long ulInterruptStatus, ulInterruptMask = 1UL;
    portBASE_TYPE xInterruptNumber;
    XIntc_Config* pxInterruptController;
    XIntc_VectorTableEntry* pxTable;

    /* Just to remove compiler warning. */
    (void)pvNullDoNotUse;

    /* Get the configuration by using the device ID - in this case it is
   assumed that only one interrupt controller is being used. */
    pxInterruptController = &XIntc_ConfigTable[XPAR_XPS_INTC_0_DEVICE_ID];

    /* Which interrupts are pending? */
    ulInterruptStatus = XIntc_mGetIntrStatus(pxInterruptController->BaseAddress);

    for(xInterruptNumber = 0; xInterruptNumber < XPAR_INTC_MAX_NUM_INTR_INPUTS; xInterruptNumber++) {
        if(ulInterruptStatus & 0x01UL) {
            /* Clear the pending interrupt. */
            XIntc_mAckIntr(pxInterruptController->BaseAddress, ulInterruptMask);

            /* Call the registered handler. */
            pxTable = &(pxInterruptController->HandlerTable[xInterruptNumber]);
            pxTable->Handler(pxTable->CallBackRef);
        }

        /* Check the next interrupt. */
        ulInterruptMask <<= 0x01UL;
        ulInterruptStatus >>= 0x01UL;

        /* Have we serviced all interrupts? */
        if(ulInterruptStatus == 0UL) {
            break;
        }
    }
}
/*-----------------------------------------------------------*/

void vPortSetupInterruptController(void)
{
    extern void vPortISRWrapper(void);

    /* Perform all library calls necessary to initialise the exception table
   and interrupt controller.  This assumes only one interrupt controller is in
   use. */
    XExc_mDisableExceptions(XEXC_NON_CRITICAL);
    XExc_Init();

    /* The library functions save the context - we then jump to a wrapper to
   save the stack into the TCB.  The wrapper then calls the handler defined
   above. */
    XExc_RegisterHandler(XEXC_ID_NON_CRITICAL_INT, (XExceptionHandler)vPortISRWrapper, NULL);
    XIntc_Initialize(&xInterruptController, XPAR_XPS_INTC_0_DEVICE_ID);
    XIntc_Start(&xInterruptController, XIN_REAL_MODE);
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortInstallInterruptHandler(unsigned char ucInterruptID,
    XInterruptHandler pxHandler,
    void* pvCallBackRef)
{
    portBASE_TYPE xReturn = pdFAIL;

    /* This function is defined here so the scope of xInterruptController can
   remain within this file. */

    if(XST_SUCCESS == XIntc_Connect(&xInterruptController, ucInterruptID, pxHandler, pvCallBackRef)) {
        XIntc_Enable(&xInterruptController, ucInterruptID);
        xReturn = pdPASS;
    }

    return xReturn;
}
