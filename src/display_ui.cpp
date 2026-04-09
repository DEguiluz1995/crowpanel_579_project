#include <Arduino.h>
#include "EPD.h"
#include "display_ui.h"
#include "app_state.h"
#include "wifi_clock.h"
#include "timer_logic.h"
#include "weather_data.h"

extern uint8_t ImageBW[27200];

void drawLabel(int x, int y, const char text[], int size)
{
    EPD_ShowString(x, y, text, size, BLACK);
}

void drawClockScreen()
{
    char timeBuffer[20];
    char statusBuffer[30];

    getClockText(timeBuffer, statusBuffer);

    //title
    drawLabel(360, 30, "CLOCK", 24);
    //time
    drawLabel(345, 90, timeBuffer, 24);
    //status
    drawLabel(315, 130, statusBuffer, 16);
    drawLabel(50, 195, "UP/DOWN = switch", 16);
    drawLabel(50, 210, "HOME = return", 16);
}

void drawWeatherScreen()
{
    char lineBuffer[50];
    char hlBuffer[20];
    char precipBuffer[10];

    // Top section
    drawLabel(300, 12, "WEATHER / Riverside", 16);

    sprintf(lineBuffer, "%dF   Feels %dF   Hum %d%%",
            currentTemp, feelsLikeTemp, currentHumidity);
    drawLabel(300, 34, lineBuffer, 12);

    drawLabel(360, 60, currentCondition, 12);

    // Forecast row spread across the full screen
   int startX = 105;
int colWidth = 115;

int yDay = 84;
int yLabel = 110;
int yHighLow = 132;
int yPrecip = 154;

// vertical separators
for (int i = 1; i < 5; i++)
{
    int xSep = startX + (i * colWidth) - 35;

    drawLabel(xSep, 88, "|", 12);
    drawLabel(xSep, 108, "|", 12);
    drawLabel(xSep, 128, "|", 12);
    drawLabel(xSep, 148, "|", 12);
}

for (int i = 0; i < 5; i++)
{
    int x = startX + (i * colWidth);

    drawLabel(x, yDay, forecast[i].day, 16);
    drawLabel(x, yLabel, forecast[i].label, 12);

    sprintf(hlBuffer, "%d/%d", forecast[i].highTemp, forecast[i].lowTemp);
    drawLabel(x, yHighLow, hlBuffer, 12);

    sprintf(precipBuffer, "%d%%", forecast[i].precipPercent);
    drawLabel(x, yPrecip, precipBuffer, 12);
}
}

void drawTimerScreen()
{
    char timerText[10];

    unsigned long shownTime = timerElapsed;
    if (timerRunning)
    {
        shownTime = millis() - timerStartTime;
    }

    formatTimer(timerText, shownTime);
    // Title
    drawLabel(360, 30, "TIMER", 24);
    // Timer
    drawLabel(360, 75, timerText, 24);
    // Status
    if (timerRunning)
        drawLabel(350, 120, "RUNNING", 24);
    else
        drawLabel(350, 120, "STOPPED", 24);
    //Controls
    drawLabel(50, 195, "PRESS = start/pause", 16);
    drawLabel(50, 210, "EXIT = reset", 16);
}

void drawScreen()
{
    Paint_NewImage(ImageBW, EPD_W, EPD_H, Rotation, WHITE);
    Paint_Clear(WHITE);

    if (currentScreen == DisplayScreenState::Clock)
        drawClockScreen();
    else if (currentScreen == DisplayScreenState::Weather)
        drawWeatherScreen();
    else if (currentScreen == DisplayScreenState::Timer)
        drawTimerScreen();

    EPD_Display(ImageBW);
    EPD_PartUpdate();
}