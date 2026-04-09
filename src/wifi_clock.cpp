#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <cstring>
#include "wifi_clock.h"
#include "app_state.h"

const char* WIFI_SSID = "ATTxrTwUTA";
const char* WIFI_PASS = "awjf57s5tfnx";

// Stored clock state
static int currentHour = 0;
static int currentMinute = 0;
static bool isPM = false;

void connectWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting to WiFi");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;
        Serial.println();
        Serial.println("WiFi connected");
        Serial.println(WiFi.localIP());
    }
    else
    {
        wifiConnected = false;
        Serial.println();
        Serial.println("WiFi connection failed");
    }
}

bool fetchTimeFromHttp()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        wifiConnected = false;
        timeSynced = false;
        return false;
    }

    wifiConnected = true;

    HTTPClient http;

    // TimeAPI-style timezone request
    String url = "https://www.timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode != 200)
    {
        Serial.print("Time HTTP error: ");
        Serial.println(httpCode);
        http.end();
        timeSynced = false;
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial.print("Time JSON parse failed: ");
        Serial.println(error.c_str());
        timeSynced = false;
        return false;
    }

    int hour24 = doc["hour"] | -1;
    int minute = doc["minute"] | -1;

    if (hour24 < 0 || minute < 0)
    {
        timeSynced = false;
        return false;
    }

    currentMinute = minute;

    if (hour24 == 0)
    {
        currentHour = 12;
        isPM = false;
    }
    else if (hour24 < 12)
    {
        currentHour = hour24;
        isPM = false;
    }
    else if (hour24 == 12)
    {
        currentHour = 12;
        isPM = true;
    }
    else
    {
        currentHour = hour24 - 12;
        isPM = true;
    }

    timeSynced = true;
    Serial.println("Clock updated from HTTP");
    return true;
}

void getClockText(char timeBuffer[], char statusBuffer[])
{
    if (timeSynced)
    {
        sprintf(timeBuffer, "%d:%02d %s",
                currentHour,
                currentMinute,
                isPM ? "PM" : "AM");

        strcpy(statusBuffer, "HTTP clock active");
    }
    else if (wifiConnected)
    {
        strcpy(timeBuffer, "--:--");
        strcpy(statusBuffer, "Connected, no time yet");
    }
    else
    {
        strcpy(timeBuffer, "--:--");
        strcpy(statusBuffer, "WiFi not connected");
    }
}