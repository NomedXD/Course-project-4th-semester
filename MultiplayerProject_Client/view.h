#pragma once;	
#include <SFML\Graphics.hpp>
using namespace sf;

sf::View view;//объ€вили sfml объект "вид", который и €вл€етс€ камерой
View getplayercoordinateforview(float x, float y) { //функци€ дл€ считывани€ координат игрока

	float tempX = x; float tempY = y;//считываем коорд игрока и провер€ем их, чтобы убрать кра€

	if (x < 640) tempX = 640;//убираем из вида левую сторону
	if (y < 360) tempY = 360;//верхнюю сторону
	if (y > 804) tempY = 804;//нижнюю сторону	

	view.setCenter(tempX, tempY); //следим за игроком, передава€ его координаты. 
	return view;
}