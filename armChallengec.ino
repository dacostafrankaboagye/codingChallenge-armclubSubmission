// for remote control - additional
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "htmltext.h"

const char* ssid = "Laye";
const char* password = "kupp3708";

WiFiMulti wifiMulti;

WebServer server(80);




// for dht
#include "DHT.h"
#define DHTPIN 23
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE );
float h,t,f;



// for relay
int relayState = LOW ;
const int relay = 26;



// for servo
#define LED 2  // to replace the servo
const int ledState = 0;


// for ldr
const int ldrPin = 34;
int analogValue = 0;


// for water level
#define POWER_PIN 17
#define SIGNAL_PIN 36

int value = 0;

int activator = 0;




void handleRoot(){
server.send(200, "text/plain", "arm challenge");
}

void handleActuator(){
server.send(200, "text/html", page);
}


void setup(){

Serial.begin(115200);
dht.begin();

pinMode(LED, OUTPUT);

pinMode(POWER_PIN, OUTPUT);
digitalWrite(POWER_PIN, LOW);

pinMode(relay, OUTPUT);

pinMode(ldrPin, INPUT);

WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);

while(WiFi.status() != WL_CONNECTED){
delay(500);
Serial.print(".");
}

Serial.print("connected to .."); 
Serial.print(WiFi.localIP());

server.on("/", handleRoot);
server.on("/controlActuator", handleActuator);

server.begin();

}


void dhtOperation(){
delay(10);
h = dht.readHumidity(); // humidity
t = dht.readTemperature(); // temperature in degree celcius
f = dht.readTemperature(true); // temperature in degree Fahrenheit

Serial.print("Humidity: "); Serial.print(h);
Serial.println("");
Serial.print("Temperature: "); Serial.print(t);
Serial.println("");
Serial.print("Temperature: "); Serial.print(f);

}

void waterLevelOperation(){
digitalWrite(POWER_PIN, HIGH);
delay(10);
value = analogRead(SIGNAL_PIN);
digitalWrite(POWER_PIN, LOW);
Serial.print("Water Level is "); Serial.print(value);


}

void ldrOperation(){
analogValue  = analogRead(ldrPin);
Serial.print("ldr reading  = "); Serial.print(analogValue); Serial.println("");
}

void loop(){

digitalWrite(relay, relayState); // initially relayState is LOW

dhtOperation();
waterLevelOperation();
ldrOperation();

// if water level is below a level turn on the pump (level assumed to be 0)
if(value != 0 && activator == 1){
relayState = HIGH;
Serial.print("pump is on"); Serial.println("");
}else{
relayState = LOW;
Serial.print("pump is off"); Serial.println("");
}

// temperature to determine when to activate the pump (temperature level assumed to be 30)
if(t > 30){
activator = 1;
Serial.print("the temperature is high. You can now turn on/off the pump"); Serial.println("");
}else{
activator = 0;
Serial.print("the temperature is low. You cannot control pump"); Serial.println("");
}

// ldr to shine light on crops (ldr value (analog) level assumed)
if(analogValue < 40){
Serial.println(" => Dark --- shining more light");
}else if(analogValue < 800){
Serial.println(" => Dim --- shining light");
}else if(analogValue < 2000){
Serial.println(" => enough sunlight");
}else{
Serial.println(" => just enough sunlight");
}


// activate fan(LED in this case) when temperature is too low (level assumed to be 30)
if(t < 30 and ledState==1){
digitalWrite(LED, HIGH);
}else{
digitalWrite(LED, LOW);
}








}
