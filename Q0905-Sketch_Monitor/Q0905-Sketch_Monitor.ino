#include "config.h"
#define pinLEDConect 16   //2-DEVKIT, WROOM  25-HELTEC  16-WEMOS 18650

// INCLUSÃO DE BIBLIOTECAS
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //MQTT
#include <Wire.h>
#include "SSD1306Wire.h"  //DISPLAY

//Dados do WiFi
//const char* ssid = "xxxxxx";
//const char* password = "xxxxxx";

// CONFIGURACAO DO MQTT BROKER (HIVEMQ)
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;

// TEXTO PARA RECEBER MENSAGEM 
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];

// CONFIGURACAO DO DISPLAY
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
SSD1306Wire oled(0x3c, SDA, SCL);

// VARIAVEIS PARA MOSTRAR NO DISPLAY
float temperatura = 0;
float umidade = 0;
float antTemperatura = 0;
float antUmidade = 0;

void setup() {
  Serial.begin(115200);

  oled.init();
  oled.flipScreenVertically();
  oled.clear();
  oled.setFont(ArialMT_Plain_24);
  oled.setTextAlignment(TEXT_ALIGN_LEFT);
  oled.drawString(0, 30, "Iniciando...");
  oled.display();
  
  pinMode(pinLEDConect, OUTPUT);
  conectaWiFi();

  //INICIO MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(recebeMsg);

  delay(500);
  Serial.println("Fim do setup()");
  oled.clear();
  oled.display();
}

void loop() {
  conectaWiFi();
  conectaMQTT();

  if ((antTemperatura != temperatura) || (antUmidade != umidade)) {

     String umidTxt = String(umidade,0);
     String tempTxt = String(temperatura,1);

     umidTxt += "%";
     tempTxt.replace(".",",");
     tempTxt += "°C";
    
     oled.clear(); 
     oled.setFont(ArialMT_Plain_16);
     oled.drawString(0, 0, umidTxt);

     oled.setFont(ArialMT_Plain_24);
     oled.drawString(0, 20, tempTxt);
     oled.display();
  }

  antTemperatura = temperatura;
  antUmidade = umidade;
}

void conectaWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Conectando");
    int contador = 0;
    while ((WiFi.status() != WL_CONNECTED) && (contador < 20)) {
      digitalWrite(pinLEDConect, !digitalRead(pinLEDConect));
      delay(500);
      Serial.print(".");
      contador++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(pinLEDConect, LOW);
      Serial.println("");
      Serial.print("Conectado na rede WiFi com o endereco IP: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void conectaMQTT() {
  if (!mqttClient.connected()) {
    Serial.println("Conectando ao Broker MQTT...");
    while (!mqttClient.connected()) {
      Serial.println("Reconectando ao Broker MQTT...");
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);

      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Conectado.");
        // subscribe nos topicos
        mqttClient.subscribe("temperatura");
        mqttClient.subscribe("umidade");
      }
    }
  }
  mqttClient.loop();
}

void recebeMsg(char* topic, byte* mensagem, unsigned int length) {
  String msgRecebida;
  
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)mensagem[i]);
    msgRecebida += (char)mensagem[i];
  }
  Serial.println();

  if (String(topic) == "temperatura") {
     temperatura = msgRecebida.toFloat();
  }

  if (String(topic) == "umidade") {
     umidade = msgRecebida.toFloat();
  }
}
