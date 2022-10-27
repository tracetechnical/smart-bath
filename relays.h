#ifndef BATH_RELAYS_H
#define BATH_RELAYS_H

#include "types.h"

extern void pumpControl(eDirection direction, uint8_t pumpIdx);

extern void purgeControl(uint8_t state);

extern void plugControl(uint8_t state);

extern void tvControl(uint8_t state);

#endif //BATH_RELAYS_H
