/**** OXIKIT SENSOR BENCH ***********/

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <RunningAverage.h>
#include "Wire.h"

#define TCAADDR 0x70

#define RA_SIZE 20
RunningAverage RA1(RA_SIZE);
RunningAverage RA2(RA_SIZE);
RunningAverage RA3(RA_SIZE);

Adafruit_ADS1115 sensor1;
Adafruit_ADS1115 sensor2;
Adafruit_ADS1115 sensor3;

double calibrationv1 = 0;
double calibrationv2 = 0;
double calibrationv3 = 0;
float multiplier;

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void read_sensor(int sensor=0) {  
  // Read Sensor 1
  if(sensor == 0)
  {
    tcaselect(0);
    int16_t millivolts = 0;
    millivolts = sensor1.readADC_Differential_0_1();
    RA1.addValue(millivolts);
  }
  if(sensor == 1)
  {
    tcaselect(1);
    int16_t millivolts = 0;
    millivolts = sensor2.readADC_Differential_0_1();
    RA2.addValue(millivolts);
  }
  if(sensor == 2)
  {
    tcaselect(2);
    int16_t millivolts = 0;
    millivolts = sensor3.readADC_Differential_0_1();
    RA3.addValue(millivolts);
  }
}

int calibrate(int x) {
  double result;
  if(x == 0)
  {
    for(int cx=0; cx<= RA_SIZE; cx++) {
      read_sensor(0);
    }
    result = RA1.getAverage();
    result = abs(result);
  }

  if(x == 1)
  {
    for(int cx=0; cx<= RA_SIZE; cx++) {
      read_sensor(1);
    }
    result = RA2.getAverage();
    result = abs(result);
  }

  if(x == 2)
  {
    for(int cx=0; cx<= RA_SIZE; cx++) {
      read_sensor(2);
    }
    result = RA3.getAverage();
    result = abs(result);
  }  
  
  delay(1000);
  return result;
}

void setup(void) {  

  Serial.begin(9600);
  Serial.println("OxiKit Sensor Bench Initalization!!!");
  multiplier = 0.0625F;
  
  // Select Sensor1
  tcaselect(0);
  sensor1.setGain(GAIN_TWO);
  sensor1.begin(); // ads1115 start

  // Select Sensor2
  tcaselect(1);
  sensor2.setGain(GAIN_TWO);
  sensor2.begin(); // ads1115 start

  // Select Sensor2
  tcaselect(2);
  sensor3.setGain(GAIN_TWO);
  sensor3.begin(); // ads1115 start
  
  RA1.clear();
  RA2.clear();
  RA3.clear();
  for(int cx=0; cx<= RA_SIZE; cx++) {
     read_sensor(0);
     read_sensor(1);
     read_sensor(2);
  }

  Serial.println("Calibration Started");
  calibrationv1 = calibrate(0);
  calibrationv2 = calibrate(1);
  calibrationv3 = calibrate(2);
  Serial.println("Calibration Completed");
}

void analysing(int sensor) {
  double currentmv=0;
  double result;
  double mv = 0.0;

  if(sensor == 0)
  {
    read_sensor(0);
    currentmv = RA1.getAverage();
    currentmv = abs(currentmv);
    
    result = (currentmv / calibrationv1) * 20.9;
    if (result > 99.9) result = 99.9;
    mv = currentmv * multiplier;
    Serial.println("Sensor1: ");Serial.println(result);Serial.println("% O2, ");Serial.println(mv);Serial.println(" mv / ");Serial.println(calibrationv1);Serial.println(" mv");
  }
  if(sensor == 1)
  {
    read_sensor(1);
    currentmv = RA2.getAverage();
    currentmv = abs(currentmv);
    
    result = (currentmv / calibrationv2) * 20.9;
    if (result > 99.9) result = 99.9;
    mv = currentmv * multiplier;
    Serial.println("Sensor2: ");Serial.println(result);Serial.println("% O2, ");Serial.println(mv);Serial.println(" mv / ");Serial.println(calibrationv1);Serial.println(" mv");
  }
  if(sensor == 2)
  {
    read_sensor(2);
    currentmv = RA3.getAverage();
    currentmv = abs(currentmv);
    
    result = (currentmv / calibrationv3) * 20.9;
    if (result > 99.9) result = 99.9;
    mv = currentmv * multiplier;
    Serial.println("Sensor3: ");Serial.println(result);Serial.println("% O2, ");Serial.println(mv);Serial.println(" mv / ");Serial.println(calibrationv1);Serial.println(" mv");
  }
}


void loop(void) {
  // Get Sensor 1 Data
  analysing(0);
  delay(200);
  // Get Sensor 2 Data
  analysing(1);
  delay(200);
  // Get Sensor 3 Data
  analysing(2);
  delay(200);
}

