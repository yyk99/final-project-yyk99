/*
*/

#ifndef TEMPERATURE_UTILS_H
#define TEMPERATURE_UTILS_H

float convertCtoF(float c) { return c * 1.8 + 32; }
float convertFtoC(float f) { return (f - 32) * 0.55555; }

#endif
