/******************************************************************************
* File Name:   audio.c
*
* Description: This file implements the interface with the PDM, as
*              well as the PDM ISR to feed the audio processing block.
*
* Related Document: See README.md
*
*
********************************************************************************
* (c) 2024-2025, Infineon Technologies AG, or an affiliate of Infineon
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
*******************************************************************************/
#include "cybsp.h"
#include "cy_pdl.h"
#include "audio.h"
#include <math.h>

/******************************************************************************
 * Macros
 *****************************************************************************/
/* PDM PCM interrupt priority */
#define PDM_PCM_ISR_PRIORITY                    (2u)

/* Channel Index */
#define PDM_CHANNEL                             (3u)

/* PDM PCM hardware FIFO size */
#define HW_FIFO_SIZE                            (64u)

/* Rx FIFO trigger level/threshold configured by user */
#define RX_FIFO_TRIG_LEVEL                      (HW_FIFO_SIZE/2)

/* Total number of interrupts to get the FRAME_SIZE number of samples*/
#define NUMBER_INTERRUPTS_FOR_FRAME             (FRAME_SIZE/RX_FIFO_TRIG_LEVEL)

/* Multiplication factor of the input signal.
 * This should ideally be 1. Higher values will have a negative impact on
 * the sampling dynamic range. However, it can be used as a last resort 
 * when MICROPHONE_GAIN is already at maximum and the ML model was trained
 * with data at a higher amplitude than the microphone captures.
 * Note: If you use the same board for recording training data and 
 * deployment of your own ML model set this to 1.0. */
#define DIGITAL_BOOST_FACTOR                    1.0f

/* Specifies the dynamic range in bits.
 * PCM word length, see the A/D specific documentation for valid ranges. */
#define AUIDO_BITS_PER_SAMPLE                  16

/* Converts given audio sample into range [-1,1] */
#define SAMPLE_NORMALIZE(sample)                (((float) (sample)) / (float) (1 << (AUIDO_BITS_PER_SAMPLE - 1)))

/* Threshold for the output score to be considered a valid detection.
*  The threshold can be adjusted to increase or decrease the sensitivity of the
 * detection. A lower value will result in more false positives, while a higher
 * value will result in more false negatives. */
#define OUTPUT_THRESHOLD_SCORE                  (0.6f)

#define PDM_PCM_GAIN                            (CY_PDM_PCM_SEL_GAIN_5DB)

/******************************************************************************
 * Global Variables
 *****************************************************************************/
/* Set up one buffer for data collection and one for processing */
static int16_t audio_buffer0[FRAME_SIZE] = {0};
static int16_t audio_buffer1[FRAME_SIZE] = {0};
static int16_t* active_rx_buffer;
static int16_t* full_rx_buffer;

/* PDM PCM interrupt configuration parameters */
static const cy_stc_sysint_t PDM_IRQ_cfg =
{
    .intrSrc = (IRQn_Type)CYBSP_PDM_CHANNEL_3_IRQ,
    .intrPriority = PDM_PCM_ISR_PRIORITY
};

/* Flag to check if the data from PDM/PCM block is ready for processing. */
static volatile bool pdm_pcm_flag;

/*******************************************************************************
* Local Function Prototypes
*******************************************************************************/
static void pdm_pcm_event_handler(void);

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: pdm_init
********************************************************************************
* Summary:
*  A function used to initialize and configure the PDM. Sets up an interrupt
*  to trigger when the PDM FIFO level passes the trigger level.
*
* Parameters:
*  None
*
* Return:
*  The status of the initialization.
*
*******************************************************************************/
cy_rslt_t pdm_init(void)
{
    cy_rslt_t result;

    /* Set up pointers to two buffers to implement a ping-pong buffer system.
     * One gets filled by the PDM while the other can be processed. */
    memset(audio_buffer0, 0, FRAME_SIZE*sizeof(int16_t));
    memset(audio_buffer1, 0, FRAME_SIZE*sizeof(int16_t));
    active_rx_buffer = audio_buffer0;
    full_rx_buffer = audio_buffer1;

    /* Initialize PDM PCM block */
    result = Cy_PDM_PCM_Init(CYBSP_PDM_HW, &CYBSP_PDM_config);
    if(CY_PDM_PCM_SUCCESS != result)
    {
        return result;
    }

    Cy_PDM_PCM_Channel_Enable(CYBSP_PDM_HW, PDM_CHANNEL);
    /* Initialize and enable PDM PCM channel 3 -Right */
    Cy_PDM_PCM_Channel_Init(CYBSP_PDM_HW, &channel_3_config, (uint8_t)PDM_CHANNEL);
    
    /* Set the PDM_PCM_GAIN as per the model. */
    Cy_PDM_PCM_SetGain(CYBSP_PDM_HW, PDM_CHANNEL, PDM_PCM_GAIN);

    /* An interrupt is registered for right channel, clear and set masks for it. */
    Cy_PDM_PCM_Channel_ClearInterrupt(CYBSP_PDM_HW, PDM_CHANNEL, CY_PDM_PCM_INTR_MASK);
    Cy_PDM_PCM_Channel_SetInterruptMask(CYBSP_PDM_HW, PDM_CHANNEL, CY_PDM_PCM_INTR_MASK);

    /* Register the IRQ handler */
    result = Cy_SysInt_Init(&PDM_IRQ_cfg, &pdm_pcm_event_handler);
    if(CY_SYSINT_SUCCESS != result)
    {
        return result;
    }
    NVIC_ClearPendingIRQ(PDM_IRQ_cfg.intrSrc);
    NVIC_EnableIRQ(PDM_IRQ_cfg.intrSrc);

    /* Global variable used to determine if PDM data is available */
    pdm_pcm_flag = false;

    /* Set up pointers to two buffers to implement a ping-pong buffer system.
     * One gets filled by the PDM while the other can be processed. */
    active_rx_buffer = audio_buffer0;
    full_rx_buffer = audio_buffer1;

    Cy_PDM_PCM_Activate_Channel(CYBSP_PDM_HW, PDM_CHANNEL);

    return CY_RSLT_SUCCESS;
}

