#include <Arduino.h>
#include "CapteurTemp.h"

// Déclaration des broches et variables
const int pinCapteurTemp = A1;
const int led = 12;
float valeurTempPrec = 0;
unsigned long lastTempMillis = 0;
int valeurTemp = 0;
float TempCel ;

void CapteurTempSetup() {
  Serial.begin(9600);    // Initialisation du port série
  pinMode(led, OUTPUT);  // Initialiser la LED comme une sortie digitale
}

void CapteurTempLoop() {
  // Obtention du temps actuel
  unsigned long currentTempMillis = millis();
  // Lecture de la valeur envoyée par le capteur de température
  valeurTemp = analogRead(pinCapteurTemp);
  TempCel = (valeurTemp / 1024.0) * 500;  // Conversion en Celsius

  if (currentTempMillis - lastTempMillis >= 500) {
    if (TempCel < 100) {
      digitalWrite(led, LOW);  // Éteindre le buzzer
    } else {
      digitalWrite(led, HIGH);  // Allumer le buzzer
    }

    if (TempCel != valeurTempPrec) {
      // Affichage de la température sur le monitor série
      Serial.print("TEMPERATURE in Celsius = ");  // Affichage de la température en Celsius
      Serial.print(TempCel);
      Serial.print("*C");
      Serial.print("    |    ");

      float TempFarh = (TempCel * 9) / 5 + 32;  // Conversion en Fahrenheit
      Serial.print("TEMPERATURE = ");           // Affichage de la température en Fahrenheit
      Serial.print(TempFarh);
      Serial.print("*F");
      Serial.println();

      // Mise à jour de la valeur précédente du capteur
      valeurTempPrec = TempCel;
    }
    // Mise à jour du temps écoulé
    lastTempMillis = currentTempMillis;
  }
}
