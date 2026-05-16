#pragma once

// ---------------------- WIFI CONNECTION ----------------------
// Attempts to connect ESP32 to configured WiFi network.
// Updates:
// wifiConnected = true  -> successful connection
// wifiConnected = false -> failed connection
// Called during startup in setup().
void connectWiFi();

// ---------------------- HTTP TIME FETCH ----------------------
// Retrieves current time from an online HTTP source.
// Returns:
// true  -> successful request and parse
// false -> failed request or JSON parse error
// Updates:
// - internal time variables
// - timeSynced status
// Called:
// - during startup
// - periodically for auto-refresh
// - manually from Clock screen
bool fetchTimeFromHttp();

// ---------------------- CLOCK DISPLAY TEXT ----------------------
// Formats time information for display.
// Outputs:
// timeBuffer
// statusBuffer
// Used by:
// drawClockScreen()
void getClockText(char timeBuffer[], char statusBuffer[]);