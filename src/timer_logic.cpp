#include <Arduino.h>
#include <cstdio>
#include "timer_logic.h"
#include "app_state.h"

// ---------------------- TIMER FORMATTER ----------------------
//
// Converts elapsed milliseconds into MM:SS format.
void formatTimer(char buffer[], unsigned long ms){
    unsigned long totalSeconds = ms / 1000;
    unsigned long minutes = totalSeconds / 60;
    unsigned long seconds = totalSeconds % 60;

    sprintf(buffer, "%02lu:%02lu", minutes, seconds);
}

// ---------------------- START / PAUSE TIMER ----------------------
// Controls stopwatch behavior.
// If timer is running:
// - Save elapsed duration
// - Stop counting
//
// If timer is stopped:
// - Resume timing using previously saved elapsed duration
// - Continue counting instead of restarting
//
// This creates pause/resume functionality.
void toggleTimer(){
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

// ---------------------- RESET TIMER ----------------------
// Completely clears timer state.
void resetTimer(){
    timerRunning = false;
    timerElapsed = 0;
}