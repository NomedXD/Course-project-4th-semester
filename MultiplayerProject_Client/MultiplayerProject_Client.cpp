#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include "map.h"
#include "view.h"

#include "NetworkClient.h"

using namespace sf;
using namespace std;

class Player { // ����� ������
public:
	float x, y, w, h, dx, dy, speed; //���������� ������ � � �, ������ ������, ��������� (�� � � �� �), ���� ��������
	int dir; //����������� (direction) �������� ������
	String File; //���� � �����������
	Image image;//���� �����������
	Texture texture;//���� ��������
	Sprite sprite, netGhost;//���� ������
	Text t;
	bool possesed = false;
	string name;
	bool turned = false;
public:
	Player(float X, float Y, float W, float H, bool possesed = false) :possesed(possesed) { 
		dx = 0; dy = 0; speed = 1; dir = 0;
	//	File = F;
		w = W; h = H;
		x = X; y = Y;
		sprite.setTextureRect(IntRect(0, 0, w, h));
		
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
		interactionWithMap();//�������� �������, ���������� �� �������������� � ������
	};

	void interactionWithMap()//�-��� �������������� � ������
	{

		for (int i = y / 42; i < (y + h) / 42; i++)//���������� �� ��������, �������������� � �������, �� ���� �� ���� ����������� ������� 32*32, ������� �� ���������� � 9 �����. ��� ������� ������� ����.
			for (int j = x / 42; j < (x + w) / 42; j++)//��� ����� �� 32, ��� ����� �������� ����� ���������, � ������� �������� �������������. (�� ���� ������ ������� 32*32, ������� ����� ������������ ������ �� ���������� ���������). � j<(x + w) / 32 - ������� ����������� ��������� �� ����. �� ���� ���������� ������ ������� ��������, ������� ������������� � ����������. ����� ������� ���� � ����� ����� ������� �� ����, ������� �� �� ������ �������� (���������������� � ������), �� ������� �������� (���������������� � ������)
			{
				if (TileMap[i][j] == '0')//���� ��� ��������� ������������� ������� 0 (�����), �� ��������� "����������� ��������" ���������:
				{
					if (dy > 0)//���� �� ��� ����,
					{
						y = i * 42 - h;//�� �������� ���������� ����� ���������. ������� �������� ���������� ������ ���������� �� �����(�����) � ����� �������� �� ������ ������� ���������.
					}
					if (dy < 0)
					{
						y = i * 42 + 42;//���������� � ������� �����. dy<0, ������ �� ���� ����� (���������� ���������� ������)
					}
					if (dx > 0)
					{
						x = j * 42 - w;//���� ���� ������, �� ���������� � ����� ����� (������ 0) ����� ������ ���������
					}
					if (dx < 0)
					{
						x = j * 42 + 42;//���������� ���� �����
					}
				}

				if (TileMap[i][j] == 's') { //���� ������ ����� 's' (������)
					x = 300; y = 300;//����� �� ��������... �������� ������������ �����
					TileMap[i][j] = ' ';//������� ������, ���� ����� �����. ����� � �� �������, ������.
				}
			}
	}
	void load(Texture& texture, Font& font)
	{
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, w, h));
		//body.setScale(2, 2);
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

};







vector<Player> playersVec;

Clock cycleTimer;
Time cycleTime;

IpAddress S_Ip;
unsigned short S_port;
string clientName;

NetworkClient netC;

void getUserInputData(string& playerName);
void addPlayer(Texture& t_player, Font& font, string clientName);

