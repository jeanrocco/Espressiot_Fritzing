//
// ESPressIoT Controller for Espresso Machines
// 2017 by Roman Schmitz
//
// Simple PID tuning procedure
// based on the blog entry http://brettbeauregard.com/blog/2012/01/arduino-pid-autotune-library/
//

double aTuneStep=100.0, aTuneThres=0.2;
double AvgUpperT=0, AvgLowerT=0;
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
      AvgLowerT += gInputTemp;
      LowerCnt++;
    }
    gOutputPwr=aTuneStep;
    setHeatPowerPercentage(aTuneStep);
  }
  else if(gInputTemp>(gTargetTemp+aTuneThres)) { // above upper threshold -> power off
    if(gOutputPwr==aTuneStep) { // just crossed upper threshold
      AvgUpperT += gInputTemp;
      UpperCnt++;
    }
    gOutputPwr=0;
    setHeatPowerPercentage(0);
  }  
}
