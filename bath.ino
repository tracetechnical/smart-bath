#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <Servo.h>
#include <avr/power.h>

#include "menu.h"
#include "relays.h"
#include "servos.h"
#include "temperature.h"

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //

#define MIN_TEMP 30
#define MAX_TEMP 45

uint8_t menuSelection = 0;
uint8_t outletSelection = 0;
uint8_t temperature = MIN_TEMP;
uint8_t mode = 0;
uint8_t lastMenuSelection = 99;
uint8_t lastOutletSelection = 99;
uint8_t lastTemperature = 99;
uint8_t lastMode = 99;
unsigned long buttonPostscaler = 0;
uint8_t lockout1 = 0;
uint8_t lockout2 = 0;
uint8_t lockout3 = 0;
uint8_t modeChanged = 0;
uint8_t lastOutlet = 0;
uint8_t drainBath = 0;
uint8_t lastDrainBath = 0;
uint8_t recBathMode = 0;

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    Serial.begin(9600);

//    pca.begin();
//    pca.setPWMFreq(60);  // Analog servos run at ~60 Hz update
//
    initialiseServos();
    initialiseTemperature();

    delay(250); // wait for the OLED to power up
    display.begin(i2c_Address, true); // Address 0x3C default
    //display.setContrast (0); // dim display

    display.oled_command(0xDB);
    display.oled_command(0x70);

    display.oled_command(0xA6);
    display.oled_command(0xAF);

    display.oled_command(0xC0);
    display.oled_command(0xA0);

    display.setContrast(255);

    display.display();
    delay(2);


    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE, 0);

    display.display();
    buildDisplay(0, 0, 30, 1);
    delay(20);

    Wire.begin(0x20);

    return;
}

//
//    buildDisplay(1, 0, 30, 1);
//    delay(2000);
//    buildDisplay(2, 0, 30, 1);
//    delay(2000);
//    buildDisplay(2, 0, 30, 2);
//    delay(2000);
//    buildDisplay(0, 0, 30, 2);
//    delay(2000);
//    buildDisplay(2, 0, 30, 3);
//    delay(2000);
//    buildDisplay(0, 0, 30, 3);
//    delay(2000);
//    buildDisplay(0, 1, 30, 2);
//    delay(2000);
//    buildDisplay(2, 3, 30, 0);
//    delay(2000);
//    buildDisplay(2, 3, 30, 1);
//    delay(2000);
//    buildDisplay(0, 3, 30, 0);
//    delay(2000);
//    buildDisplay(0, 3, 30, 1);
//    delay(2000);
//    buildDisplay(0, 0, 30, 1);
//    delay(2000);
//    buildDisplay(0, 0, 30, 0);
//    delay(2000);
//
uint16_t post = 0;

void loop() {
    handleButtons();
    runTemperatureControl();
    if (post++ > 700) {
        post = 0;
        updateTemp();
    }
    if (menuSelection != lastMenuSelection ||
        outletSelection != lastOutletSelection ||
        temperature != lastTemperature ||
        mode != lastMode) {
        setTemperatureSP(temperature);
        buildDisplay(menuSelection, outletSelection, temperature, mode);
        lastMenuSelection = menuSelection;
        lastOutletSelection = outletSelection;
        lastTemperature = temperature;
        lastMode = mode;
    }
    if (drainBath != lastDrainBath) {
        plugControl(drainBath);
        lastDrainBath = drainBath;
    }

}

void handleButtons() {
    if (++buttonPostscaler > 50) {
        handleLeftButton();
        handleRightButton();
        handleOkButton();
        buttonPostscaler = 0;
    }
}

void handleLeftButton() {
    uint8_t leftButton = digitalRead(14);
    if (!leftButton) {
        lockout1 = 2;
        switch (menuSelection) {
            case 0:
                outletSelection -= outletSelection > 0 ? 1 : 0;
                break;
            case 1:
                temperature -= temperature > MIN_TEMP ? 1 : 0;
                break;
            case 2:
            case 3:
                modeChanged = 1;
                if(menuSelection == 3) {
                    menuSelection = 2;
                }
                mode -= mode > 0 ? 1 : 0;
                break;
        }
    }
}

