#ifndef BATH_MENU_H
#define BATH_MENU_H

extern void buildDisplay(uint8_t selectionIdx, uint8_t outletSelection, uint8_t temperature, uint8_t mode);
extern void setBathMode(uint8_t mode);
extern void updateTemp(void);

#endif //BATH_MENU_H
