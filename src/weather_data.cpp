#include <cstring>
#include "weather_data.h"

int currentTemp = 72;
int feelsLikeTemp = 75;
int currentHumidity = 48;
char currentCondition[20] = "Sunny";

WeatherDay forecast[5];

void loadMockWeatherData()
{
    strcpy(forecast[0].day, "Mon");
    strcpy(forecast[0].label, "SUN");
    forecast[0].highTemp = 78;
    forecast[0].lowTemp = 52;
    forecast[0].precipPercent = 10;

    strcpy(forecast[1].day, "Tue");
    strcpy(forecast[1].label, "CLD");
    forecast[1].highTemp = 74;
    forecast[1].lowTemp = 50;
    forecast[1].precipPercent = 20;

    strcpy(forecast[2].day, "Wed");
    strcpy(forecast[2].label, "RAIN");
    forecast[2].highTemp = 67;
    forecast[2].lowTemp = 48;
    forecast[2].precipPercent = 70;

    strcpy(forecast[3].day, "Thu");
    strcpy(forecast[3].label, "SUN");
    forecast[3].highTemp = 75;
    forecast[3].lowTemp = 49;
    forecast[3].precipPercent = 0;

    strcpy(forecast[4].day, "Fri");
    strcpy(forecast[4].label, "CLD");
    forecast[4].highTemp = 73;
    forecast[4].lowTemp = 51;
    forecast[4].precipPercent = 15;
}