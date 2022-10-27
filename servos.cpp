#include <Adafruit_PWMServoDriver.h>
#include "types.h"

#define P_MIN_OUTLET_1    500
#define P_MAX_OUTLET_1    2500
#define P_MIN_OUTLET_2    500
#define P_MAX_OUTLET_2    2500
#define P_MIN_GATE        400
#define P_MAX_GATE        1600
#define P_MIN_TEMPERATURE 1000
#define P_MAX_TEMPERATURE 2500

#define P_DIFF_OUTLET_1    P_MAX_OUTLET_1 - P_MIN_OUTLET_1
#define P_DIFF_OUTLET_2    P_MAX_OUTLET_2 - P_MIN_OUTLET_2
#define P_DIFF_GATE        P_MAX_GATE - P_MIN_GATE
#define P_DIFF_TEMPERATURE P_MAX_TEMPERATURE - P_MIN_TEMPERATURE

void initialiseServos(void);

void setOutlet(eOutlet outlet);

void setGate(uint8_t state);

void setTemperaturePosition(uint8_t position);

void setPosition(eServo motor, uint8_t position);

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

void initialiseServos(void) {
    pca.begin();
    pca.setPWMFreq(60);  // Analog servos run at ~60 Hz update

    setGate(0);
    setOutlet(O_BATH);
    setTemperaturePosition(250);

}

void setOutlet(eOutlet outlet) {
    Serial.println("---");
    Serial.println(outlet);
    uint8_t motor1 = (outlet & 0b01);
    uint8_t motor2 = (outlet & 0b10);
    Serial.println(motor1);
    Serial.println(motor2);
    setPosition(S_OUTLET_1, motor1 ? 255 : 0);
    setPosition(S_OUTLET_2, motor2 ? 255 : 0);
}

void setGate(uint8_t state) {
    setPosition(S_GATE, state ? 0 : 255);
}

void setTemperaturePosition(uint8_t position) {
    setPosition(S_TEMPERATURE, position);
}

void setPosition(eServo motor, uint8_t position) {
    float temp = 0;
    uint16_t pos = 0;
    uint16_t diff = 0;
    uint16_t mini = 0;

    switch (motor) {
        case S_GATE:
            diff = P_DIFF_GATE;
            mini = P_MIN_GATE;
            break;
        case S_OUTLET_1:
            diff = P_DIFF_OUTLET_1;
            mini = P_MIN_OUTLET_1;
            break;
        case S_OUTLET_2:
            diff = P_DIFF_OUTLET_2;
            mini = P_MIN_OUTLET_2;
            break;
        case S_TEMPERATURE:
            diff = P_DIFF_TEMPERATURE;
            mini = P_MIN_TEMPERATURE;
            break;
    }

    temp = ((float)position / 255) * diff;
    pos = mini + (uint16_t) temp;
//    Serial.println("--------------");
//    Serial.print("S:");
//    Serial.println(position);
//    Serial.print("T:");
//    Serial.println(temp);
//    Serial.print("M:");
//    Serial.println(mini);
//    Serial.print("D:");
//    Serial.println(diff);
//    Serial.print("P:");
//    Serial.println(pos);
//    Serial.print("G:");
//    Serial.println(motor);
    pca.writeMicroseconds(motor, pos);
}
