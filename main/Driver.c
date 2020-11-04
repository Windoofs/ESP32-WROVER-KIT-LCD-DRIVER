/*! @file Driver.c

  @brief write the Framebuffer
  @author  Hendrik Schutter
  @version V1.0
  @date    03.11.2020

 This code used SPI to write data to the LCD driver like ILI9341 or ST7789V.
*/

#include "Driver.h"

spi_device_handle_t spi; //SPI

uint16_t *lines[2];
int16_t sending_line=-1;
int16_t calc_line=0;

uint32_t vDriver_GetId(void);
static esp_err_t iDriver_sendLineFinish(void);
void vDriver_spiPreTransferCallback(spi_transaction_t *t);
esp_err_t iDriver_sendCmd(const uint8_t u8Cmd);
static esp_err_t iDriver_sendLines(uint16_t u16Ypos, uint16_t *pu16Linedata);
esp_err_t iDriver_SendData(const uint8_t *pu8Data, uint16_t u16Len);
void vDriver_getFramenufferPerLine(uint16_t *pu16Dest, uint16_t u16Line, uint16_t u16Linect, uint16_t ***pu16Framebuffer);

//Place data into DRAM. Constant data gets placed into DROM by default, which is not accessible by DMA.
DRAM_ATTR static const lcd_init_cmd_t st_init_cmds[]= {
    /* Memory Data Access Control, MX=MV=1, MY=ML=MH=0, RGB=0 */
    {0x36, {(1<<5)|(1<<6)}, 1},
    /* Interface Pixel Format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x55}, 1},
    /* Porch Setting */
    {0xB2, {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5},
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    {0xB7, {0x45}, 1},
    /* VCOM Setting, VCOM=1.175V */
    {0xBB, {0x2B}, 1},
    /* LCM Control, XOR: BGR, MX, MH */
    {0xC0, {0x2C}, 1},
    /* VDV and VRH Command Enable, enable=1 */
    {0xC2, {0x01, 0xff}, 2},
    /* VRH Set, Vap=4.4+... */
    {0xC3, {0x11}, 1},
    /* VDV Set, VDV=0 */
    {0xC4, {0x20}, 1},
    /* Frame Rate Control, 60Hz, inversion=0 */
    {0xC6, {0x0f}, 1},
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    {0xD0, {0xA4, 0xA1}, 1},
    /* Positive Voltage Gamma Control */
    {0xE0, {0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19}, 14},
    /* Negative Voltage Gamma Control */
    {0xE1, {0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19}, 14},
    /* Sleep Out */
    {0x11, {0}, 0x80},
    /* Display On */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff}
};

DRAM_ATTR static const lcd_init_cmd_t ili_init_cmds[]= {
    /* Power contorl B, power control = 0, DC_ENA = 1 */
    {0xCF, {0x00, 0x83, 0X30}, 3},
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
    /* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
    {0xE8, {0x85, 0x01, 0x79}, 3},
    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    /* Pump ratio control, DDVDH=2xVCl */
    {0xF7, {0x20}, 1},
    /* Driver timing control, all=0 unit */
    {0xEA, {0x00, 0x00}, 2},
    /* Power control 1, GVDD=4.75V */
    {0xC0, {0x26}, 1},
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    {0xC1, {0x11}, 1},
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    {0xC5, {0x35, 0x3E}, 2},
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    {0xC7, {0xBE}, 1},
    /* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
    {0x36, {0x28}, 1},
    /* Pixel format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x55}, 1},
    /* Frame rate control, f=fosc, 70Hz fps */
    {0xB1, {0x00, 0x1B}, 2},
    /* Enable 3G, disabled */
    {0xF2, {0x08}, 1},
    /* Gamma set, curve 1 */
    {0x26, {0x01}, 1},
    /* Positive gamma correction */
    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
    /* Negative gamma correction */
    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
    /* Column address set, SC=0, EC=0xEF */
    {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
    /* Page address set, SP=0, EP=0x013F */
    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
    /* Memory write */
    {0x2C, {0}, 0},
    /* Entry mode set, Low vol detect disabled, normal display */
    {0xB7, {0x07}, 1},
    /* Display function control */
    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
    /* Sleep out */
    {0x11, {0}, 0x80},
    /* Display on */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff},
};

