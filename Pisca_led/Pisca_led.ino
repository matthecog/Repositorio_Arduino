void setup()
{
  //Define a porta do Arduino como saida
  pinMode(12, OUTPUT);
}
   
void loop()
{
  //Acende led
  digitalWrite(12, HIGH);
     
  //Aguarda intervalo de tempo (milissegundos)
  delay(1000);
     
  //Apaga o led
  digitalWrite(12, LOW);
     
  //Aguarda intervalo de tempo (milissegundos)
  delay(1000);
}