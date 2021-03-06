#include "GSNetworkConnector.hpp"

#include "GSNetworkGame.hpp"
#include "GSMenu.hpp"
#include "GSError.hpp"

#include <sstream>



GSNetworkConnector::GSNetworkConnector(sf::RenderWindow& window, Settings& settings, Resourcemanager* resourcemanager) :
GameState(window, settings), Gui(window, settings, resourcemanager), myResourcemanager(resourcemanager),
myMenupoint(networkconnector::NAME), myMenustatus(networkconnector::CHOOSE), mySocket(NULL)
{

}


GSNetworkConnector::~GSNetworkConnector()
{
	delete myResourcemanager;
	delete mySocket;
}



void GSNetworkConnector::OnEnter()
{
	//try
	{
		Gui::LoadResources();
	}
	
	myNameString = GameState::mySettings.GetSettings("Game", "Name");
	myIpString = GameState::mySettings.GetSettings("Game", "Ip");
	myPortString = GameState::mySettings.GetSettings("Game", "Port");
	
	Gui::SetMenupointText(0, "Name: " + myNameString);
	Gui::SetMenupointText(1, "Ip: " + myIpString);
	Gui::SetMenupointText(2, "Port: " + myPortString);
	Gui::SetMenupointText(3, "Connect");
	Gui::SetMenupointText(4, "Main Menu");
	
	myNextStatus = CONTINUE;
	Gui::FadeIn();
}



Status GSNetworkConnector::Update()
{
	while(GameState::myWindow.pollEvent(myEvent))
	{
		if(myEvent.type == sf::Event::Closed)
		{
			return QUIT;
		}
		
		if(myMenustatus != networkconnector::EDIT)
		{
			if(myEvent.type == sf::Event::KeyPressed && myEvent.key.code == sf::Keyboard::Escape)
			{
				myNextState = new GSMenu(GameState::myWindow, GameState::mySettings, myResourcemanager);
				myResourcemanager = NULL;
			
				myNextStatus = NEXTSTATE;
			}
			else
			{
				Gui::CheckEvents(myEvent);
			}
		}
		else
		{
			if(myEvent.type == sf::Event::KeyPressed && (myEvent.key.code == sf::Keyboard::Escape || myEvent.key.code == sf::Keyboard::Return))
			{
				myMenustatus = networkconnector::CHOOSE;
			}
			else
			{
				Edit();
				
				if(myMenupoint == networkconnector::NAME)
				{
					Gui::SetMenupointText(0, "Name: " + *editstr);
				}
				else if(myMenupoint == networkconnector::IP)
				{
					Gui::SetMenupointText(1, "Ip: " + *editstr);
				}
				else if(myMenupoint == networkconnector::PORT)
				{
					Gui::SetMenupointText(2, "Port: " + *editstr);
				}
			}
		}
	}
	
	return myNextStatus;
}


void GSNetworkConnector::Render()
{
	Gui::Render();
}



GameState* GSNetworkConnector::OnLeave()
{
	return myNextState;
}



void GSNetworkConnector::Edit()
{
	if(myEvent.type == sf::Event::KeyPressed)
	{
		if(myEvent.key.code == sf::Keyboard::Back && myCurserPosition > 0)
		{
			editstr->erase(--myCurserPosition, 1);
		}
		else if(myEvent.key.code == sf::Keyboard::Left && myCurserPosition > 0)
		{
			--myCurserPosition;
		}
		else if(myEvent.key.code == sf::Keyboard::Right && editstr->length() > myCurserPosition)
		{
			++myCurserPosition;
		}
		else if(myEvent.key.code == sf::Keyboard::Up)
		{
			myCurserPosition = 0;
		}
		else if(myEvent.key.code == sf::Keyboard::Down)
		{
			myCurserPosition = editstr->length();
		}	
	}
	else if(myEvent.type == sf::Event::TextEntered && myEvent.text.unicode != 13 && myEvent.text.unicode != 8)
	{
		if(myMenupoint == networkconnector::PORT)
		{
			switch(static_cast<char>(myEvent.text.unicode))
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					myPortString.insert(myCurserPosition, std::size_t(1), static_cast<char>(myEvent.text.unicode));
					++myCurserPosition;
				break;
			}
		}
		else
		{
			editstr->insert(myCurserPosition, std::size_t(1), static_cast<char>(myEvent.text.unicode));
			++myCurserPosition;
		}
	}
}




void GSNetworkConnector::Connect()
{
	std::stringstream sstr(myPortString);
	unsigned short port;
	sstr >> port;
	
	mySocket = new sf::TcpSocket();
	
	switch(mySocket->connect(sf::IpAddress(myIpString), port, sf::milliseconds(3000)))
	{
		case sf::Socket::Done:
			myMenustatus = networkconnector::CONNECTED;
			Gui::SetMenupointText(3, "Start Game with " + mySocket->getRemoteAddress().toString());
			Gui::SetMenupointText(4, "Drop Connection");
		break;
		
		default:
			delete mySocket;
			mySocket = NULL;
			myNextState = new GSError(GameState::myWindow, GameState::mySettings, CANNOTCONNECT, "Cannot connect to " + myIpString + " on port " + myPortString + ".");
			myNextStatus = NEXTSTATE;
		break;
	}
}



void GSNetworkConnector::Slot1()
{
	myMenustatus = networkconnector::EDIT;
	myCurserPosition = myNameString.length();
	editstr = &myNameString;
	myMenupoint = networkconnector::NAME;
}


void GSNetworkConnector::Slot2()
{
	myMenustatus = networkconnector::EDIT;
	myCurserPosition = myIpString.length();
	editstr = &myIpString;
	myMenupoint = networkconnector::IP;
}


void GSNetworkConnector::Slot3()
{
	myMenustatus = networkconnector::EDIT;
	myCurserPosition = myPortString.length();
	editstr = &myPortString;
	myMenupoint = networkconnector::PORT;
}


void GSNetworkConnector::Slot4()
{
	switch(myMenustatus)
	{
		case networkconnector::CHOOSE:
			Connect();
//		break; // FIXME auskommentiert direkt ins spiel!

			if(myMenustatus != networkconnector::CONNECTED) break;
		
		case networkconnector::CONNECTED:
			myNextState = new GSNetworkGame(GameState::myWindow, GameState::mySettings, mySocket);
			mySocket = NULL;
			myNextStatus = NEXTSTATE;
		break;
		
		default: break;
	}
}


void GSNetworkConnector::Slot5()
{
	switch(myMenustatus)
	{
		case networkconnector::CHOOSE:
			myNextState = new GSMenu(GameState::myWindow, GameState::mySettings, myResourcemanager);
			myResourcemanager = NULL;
			myNextStatus = NEXTSTATE;
		break;
		
		case networkconnector::CONNECTED:
			delete mySocket;
			mySocket = NULL;
			myMenustatus = networkconnector::CHOOSE;
		break;
		
		default: break;
	}
}



