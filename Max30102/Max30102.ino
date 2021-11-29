#include <heartRate.h>
#include <MAX30105.h>
#include <spo2_algorithm.h>
#include "heartRate.h"
#include <Wire.h>

#define TIMETOBOOT 3000
#define SCALE 88.0
#define SAMPLING 5
#define FINGER_ON 30000
#define MINIMUM_SpO2 80.0
#define USEFIFO

MAX30105 particleSensor;

double avered = 0; double aveir = 0;
double sumirrms =0; double sumredrms = 0 ;
int i = 0; int Num = 200;

double ESpO2 = 95.0;
double FSpO2 = 0.7;
double frate = 0.95;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
floaty beatsPerMinute;
int beatAvg;

void setup(){
  while(! particleSensor.begin(Wire, I2C_SPEEDFAST)))
  {
    Serial.println("Max30102 was not found. Please check your Connetions");
  }  
  byte ledBrightness = 0x7F;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 200;
  int pulseWidth = 411;
  int adcRange = 16384;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange );

  particle.enableDIETEMPRDY();
}

void loop()
{
  uint32_t ir, red, green;
  double fred, fir;
  double SpO2 = 0;
  #ifdef USEFIFO
    particleSensor.check();
    while(partileSensor.available());
  #ifdef MAX30105
    red = particleSensor.getFIFORed();
    ir = particleSensor.getFIFOIR();
  #else
    red = particleSensor.getFIFOIR();
    ir = particleSensor.getFIFORed();
  #endif

  i++;
  fred = (double)red;
  fir = (double)ir;
  avered = avered * frate + (double)red * (1.0 - frate);
  aveir = aveir * frate + (double)ir * (1.0 - frate);
  sumredrms += (fred - avered) * (fred - avered);
  sumirrms += (fir - aveir) * (fir - aveir);

  if ((i % SAMPLING) == 0) {
    if (millis() > TIMETOBOOT) {
      if (ir < FINGER_ON) ESpO2 = MINIMUM_SpO2;
      float temp = particleSensor.readTemperatureF();

        Serial.println("Blood Oxygen = ");
        Serial.print(ESpO2);
    }
  }
  
  if ((i % Num) == 0) {
    double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
    SpO2 = -23.3 * (R - 0.4) + 100;
    ESpO2 = FSpO2 * ESpO2 + (1.0 - FSpO2) * SpO2;

    sumredrms = 0.0; sumirrms = 0.0; i = 0;
    break;
  }
  particleSensor.nextSample();
  }
#endif   
}
