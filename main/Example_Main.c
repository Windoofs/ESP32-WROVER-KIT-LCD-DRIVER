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

void app_main(void)
{
    printf("Hello World!\n");

    vLCD_init();

    
    while(1){
        
    clear_framebuffer(COLOR_RED);    
    vTaskDelay(1000 / portTICK_RATE_MS);
    
    clear_framebuffer(COLOR_GREEN);    
    vTaskDelay(1000 / portTICK_RATE_MS);
    
  
    clear_framebuffer(COLOR_BLUE);    
    vTaskDelay(1000 / portTICK_RATE_MS);
    
    clear_framebuffer(COLOR_WHITE);    
    vTaskDelay(1000 / portTICK_RATE_MS);
    
    clear_framebuffer(COLOR_BLACK);    
    vTaskDelay(1000 / portTICK_RATE_MS);
        
    }
    
    printf("end\n");
}
