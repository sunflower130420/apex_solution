#pragma once

#include <windows.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <list>
#include <algorithm>
#include "math.h"
#include "overlay.h"
#include "vector.h"
ImColor teamColor[] = {
	ImColor(0, 0, 0),          // 0: Doesn't exit 
	ImColor(0, 0, 0),          // 0: Spectator
	ImColor(17, 124, 141),     // 1: Tealish
	ImColor(33, 71, 103),      // 2: Dark Blue
	ImColor(31, 84, 205),      // 3: Blue
	ImColor(68, 42, 96),       // 4: Light Purple
	ImColor(110, 44, 111),     // 5: Light Green
	ImColor(173, 45, 119),     // 6: Light Magenta
	ImColor(175, 30, 81),      // 7: Green
	ImColor(194, 5, 15),       // 8: Red
	ImColor(197, 67, 32),      // 9: Pink
	ImColor(119, 32, 22),      // 10: Brownish Orange
	ImColor(158, 60, 16),      // 11: Orange
	ImColor(118, 75, 5),       // 12: Olive Green
	ImColor(170, 101, 34),     // 13: Yellowish Orange
	ImColor(150, 125, 0),      // 14: Yellow-Green
	ImColor(133, 147, 10),     // 15: Light Green
	ImColor(73, 88, 3),        // 16: Dark Green
	ImColor(107, 143, 66),     // 17: Light Olive
	ImColor(59, 131, 57),      // 18: Olive
	ImColor(47, 90, 26),       // 19: Brown
	ImColor(5, 116, 88),       // 20: Purple
	ImColor(110, 50, 6),       // 21: Bluish Green
	ImColor(14, 88, 106),      // 22: Sky Blue
	ImColor(10, 130, 148),     // 23: Bright Green
	ImColor(27, 71, 105),      // 24: Pale Blue
	ImColor(38, 84, 195),      // 25: Bright Blue
	ImColor(70, 48, 96),       // 26: Purple
	ImColor(110, 44, 111),     // 27: Light Green
	ImColor(173, 45, 119),     // 28: Light Magenta
	ImColor(176, 28, 81),      // 29: Light Red
	ImColor(195, 0, 11)        // 30: Bright Red
};

bool armorbaseglow = false;
bool vischeck_glow = false;