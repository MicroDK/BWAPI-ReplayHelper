#include "ExampleTournamentModule.h"

using namespace BWAPI;

int cameraMoveTime = 4*24;
int lastMoved = 0;
int localSpeed = 0;
int frameSkip = 0;
std::string mapName = "";

bool drawBotNames = true;
bool drawUnitInfo = true;
bool drawGameInfo = true;

char buffer[MAX_PATH];

void ExampleTournamentAI::onStart()
{	
	GetModuleFileName(NULL, buffer, MAX_PATH);
	Broodwar->printf("Path is %s", buffer);

	// Set the command optimization level (reduces high APM, size of bloated replays, etc)
	Broodwar->setCommandOptimizationLevel(DEFAULT_COMMAND_OPTIMIZATION);

	parseConfigFile("bwapi-data\\ReplayHelper_settings.ini");
	
	Broodwar->setLocalSpeed(localSpeed);
	Broodwar->setFrameSkip(frameSkip);

	// Grab only the alpha characters from the map name to remove version numbers
	for (auto &c : Broodwar->mapName()) {
		if (isalpha(c))
			mapName.push_back(c);
		//if (c == '.')
		//	break;
	}
}

void ExampleTournamentAI::onEnd(bool isWinner)
{
}

void ExampleTournamentAI::onFrame()
{
	if (Broodwar->isPaused())
		return;

	//Broodwar->setLocalSpeed(localSpeed);

	_autoObserver.onFrame();

	drawTournamentModuleSettings(10, 10);

	if (drawUnitInfo)
	{
		drawUnitInformation(460,10);
	}
}

void ExampleTournamentAI::drawTournamentModuleSettings(int x, int y)
{
	int drawX_bots = x + 300;
	int drawY_bots = y;
	if (drawUnitInfo)
	{
		drawX_bots = x + 160;
		drawY_bots = y + 304;
	}
	int drawX_info = x;
	int drawY_info = y;
	int width = 120;

	if (drawBotNames)
	{
		Playerset playerset = Broodwar->getPlayers();
		for each (Player player in playerset)
		{
			if (!player->isObserver())
			{
				int workers = 0;
				Unitset units = player->getUnits();
				for (UnitInterface * unit : units)
				{
					if (unit->getType() == UnitTypes::Zerg_Drone || 
						unit->getType() == UnitTypes::Terran_SCV || 
						unit->getType() == UnitTypes::Protoss_Probe)
					{
						workers += 1;
					}
				}

				//Broodwar->setTextSize(BWAPI::Text::Size::Large);
				char textcolor = player->getTextColor();
				Broodwar->drawTextScreen(drawX_bots, drawY_bots, "\%c%s", textcolor, player->getName().c_str());
				Broodwar->drawTextScreen(drawX_bots + 120, drawY_bots, "\x1Fm: \x04%d", player->minerals());
				Broodwar->drawTextScreen(drawX_bots + 172, drawY_bots, "\x07g: \x04%d", player->gas());
				Broodwar->drawTextScreen(drawX_bots + 224, drawY_bots, "\x05w: \x04%d", workers);
				Broodwar->drawTextScreen(drawX_bots + 270, drawY_bots, "\x01s: \x04%d / %d", int(player->supplyUsed()/2), int(player->supplyTotal()/2));
				drawY_bots += 12;
			}
		}
	}

	if (drawGameInfo)
	{
		Broodwar->drawTextScreen(drawX_info, drawY_info, "\x01%s\x04%s", "Map Name: ", mapName.c_str());
		drawY_info += 12;

		Broodwar->drawTextScreen(drawX_info, drawY_info, "\x01%s\x04%d", "Frame Count: ", Broodwar->getFrameCount());
		drawY_info += 12;
		
		Broodwar->drawTextScreen(drawX_info, drawY_info, "\x01%s\x04%d", "Local Speed: ", localSpeed);
		drawY_info += 12;

		Broodwar->drawTextScreen(drawX_info, drawY_info, "\x01%s\x04%d", "Frame Skip: ", frameSkip);
		drawY_info += 12;
	}
}

