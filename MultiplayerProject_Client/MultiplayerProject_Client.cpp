#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include "map.h"
#include "view.h"
#include <list>

#include "NetworkClient.h"

using namespace sf;
using namespace std;
class Bullet;
class Player { // класс Игрока
public:
	float x, y, w, h, dx, dy, speed; //координаты игрока х и у, высота ширина, ускорение (по х и по у), сама скорость
	int dir; //направление (direction) движения игрока
	int health;
	Sprite sprite, netGhost;//сфмл спрайт
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
	//	File = F;
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
		interactionWithMap();//вызываем функцию, отвечающую за взаимодействие с картой
	};

	void interactionWithMap()//ф-ция взаимодействия с картой
	{

		for (int i = y / 42; i < (y + h) / 42; i++)//проходимся по тайликам, контактирующим с игроком, то есть по всем квадратикам размера 32*32, которые мы окрашивали в 9 уроке. про условия читайте ниже.
			for (int j = x / 42; j < (x + w) / 42; j++)//икс делим на 32, тем самым получаем левый квадратик, с которым персонаж соприкасается. (он ведь больше размера 32*32, поэтому может одновременно стоять на нескольких квадратах). А j<(x + w) / 32 - условие ограничения координат по иксу. то есть координата самого правого квадрата, который соприкасается с персонажем. таким образом идем в цикле слева направо по иксу, проходя по от левого квадрата (соприкасающегося с героем), до правого квадрата (соприкасающегося с героем)
			{
				if (TileMap[i][j] == '0')//если наш квадратик соответствует символу 0 (стена), то проверяем "направление скорости" персонажа:
				{
					if (dy > 0)//если мы шли вниз,
					{
						y = i * 42 - h;//то стопорим координату игрек персонажа. сначала получаем координату нашего квадратика на карте(стены) и затем вычитаем из высоты спрайта персонажа.
					}
					if (dy < 0)
					{
						y = i * 42 + 42;//аналогично с ходьбой вверх. dy<0, значит мы идем вверх (вспоминаем координаты паинта)
					}
					if (dx > 0)
					{
						x = j * 42 - w;//если идем вправо, то координата Х равна стена (символ 0) минус ширина персонажа
					}
					if (dx < 0)
					{
						x = j * 42 + 42;//аналогично идем влево
					}
				}

				if (TileMap[i][j] == 's') { //если символ равен 's' (камень)
					x = 300; y = 300;//какое то действие... например телепортация героя
					TileMap[i][j] = ' ';//убираем камень, типа взяли бонус. можем и не убирать, кстати.
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


class Bullet{//класс пули
public:
	int direction;//направление пули
	float x, y, w, h, dx, dy, speed; //координаты игрока х и у, высота ширина, ускорение (по х и по у), сама скорость
	int dir; //направление (direction) движения игрока
	Sprite sprite;
	Image BulletImage;//изображение для пули
	Texture bulletTexture;
	bool life;
	Bullet(float X, float Y, int W, int H, int dir){//всё так же, только взяли в конце состояние игрока (int dir)
		x = X;
		y = Y;
		direction = dir;
		speed = 0.2;
		w = h = W;
		life = true;
		BulletImage.loadFromFile("D:/MultiPlayerProject-master/MultiplayerProject_Client/Debug/images/tank.png");//загрузили картинку в объект изображения
		BulletImage.createMaskFromColor(sf::Color::White);//маска для пули по черному цвету
		bulletTexture.loadFromImage(BulletImage);
		sprite.setTexture(bulletTexture);
		sprite.setTextureRect(IntRect(42*7,84,42,42));
		sprite.setPosition(x + w / 2, y + h / 2);
	}
	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}


	void update(float time)
	{
		switch (direction)
		{
		case 1: dx = -speed; dy = 0;   break;//интовое значение state = left
		case 0: dx = speed; dy = 0;   break;//интовое значение state = right
		case 2: dx = 0; dy = speed;   break;//интовое значение state = down
		case 3: dx = 0; dy = -speed;   break;//интовое значение state = up
		}

		x += dx * time;//само движение пули по х
		y += dy * time;//по у

		if (x <= 0) x = 1;// задержка пули в левой стене, чтобы при проседании кадров она случайно не вылетела за предел карты и не было ошибки
		if (y <= 0) y = 1;

		for (int i = 0; i < HEIGHT_MAP; i++) {//проход по объектам solid
			for (int j = 0; j < WIDTH_MAP; j++) {
				
				if (TileMap[i][j] == '0' && getRect().intersects(FloatRect(j * 42, i * 42, 42, 42))) //если этот объект столкнулся с пулей,
				{
					life = false;// то пуля умирает
				}
			}
		}
		
		sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
	}

};

vector<Player> playersVec;

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
		addPlayer(t_player,font, namesVec[i]);
	}

	Packet receivedDataPacket;
	Packet sendDataPacket;

	Clock clock;

	Image map_image;//объект изображения для карты
	map_image.loadFromFile("..images/tank.png");//загружаем файл для карты
	Texture map;//текстура карты
	map.loadFromImage(map_image);//заряжаем текстуру картинкой
	Sprite s_map;//создаём спрайт для карты
	s_map.setTexture(map);//заливаем текстуру спрайтом



	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		// Перезагрузка времени
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

		// При условии, что был выстрел
		if (player.isShoot == true)
		{
			player.changeBulSize = true;
			// Создание объекта пули
			player.isShoot = false;
			player.bullets.push_back(new Bullet(player.x-21, player.y-21, 42, 42, player.dir));
			//shoot.play();
			player.FirstShoot = true;
			player.timeShootC.restart();
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (playersVec[i].changeBulSize == true)
			{
				playersVec[i].bullets.push_back(new Bullet(playersVec[i].x - 21, playersVec[i].y - 21, 42, 42, playersVec[i].dir));
			}
			
		}

		if (window.hasFocus())
		{
			if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
				player.dir = 1; player.speed = 0.08;//dir =1 - направление вверх, speed =0.1 - скорость движения. Заметьте - время мы уже здесь ни на что не умножаем и нигде не используем каждый раз
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame)+42, 42*3, -42, 42)); //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
			}
			
			if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
				player.dir = 0; player.speed = 0.08;//направление вправо, см выше
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame), 42 * 3, 42, 42));  //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
				player.dir = 3; player.speed = 0.08;//направление вправо, см выше
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame), 0, 42, 42));  //через объект p класса player меняем спрайт, делая анимацию (используя оператор точку)
			}

			if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) { //если нажата клавиша стрелка влево или англ буква А
				player.dir = 2; player.speed = 0.08;//направление вправо, см выше
				player.currentFrame += 0.005 * time;
				if (player.currentFrame > 8) player.currentFrame -= 8;
				player.sprite.setTextureRect(IntRect(42 * int(player.currentFrame), 42, 42, -42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
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
			if (b->life == false) { player.it = player.bullets.erase(player.it); delete b; }// если этот объект мертв, то удаляем его
			else player.it++;
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
			std::list<Bullet*>::iterator iterator;
			for (iterator = playersVec[i].bullets.begin(); iterator != playersVec[i].bullets.end();)
			{
				Bullet* b = *iterator;
				b->update(time);
				if (b->life == false) { iterator = playersVec[i].bullets.erase(iterator); delete b; }// если этот объект мертв, то удаляем его
				else iterator++;
			}
		}
		player.update(time);
		for (int i = 0; i < playersVec.size(); i++)
		{
			if (player.getRect().intersects(playersVec[i].getRect())) {
				if (player.dy > 0)//если мы шли вниз,
				{
					player.y = playersVec[i].y - player.h;//то стопорим координату игрек персонажа. сначала получаем координату нашего квадратика на карте(стены) и затем вычитаем из высоты спрайта персонажа.
				}
				if (player.dy < 0)
				{
					player.y = playersVec[i].y + player.h;//аналогично с ходьбой вверх. dy<0, значит мы идем вверх (вспоминаем координаты паинта)
				}
				if (player.dx > 0)
				{
					player.x = playersVec[i].x - player.w;//если идем вправо, то координата Х равна стена (символ 0) минус ширина персонажа
				}
				if (player.dx < 0)
				{
					player.x = playersVec[i].x +player.w;//аналогично идем влево
				}
			}
		}

		window.setView(view);
		window.clear();
		/////////////////////////////Рисуем карту/////////////////////
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 42*2, 42, 42)); //если встретили символ пробел, то рисуем 1й квадратик
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(42*3, 42 * 2, 42, 42));//если встретили символ s, то рисуем 2й квадратик
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(42*2, 42 * 2, 42, 42));//если встретили символ 0, то рисуем 3й квадратик


				s_map.setPosition(j * 42, i * 42);//по сути раскидывает квадратики, превращая в карту. то есть задает каждому из них позицию. если убрать, то вся карта нарисуется в одном квадрате 32*32 и мы увидим один квадрат

				window.draw(s_map);//рисуем квадратики на экран
			}
		for (int i = 0; i < playersVec.size(); i++)
		{
			std::list<Bullet*>::iterator iterator;
			for (iterator = playersVec[i].bullets.begin(); iterator != playersVec[i].bullets.end(); iterator++)
			{
				if ((*iterator)->getRect().intersects(player.getRect())) {
					(*iterator)->life = false;
					player.health -= 50;
				}
			}
			
		}
		for (player.it = player.bullets.begin(); player.it != player.bullets.end(); player.it++)
		{
			for (int i = 0; i < playersVec.size(); i++)
			{
				if ((*player.it)->getRect().intersects(playersVec[i].getRect())) {
					(*player.it)->life = false;
					playersVec[i].health -= 50;
				}
			}
		}
		for (std::vector<Player>::iterator playersVecorIterator = playersVec.begin(); playersVecorIterator != playersVec.end();) {
			if ((*playersVecorIterator).health <=0) 
				{ 
					playersVecorIterator = playersVec.erase(playersVecorIterator); 
				}// если этот объект мертв, то удаляем его
			else playersVecorIterator++;
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
				
				std::list<Bullet*>::iterator iterator;
				for (iterator = playersVec[i].bullets.begin(); iterator != playersVec[i].bullets.end(); iterator++)
				{
					window.draw((*iterator)->sprite);
				}
				playersVec[i].drawVec(window);
			
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
