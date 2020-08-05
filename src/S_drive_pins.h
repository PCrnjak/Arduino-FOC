#ifndef GLOBAL_VARIABLES
#define GLOBAL_VARIABLES

/*****************************************************************************
  Pin definitions
******************************************************************************/
#define LED PB1

#define TERMISTOR PA7

#define RS485_RE_DE PA8

#define EN1 PC13
#define EN2 PC14
#define EN3 PC15

#define PULS1 PB7
#define PULS2 PB8
#define PULS3 PB9

#define BEMF1 PA6
#define BEMF2 PA5
#define BEMF3 PA4

#define SUPPLY PA0

#define CURRENT_pin PB0


/*****************************************************************************/
//Encoder pins

#define CSn PA11 // ih HIGH state locks Index ,A,B to HIGH, going to LOW state releases them

#define CLK PA12

#define DO PB5

#define INDEX PB14

#define PROG PB15

#define EEPROM_SCL PB10

#define EEPROM_SDA PB11

#define EEPROM_WP PA1

#endif