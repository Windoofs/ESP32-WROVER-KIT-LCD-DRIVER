# moved to https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER

# ESP32-WROVER-KIT-LCD-DRIVER

LCD Driver for ST7789V/ILI9341 used ESP32-WROVER-KIT

## Overview

In the  [./main](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/main) directory you can find following files:

[_Example_Main.c_](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/main/Example_Main.c)  Example for this project

[_LCD.c_](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/main/LCD.c)           LCD Framebuffer Module  
[_LCD.h_](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/main/LCD.h)           Header for importing in user application  

[_Driver.c_](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/main/Driver.c)        SPI LCD Driver  
[_Driver.h_](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/main/Driver.h)        Header SPI LCD Driver  

## Code example

``` c
iLCD_init(); //init lcd screen

iLCD_clearFramebuffer(COLOR_BLACK); //set complete screen to desired color

iLCD_writeString(42,42,"Hello World!",COLOR_WHITE,COLOR_BLACK); //Write text to screen

iLCD_writeString(42,50,"ESP32",COLOR_WHITE,COLOR_GREEN); //Write text to screen
```

<img src="https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/raw/branch/master/code_example_output.png" alt="code_example_output" width="500"/>

## Documentation

You can find the Doxygen HTML in [./Doxygen](https://git.mosad.xyz/localhorst/ESP32-WROVER-KIT-LCD-DRIVER/src/branch/master/Doxygen/html)

## Existing code used
https://github.com/Everlers/demo_esp_lcd_st7789v  
https://github.com/espressif/esp-idf/blob/master/examples/peripherals/spi_master/lcd
