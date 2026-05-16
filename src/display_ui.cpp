#include <Arduino.h>
#include "EPD.h"
#include "display_ui.h"
#include "app_state.h"
#include "wifi_clock.h"
#include "timer_logic.h"
#include "weather_data.h"

// Main image buffer created in main.cpp.
// All UI drawing occurs here before updating the display.
extern uint8_t ImageBW[27200];


// ---------------------- REFRESH SETTINGS ----------------------
// Currently highlighted refresh option
int selectedRefreshOption = 0;

// Active refresh behavior:
// 0 = Full Refresh (Disabled)
// 1 = Partial Refresh (Default)
// 2 = Hold / No automatic redraw
int activeRefreshOption = 1; 

// Tracks whether user is actively editing refresh settings.
bool editingRefreshMenu = false;

// ---------------------- TEXT DRAWING HELPER ----------------------
// Generic helper function used by all screens.
// Parameters:
// x = horizontal position
// y = vertical position
// text = text to display
// size = font size
void drawLabel(int x, int y, const char text[], int size)
{
    EPD_ShowString(x, y, text, size, BLACK);
}

// ---------------------- CLOCK SCREEN ----------------------
// Displays:
// - HTTP time
// - connection status
// - navigation instructions
void drawClockScreen(){
    char timeBuffer[20];
    char statusBuffer[30];

    getClockText(timeBuffer, statusBuffer);

    drawLabel(360, 30, "CLOCK", 24);
    drawLabel(345, 90, timeBuffer, 24);
    drawLabel(315, 130, statusBuffer, 16);

    drawLabel(50, 195, "UP/DOWN = switch", 16);
    drawLabel(50, 210, "HOME = return", 16);
}

// ---------------------- WEATHER SCREEN ----------------------
// Displays:
// - Current conditions
// - Feels-like temperature
// - Humidity
// - Multi-day forecast
void drawWeatherScreen(){
    char lineBuffer[50];
    char hlBuffer[20];
    char precipBuffer[10];

    drawLabel(300, 12, "WEATHER / Riverside", 16);

    sprintf(lineBuffer, "%dF   Feels %dF   Hum %d%%", currentTemp, feelsLikeTemp, currentHumidity);
    drawLabel(300, 34, lineBuffer, 12);

    drawLabel(360, 60, currentCondition, 12);

    int startX = 105;
    int colWidth = 115;

    int yDay = 84;
    int yLabel = 110;
    int yHighLow = 132;
    int yPrecip = 154;

    for (int i = 1; i < 5; i++){
        int xSep = startX + (i * colWidth) - 35;

        drawLabel(xSep, 88, "|", 12);
        drawLabel(xSep, 108, "|", 12);
        drawLabel(xSep, 128, "|", 12);
        drawLabel(xSep, 148, "|", 12);
    }

    for (int i = 0; i < 5; i++){
        int x = startX + (i * colWidth);

        drawLabel(x, yDay, forecast[i].day, 16);
        drawLabel(x, yLabel, forecast[i].label, 12);

        sprintf(hlBuffer, "%d/%d", forecast[i].highTemp, forecast[i].lowTemp);
        drawLabel(x, yHighLow, hlBuffer, 12);

        sprintf(precipBuffer, "%d%%", forecast[i].precipPercent);
        drawLabel(x, yPrecip, precipBuffer, 12);
    }

    drawLabel(50, 195, "UP/DOWN = switch", 16);
    drawLabel(50, 210, "HOME = return", 16);
}

// ---------------------- TIMER SCREEN ----------------------
// Stopwatch display.
// Timer continues internally even when Hold pauses redraws.
void drawTimerScreen(){
    char timerText[10];

    unsigned long shownTime = timerElapsed;

    if (timerRunning){
        shownTime = millis() - timerStartTime;
    }

    formatTimer(timerText, shownTime);

    drawLabel(360, 30, "TIMER", 24);
    drawLabel(360, 75, timerText, 24);

    if (timerRunning)
        drawLabel(350, 120, "RUNNING", 24);
    else
        drawLabel(350, 120, "STOPPED", 24);

    drawLabel(50, 195, "PRESS = start/pause", 16);
    drawLabel(50, 210, "EXIT = reset", 16);
}


// ---------------------- PRESTIGE SCREEN ----------------------
// Displays:
// Level
// XP
// Rank
// XP progress bar
void drawPrestigeScreen(){
    int currentXP = 2750;
    int requiredXP = 5000;
    int level = 12;

    int percent = (currentXP * 100) / requiredXP;
    int filledBars = (currentXP * 10) / requiredXP;

    char xpBuffer[30];
    char percentBuffer[20];
    char levelBuffer[20];
    char xpBar[20];

    sprintf(levelBuffer, "Level: %d", level);
    sprintf(xpBuffer, "%d / %d XP", currentXP, requiredXP);
    sprintf(percentBuffer, "%d%% Complete", percent);

    xpBar[0] = '[';

    for (int i = 0; i < 10; i++){
        if (i < filledBars)
            xpBar[i + 1] = '#';
        else
            xpBar[i + 1] = '-';
    }

    xpBar[11] = ']';
    xpBar[12] = '\0';

    drawLabel(330, 20, "PRESTIGE", 24);

    drawLabel(280, 65, levelBuffer, 16);
    drawLabel(280, 90, "Experience:", 16);
    drawLabel(280, 112, xpBuffer, 16);

    drawLabel(280, 140, xpBar, 16);
    drawLabel(430, 140, percentBuffer, 16);

    drawLabel(280, 165, "Rank: Silver II", 16);

    drawLabel(50, 195, "UP/DOWN = switch", 16);
    drawLabel(50, 210, "HOME = return", 16);
}

