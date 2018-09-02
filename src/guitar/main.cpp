#include <Arduino.h>
#include <RF24.h>
#include <LowPower.h>
#include "../include/const.h"


RF24 _radio(7, 8);


uint8_t histA = 0;
uint8_t histB = 0;
uint8_t histC = 0;
uint8_t histD = 0;


#define BUTTON_PIN_A 2 
#define BUTTON_PIN_B 3
#define BUTTON_PIN_C 4
#define BUTTON_PIN_D 5



/**
 * 
 */
void setup() { 
	_radio.begin();
    _radio.setPayloadSize(2);
    _radio.openWritingPipe(_radio_address);
    _radio.stopListening();
  
	pinMode(BUTTON_PIN_A, INPUT_PULLUP);
	pinMode(BUTTON_PIN_B, INPUT_PULLUP);
	pinMode(BUTTON_PIN_C, INPUT_PULLUP);
	pinMode(BUTTON_PIN_D, INPUT_PULLUP);
}



/**
 * 
 */
int pollButton(uint8_t* pHist, uint8_t pin)
{
	*pHist = (*pHist << 1) | !digitalRead(pin);

	if ((*pHist & 0b11000111) == 0b00000111)
	{	
		*pHist = 0xFF;
        return BUTTON_PRESSED;
	}
	
	if ((*pHist & 0b11000111) == 0b11000000)
	{
		*pHist = 0x00;
        return BUTTON_RELEASED;
	}

    return BUTTON_NO_CHANGE;
}



/**
 * 
 */
void sendCommand(uint16_t command) {
    _radio.powerUp();
    _radio.write(&command, sizeof(command));
}



/**
 * 
 */
void transmitEvent(int event, uint8_t btnCode) {

    if (event != BUTTON_NO_CHANGE) {
        uint16_t command = (btnCode << 8);

        if (event == BUTTON_PRESSED) {
            command |= 0x01;
        }

        sendCommand(command);
    }
}



/**
 * 
 */
void loop() {
    _radio.powerDown();
    LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);    

    transmitEvent(pollButton(&histA, BUTTON_PIN_A), BUTTON_A);
    transmitEvent(pollButton(&histB, BUTTON_PIN_B), BUTTON_B);
    transmitEvent(pollButton(&histC, BUTTON_PIN_C), BUTTON_C);
    transmitEvent(pollButton(&histD, BUTTON_PIN_D), BUTTON_D);
}