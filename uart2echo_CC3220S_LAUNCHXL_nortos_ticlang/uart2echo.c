/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uart2echo.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char input;
    const char echoPrompt[] = "Echoing characters:\r\n";
    //error statement to be called in case statements
    const char failPrompt[] = "Not a vaild entry start over\n";
    UART2_Handle uart;
    UART2_Params uartParams;
    size_t bytesRead;
    size_t bytesWritten = 0;
    uint32_t status     = UART2_STATUS_SUCCESS;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Create a UART where the default read and write mode is BLOCKING */
    UART2_Params_init(&uartParams);
    uartParams.baudRate = 115200;

    uart = UART2_open(CONFIG_UART2_0, &uartParams);

    if (uart == NULL)
    {
        /* UART2_open() failed */
        while (1) {}
    }

    /* Turn on user LED to indicate successful initialization */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    UART2_write(uart, echoPrompt, sizeof(echoPrompt), &bytesWritten);

    /* Loop forever echoing */
    while (1)
    {
        bytesRead = 0;
        while (bytesRead == 0)
        {
            status = UART2_read(uart, &input, 1, &bytesRead);

            if (status != UART2_STATUS_SUCCESS)
            {
                /* UART2_read() failed */
                while (1) {}
            }
        }
        //switches on the input read from the terminal
        switch(input){
            //goes into this switch case if the enter letter is O
            case'O':
                //Write the input back to the terminal
                status = UART2_write(uart, &input, 1, &bytesWritten);\
                //reads the next input as the input buffer only read on input at a time
                status = UART2_read(uart, &input, 1, &bytesRead);
                //switches on the new input
                switch(input){
                    //if the new input is N the ON was enterd and the light turns on and exits the switch cases
                    case'N':
                        //writes to the board to turn on the light
                        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                        //breaks out of the nested switch statement
                        break;
                    //if the new input is F then it checks for the next input to see if OFF was enter correctly
                    case'F':
                        //Write the input back to the terminal
                        status = UART2_write(uart, &input, 1, &bytesWritten);
                        //reads the next input as the input buffer only read on input at a time
                        status = UART2_read(uart, &input, 1, &bytesRead);
                        //switches on the new input
                        switch(input){
                            //If F was entered then OFF was entered correctly
                            case'F':
                                //Turns the boards light off and breaks out of the nested case stament
                                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                                break;
                            //If F wasnt enterd it brints a error message and breaks out of the nested case statement
                            default:
                                //Prints error staetement to terminal
                                UART2_write(uart, failPrompt, sizeof(failPrompt), &bytesWritten);
                                break;
                        }
                        //breaks the nested switch statement so that it doesnt print a inaccurate erroR statement
                        break;
                    default:
                        //Prints error staetement to terminal
                        UART2_write(uart, failPrompt, sizeof(failPrompt), &bytesWritten);
                        break;
                }
                //breaks the nested switch statement so that it doesnt print a inaccurate erroR statement
                break;
            //ON and OFF both start with O so if the fist input isnt a O the switch  case defaults to a error message
            default:
                //Prints error statement to terminal
                UART2_write(uart, failPrompt, sizeof(failPrompt), &bytesWritten);
                break;
        }

        bytesWritten = 0;
        while (bytesWritten == 0)
        {
            status = UART2_write(uart, &input, 1, &bytesWritten);

            if (status != UART2_STATUS_SUCCESS)
            {
                /* UART2_write() failed */
                while (1) {}
            }
        }
    }
}
