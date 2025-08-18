#include <Arduino.h>
#include "Electrovanne.h"
#include "CapteurGaz.h"
#include <Servo.h>   // La bibliothèque Servo pour contrôler le servo moteur
#include <Keypad.h>  // La bibliothèque Keypad pour lire les entrées du clavier

int temperatureTarget = 0;  // Initialise la variable pour stocker la température cible
float angle = 0;            // Initialise la variable pour stocker l'angle du servo moteur

Servo myservo;  // Crée un objet Servo pour contrôler le servo moteur

const byte ROWS = 4;  // Définit le nombre de lignes du clavier
const byte COLS = 3;  // Définit le nombre de colonnes du clavier

// Définit la disposition des touches sur le clavier
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// Définit les broches Arduino connectées aux lignes du clavier
byte rowPins[ROWS] = { 10, 9, 8, 7 };
// Définit les broches Arduino connectées aux colonnes du clavier
byte colPins[COLS] = { 11, 12, 13 };

// Crée un objet Keypad pour gérer les entrées du clavier
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Fonction pour convertir la température en angle du servo
float temperature_Angle(float temperature) {
  float m = 180.0 / 1100.0;    // Pente de la conversion linéaire
  float b = -90.0;             // Décalage de la conversion linéaire
  return m * temperature + b;  // Calcule l'angle à partir de la température
}

void ElectrovanneSetup() {
  // Initialise la communication série à 9600 bauds
  Serial.begin(9600);
  // Attache le servo à la broche 4 de l'Arduino
  myservo.attach(4);
}

void ElectrovanneLoop() {
  if (valeurCapteurGaz > 500 && (valeurCapteurGaz != valeurPrec)) {
    Serial.println("Niveau de gaz eleve ! Fermeture du gaz.");
    myservo.write(0);  // Ramener le servomoteur à zéro
    valeurPrec = valeurCapteurGaz;
  } else {
    static int userInput = 0;    // Variable statique pour stocker l'entrée utilisateur
    static int digitCount = 0;   // Compteur statique pour les chiffres entrés
    char key = keypad.getKey();  // Lire une touche du clavier

    if (key != NO_KEY) {                           // Si une touche est pressée
      if (isdigit(key)) {                          // Si la touche est un chiffre
        userInput = userInput * 10 + (key - '0');  // Ajouter le chiffre à l'entrée utilisateur
        digitCount++;                              // Incrémenter le compteur de chiffres
        Serial.print(key);                         // Afficher le chiffre entré sur le moniteur série
      } else if (key == '*') {                     // Si l'utilisateur appuie sur '*'
        digitCount = 0;                            // Réinitialiser le compteur de chiffres
        userInput = 0;                             // Réinitialiser l'entrée utilisateur
        Serial.println("Réinitialisation de l'entrée.");
      } else if (key == '#') {  // Si l'utilisateur appuie sur '#'
        // Vérification de la température entrée
        if (digitCount >= 2 && userInput >= 10 && userInput <= 1100) {
          Serial.print("Temperature valide : ");
          Serial.println(userInput);

          // Convertir l'entrée utilisateur en température cible
          temperatureTarget = userInput;

          // Conversion de la température en angle du servo
          float tempObtenue = temperature_Angle(temperatureTarget);
          angle = map(tempObtenue, -90, 90, 1000, 2000);  // Conversion de l'angle en une plage de 0 à 180 degrés pour le servo

          // Définir l'angle du servo
          myservo.write(angle);

          Serial.print("Angle pour le servo : ");
          Serial.println(tempObtenue);
        } else {
          Serial.println("Entrée invalide. Veuillez entrer une température entre 10 et 1100.");
        }
        digitCount = 0;
        userInput = 0;
      }
    }
  }
}
