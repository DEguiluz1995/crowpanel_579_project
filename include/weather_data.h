#pragma once

// ---------------------- WEATHER STRUCT ----------------------
//
// Represents a single forecast day.
//
// Stores:
//
// day            → abbreviated weekday
// label          → weather condition (SUN, CLD, RAIN)
// highTemp       → predicted daily high
// lowTemp        → predicted daily low
// precipPercent  → chance of precipitation
struct WeatherDay{
    char day[8];
    char label[12];
    int highTemp;
    int lowTemp;
    int precipPercent;
};

// ---------------------- CURRENT WEATHER ----------------------

// Current measured temperature.
extern int currentTemp;

// "Feels like" temperature accounting for conditions.
extern int feelsLikeTemp;

// Current humidity percentage.
extern int currentHumidity;

// Current weather description.
extern char currentCondition[20];

// ---------------------- FORECAST DATA ----------------------
// Stores 5-day weather forecast.
extern WeatherDay forecast[5];

// ---------------------- WEATHER FUNCTIONS ----------------------
// Loads fallback/mock weather values.
void loadMockWeatherData();

// Fetches live weather data via HTTP.
bool fetchWeatherData();