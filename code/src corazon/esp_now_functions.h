//esp_now_functions.h

#ifndef ESP_NOW_FUNCTIONS_H
#define ESP_NOW_FUNCTIONS_H

#include <Arduino.h>
#include <WiFi.h>
#include <configuration.h>
#include <esp_now.h>


//===========================================//
//             Variables globales            //
//===========================================//

extern uint8_t peer_mac[6];
extern String nombrePlaca;
extern uint8_t heartType; // tipo de corazón actual (1-6)
//===========================================//
//           Prototipos de funciones         //
//===========================================//

uint8_t* stringToMac(String macStr);
void seleccion_placa(String& nombrePlaca);
void espNOW_setup();

// --------- Funciones Corazón ---------
void sendHeartType(uint8_t heartType);
void corRecvCallback(const esp_now_recv_info_t * info, const uint8_t * data, int data_len);

// --------- Funciones Tórax ----------
void askForHeartType();
void torRecvCallback(const esp_now_recv_info_t * info, const uint8_t * data, int data_len);

// --------- Callbacks ESP-NOW --------
void sentCallback(const esp_now_send_info_t *mac_addr, esp_now_send_status_t status);

#endif