/**
 * @fn esp_err_t vDriver_init(void)
 * @brief Initialize the driver
 * @param void
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
esp_err_t vDriver_init(void)
{
    esp_err_t ret; //store esp error code

    spi_bus_config_t buscfg= {
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=PARALLEL_LINES*320*2+8
    };

    spi_device_interface_config_t devcfg= {
#ifdef CONFIG_LCD_OVERCLOCK
        .clock_speed_hz=26*1000*1000,           //Clock out at 26 MHz
#else
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
#endif
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=vDriver_spiPreTransferCallback,  //Specify pre-transfer callback to handle D/C line
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(LCD_HOST, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(LCD_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    int cmd=0;
    const lcd_init_cmd_t* lcd_init_cmds;

    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    //detect LCD type
    uint32_t lcd_id = vDriver_GetId();
    int lcd_detected_type = 0;
    int lcd_type;

    printf("LCD ID: %08X\n", lcd_id);
    if ( lcd_id == 0 ) {
        //zero, ili
        lcd_detected_type = LCD_TYPE_ILI;
        printf("ILI9341 detected.\n");
    } else {
        // none-zero, ST
        lcd_detected_type = LCD_TYPE_ST;
        printf("ST7789V detected.\n");
    }

#ifdef CONFIG_LCD_TYPE_AUTO
    lcd_type = lcd_detected_type;
#elif defined( CONFIG_LCD_TYPE_ST7789V )
    printf("kconfig: force CONFIG_LCD_TYPE_ST7789V.\n");
    lcd_type = LCD_TYPE_ST;
#elif defined( CONFIG_LCD_TYPE_ILI9341 )
    printf("kconfig: force CONFIG_LCD_TYPE_ILI9341.\n");
    lcd_type = LCD_TYPE_ILI;
#endif
    if ( lcd_type == LCD_TYPE_ST ) {
        printf("LCD ST7789V initialization.\n");
        lcd_init_cmds = st_init_cmds;
    } else {
        printf("LCD ILI9341 initialization.\n");
        lcd_init_cmds = ili_init_cmds;
    }

    //Send all the commands
    while (lcd_init_cmds[cmd].databytes!=0xff) {
        ret = iDriver_sendCmd(lcd_init_cmds[cmd].cmd);
        assert(ret==ESP_OK);
        ret = iDriver_SendData(lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes&0x1F);
        if (lcd_init_cmds[cmd].databytes&0x80) {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        cmd++;
    }

    //Allocate memory for the line buffers
    for (int i=0; i<2; i++) {
        lines[i]=heap_caps_malloc(320*PARALLEL_LINES*sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(lines[i]!=NULL);
    }

    ///Enable backlight
    gpio_set_level(PIN_NUM_BCKL, 0);

    return ret;
}

/**
 * @fn void iDriver_sendCmd(const uint8_t u8Cmd)
 * @brief Send a command to the LCD.
 * @param const uint8_t u8Cmd
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 *
 * Send a command to the LCD. Uses spi_device_polling_transmit, which waits
 * until the transfer is complete.
 *
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
esp_err_t iDriver_sendCmd(const uint8_t u8Cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&u8Cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
    return ret;
}

/**
 * @fn esp_err_t iDriver_SendData(const uint8_t *pu8Data, uint16_t u16Len)
 * @brief Send data to the LCD.
 * @param const uint8_t *pu8Data
 * @param uint16_t u16Len
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 *
 * Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
 * transfer is complete.
 *
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
esp_err_t iDriver_SendData(const uint8_t *pu8Data, uint16_t u16Len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (u16Len==0) return ESP_FAIL;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=u16Len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=pu8Data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    return ret;
}

/**
 * @fn void vDriver_spiPreTransferCallback(spi_transaction_t *t)
 * @brief Pre Transfer Callback
 * @param spi_transaction_t *t
 * @return void
 * @author Hendrik Schutter
 * @date 3.11.2020
 *
 * This function is called (in irq context!) just before a transmission starts. It will
 * set the D/C line to the value indicated in the user field.
 */
void vDriver_spiPreTransferCallback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

