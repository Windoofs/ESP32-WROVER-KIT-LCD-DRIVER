# ESP32-WROVER-KIT-LCD-DRIVER

LCD Driver for ST7789V/ILI9341 used ESP32-WROVER-KIT

## Overview

In the ./main directory you can find following files:

_Example_Main.c_  Example for this project

_LCD.c_           LCD Framebuffer Module  
_LCD.h_           Header for importing in user application  

_Driver.c_        SPI LCD Driver  
_Driver.h_        Header SPI LCD Driver  

## Code example

``` c
iLCD_init(); //init lcd screen

iLCD_clearFramebuffer(COLOR_BLACK); //set complete screen to desired color

iLCD_writeString(42,42,"Hello World!",COLOR_WHITE,COLOR_BLACK); //Write text to screen

iLCD_writeString(42,50,"ESP32",COLOR_WHITE,COLOR_GREEN); //Write text to screen
```
## Documentation

You can find the Doxygen HTML in ./Doxygen

## Existing code used
https://github.com/Everlers/demo_esp_lcd_st7789v  
https://github.com/espressif/esp-idf/blob/master/examples/peripherals/spi_master/lcd
