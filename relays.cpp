#include <Wire.h>
#include <Arduino.h>

#include "relays.h"
#include "types.h"

uint8_t relayState = 0x00;

//Int
void setState(uint8_t idx, uint8_t state);

void sendRelay(void);

//Ext
void pumpControl(eDirection direction, uint8_t pumpIdx);

void purgeControl(uint8_t state);

void plugControl(uint8_t state);

void tvControl(uint8_t state);

void pumpControl(eDirection direction, uint8_t pumpIdx) {
    if (direction != off) {
        setState(0, direction);
        setState(1 + pumpIdx, 1);
    } else {
        setState(1 + pumpIdx, 0);
    }
    sendRelay();
}

void purgeControl(uint8_t state) {
    setState(7, state);
    sendRelay();
}

void plugControl(uint8_t stateV) {
    setState(R_MOT_DIR1, 1); //Off - Both Neg
    setState(R_MOT_DIR2, 0);
    delay(20);
    sendRelay();
    setState(2, 1);
    setState(0, stateV);
    setState(1, stateV);
    sendRelay();
}

void tvControl(uint8_t stateV) {
    setState(R_MOT_DIR1, 1); //Off - Both Neg
    setState(R_MOT_DIR2, 0);
    delay(20);
    sendRelay();
    setState(R_PLUGTVSEL, 0);
    setState(R_MOT_DIR1, stateV);
    setState(R_MOT_DIR2, stateV);
    sendRelay();
}

void setState(uint8_t idx, uint8_t stateV) {
    uint8_t tempval;
    uint8_t temp2;
//    Serial.println("-----");
//    Serial.println(idx);
//    Serial.println(stateV);
    tempval = 1 << idx;
    if (stateV) {
        relayState = relayState | tempval;
    } else {
        temp2 = 0xFF ^ tempval;
        relayState = relayState & temp2;
    }
}

void sendRelay(void) {
    Wire.beginTransmission(0x20);
    Wire.write(0xFF - relayState);
    Wire.endTransmission();
}