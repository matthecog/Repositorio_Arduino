#include <dht.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// ================== DHT ==================
#define pinSensorUT 24
dht sensorDHT;

float temperatura = 0.0;
float umidade = 0.0;
unsigned long lastDHT = 0;

// ================== DISPLAY ==================
byte seven_seg_digits[20][7] = {
  { 1,1,1,1,1,1,0 }, { 0,1,1,0,0,0,0 }, { 1,1,0,1,1,0,1 }, { 1,1,1,1,0,0,1 },
  { 0,1,1,0,0,1,1 }, { 1,0,1,1,0,1,1 }, { 1,0,1,1,1,1,1 }, { 1,1,1,0,0,0,0 },
  { 1,1,1,1,1,1,1 }, { 1,1,1,0,0,1,1 }, { 1,1,1,0,1,1,1 }, { 0,0,1,1,1,1,1 },  
  { 1,0,0,1,1,1,0 }, { 0,1,1,1,1,0,1 }, { 1,0,0,1,1,1,1 }, { 1,0,0,0,1,1,1 },  
  { 1,0,1,1,1,1,1 }, { 0,1,1,0,1,1,1 }, { 0,0,0,0,1,1,0 }, { 0,1,1,1,1,0,0 }, 
};

unsigned long lastDisplay = 0;
byte displayDigit = 0;

// ================== ETHERNET / MQTT ==================
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

const char* mqttserver = "1.1.1.1";
int mqttserverport = 1883;
const char* mqttuser = "user";
const char* mqttpass = "user";

EthernetClient ethClient;
PubSubClient mqtt (ethClient);

unsigned long lastMQTT = 0;

// ================== FUNÇÕES ================
void writePonto(byte dot) {       //Funcao que aciona o ponto no display  
  digitalWrite(37, dot);
}

void sevenSegWrite(byte digit) {  //Funcao que aciona o display
  byte pin = 30;
  for (byte i = 0; i < 7; i++) { //Percorre o array ligando os segmentos correspondentes ao digito 
    digitalWrite(pin++, seven_seg_digits[digit][i]);
  }
  writePonto(1);  //Liga o ponto
}

void reconnect() {
  mqtt.setServer(mqttserver, mqttserverport);

  while (!mqtt.connected()) {
    Serial.println("Conectando MQTT... ");
    if (mqtt.connect("sensor_temp_01",mqttuser,mqttpass)){
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc= ");
      Serial.println(mqtt.state());
      delay(2000);
    }
    
  }
}

// ================== SETUP ==================
void setup() {
  Serial.begin(9600);
  
  // start Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Falha ao configurar a Ethernet usando DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("O shield Ethernet não foi encontrado. Desculpe, não é possível executar sem hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("O cabo Ethernet não está conectado.");
      }
    while (true) {
    }
  }
  // Imprime o IP atual e o Status:
  Serial.print("IP Atual: ");
  Serial.println(Ethernet.localIP());
  Serial.println(Ethernet.linkStatus());

  reconnect();

  //MQTT Discovery
  String discoveryTemperatura =
    "{\"unique_id\":\"sensor_temperatura_escritorio_01\","
    "\"device_class\":\"temperature\","
    "\"unit_of_measurement\":\"°C\","
    "\"name\":\"Sensor Temp\","
    "\"state_topic\":\"arduinomesa/temperatura/state\"}";

  mqtt.publish(
    "homeassistant/sensor/arduinomesa/temperatura/config",
    discoveryTemperatura.c_str(),
    true
  );
  
  String discoveryUmidade =
    "{\"unique_id\":\"sensor_umidade_escritorio_01\","
    "\"device_class\":\"humidity\","
    "\"unit_of_measurement\":\"%\","
    "\"name\":\"Sensor Umidade\","
    "\"state_topic\":\"arduinomesa/umidade/state\"}";

  mqtt.publish(
    "homeassistant/sensor/arduinomesa/umidade/config",
    discoveryUmidade.c_str(),
    true
  );

  //Display_LED
  for (byte i = 30; i <= 37; i++) pinMode(i, OUTPUT);
  writePonto(0);
}

// ================== LOOP ==================
void loop() {

  if (!mqtt.connected()) reconnect();
  mqtt.loop();

  unsigned long now = millis();

  //Leitura DHT a cada 3s
  if (now - lastDHT > 3000) {
    lastDHT = now;
    sensorDHT.read(pinSensorUT);
    temperatura = sensorDHT.temperature;
    umidade = sensorDHT.humidity;

    //Imprime o sensor umidade/temperatura
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println("C");
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println("% ");
  }

  //Envio MQTT a cada 5s
  if (now - lastMQTT > 5000) {
    lastMQTT = now;

    char payloadTemp[10];
    char payloadHum[10];

    dtostrf(temperatura, 4, 1, payloadTemp);
    dtostrf(umidade, 4, 1, payloadHum);

 
    mqtt.publish("arduinomesa/temperatura/state", payloadTemp);      // temperatura
    mqtt.publish("arduinomesa/umidade/state", payloadHum);    // umidade

    Serial.print("Temp enviada MQTT: ");
    Serial.println(payloadTemp);
    Serial.print("Umidade enviada MQTT: ");
    Serial.println(payloadHum);
  
  }

  //Display_LED
  if (now - lastDisplay > 2000) {
    lastDisplay = now;
    sevenSegWrite(displayDigit);
  }
}
