#include "EEPROMAnything.h"
#include "SerialCommand.h"

#define SerialGate Serial1
#define Baudrate 9600
#define leftForward 5
#define leftBackward 6
#define rightForward 9
#define rightBackward 10


SerialCommand sCmd1(SerialGate);

bool debug;
bool running = LOW;
unsigned long previous;
unsigned long calculationTime;
const int sensor[] = {A0, A1, A2, A3, A4, A5 };
int ledPowerOn = 3;
int buttonStartStop = 2;
float iTerm;
float lastErr;



struct param_t{
  unsigned long cycleTime;
  float Kp;
  float ki;
  float kd;
  float diff;
  int power; 
  int black[6];
  int white[6];
  float position;
} params;

 int normalised[6];
 
void setup()
{
  SerialGate.begin(Baudrate);

  sCmd1.addCommand("SET", onSet);
  sCmd1.addCommand("DEBUG", onDebug);
  sCmd1.addCommand("CALIBRATE", onCalibrate);
  sCmd1.addCommand("START", onStart);
  sCmd1.addCommand("STOP", onStop);
  sCmd1.addCommand("VALUES", onValues);
  sCmd1.setDefaultHandler(onUnknownCommand);

  EEPROM_readAnything(0, params);

  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(ledPowerOn, OUTPUT);
  pinMode(buttonStartStop, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonStartStop), interrupt, RISING);
}


void loop()
{
  sCmd1.readSerial(); 
  digitalWrite(ledPowerOn, running);
  
  unsigned long current = micros();
  if (current - previous >= params.cycleTime)
  {
    previous = current;
   
    
    for (int i = 0; i < 6; i ++)
    {
      long value = analogRead(sensor[i]);
      normalised[i] = map(value, params.black[i], params.white[i], 0, 1000);
    }

    int index = 0;
    
    for (int i = 1; i < 6; i++) if (normalised[i] < normalised[index]) index = i;
    
    if(normalised[index] > 700) running = false;
    
    
    if (index == 0) params.position = -30;
    else if (index == 5) params.position = 30;
    else{
    int sZero = normalised[index];
    int sMinusOne = normalised[index-1];
    int sPlusOne = normalised[index+1];

    float b = sPlusOne - sMinusOne;
    b = b / 2;
    float a = sPlusOne - b - sZero;

    params.position = -b / (2 * a);  
    params.position += index; 
    params.position -= 2.5;   
    params.position *= 15;
    }
    
    float error  = -params.position;
    float output = error * params.Kp;

    iTerm += params.ki * error;
    iTerm = constrain(iTerm, -510, 510);
    output += iTerm;

    output += params.kd * (error - lastErr);
    lastErr = error;

    output = constrain(output, -510, 510);

    int powerLeft = 0;
    int powerRight = 0;
    
  if(running)  if (output <= 0){
  powerLeft = constrain(params.power + params.diff * output, -255, 255);
  powerRight = constrain(powerLeft - output, -255, 255);
  powerLeft = powerRight + output;
  }
  else{
  powerRight = constrain(params.power - params.diff * output, -255, 255);
  powerLeft = constrain(powerRight + output, -255, 255);
  powerRight = powerLeft - output;
  }
    
  analogWrite(leftForward, powerLeft > 0 ? powerLeft : 0);
  analogWrite(leftBackward, powerLeft < 0 ? -powerLeft : 0);
  analogWrite(rightForward, powerRight > 0 ? powerRight : 0);
  analogWrite(rightBackward, powerRight < 0 ? -powerRight : 0);
    
     //EEPROM_writeAnything(0, params);
  }
  unsigned long difference = micros() - current;
  if (difference > calculationTime) calculationTime = difference;
}


void onUnknownCommand(char *command){
  SerialGate.print("unknown command: \"");
  SerialGate.print(command);
  SerialGate.println("\"");
}


void onSet(){  

  char* param1 = sCmd1.next(); //DRAADLOOS
  char* value1 = sCmd1.next();
  if (strcmp(param1, "cycle") == 0){
    long newCycleTime = atol(value1);
    float ratio = ((float) newCycleTime) / ((float) params.cycleTime);

    params.ki *= ratio;

    params.cycleTime = newCycleTime;
  }
  else if (strcmp(param1, "ki") == 0){
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.ki = atof(value1) * cycleTimeInSec;
  }
  else if (strcmp(param1, "kd") == 0){
    float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
    params.kd = atof(value1) / cycleTimeInSec;
  }
  
  if (strcmp(param1, "power") == 0) params.power = atoi(value1);
  if (strcmp(param1, "diff") == 0) params.diff = atof(value1);
  if (strcmp(param1, "kp") == 0) params.Kp = atof(value1);
  
  EEPROM_writeAnything(0, params);
}


void onDebug(){
  SerialGate.print("cycle time: ");
  SerialGate.println(params.cycleTime);

  SerialGate.print("calculation time: ");
  SerialGate.println(calculationTime);

  SerialGate.print("calibration white: ");
  for (int i = 0; i < 6; i++){  
    SerialGate.print(params.white[i]);
    SerialGate.print(" ");
  } SerialGate.println(" ");

  SerialGate.print("calibration black: ");
  for (int i = 0; i < 6; i++){  
    SerialGate.print(params.black[i]);
    SerialGate.print(" ");
  } SerialGate.println(" ");

SerialGate.print("normalised: ");
  for (int i = 0; i < 6; i++)
  {
  SerialGate.print(normalised[i]);
  SerialGate.print(" ");
  }
  SerialGate.println(" ");

  SerialGate.print("Position: ");
  SerialGate.println(params.position);

  SerialGate.print("Kp: ");
  SerialGate.println(params.Kp);

  float cycleTimeInSec = ((float) params.cycleTime) / 1000000;
  float ki = params.ki / cycleTimeInSec;
  SerialGate.print("Ki: ");
  SerialGate.println(ki);

  float kd = params.kd * cycleTimeInSec;
  SerialGate.print("Kd: ");
  SerialGate.println(kd);

  SerialGate.print("Diff: ");
  SerialGate.println(params.diff);

  SerialGate.print("Power: ");
  SerialGate.println(params.power);
  
  calculationTime = 0;
}

void onCalibrate(){
  char* param1 = sCmd1.next();  //DRAADLOOS

  if (strcmp(param1, "black") == 0)
  {
    SerialGate.print("start calibrating black... ");
    for (int i = 0; i < 6; i++) params.black[i]=analogRead(sensor[i]);
    SerialGate.println("done");
  }
  else if (strcmp(param1, "white") == 0)
  {
    SerialGate.print("start calibrating white... ");    
    for (int i = 0; i < 6; i++) params.white[i]=analogRead(sensor[i]);  
    SerialGate.println("done");      
  }
  
  EEPROM_writeAnything(0, params);
}

void interrupt(){
  static unsigned long vorigeInterrupt = 0;
  unsigned long interruptTijd = millis();
  if (interruptTijd - vorigeInterrupt > 100){
    running =! running;
  }
  vorigeInterrupt = interruptTijd;
}

void onStart(){
  running = true;
  SerialGate.println("STARTED");
}

void onStop(){
  running = false;
  SerialGate.println("STOPPED");
}

void onValues(){
  SerialGate.print("Normalalised values: ");
  for (int i = 0; i < 6; i ++){
   SerialGate.print(normalised[i]);
      SerialGate.print(" ");
    }
    SerialGate.println(" ");
}