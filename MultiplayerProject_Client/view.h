#pragma once;	
#include <SFML\Graphics.hpp>
using namespace sf;

sf::View view;//�������� sfml ������ "���", ������� � �������� �������
View getplayercoordinateforview(float x, float y) { //������� ��� ���������� ��������� ������

	float tempX = x; float tempY = y;//��������� ����� ������ � ��������� ��, ����� ������ ����

	if (x < 640) tempX = 640;//������� �� ���� ����� �������
	if (y < 360) tempY = 360;//������� �������
	if (y > 804) tempY = 804;//������ �������	

	view.setCenter(tempX, tempY); //������ �� �������, ��������� ��� ����������. 
	return view;
}