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
#define pinBotaoDisplay 22

byte seven_seg_digits[20][7] = {
  { 1,1,1,1,1,1,0 }, { 0,1,1,0,0,0,0 }, { 1,1,0,1,1,0,1 }, { 1,1,1,1,0,0,1 },
  { 0,1,1,0,0,1,1 }, { 1,0,1,1,0,1,1 }, { 1,0,1,1,1,1,1 }, { 1,1,1,0,0,0,0 },
  { 1,1,1,1,1,1,1 }, { 1,1,1,0,0,1,1 }, { 1,1,1,0,1,1,1 }, { 0,0,1,1,1,1,1 },
  { 1,0,0,1,1,1,0 }, { 0,1,1,1,1,0,1 }, { 1,0,0,1,1,1,1 }, { 1,0,0,0,1,1,1 },
  { 1,0,1,1,1,1,1 }, { 0,1,1,0,1,1,1 }, { 0,0,0,0,1,1,0 }, { 0,1,1,1,1,0,0 },
};

const byte totalDisplayDigits = sizeof(seven_seg_digits) / sizeof(seven_seg_digits[0]);
const byte displayIntervalMinutes = 10;
const unsigned long displayIntervalMs = (unsigned long)displayIntervalMinutes * 60UL * 1000UL;
const unsigned long debounceDelayMs = 50;

byte displayDigit = 0;
byte buttonLastReading = HIGH;
byte buttonStableState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastDisplay = 0;

// ================== ETHERNET / MQTT ==================
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

const char* mqttserver = "192.168.1.224";
int mqttserverport = 1883;
const char* mqttuser = "matheus";
const char* mqttpass = "Matthecog";

EthernetClient ethClient;
PubSubClient mqtt (ethClient);

unsigned long lastMQTT = 0;
unsigned long lastMQTTReconnectAttempt = 0;
const unsigned long mqttReconnectIntervalMs = 5000UL;

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

void advanceDisplayDigit() {
  displayDigit = (displayDigit + 1) % totalDisplayDigits;
  sevenSegWrite(displayDigit);
}

bool reconnect() {
  mqtt.setServer(mqttserver, mqttserverport);

  Serial.println("Conectando MQTT... ");
  if (mqtt.connect("sensor_temp_01",mqttuser,mqttpass)) {
    Serial.println("Conectado!");
    return true;
  }

  Serial.print("Falhou, rc= ");
  Serial.println(mqtt.state());
  return false;
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

  if (reconnect()) {

  //MQTT Discovery
  String discoveryTemperatura =
    "{\"unique_id\":\"sensor_temperatura_escritorio_01\","
    "\"device_class\":\"temperature\","
    "\"unit_of_measurement\":\"°C\","
    "\"name\":\"Sensor de Temperatura\","
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
    "\"name\":\"Sensor de Umidade\","
    "\"state_topic\":\"arduinomesa/umidade/state\"}";

  mqtt.publish(
    "homeassistant/sensor/arduinomesa/umidade/config",
    discoveryUmidade.c_str(),
    true
  );
  }

  //Display_LED
  for (byte i = 30; i <= 37; i++) pinMode(i, OUTPUT);
  pinMode(pinBotaoDisplay, INPUT_PULLUP);

  sevenSegWrite(displayDigit);
  lastDisplay = millis();
}

// ================== LOOP ==================
void loop() {

  unsigned long now = millis();

  if (!mqtt.connected()) {
    if (now - lastMQTTReconnectAttempt >= mqttReconnectIntervalMs) {
      lastMQTTReconnectAttempt = now;
      reconnect();
    }
  } else {
    mqtt.loop();
  }

  //Leitura DHT a cada 5s
  if (now - lastDHT > 5000) {
    lastDHT = now;
    int statusDHT = sensorDHT.read(pinSensorUT);

    if (statusDHT == DHTLIB_OK) {
      temperatura = sensorDHT.temperature;
      umidade = sensorDHT.humidity;
    } else {
      Serial.print("Falha na leitura do DHT, codigo: ");
      Serial.println(statusDHT);
    }

    //Imprime o sensor umidade/temperatura
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println("C");
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println("% ");
  }

  //Envio MQTT a cada 10s
  if (now - lastMQTT > 10000) {
    lastMQTT = now;

    char payloadTemp[10];
    char payloadHum[10];

    dtostrf(temperatura, 4, 1, payloadTemp);
    dtostrf(umidade, 4, 1, payloadHum);

    if (mqtt.connected()) {
      mqtt.publish("arduinomesa/temperatura/state", payloadTemp);      // temperatura
      mqtt.publish("arduinomesa/umidade/state", payloadHum);    // umidade
    }

    Serial.print("Temp enviada MQTT: ");
    Serial.println(payloadTemp);
    Serial.print("Umidade enviada MQTT: ");
    Serial.println(payloadHum);
  }

  //Display_LED - muda a cada 10 minutos
  if (now - lastDisplay >= displayIntervalMs) {
    advanceDisplayDigit();
    lastDisplay = now;
  }

  //Display_LED - muda também no botão físico e reinicia cronômetro
  byte reading = digitalRead(pinBotaoDisplay);

  if (reading != buttonLastReading) {
    lastDebounceTime = now;
    buttonLastReading = reading;
  }

  if ((now - lastDebounceTime) > debounceDelayMs && reading != buttonStableState) {
    buttonStableState = reading;

    if (buttonStableState == LOW) {
      advanceDisplayDigit();
      lastDisplay = now;
    }
  }
}