void handleOkButton() {
    uint8_t okButton = digitalRead(15);
    if (!okButton && !lockout2) {
        lockout2 = 3;
        if (lastOutlet != outletSelection) {
            if (menuSelection == 0) {
                mode = 0; //Stop
                modeChanged = 1;
            }
            switch (outletSelection) {
                case 0:
                    setOutlet(O_BATH);
                    break;
                case 1:
                    setOutlet(O_HANDHELD_SHOWER);
                    break;
                case 2:
                    setOutlet(O_RAIN_SHOWER);
                    break;
            }
            lastOutlet = outletSelection;
        }
        if (modeChanged) {
            if (mode == 0) {
                setGate(0);
            }
            if (mode == 1) {
                setGate(255);
            }
            if (outletSelection >= 1) {
                drainBath = 1; //Showers should always have open plug
            }
            if (outletSelection == 0) {
                if (mode <= 1) {
                    setBathMode(mode); //Record run/stop mode
                    recBathMode = mode;
                }
                if (mode == 2) {
                    drainBath = 1;
                }
                if (mode == 1) {
                    drainBath = 0;
                    plugControl(0);
                }
                if (mode == 3) {
                    //Add bubbles menu here
                    mode = 0;
                    setBathMode(0);
                }
                if (mode == 4) {
                    tvControl(1);
                    mode = recBathMode;
                }
                if (mode == 5) {
                    tvControl(0);
                    mode = recBathMode;
                }
            }
            menuSelection = 3;
            modeChanged = 0;
        } else {
            menuSelection++;
            if (menuSelection > 2) {
                menuSelection = 0;
            }
        }
    }
    if(okButton && lockout2 > 0) {
        lockout2--;
    }
}

void handleRightButton() {
    uint8_t rightButton = digitalRead(16);
    if (!rightButton) {
        lockout3 = 2;
        switch (menuSelection) {
            case 0:
                outletSelection += outletSelection < 2 ? 1 : 0;
                break;
            case 1:
                temperature += temperature < MAX_TEMP ? 1 : 0;
                break;
            case 2:
            case 3:
                modeChanged = 1;
                if(menuSelection == 3) {
                    menuSelection = 2;
                }
                switch (outletSelection) {
                    case 0:
                        mode += mode < 6 ? 1 : 0;
                        break;
                    case 1:
                    case 2:
                        mode += mode < 1 ? 1 : 0;
                        break;
                }
                break;
        }
    }
}

void bubblesMenu() {

//    display.clearDisplay();
//    display.setCursor(0,0);
//    display.setTextSize(1);
//
//    writeLineSmall(0,1,9,0);
//    display.print("Flavour 1");
//
//    display.setCursor(0,40);
//    display.setTextSize(1);
//    display.println("Dose");
//    display.println("Bath");
//
//
//    writeLineLarge(0,0,3,20);
//    display.print("---");
//
//    display.setCursor(100,45);
//    display.setTextSize(1);
//    display.println("Exit");
//    display.display();
//    delay(2000);
//
//    writeLineSmall(1,1,9,0);
//    display.print("Flavour 2");
//    display.display();
//    delay(2000);
//
//    writeLineSmall(1,0,9,0);
//    display.print("Flavour 3");
//    display.display();
//    delay(2000);
//
//    writeLineSmall(0,0,9,0);
//    display.print("Flavour 3");
//    writeLineLarge(1,1,3,20);
//    display.print("---");
//
//    display.display();
}
//
//void nope() {
//    handleDisplay();
//    handleTemperature();
//    handleButtons();
//    handleOutlets();
//    handleGate();
//
//    if (menuLevel == 0) {
//        onOff = 0;
//    }
//}
//
//void handleGate() {
//    if (onOff != lastOnOff) {
//        gate.write(onOff ? GATE_ON : GATE_OFF);
//        lastOnOff = onOff;
//    }
//}
//
//void handleDisplay() {
//    if (displayChanged) {
//        displayChanged = 0;
//        if (menuLevel == 0) {
//            mainMenu();
//        }
//        if (menuLevel == 1) {
//            runtimeMenu();
//        }
//        display.display();
//    }
//}

