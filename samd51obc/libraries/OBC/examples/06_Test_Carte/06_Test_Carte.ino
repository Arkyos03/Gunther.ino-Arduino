// Test complet de la carte : LEDs, switchs, jacks, carte SD, batterie, bus I2C et SPI

#include <OBC.h>
#include <Wire.h>
#include <SPI.h>

uint8_t lireRegistreBMI088(uint8_t cs, uint8_t reg, bool accel) {
  SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  SPI1.transfer(reg | 0x80);           // bit 7 = lecture
  if (accel) SPI1.transfer(0x00);      // l'accéléro renvoie un octet "dummy" d'abord
  uint8_t v = SPI1.transfer(0x00);
  digitalWrite(cs, HIGH);
  SPI1.endTransaction();
  return v;
}

void setup() {
  Serial.begin(115200);
  OBC.begin();
  Wire.begin();
  SPI1.begin();
  while (!Serial && millis() < 5000) { }

  Serial.println("=== Test OBC-STD Minifs ===");

  // BMP390 : registre CHIP_ID (0x00) = 0x60
  Wire.beginTransmission(0x76);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(0x76, 1);
  int id = Wire.read();
  Serial.printf("BMP390 chip id : 0x%02X %s\n", id, id == 0x60 ? "OK" : "ERREUR");

  // BMI088 : la 1re lecture de l'accéléro le passe en mode SPI
  lireRegistreBMI088(BMI088_ACCEL_CS, 0x00, true);
  uint8_t acc = lireRegistreBMI088(BMI088_ACCEL_CS, 0x00, true);   // 0x1E attendu
  uint8_t gyr = lireRegistreBMI088(BMI088_GYRO_CS, 0x00, false);   // 0x0F attendu
  Serial.printf("BMI088 accel id : 0x%02X %s\n", acc, acc == 0x1E ? "OK" : "ERREUR");
  Serial.printf("BMI088 gyro id  : 0x%02X %s\n", gyr, gyr == 0x0F ? "OK" : "ERREUR");

  OBC.beepCount(3);
}

void loop() {
  static uint8_t led = 1;
  OBC.allLeds(false);
  OBC.led(led, true);
  led = (led % 4) + 1;

  Serial.printf("Batterie %.2f V | SD %s | SW %d%d%d%d | JACK %d%d\n",
                OBC.batteryVoltage(),
                OBC.sdCardPresent() ? "oui" : "non",
                OBC.switchClosed(1), OBC.switchClosed(2), OBC.switchClosed(3), OBC.switchClosed(4),
                OBC.jackPlugged(1), OBC.jackPlugged(2));
  delay(500);
}