/**
 * @fn uint32_t vDriver_GetId(void)
 * @brief get LCD driver chip id
 * @param void
 * @return uint32_t
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
uint32_t vDriver_GetId(void)
{
    esp_err_t ret;
    //get_id cmd
    ret = iDriver_sendCmd(0x04);
    assert(ret==ESP_OK);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=8*3;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    ret = spi_device_polling_transmit(spi, &t);
    assert( ret == ESP_OK );

    return *(uint32_t*)t.rx_data;
}

/**
 * @fn esp_err_t iDriver_writeFramebuffer(uint16_t ***pu16Framebuffer)
 * @brief write framebuffer to LCD via SPI
 * @param uint16_t ***pu16Framebuffer
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
esp_err_t iDriver_writeFramebuffer(uint16_t ***pu16Framebuffer)
{
    esp_err_t ret;
    for (int y=0; y<240; y+=PARALLEL_LINES) {
        vDriver_getFramenufferPerLine(lines[calc_line], y, PARALLEL_LINES, pu16Framebuffer);
        //Finish up the sending process of the previous line, if any
        if (sending_line!=-1)
        {
            ret = iDriver_sendLineFinish();
        }
        //Swap sending_line and calc_line
        sending_line=calc_line;
        calc_line=(calc_line==1)?0:1;
        //Send the line we currently calculated.
        iDriver_sendLines(y, lines[sending_line]);
        //The line set is queued up for sending now; the actual sending happens in the
        //background. We can go on to calculate the next line set as long as we do not
        //touch line[sending_line]; the SPI sending process is still reading from that.
    }
    return ret;
}

/**
 * @fn void vDriver_getFramenufferPerLine(uint16_t *pu16Dest, uint16_t u16Line, uint16_t u16Linect, uint16_t ***pu16Framebuffer)
 * @brief Get the pixel data for a set of lines (with implied line size of 320)
 * @param uint16_t *pu16Dest
 * @param uint16_t u16Line
 * @param uint16_t u16Linect
 * @param uint16_t ***pu16Framebuffer
 * @return void
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
void vDriver_getFramenufferPerLine(uint16_t *pu16Dest, uint16_t u16Line, uint16_t u16Linect, uint16_t ***pu16Framebuffer)
{
    for (uint16_t y=u16Line; y<u16Line+u16Linect; y++) {
        for (uint16_t x=0; x<320; x++) {
            *pu16Dest++=(*pu16Framebuffer)[y][x];
        }
    }
}

/**
 * @fn static esp_err_t iDriver_sendLines(uint16_t u16Ypos, uint16_t *pu16Linedata)
 * @brief send line to lcd chip
 * @param uint16_t u16Ypos
 * @param uint16_t *pu16Linedata
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 *
 * To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
 * because the D/C line needs to be toggled in the middle.)
 * This routine queues these commands up as interrupt transactions so they get
 * sent faster (compared to calling spi_device_transmit several times), and at
 * the mean while the lines for next transactions can get calculated.
 */
static esp_err_t iDriver_sendLines(uint16_t u16Ypos, uint16_t *pu16Linedata)
{
    esp_err_t ret;
    //Transaction descriptors. Declared static so they're not allocated on the stack; we need this memory even when this
    //function is finished because the SPI driver needs access to it even while we're already calculating the next line.
    static spi_transaction_t trans[6];

    //In theory, it's better to initialize trans and data only once and hang on to the initialized
    //variables. We allocate them on the stack, so we need to re-init them each call.
    for (uint8_t x=0; x<6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x&1)==0) {
            //Even transfers are commands
            trans[x].length=8;
            trans[x].user=(void*)0;
        } else {
            //Odd transfers are data
            trans[x].length=8*4;
            trans[x].user=(void*)1;
        }
        trans[x].flags=SPI_TRANS_USE_TXDATA;
    }
    trans[0].tx_data[0]=0x2A;           //Column Address Set
    trans[1].tx_data[0]=0;              //Start Col High
    trans[1].tx_data[1]=0;              //Start Col Low
    trans[1].tx_data[2]=(320)>>8;       //End Col High
    trans[1].tx_data[3]=(320)&0xff;     //End Col Low
    trans[2].tx_data[0]=0x2B;           //Page address set
    trans[3].tx_data[0]=u16Ypos>>8;        //Start page high
    trans[3].tx_data[1]=u16Ypos&0xff;      //start page low
    trans[3].tx_data[2]=(u16Ypos+PARALLEL_LINES)>>8;    //end page high
    trans[3].tx_data[3]=(u16Ypos+PARALLEL_LINES)&0xff;  //end page low
    trans[4].tx_data[0]=0x2C;           //memory write
    trans[5].tx_buffer=pu16Linedata;        //finally send the line data
    trans[5].length=320*2*8*PARALLEL_LINES;          //Data length, in bits
    trans[5].flags=0; //undo SPI_TRANS_USE_TXDATA flag

    //Queue all transactions.
    for (uint8_t x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi, &trans[x], portMAX_DELAY);
        assert(ret==ESP_OK);
    }

    //When we are here, the SPI driver is busy (in the background) getting the transactions sent. That happens
    //mostly using DMA, so the CPU doesn't have much to do here. We're not going to wait for the transaction to
    //finish because we may as well spend the time calculating the next line. When that is done, we can call
    //send_line_finish, which will wait for the transfers to be done and check their status.
    return ret;
}

/**
 * @fn static esp_err_t iDriver_sendLineFinish(void)
 * @brief send finish after line
 * @param void
 * @return esp error code
 * @author Hendrik Schutter
 * @date 3.11.2020
 */
static esp_err_t iDriver_sendLineFinish(void)
{
    spi_transaction_t *rtrans;
    esp_err_t ret;
    //Wait for all 6 transactions to be done and get back the results.
    for (uint8_t x=0; x<6; x++) {
        ret=spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
        assert(ret==ESP_OK);
        //We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
    }
    return ret;
}