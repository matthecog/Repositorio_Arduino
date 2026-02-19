#define pin1 5
int LED = 8;

void setup(){
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(pin1, OUTPUT);
}

void loop(){
  int LDR = analogRead(A0);
  Serial.println(LDR);

  if(LDR<15){
    digitalWrite(LED,HIGH);
    digitalWrite(pin1,LOW);
  }
  else{
    digitalWrite(LED,LOW);
    digitalWrite(pin1,HIGH);
    }

  delay(500);
}