#pragma once

// ---------------------- BUTTON PIN CONFIGURATION ----------------------
// These constants define the GPIO pins connected to
// physical buttons on the CrowPanel.
// HOME button
// Returns user to Clock screen from anywhere.
#define HOME_KEY 2

// EXIT button
// Used to:
// - reset timer
// - leave refresh submenu
#define EXIT_KEY 1

// Previous button
// Navigates backward through screens or upward through submenu selections.
#define PRV_KEY  6

// Next button
// Navigates forward through screens
// or downward through submenu selections.
#define NEXT_KEY 4

// OK button
// Confirms selections or performs screen-specific actions.
#define OK_KEY   5

// ---------------------- DISPLAY POWER ----------------------
// Controls power supplied to e-paper display.
#define EPD_POWER 7
