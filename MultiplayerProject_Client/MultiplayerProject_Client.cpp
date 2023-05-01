#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include "map.h"
#include "view.h"
#include <list>

#include "NetworkClient.h"
/*
* коммит small fix является последним рабочим вариантам. Там реализовано два игрока путем удаления из вектора убитого врага. При этом нет
* проверки пересечения пули игрока из вектора игроков с еще одним другим игром. В этом случае, игра плохо работает с 3 и более игроками.
* В данной версии игроки не удаляются из вектора, вместо этого используются if с проверкой здоровья. Кроме того, это позволяет делать проверку 
* на пересечение других игроков между собой, то есть с 3-мя игроками в данном случае игра работает отлично.
*/
using namespace sf;
using namespace std;
class Bullet;
class Player { 
public:
	float x, y, w, h, dx, dy, speed; 
	int dir;
	int health;
	Sprite sprite, netGhost;
	Text t;
	bool possesed = false;
	string name;
	bool turned = false;
	bool isShoot;
	float currentFrame;
	bool FirstShoot;
	Clock timeShootC;
	int sendBulletSize;
	bool changeBulSize;
	std::list<Bullet*> bullets;
	std::list<Bullet*>::iterator it;
public:
	Player(float X, float Y, float W, float H, bool possesed = false) :possesed(possesed) { 
		dx = 0; dy = 0; speed = 0.08; dir = 3;
		w = W; h = H;
		x = X; y = Y;
		sprite.setTextureRect(IntRect(0, 0, w, h));
		currentFrame = 0;
		isShoot = false;
		FirstShoot = false;
		sendBulletSize = 0;
		changeBulSize = false;
		health = 100;
	};
	void update(float time)
	{
		switch (dir)
		{
		case 0: dx = speed; dy = 0; break;
		case 1: dx = -speed; dy = 0; break;
		case 2: dx = 0; dy = speed; break;
		case 3: dx = 0; dy = -speed; break;
		}

		x += dx * time;
		y += dy * time;

		speed = 0;
		sprite.setPosition(x, y);
		t.setPosition(x + w / 2 - t.getGlobalBounds().width / 2, y - t.getGlobalBounds().height);
		interactionWithMap();
	};

	void interactionWithMap()
	{

		for (int i = y / 42; i < (y + h) / 42; i++)
			for (int j = x / 42; j < (x + w) / 42; j++)
			{
				if (TileMap[i][j] == '0')
				{
					if (dy > 0)
					{
						y = i * 42 - h;
					}
					if (dy < 0)
					{
						y = i * 42 + 42;
					}
					if (dx > 0)
					{
						x = j * 42 - w;
					}
					if (dx < 0)
					{
						x = j * 42 + 42;
					}
				}

				if (TileMap[i][j] == 's') {
					x = 300; y = 300;
					TileMap[i][j] = ' ';
				}
			}
	}
	void load(Texture& texture, Font& font)
	{
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, w, h));
		if (!possesed) sprite.setColor(Color::Red);
		netGhost.setTexture(texture);
		//name = playerName;

		t.setFont(font);
		t.setString(name);
		t.setFillColor(sf::Color::Red);
		t.setPosition(w / 2 - t.getGlobalBounds().width / 2, y - t.getGlobalBounds().height);
	};
	bool isPossesed() { return possesed; };
	void draw(RenderWindow& window)
	{
		//window.draw(netGhost);
		window.draw(sprite);
		window.draw(t);
	};
	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}
	void drawVec(RenderWindow& window)
	{
		//window.draw(netGhost);
		switch (dir)
		{
		case 0: {sprite.setTextureRect(IntRect(42 * int(currentFrame), 42 * 3, 42, 42)); break; }
		case 1: {sprite.setTextureRect(IntRect(42 * int(currentFrame) + 42, 42 * 3, -42, 42));  break; }
		case 2: { sprite.setTextureRect(IntRect(42 * int(currentFrame), 42, 42, -42));  break; }
		case 3: {  sprite.setTextureRect(IntRect(42 * int(currentFrame), 0, 42, 42));  break; }
		}
		window.draw(sprite);
		window.draw(t);
	};

};


