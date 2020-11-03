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


//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define PARALLEL_LINES 16


void vLCD_init(void);
void clear_framebuffer(uint16_t u16Color);




#endif /* __LCD_H */
