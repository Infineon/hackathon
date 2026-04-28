/*****************************************************************************
* File Name        : main.c
*
* Description      : This is the source code for DEEPCRAFT Deploy Audio Example.
*
* Related Document : See README.md
*
******************************************************************************
* (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
* 
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*****************************************************************************/

/*******************************************************************************
* Header File
*******************************************************************************/

#include "cybsp.h"

#ifdef ML_DEEPCRAFT_CM55
#include "stdlib.h"
#include "retarget_io_init.h"

#include "audio.h"
#endif /* ML_DEEPCRAFT_CM55 */

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef ML_DEEPCRAFT_CM55
static cy_rslt_t system_init(void);
static void cm55_ml_deepcraft_task(void);
#endif /* ML_DEEPCRAFT_CM55 */

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  This is the main function. It initializes the system on the CM55 CPU.
*  If the model inferencing is set to CM55 + U55, it starts the Deploy Audio
*  application, else, it enters Deepsleep mode.
*
* Parameters:
*  None
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

#ifdef ML_DEEPCRAFT_CM55
    /* If ML_DEEPCRAFT_CPU is set as CM55, start the task */
    cm55_ml_deepcraft_task();
#else

    for (;;)
    {
        /* If ML_DEEPCRAFT_CPU is set as CM33, put the CM55 to sleep */
        Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
#endif /* ML_DEEPCRAFT_CM55 */
}

#ifdef ML_DEEPCRAFT_CM55
/*******************************************************************************
* Function Name: system_init
********************************************************************************
* Summary:
*  Initializes the neural network based on the DEEPCRAFT model and the
*  DEEPCRAFT pre-processor and initializes the PDM PCM block.
*
* Parameters:
*  None
*
* Returns:
*  The status of the initialization.
*
*******************************************************************************/
static cy_rslt_t system_init(void)
{
    cy_rslt_t result;

    /* Initialize DEEPCRAFT pre-processing library */
    IMAI_init();

    /* Configure PDM, PDM clocks, and PDM event */
    result = pdm_init();

    return result;
}

/*******************************************************************************
* Function Name: cm55_ml_deepcraft_task
********************************************************************************
* Summary:
*  Contains the main loop for the application. It sets up the UART for
*  logs and initialises the system (DEEPCRAFT pre-processor and PDM/PCM block
*  for audio input). It then invokes the PDM Data Processing function that
*  sends the data for pre-processing, inferencing, and prints in the results
*  when enough data is received.
*
* Parameters:
*  None
*
* Returns:
*  None
*
*******************************************************************************/
static void cm55_ml_deepcraft_task(void)
{
    cy_rslt_t result;

    /* Initialize retarget-io middleware */
    init_retarget_io();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    /* Initialize inference engine and sensors */
    result = system_init();

    /* Initialization failed */
    if(CY_RSLT_SUCCESS != result)
    {
        /* Failed to initialize properly */
        printf("System initialization fail\r\n");
        while(1);
    }

    for (;;)
    {
        /* Invoke the PDM Data Processing function that sends the data for
         * pre-processing, inferencing, and print the results when enough data
         * is received.
         */
        pdm_data_process();
    }
}
#endif /* ML_DEEPCRAFT_CM55 */

/* [] END OF FILE */
