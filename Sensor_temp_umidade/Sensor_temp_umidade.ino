#include <dht.h>

#define pinSensor 24
//#define intervalo 5000

unsigned long delayIntervalo;

dht sensorDHT;


void setup () {

  Serial.begin(9600);

}

void loop (){

  sensorDHT.read (pinSensor);

  Serial.print("Umidade: ");
  Serial.print(sensorDHT.humidity);
  Serial.print("% ");

  Serial.print(" Temperatura: ");
  Serial.print(sensorDHT.temperature);
  Serial.println("C");
  
  delay(50000);
  
}