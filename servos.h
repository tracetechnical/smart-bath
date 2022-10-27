#ifndef BATH_SERVOS_H
#define BATH_SERVOS_H

#include "types.h" 

extern void initialiseServos(void);

extern void setOutlet(eOutlet outlet);

extern void setGate(uint8_t state);

extern void setTemperaturePosition(uint8_t position);

#endif //BATH_SERVOS_H  