class Bullet{
public:
	int direction;
	float x, y, w, h, dx, dy, speed;
	Sprite sprite;
	Image BulletImage;
	Texture bulletTexture;
	bool life;
	Bullet(float X, float Y, int W, int H, int dir){
		x = X;
		y = Y;
		direction = dir;
		speed = 0.2;
		w = h = W;
		life = true;
		BulletImage.loadFromFile("images/tank.png");
		BulletImage.createMaskFromColor(sf::Color::White);
		bulletTexture.loadFromImage(BulletImage);
		sprite.setTexture(bulletTexture);
		sprite.setTextureRect(IntRect(42*7,84,42,42));
		sprite.setPosition(x, y);
	}
	FloatRect getRect() {
		return FloatRect(x + 16, y + 16, 9, 9);
	}


	void update(float time)
	{
		switch (direction)
		{
		case 1: dx = -speed; dy = 0;   break;
		case 0: dx = speed; dy = 0;   break;
		case 2: dx = 0; dy = speed;   break;
		case 3: dx = 0; dy = -speed;   break;
		}

		x += dx * time;
		y += dy * time;

		if (x <= 0) x = 1;
		if (y <= 0) y = 1;

		for (int i = 0; i < HEIGHT_MAP; i++) {
			for (int j = 0; j < WIDTH_MAP; j++) {
				
				if (TileMap[i][j] == '0' && getRect().intersects(FloatRect(j * 42, i * 42, 42, 42))) 
				{
					life = false;
				}
			}
		}
		
		sprite.setPosition(x, y);
	}

};

vector<Player> playersVec;

IpAddress S_Ip;
unsigned short S_port;
string clientName;

NetworkClient netC;

void getUserInputData(string& playerName);
void addPlayer(Texture& t_player, Font& font, string clientName, int spawnX, int spawnY);

