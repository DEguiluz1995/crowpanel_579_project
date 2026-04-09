#include <Arduino.h>
#include <cstdio>
#include "timer_logic.h"
#include "app_state.h"

void formatTimer(char buffer[], unsigned long ms)
{
    unsigned long totalSeconds = ms / 1000;
    unsigned long minutes = totalSeconds / 60;
    unsigned long seconds = totalSeconds % 60;

    sprintf(buffer, "%02lu:%02lu", minutes, seconds);
}

void toggleTimer()
{
    if (timerRunning)
    {
        timerElapsed = millis() - timerStartTime;
        timerRunning = false;
    }
    else
    {
        timerStartTime = millis() - timerElapsed;
        timerRunning = true;
    }
}

void resetTimer()
{
    timerRunning = false;
    timerElapsed = 0;
}