// ---------------------- STREET PASS ----------------------
// Displays:
// User
// Major
// Location
// Greeting
void drawStreetPassScreen(){
    const char userName[] = "Dustin";
    const char userMajor[] = "EMSET";
    const char locationName[] = "CPP";
    const char greeting[] = "Welcome, traveler!";

    char nameBuffer[30];
    char majorBuffer[30];
    char locationBuffer[30];

    sprintf(nameBuffer, "Name: %s", userName);
    sprintf(majorBuffer, "Major: %s", userMajor);
    sprintf(locationBuffer, "Location: %s", locationName);

    drawLabel(315, 20, "STREET PASS", 24);

    drawLabel(260, 65, nameBuffer, 16);
    drawLabel(260, 90, majorBuffer, 16);
    drawLabel(260, 115, locationBuffer, 16);

    drawLabel(240, 150, greeting, 16);

    drawLabel(50, 195, "UP/DOWN = switch", 16);
    drawLabel(50, 210, "HOME = return", 16);
}

// ---------------------- REFRESH SETTINGS ----------------------
// Allows selection:
// Full Refresh (Disabled)
// Partial Refresh
// Hold (no screen refresh)
void drawRefreshSettingsScreen(){
    drawLabel(285, 20, "REFRESH RATE", 24);
    drawLabel(245, 65, "Selectable Refresh Options", 16);

    if (selectedRefreshOption == 0)
        drawLabel(260, 95, "> Full Refresh (Disabled)", 16);
    else
        drawLabel(260, 95, "  Full Refresh (Disabled)", 16);

    if (selectedRefreshOption == 1)
        drawLabel(260, 120, "> Partial Refresh", 16);
    else
        drawLabel(260, 120, "  Partial Refresh", 16);

    if (selectedRefreshOption == 2)
        drawLabel(260, 145, "> Hold / No Redraw", 16);
    else
        drawLabel(260, 145, "  Hold / No Redraw", 16);

    char activeMode[30];

    if (activeRefreshOption == 0)
        sprintf(activeMode, "Active: Full (Disabled)");
    else if (activeRefreshOption == 1)
        sprintf(activeMode, "Active: Partial");
    else
        sprintf(activeMode, "Active: Hold");

    drawLabel(260, 170, activeMode, 16);

    if (!editingRefreshMenu)
    {
        drawLabel(50, 195, "OK = edit options", 16);
        drawLabel(50, 210, "UP/DOWN = switch", 16);
    }
    else
    {
        drawLabel(50, 180, "OK = select", 16);
        drawLabel(50, 195, "EXIT = leave menu", 16);
        drawLabel(50, 210, "UP/DOWN = change", 16);
    }
}

// ---------------------- MAIN REDRAW FUNCTION ----------------------
// Responsibilities:
// 1. Clear image buffer
// 2. Determine active screen
// 3. Draw appropriate screen
// 4. Send image to display
// Partial refresh is used because it produced the most stable behavior.
void drawScreen(){
    Paint_NewImage(ImageBW, EPD_W, EPD_H, Rotation, WHITE);
    Paint_Clear(WHITE);

    if (currentScreen == DisplayScreenState::Clock)
        drawClockScreen();
    else if (currentScreen == DisplayScreenState::Weather)
        drawWeatherScreen();
    else if (currentScreen == DisplayScreenState::Timer)
        drawTimerScreen();
    else if (currentScreen == DisplayScreenState::Prestige)
        drawPrestigeScreen();
    else if (currentScreen == DisplayScreenState::StreetPass)
        drawStreetPassScreen();
    else if (currentScreen == DisplayScreenState::RefreshSettings)
        drawRefreshSettingsScreen();

    EPD_Display(ImageBW);
    EPD_PartUpdate();
}

// ---------------------- FULL REFRESH ----------------------
//
// Investigated during project development.
//
// Produced:
// - ghosting
// - text stacking
// - unstable redraw behavior
//
// Disabled in final implementation.
void drawScreenFullRefresh(){
    Paint_NewImage(ImageBW, EPD_W, EPD_H, Rotation, WHITE);
    Paint_Clear(WHITE);

    if (currentScreen == DisplayScreenState::Clock)
        drawClockScreen();
    else if (currentScreen == DisplayScreenState::Weather)
        drawWeatherScreen();
    else if (currentScreen == DisplayScreenState::Timer)
        drawTimerScreen();
    else if (currentScreen == DisplayScreenState::Prestige)
        drawPrestigeScreen();
    else if (currentScreen == DisplayScreenState::StreetPass)
        drawStreetPassScreen();
    else if (currentScreen == DisplayScreenState::RefreshSettings)
        drawRefreshSettingsScreen();

    EPD_Display_Clear();
    EPD_Update();

    delay(300);

    EPD_Display(ImageBW);
    EPD_Update();
}