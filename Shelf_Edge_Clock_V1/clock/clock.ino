#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#ifdef __AVR__
#endif
//debug
#define debug_wifi true
#define debug_clock false
#define debug_ntp true
#define DEBUG_GLOBAL_TIME false
const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);

// Maintained state for reporting to HA
byte red = 255;
byte green = 255;
byte blue = 255;
byte brightness = 255;

// Real values to write to the LEDs (ex. including brightness and state)
byte realRed = 0;
byte realGreen = 0;
byte realBlue = 0;

bool stateOn = false;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000);
//
int global_hour = 0;
int global_min  = 0;

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN    D1
// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT 216


// Declare our NeoPixel objects:
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel downlight(12, D2, NEO_RGB + NEO_KHZ800);

int clockMinuteColour = downlight.Color(0,0,255);
int clockHourColour = downlight.Color(255,8,255);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(9600);

  stripClock.begin();           // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.show();            // Turn OFF all pixels ASAP
  stripClock.setBrightness(20); // Set inital BRIGHTNESS (max = 255)
  
  downlight.begin();
  downlight.setBrightness(255);

  WiFi.begin("Project14", "B@ssettwifi2018");
  client.setServer(CONFIG_MQTT_HOST, CONFIG_MQTT_PORT);
  client.setCallback(callback);
  if(debug_wifi)
    {
      Serial.print("Connecting");
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
      Serial.println();
      Serial.print("Connected, IP address: ");
      Serial.println(WiFi.localIP());
    }
  timeClient.begin();
  for(int i = 0;i < 4;i++)
  {
    delay(1000);
    readTheTime(); //updates NTP clock
  }
  
}

void loop() {
  
  readInternalTime();
  delay(100);//resets watchdog timer //slows down loop
  if(DEBUG_GLOBAL_TIME)
  {
    Serial.print(global_hour);
    Serial.print(":");
    Serial.print(global_min);
    Serial.println();
  }
  displayTheTime();  //setup clock //don't do this fast
  if (!client.connected()) {
    reconnect();
  }

  client.loop();


}


unsigned long previousMillis = 0;

void readInternalTime()
{
  
  static int seconds = timeClient.getSeconds();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000)
  {
    previousMillis = currentMillis;
    seconds++;
  
    if(seconds == 60)
    {
      seconds=0;
      global_min++;
    }
    if(global_min == 60)
    {
      global_min = 0;
      global_hour++;
      readTheTime();  
    }
    if(global_hour == 24)
    {
      global_hour = 0;
    }
  }
}


void readTheTime()
{
  if(!timeClient.update())
    {
      Serial.println("TIME CLIENT FAILURE");
    }
  global_hour = timeClient.getHours();
  global_min  = timeClient.getMinutes();
  
  if(debug_ntp)
    {
      Serial.print("NTP Server Time: ");
      Serial.print(timeClient.getHours());
      Serial.print(":"); 
      Serial.print(timeClient.getMinutes());
      Serial.println();
    }
}

  
void displayTheTime()
  {
    stripClock.clear(); //clear the clock face 

    //minutes
    int firstMinuteDigit = global_min % 10; //work out the value of the first digit and then display it
    displayNumber(firstMinuteDigit, 0, clockMinuteColour);
   
    int secondMinuteDigit = floor(global_min / 10); //work out the value for the second digit and then display it
    displayNumber(secondMinuteDigit, 63, clockMinuteColour);  

    //adjust 24 hour to hour
    int hour = global_hour;
    if(global_hour > 12)
    {
     hour = global_hour-12;
    }
//    Serial.print(global_hour);
//    Serial.print("->");
//    Serial.print(hour);
//    Serial.println();
    //hours
    int firstHourDigit = hour % 10; //work out the value of the first digit and then display it
    displayNumber(firstHourDigit, 126, clockHourColour);
   
    int secondHourDigit = floor(hour / 10); //work out the value for the second digit and then display it
    if(secondHourDigit != 0)
    {
      if(secondHourDigit <= 1)
      {
        secondHourDigit = 3;
        displayNumber(secondHourDigit, 189, clockHourColour);
      } 
    }

if(debug_clock)
  {
      Serial.print(secondHourDigit);
      Serial.print(firstHourDigit);
      Serial.print(":");
      Serial.print(secondMinuteDigit);
      Serial.print(firstMinuteDigit);
      Serial.println();
  }    
    stripClock.show();
  }




  //get colors from mqtt
  void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if (!processJson(message)) {
    return;
  }

  if (stateOn) {
    // Update lights
    realRed = map(red, 0, 255, 0, brightness);
    realGreen = map(green, 0, 255, 0, brightness);
    realBlue = map(blue, 0, 255, 0, brightness);
  }
  else {
    realRed = 0;
    realGreen = 0;
    realBlue = 0;
  }
  sendState(); //send confirmation back to HA
  updateColors(); //update neopixels
}

bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return false;
  }

  if (root.containsKey("state")) {
    if (strcmp(root["state"], CONFIG_MQTT_PAYLOAD_ON) == 0) {
      stateOn = true;
    }
    else if (strcmp(root["state"], CONFIG_MQTT_PAYLOAD_OFF) == 0) {
      stateOn = false;
    }
  }
    if (root.containsKey("color")) {
      red = root["color"]["r"];
      green = root["color"]["g"];
      blue = root["color"]["b"];
    }
    if (root.containsKey("brightness")) {
      brightness = root["brightness"];
    }   
    return true;
}

void sendState() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["state"] = (stateOn) ? CONFIG_MQTT_PAYLOAD_ON : CONFIG_MQTT_PAYLOAD_OFF;
  //root["state"] = "OFF";
  JsonObject& color = root.createNestedObject("color");
  color["r"] = red;
  color["g"] = green;
  color["b"] = blue;

  root["brightness"] = brightness;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));
  client.publish(CONFIG_MQTT_TOPIC_STATE, buffer, true);
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CONFIG_MQTT_CLIENT_ID, CONFIG_MQTT_USER, CONFIG_MQTT_PASS)) {
      Serial.println("connected");
      client.subscribe(CONFIG_MQTT_TOPIC_SET);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void updateColors()
{
  downlight.fill(downlight.Color(realGreen,realRed,realBlue), 0, 12);
  downlight.show();  
}
