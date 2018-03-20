/*
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include<ThingSpeak.h>
#include <DallasTemperature.h>

#define myPeriodic 15
#define ONE_WIRE_BUS D4
#define TEMPERATURE_PRECISION 11

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long myChannelNumber = 445513;            // Thingspeak channel ID here
const char* host = "api.thingspeak.com"; 
const char* myWriteAPIKey = "FZHX32ACBNVQD6Z4";
const char* ssid = "WIFIID";
const char* pass = "WIFIPASSWORD";

int fieldStart = 1;  

int updatePeriod = 60; 

int status = WL_IDLE_STATUS;
WiFiClient  client;

int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(20);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
  ThingSpeak.begin(client);
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  for(int i=0;i<numberOfDevices; i++)
  {
    if(sensors.getAddress(tempDeviceAddress, i))
    {
      Serial.print("Found device "); 
      Serial.print(i, DEC); 
      Serial.print(" with address: "); 
      printAddress(tempDeviceAddress); 
      Serial.println(); 
       
      Serial.print("Setting resolution to "); 
      Serial.println(TEMPERATURE_PRECISION, DEC); 
       
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions) 
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION); 
      
      Serial.print("Resolution actually set to: "); 
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);  
      Serial.println(); 
     }
     else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
     }
  }
}
void printAddress(DeviceAddress deviceAddress){
    for (uint8_t i = 0; i < 8; i++){
     if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}
void loop() {
 float temp;
 sensors.requestTemperatures();
 temp = sensors.getTempCByIndex(0);
 for (int i=0; i<=(numberOfDevices - 1); i++){
    float temp=sensors.getTempCByIndex(i);
    ThingSpeak.setField(i+fieldStart,temp);        
    Serial.println("Sensor #:");
    Serial.println(i);
    Serial.println("Temperature:");
    Serial.println(temp);      
 }
 ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
 Serial.println("Data sent to ThinkSpeak");
 delay(updatePeriod * 1000);
}
*/
//thinkspeak를 이용한 데이터 읽어오기
#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
//#include <ESP8266WiFiMulti.h>
#define WIFISID "WIFIID"
#define WIFIPASS "WIFIPASSWORD"
// Definimos los parámetros para el IFTTT
#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "temperature"
#define IFTTTKEY "cfgHollfHSRLHor_ETinLp"
#define ONE_WIRE_BUS D4
#define TEMPERATURE_PRECISION 11

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//ESP8266WiFiMulti WiFiMulti;
WiFiClientSecure client;
bool ejecutado = false;
void setup() {
Serial.begin(115200);
delay(10);
WiFi.begin(WIFISID, WIFIPASS);
Serial.println("");
Serial.print("Eperando a conectar a la WiFi... ");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFISID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

//randomSeed(analogRead(A0));
}
void loop() {
  if (!ejecutado)
  {
    // Obtenemos los valores
     float temp;
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    float valor1 = temp;
    Serial.println(valor1);
    enviar_tweet(valor1,0,0);
    //ejecutado = true;  
  }
  delay(3000);
}

void enviar_tweet(float valor1,float valor2,float valor3)
{
  if (client.connected())
  {
    client.stop();
  }

  client.flush();

  if (client.connect(HOSTIFTTT,443)) {
    Serial.println("Connected");
    // Construimos la petición HTTP
    String toSend = "GET /trigger/";
    toSend += EVENTO;
    toSend += "/with/key/";
    toSend += IFTTTKEY;
    toSend += "?value1=";
    toSend += valor1;
   // toSend += "&value2=";
    //toSend += valor2;
   // toSend += "&value3=";
    //toSend += valor3;
    toSend += " HTTP/1.1\r\n";
    toSend += "Host: ";
    toSend += HOSTIFTTT;
    toSend += "\r\n";
    toSend += "Connection: close\r\n\r\n";

    client.print(toSend);
  }

  client.flush();
  client.stop();
}
