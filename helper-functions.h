#pragma once

/**
 * Creates a callback to the handlerFunc when Ctrl + C is pressed
 * \param handlerFunc A function pointer to the function which will be called.
 * Signature of handlerFunc must return `void` and take an `int` - the identifier
 * of the signal that was caught.
 * \return Nothing
 */
void setCtrlCHandler(void (*handlerFunc)(int));

/**
 * Pause execution of the program for at least the set amount of time
 * \param milliSeconds the number of milliseconds to sleep for
 */
void msleep(int milliSeconds);

/**
 * Get time in milliseconds since the epoch
 */
long getTimeMillis();

/**
 * Get time in microseconds since the epoch
 */
long getTimeMicros();

/**
 * Get time in nanoseconds since the epoch
 */
long getTimeNanos();