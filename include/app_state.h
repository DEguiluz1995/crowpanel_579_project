#pragma once

// Each value represents one display screen the user can navigate to.
enum class DisplayScreenState{
    Clock = 0,            // HTTP clock screen
    Weather = 1,          // Live weather + forecast screen
    Timer = 2,            // Stopwatch/timer screen
    Prestige = 3,         // XP / progression screen
    StreetPass = 4,       // User profile / greeting screen
    RefreshSettings = 5   // Refresh mode settings submenu
};

// Current active screen being displayed.
extern DisplayScreenState currentScreen;


// ---------------------- TIMER STATE ----------------------
// True while timer is actively counting.
extern bool timerRunning;

// Timestamp when timer started.
// Used with millis() to calculate elapsed time.
extern unsigned long timerStartTime;

// Stores elapsed timer duration when paused.
// Allows timer to resume instead of resetting.
extern unsigned long timerElapsed;


// ---------------------- WIFI / HTTP STATUS ----------------------
// Indicates whether ESP32 successfully connected to WiFi.
extern bool wifiConnected;

// Indicates whether HTTP time synchronization succeeded.
extern bool timeSynced;