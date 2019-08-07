//
// esp8266 drive un display Oled et recoit ses donnees du broker mqtt
//
//

#include <ESP8266WiFi.h>

// WIFI

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "Your_password"

// options for special modules
#define ENABLE_JSON
#define ENABLE_HTTP
#define ENABLE_MQTT

// use simulation or real heater and sensors
//#define SIMULATION_MODE

//
// STANDARD reset values based on Gaggia CC
//
#define S_P 115.0
#define S_I 4.0
#define S_D 850.0
#define S_aP 100.0
#define S_aI 0.0
#define S_aD 0.0
#define S_TSET 94.0
#define S_TBAND 1.5

//
// Intervals for I/O
//
#define HEATER_INTERVAL 1000
#define DISPLAY_INTERVAL 1000
#define PID_INTERVAL 200



// global variables
//
double gTargetTemp=S_TSET;
double gOvershoot=S_TBAND;
double gInputTemp=20.0;
double gOutputPwr=0.0;
double gP = S_P, gI = S_I, gD = S_D;
double gaP = S_aP, gaI = S_aI, gaD = S_aD;

unsigned long time_now=0;
unsigned long time_last=0;

boolean tuning = false;
boolean osmode = false;
boolean poweroffMode = false;



void setup()
{
  Serial.begin(115200);
  Serial.println("Settin up WIFI...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("");
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  #ifdef ENABLE_HTTP
  setupWebSrv();
  #endif
  
  #ifdef ENABLE_MQTT
  setupMQTT();
  #endif

  
  // setup components
//  setupHeater();
//  setupSensor();
  setupOled();

  
}

void serialStatus() {
  Serial.print(gInputTemp, 2); Serial.print(" ");
  Serial.print(gTargetTemp, 2); Serial.print(" ");
  Serial.print(gOutputPwr, 2); Serial.print(" ");
  Serial.print(gP, 2); Serial.print(" ");
  Serial.print(gI, 2); Serial.print(" ");
  Serial.print(gD, 2); Serial.print(" ");
//  Serial.print(ESPPID.GetKp(), 2); Serial.print(" ");
//  Serial.print(ESPPID.GetKi(), 2); Serial.print(" ");
//  Serial.print(ESPPID.GetKd(), 2);
  Serial.println("");
}



void loop() {

  #ifdef ENABLE_MQTT
  loopMQTT();
  #endif
    
//  serialStatus();

  updateOled();
  #ifdef ENABLE_HTTP
  loopWebSrv();
  #endif 

}
