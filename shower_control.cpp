#include "Arduino.h"
#include "types.h"
#include "servos.h"
#include "relays.h"
#include "temperature.h"

typedef enum {
    SCS_WAIT_START,
    SCS_SET_OUTLET_FOR_PURGE,
    SCS_PERFORM_PURGE,
    SCS_WAIT_PURGE,
    SCS_PREP_TEMP,
    SCS_WAIT_TEMP,
    SCS_LOOP,
    SCS_RESET
} eShowerControlState;

typedef enum {
    RAIN,
    HANDHELD,
    OFF
} eState;

eState state;
eState lastState;
eShowerControlState showerControlState;

extern uint8_t temperatureSetpoint;

void runShowerControl(void);

void startHandheldShower(void);

void stopHandheldShower(void);

void startRainShower(void);

void stopRainShower(void);

void startHandheldShower(void) {
    state = HANDHELD;
}

void stopHandheldShower(void) {
    state = OFF;
}

void startRainShower(void) {
    state = RAIN;
}

void stopRainShower(void) {
    state = OFF;
}

void runShowerControl(void) {
    switch (showerControlState) {
        case SCS_WAIT_START:
            if (state != OFF) {
                showerControlState = SCS_SET_OUTLET_FOR_PURGE;
                lastState = OFF;
            }
            break;

        case SCS_SET_OUTLET_FOR_PURGE:
            setGate(0);
            setOutlet(O_RAIN_SHOWER);
            showerControlState = SCS_PERFORM_PURGE;
            break;

        case SCS_PERFORM_PURGE:
            purgeControl(1);
            setTemperatureSP(45);
            setGate(1);
            showerControlState = SCS_WAIT_PURGE;
            break;

        case SCS_WAIT_PURGE:
            if (isReturnNearTemperature()) {
                showerControlState = SCS_PREP_TEMP;
            }
            break;

        case SCS_PREP_TEMP:
            setTemperatureSP(temperatureSetpoint);
            showerControlState = SCS_WAIT_TEMP;
            break;

        case SCS_WAIT_TEMP:
            if (isSourceAtTemperature()) {
                purgeControl(0);
                showerControlState = SCS_LOOP;
            }
            break;

        case SCS_LOOP:
            if (lastState != state) {
                if (state == RAIN) {
                    setOutlet(O_RAIN_SHOWER);
                    setGate(1);
                    lastState = RAIN;
                }
                if (state == HANDHELD) {
                    setOutlet(O_HANDHELD_SHOWER);
                    setGate(1);
                    lastState = HANDHELD;
                }
            }
            if (state == OFF) {
                setGate(0);
                lastState = OFF;
                showerControlState = SCS_RESET;
            }
            break;

        case SCS_RESET:
            setGate(0);
            purgeControl(0);
            showerControlState = SCS_WAIT_START;
            break;
    }
}