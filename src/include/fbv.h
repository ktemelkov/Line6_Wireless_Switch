#ifndef __COMMON_FBV_H
#define __COMMON_FBV_H


/**
 * 
 */
typedef struct tagFBVMessage {
    uint8_t start;
    uint8_t len;
    uint8_t command;
    uint8_t data[255];
} __attribute__ ((packed)) FBVMessage_t;


static const FBVMessage_t FBV_HEART_BEAT_REPLY PROGMEM = { 0xF0, 0x07, 0x80, { 0x00, 0x02, 0x00, 0x01, 0x01, 0x00 } };
static const FBVMessage_t FBV_BOOT_MESSAGE PROGMEM = { 0xF0, 0x02, 0x90, { 0x00 } };
static const FBVMessage_t FBV_BOOT_MESSAGE2 PROGMEM = { 0xF0, 0x02, 0x30, { 0x08 } };
static const FBVMessage_t FBV_SETTINGS_REQUEST PROGMEM = { 0xF0, 0x03, 0x80, { 0x00, 01 } };
static const FBVMessage_t FBV_BUTTON_MESSAGE PROGMEM = { 0xF0, 0x03, 0x81, { 0x00 } };


#endif // __COMMON_FBV_H