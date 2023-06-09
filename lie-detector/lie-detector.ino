#include "Wire.h"
#include "DFRobot_SHT20.h"
DFRobot_SHT20    sht20;
#include <Wire.h>
#include "MAX30105.h"
//#include <LiquidCrystal.h>
#include "heartRate.h"
MAX30105 particleSensor;
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
void setup()
{
Serial.begin(9600);
sht20.initSHT20(); // Init SHT20 Sensor
delay(100);
sht20.checkSHT20(); // Check SHT20 Sensor

  // Initialize sensor
if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
{
  Serial.println("MAX30105 was not found. Please check wiring/power. ");
  while (1);
}
Serial.println("Place your index finger on the sensor with steady pressure.");

particleSensor.setup(); //Configure sensor with default settings
  //particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  //particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}
void loop()
{
float humd = sht20.readHumidity(); // Read Humidity
float temp = sht20.readTemperature(); // Read Temperature
Serial.print(temp, 2);
Serial.print(",");
Serial.print(analogRead(A3)); // Read Pulse sensor value
Serial.print(",");
Serial.print(humd, 1);
Serial.print(",");
//Serial.println(analogRead(A2)); // Read Breath rate
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
  //Serial.setCursor(0,0);
  Serial.print("BPM: ");
  Serial.print(beatAvg);
  //Serial.setCursor(0,1);
  Serial.print(" IR: ");
  Serial.print(irValue);
}
