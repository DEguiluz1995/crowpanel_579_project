#include "app_state.h"

// ---------------------- SCREEN STATE ----------------------
// Default startup screen.
// When the device powers on, the Clock screen appears first.
DisplayScreenState currentScreen = DisplayScreenState::Clock;


// ---------------------- TIMER STATE ----------------------
// Timer begins in a stopped state.
bool timerRunning = false;

// Stores the millis() value when the timer starts.
// Used to calculate elapsed time while running.
unsigned long timerStartTime = 0;

// Stores accumulated elapsed time when paused.
// Prevents timer from resetting every stop/start cycle.
unsigned long timerElapsed = 0;


// ---------------------- WIFI / HTTP STATUS ----------------------
// Starts false until ESP32 successfully connects to WiFi.
bool wifiConnected = false;

// Starts false until HTTP time synchronization succeeds.
bool timeSynced = false;