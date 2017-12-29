#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
#include <inttypes.h>
/// <param name="power">The desired power of 10.</param>
/// <returns>Returns 10 to the power of 'power'. </returns>
uint32_t exp10(uint8_t power);

/// <summary>Converts text into an integer value.</summary>
/// <param name="text">A pointer to a string that is going to be used to find the value of a number string.</param>
/// <returns> Returns the integer representation of text as a uint16_t.</returns>
/// <remarks>The string is assumed to be a string containing only numbers.</remarks>
uint16_t textToInt(uint8_t *text);

/// <summary>The parity of a uint8_t number.<summary>
/// <param name="number">The number that is having it`s parity tested.</param>
/// <returns> Returns a 1 if even parity, while odd parity returns a 0.</returns>
uint8_t parity(uint8_t number);

/// <summary> Converts a uint8_t number into a string.</summary>
/// <param name="str">The string that the number will be placed in.</param>
/// <param name="num">The number that will be changed to a string.</param>
/// <remarks>Zeros out the current contents of str, assumes the number of digits in num will fit into str.</remarks>
void intToText(uint8_t *str,uint8_t num);
#endif // UTIL_H_INCLUDED
