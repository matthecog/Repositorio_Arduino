//sensor umidade/temperatura
#include <dht.h>
#define pinSensorUT 24

dht sensorDHT;

//Display_LED
byte seven_seg_digits[20][7] = {
                                 { 1,1,1,1,1,1,0 },  // = Digito 0
                                 { 0,1,1,0,0,0,0 },  // = Digito 1
                                 { 1,1,0,1,1,0,1 },  // = Digito 2
                                 { 1,1,1,1,0,0,1 },  // = Digito 3
                                 { 0,1,1,0,0,1,1 },  // = Digito 4
                                 { 1,0,1,1,0,1,1 },  // = Digito 5
                                 { 1,0,1,1,1,1,1 },  // = Digito 6
                                 { 1,1,1,0,0,0,0 },  // = Digito 7
                                 { 1,1,1,1,1,1,1 },  // = Digito 8
                                 { 1,1,1,0,0,1,1 },  // = Digito 9
                                 { 1,1,1,0,1,1,1 },  // = Digito A
                                 { 0,0,1,1,1,1,1 },  // = Digito B
                                 { 1,0,0,1,1,1,0 },  // = Digito C
                                 { 0,1,1,1,1,0,1 },  // = Digito D
                                 { 1,0,0,1,1,1,1 },  // = Digito E
                                 { 1,0,0,0,1,1,1 },  // = Digito F
                                 { 1,0,1,1,1,1,1 },  // = Digito G
                                 { 0,1,1,0,1,1,1 },  // = Digito H
                                 { 0,0,0,0,1,1,0 },  // = Digito I
                                 { 0,1,1,1,1,0,0 },  // = Digito J
};

void setup () {
  //sensor umidade/temperatura
  Serial.begin(9600);
  
  //Display_LED
  pinMode(30, OUTPUT); //Pino 2 do Arduino ligado ao segmento A  
  pinMode(31, OUTPUT); //Pino 3 do Arduino ligado ao segmento B
  pinMode(32, OUTPUT); //Pino 4 do Arduino ligado ao segmento C
  pinMode(33, OUTPUT); //Pino 5 do Arduino ligado ao segmento D
  pinMode(34, OUTPUT); //Pino 6 do Arduino ligado ao segmento E
  pinMode(35, OUTPUT); //Pino 7 do Arduino ligado ao segmento F
  pinMode(36, OUTPUT); //Pino 8 do Arduino ligado ao segmento G
  pinMode(37, OUTPUT); //Pino 9 do Arduino ligado ao segmento PONTO
  writePonto(0);  // Inicia com o ponto desligado
}

void writePonto(byte dot) { //Funcao que aciona o ponto no display  
  digitalWrite(37, dot);
}

void sevenSegWrite(byte digit) {  //Funcao que aciona o display
  byte pin = 30;
  for (byte segCount = 0; segCount < 7; ++segCount) { //Percorre o array ligando os segmentos correspondentes ao digito 
    digitalWrite(pin, seven_seg_digits[digit][segCount]);
    ++pin;
  }
  writePonto(1);  //Liga o ponto
  delay(2000);   //Aguarda 2000 milisegundos
  writePonto(0);  //Desliga o ponto
}

void loop (){
  //sensor umidade/temperatura
  sensorDHT.read (pinSensorUT);
  Serial.print("Umidade: ");
  Serial.print(sensorDHT.humidity);
  Serial.print("% ");
  Serial.print(" Temperatura: ");
  Serial.print(sensorDHT.temperature);
  Serial.println("C");
  delay(1000);

  //Display_LED
  for (byte count = 0; count < 20; count++) {
     delay(500);
     sevenSegWrite(count);
  }
  delay(5000);
}
