#include "Arduino.h"
#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include "structs.h"
#include "main.h"
#include "utils.h"
#include "devices.h"


// Ultrasonic
void usDevice(byte selectedMenuItem) {
    Button_t buttonPressed;      // Contient le bouton appuyé
    bool scroll = false;

    Menu_t MENU = US_MENU;

 
    if (selectedMenuItem == 0){
         displayMenu(MAIN_MENU);
    }
    else if (selectedMenuItem == 255){
         displayMenu(MENU);
    }
    else if (selectedMenuItem <= MENU.nbParams){
        lcd.setCursor(8, 0);
        lcd.blink_on();	
        lcd.cursor_on();


        while (1) {
            lcd.setCursor(8, 0);
            lcd.print("       ");
            lcd.setCursor(8, 0);
            lcd.print(MENU.params[selectedMenuItem-1]);

            if (!scroll){
                while(readPushButton() != BP_NONE);
                while((buttonPressed = readPushButton()) == BP_NONE);
                delay(30);
                
                unsigned long start = millis();
                while(readPushButton() != BP_NONE){
                    if (millis() - start > 500) {
                        scroll = true;
                        break;
                    }
                };

            } 
            else if (readPushButton() == BP_NONE && scroll == true) {
                scroll = false;
            }
            else {
                delay(40);
            }
        
            /* Gére l'appui sur le bouton */
            switch(buttonPressed) {
                case BP_UP: 
                    if (MENU.params[selectedMenuItem-1] < MENU.limits[selectedMenuItem-1].max) {
                        MENU.params[selectedMenuItem-1] = MENU.params[selectedMenuItem-1] + MENU.limits[selectedMenuItem-1].step;
                    }
                    break;

                case BP_DOWN: // Bouton bas = choix suivant
                    if (MENU.params[selectedMenuItem-1] > MENU.limits[selectedMenuItem-1].min) {
                        MENU.params[selectedMenuItem-1] = MENU.params[selectedMenuItem-1] - MENU.limits[selectedMenuItem-1].step;
                    }
                    break;
            
                case BP_SELECT:
                    lcd.blink_off();	
                    lcd.cursor_off();
                    return; 
            }
            
            
        }

    
    }
    else {


        digitalWrite(LED_BUILTIN, HIGH);
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("CANCEL ANY KEY");
        lcd.setCursor(0, 1);
        lcd.print("Dist (cm): ");


        int triggerPin = 4;  // Arduino pin tied to trigger pin on the ultrasonic sensor.
        int echoPin = 3;  // Arduino pin tied to echo pin on the ultrasonic sensor.

        int maxDistance = MENU.params[0];
        NewPing sonar(triggerPin, echoPin, 500); // NewPing setup of pins and maximum distance.

        int oldDist = sonar.ping_cm();
        int newDist;
        while ((buttonPressed = readPushButton()) == BP_NONE){
            delay(100);
            newDist = sonar.ping_cm();
            if (newDist > maxDistance) {
                    newDist = maxDistance;
                }
            if (newDist != oldDist) {
                
                oldDist = newDist;
                lcd.setCursor(11, 1);
                lcd.print("     ");
                lcd.setCursor(11, 1);
                lcd.print(oldDist);
            }
             
           
        }

        digitalWrite(LED_BUILTIN, LOW);
        
    }


}

