#include <Arduino.h>
#include <RF24.h>
#include "../include/const.h"
#include "../include/fbv.h"


#define BUTTON_PIN_A 2
#define BUTTON_PIN_B 3


RF24 _radio(7, 8);


void sendFbvMessage(const FBVMessage_t& msg);


/**
 * 
 */
void setup() {
    Serial.begin(31250);

    delay(200);

    _radio.begin();
    _radio.setPayloadSize(2);
    _radio.openReadingPipe(0, _radio_address);
    _radio.startListening();

    delay(500);

    sendFbvMessage(FBV_BOOT_MESSAGE);
    sendFbvMessage(FBV_BOOT_MESSAGE2);
    sendFbvMessage(FBV_SETTINGS_REQUEST);
}


/**
 * 
 */
FBVMessage_t initFbvButtonMessage(uint8_t button, uint8_t value) {
    FBVMessage_t res = FBV_BUTTON_MESSAGE;
    res.data[0] = button;
    res.data[1] = value % 2;
    return res;
}


/**
 * 
 */
void decodeCommand(uint16_t command) {
    uint8_t code = (uint8_t)(command >> 8);
    uint8_t value = (uint8_t)(command & 0x00FF);

    switch (code) {
        case BUTTON_A: {
            sendFbvMessage(initFbvButtonMessage(0x20, value));
            break;
        }
        case BUTTON_B: {
            sendFbvMessage(initFbvButtonMessage(0x30, value));
            break;
        }
        case BUTTON_C: {
            sendFbvMessage(initFbvButtonMessage(0x40, value));
            break;
        }
        case BUTTON_D: {
            sendFbvMessage(initFbvButtonMessage(0x50, value));
            break;
        }
    }
}



/**
 * 
 */
void handleRadio() {
    uint16_t command = 0;

    if (_radio.available()) {
        _radio.read(&command, 2);

        decodeCommand(command);
    }
}



/**
 * 
 */
void sendFbvMessage(const FBVMessage_t& msg) {
    Serial.write((uint8_t*)&msg, (size_t)(2 + msg.len));
}



/**
 * 
 */
void decodeFbvMessage(FBVMessage_t* msg) {

    switch (msg->command) {
        case 0x01: { // Heartbeat
            sendFbvMessage(FBV_HEART_BEAT_REPLY);
            break;
        } 
        case 0x04: { // LED status
            break;
        } 
        case 0x08: { // Small Display
            break;
        }
        case 0x10: { // Large Display
            break;
        }
    }
}



/**
 * 
 */
void handleFBV() {
    static uint8_t buff[257] = { 0, 0xFF, 0 };
    static int  len = 0;

    int avail = Serial.available();

    if (avail > 32) {
        avail = 32;
    }

    for (; avail > 0; avail--) {
        int b = Serial.read();

        if (len == 0 && b != 0xF0) {
            break;
        }

        buff[len++] = (uint8_t)Serial.read();
        int msgLen = buff[1];

        if ((msgLen + 2) <= len) {
            decodeFbvMessage((FBVMessage_t*)buff);

            len = 0;
            buff[1] = 0xFF;
        }
    }
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
void sendBtnEvent(int event, uint8_t btnCode) {

    if (event != BUTTON_NO_CHANGE) {
        sendFbvMessage(initFbvButtonMessage(btnCode, (event == BUTTON_PRESSED) ? 1 : 0));
    }
}



/**
 * 
 */
void handleButtons() {
    static uint8_t histA = 0;
    static uint8_t histB = 0;

    sendBtnEvent(pollButton(&histA, BUTTON_PIN_A), 0x30);
    sendBtnEvent(pollButton(&histB, BUTTON_PIN_B), 0x40);
}



/**
 * 
 */
void loop() {
    handleRadio();
    handleFBV();
    handleButtons();
}