void ExampleTournamentAI::drawUnitInformation(int x, int y) 
{
	int drawX = x;
	int drawY = y;
	std::string prefix = "\x04";

	Playerset playerset = Broodwar->getPlayers();
	for each (Player player in playerset)
	{
		if (!player->isObserver())
		{
			Broodwar->drawTextScreen(drawX, drawY, "\x04%s's Units", player->getName().c_str());
			Broodwar->drawTextScreen(drawX + 140, drawY, "\x04#");
			Broodwar->drawTextScreen(drawX + 160, drawY, "\x04X");

			int yspace = 0;

			// for each unit in the queue
			for each (UnitType t in UnitTypes::allUnitTypes()) 
			{
				int numUnits = player->completedUnitCount(t) + player->incompleteUnitCount(t);
				int numDeadUnits = player->deadUnitCount(t);

				// if there exist units in the vector
				if (numUnits > 0) 
				{
					if (t.isWorker())			{ prefix = "\x0F"; }
					else if (t.isDetector())	{ prefix = "\x07"; }		
					else if (t.canAttack())		{ prefix = "\x08"; }		
					else if (t.isBuilding())	{ prefix = "\x03"; }
					else						{ prefix = ""; }

					Broodwar->drawTextScreen(drawX, drawY+10+((yspace)*10), "%s%s", prefix.c_str(), t.getName().c_str());
					Broodwar->drawTextScreen(drawX+140, drawY+10+((yspace)*10), "%s%d", prefix.c_str(), numUnits);
					Broodwar->drawTextScreen(drawX+160, drawY+10+((yspace++)*10), "%s%d", prefix.c_str(), numDeadUnits);
				}
			}

			drawX -= 200;
		}
	}
}

void ExampleTournamentAI::onSendText(std::string text)
{
	Broodwar->printf("onSendText: %s", text.c_str());

	std::stringstream ss(text);

	std::string command;
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);

	std::string variableName;
	std::transform(variableName.begin(), variableName.end(), variableName.begin(), ::tolower);

	std::string val;

	ss >> command;
	ss >> variableName;
	ss >> val;

	if (command == "/set")
	{
		if (variableName == "localspeed")
		{
			localSpeed = GetIntFromString(val);
			Broodwar->setLocalSpeed(localSpeed);
			Broodwar->printf("setLocalSpeed set to %d", localSpeed);
		}
		else if (variableName == "frameskip")
		{
			frameSkip = GetIntFromString(val);
			Broodwar->setFrameSkip(frameSkip);
			Broodwar->printf("setFrameSkip set to %d", frameSkip);
		}
		else if (variableName == "drawbotnames")
		{
			if (GetIntFromString(val) == 1)
				drawBotNames = true;
			else
				drawBotNames = false;
			Broodwar->printf("drawBotNames set to %d", drawBotNames);
		}
		else if (variableName == "drawunitinfo")
		{
			if (GetIntFromString(val) == 1)
				drawUnitInfo = true;
			else
				drawUnitInfo = false;
			Broodwar->printf("drawUnitInfo set to %d", drawUnitInfo);
		}
		else if (variableName == "drawgameinfo")
		{
			if (GetIntFromString(val) == 1)
				drawGameInfo = true;
			else
				drawGameInfo = false;
			Broodwar->printf("drawGameInfo set to %d", drawGameInfo);
		}
	}
}

void ExampleTournamentAI::onReceiveText(BWAPI::Player player, std::string text)
{
	Broodwar->printf("onReceiveText: %s", text.c_str());
}

void ExampleTournamentAI::onPlayerLeft(BWAPI::Player player)
{
}

void ExampleTournamentAI::onPlayerDropped(BWAPI::Player* player)
{
}

void ExampleTournamentAI::onNukeDetect(BWAPI::Position target)
{
}

