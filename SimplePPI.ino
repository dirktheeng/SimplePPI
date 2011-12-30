int pulseMS = 5;
float PPI = 400;
float ppiX = 157.4744*25.4; // conversion of pulses in x to inches
float ppiY = 157.76525*25.4; // conversion of pulses in y to inches
int XDir = 0;
int YDir = 0;
int XStep = 0;
int YStep = 0;
int XStepOld = 1;
int YStepOld = 1;
int LsrCmd = 0;
int LsrCmdPrev = 0;
int LsrSet = 0;
int FirstOnState = 0;
int XCnt = 0;
int YCnt = 0;
int XCntPrev = 0;
int YCntPrev = 0;
int NSteps = 0;
int NStepsPrev = 0;
int mStopCnt = 0;
int mStopPrev = 0;
unsigned long timeOld = 0;
float cumDist = 0;
int pulse = 0;

//define the pins to make it more human freindly
#define XStepPin 8
#define XDirPin 9
#define YStepPin 10
#define YDirPin 11
#define LsrCmdPin 12
#define PPIOnOffPin 6
#define LsrSetPin 7
// Do not set LsrCmdPin or LsrSetPin to 13... that pin is tied to an
// led that flashes during writing cycles and will flash your laser
// when you don't want it to.

void setup() {
  // set pin modes
  pinMode(XStepPin, INPUT);
  pinMode(XDirPin, INPUT);
  pinMode(YStepPin, INPUT);
  pinMode(YDirPin, INPUT);
  pinMode(LsrCmdPin, INPUT);
  pinMode(PPIOnOffPin, INPUT);
  pinMode(LsrSetPin, OUTPUT);

  digitalWrite(LsrCmdPin, HIGH);
  digitalWrite(XStepPin,LOW);
  // mase sure laser is off at startup
  digitalWrite(LsrSetPin, LOW);
}

void loop() {
  if (digitalRead(PPIOnOffPin)){
    ReadPins();
    UpdateLsrCmd();
    UpdateCnts();
    if (LsrCmd){
      if (CheckForMotion()){
        CalcTravel();
        if(FirstOnState){
          cumDist = 0;
          timeOld = millis();
          pulse = 1;
          FirstOnState = 0;
        }
        if (millis() - timeOld >= pulseMS) {pulse = 0;}
        if (cumDist >= 1/PPI) {
          cumDist = 0;
          timeOld = millis();
          pulse = 1;
        }
        if (pulse) {digitalWrite(LsrSetPin,HIGH);} else {digitalWrite(LsrSetPin,LOW);}
      }else{
        digitalWrite(LsrSetPin,LOW);
      }
    }else{
      digitalWrite(LsrSetPin,LOW);
      //have to reset the counts here or after first time run, laser flashes once b/4 moving
      XCnt = 0;
      YCnt = 0;
    }
  }else{
    digitalWrite(LsrSetPin, digitalRead(LsrCmdPin));
  }
}

void ReadPins() {
  XStepOld = XStep;
  YStepOld = YStep;
  XStep = digitalRead(XStepPin);
  YStep = digitalRead(YStepPin);
  XDir = digitalRead(XDirPin);
  YDir = digitalRead(YDirPin);
  LsrCmd = digitalRead(LsrCmdPin);
}

void UpdateLsrCmd() {
  if ((LsrCmdPrev == 0) && (LsrCmd == 1)) {FirstOnState = 1;}
  LsrCmdPrev = LsrCmd;
}

void UpdateCnts() {
  if ((XStepOld == 0) && (XStep == 1)) {
    XCntPrev = XCnt;
    XCnt += XDir*2-1;
    NStepsPrev = NSteps;
    NSteps ++;
  }
  if ((YStepOld == 0) && (YStep == 1)) {
    XCntPrev = XCnt;
    YCnt += YDir*2-1;
    NStepsPrev = NSteps;
    NSteps ++;  
  }
}

int CheckForMotion() {
  if ((XCntPrev == XCnt) && (YCntPrev == YCnt)) {
    mStopCnt ++;
    if (mStopCnt >= 50){mStopPrev = 0;}
    return mStopPrev;
  }else{
    mStopCnt = 0;
    mStopPrev = 1;
    return 1;
  }
}

void CalcTravel() {
  if (NSteps >= 4){
    cumDist += sqrt(pow(XCnt/ppiX,2) + pow(YCnt/ppiY,2));
    XCnt = 0;
    YCnt = 0;
  }
}
  
