#pragma once;
#include <SFML\Graphics.hpp>
const int HEIGHT_MAP = 20;
const int WIDTH_MAP = 42;

sf::String TileMap[HEIGHT_MAP] = {
    "00000000000000000000000000000000000000000",
	"0                           0           0",
	"0  0 0   0                  0     0 0   0",
	"0  0 0                 000        0 0   0",
	"0  0 0          0             c   0 0   0",
	"0  0 0          0000000000        0 0   0",
	"0  0 0          0      0          0 0   0",
	"0  0                   0            0   0",
	"0           0                           0",
	"0   00      0                     00    0",
	"0                0     0                0",
	"0  0             0000000            0   0",
	"0  0 0           0     0          0 0   0",
	"0  0 0           0     0          0 0   0",
	"0  0 0     s        0             0 0   0",
	"0          0        0   0000            0",
	"0          000      0                   0",
	"0                   0        0          0",
	"0                                       0",
	"00000000000000000000000000000000000000000",
};