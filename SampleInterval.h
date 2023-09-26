#ifndef SampleInterval_h
#define SampleInterval_h

class SampleInterval
{
  public:
        // Intervals in milliseconds
        static inline const int Reporting = 10000;
        static inline const int ScreenUpdate = 5000;
        static inline const int TVOC = 1000;
        static inline const int CO2 = 5000;
        static inline const int PM = 5000;
        static inline const int TemperatureHumidity = 2500;
};

#endif