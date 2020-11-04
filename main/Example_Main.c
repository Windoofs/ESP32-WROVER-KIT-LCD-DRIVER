/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LCD.h"

/**
 * @fn void app_main(void)
 * @brief main task
 * @param void
 * @return void
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
void app_main(void)
{
    printf("Hello World!\n");

    iLCD_init(); //init lcd screen

    iLCD_clearFramebuffer(COLOR_BLACK); //set complete screen to desired color

    vTaskDelay(1000 / portTICK_RATE_MS); //wait one sec

    iLCD_writeString(42,42,"Hello World!",COLOR_WHITE,COLOR_BLACK); //Write text to screen

    iLCD_writeString(42,50,"ESP32",COLOR_WHITE,COLOR_GREEN); //Write text to screen

    while(1) {
        //run more code here
    }

}