/*******************************************************************************
* Function Name: pdm_pcm_event_handler
********************************************************************************
* Summary:
*  PDM/PCM ISR handler. Check the interrupt status and clears it.
*  Fills a buffer and then swaps that buffer with an empty one.
*  Once a buffer is full, a flag is set which is used in main.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void pdm_pcm_event_handler(void)
{
    /* Used to track how full the buffer is */
    static uint16_t frame_counter = 0;

    /* Check the interrupt status */
    uint32_t intr_status = Cy_PDM_PCM_Channel_GetInterruptStatusMasked(CYBSP_PDM_HW, PDM_CHANNEL);
    if(CY_PDM_PCM_INTR_RX_TRIGGER & intr_status)
    {
        /* Move data from the PDM fifo and place it in a buffer */
        for(uint32_t index=0; index < RX_FIFO_TRIG_LEVEL; index++)
        {
            int32_t data = (int32_t)Cy_PDM_PCM_Channel_ReadFifo(CYBSP_PDM_HW, PDM_CHANNEL);
            active_rx_buffer[frame_counter * RX_FIFO_TRIG_LEVEL + index] = (int16_t)(data);
        }
        Cy_PDM_PCM_Channel_ClearInterrupt(CYBSP_PDM_HW, PDM_CHANNEL, CY_PDM_PCM_INTR_RX_TRIGGER);
        frame_counter++;
    }
    /* Check if the buffer is full */
    if((NUMBER_INTERRUPTS_FOR_FRAME) <= frame_counter)
    {
        /* Flip the active and the next rx buffers */
        int16_t* temp = active_rx_buffer;
        active_rx_buffer = full_rx_buffer;
        full_rx_buffer = temp;

        /* Set the PDM_PCM flag as true, signaling there is data ready for use */
        pdm_pcm_flag = true;
        frame_counter = 0;
    }

    if((CY_PDM_PCM_INTR_RX_FIR_OVERFLOW | CY_PDM_PCM_INTR_RX_OVERFLOW|
            CY_PDM_PCM_INTR_RX_IF_OVERFLOW | CY_PDM_PCM_INTR_RX_UNDERFLOW) & intr_status)
    {
        Cy_PDM_PCM_Channel_ClearInterrupt(CYBSP_PDM_HW, PDM_CHANNEL, CY_PDM_PCM_INTR_MASK);
    }
}

/*******************************************************************************
* Function Name: pdm_data_process
********************************************************************************
* Summary:
*  This function feeds the data to the DEEPCRAFT pre-processor and returns the
*  processed results.
*
* Parameters:
*  None
*
* Return:
*  CY_RSLT_SUCCESS if successful, otherwise an error code indicating failure.
*
*******************************************************************************/
cy_rslt_t pdm_data_process(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    int16_t best_label = 0;
    float sample = 0.0f;
    float max_score = 0.0f;
    float label_scores[IMAI_DATA_OUT_COUNT];
    char *label_text[] = IMAI_DATA_OUT_SYMBOLS;

    /* Check if PDM PCM Data is ready to be processed */
    if (!pdm_pcm_flag)
    {
        result = PDM_PCM_DATA_NOT_READY;
        return result;
    }

    /* Reset the flag to false, indicating that the data is being processed */
    pdm_pcm_flag = false;

    /* Move cursor home */
    printf("\033[H\n");

#ifdef COMPONENT_CM33
    printf("DEEPCRAFT Studio Deploy Audio Example - CM33\r\n\n");
#else
    printf("DEEPCRAFT Studio Deploy Audio Example - CM55\r\n\n");
#endif /* COMPONENT_CM33 */

    for (uint32_t index = 0; index < FRAME_SIZE ; index++)
    {
        int16_t val_temp = full_rx_buffer[index];
        sample = SAMPLE_NORMALIZE(val_temp) * DIGITAL_BOOST_FACTOR;
        if (sample > 1.0)
        {
            sample = 1.0;
        }
        else if (sample < -1.0)
        {
            sample = -1.0;
        }

        /* Pass the audio sample for enqueue */
        result = IMAI_enqueue(&sample);
        CY_ASSERT(IMAI_RET_SUCCESS == result);

        best_label = 0;
        max_score = -1000.0f;

        /* Check if there is any model output to process */
        switch(IMAI_dequeue(label_scores))
        {
            case IMAI_RET_SUCCESS:      /* We have data, display it */
            {
                for(int i = 0; i < IMAI_DATA_OUT_COUNT; i++)
                {
                    printf("label: %-11s: score: %.4f\r\n", label_text[i], label_scores[i]);
                    if (label_scores[i] > max_score)
                    {
                        max_score = label_scores[i];
                        best_label = i;
                    }
                }

                if(max_score >= OUTPUT_THRESHOLD_SCORE)
                {
                    printf("\n\nOutput: %-10s\r\n", label_text[best_label]);
                }
                else
                {
                    printf("\n\nOutput: %-10s\r\n", "");
                }

                break;
            }

            case IMAI_RET_NODATA:   /* No new output, continue with sampling */
            {
                break;
            }
            case IMAI_RET_ERROR:    /* Abort on error */
            {
                CY_ASSERT(0);
                break;
            }
        }
    }

    return result;
}

/* [] END OF FILE */
