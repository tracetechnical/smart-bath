#include <OneWire.h>
#include <DallasTemperature.h>
#include "Arduino.h"
#include "types.h"
#include "servos.h"

#define ONE_WIRE_BUS 4
#define NEAR_THRESH 5
#define UNDERSHOOT_THRESH 3
#define OVERSHOOT_THRESH 2

#define MIN_TEMP 25
#define MIN_TEMP_POS 200
#define MAX_TEMP 50
#define MAX_TEMP_POS 0
#define DELTA_TEMP (MAX_TEMP - MIN_TEMP)
#define DELTA_POS  (MAX_TEMP_POS - MIN_TEMP_POS)

typedef enum {
    T_START_ACQ,
    T_WAIT,
    T_CONVERT
} eTempStates;

void setTemperatureSP(uint8_t temp);

uint8_t isSourceNearTemperature(void);

uint8_t isSourceAtTemperature(void);

uint8_t isReturnNearTemperature(void);

uint8_t isReturnAtTemperature(void);

int8_t getSourceError(void);

int8_t getReturnError(void);

eTempStates tempState;
float sourceTemp;
float returnTemp;
float setpointTemp;
unsigned long start;
unsigned long stop;
unsigned long stop2;
uint16_t trimPostscaler;
uint16_t measurePostscaler;
uint8_t lastMotorPosition;

DeviceAddress tempDeviceAddress;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void initialiseTemperature(void) {
    sensors.begin();
    sensors.getAddress(tempDeviceAddress, 0);
    sensors.setResolution(tempDeviceAddress, 12);
}

void setTemperatureSP(uint8_t temp) {
    setpointTemp = temp;
}

uint8_t isSourceNearTemperature(void) {
    return sourceTemp >= (setpointTemp - NEAR_THRESH)
           && sourceTemp <= (setpointTemp + NEAR_THRESH);
}

uint8_t isSourceAtTemperature(void) {
    return sourceTemp >= (setpointTemp - UNDERSHOOT_THRESH)
           && sourceTemp <= (setpointTemp + OVERSHOOT_THRESH);
}

uint8_t isReturnNearTemperature(void) {
    return returnTemp >= (setpointTemp - NEAR_THRESH)
           && returnTemp <= (setpointTemp + NEAR_THRESH);
}

uint8_t isReturnAtTemperature(void) {
    return returnTemp >= (setpointTemp - UNDERSHOOT_THRESH)
           && returnTemp <= (setpointTemp + OVERSHOOT_THRESH);
}

int8_t getSourceError(void) {
    return sourceTemp - setpointTemp;
}

int8_t getReturnError(void) {
    return returnTemp - setpointTemp;
}

uint8_t getSourceTemp(void) {
    return sourceTemp;
}

void runTemperatureControl(void) {
    if(trimPostscaler++ > 200) {
        int8_t trim = getSourceError() * 6;
        float tempRatio = (setpointTemp - MIN_TEMP);
        tempRatio /= (float)DELTA_TEMP;
        float motorPosn = tempRatio * (float)DELTA_POS;
        uint8_t motorPosition = MIN_TEMP_POS + motorPosn;
        int16_t overflow = (int16_t)motorPosition + (int16_t)trim;
        Serial.println("----------");
        Serial.print(overflow);
        Serial.print("/");
        Serial.print(motorPosition);
        Serial.print("/");
        Serial.print(trim);

        if(overflow > 254) {
            motorPosition = 255;
        }
        if(overflow < 0) {
            motorPosition = 0;
        }
        if(motorPosition != 255 && motorPosition != 0) {
            motorPosition += trim;
        }
        if(motorPosition != lastMotorPosition) {
            setTemperaturePosition(motorPosition);
            lastMotorPosition = motorPosition;
        }
    }
    if(measurePostscaler++ > 200) {
        switch (tempState) {
            case T_START_ACQ:
                start = millis();
                sensors.setWaitForConversion(false);
                sensors.requestTemperatures();
                tempState = T_WAIT;
                break;

            case T_WAIT:
                stop = millis();
                if ((stop - start) > 1000) {
                    sensors.setWaitForConversion(true);
                    stop2 = millis();
                    tempState = T_CONVERT;
                }
                break;

            case T_CONVERT:
                sourceTemp = sensors.getTempCByIndex(0);
                returnTemp = sensors.getTempCByIndex(1);
                Serial.println("----------");
                Serial.println(sourceTemp);
                Serial.println(returnTemp);
                tempState = T_START_ACQ;
                break;
        }
    }
}
