#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <cstring>
#include <cstdio>
#include "weather_data.h"

int currentTemp = 72;
int feelsLikeTemp = 75;
int currentHumidity = 48;
char currentCondition[20] = "Sunny";

WeatherDay forecast[5];

void loadMockWeatherData(){
    currentTemp = 72;
    feelsLikeTemp = 75;
    currentHumidity = 48;
    strcpy(currentCondition, "Sunny");

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

void getShortDayName(const char dateText[], char dayBuffer[]){
    int year = 0;
    int month = 0;
    int day = 0;

    sscanf(dateText, "%d-%d-%d", &year, &month, &day);

    if (month < 3){
        month += 12;
        year--;
    }

    int K = year % 100;
    int J = year / 100;

    int h = (day + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

    if (h == 0)
        strcpy(dayBuffer, "Sat");
    else if (h == 1)
        strcpy(dayBuffer, "Sun");
    else if (h == 2)
        strcpy(dayBuffer, "Mon");
    else if (h == 3)
        strcpy(dayBuffer, "Tue");
    else if (h == 4)
        strcpy(dayBuffer, "Wed");
    else if (h == 5)
        strcpy(dayBuffer, "Thu");
    else
        strcpy(dayBuffer, "Fri");
}

void weatherCodeToText(int code, char conditionBuffer[], char labelBuffer[]){
    if (code == 0){
        strcpy(conditionBuffer, "Clear");
        strcpy(labelBuffer, "SUN");
    }
    else if (code == 1 || code == 2 || code == 3){
        strcpy(conditionBuffer, "Cloudy");
        strcpy(labelBuffer, "CLD");
    }
    else if (code == 45 || code == 48){
        strcpy(conditionBuffer, "Fog");
        strcpy(labelBuffer, "FOG");
    }
    else if (code == 51 || code == 53 || code == 55 ||
             code == 61 || code == 63 || code == 65 ||
             code == 80 || code == 81 || code == 82)
    {
        strcpy(conditionBuffer, "Rain");
        strcpy(labelBuffer, "RAIN");
    }
    else if (code == 71 || code == 73 || code == 75 ||
             code == 85 || code == 86)
    {
        strcpy(conditionBuffer, "Snow");
        strcpy(labelBuffer, "SNOW");
    }
    else if (code == 95 || code == 96 || code == 99){
        strcpy(conditionBuffer, "Storm");
        strcpy(labelBuffer, "STM");
    }
    else{
        strcpy(conditionBuffer, "Weather");
        strcpy(labelBuffer, "N/A");
    }
}

bool fetchWeatherData(){
    if (WiFi.status() != WL_CONNECTED){
        Serial.println("Weather fetch failed: WiFi not connected");
        return false;
    }

    HTTPClient http;

    String url =
        "https://api.open-meteo.com/v1/forecast"
        "?latitude=33.971203"
        "&longitude=-117.37882"
        "&current=temperature_2m,relative_humidity_2m,apparent_temperature,weather_code"
        "&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max"
        "&temperature_unit=fahrenheit"
        "&timezone=America%2FLos_Angeles"
        "&forecast_days=5";

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode != 200){
        Serial.print("Weather HTTP error: ");
        Serial.println(httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, payload);

    if (error){
        Serial.print("Weather JSON parse failed: ");
        Serial.println(error.c_str());
        return false;
    }

    float tempNow =
        doc["current"]["temperature_2m"].as<float>();

    float feelsNow =
        doc["current"]["apparent_temperature"].as<float>();

    currentTemp =
        (int)(tempNow + 0.5);

    feelsLikeTemp =
        (int)(feelsNow + 0.5);

    currentHumidity =
        doc["current"]["relative_humidity_2m"].as<int>();

    int currentCode =
        doc["current"]["weather_code"].as<int>();

    char throwawayLabel[12];

    weatherCodeToText(
        currentCode,
        currentCondition,
        throwawayLabel
    );

    for (int i = 0; i < 5; i++){
        const char* dateText =
            doc["daily"]["time"][i] | "";

        getShortDayName(
            dateText,
            forecast[i].day
        );

        int dayCode =
            doc["daily"]["weather_code"][i].as<int>();

        char throwawayCondition[20];

        weatherCodeToText(
            dayCode,
            throwawayCondition,
            forecast[i].label
        );

        float highValue =
            doc["daily"]["temperature_2m_max"][i].as<float>();

        float lowValue =
            doc["daily"]["temperature_2m_min"][i].as<float>();

        forecast[i].highTemp =
            (int)(highValue + 0.5);

        forecast[i].lowTemp =
            (int)(lowValue + 0.5);

        forecast[i].precipPercent =
            doc["daily"]["precipitation_probability_max"][i].as<int>();
    }

    Serial.println("Weather updated from HTTP");

    return true;
}