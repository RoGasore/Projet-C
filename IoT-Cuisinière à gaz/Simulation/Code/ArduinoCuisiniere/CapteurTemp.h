#ifndef CAPTEURTEMP_H
#define CAPTEURTEMP_H

// Déclaration de "TempValue" comme une variable globale
extern float TempCel;
extern float valeurTempPrec;

void CapteurTempSetup();
void CapteurTempLoop();

#endif
