#include "Arduino.h"
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>

#include "menu.h"
#include "temperature.h"

#define OUTLET_Y_POS 0
#define TEMP_Y_POS   16
#define MODE_Y_POS   38
#define NEXT_Y_POS   56

const char *outlets[] = {"Bath", "Handheld Shower", "Rain Shower"};
const uint8_t outletSize[] = {4, 15, 11};

const char *bathModes[] = {"Stopped", "Running", "Draining"};
const uint8_t bathModesSize[] = {7, 7, 8, 12};
const char *bathModeSelect[] = {"Stop", "Run", "Drain", "Bubbles Menu", "TV Up", "TV Down"};
const uint8_t bathModeSelectSize[] = {4, 3, 5, 12, 5, 7};

const char *showerModes[] = {"Stopped", "Running"};
const uint8_t showerModesSize[] = {7, 7};
const char *showerModeSelect[] = {"Stop", "Run"};
const uint8_t showerModeSelectSize[] = {3, 4};
uint8_t bathMode;

extern Adafruit_SH1106G display;
extern uint8_t menuSelection;

//Int
void writeLineSmall(uint8_t left, uint8_t right, uint8_t count, uint8_t yPos);
void writeLineLarge(uint8_t left, uint8_t right, uint8_t count, uint8_t yPos);

//Ext
void buildDisplay(uint8_t selectionIdx, uint8_t outletSelection, uint8_t temperature, uint8_t mode);
void setBathMode(uint8_t mode);
void updateTemp(void);

void setBathMode(uint8_t mode) {
  bathMode = mode;
}

void updateTemp(void) {
    if (menuSelection == 2) {
        writeLineSmall(0, 0, 4, NEXT_Y_POS);
        display.print("[OK]  ");
        display.print(getSourceTemp());
        display.print("C");
    } else {
        writeLineSmall(0, 0, 6, NEXT_Y_POS);
        display.print("[NEXT]  ");
        display.print(getSourceTemp());
        display.print("C");
    }
    display.display();
}
void buildDisplay(uint8_t selectionIdx, uint8_t outletSelection, uint8_t temperature, uint8_t mode) {
    uint8_t left = 0;
    uint8_t right = 0;

    if (selectionIdx == 0) {
        if (outletSelection > 0) {
            left = 1;
        }
        if (outletSelection < 2) {
            right = 1;
        }
    } else {
        left = 0;
        right = 0;
    }
    writeLineSmall(left, right, outletSize[outletSelection], OUTLET_Y_POS);
    display.print(outlets[outletSelection]);

    if (outletSelection == 3) {
        writeLineLarge(0, 0, 3, 22);
    } else {
        if (selectionIdx == 1) {
            if (temperature > 30) {
                left = 1;
            }
            if (temperature <= 45) {
                right = 1;
            }
        } else {
            left = 0;
            right = 0;
        }
        writeLineLarge(left, right, 3, TEMP_Y_POS);
        display.print(temperature);
        display.print("C");
    }

    if (selectionIdx == 2 || selectionIdx == 3) {
        if (mode > 0) {
            left = 1;
        }
        if (mode <= 4) {
            right = 1;
        }
    } else {
        left = 0;
        right = 0;
    }
    if (outletSelection == 0) {
        uint8_t tempMode = mode;
        if (selectionIdx != 2 && mode > 2) {
            tempMode = bathMode;
        }
        writeLineSmall(left, right, selectionIdx == 2 ? bathModeSelectSize[mode] : bathModesSize[tempMode], MODE_Y_POS);
        display.print(selectionIdx == 2 ? bathModeSelect[mode] : bathModes[tempMode]);
    }
    if (outletSelection == 1 || outletSelection == 2) {
        writeLineSmall(left, right, selectionIdx == 2 ? showerModeSelectSize[mode] : showerModesSize[mode], MODE_Y_POS);
        display.print(selectionIdx == 2 ? showerModeSelect[mode] : showerModes[mode]);
    }
    if (selectionIdx == 2) {
        writeLineSmall(0, 0, 4, NEXT_Y_POS);
        display.print("[OK]  ");
        display.print(getSourceTemp());
        display.print("C");
    } else {
        writeLineSmall(0, 0, 6, NEXT_Y_POS);
        display.print("[NEXT]  ");
        display.print(getSourceTemp());
        display.print("C");
    }
    display.display();
}


void writeLineSmall(uint8_t left, uint8_t right, uint8_t count, uint8_t yPos) {
    display.setTextSize(1);
    uint8_t maxwidth = 21;

    if(count > maxwidth) {
        return;
    }

    uint8_t pad = ((maxwidth*6) - (count * 6))/2;

    display.setCursor(0, yPos);
    display.print("                     ");

    display.setCursor(0, yPos);
    display.print(left ? "<" : " ");

    display.setCursor(122, yPos);
    display.print(right ? ">" : " ");

    display.setCursor(pad, yPos);
}

void writeLineLarge(uint8_t left, uint8_t right, uint8_t count, uint8_t yPos) {
    display.setTextSize(2);
    uint8_t maxwidth = 10;

    if(count > maxwidth) {
        return;
    }

    uint8_t pad = ((maxwidth*13) - (count * 12))/2;

    display.setCursor(0, yPos);
    display.print("          ");

    display.setCursor(0, yPos);
    display.print(left ? "<" : " ");

    display.setCursor(116, yPos);
    display.print(right ? ">" : " ");

    display.setCursor(pad, yPos);
}
//==========================//
//
//buildDisplay(1, 0, 30, 1);
//delay(2000);
//buildDisplay(2, 0, 30, 1);
//delay(2000);
//buildDisplay(2, 0, 30, 2);
//delay(2000);
//buildDisplay(0, 0, 30, 2);
//delay(2000);
//buildDisplay(2, 0, 30, 3);
//delay(2000);
//buildDisplay(0, 0, 30, 3);
//delay(2000);
//buildDisplay(0, 1, 30, 2);
//delay(2000);
//buildDisplay(2, 3, 30, 0);
//delay(2000);
//buildDisplay(2, 3, 30, 1);
//delay(2000);
//buildDisplay(0, 3, 30, 0);
//delay(2000);
//buildDisplay(0, 3, 30, 1);
//delay(2000);
//buildDisplay(0, 0, 30, 1);
//delay(2000);
//buildDisplay(0, 0, 30, 0);
//delay(2000);
//
//display.clearDisplay();
//display.setCursor(0,0);
//display.setTextSize(1);
//
//writeLineSmall(0,1,9,0);
//display.print("Flavour 1");
//
//display.setCursor(0,40);
//display.setTextSize(1);
//display.println("Dose");
//display.println("Bath");
//
//
//writeLineLarge(0,0,3,20);
//display.print("---");
//
//display.setCursor(100,45);
//display.setTextSize(1);
//display.println("Exit");
//display.display();
//delay(2000);
//
//writeLineSmall(1,1,9,0);
//display.print("Flavour 2");
//display.display();
//delay(2000);
//
//writeLineSmall(1,0,9,0);
//display.print("Flavour 3");
//display.display();
//delay(2000);
//
//writeLineSmall(0,0,9,0);
//display.print("Flavour 3");
//writeLineLarge(1,1,3,20);
//display.print("---");
//
//display.display();
//
//==========================//
