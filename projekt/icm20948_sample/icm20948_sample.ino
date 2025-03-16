#include "ICM_20948.h" 
#include "string.h"
#include "model_ski3_randomForestClassifier.h"
#include <WiFi.h>
//#include "Adafruit_MQTT.h"
//#include "Adafruit_MQTT_Client.h"
//#include "mqtt_handling.h"
#include "http_handling.h"

#define SERIAL_PORT Serial
#define SPI_PORT SPI 
#define CS_PIN 2   
#define WIRE_PORT Wire 
#define AD0_VAL 1
#define SIZE_BATCH 150 // 3 Seconds at 50 Hz

#define USE_INTERNET 1

ICM_20948_I2C myICM; 
Eloquent::ML::Port::RandomForest classifier;

ICM_20948_smplrt_t sampleRate;
ICM_20948_fss_t fullScale;

float accx_save[SIZE_BATCH];
float accy_save[SIZE_BATCH];
float accz_save[SIZE_BATCH];
float gyrx_save[SIZE_BATCH];
float gyry_save[SIZE_BATCH];
float gyrz_save[SIZE_BATCH];
float full_sample[6*SIZE_BATCH];
/*
// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLUETOOTH
#include <BluetoothSerial.h>
// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"
#include <RemoteXY.h>


// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 59 bytes
  { 255,0,0,3,0,52,0,19,0,0,0,115,107,105,95,99,108,97,115,115,
  105,102,105,99,97,116,105,111,110,0,31,1,106,200,1,1,2,0,65,3,
  24,65,20,113,129,4,26,61,17,64,177,83,97,120,110,105,110,103,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  uint8_t sax_r; // =0..255 LED Red brightness
  uint8_t sax_g; // =0..255 LED Green brightness
  uint8_t sax_b; // =0..255 LED Green brightness

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)*/

void PredictionResult(void)
{
    // Predict class
    int prediction = classifier.predict(full_sample);

    switch (prediction)
    {
        case 0:
            Serial.println("Predicted class: Saxning (Herringbone technique) \n\n");
            pred = "Saxning (Herringbone technique)";
            break;
        case 1:
            Serial.println("Predicted class: Skate (Skating) \n\n");
            pred = "Skate (Skating)";
            break;
        case 2:
            Serial.println("Predicted class: Diagonalande (Diagonal stride) \n\n");
            pred = "Diagonalande (Diagonal stride)";
            break;
        case 3:
            Serial.println("Predicted class: Stakning (Double poling) \n\n");
            pred = "Stakning (Double Poling)";
            break;
        case 4:
            Serial.println("Predicted class: Stilla (Still) \n\n");
            pred = "Stilla (Still)";
            break;
        default:
            Serial.println("WRONG\n\n");
            pred = "Unknown"; // Ensure pred always has a value
            break;
    }

#if USE_INTERNET
    sendPrediction_http();
#endif // USE_INTERNET
}

void printPredictionResult(void)
{
    // Predict class
    int prediction = classifier.predict(full_sample);

    switch(prediction){
      case 0:
        Serial.println("Predicted class: Saxning (Herringbone technique) \n\n");
        break;
      case 1:
        Serial.println("Predicted class: Skate (Skating) \n\n");
        break;      
      case 2:
        Serial.println("Predicted class: Diagonalande (Diagonal stride) \n\n");
        break;      
      case 3:
        Serial.println("Predicted class: Stakning (Double poling) \n\n");
        break;
      case 4:
        Serial.println("Predicted class: Stilla (Still) \n\n");
        break;
      default:
        Serial.println("WRONG\n\n");
        break;
    }
}


void setup()
{
  delay(2000);

  pinMode(LED_PIN, OUTPUT);

  SERIAL_PORT.begin(460800);
  while (!SERIAL_PORT) {}

  WIRE_PORT.begin(23, 22);  // ESP32 Feather default: SDA = 23, SCL = 22
  WIRE_PORT.setClock(400000);

  bool initialized = false;
  while (!initialized)
  {
    myICM.begin(WIRE_PORT, AD0_VAL);
    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM.statusString());

    if (myICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }

#if USE_INTERNET
    connectWiFi();
#endif // USE_INTERNET
  }

  sampleRate.a = 19;  // 50Hz for Accelerometer (SRD = 19)
  sampleRate.g = 19;  // 50Hz for Gyroscope (SRD = 19)

  fullScale.a = 2; // 2 corresponds to ±8g for the accelerometer
  fullScale.g = 2; // 2 corresponds to ±1000°/s for the gyroscope

  // Set sample rates to 50Hz
  myICM.setSampleRate(ICM_20948_Internal_Acc, sampleRate);
  myICM.setSampleRate(ICM_20948_Internal_Gyr, sampleRate);
  myICM.setFullScale(ICM_20948_Internal_Acc, fullScale);
  myICM.setFullScale(ICM_20948_Internal_Gyr, fullScale);

  SERIAL_PORT.println("Sampling frequency set to 50Hz for all sensors.");

  //RemoteXY_Init(); 

