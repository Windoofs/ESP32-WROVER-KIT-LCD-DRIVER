/*! @file LCD.h

  @brief
  @author  Hendrik Schutter
  @version V1.0
  @date    03.11.2020
*/

#ifndef __LCD_H
#define __LCD_H

#include <stdio.h>
#include <stdlib.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <string.h>
#include "esp_system.h"

#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define COLOR_WHITE 0xFFFF
#define COLOR_BLACK 0x0000

esp_err_t iLCD_init(void);
esp_err_t iLCD_clearFramebuffer(uint16_t u16Color);
esp_err_t iLCD_writeString(uint16_t u16xPos, uint16_t u16yPos, char *pcText, uint16_t u16ColorFont, uint16_t u16ColorBackground);

#endif /* __LCD_H */
