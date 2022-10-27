//
// Created by Admin on 26/10/2022.
//

#ifndef BATH_TYPES_H
#define BATH_TYPES_H

typedef enum {
    S_TEMPERATURE = 0,
    S_GATE = 1,
    S_OUTLET_1 = 2,
    S_OUTLET_2 = 3
} eServo;

typedef enum {
    O_NONE            = 0b00,
    O_RAIN_SHOWER     = 0b01,
    O_HANDHELD_SHOWER = 0b10,
    O_BATH            = 0b11,
} eOutlet;

typedef enum {
    forward = 0,
    backward = 1,
    off = 2,
} eDirection;

typedef enum {
    R_MOT_DIR1,
    R_MOT_DIR2,
    R_PLUGTVSEL,
    R_PUMP1,
    R_PUMP2,
    R_PUMP3,
    R_PUMP4,
    R_PURGE
} eRelays;

#endif //BATH_TYPES_H
