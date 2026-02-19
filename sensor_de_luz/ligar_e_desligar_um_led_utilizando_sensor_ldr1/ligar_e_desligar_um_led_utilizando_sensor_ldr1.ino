/*
----  LIGAR E DESLIGAR LED UTILIZANDO LDR    ----
=================================================
== BLOG DA ROBOTICA - www.blogdarobotica.com ==
=================================================
Autor: Carol Correia Viana
E-mail: contato@blogdarobotica.com
Facebook: facebook.com/blogdarobotica
Instagram:@blogdarobotica
YouTube: youtube.com/user/blogdarobotica
=================================================
== CASA DA ROBOTICA - www.casadarobotica.com ==
=================================================
Facebook: facebook.com/casadaroboticaoficial
Instagram:@casadarobotica
=================================================
Tutorial:
www.blogdarobotica.com/controle-de-luz-com-sensor-ldr-e-arduino/

*/

int led = 10; //Atribui o valor 10 a variável led
int ldr = A0; //Atribui A0 a variável ldr
int valorldr = 0;//Declara a variável valorldr como inteiro

void setup() {
  pinMode (led, OUTPUT); //Define led (pino digital 10) como saída  
  pinMode (ldr, INPUT); //Define ldr (pino analógico A0) como entrada  
  Serial.begin(9600); //Inicialização da comunicação serial, com velocidade de comunicação de 9600
}

void loop() {
   valorldr = analogRead (ldr);//Lê o valor do sensor ldr e armazena na variável valorldr
   Serial.println(valorldr);//Imprime na serial os dados de valorldr
   
   if((valorldr) < 1){ //Se o valor de valorldr for menor que 500:
    digitalWrite(led, HIGH);//Coloca led em alto para acioná-lo
  }  
  else{ //Senão: 
    digitalWrite(led, LOW);//Coloca led em baixo para que o mesmo desligue ou permaneça desligado
  }  
}