int main()
{
	RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");

	Image playerImage;
	playerImage.loadFromFile("images/tank.png");
	playerImage.createMaskFromColor(sf::Color::White);
	Texture t_player;
	t_player.loadFromImage(playerImage);
	Player player(250, 250, 42, 42, true);
	view.reset(FloatRect(player.x, player.y, 1280, 720));

	Font font;
	font.loadFromFile("fonts/Inkulinati-Regular.otf");
	getUserInputData(player.name);
	player.load(t_player,font);

	netC.init();
	netC.registerOnServer(S_Ip, S_port, player.name);

	vector<string> namesVec;
	netC.receiveConnectedClientsNames(namesVec);
	for (int i = 0; i < namesVec.size(); i++)
	{
		int tempCoordY = 0;
		int tempCoordX = 0;
		switch (i)
		{
		case 0: {
			int tempCoordX = 250;
			int tempCoordY = 250;
			break;
		}
		case 1: {
			int tempCoordX = 250;
			int tempCoordY = 714;
			break;
		}
		case 2: {
			int tempCoordX = 1340;
			int tempCoordY = 250;
			break;
		}
		default:
			int tempCoordX = 250;
			int tempCoordY = 250;
			break;
		}
		addPlayer(t_player,font, namesVec[i], tempCoordX, tempCoordY);
	}
	switch (namesVec.size())
	{
	case 0: {
		player.x = 250;
		player.y = 250;
		view.reset(FloatRect(player.x, player.y, 1280, 720));
		break;
	}
	case 1: {
		player.x = 250;
		player.y = 714;
		view.reset(FloatRect(player.x, player.y, 1280, 720));
		break;
	}
	case 2: {
		player.x = 1340;
		player.y = 250;
		view.reset(FloatRect(player.x, player.y, 1280, 720));
		break;
	}
	default: {
		player.x = 1340;
		player.y = 714;
		view.reset(FloatRect(player.x, player.y, 1280, 720));
		break;
	}
	}


	Packet receivedDataPacket;
	Packet sendDataPacket;

	Clock clock;

	Image map_image;
	map_image.loadFromFile("images/tank.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);



	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;

		if (netC.receiveData(receivedDataPacket, S_Ip, S_port) == Socket::Status::Done)
		{
			if (receivedDataPacket.getDataSize() > 0)
			{
				string s;
				if (receivedDataPacket >> s)
				{
					if (s == "NEW")
					{
						if (receivedDataPacket >> s)
						{
							if (s != clientName)
							{
								int tempCoordX = 0;
								int tempCoordY = 0;
								switch (playersVec.size())
								{
								case 0: {
									tempCoordX = 250;
									tempCoordY = 714;
									break;
								}
								case 1: {
									tempCoordX = 1340;
									tempCoordY = 250;
									break;
								}
								case 2: {
									tempCoordX = 1340;
									tempCoordY = 714;
									break;
								}
								default:
									tempCoordX = 1340;
									tempCoordY = 714;
									break;
								}
								addPlayer(t_player,font, s, tempCoordX, tempCoordY);
								cout << "New player connected: " << playersVec.back().name << endl;
							}
						}
					}
					if (s == "DATA")
					{
						while (!receivedDataPacket.endOfPacket())
						{
							float x, y;
							receivedDataPacket >> s;
							receivedDataPacket >> x;
							receivedDataPacket >> y;
							bool turn;
							receivedDataPacket >> turn;
							int direction;
							receivedDataPacket >> direction;
							float curFrame;
							receivedDataPacket >> curFrame;
							int size;
							receivedDataPacket >> size;
							bool changeSize;
							receivedDataPacket >> changeSize;
							for (int i = 0; i < playersVec.size(); i++)
							{
								if (s == playersVec[i].name) {
									playersVec[i].turned = turn;
									playersVec[i].x = x;
									playersVec[i].y = y;

									playersVec[i].sprite.setPosition(x, y);
									playersVec[i].t.setPosition(x + playersVec[i].w / 2 - playersVec[i].t.getGlobalBounds().width / 2, y - playersVec[i].t.getGlobalBounds().height);
									playersVec[i].dir = direction;
									playersVec[i].currentFrame = curFrame;
									playersVec[i].sendBulletSize = size;
									playersVec[i].changeBulSize = changeSize;
								}

							}
						}
					}
				}
			}	
		}

		sendDataPacket.clear();
		sendDataPacket << "DATA" << player.x << player.y << player.turned << player.dir << player.currentFrame << player.bullets.size() <<
			player.changeBulSize;
		player.changeBulSize = false;
		netC.sendData(sendDataPacket);
		
		 

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (player.isShoot == true)
		{
			player.changeBulSize = true;
			player.isShoot = false;
			player.bullets.push_back(new Bullet(player.x, player.y, 42, 42, player.dir));
			//shoot.play();
			player.FirstShoot = true;
			player.timeShootC.restart();
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (playersVec[i].changeBulSize == true)
			{
				playersVec[i].bullets.push_back(new Bullet(playersVec[i].x, playersVec[i].y, 42, 42, playersVec[i].dir));
			}
		}

		if (window.hasFocus())
		{
			if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
				player.dir = 1; player.speed = 0.08;
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame)+42, 42*3, -42, 42));
			}
			
			if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
				player.dir = 0; player.speed = 0.08;
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame), 42 * 3, 42, 42));  
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
				player.dir = 3; player.speed = 0.08;
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame), 0, 42, 42));
			}

			if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) {
				player.dir = 2; player.speed = 0.08;
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame), 42, 42, -42)); 
			}
			if (Keyboard::isKeyPressed(Keyboard::Space) && (player.FirstShoot == false || player.timeShootC.getElapsedTime().asSeconds() > 0.5)) {
				player.isShoot = true;

			}
		}

		getplayercoordinateforview(player.x, player.y);
		for (player.it = player.bullets.begin(); player.it != player.bullets.end();)
		{
			Bullet* b = *player.it;
			b->update(time);
			if (b->life == false) { player.it = player.bullets.erase(player.it); delete b; }
			else player.it++;
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (playersVec[i].health > 0) {
				std::list<Bullet*>::iterator iterator;
				for (iterator = playersVec[i].bullets.begin(); iterator != playersVec[i].bullets.end();)
				{
					Bullet* b = *iterator;
					b->update(time);
					if (b->life == false) { iterator = playersVec[i].bullets.erase(iterator); delete b; }
					else iterator++;
				}
			}
		}
		player.update(time);
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (player.getRect().intersects(playersVec[i].getRect()) && playersVec[i].health >0) {
				if (player.dy > 0)
				{
					player.y = playersVec[i].y - player.h;
				}
				if (player.dy < 0)
				{
					player.y = playersVec[i].y + player.h;
				}
				if (player.dx > 0)
				{
					player.x = playersVec[i].x - player.w;
				}
				if (player.dx < 0)
				{
					player.x = playersVec[i].x +player.w;
				}
			}
		}

		window.setView(view);
		window.clear();

		for (int i = 0; i < HEIGHT_MAP; i++) 
		{
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 42 * 2, 42, 42));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(42 * 3, 42 * 2, 42, 42));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(42 * 2, 42 * 2, 42, 42));
				s_map.setPosition(j * 42, i * 42);
				window.draw(s_map);
			}
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (playersVec[i].health > 0) {
				std::list<Bullet*>::iterator iterator;
				for (iterator = playersVec[i].bullets.begin(); iterator != playersVec[i].bullets.end(); iterator++)
				{
					if ((*iterator)->getRect().intersects(player.getRect())) {
						(*iterator)->life = false;
						player.health -= 50;
					}
					for (int j = 0; j < i; j++) 
					{
						if ((*iterator)->getRect().intersects(playersVec[j].getRect()) && playersVec[j].health >0) {
							(*iterator)->life = false;
							playersVec[j].health -= 50;
						}
					}
					for (int j = i+1; j < playersVec.size(); j++)
					{
						if ((*iterator)->getRect().intersects(playersVec[j].getRect()) && playersVec[j].health > 0) {
							(*iterator)->life = false;
							playersVec[j].health -= 50;
						}
					}
				}
				
			}
			
		}
		for (player.it = player.bullets.begin(); player.it != player.bullets.end(); player.it++)
		{
			for (int i = 0; i < playersVec.size(); i++)
			{
				if (playersVec[i].health > 0) {
					if ((*player.it)->getRect().intersects(playersVec[i].getRect())) {
						(*player.it)->life = false;
						playersVec[i].health -= 50;
					}
				}
			}
		}
		//for (std::vector<Player>::iterator playersVecorIterator = playersVec.begin(); playersVecorIterator != playersVec.end();) {
		//	if ((*playersVecorIterator).health <=0) 
		//		{ 
		//			playersVecorIterator = playersVec.erase(playersVecorIterator); 
		//		}// если этот объект мертв, то удаляем его
		//	else playersVecorIterator++;
		//}
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (playersVec[i].health > 0) {

				std::list<Bullet*>::iterator iterator;
				for (iterator = playersVec[i].bullets.begin(); iterator != playersVec[i].bullets.end(); iterator++)
				{
					window.draw((*iterator)->sprite);
				}
				playersVec[i].drawVec(window);
			}
			
		}
		for (player.it = player.bullets.begin(); player.it != player.bullets.end(); player.it++) 
		{
			window.draw((*player.it)->sprite);
		}

		player.draw(window);
		if (player.health <= 0) {
			break;
		}

		window.display();
	}
	return 0;
}

void getUserInputData(string& playerName)
{
	cout << "Enter server IP: ";
	cin >> S_Ip;
	//S_Ip = "localhost";
	cout << endl;
	cout << "Enter server registration port: ";
	cin >> S_port;
	cout << endl;
	cout << "Enter name: ";
	cin >> playerName;
};

void addPlayer(Texture& t_player, Font& font, string clientName, int spawnX, int spawnY)
{
	Player p(spawnX, spawnY, 42, 42, true);
	playersVec.push_back(p);
	playersVec.back().name = clientName;
	playersVec.back().load(t_player,font);
};
