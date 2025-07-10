//esp_now_functions.cpp
#include <WiFi.h>
#include <esp_now.h>
#include "esp_now_functions.h"


static const char* TAG = "ESP_NOW";

//===========================================//
//               Configuración               //
//===========================================//

void espNOW_setup(){ // Inicializa el ESP-NOW y registra los peers y los callbacks 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }
  
  seleccion_placa(nombrePlaca);
  
  // Registrar las funciones de callback
  if (nombrePlaca == "Corazón"){
    esp_now_register_recv_cb(corRecvCallback);
  } else if (nombrePlaca == "Tórax"){
    esp_now_register_recv_cb(torRecvCallback);
  } else {
    Serial.println("Placa no reconocida, revisa la MAC introducida");
  }
   // Cambair esto por el callback de Tor o Cor
  esp_now_register_send_cb(sentCallback);
  // Registrar el receptor (peer)
  esp_now_peer_info_t peerInfo = {};  // Limpia la estructura
  if (memcmp(peer_mac, (uint8_t[6]){0,0,0,0,0,0}, 6) == 0) {
    Serial.println("MAC inválida. No se registrará peer.");
    return;
  }
  memcpy(peerInfo.peer_addr, peer_mac, 6);
  peerInfo.channel = 0;  // Usar el mismo canal del Wi-Fi
  peerInfo.encrypt = false;  // Sin cifrado
  delay(100);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el receptor");
    return;
  }
  Serial.print("Peer registrado correctamente con MAC: ");
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
               peer_mac[0], peer_mac[1], peer_mac[2],
               peer_mac[3], peer_mac[4], peer_mac[5]);
}

uint8_t* stringToMac(String macStr) { // Pasa de la mac en formato string a uno reconocible por ESP-NOW
  static uint8_t macBytes[6];  // static para que no se destruya al salir de la función
  int macIndex = 0;
  for (int i = 0; i < macStr.length(); i += 3) {
    String byteStr = macStr.substring(i, i + 2);
    macBytes[macIndex++] = (uint8_t) strtol(byteStr.c_str(), NULL, 16);
  }
  return macBytes;
}

void seleccion_placa(String& nombrePlaca){
  delay(100);
  // TODO: Cambiar MACs
  static uint8_t mac1[] = {0x3C, 0x8A, 0x1F, 0x9A, 0x0C, 0x20}; // Corazón
  static uint8_t mac2[] = {0xF0, 0x24, 0xF9, 0x43, 0x4A, 0xB8}; // Tórax
  static uint8_t mac_null[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t* macAddress = stringToMac(WiFi.macAddress());
  Serial.printf("MAC leída: %02X:%02X:%02X:%02X:%02X:%02X\n",
               macAddress[0], macAddress[1], macAddress[2],
               macAddress[3], macAddress[4], macAddress[5]);
  if (memcmp(macAddress, mac1, 6) == 0) {
    nombrePlaca = "Corazón";
    memcpy(peer_mac, mac2, 6);
  } else if (memcmp(macAddress, mac2, 6) == 0) {
    nombrePlaca = "Tórax";
    memcpy(peer_mac, mac1, 6);
  } else {
    nombrePlaca = "Desconocido";
    memcpy(peer_mac, mac_null, 6);
  }
  Serial.print("Parte del cuerpo: ");
  Serial.println(nombrePlaca);

}

//===========================================//
//             Funciones Corazón             //
//===========================================//

void sendHeartType(uint8_t heartType){ // Manda el tipo de corazón que debe sonar
  // heartType debe estar entre 1 y 6
  if (heartType < 1 || heartType > 6) {
    Serial.printf("Valor de heartType fuera de rango (1-6)");
    return;
  }

  // enviamos un solo byte
  esp_err_t err = esp_now_send(peer_mac, &heartType, sizeof(heartType));
  if (err == ESP_OK) {
    Serial.printf("Enviado heartType=%u correctamente\n", heartType);
  } else {
    Serial.printf("Error al enviar heartType=%u: %d\n", heartType, err);
  }
}

void corRecvCallback(const esp_now_recv_info_t * info, const uint8_t * data, int data_len){ // Función de callback de la placa del corazón
  if (data_len == 8 && memcmp(data, "ASKTYPE", 7) == 0){
    sendHeartType(heartType);
  } else {
    Serial.printf("Tipo de dato inválido recibido");
  }
}

//===========================================//
//              Funciones Tórax              //
//===========================================//

void askForHeartType(){ // Cuando se le llame le pide al otro ESP32 el tipo de corazón
  const char request[] = "ASKTYPE";
  esp_err_t err = esp_now_send(peer_mac, (uint8_t*)request, sizeof(request));
  if (err == ESP_OK) {
    Serial.println(">> Petición ASKTYPE enviada");
  } else {
    Serial.printf("!! Error enviando ASKTYPE: %d\n", err);
  }
}

void torRecvCallback(const esp_now_recv_info_t * info, const uint8_t * data, int data_len){ // Función de callback de la placa del tórax
  if(data_len == sizeof(uint8_t)){
    uint8_t receivedHeartType = *data;
    if (receivedHeartType >= 1 && receivedHeartType <= 6) {
      heartType = receivedHeartType;
      Serial.printf("Tipo de corazón recibido: %u", heartType);
    } else {
      Serial.printf("Tipo de corazón recibido fuera de rango (1-6): %u", receivedHeartType);
    }
  } else {
    Serial.printf("Tipo de dato inválido recibido (len=%d)", data_len);
  }
}


//===========================================//
//              OnDataSentCbck               //
//===========================================//

void sentCallback(const uint8_t *mac_addr, esp_now_send_status_t status){
  Serial.print("Estado del envío: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito" : "Falló");
}
