#ifndef SampleInterval_h
#define SampleInterval_h

// Defines time interval to wait between taking samples in milliseconds
class SampleInterval
{
  public:
        // Interval for posting data to a data target.
        static inline const int Reporting = 10000;

        // Interval for updating the OLED display.
        static inline const int ScreenUpdate = 5000;

        // Interval for processing the VOC and NOX algorithms.
        static inline const int TVOC = 1000;

        // Interval for reading Carbon Dioxide.
        static inline const int CO2 = 5000;

        // Interval for reading Particulate Matter.
        static inline const int PM = 5000;

        // Interval for checking the temperature and humidity.
        static inline const int TemperatureHumidity = 2500;
};

#endif