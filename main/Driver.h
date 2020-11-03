/*! @file Driver.h

  @brief
  @author  Hendrik Schutter
  @version V1.0
  @date    03.11.2020


*/

#ifndef __DRIVER_H
#define __DRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/spi_master.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#ifdef CONFIG_IDF_TARGET_ESP32
#define LCD_HOST    HSPI_HOST
#define DMA_CHAN    2

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5
#endif

/*
 The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

typedef enum {
    LCD_TYPE_ILI = 1,
    LCD_TYPE_ST,
    LCD_TYPE_MAX,
} type_lcd_t;


void vDriver_init(spi_device_handle_t spi);

void vDriver_spi_pre_transfer_callback(spi_transaction_t *t);
#endif /* __DRIVER_H */
