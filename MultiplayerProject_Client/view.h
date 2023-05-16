#pragma once;	
#include <SFML\Graphics.hpp>
using namespace sf;

sf::View view;
View getplayercoordinateforview(float x, float y) { 

	float tempX = x; float tempY = y;

	if (x < 640) tempX = 640;
	if (x > 1082) tempX = 1082;
	if (y < 360) tempY = 360;
	if (y > 480) tempY = 480;	

	view.setCenter(tempX, tempY); 
	return view;
}