int main()
{
	RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");

	Image playerImage;
	playerImage.loadFromFile("D:/MultiPlayerProject-master/MultiplayerProject_Client/Debug/images/tank.png");
	playerImage.createMaskFromColor(sf::Color::White);
	Texture t_player;
	t_player.loadFromImage(playerImage);
	Player player(250, 250, 42, 42, true);
	view.reset(FloatRect(player.x, player.y, 1280, 720));

	Font font;
	font.loadFromFile("D:/MultiPlayerProject-master/MultiplayerProject_Client/Debug/fonts/Inkulinati-Regular.otf");
	getUserInputData(player.name);
	player.load(t_player,font);




	netC.init();
	netC.registerOnServer(S_Ip, S_port, player.name);

	vector<string> namesVec;
	netC.receiveConnectedClientsNames(namesVec);
	for (int i = 0; i < namesVec.size(); i++)
	{
		addPlayer(t_player,font, namesVec[i]);
	}

	Packet receivedDataPacket;
	Packet sendDataPacket;

	float CurrentFrame = 0;//������ ������� ����
	Clock clock;
	float time;

	Image map_image;//������ ����������� ��� �����
	map_image.loadFromFile("D:/MultiPlayerProject-master/MultiplayerProject_Client/Debug/images/tank.png");//��������� ���� ��� �����
	Texture map;//�������� �����
	map.loadFromImage(map_image);//�������� �������� ���������
	Sprite s_map;//������ ������ ��� �����
	s_map.setTexture(map);//�������� �������� ��������



	while (window.isOpen())
	{
		cycleTime = cycleTimer.restart();
		clock.restart();

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
								addPlayer(t_player,font, s);
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
							for (int i = 0; i < playersVec.size(); i++)
							{
								if (s == playersVec[i].name) {
									playersVec[i].turned = turn;
									playersVec[i].sprite.setPosition(x, y);
									playersVec[i].t.setPosition(x + playersVec[i].w / 2 - playersVec[i].t.getGlobalBounds().width / 2, y - playersVec[i].t.getGlobalBounds().height);
								}
							}
						}
					}
				}
			}
		}

		sendDataPacket.clear();
		sendDataPacket << "DATA" << player.x << player.y << player.turned;
		netC.sendData(sendDataPacket);




		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (window.hasFocus())
		{
			if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
				player.dir = 1; player.speed = 1;//dir =1 - ����������� �����, speed =0.1 - �������� ��������. �������� - ����� �� ��� ����� �� �� ��� �� �������� � ����� �� ���������� ������ ���
				player.sprite.setTextureRect(IntRect(42 * 0, 0, 42, 42)); //����� ������ p ������ player ������ ������, ����� �������� (��������� �������� �����)
			}

			if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
				player.dir = 0; player.speed = 1;//����������� ������, �� ����
				player.sprite.setTextureRect(IntRect(42 * 2, 0, 42, 42));  //����� ������ p ������ player ������ ������, ����� �������� (��������� �������� �����)
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
				player.dir = 3; player.speed = 1;//����������� ������, �� ����
				player.sprite.setTextureRect(IntRect(42 * 1, 0, 42, 42));  //����� ������ p ������ player ������ ������, ����� �������� (��������� �������� �����)
			}

			if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) { //���� ������ ������� ������� ����� ��� ���� ����� �
				player.dir = 2; player.speed = 1;//����������� ������, �� ����
				player.sprite.setTextureRect(IntRect(96 * 3, 0, 42, 42)); //���������� �� ����������� �. ���������� 96,96*2,96*3 � ����� 96
			}
		}
		getplayercoordinateforview(player.x, player.y);
		float time = clock.getElapsedTime().asMicroseconds();
		time = time / 500;
		player.update(time);
		//for (int i = 0; i < playersVec.size(); i++)
		//{
		//	if (player.getRect().intersects(playersVec[i].getRect())) {
		//		if (player.dy > 0)//���� �� ��� ����,
		//		{
		//			player.y = playersVec[i].y - player.h;//�� �������� ���������� ����� ���������. ������� �������� ���������� ������ ���������� �� �����(�����) � ����� �������� �� ������ ������� ���������.
		//		}
		//		if (player.dy < 0)
		//		{
		//			player.y = playersVec[i].y + player.h;//���������� � ������� �����. dy<0, ������ �� ���� ����� (���������� ���������� ������)
		//		}
		//		if (player.dx > 0)
		//		{
		//			player.x = playersVec[i].x - player.w;//���� ���� ������, �� ���������� � ����� ����� (������ 0) ����� ������ ���������
		//		}
		//		if (player.dx < 0)
		//		{
		//			player.x = playersVec[i].x +player.w;//���������� ���� �����
		//		}
		//	}
		//}

		window.setView(view);
		window.clear();
		/////////////////////////////������ �����/////////////////////
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 42*2, 42, 42)); //���� ��������� ������ ������, �� ������ 1� ���������
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(42*3, 42 * 2, 42, 42));//���� ��������� ������ s, �� ������ 2� ���������
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(42*2, 42 * 2, 42, 42));//���� ��������� ������ 0, �� ������ 3� ���������


				s_map.setPosition(j * 42, i * 42);//�� ���� ����������� ����������, ��������� � �����. �� ���� ������ ������� �� ��� �������. ���� ������, �� ��� ����� ���������� � ����� �������� 32*32 � �� ������ ���� �������

				window.draw(s_map);//������ ���������� �� �����
			}

		for (int i = 0; i < playersVec.size(); i++)
		{
			playersVec[i].draw(window);
		}

		player.draw(window);

		window.display();
	}
		

	return 0;
	
}

void getUserInputData(string& playerName)
{
	cout << "Enter server IP: ";//huytyttyt
	cin >> S_Ip;
	//S_Ip = "localhost";
	cout << endl;
	cout << "Enter server registration port: ";
	cin >> S_port;
	cout << endl;
	cout << "Enter name: ";
	cin >> playerName;
};

void addPlayer(Texture& t_player, Font& font, string clientName)
{
	Player p(250, 250, 42, 42, true);
	playersVec.push_back(p);
	playersVec.back().name = clientName;
	playersVec.back().load(t_player,font);
};
