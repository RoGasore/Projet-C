// Importation des fonctions nécessaires du SDK Firebase
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.3/firebase-app.js";
import { getDatabase, ref, onValue, set } from "https://www.gstatic.com/firebasejs/10.12.3/firebase-database.js";

// Configuration de Firebase pour l'application web
const firebaseConfig = {
  apiKey: "AIzaSyAXUhDp7WEIeLEsJnVq5W9Hs9N-K8Ba9Nc",
  authDomain: "gaz-guard1.firebaseapp.com",
  databaseURL: "https://gaz-guard1-default-rtdb.firebaseio.com",
  projectId: "gaz-guard1",
  storageBucket: "gaz-guard1.appspot.com",
  messagingSenderId: "102101497881",
  appId: "1:102101497881:web:2ef739962cae5947409288"
};

// Initialisation de Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Fonction pour mettre à jour les données du capteur sur la page web
function updateSensorData(ppm, message, resteGaz, currentTemp) {
  // Mettre à jour le paragraphe du niveau de gaz
  document.getElementById("ppm").textContent = `Niveau de gaz: ${ppm}`;
  
  // Mettre à jour le paragraphe du message
  document.getElementById("message").textContent = `Message: ${message}`;

  document.getElementById("QantRest").textContent = ` ${resteGaz}`;

  

// Mettre à jour la température actuelle
  if (currentTemp !== undefined && currentTemp !== null) {
    document.getElementById("current-temp").textContent = `${currentTemp}°C`;
  } else {
    document.getElementById("current-temp").textContent = `données indisponibles`;
  }
}

// Référence aux données du capteur dans la base de données Firebase
const ppmRef = ref(database, '/capteurGaz/ppm');
const messageRef = ref(database, '/capteurGaz/message');
const currentTempRef = ref(database, '/capteurTemperature/temperature');

// Écouter les changements dans les données du capteur
onValue(ppmRef, (snapshot) => {
  const ppm = snapshot.val();
  updateSensorData(ppm, document.getElementById("message").textContent.split(": ")[1], document.getElementById("current-temp").textContent.split(": ")[1]);
});

onValue(messageRef, (snapshot) => {
  const message = snapshot.val();
  updateSensorData(document.getElementById("ppm").textContent.split(": ")[1], message, document.getElementById("current-temp").textContent.split(": ")[1]);
});

onValue(currentTempRef, (snapshot) => {
  const currentTemp = snapshot.val();
  console.log("Current Temp:", currentTemp);
  updateSensorData(document.getElementById("ppm").textContent.split(": ")[1], document.getElementById("message").textContent.split(": ")[1], currentTemp);
});

// Références aux éléments DOM pour la température
const temperatureInput = document.getElementById("temperature");
const sendTemperatureButton = document.getElementById("sendTemperature");

// Fonction pour envoyer la température à Firebase
sendTemperatureButton.addEventListener("click", () => {
  const temperature = temperatureInput.value;
  if (temperature !== "" && temperature >= 20 && temperature <= 200) {
    set(ref(database, '/capteurTemperature/tempSouhaitee'), parseInt(temperature))
      .then(() => {
        alert("Température envoyée avec succès !");
      })
      .catch((error) => {
        console.error("Erreur lors de l'envoi de la température : ", error);
      });
  } else {
    alert("Veuillez entrer une température valide entre 20 et 200°C.");
  }
});
