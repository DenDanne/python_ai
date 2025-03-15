#include "ICM_20948.h" 
#include "string.h"
#include "model_randomForestClassifier2.h"
#include <WiFi.h> // Required for ESP32
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
//#include "mqtt_handling.h"
#include "http_handling.h"


#define SERIAL_PORT Serial
#define SPI_PORT SPI 
#define CS_PIN 2   
#define WIRE_PORT Wire 
#define AD0_VAL 1
#define SIZE_BATCH 150 // 3 Seconds at 50 Hz

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

void printPredictionResult(void)
{
    // Predict class
    int prediction = classifier.predict(full_sample);

    switch(prediction){
      case 0:
        Serial.println("Predicted class: Circles\n\n");
        break;
      case 1:
        Serial.println("Predicted class: Side to side\n\n");
        break;      
      case 2:
        Serial.println("Predicted class: Up and down\n\n");
        break;      
      case 3:
        Serial.println("Predicted class: Diagonal\n\n");
        break;
      case 4:
        Serial.println("Predicted class: Still\n\n");
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

    connectWiFi();
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
}


void loop()
{
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

void check_print()
{
    for (int i = 0; i < SIZE_BATCH; i++)
    {
        Serial.print("accx_save["); Serial.print(i); Serial.print("]: ");
        Serial.print(accx_save[i], 6);
        Serial.print(" | full_sample["); Serial.print(i); Serial.print("]: ");
        Serial.println(full_sample[i], 6);
    }

    for (int i = 0; i < SIZE_BATCH; i++)
    {
        Serial.print("accy_save["); Serial.print(i); Serial.print("]: ");
        Serial.print(accy_save[i], 6);
        Serial.print(" | full_sample["); Serial.print(SIZE_BATCH + i); Serial.print("]: ");
        Serial.println(full_sample[SIZE_BATCH + i], 6);
    }

    for (int i = 0; i < SIZE_BATCH; i++)
    {
        Serial.print("accz_save["); Serial.print(i); Serial.print("]: ");
        Serial.print(accz_save[i], 6);
        Serial.print(" | full_sample["); Serial.print(2 * SIZE_BATCH + i); Serial.print("]: ");
        Serial.println(full_sample[2 * SIZE_BATCH + i], 6);
    }

    for (int i = 0; i < SIZE_BATCH; i++)
    {
        Serial.print("gyrx_save["); Serial.print(i); Serial.print("]: ");
        Serial.print(gyrx_save[i], 6);
        Serial.print(" | full_sample["); Serial.print(3 * SIZE_BATCH + i); Serial.print("]: ");
        Serial.println(full_sample[3 * SIZE_BATCH + i], 6);
    }

    for (int i = 0; i < SIZE_BATCH; i++)
    {
        Serial.print("gyry_save["); Serial.print(i); Serial.print("]: ");
        Serial.print(gyry_save[i], 6);
        Serial.print(" | full_sample["); Serial.print(4 * SIZE_BATCH + i); Serial.print("]: ");
        Serial.println(full_sample[4 * SIZE_BATCH + i], 6);
    }

    for (int i = 0; i < SIZE_BATCH; i++)
    {
        Serial.print("gyrz_save["); Serial.print(i); Serial.print("]: ");
        Serial.print(gyrz_save[i], 6);
        Serial.print(" | full_sample["); Serial.print(5 * SIZE_BATCH + i); Serial.print("]: ");
        Serial.println(full_sample[5 * SIZE_BATCH + i], 6);
    }
    Serial.println(" ");
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

     sendHTTPDataFaster(full_sample, 6*SIZE_BATCH);
    // printPredictionResult();
     //Serial.println(millis() / 1000);
    }
}

