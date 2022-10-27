#ifndef BATH_TEMPERATURE_H
#define BATH_TEMPERATURE_H

extern void initialiseTemperature(void);

extern void runTemperatureControl(void);

extern void setTemperatureSP(uint8_t temp);

extern uint8_t isSourceNearTemperature(void);

extern uint8_t isSourceAtTemperature(void);

extern uint8_t isReturnNearTemperature(void);

extern uint8_t isReturnAtTemperature(void);

extern int8_t getSourceError(void);

extern int8_t getReturnError(void);

extern uint8_t getSourceTemp(void);

#endif //BATH_TEMPERATURE_H
