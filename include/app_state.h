#pragma once

enum class DisplayScreenState
{
    Clock = 0,
    Weather = 1,
    Timer = 2
};

struct ForecastDay
{
    char day[12];
    int highTemp;
    int lowTemp;
};

extern DisplayScreenState currentScreen;
extern bool timerRunning;
extern unsigned long timerStartTime;
extern unsigned long timerElapsed;
extern bool wifiConnected;
extern bool timeSynced;