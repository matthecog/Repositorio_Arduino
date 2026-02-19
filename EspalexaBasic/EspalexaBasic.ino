 nb/*
 * This is a basic example on how to use Espalexa and its device declaration methods.
 */ 
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);
//void secondLightChanged(uint8_t brightness);
//void thirdLightChanged(uint8_t brightness);

// Change this!!
const char* ssid = "cordeiro";
const char* password = "Rom@8585";

boolean wifiConnected = false;

Espalexa espalexa;

EspalexaDevice* device1; //this is optional

void setup()
{
  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    
    // Define your devices here. 
   
    
   // espalexa.addDevice("luz da entrada", firstLightChanged); //simplest definition, default state off
   // espalexa.addDevice("Light 2", secondLightChanged, 255); //third parameter is beginning state (here fully on)
    
    device1 = new EspalexaDevice("Luz da entrada", firstLightChanged); //you can also create the Device objects yourself like here
    espalexa.addDevice(device1); //and then add them
    device1->setValue(128); //this allows you to e.g. update their state value at any time!

    espalexa.begin();
    
  } else
  {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}
 
void loop()
{
   espalexa.loop();
   delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
    Serial.print("L<uz da entrada alterado para ");
    
    //do what you need to do here

    //EXAMPLE
    if (brightness) {
      Serial.print("ON, brightness ");
      Serial.println(brightness);
      digitalWrite(0,HIGH);
    }
    else  {
      Serial.println("OFF");
      digitalWrite(0,LOW);
    }
}

//void secondLightChanged(uint8_t brightness) {
  //do what you need to do here
//}

//void thirdLightChanged(uint8_t brightness) {
  //do what you need to do here
//}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state){
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}
