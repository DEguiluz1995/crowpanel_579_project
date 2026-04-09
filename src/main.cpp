#include <Arduino.h>
#include "EPD.h"
#include "config.h"
#include "app_state.h"
#include "display_ui.h"
#include "wifi_clock.h"
#include "timer_logic.h"
#include "weather_data.h"

uint8_t ImageBW[27200];

void goToNextScreen()
{
    if (currentScreen == DisplayScreenState::Clock)
        currentScreen = DisplayScreenState::Weather;
    else if (currentScreen == DisplayScreenState::Weather)
        currentScreen = DisplayScreenState::Timer;
    else
        currentScreen = DisplayScreenState::Clock;
}

void goToPreviousScreen()
{
    if (currentScreen == DisplayScreenState::Clock)
        currentScreen = DisplayScreenState::Timer;
    else if (currentScreen == DisplayScreenState::Weather)
        currentScreen = DisplayScreenState::Clock;
    else
        currentScreen = DisplayScreenState::Weather;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("STARTED");

    pinMode(EPD_POWER, OUTPUT);
    digitalWrite(EPD_POWER, HIGH);

    pinMode(HOME_KEY, INPUT_PULLUP);
    pinMode(EXIT_KEY, INPUT_PULLUP);
    pinMode(PRV_KEY, INPUT_PULLUP);
    pinMode(NEXT_KEY, INPUT_PULLUP);
    pinMode(OK_KEY, INPUT_PULLUP);

    EPD_GPIOInit();
    Paint_NewImage(ImageBW, EPD_W, EPD_H, Rotation, WHITE);
    Paint_Clear(WHITE);

    EPD_FastMode1Init();
    EPD_Display_Clear();
    EPD_Update();
    EPD_Clear_R26A6H();

    loadMockWeatherData();

    connectWiFi();
    fetchTimeFromHttp();
   // fetchWeatherData();

    drawScreen();
}

void loop()
{
    static unsigned long lastClockRefresh = 0;
    static unsigned long lastWeatherRefresh = 0;
    static unsigned long lastTimerRefresh = 0;

    // auto refresh clock every 60 seconds
    if (millis() - lastClockRefresh >= 60000)
    {
        lastClockRefresh = millis();

        if (fetchTimeFromHttp())
        {
            Serial.println("Clock auto-refreshed");

            if (currentScreen == DisplayScreenState::Clock)
            {
                drawScreen();
            }
        }
    }

    // auto refresh weather every 10 minutes
    /*
    if (millis() - lastWeatherRefresh >= 600000)
    {
        lastWeatherRefresh = millis();

        if (fetchWeatherData())
        {
            Serial.println("Weather auto-refreshed");

            if (currentScreen == DisplayScreenState::Weather)
            {
                drawScreen();
            }
        }
    }
*/
    if (digitalRead(HOME_KEY) == 0)
    {
        currentScreen = DisplayScreenState::Clock;
        fetchTimeFromHttp();
        drawScreen();
        delay(300);
    }

    if (digitalRead(EXIT_KEY) == 0)
    {
        if (currentScreen == DisplayScreenState::Timer)
        {
            resetTimer();
        }
        drawScreen();
        delay(300);
    }

    if (digitalRead(PRV_KEY) == 0)
    {
        goToPreviousScreen();
        drawScreen();
        delay(300);
    }

    if (digitalRead(NEXT_KEY) == 0)
    {
        goToNextScreen();
        drawScreen();
        delay(300);
    }

    if (digitalRead(OK_KEY) == 0)
    {
        if (currentScreen == DisplayScreenState::Timer)
        {
            toggleTimer();
        }
        else if (currentScreen == DisplayScreenState::Weather)
        {
            //fetchWeatherData();
        }
        else if (currentScreen == DisplayScreenState::Clock)
        {
            fetchTimeFromHttp();
        }

        drawScreen();
        delay(300);
    }

    if (currentScreen == DisplayScreenState::Timer && timerRunning)
    {
        if (millis() - lastTimerRefresh >= 1000)
        {
            lastTimerRefresh = millis();
            drawScreen();
        }
    }
}