#include <Arduino.h>
#include "CapteurGaz.h"
#include "CapteurTemp.h"
#include "Electrovanne.h"
#include "ManoDig.h"

void setup() {
  // Initialiser la communication série du port COM (optionnel)
  Serial.begin(9600);
  CapteurGazSetup();
  CapteurTempSetup();
  ElectrovanneSetup();
  ManoDigSetup();
}

void loop() {
  CapteurGazLoop();
  CapteurTempLoop();
  ElectrovanneLoop();
  ManoDigLoop();
}
