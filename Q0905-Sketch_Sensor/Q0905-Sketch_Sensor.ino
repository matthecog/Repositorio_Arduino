#include "config.h"

#define pinLEDConect 16   //2-DEVKIT, WROOM  25-HELTEC  16-WEMOS 18650

// INCLUSÃO DE BIBLIOTECAS
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //MQTT
#include "dht.h"

#define DHTPIN  4       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
 
DHT dht(DHTPIN, DHTTYPE);
 
float h ;
float t;

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

void setup() {
  Serial.begin(115200);
  pinMode(pinLEDConect, OUTPUT);
  conectaWiFi();

  //INICIO MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(recebeMsg);

  delay(500);
  dht.begin();
  Serial.println("Fim do setup()");
}

void loop() {
  static float hAnt;
  static float tAnt;
  
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (isnan(h) || isnan(t) )  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  conectaWiFi();
  conectaMQTT();

  static unsigned long delayMsg = 0;
  static int value = 0;
  unsigned long now = millis();
  if ((now - delayMsg > 2000) && ((hAnt != h) || (tAnt != t)) ){
    delayMsg = now;

    snprintf (msg, MSG_BUFFER_SIZE, "%f", t);
    Serial.print("Publicando mensagem (temperatura): ");
    Serial.println(t);
    mqttClient.publish("temperatura", msg);

    snprintf (msg, MSG_BUFFER_SIZE, "%f", h);
    Serial.print("Publicando mensagem (umidade): ");
    Serial.println(h);
    mqttClient.publish("umidade", msg);
  }

  hAnt = h;
  tAnt = t;
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
        //mqttClient.subscribe("topicoTeste");
      }
    }
  }
  mqttClient.loop();
}

void recebeMsg(char* topic, byte* mensagem, unsigned int length) {
  Serial.print("Mensagem recebida: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)mensagem[i]);
  }
  Serial.println();
}
