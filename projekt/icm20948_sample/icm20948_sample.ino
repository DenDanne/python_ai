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

  fullScale.a = 2; // 3 corresponds to ±16g for the accelerometer
  fullScale.g = 2; // 3 corresponds to ±2000°/s for the gyroscope

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


void printScaledAGMT(ICM_20948_I2C *sensor)
{
  static int i = 0;
  printFormattedFloat(sensor->accX(), 5, 2);
  SERIAL_PORT.print(" ");
  printFormattedFloat(sensor->accY(), 5, 2);
  SERIAL_PORT.print(" ");
  printFormattedFloat(sensor->accZ(), 5, 2);
  SERIAL_PORT.print(" ");
  printFormattedFloat(sensor->gyrX(), 5, 2);
  SERIAL_PORT.print(" ");
  printFormattedFloat(sensor->gyrY(), 5, 2);
  SERIAL_PORT.print(" ");
  printFormattedFloat(sensor->gyrZ(), 5, 2);
  SERIAL_PORT.println();

  if(i % 100 == 0){
    SERIAL_PORT.println();
    i = 0;
  }
  i++;

}
