#include <Arduino.h>
#include "EPD.h"
#include "config.h"
#include "app_state.h"
#include "display_ui.h"
#include "wifi_clock.h"
#include "timer_logic.h"
#include "weather_data.h"

// These variables are defined in display_ui.cpp.
// main.cpp uses them to control the refresh settings submenu.
extern int activeRefreshOption;
extern int selectedRefreshOption;
extern bool editingRefreshMenu;

// Main black-and-white image buffer for the e-paper display.
// All screen drawing is written into this buffer before being sent to the display.
uint8_t ImageBW[27200];

// Moves forward through the screen state machine.
// This controls the order of screens when the NEXT button is pressed.
void goToNextScreen(){
    if (currentScreen == DisplayScreenState::Clock)
        currentScreen = DisplayScreenState::Weather;
    else if (currentScreen == DisplayScreenState::Weather)
        currentScreen = DisplayScreenState::Timer;
    else if (currentScreen == DisplayScreenState::Timer)
        currentScreen = DisplayScreenState::Prestige;
    else if (currentScreen == DisplayScreenState::Prestige)
        currentScreen = DisplayScreenState::StreetPass;
    else if (currentScreen == DisplayScreenState::StreetPass)
        currentScreen = DisplayScreenState::RefreshSettings;
    else
        currentScreen = DisplayScreenState::Clock;
}

// Moves backward through the screen state machine.
// This gives the user two-way navigation through the project screens.
void goToPreviousScreen(){
    if (currentScreen == DisplayScreenState::Clock)
        currentScreen = DisplayScreenState::RefreshSettings;
    else if (currentScreen == DisplayScreenState::Weather)
        currentScreen = DisplayScreenState::Clock;
    else if (currentScreen == DisplayScreenState::Timer)
        currentScreen = DisplayScreenState::Weather;
    else if (currentScreen == DisplayScreenState::Prestige)
        currentScreen = DisplayScreenState::Timer;
    else if (currentScreen == DisplayScreenState::StreetPass)
        currentScreen = DisplayScreenState::Prestige;
    else
        currentScreen = DisplayScreenState::StreetPass;
}

void setup(){
    Serial.begin(115200);
    Serial.println("STARTED");

    // Power on the e-paper display.
    pinMode(EPD_POWER, OUTPUT);
    digitalWrite(EPD_POWER, HIGH);

    // Set up all button inputs.
    // INPUT_PULLUP means the button reads LOW when pressed.
    pinMode(HOME_KEY, INPUT_PULLUP);
    pinMode(EXIT_KEY, INPUT_PULLUP);
    pinMode(PRV_KEY, INPUT_PULLUP);
    pinMode(NEXT_KEY, INPUT_PULLUP);
    pinMode(OK_KEY, INPUT_PULLUP);

    // Initialize e-paper display hardware and drawing buffer.
    EPD_GPIOInit();
    Paint_NewImage(ImageBW, EPD_W, EPD_H, Rotation, WHITE);
    Paint_Clear(WHITE);

    // Initial display setup and screen clear.
    EPD_FastMode1Init();
    EPD_Display_Clear();
    EPD_Update();
    EPD_Clear_R26A6H();

    // Load placeholder weather first.
    // If WiFi or HTTP fails, the screen still has fallback data to display.
    loadMockWeatherData();

    // Connect to WiFi and fetch live HTTP data.
    connectWiFi();
    fetchTimeFromHttp();
    fetchWeatherData();

    // Draw the first screen after setup is complete.
    drawScreen();
}

void loop(){
    static unsigned long lastClockRefresh = 0;
    static unsigned long lastWeatherRefresh = 0;
    static unsigned long lastTimerRefresh = 0;

    // Automatically refresh the HTTP clock every 60 seconds.
    // Hold mode pauses automatic redraws, but the time can still update internally.
    if (millis() - lastClockRefresh >= 60000){
        lastClockRefresh = millis();

        if (fetchTimeFromHttp()){
            if (currentScreen == DisplayScreenState::Clock && activeRefreshOption != 2){
                drawScreen();
            }
        }
    }

    // Automatically refresh weather every 10 minutes.
    // This avoids constantly calling the weather API.
    if (millis() - lastWeatherRefresh >= 600000){
        lastWeatherRefresh = millis();

        if (fetchWeatherData()){
            if (currentScreen == DisplayScreenState::Weather && activeRefreshOption != 2){
                drawScreen();
            }
        }
    }

    // HOME always returns the user to the Clock screen.
    if (digitalRead(HOME_KEY) == 0){
        currentScreen = DisplayScreenState::Clock;
        editingRefreshMenu = false;

        fetchTimeFromHttp();
        drawScreen();
        delay(300);
    }

    // EXIT resets the timer when on the Timer screen.
    // On the Refresh Settings screen, EXIT leaves edit mode.
    if (digitalRead(EXIT_KEY) == 0){
        if (currentScreen == DisplayScreenState::Timer){
            resetTimer();
        }

        if (currentScreen == DisplayScreenState::RefreshSettings){
            editingRefreshMenu = false;
        }

        drawScreen();
        delay(300);
    }

    // Previous button behavior:
    // - Normal mode: move to the previous screen
    // - Refresh edit mode: move upward through refresh options
    if (digitalRead(PRV_KEY) == 0){
        if (currentScreen == DisplayScreenState::RefreshSettings && editingRefreshMenu){
            selectedRefreshOption--;

            if (selectedRefreshOption < 0)
                selectedRefreshOption = 2;
        }
        else{
            goToPreviousScreen();
        }

        drawScreen();
        delay(300);
    }

    // Next button behavior:
    // - Normal mode: move to the next screen
    // - Refresh edit mode: move downward through refresh options
    if (digitalRead(NEXT_KEY) == 0){
        if (currentScreen == DisplayScreenState::RefreshSettings && editingRefreshMenu){
            selectedRefreshOption++;

            if (selectedRefreshOption > 2)
                selectedRefreshOption = 0;
        }
        else{
            goToNextScreen();
        }

        drawScreen();
        delay(300);
    }

    // OK button performs the main action for the current screen.
    if (digitalRead(OK_KEY) == 0){
        if (currentScreen == DisplayScreenState::RefreshSettings){
            // First OK press enters refresh option edit mode.
            if (!editingRefreshMenu){
                editingRefreshMenu = true;
            }

            // Second OK press saves the selected refresh option.
            else{
                activeRefreshOption = selectedRefreshOption;
                editingRefreshMenu = false;

                if (activeRefreshOption == 0){
                    // Full refresh was tested but caused artifacts on this 5.79" display.
                    // It is shown as disabled and automatically returns to Partial mode.
                    activeRefreshOption = 1;
                    selectedRefreshOption = 1;
                }
            }
        }
        else if (currentScreen == DisplayScreenState::Timer){
            toggleTimer();
        }
        else if (currentScreen == DisplayScreenState::Weather){
            fetchWeatherData();
        }
        else if (currentScreen == DisplayScreenState::Clock){
            fetchTimeFromHttp();
        }

        drawScreen();
        delay(300);
    }

    // Timer screen refreshes once per second while running.
    // Hold mode allows the timer to keep running internally while pausing visual updates.
    if (currentScreen == DisplayScreenState::Timer && timerRunning){
        if (millis() - lastTimerRefresh >= 1000){
            lastTimerRefresh = millis();

            if (activeRefreshOption != 2){
                drawScreen();
            }
        }
    }
}