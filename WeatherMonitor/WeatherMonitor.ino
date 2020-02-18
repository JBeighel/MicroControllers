//----- Include files     -----//
  #include "CommonUtils.h"
  #include "BME280Driver.h"

//----- Constants         -----//
  #define LIGHT_RED     5
  #define LIGHT_YELLOW  3
  #define LIGHT_GREEN   1

//----- Definitions       -----//

//----- Globals           -----//
  cBME280_t gWeatherSense = cBME280_t(false, &Wire);

//----- Arduino Functions -----//
void setup() {
  bool bResult;
  uint8_t nCtr;
  
  //Initialize arduino drivers
  Serial.begin(9600);
  Wire.begin();

  pinMode(LIGHT_RED, OUTPUT);
  digitalWrite(LIGHT_RED, LOW);
  
  pinMode(LIGHT_YELLOW, OUTPUT) ;
  digitalWrite(LIGHT_YELLOW, LOW);
  
  pinMode(LIGHT_GREEN, OUTPUT);
  digitalWrite(LIGHT_GREEN, HIGH);

  //Initialize peripheral drivers
  bResult = gWeatherSense.CheckDevice();

  if (bResult == false) {
    while (true) {
      digitalWrite(LIGHT_RED, HIGH);
      Serial.println("Device check failed!");
      
      for (nCtr = 0; nCtr < 3; nCtr++) {
        digitalWrite(LIGHT_YELLOW, HIGH);
        delay(100);
        digitalWrite(LIGHT_YELLOW, LOW);
        delay(100);
      }

      delay(1000);
    }
  }

  bResult = gWeatherSense.SetConfig();
  if (bResult == false) {
    while (true) {
      digitalWrite(LIGHT_RED, HIGH);
      Serial.println("Device config failed!");
      
      for (nCtr = 0; nCtr < 2; nCtr++) {
        digitalWrite(LIGHT_YELLOW, HIGH);
        delay(100);
        digitalWrite(LIGHT_YELLOW, LOW);
        delay(100);
      }

      delay(1000);
    }
  }
  
  bResult = gWeatherSense.ReadCalibrationValues();
  if (bResult == false) {
    while (true) {
      digitalWrite(LIGHT_RED, HIGH);
      Serial.println("Device cal read failed!");
      
      digitalWrite(LIGHT_YELLOW, HIGH);
      delay(100);
      digitalWrite(LIGHT_YELLOW, LOW);

      delay(1000);
    }
  }

  return;
}

void loop() {
  float nTemp, nPressure, nHumidity;

  digitalWrite(LIGHT_GREEN, HIGH);
  gWeatherSense.ReadMeasurements(&nTemp, &nPressure, &nHumidity);

  Serial.print("Temp: ");
  Serial.print(nTemp);
  Serial.print(", Humidity: ");
  Serial.print(nHumidity);
  Serial.print(", Pressure: ");
  Serial.print(nPressure);
  Serial.print("\n");
  digitalWrite(LIGHT_GREEN, LOW);

  digitalWrite(LIGHT_YELLOW, HIGH);
  delay(1000);
  digitalWrite(LIGHT_YELLOW, LOW);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
