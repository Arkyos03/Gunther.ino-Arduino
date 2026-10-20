// Ejection du parachute par servo
//  - le jack de décollage est branché sur JACK_1 (fermé = jack en place)
//  - quand le jack est arraché, on attend DELAI_EJECTION puis on ouvre la trappe

#include <OBC.h>
#include <Servo.h>

const unsigned long DELAI_EJECTION = 6000;   // ms après le décollage
const int ANGLE_FERME  = 0;
const int ANGLE_OUVERT = 90;

Servo trappe;
bool decolle = false;
unsigned long heureDecollage = 0;

void setup() {
  OBC.begin();
  trappe.attach(SERVO_1);
  trappe.write(ANGLE_FERME);
  OBC.beepCount(2);            // 2 bips : carte prête
}

void loop() {
  if (!decolle && OBC.liftoff()) {
    decolle = true;
    heureDecollage = millis();
    OBC.led(1, true);
  }

  if (decolle && millis() - heureDecollage >= DELAI_EJECTION) {
    trappe.write(ANGLE_OUVERT);
    OBC.led(2, true);
    OBC.beep(3000, 1000);
    while (true) { delay(100); } // fin du vol (delay garde l USB actif)
  }
}
