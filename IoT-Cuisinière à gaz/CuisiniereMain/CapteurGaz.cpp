#include <Arduino.h>
#include "CapteurGaz.h"

// Déclaration des broches
const int pinCapteurGaz = 34;
const int buzzer = 23;
// Initialisation des valeurs initiales
int valeurPrec = 0;
unsigned long lastMillis = 0;
int valeurCapteurGaz = 0; // Définir `valeurCapteurGaz` comme variable globale


void CapteurGazSetup() {
  Serial.begin(9600); // Initialisation du monitor série 
  pinMode(buzzer, OUTPUT); // Initialiser le port 25 comme une sortie digitale
}

void CapteurGazLoop() {
  // Obtention du temps actuel
  unsigned long currentMillis = millis();
  // Lecture de la valeur envoyée par le capteur
  valeurCapteurGaz = analogRead(pinCapteurGaz);

  if ((valeurCapteurGaz != valeurPrec) && (currentMillis - lastMillis >= 500)) {
    if (valeurCapteurGaz < 500) {
      digitalWrite(buzzer, LOW); // Eteindre le buzzer
    } else {
      digitalWrite(buzzer, HIGH); // Allumer le buzzer
    }
    // Affichage du niveau de gaz sur le monitor série 
    Serial.print("Niveau de Gaz : ");
    Serial.println(valeurCapteurGaz);

    // Mis à jour de la valeur précédente du capteur 
    valeurPrec = valeurCapteurGaz;
    // Mis à jour du temps écoulé 
    lastMillis = currentMillis;
  }
}
