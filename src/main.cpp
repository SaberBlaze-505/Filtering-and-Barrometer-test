#include <Arduino.h>
#include <Wire.h>
#include <MS5611.h>
#include <math.h>
MS5611 ms5611(0x77);
const int SDA_PIN = 21;
const int SCL_PIN = 22;
float referencePressure = 0.0; // 0m ref
const int CALIBRATION_SAMPLES = 100; // sample

float kalmanEstimate = 0.0; // Estimated altitude
float estimationError = 1.0; // error treshold

float processNoise = 0.01; // Q kasar tp responsif
float measurementNoise = 1.0; // R halus tp lambat

unsigned long sampleNumber = 0; //sample debug

// Previous valid raw altitude
float previousValidAltitude = 0.0;
// Maximum allowed change between consecutive measurements.
const float SPIKE_THRESHOLD = 2.5;

float pressureToRelativeAltitude(float pressure) //rumus
{
    return 44330.0 * (1.0 - pow(pressure / referencePressure, 0.1903));
}

float kalmanFilter(float measurement)
{
    estimationError += processNoise;

    float kalmanGain = estimationError / (estimationError + measurementNoise);

    kalmanEstimate += kalmanGain * (measurement - kalmanEstimate);

    estimationError =
        (1.0 - kalmanGain) *
        estimationError;

    return kalmanEstimate;
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println("GY-63");
    Wire.begin(SDA_PIN, SCL_PIN);
    if (!ms5611.begin())
    {
        Serial.println("ERROR: MS5611 not detected!");
        Serial.println("Check SDA, SCL, VCC and GND.");

        while (1)
        {
            delay(1000);
        }
    }

    Serial.println("MS5611 detected");
    Serial.println("sampling");
    float pressureSum = 0.0;

    for (int i = 0; i < CALIBRATION_SAMPLES; i++)
    {
        ms5611.read();

        float pressure = ms5611.getPressure();

        pressureSum += pressure;

        delay(50);
    }


    referencePressure =
        pressureSum / CALIBRATION_SAMPLES;


    Serial.print("Reference 0m pressure: ");
    Serial.print(referencePressure, 3);
    Serial.println(" mbar");
    Serial.println("Pressure(mbar),RawAltitude(m),KalmanAltitude(m),Status");
}

void loop()
{
    ms5611.read();

    float pressure =
        ms5611.getPressure();


    // Convert
    float rawAltitude =
        pressureToRelativeAltitude(pressure);


    float filteredAltitude = kalmanEstimate;


    filteredAltitude =
        kalmanFilter(rawAltitude);

    previousValidAltitude =
        rawAltitude;


    // sample debug
    sampleNumber++;
    Serial.print("Sample ");
    Serial.print(sampleNumber);
    Serial.print(",");

    //output
    Serial.print(pressure, 3);
    Serial.print(" mbar,");

    Serial.print(rawAltitude, 3);
    Serial.print(" m,");

    Serial.print(filteredAltitude, 3);
    Serial.print(" m,");
    delay(200);
}