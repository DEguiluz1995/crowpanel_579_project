#pragma once

struct WeatherDay
{
    char day[8];
    char label[8];
    int highTemp;
    int lowTemp;
    int precipPercent;
};

extern int currentTemp;
extern int feelsLikeTemp;
extern int currentHumidity;
extern char currentCondition[20];

extern WeatherDay forecast[5];

void loadMockWeatherData();