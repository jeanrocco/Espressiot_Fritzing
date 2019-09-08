//
// ESPressIoT Controller for Espresso Machines
// 2017 by Roman Schmitz
//
// Simple PID tuning procedure
// based on the blog entry http://brettbeauregard.com/blog/2012/01/arduino-pid-autotune-library/
// 
//  Modified to allow detection of maximum and minimum temperature by jrb. (ref: https://github.com/Schm1tz1/ESPressIoT/issues/2)


double aTuneStep=100.0, aTuneThres=0.2;
double AvgUpperT=0, AvgLowerT=0;
//double MaxUpperT=0, MaxLowerT=0;
uint8_t sm1 = 2, sm2 = 2;
int UpperCnt=0, LowerCnt=0;
int tune_count=0;

unsigned long tune_time=0;
unsigned long tune_start=0;

void tuning_on() {
  tune_time=0;
  tune_start=0;
  tune_count=0;
  UpperCnt=0; LowerCnt=0;
  AvgUpperT=0; AvgLowerT=0;
  MaxUpperT=0; MaxLowerT=0;
  sm1=2;sm2=2;
  ESPPID.SetMode(MANUAL);
  tuning=true;
}

void tuning_off() {
  ESPPID.SetMode(AUTOMATIC);
  tuning=false;

  double dt = float(tune_time-tune_start)/tune_count;
  double dT = ((AvgUpperT/UpperCnt)-(AvgLowerT/LowerCnt));

  double Ku = 4*(2*aTuneStep)/(dT*3.14159);
  double Pu = dt / 1000; // units of seconds

  gP=0.6*Ku;
  gI=1.2*Ku/Pu;
  gD=0.075*Ku*Pu;
}

void tuning_loop() {
  //
  // count seconds between power-on-cycles
  //
  //
  if(gInputTemp<(gTargetTemp-aTuneThres)) {                             // below lower threshold -> power on
    if(gOutputPwr==0) {                                                 // just fell below threshold
      if(tune_count==0) tune_start=time_now;
      tune_time=time_now;
      tune_count++;
      MaxLowerT = gInputTemp;
//      Serial.print("MaxLowerT1 = "); Serial.println(MaxLowerT);
//      AvgLowerT = gInputTemp;
      sm2 = 2;                                                           // set reset ...  
    }
    if(MaxLowerT == 0) MaxLowerT = gInputTemp;                           // it happens
    if(gInputTemp < MaxLowerT) MaxLowerT = gInputTemp;                   // find the minimum lower temperature
//    Serial.print("MaxLowerT = "); Serial.println(MaxLowerT); 
//    Serial.print(MaxLowerT, 2); Serial.print(" ");
    gOutputPwr=aTuneStep;
    setHeatPowerPercentage(aTuneStep);
  }
  if(gInputTemp<(gTargetTemp+aTuneThres) && gInputTemp > gTargetTemp) {  // wait till the end of the ON power cycle to add MaxLowerT to AvgLowerT ...
    if(gOutputPwr==aTuneStep) {                                          // this makes sure we are indeed going upward...
      if(sm1 == 2) {                                                     // switch to add MaxLowerT only once...
        AvgLowerT += MaxLowerT;
        LowerCnt++;
        sm1 = 1;                                                         // close the switch.
//        Serial.print(MaxLowerT, 2); Serial.print(" ");
        MaxLowerT = gTargetTemp - aTuneThres;                            // reset the MaxlowerT to it's higher limit.
      }
    }
  }

  
  if(gInputTemp>(gTargetTemp+aTuneThres)) {                              // above upper threshold -> power off
    if(gOutputPwr==aTuneStep) {                                          // just crossed upper threshold
//      AvgUpperT += gInputTemp;
//      UpperCnt++;
      MaxUpperT = gInputTemp;
//      AvgUpperT = gInputTemp;
      sm1 = 2;
      
    }
    if(gInputTemp > MaxUpperT) MaxUpperT = gInputTemp;                    // find out the maximum upper temperature
//    Serial.print("  MaxUpperT = "); Serial.println(MaxUpperT);
//    Serial.print(MaxUpperT,2); Serial.print(" ");
    gOutputPwr=0;
    setHeatPowerPercentage(0);
  }
  if(gInputTemp>(gTargetTemp-aTuneThres) && gInputTemp < gTargetTemp) {   // wait till the end of the OFF power cycle to add MaxUpperT to AvgUpperT
    if(gOutputPwr== 0) {                                                  // are we are going downward ?     
      if(sm2 == 2) {                                                      // add MaxUpperT only once per cycle.
        AvgUpperT += MaxUpperT;
        UpperCnt++;
        sm2 = 1;                                                          // close the switch
//        Serial.print(MaxUpperT,2); Serial.print(" ");
        MaxUpperT = gTargetTemp + aTuneThres;                             // reset MaxUpperT to it's lower limit
      }
    }
  }    
}
