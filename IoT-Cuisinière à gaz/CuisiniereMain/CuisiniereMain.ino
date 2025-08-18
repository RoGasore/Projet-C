#include <Arduino.h>
#include <WiFi.h>
#include "LittleFS.h"
#include <Firebase_ESP_Client.h>
#include <ESPAsyncWebServer.h>
#include "CapteurGaz.h"
#include "CapteurTemperature.h"
#include "CompteurQuantGaz.h"

// Les informations du Wi-Fi sur lequel l'ESP va se connecter
#define WIFI_SSID "Pixel"
#define WIFI_PASSWORD "12345678ro"

// Les informations de votre base de données Firebase en ligne
#define FIREBASE_HOST "https://gaz-guard1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyAXUhDp7WEIeLEsJnVq5W9Hs9N-K8Ba9Nc"

// Objets Firebase
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Serveur web
AsyncWebServer server(80);
int temperatureSouhaitee;

// Fonction pour se connecter au Wi-Fi
void ConnectionWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connexion au Wi-Fi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connecté!");
  Serial.print("Adresse IP de l'ESP32 : ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Initialisation série pour le débogage
  Serial.begin(115200);

  // Initialisation des fichiers modulaires
  CapteurGazSetup();
  CapteurTempSetup();
  CompteurSetup();

  // Connexion au réseau wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("La cuisinière est connectée au réseau");

  // Affichage de l'adresse IP sur le moniteur série
  Serial.print("Adresse IP de l'ESP32 : ");
  Serial.println(WiFi.localIP());

  // Initialisation de LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Erreur de montage LittleFS");
    return;
  }

  // Configuration des routes du serveur web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script.js", "application/javascript");
  });

  server.begin();

  // Configuration Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialiser les chemins si nécessaire
  if (!Firebase.RTDB.getInt(&firebaseData, "/capteurTemperature/tempSouhaitee")) {
    Firebase.RTDB.setInt(&firebaseData, "/capteurTemperature/tempSouhaitee", 100);  // Valeur par défaut
    Serial.println("Chemin /capteurTemperature/tempSouhaitee initialisé.");
  }
  if (!Firebase.RTDB.getInt(&firebaseData, "/capteurTemperature/temperature")) {
    Firebase.RTDB.setInt(&firebaseData, "/capteurTemperature/temperature", 25);  // Valeur par défaut
    Serial.println("Chemin /capteurTemperature/temperature initialisé.");
  }
}

void loop() {
  CapteurGazLoop();
  CapteurTempLoop();
  CompteurLoop();

  // Vérification de la connexion Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi déconnecté, tentative de reconnexion...");
    ConnectionWiFi();
  }

  if (valeurCapteurGaz != valeurPrec) {  // Pour mettre à jour la base de données si la valeur du niveau de gaz a changé
    if (Firebase.RTDB.setInt(&firebaseData, "/capteurGaz/ppm", valeurCapteurGaz)) {
      Serial.println("La base de données est mise à jour avec succès");

      // Envoi de message à la base de données
      if (valeurCapteurGaz > 500) {
        if (Firebase.RTDB.setString(&firebaseData, "/capteurGaz/message", "Niveau de gaz élevé !")) {
          Serial.println("Alerte envoyée à Firebase.");
        } else {
          Serial.println("Erreur lors de l'envoi du message à Firebase.");
          Serial.println(firebaseData.errorReason());
        }
      } else {
        if (Firebase.RTDB.setString(&firebaseData, "/capteurGaz/message", "Niveau de gaz normal.")) {
          Serial.println("Message normal envoyé à Firebase.");
        } else {
          Serial.println("Erreur lors de l'envoi du message normal à Firebase.");
          Serial.println(firebaseData.errorReason());
        }
      }
    } else {
      Serial.println("Échec de la mise à jour de Firebase");
      Serial.println(firebaseData.errorReason());
    }
  }

  // Mise à jour des données de température dans Firebase
  if (temperatureSouhaitee = !0) {
    if (Firebase.RTDB.setInt(&firebaseData, "/capteurTemperature/temperature", temperatureSouhaitee)) {
      Serial.println("La température est mise à jour avec succès dans Firebase.");
    } else {
      Serial.println("Échec de la mise à jour de la température dans Firebase.");
      Serial.println(firebaseData.errorReason());
    }
  }

  // Lire la température cible depuis Firebase
  if (Firebase.RTDB.getInt(&firebaseData, "/capteurTemperature/tempSouhaitee")) {
    temperatureSouhaitee = firebaseData.intData();
    Serial.print("Température cible reçue de Firebase : ");
    Serial.println(temperatureSouhaitee);
  } else {
    Serial.println("Échec de la lecture de la température cible depuis Firebase.");
    Serial.println(firebaseData.errorReason());
  }

}