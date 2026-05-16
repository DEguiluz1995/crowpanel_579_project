#pragma once

// Draws text onto the e-paper display buffer.
//
// Parameters:
// x = horizontal position
// y = vertical position
// text = string to display
// size = font size
void drawLabel(int x, int y, const char text[], int size);


// ---------------------- SCREEN DRAW FUNCTIONS ----------------------
// Draws the Clock screen.
// Displays HTTP time and connection status.
void drawClockScreen();

// Draws the Weather screen.
// Displays current conditions and multi-day forecast.
void drawWeatherScreen();

// Draws the Timer screen.
// Displays stopwatch time and running/stopped state.
void drawTimerScreen();


// ---------------------- PROJECT FEATURE SCREENS ----------------------
// Prestige Team screen.
// Displays level, XP progression, and rank information.
void drawPrestigeScreen();

// Street Pass Team screen.
// Displays user information and greeting.
void drawStreetPassScreen();

// Refresh Settings submenu.
// Allows user selection between refresh modes.
void drawRefreshSettingsScreen();


// ---------------------- REFRESH FUNCTIONS ----------------------
// Experimental full refresh function.
// Used for testing full display updates.
// Currently disabled due to artifacts on hardware.
void drawScreenFullRefresh();

// Main display redraw function.
//
// Responsible for:
// 1. Clearing image buffer
// 2. Determining current screen state
// 3. Calling appropriate draw function
// 4. Sending image to e-paper display
void drawScreen();