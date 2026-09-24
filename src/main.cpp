#include <Arduino.h>
#include <Wire.h>
#include <MS5611.h>

MS5611 ms5611(0x77);

const float P0 = 1013.25;


float referenceAltitude = 0.0;



float pressureToAltitude(float pressure)
{
    return 44330.0 * (1.0 - pow(pressure / P0, 0.1903));
}


void setup()
{
    Serial.begin(115200);
    delay(1000);

    Wire.begin(21, 22);

    Serial.println("Starting GY-63 / MS5611...");

    if (!ms5611.begin())
    {
        Serial.println("ERROR: MS5611 not detected!");

    while (1)
        {
        delay(1000);
        }
    }

    Serial.println("MS5611 detected!");
    Serial.println("Calibrating zero altitude...");
    Serial.println("Keep the sensor still!");


    float altitudeSum = 0.0;

    for (int i = 0; i < 20; i++)
    {
        ms5611.read();
        float pressure = ms5611.getPressure();
        float altitude = pressureToAltitude(pressure);
        altitudeSum += altitude;
        delay(100);
    }

    referenceAltitude = altitudeSum / 20.0;
    Serial.print("Reference altitude: ");
    Serial.print(referenceAltitude, 2);
    Serial.println(" m");
    Serial.println();
    Serial.println("Relative altimeter ready!");
    Serial.println();
}


void loop()
{
    ms5611.read();

    float pressure = ms5611.getPressure();

    float absoluteAltitude =
    pressureToAltitude(pressure);

    float relativeAltitude =
    absoluteAltitude - referenceAltitude;

    Serial.print("Pressure: ");
    Serial.print(pressure, 2);
    Serial.print(" mbar | Absolute: ");
    Serial.print(absoluteAltitude, 2);
    Serial.print(" m | Relative: ");
    Serial.print(relativeAltitude, 2);
    Serial.println(" m");
    delay(500);
}