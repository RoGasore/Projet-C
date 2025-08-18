#ifndef COMPTEURQUANTGAZ_H
#define COMPTEURQUANTGAZ_H

#include <Arduino.h>

extern int resteGaz;
extern int temperatureSouhaitee;

void CompteurSetup();
void CompteurLoop();

#endif // VANNE_PROPORTIONNELLE_H
