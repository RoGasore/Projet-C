#include <Arduino.h>
#include "ManoDig.h"
const int sensorPin = A2; // Broche analogique où le capteur est connecté
int sensorValuePrec = 0;

void ManoDigSetup() {
  Serial.begin(9600); // Initialiser la communication série
}

void ManoDigLoop() {
  // Lire la valeur analogique du capteur
  int sensorValue = analogRead(sensorPin);

  if(sensorValue != sensorValuePrec){
     // Convertir la valeur analogique en tension (0 - 5V)
  float voltage = sensorValue * (5.0 / 1023.0);

  // Convertir la tension en pression (kPa)
  // MPX4250 : 0.2V à 4.9V correspond à 20 kPa à 250 kPa
  float pressure_kPa = (voltage - 0.2) * (250.0 - 20.0) / (4.9 - 0.2) + 20.0;

  // Calculer la pression en pourcentage
  float pressure_percentage = (pressure_kPa - 20.0) * 100 / (250.0 - 20.0);
   // Afficher les résultats sur le moniteur série
  Serial.print("Tension (V): ");
  Serial.print(voltage);
  Serial.print(" | Pression (kPa): ");
  Serial.print(pressure_kPa);
  Serial.print(" | Pression (%): ");
  Serial.println(pressure_percentage); 

  sensorValuePrec = sensorValue;
  }

  
}