void handleTemperature() {
//    if (getTempPostscaler == 0) {
//        start = millis();
//        sensors.setWaitForConversion(false);
//        sensors.requestTemperatures();
//        getTempPostscaler = 1;
//    }
//    stop = millis();
//    if ((stop - start) > 1000 && getTempPostscaler == 1) {
//        sensors.setWaitForConversion(true);
//        stop2 = millis();
//        temperatureProcVal = sensors.getTempCByIndex(0);
////    Serial.println(sensors.getTempCByIndex(0));
//        getTempPostscaler = 0;
//        displayChanged = 1;
//    }
//    if (++runTempPostscaler > 40000) {
//        tempError = temperatureSetpoint - temperatureProcVal;
//        handleTrim();
//        float delta =
//                ((float) temperatureSetpoint - (float) MIN_SETPOINT) / ((float) MAX_SETPOINT - (float) MIN_SETPOINT);
//        delta *= 180;
//        tempTrim.write(180 - delta);
//        runTempPostscaler = 0;
//    }
}

//void handleTrim() {
//    trimVal = tempError / 5;
//}
//
//void handleOutlets() {
//    if (outletSelection != lastOutlet) {
//        //out thru
//        outlet0.write(outletSelection == 0 ? 0 : 180);
//        //out thru
//        outlet1.write(outletSelection == 1 ? 0 : 180);
//        lastOutlet = outletSelection;
//    }
//}
//
//
//void runtimeMenu() {
//    display.clearDisplay();
//    display.setTextSize(1);
//    switch (outletSelection) {
//        case 0:
//            display.setCursor(30, 0);
//            display.println("Handheld Shower");
//            break;
//        case 1:
//            display.setCursor(20, 0);
//            display.println("Rain Shower");
//            break;
//        case 2:
//            display.setCursor(50, 0);
//            display.println("Bath");
//            break;
//    }
//
//    displayTempSetpoint();
//
//    display.setTextSize(1);
//    if (onOff) {
//        display.setCursor(43, 55);
//        display.println("> STOP <");
//    } else {
//        display.setCursor(45, 55);
//        display.println("= RUN =");
//    }
//    display.setCursor(0, 45);
//    display.print("ERR");
//    display.setCursor(0, 55);
//    display.print(tempError);
//    display.print("C");
//    display.setCursor(110, 45);
//    display.print("PV");
//    display.setCursor(110, 55);
//    display.print(temperatureProcVal);
//    display.print("C");
//}
//
//void displayTempSetpoint() {
//    display.setTextColor(SH110X_WHITE, 0);
//    display.setTextSize(2);
//    display.setCursor(0, 25);
//    if (temperatureSetpoint > MIN_SETPOINT) {
//        display.println("<");
//    } else {
//        display.println(" ");
//    }
//    display.setCursor(116, 25);
//    if (temperatureSetpoint < MAX_SETPOINT) {
//        display.println(">");
//    } else {
//        display.println(" ");
//    }
//
//    display.setTextSize(3);
//    display.setTextColor(SH110X_WHITE, 0);
//    display.setCursor(40, 20);
//    display.print(temperatureSetpoint);
//    display.print("C");
//}
//
//void mainMenu() {
//    display.clearDisplay();
//    display.setTextSize(2);
//    switch (mainMenuSelection) {
//        case 0:
//            display.setCursor(40, 10);
//            display.println("Rain");
//            display.setCursor(30, 30);
//            display.println("Shower");
//            break;
//        case 1:
//            display.setCursor(20, 10);
//            display.println("Handheld");
//            display.setCursor(30, 30);
//            display.println("Shower");
//            break;
//        case 2:
//            display.setCursor(40, 20);
//            display.println("Bath");
//            break;
//    }
//    outletSelection = mainMenuSelection; //DEBUG!!
//    display.setTextSize(2);
//    if (mainMenuSelection > 0) {
//        display.setCursor(0, 20);
//        display.println("<");
//    }
//    if (mainMenuSelection < 2) {
//        display.setCursor(116, 20);
//        display.println(">");
//    }
//
//    display.setTextSize(1);
//    display.setCursor(50, 55);
//    display.println("= GO =");
//    display.display();
//}
////