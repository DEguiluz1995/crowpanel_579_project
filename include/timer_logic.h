#pragma once

// ---------------------- TIMER FORMATTER ----------------------
//
// Converts elapsed milliseconds into a readable timer format.
void formatTimer(char buffer[], unsigned long ms);

// ---------------------- TIMER CONTROL ----------------------
// Starts or pauses the stopwatch.
void toggleTimer();

// ---------------------- TIMER RESET ----------------------
// Resets stopwatch state.
void resetTimer();