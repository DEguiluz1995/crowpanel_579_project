#include "app_state.h"

DisplayScreenState currentScreen = DisplayScreenState::Clock;
bool timerRunning = false;
unsigned long timerStartTime = 0;
unsigned long timerElapsed = 0;
bool wifiConnected = false;
bool timeSynced = false;