#ifndef PONGGAME_H
#define PONGGAME_H

#include <Arduino.h>

void PongGame_init(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display);
void PongGame(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display);

#endif