void ExampleTournamentAI::onUnitDiscover(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onUnitCreate(BWAPI::Unit unit)
{
	int mult = 3;

	if (Broodwar->getFrameCount() - lastMoved < cameraMoveTime*mult)
	{
		return;
	}

	Broodwar->setScreenPosition(unit->getPosition() - Position(320, 240));
	lastMoved = Broodwar->getFrameCount();
}

void ExampleTournamentAI::onUnitDestroy(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onUnitMorph(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onUnitComplete(BWAPI::Unit *unit)
{
}

void ExampleTournamentAI::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onSaveGame(std::string gameName)
{
}
/*
bool ExampleTournamentModule::onAction(BWAPI::Tournament::ActionID actionType, void *parameter)
{
	switch ( actionType )
	{
		case Tournament::EnableFlag:
			switch ( *(int*)parameter )
			{
				case Flag::CompleteMapInformation:		return false;
				case Flag::UserInput:					return false;
				default:								break;
			}
			// If more flags are added, by default disallow unrecognized flags
			return false;

		case Tournament::PauseGame:						return false;
	//	case Tournament::RestartGame:					return false;
		case Tournament::ResumeGame:					return false;
		case Tournament::SetFrameSkip:					return false;
		case Tournament::SetGUI:						return false;
		case Tournament::SetLocalSpeed:					return false;
		case Tournament::SetMap:						return false;
		case Tournament::LeaveGame:						return true;
	//	case Tournament::ChangeRace:					return false;
		case Tournament::SetLatCom:						return true;
		case Tournament::SetTextSize:					return false;
		case Tournament::SendText:						return false;
		case Tournament::Printf:						return false;
		case Tournament::SetCommandOptimizationLevel:
			return *(int*)parameter >= MINIMUM_COMMAND_OPTIMIZATION;
							
		default:										break;
	}

	return true;
}

void ExampleTournamentModule::onFirstAdvertisement()
{
}
*/
std::vector<std::string> ExampleTournamentAI::getLines(const std::string & filename)
{
    // set up the file
    std::ifstream fin(filename.c_str());
    if (!fin.is_open())
    {
		Broodwar->printf("Tournament Module Settings File Not Found, Using Defaults", filename.c_str());
		return std::vector<std::string>();
    }

	std::string line;

    std::vector<std::string> lines;

    // each line of the file will be a new player to add
    while (fin.good())
    {
        // get the line and set up the string stream
        getline(fin, line);
       
        // skip blank lines and comments
        if (line.length() > 1 && line[0] != '#')
        {
            lines.push_back(line);
        }
    }

	fin.close();

    return lines;
}

void ExampleTournamentAI::parseConfigFile(const std::string & filename)
{
    std::vector<std::string> lines(getLines(filename));

    for (size_t l(0); l<lines.size(); ++l)
    {
        std::istringstream iss(lines[l]);
        std::string option;
        iss >> option;

        if (strcmp(option.c_str(), "LocalSpeed") == 0)
        {
			iss >> localSpeed;
        }
        else if (strcmp(option.c_str(), "FrameSkip") == 0)
        {
            iss >> frameSkip;
        }
		else if (strcmp(option.c_str(), "DrawUnitInfo") == 0)
        {
            std::string val;
			iss >> val;
            
			if (strcmp(val.c_str(), "false") == 0)
			{
				drawUnitInfo = false;
			}
        }
		else if (strcmp(option.c_str(), "DrawGameInfo") == 0)
        {
            std::string val;
			iss >> val;
            
			if (strcmp(val.c_str(), "false") == 0)
			{
				drawGameInfo = false;
			}
        }
		else if (strcmp(option.c_str(), "DrawBotNames") == 0)
        {
            std::string val;
			iss >> val;
            
			if (strcmp(val.c_str(), "false") == 0)
			{
				drawBotNames = false;
			}
        }
		else
		{
			Broodwar->printf("Invalid Option in Tournament Module Settings: %s", option.c_str());
		}
    }
}

int ExampleTournamentAI::GetIntFromString(const std::string & s)
{
	std::stringstream ss(s);
	int a = 0;
	ss >> a;
	return a;
}