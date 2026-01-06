#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// ------------------------------------
// 1. CONFIGURAÇÕES DE REDE
// ------------------------------------
const char* ssid     = "";         // *** MUDAR AQUI *** Nome da sua rede Wi-Fi
const char* password = "";        // *** MUDAR AQUI *** Senha da sua rede Wi-Fi

// ------------------------------------
// 2. CONFIGURAÇÕES DE RELÉS
// ------------------------------------
// Define os pinos D1 (GPIO) que serão usados.
// D5 = GPIO14, D6 = GPIO12, D7 = GPIO13.
#define RELE_PIN_1 D5 
#define RELE_PIN_2 D6 
#define RELE_PIN_3 D7 

// --- ATENÇÃO --- 
// Se o seu módulo relé for Ativo em LOW, o sinal para ligar é LOW.
// Se for Ativo em HIGH (mais comum), o sinal para ligar é HIGH.
// O código abaixo assume que HIGH LIGA o relé. Se for Ativo em LOW, 
// você precisará inverter HIGH por LOW e LOW por HIGH na seção "Lógica de Controle".
#define RELAY_ON  LOW
#define RELAY_OFF HIGH  

// ------------------------------------
// 3. CONFIGURAÇÕES NTP
// ------------------------------------
// Servidor NTP brasileiro
const char* ntpServer = "a.st1.ntp.br"; 

// Fuso horário de Brasília (GMT-3) em segundos
const long utcOffsetInSeconds = -10800; 

// Intervalo de atualização do NTP (5 minutos em milissegundos)
const long updateInterval = 5 * 60 * 1000; 

// Cria objetos para comunicação UDP e NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, utcOffsetInSeconds, updateInterval);

// ------------------------------------
// 4. HORÁRIOS DE CONTROLE
// ------------------------------------
const int HORA_LIGAR   = 18; // 18:00
const int HORA_DESLIGAR = 6;  // 06:00

// ====================================
// FUNÇÃO SETUP
// ====================================
void setup() {
  Serial.begin(115200);
  Serial.println();

  // Configura os pinos dos relés como SAÍDA
  pinMode(RELE_PIN_1, OUTPUT);
  pinMode(RELE_PIN_2, OUTPUT);
  pinMode(RELE_PIN_3, OUTPUT);

  // Define o estado inicial: Relés DESLIGADOS
  digitalWrite(RELE_PIN_1, RELAY_OFF);
  digitalWrite(RELE_PIN_2, RELAY_OFF);
  digitalWrite(RELE_PIN_3, RELAY_OFF);

  // Conexão Wi-Fi
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ WiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicia o cliente NTP
  timeClient.begin();
  Serial.println("⏳ Cliente NTP iniciado.");
}

// ====================================
// FUNÇÃO LOOP
// ====================================
void loop() {
  // Tenta atualizar o tempo (só fará isso no intervalo definido)
  timeClient.update();

  // Obtém a hora atual do NTP
  int currentHour = timeClient.getHours();
  String formattedTime = timeClient.getFormattedTime();

  Serial.print("🕒 Horário Atual: ");
  Serial.println(formattedTime);

  // ------------------------------------
  // 5. LÓGICA DE CONTROLE
  // ------------------------------------
  
  // Condição para LIGAR: entre 18h00 e 05h59
  // (currentHour >= 18) OU (currentHour < 6)
  if (currentHour >= HORA_LIGAR || currentHour < HORA_DESLIGAR) {
    // Liga os Relés
    digitalWrite(RELE_PIN_1, RELAY_ON); 
    digitalWrite(RELE_PIN_2, RELAY_ON); 
    digitalWrite(RELE_PIN_3, RELAY_ON); 
    
    Serial.println("⚡ Relés LIGADOS (Noite/Madrugada)");

  } else {
    // Desliga os Relés
    digitalWrite(RELE_PIN_1, RELAY_OFF); 
    digitalWrite(RELE_PIN_2, RELAY_OFF); 
    digitalWrite(RELE_PIN_3, RELAY_OFF); 
    
    Serial.println("💤 Relés DESLIGADOS (Dia)");
  }
  
  // Aguarda 1 minuto (60000 ms) antes de checar a hora novamente
  delay(60000); 
}
