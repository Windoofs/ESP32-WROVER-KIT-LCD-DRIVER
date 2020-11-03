/*! @file LCD.c

  @brief   sets and writes framebuffer
  @author  Hendrik Schutter
  @version V1.0
  @date    03.11.2020

 This code displays graphics on the 320x240 LCD on an ESP-WROVER_KIT board.

 Some info about the ILI9341/ST7789V: It has an C/D line, which is connected to a GPIO here. It expects this
 line to be low for a command and high for data. We use a pre-transmit callback here to control that
 line: every transaction has as the user-definable argument the needed state of the D/C line and just
 before the transaction is sent, the callback will set this line to the correct state.

*/

#include "LCD.h"
#include "Driver.h"

esp_err_t iLCD_allocateFramebuffer(uint16_t ***pPixels);

uint16_t **pu16Framebuffer; //framebuffer

/**
 * @fn esp_err_t iLCD_init(void)
 * @brief init
 * @param void
 * @return int
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
esp_err_t iLCD_init(void) {
    esp_err_t ret;

    //Initialize the Driver
    vDriver_init();

    //Initialize the framebuffer
    ret= iLCD_allocateFramebuffer(&pu16Framebuffer);
    ESP_ERROR_CHECK(ret);

    return ret;
}

/**
 * @fn esp_err_t iLCD_allocateFramebuffer(uint16_t ***pPixels)
 * @brief allocate framebuffer
 * @param pointer of framebuffer two dimensional array
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
esp_err_t iLCD_allocateFramebuffer(uint16_t ***pPixels)
{
    *pPixels = NULL;
    esp_err_t ret = ESP_OK;
    //Alocate pixel memory. Each line is an array of IMAGE_W 16-bit pixels; the `*pixels` array itself contains pointers to these lines.
    *pPixels = calloc(240, sizeof(uint16_t *));
    if (*pPixels == NULL) {
        printf("Error allocating memory for lines");
        ret = ESP_ERR_NO_MEM;
    }
    for (int i = 0; i < 240; i++) {
        (*pPixels)[i] = malloc(320 * sizeof(uint16_t));
        if ((*pPixels)[i] == NULL) {
            printf("Error allocating memory for line %d", i);
            ret = ESP_ERR_NO_MEM;
        }
    }
    return ret;
}

/**
 * @fn esp_err_t iLCD_clearFramebuffer(uint16_t u16Color)
 * @brief clear complete framebuffer with a color
 * @param RGB586 color code
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
esp_err_t iLCD_clearFramebuffer(uint16_t u16Color) {
    // uint8_t *in = (uint8_t *)bitmap;
    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            //The LCD wants the 16-bit value in big-endian, so swap bytes
            u16Color = (u16Color >> 8) | (u16Color << 8);
            pu16Framebuffer[y][x] = u16Color;
        }
    }
    return iDriver_writeFramebuffer(&pu16Framebuffer);
}
