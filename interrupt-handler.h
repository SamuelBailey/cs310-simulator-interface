#pragma once

/**
 * Creates a callback to the handlerFunc when Ctrl + C is pressed
 * \param handlerFunc A function pointer to the function which will be called.
 * Signature of handlerFunc must return `void` and take an `int` - the identifier
 * of the signal that was caught.
 * \return Nothing
 */
void setCtrlCHandler(void (*handlerFunc)(int));