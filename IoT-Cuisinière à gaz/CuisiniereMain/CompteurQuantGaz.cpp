#include <Arduino.h>
#include "CompteurQuantGaz.h"

int tempsEcoule = 0;
int resteGaz;
extern int temperatureSouhaitee;

void CompteurSetup(){
  Serial.begin(9600);
}
void CompteurLoop(){
  tempsEcoule = millis();
  resteGaz = 100 - ((temperatureSouhaitee * tempsEcoule)/100);
}