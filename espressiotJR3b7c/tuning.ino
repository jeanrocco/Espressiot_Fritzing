//
// ESPressIoT Controller for Espresso Machines
// 2017 by Roman Schmitz
//
// Simple PID tuning procedure
// based on the blog entry http://brettbeauregard.com/blog/2012/01/arduino-pid-autotune-library/
//

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
  if(gInputTemp<(gTargetTemp-aTuneThres)) { // below lower threshold -> power on
    if(gOutputPwr==0) { // just fell below threshold
      if(tune_count==0) tune_start=time_now;
      tune_time=time_now;
      tune_count++;
      MaxLowerT = gInputTemp;
//      Serial.print("MaxLowerT1 = "); Serial.println(MaxLowerT);
//      AvgLowerT = gInputTemp;
      sm2 = 2;   
//      AvgLowerT += gInputTemp;
//      LowerCnt++;
    }
    if(MaxLowerT == 0) MaxLowerT = gInputTemp;
    if(gInputTemp < MaxLowerT) MaxLowerT = gInputTemp;   
//    Serial.print("MaxLowerT = "); Serial.println(MaxLowerT); 
//    Serial.print(MaxLowerT, 2); Serial.print(" ");
    gOutputPwr=aTuneStep;
    setHeatPowerPercentage(aTuneStep);
  }
  if(gInputTemp<(gTargetTemp+aTuneThres) && gInputTemp > gTargetTemp) {
    if(gOutputPwr==aTuneStep) {
      if(sm1 == 2) {
        AvgLowerT += MaxLowerT;
        LowerCnt++;
        sm1 = 1;
//        Serial.print(MaxLowerT, 2); Serial.print(" ");
        MaxLowerT = gTargetTemp - aTuneThres;
      }
    }
  }
/*  switch(sm1) {

    case 1:

      AvgLowerT += MaxLowerT;
      LowerCnt++;
      sm1 = 3;
      break;

    case 2:

      sm1 = 3;
      break;

    case 3:

      break;
  }
*/


  
  if(gInputTemp>(gTargetTemp+aTuneThres)) { // above upper threshold -> power off
    if(gOutputPwr==aTuneStep) { // just crossed upper threshold
//      AvgUpperT += gInputTemp;
//      UpperCnt++;
      MaxUpperT = gInputTemp;
//      AvgUpperT = gInputTemp;
      sm1 = 2;
      
    }
    if(gInputTemp > MaxUpperT) MaxUpperT = gInputTemp;
//    Serial.print("  MaxUpperT = "); Serial.println(MaxUpperT);
//    Serial.print(MaxUpperT,2); Serial.print(" ");
    gOutputPwr=0;
    setHeatPowerPercentage(0);
  }
  if(gInputTemp>(gTargetTemp-aTuneThres) && gInputTemp < gTargetTemp) {
    if(gOutputPwr== 0) {
      if(sm2 == 2) {
        AvgUpperT += MaxUpperT;
        UpperCnt++;
        sm2 = 1;
//        Serial.print(MaxUpperT,2); Serial.print(" ");
        MaxUpperT = gTargetTemp + aTuneThres;
      }
    }
  }  
  
/*  switch(sm2) {

      case 1:

        AvgUpperT += MaxUpperT;
        UpperCnt++;
        sm2 = 2;
        break;

      case 2:

        break;
  }
*/  
}
