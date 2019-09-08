//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// TSIC-Sensors using Arduino-TSIC-Library
//

#ifndef SIMULATION_MODE

//#include <TSIC.h>
#include <Adafruit_MAX31865.h>


//Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 maxJR = Adafruit_MAX31865(16, 14, 12, 13);
 
//#define TSIC_SIG 4 // D4 pin of NodeMCU just in the right position


#define RREF       4300.0      // resistance de reference
#define RNOMINAL   1000.0      // rtd a 0 C.

int SMP_TIME1 = 10;   // pour le bias enable...
int SMP_TIME2 = 65;   // pour le one shot, qui demarre la conversion
uint16_t cnt1 = 0; uint16_t cnt2 = 0;
int nread = 8;        // nombre de valeurs analogiques pour la moyenne...
boolean biasEnab = false;
boolean oneShot = false;

float readings[8];
uint16_t idx = 0;
float total = 22.0 * nread;
float moy = 22.0;

 
//#define ACCURACY 0.1

//TSIC TSens1(TSIC_SIG);

float lastT = 0.0;
float SumT = 0.0;
float lastErr = 0.0;
int CntT = 0;
uint16_t raw_temp = 0;
unsigned long lastSensTime;
uint8_t sm = 1;                 // demarre avec l'etat 1...
uint16_t rtd = 1000;

void setupSensor() {  
  maxJR.begin(MAX31865_4WIRE);  // set to 2WIRE or 4WIRE as necessary
  for ( int lect = 0; lect < nread; lect++ ) {  // initialize 8 readings to 22.0 degres...
    readings[lect] = 22.0;
  }
  total = 22.0 * nread;
//  lastSensTime=millis();
  sm = 1;                       // etat 1...
}

void updateTempSensor() {
  
  switch (sm) {

    case 1:
      
      maxJR.clearFault();
      maxJR.enableBias(true);
      lastSensTime = millis();
      cnt1 = 0; cnt2 = 0;
      sm = 2;
      break;
      
    case 2:

      cnt1++;
      if (abs(millis() - lastSensTime) >= SMP_TIME1) {
        
        uint8_t registre = maxJR.readRegister8(0);  // lire le registre de configuration adr 0
        registre |= 0x20;                           // le bit pour oneshot
        maxJR.writeRegister8(0, registre);          // et demarre la conversion
        lastSensTime = millis();
        sm = 3;
        
      }
     break;
      
    case 3:

       cnt2++;
       if (abs(millis() - lastSensTime) >= SMP_TIME2) {

        rtd = maxJR.readRegister16(0x01);
        rtd >>= 1;                                  // enleve le bit fault...
        float curT = maxJR.temperature(RNOMINAL, RREF, rtd);
      
/*      // very simple selection of noise hits/invalid values 
        if(abs(curT-lastT)<1.0 || lastT<1) {
        SumT+=curT;
        CntT++;        
        } 
*/
        total = total - readings[idx];    // enleve la derniere valeur...
        readings[idx] = curT;
        total = total + readings[idx];
        idx++;
        if (idx >= nread) {
          idx = 0;
        }

        
        sm = 1; 
        
//        Serial.print("cnt1 = "); Serial.print(cnt1); Serial.print("  cnt2 = "); Serial.println(cnt2);
//        J'ai ~ 70 usec de loop ... cnt1= 144 pour 10 msec et cnt2= 929 pour 65 msec... ca clanche en ta !
       }

       
      break;
  }
  
  
  
/*  if (abs(millis() - lastSensTime) >= TSIC_SMP_TIME) {
    if(TSens1.getTemperature(&raw_temp)) {
      float curT = TSens1.calc_Celsius(&raw_temp);

      // very simple selection of noise hits/invalid values 
      if(abs(curT-lastT)<1.0 || lastT<1) {
        SumT+=curT;
        CntT++;        
      }     
      lastSensTime=millis();
    }
  } */ 
}

float getTemp() {
  float retVal=gInputTemp;

/*    if(CntT>=1) {
    retVal=(SumT/CntT);
    SumT=0.;
    CntT=0;
  }
//  retVal = maxJR.temperature(RNOMINAL, RREF, rtd);
*/
  retVal = total/nread;                   // divise par 8

  return retVal;
}

#endif
