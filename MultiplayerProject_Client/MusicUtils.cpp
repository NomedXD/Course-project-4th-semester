#include "MusicUtils.h"

void MusicUtils::initialize()
{
	static SoundBuffer buffer1;
	buffer1.loadFromFile("music/wall.wav");
	shootWall.setBuffer(buffer1);
	static SoundBuffer buffer2;
	buffer2.loadFromFile("music/teleport.wav");
	teleport.setBuffer(buffer2);
	static SoundBuffer buffer3;
	buffer3.loadFromFile("music/otherTank.wav");
	shootTank.setBuffer(buffer3);
	static SoundBuffer buffer4;
	buffer4.loadFromFile("music/getBullet.wav");
	getBullet.setBuffer(buffer4);
}
