#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../config.h"

#define SS_PIN 5
#define RST_PIN 22
#define LED_VERDE 12 
#define LED_ROJO  14 

MFRC522 rfid(SS_PIN, RST_PIN);

const char* serverUrl = "http://192.168.1.3/RegistroTracking/includes/ingresorfid.php";

// Placa NodeMCU-32S

const String Area = "5";  // operaciones 1

void setup() {
  Serial.begin(115200);
  Serial.print("\033[2J");
  delay(100);

  pinMode(LED_VERDE, OUTPUT);

  pinMode(LED_ROJO, OUTPUT);

  // Inicializar el lector RFID
  SPI.begin();           
  rfid.PCD_Init();      

  // Conexión WiFi
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) { // Esperar un máximo de 10 intentos
    delay(1000); // Esperar 1 segundo entre intentos
    Serial.print("Intento ");
    Serial.println(attempts + 1);
    attempts++;
  }

  // Verificar si se conectó exitosamente
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado a la red WiFi!");
    Serial.println(WiFi.localIP());  // Imprimir la dirección IP asignada
  } else {
    Serial.println("No se pudo conectar a la red WiFi.");
  }
}

void loop() {
  delay(200);
  digitalWrite(LED_VERDE, 0); 
  digitalWrite(LED_ROJO, 0);
  // Verificar si hay una nueva tarjeta presente
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("Tarjeta Leída");
    String uidStr = "";
    // Leer el UID de la tarjeta

    for (byte i = 0; i < rfid.uid.size; i++) {
      uidStr += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      uidStr += String(rfid.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase(); 
    Serial.print("UID leído: ");
    Serial.println(uidStr);
    Serial.println(Area);
    String datos_a_enviar = "uid=" + uidStr + "&Area=" + Area;  // Cambié el formato de envío de datos
    
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");      

    int codigo_respuesta = http.POST(datos_a_enviar);
    String payload = http.getString();
    Serial.println("------------------------------------");
    Serial.println(serverUrl);
    Serial.println(datos_a_enviar);
    Serial.println(codigo_respuesta);
    Serial.println(payload);
    Serial.println("------------------------------------");
    int lastIndex = payload.lastIndexOf('\n');
    String ultimaLinea;
    if (lastIndex != -1) {
        ultimaLinea = payload.substring(lastIndex + 1); // Extraer la última línea
        Serial.println("Última línea: " + ultimaLinea); 
        if (ultimaLinea == "Acceso permitido al área") {
            Serial.println(ultimaLinea);
            digitalWrite(LED_VERDE, 1); 
            delay(500);
            digitalWrite(LED_VERDE, 0);
        } else {
            Serial.println(ultimaLinea);
            digitalWrite(LED_ROJO, 1); 
            delay(500); 
            digitalWrite(LED_ROJO, 0); 
        }
    } else {
            Serial.println("No se encontraron resultados");
        }

  }
}
