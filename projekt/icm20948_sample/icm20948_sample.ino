/****************************************************************
 * Example1_Basics.ino
 * ICM 20948 Arduino Library Demo
 * Use the default configuration to stream 9-axis IMU data
 * Owen Lyke @ SparkFun Electronics
 * Original Creation Date: April 17 2019
 *
 * Please see License.md for the license information.
 *
 * Distributed as-is; no warranty is given.
 ***************************************************************/
#include "ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU
#include "string.h"
#include "model_randomForestClassifier.h"
//#define USE_SPI       // Uncomment this to use SPI

#define SERIAL_PORT Serial

#define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2     // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1

#ifdef USE_SPI
ICM_20948_SPI myICM; // If using SPI create an ICM_20948_SPI object
#else
ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object
#endif

ICM_20948_smplrt_t sampleRate;
ICM_20948_fss_t fullScale;


Eloquent::ML::Port::RandomForest classifier;

#define SIZE_BATCH 100
float accx_save[SIZE_BATCH];
float accy_save[SIZE_BATCH];
float accz_save[SIZE_BATCH];
float gyrx_save[SIZE_BATCH];
float gyry_save[SIZE_BATCH];
float gyrz_save[SIZE_BATCH];
float full_sample[6*SIZE_BATCH];

void setup()
{
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

 /* while(1)
  {
    // Example input data
    float sample[] = { -0.006, -0.001,  0.002, -0.001, -0.002, -0.004, -0.003,  0.001,
         0.007,  0.002, -0.003, -0.007, -0.001,  0.001,  0.005,  0.004,
         0.002, -0.008, -0.011, -0.01 , -0.013, -0.012, -0.007, -0.006,
        -0.007, -0.009, -0.005,  0.001,  0.031,  0.009, -0.006, -0.013,
         0.007,  0.016,  0.006, -0.   , -0.003,  0.008,  0.007,  0.   ,
         0.006,  0.01 ,  0.004,  0.003, -0.003,  0.023, -0.003, -0.004,
        -0.011, -0.013, -0.011, -0.01 , -0.005, -0.006, -0.009, -0.011,
        -0.009, -0.004, -0.009, -0.006,  0.008,  0.019,  0.008, -0.009,
        -0.002,  0.01 ,  0.016,  0.016,  0.013,  0.006,  0.007,  0.014,
         0.013,  0.012,  0.007,  0.004, -0.   ,  0.002, -0.001, -0.001,
        -0.005, -0.01 , -0.009, -0.008, -0.012, -0.008, -0.006,  0.001,
         0.   , -0.002, -0.005, -0.001,  0.002,  0.005, -0.005, -0.017,
        -0.019, -0.006, -0.002,  0.001,  0.019,  0.045,  0.022, -0.015,
        -0.037, -0.036, -0.016, -0.002, -0.011, -0.013, -0.009, -0.003,
         0.006,  0.001,  0.007, -0.   ,  0.008,  0.   , -0.011, -0.02 ,
        -0.016, -0.008, -0.005, -0.01 , -0.015, -0.016, -0.014,  0.   ,
         0.006,  0.01 , -0.006, -0.008,  0.027,  0.038,  0.009, -0.033,
        -0.036, -0.011, -0.009, -0.013, -0.003,  0.005, -0.002,  0.006,
         0.011, -0.027, -0.   , -0.003, -0.003, -0.008, -0.012, -0.013,
        -0.004, -0.013, -0.021, -0.018, -0.004, -0.015, -0.023, -0.01 ,
         0.038,  0.024, -0.003, -0.018,  0.011,  0.02 ,  0.02 , -0.008,
        -0.023, -0.022, -0.016, -0.001,  0.002,  0.003, -0.001,  0.001,
         0.002,  0.004,  0.003,  0.007,  0.008, -0.003, -0.005, -0.01 ,
        -0.011, -0.007, -0.006,  0.002, -0.004, -0.012, -0.003,  0.019,
         0.028,  0.016,  0.027,  0.023,  0.01 ,  0.003, -0.003, -0.026,
         0.156,  0.181,  0.239,  0.273,  0.287,  0.268,  0.238,  0.222,
         0.221,  0.222,  0.209,  0.173,  0.138,  0.112,  0.085,  0.067,
         0.052,  0.042,  0.033,  0.021,  0.017,  0.02 ,  0.019,  0.033,
         0.049,  0.06 ,  0.061,  0.076,  0.092,  0.139,  0.163,  0.175,
         0.179,  0.219,  0.261,  0.285,  0.267,  0.244,  0.23 ,  0.222,
         0.212,  0.206,  0.182,  0.155,  0.138,  0.126,  0.09 ,  0.067,
         0.054,  0.037,  0.029,  0.018,  0.018,  0.032,  0.04 ,  0.046,
         0.057,  0.066,  0.076,  0.074,  0.106,  0.149,  0.184,  0.183,
         0.176,  0.197,  0.238,  0.267,  0.275,  0.263,  0.233,  0.21 ,
         0.203,  0.197,  0.173,  0.148,  0.126,  0.098,  0.074,  0.054,
         0.045,  0.039,  0.023,  0.018,  0.022,  0.019,  0.023,  0.039,
         0.059,  0.072,  0.077,  0.093,  0.115,  0.162,  0.185,  0.195,
         0.211,  0.251,  0.283,  0.285,  0.052,  0.066,  0.077,  0.063,
         0.03 , -0.007, -0.029, -0.044, -0.051, -0.069, -0.084, -0.102,
        -0.117, -0.115, -0.113, -0.105, -0.081, -0.055, -0.035, -0.021,
        -0.012,  0.004,  0.029,  0.054,  0.072,  0.081,  0.076,  0.081,
         0.082,  0.095,  0.094,  0.078,  0.071,  0.073,  0.077,  0.065,
         0.026, -0.009, -0.027, -0.042, -0.056, -0.067, -0.08 , -0.1  ,
        -0.109, -0.114, -0.113, -0.102, -0.08 , -0.064, -0.041, -0.018,
         0.003,  0.018,  0.043,  0.051,  0.062,  0.068,  0.063,  0.066,
         0.077,  0.095,  0.097,  0.087,  0.077,  0.065,  0.063,  0.053,
         0.028, -0.011, -0.046, -0.07 , -0.085, -0.093, -0.1  , -0.109,
        -0.117, -0.108, -0.105, -0.093, -0.074, -0.047, -0.025, -0.005,
         0.01 ,  0.033,  0.047,  0.067,  0.087,  0.089,  0.092,  0.092,
         0.097,  0.089,  0.098,  0.1  ,  0.086,  0.078,  0.059,  0.027,
         0.009,  0.044,  0.054,  0.041,  0.016, -0.005, -0.004,  0.   ,
         0.006,  0.001, -0.012, -0.024, -0.019, -0.023, -0.021, -0.02 ,
        -0.022, -0.021, -0.022, -0.015, -0.01 , -0.004, -0.005, -0.004,
        -0.012, -0.016, -0.008,  0.004,  0.008,  0.011, -0.005, -0.003,
         0.024,  0.047,  0.035,  0.011, -0.006, -0.005, -0.002, -0.002,
        -0.001, -0.008, -0.016, -0.017, -0.014, -0.019, -0.02 , -0.02 ,
        -0.018, -0.013, -0.005,  0.006,  0.011,  0.008, -0.004, -0.006,
        -0.012, -0.008, -0.002,  0.001,  0.004,  0.005, -0.017, -0.024,
        -0.007,  0.036,  0.04 ,  0.02 , -0.002, -0.013, -0.01 ,  0.009,
         0.015, -0.001, -0.014, -0.018, -0.017, -0.024, -0.021, -0.009,
        -0.004, -0.01 , -0.009, -0.001,  0.003,  0.004,  0.007,  0.006,
         0.001, -0.004, -0.003,  0.004,  0.011,  0.008, -0.003, -0.006,
         0.025,  0.044,  0.026,  0.007, -0.029, -0.009,  0.009,  0.011,
        -0.003, -0.012, -0.011,  0.004,  0.007, -0.002, -0.012, -0.014,
        -0.011, -0.001,  0.008,  0.006,  0.012,  0.018,  0.021,  0.012,
         0.009,  0.008,  0.008,  0.007, -0.009, -0.021, -0.015, -0.01 ,
        -0.014, -0.013, -0.03 , -0.033, -0.005,  0.026,  0.025,  0.004,
        -0.018, -0.006,  0.012,  0.005, -0.   ,  0.003,  0.006,  0.006,
         0.015,  0.021,  0.012,  0.009,  0.015,  0.013,  0.013,  0.01 ,
         0.011,  0.012,  0.003, -0.009, -0.013, -0.011, -0.022, -0.024,
        -0.002, -0.009, -0.027, -0.028, -0.018,  0.001,  0.005,  0.004,
        -0.009, -0.019, -0.011,  0.004,  0.011,  0.011,  0.   , -0.005,
        -0.001, -0.001,  0.005,  0.014,  0.018,  0.021,  0.019,  0.014,
         0.01 ,  0.007, -0.   , -0.006, -0.008, -0.015, -0.016, -0.006,
         0.003, -0.006, -0.012,  0.003,  0.013,  0.01 ,  0.   , -0.015 };

    // Predict class
    int prediction = classifier.predict(sample);

    Serial.print("Predicted class: ");
    Serial.println(prediction);

    delay(1000); // Adjust delay as needed
  }*/

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

      check_print();

    }



   /* printFormatFloat(accX, 3);
    SERIAL_PORT.print(" ");
    printFormatFloat(accY, 3);
    SERIAL_PORT.print(" ");
    printFormatFloat(accZ, 3);
    SERIAL_PORT.print(" ");
    printFormatFloat(gyrX, 3);
    SERIAL_PORT.print(" ");
    printFormatFloat(gyrY, 3);
    SERIAL_PORT.print(" ");
    printFormatFloat(gyrZ, 3);
    SERIAL_PORT.println();*/

}