//  reset_colors();
}


void loop()
{
  //RemoteXY_Handler();

  static uint32_t lastTime = 0;  // Stores the last execution time
  const uint32_t interval = 20;  // 50Hz = 20ms per loop

  if (millis() - lastTime >= interval)
  {
    lastTime = millis();  // Update last execution time

    if (myICM.dataReady())  // Check if new data is available
    {
      myICM.getAGMT();         // Read sensor data
      printScaledAGMT(&myICM); // Print data
    }
    else{
      Serial.println("Not ");
    }
  }
}

void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  float aval = abs(val);
  if (val < 0)
  {
    SERIAL_PORT.print("-");
  }
  else
  {
    SERIAL_PORT.print(" ");
  }
  for (uint8_t indi = 0; indi < leading; indi++)
  {
    uint32_t tenpow = 0;
    if (indi < (leading - 1))
    {
      tenpow = 1;
    }
    for (uint8_t c = 0; c < (leading - 1 - indi); c++)
    {
      tenpow *= 10;
    }
    if (aval < tenpow)
    {
      SERIAL_PORT.print("0");
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    SERIAL_PORT.print(-val, decimals);
  }
  else
  {
    SERIAL_PORT.print(val, decimals);
  }
}

void scale_value(ICM_20948_I2C *sensor, float &accX, float &accY, float &accZ, float &gyrX, float &gyrY, float &gyrZ)
{
    // Read raw sensor values
    accX = sensor->accX() / 8000.0;
    accY = sensor->accY() / 8000.0;
    accZ = sensor->accZ() / 8000.0;

    gyrX = sensor->gyrX() / 1000.0;
    gyrY = sensor->gyrY() / 1000.0;
    gyrZ = sensor->gyrZ() / 1000.0;
}

void printFormatFloat(float val, uint8_t decimals)
{
    if (val < 0)
    {
        SERIAL_PORT.print("-");
        val = -val;  // Convert to positive for formatting
    }
    SERIAL_PORT.print(val, decimals);  // Print without leading zeros
}


void printScaledAGMT(ICM_20948_I2C *sensor)
{
    float accX, accY, accZ, gyrX, gyrY, gyrZ;

    // Scale sensor values
    scale_value(sensor, accX, accY, accZ, gyrX, gyrY, gyrZ);

    static int i = 0;
    accx_save[i] = accX;
    accy_save[i] = accY;
    accz_save[i] = accZ;
    gyrx_save[i] = gyrX;
    gyry_save[i] = gyrY;
    gyrz_save[i] = gyrZ;
    i++;
    if(i == SIZE_BATCH)
    {
      memcpy(&full_sample[0*SIZE_BATCH], accx_save, SIZE_BATCH * sizeof(float));
      memcpy(&full_sample[1*SIZE_BATCH], accy_save, SIZE_BATCH * sizeof(float));
      memcpy(&full_sample[2*SIZE_BATCH], accz_save, SIZE_BATCH * sizeof(float));
      memcpy(&full_sample[3*SIZE_BATCH], gyrx_save, SIZE_BATCH * sizeof(float));
      memcpy(&full_sample[4*SIZE_BATCH], gyry_save, SIZE_BATCH * sizeof(float));
      memcpy(&full_sample[5*SIZE_BATCH], gyrz_save, SIZE_BATCH * sizeof(float));
      i = 0;

#if USE_INTERNET
    // sendHTTPDataFaster(full_sample, 6*SIZE_BATCH);
#endif // USE_INTERNET

     printPredictionResult();
   //  sendPrediction_http();
      PredictionResult();
     //Serial.println(millis() / 1000);
    }
}

