#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "NetworkServer.h"

using namespace sf;
using namespace std;

NetworkServer netS;


int main()
{
	netS.init();
	

	Packet packet;
	packet << "DATA";
	for (int i = 0; i < netS.clientsVec.size(); i++)
	{
		packet << netS.clientsVec[i].name << netS.clientsVec[i].pos.x << netS.clientsVec[i].pos.y << 
			netS.clientsVec[i].turned << netS.clientsVec[i].direction << netS.clientsVec[i].curFrame <<
			netS.clientsVec[i].bulSize;
		for (int j = 0; j < netS.clientsVec[i].bulSize * 2; j++)
		{
			packet << netS.clientsVec[i].bulCoordVec.at(j);
		}
	}

	while (true)
	{
		netS.registerNewClients();
		netS.sendConnectedClientsRecords();

		if (netS.sendDataToAll(packet) == Socket::Status::Done)
		{
			packet.clear();
			packet << "DATA";
			for (int i = 0; i < netS.clientsVec.size(); i++)
			{
				packet << netS.clientsVec[i].name << netS.clientsVec[i].pos.x << netS.clientsVec[i].pos.y << 
					netS.clientsVec[i].turned << netS.clientsVec[i].direction << netS.clientsVec[i].curFrame<<
					netS.clientsVec[i].bulSize;
				for (int j = 0; j < netS.clientsVec[i].bulSize * 2; j++) // ÏÎÑÌÎÒÐÅÒÜ, ÈÍÄÅÊÑÛ I ÑÎÂÏÀÄÀÞÒ â öèêëàõ!!!!!!!
				{
					packet << netS.clientsVec[i].bulCoordVec.at(j);
				}
			}
		}


		unsigned int receivedClientIndex;
		if (netS.receiveData(receivedClientIndex) == Socket::Status::Done)
		{
			if (netS.clientsVec[receivedClientIndex].rDataPacket.getDataSize() > 0)
			{
				string s;
				if (netS.clientsVec[receivedClientIndex].rDataPacket >> s)
				{
					if (s == "DATA")
					{
						float x, y;
						if (netS.clientsVec[receivedClientIndex].rDataPacket >> x)
						{
							netS.clientsVec[receivedClientIndex].pos.x = x;
						}
						if (netS.clientsVec[receivedClientIndex].rDataPacket >> y)
						{
							netS.clientsVec[receivedClientIndex].pos.y = y;
						}
						bool turn;
						if (netS.clientsVec[receivedClientIndex].rDataPacket >> turn)
						{
							netS.clientsVec[receivedClientIndex].turned = turn;
						}
						int dn;
						if (netS.clientsVec[receivedClientIndex].rDataPacket >> dn)
						{
							netS.clientsVec[receivedClientIndex].direction = dn;
						}
						float frame;
						if (netS.clientsVec[receivedClientIndex].rDataPacket >> frame)
						{
							netS.clientsVec[receivedClientIndex].curFrame = frame;
						}
						int bulNum;
						if (netS.clientsVec[receivedClientIndex].rDataPacket >> bulNum)
						{
							netS.clientsVec[receivedClientIndex].bulSize = bulNum;
						}
						netS.clientsVec[receivedClientIndex].bulCoordVec.clear();
						for (int i = 0; i < bulNum; i++)
						{
							float tempBulX, tempBulY;
							if (netS.clientsVec[receivedClientIndex].rDataPacket >> tempBulX)
							{
								netS.clientsVec[receivedClientIndex].bulCoordVec.push_back(tempBulX);
							}
							if (netS.clientsVec[receivedClientIndex].rDataPacket >> tempBulY)
							{
								netS.clientsVec[receivedClientIndex].bulCoordVec.push_back(tempBulY);
							}
						}
						netS.clientsVec[receivedClientIndex].rDataPacket.clear();
					}
				}
			}
		}
	}


	getchar();
	return 0;
}
