#include "Settings.hpp"

#include <sstream>



Settings::Settings(int argc, char* argv[]) :
myArgc(argc), myArgv(argv)
{
	std::stringstream stringstream; stringstream << sf::Style::Close;
	WindowStyleClose = stringstream.str();
	stringstream.clear(); stringstream << sf::Style::Fullscreen;
	WindowStyleFullscreen = stringstream.str();
	WindowStyleEmulation = "0";
	
	LoadDefaults();
}


Settings::~Settings()
{
	iniManager::SaveConfigFile();
}



void Settings::LoadDefaults()
{
	iniManager::mySettings["Window"]["Width"] = "800";
	iniManager::mySettings["Window"]["Height"] = "600";
	iniManager::mySettings["Window"]["Style"] = WindowStyleClose;
	
	iniManager::mySettings["Player1"]["Left"] = "1";
	iniManager::mySettings["Player1"]["Right"] = "3";
	iniManager::mySettings["Player1"]["Down"] = "2";
	iniManager::mySettings["Player1"]["Rotate"] = "5";
	
	iniManager::mySettings["Player2"]["Left"] = "a";
	iniManager::mySettings["Player2"]["Right"] = "d";
	iniManager::mySettings["Player2"]["Down"] = "s";
	iniManager::mySettings["Player2"]["Rotate"] = "w";
	
	iniManager::mySettings["Game"]["Name"] = "Player";
	iniManager::mySettings["Game"]["Ip"] = "127.0.0.1";
	iniManager::mySettings["Game"]["Port"] = "4200";
	iniManager::mySettings["Game"]["MusicVolume"] = "85";
	iniManager::mySettings["Game"]["SoundVolume"] = "75";
	iniManager::mySettings["Game"]["Sensitivity"] = "1";
	iniManager::mySettings["Game"]["AiLevel"] = "1";
	
	iniManager::mySettings["Resource"]["Font"] = "img/font.ttf";
	iniManager::mySettings["Resource"]["GameBackground"] = "img/background_game.png";
	iniManager::mySettings["Resource"]["MenuBackground"] = "img/background_menu.png";
	iniManager::mySettings["Resource"]["Bubbles"] = "img/bubble.png";
}



int Settings::ParseArguments()
{
	for(int i = 1; i < myArgc; ++i)
	{
		std::string argument = myArgv[i];
		
		if(argument == "--fullscreen" || argument == "-full")
		{
			iniManager::mySettings["Window"]["Style"] = WindowStyleFullscreen;
		}
		else if(argument == "--fullscreenemulation" || argument == "-fsemu")
		{
			iniManager::mySettings["Window"]["Style"] = WindowStyleEmulation;
			SetDesktopResolution();
		}
		else if(argument == "--window" || argument == "-win")
		{
			iniManager::mySettings["Window"]["Style"] = WindowStyleClose;
		}
		else if(argument == "--desktop-mode")
		{
			SetDesktopResolution();
		}
		else if(argument.substr(0,13) == "--resolution=" && argument.length() > 18)
		{
			std::string string(argument.substr(13));
			SetResolution(string);
		}
		else if(argument.substr(0,5) == "-res=" && argument.length() > 10)
		{
			std::string string(argument.substr(5));
			SetResolution(string);
		}
		else if(argument == "--help" || argument == "-h")
		{
			Help();
			return 1;
		}
		else
		{
			std::cout << myArgv[0] << ": invalid optin '" << argument << "'.\n";
			std::cout << "Try '" << myArgv[0] << " --help' to get more information." << std::endl;
			return -1;
		}
	}
	
	return 0;
}



void Settings::SetResolution(const std::string& string)
{
	iniManager::mySettings["Window"]["Width"] = string.substr(0, string.find(','));
	iniManager::mySettings["Window"]["Height"] = string.substr(string.find(',') + 1);
}


void Settings::SetDesktopResolution()
{
	std::stringstream sstr;
	sstr << sf::VideoMode::getDesktopMode().width;
	sstr >> iniManager::mySettings["Window"]["Width"];
	sstr.clear(); sstr << sf::VideoMode::getDesktopMode().height;
	sstr >> iniManager::mySettings["Window"]["Height"];
}


void Settings::Help() const
{
	std::cout << "\tPapouja - An open source PuyoPuyo clone with networkgame mode.\n\n";
	std::cout << "Arguments are:\n";
	std::cout << "\t-h\t  --help\t\t  Show this help text.\n";
	std::cout << "\t-win\t  --window\t\t  Play the game in window mode.\n";
	std::cout << "\t-full\t  --fullscreen\t\t  Play the game in fullscreen mode.\n";
	std::cout << "\t-fsemu\t  --fullscreenemulation\t  Emulate a fullscreen window.\n";
	std::cout << "\t-res=x,y  --resolution=x,y\t  Play with the resolution x * y.\n";
	std::cout << "\t\t  --desktop-mode\t  Play with the desktop resolution.\n";
	std::cout << std::endl;
}



sf::VideoMode Settings::GetVideoMode() const
{
	std::stringstream sstr;
	unsigned int x, y;
	sstr << (*(*iniManager::mySettings.find("Window")).second.find("Width")).second;
	sstr >> x; sstr.clear();
	sstr << (*(*iniManager::mySettings.find("Window")).second.find("Height")).second;
	sstr >> y;
	
	return sf::VideoMode(x, y);
}


sf::Uint32 Settings::GetWindowStyle() const
{
	std::stringstream sstr;
	sf::Uint32 style;
	sstr << (*(*iniManager::mySettings.find("Window")).second.find("Style")).second;
	sstr >> style;
	return style;
}


sf::Vector2f Settings::GetScaleFactor() const
{
	return sf::Vector2f(static_cast<float>(GetVideoMode().width) / 1600.f, static_cast<float>(GetVideoMode().height) / 1200.f);
}


