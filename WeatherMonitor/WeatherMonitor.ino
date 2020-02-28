//----- Include files     -----//
  #include "CommonUtils.h"
  #include "BME280Driver.h"
  #include "SSD1306Driver.h"

//----- Constants         -----//
  #define LIGHT_RED         5
  #define LIGHT_YELLOW      3
  #define LIGHT_GREEN       1

  #define SCREEN_DC         16
  #define SCREEN_RESET      17
  #define SCREEN_CS         18

  #define SCREEN_LINEHEIGHT 18
  #define SCREEN_CHARWIDTH  6

  #define ANALOG_BATTERY    0
  #define BATTERY_CAL       (5.88f / 0.52f)

//----- Definitions       -----//

//----- Globals           -----//
  cBME280_t gWeatherSense = cBME280_t(false, &Wire);
  SSD1306 gScreen = SSD1306();

//----- Arduino Functions -----//
void setup() {
  bool bResult;
  uint8_t nCtr;
  
  //Initialize arduino drivers
  Serial.begin(9600);
  Wire.begin();
  SPI.begin();

  pinMode(LIGHT_RED, OUTPUT);
  digitalWrite(LIGHT_RED, LOW);
  
  pinMode(LIGHT_YELLOW, OUTPUT) ;
  digitalWrite(LIGHT_YELLOW, LOW);
  
  pinMode(LIGHT_GREEN, OUTPUT);
  digitalWrite(LIGHT_GREEN, HIGH);

  pinMode(SCREEN_DC, OUTPUT);
  pinMode(SCREEN_RESET, OUTPUT);
  pinMode(SCREEN_CS, OUTPUT);

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

  gScreen.BeginSPI(&SPI, 128, 64, SCREEN_DC, SCREEN_CS, SCREEN_RESET);
  gScreen.ClearDrawing();
  gScreen.SendToScreen();

  return;
}

void loop() {
  float nTemp, nPressure, nHumidity, nBattery;
  char Text[10];

  digitalWrite(LIGHT_GREEN, HIGH);

  //Pull in readings and measurements
  gWeatherSense.ReadMeasurements(&nTemp, &nPressure, &nHumidity);

  nTemp = ((nTemp * 9.0f) / 5.0f) + 32; //Convert degrees C to F

  nBattery = analogRead(ANALOG_BATTERY);
  nBattery = (nBattery * 3.3f) / 1024.0f; //Convert to volts on the pin
  nBattery *= BATTERY_CAL;

  //Write to serial for debug
  Serial.print("Temp: ");
  Serial.print(nTemp);
  Serial.print(", Humidity: ");
  Serial.print(nHumidity);
  Serial.print(", Pressure: ");
  Serial.print(nPressure);
  Serial.print(", Battery: ");
  Serial.print(nBattery);
  Serial.print("\n");
  
  //Send readings to the screen
  gScreen.ClearDrawing();
  gScreen.DrawText(0, 0 * SCREEN_LINEHEIGHT, "Temper"); //Degrees farenheit
  gScreen.DrawText(0, 1 * SCREEN_LINEHEIGHT, "Humidity"); //% air/water mixture
  gScreen.DrawText(0, 2 * SCREEN_LINEHEIGHT, "Pressure"); //Pascals
  gScreen.DrawText(0, 3 * SCREEN_LINEHEIGHT, "Battery"); //Volts

  sprintf(Text, "%d.%02d", (int32_t)nTemp, (int32_t)(nTemp * 100) - ((int32_t)nTemp * 100));
  gScreen.DrawText(9 * SCREEN_CHARWIDTH, 0 * SCREEN_LINEHEIGHT, Text);

  sprintf(Text, "%d.%02d", (int32_t)nHumidity, (int32_t)(nHumidity * 100) - ((int32_t)nHumidity * 100));
  gScreen.DrawText(9 * SCREEN_CHARWIDTH, 1 * SCREEN_LINEHEIGHT, Text);
  
  sprintf(Text, "%d.%02d", (int32_t)nPressure, (int32_t)(nPressure * 100) - ((int32_t)nPressure * 100));
  gScreen.DrawText(9 * SCREEN_CHARWIDTH, 2 * SCREEN_LINEHEIGHT, Text);

  sprintf(Text, "%d.%02d", (int32_t)nBattery, (int32_t)(nBattery * 100) - ((int32_t)nBattery * 100));
  gScreen.DrawText(9 * SCREEN_CHARWIDTH, 3 * SCREEN_LINEHEIGHT, Text);
  
  gScreen.SendToScreen();

  digitalWrite(LIGHT_GREEN, LOW);
  digitalWrite(LIGHT_YELLOW, HIGH);
  delay(1000);
  digitalWrite(LIGHT_YELLOW, LOW);
  
  return;
}

//----- Callback Functions -----//

//----- Application Code   -----//
