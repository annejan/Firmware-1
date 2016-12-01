#include "gde.h"
#include "driver/gpio.h"
#include "pins.h"
#include "rom/ets_sys.h"
#include "rom/gpio.h"
#include "sdkconfig.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include <stdio.h>
#include <string.h>

#ifndef LOW
#define LOW 0
#endif
#ifndef HIGH
#define HIGH 1
#endif

void resetDisplay() {
  gpio_set_level(PIN_NUM_RESET, LOW);
  ets_delay_us(100000);
  gpio_set_level(PIN_NUM_RESET, HIGH);
  ets_delay_us(100000);
}

void writeCommand(unsigned char command) {
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  spiWriteByte(spi, command);
  gpio_set_level(PIN_NUM_CS, HIGH);
}

void writeData(unsigned char data) {
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  gpio_set_level(PIN_NUM_DATA, HIGH);
  spiWriteByte(spi, data);
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_DATA, LOW);
}

void writeCMD_p1(unsigned char command, unsigned char para) {
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  spiWriteByte(spi, command);
  gpio_set_level(PIN_NUM_DATA, HIGH);
  spiWriteByte(spi, para);
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_DATA, LOW);
}

void writeCMD_p2(unsigned char command, unsigned char para1,
                 unsigned char para2) {
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  spiWriteByte(spi, command);
  gpio_set_level(PIN_NUM_DATA, HIGH);
  spiWriteByte(spi, para1);
  spiWriteByte(spi, para2);
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_DATA, LOW);
}

void writeStream(unsigned char *value, unsigned char datalen) {
  unsigned char i = 0;
  unsigned char *ptemp;

  ptemp = value;
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  spiWriteByte(spi, *ptemp);
  ptemp++;
  gpio_set_level(PIN_NUM_DATA, HIGH);
  for (i = 0; i < datalen - 1; i++) // sub the command
  {
    spiWriteByte(spi, *ptemp);
    ptemp++;
  }
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_DATA, LOW);
}

void writeDispRam(unsigned char xSize, unsigned int ySize,
                  const unsigned char *dispdata) {
  unsigned int i = 0, j = 0, c = 0;
  char data;

  if (xSize % 8 != 0) {
    xSize = xSize + (8 - xSize % 8);
  }
  xSize = xSize / 8;

  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  spiWriteByte(spi, 0x24);

  gpio_set_level(PIN_NUM_DATA, HIGH);
  for (i = 0; i < ySize; i++) {
    for (j = 0; j < xSize; j++) {
      data = dispdata[c];
      spiWriteByte(spi, ~data);
      c++;
    }
  }
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_DATA, LOW);
}

void writeDispRamMono(unsigned char xSize, unsigned int ySize,
                      unsigned char dispdata) {
  unsigned int i = 0, j = 0;
  if (xSize % 8 != 0) {
    xSize = xSize + (8 - xSize % 8);
  }
  xSize = xSize / 8;

  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_CS, LOW);
  spiWriteByte(spi, 0x24);

  gpio_set_level(PIN_NUM_DATA, HIGH);
  for (i = 0; i < ySize; i++) {
    for (j = 0; j < xSize; j++) {
      spiWriteByte(spi, dispdata);
    }
  }
  gpio_set_level(PIN_NUM_CS, HIGH);
  gpio_set_level(PIN_NUM_DATA, LOW);
}

void initSPI() {
  gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_DATA, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_RESET, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_BUSY, GPIO_MODE_INPUT);

  spi = spiStartBus(VSPI, 1000000, SPI_MODE0, SPI_MSBFIRST);
  spiAttachSCK(spi, PIN_NUM_CLK);
  spiAttachMISO(spi, 34); // just cause . .
  spiAttachMOSI(spi, PIN_NUM_MOSI);
  spiAttachSS(spi, 0, PIN_NUM_CS); // if you want hardware SS
  spiEnableSSPins(spi, 1 << 0);    // activate SS for CS0
  spiSSEnable(spi);
}