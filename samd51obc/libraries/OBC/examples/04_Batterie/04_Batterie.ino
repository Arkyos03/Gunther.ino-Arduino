// Affiche la tension de la batterie dans le moniteur série

#include <OBC.h>

void setup() {
  Serial.begin(115200);
  OBC.begin();
}

void loop() {
  float v = OBC.batteryVoltage();
  Serial.print("Batterie : ");
  Serial.print(v, 2);
  Serial.println(" V");

  if (v < 7.0) OBC.beep(2000, 200);   // alerte batterie faible
  delay(1000);
}
