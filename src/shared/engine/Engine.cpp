#include "Engine.h"
#include "engine.h"
#include <iostream>
#include <unistd.h>

using namespace state;
using namespace engine;
using namespace std;

Engine::Engine() : currentState("engine")
{
    record["lenght"] = 0;
	record["commands"][0] = "";
}

Engine::Engine(std::string stateMode) : currentState(stateMode)
{
    record["lenght"] = 0;
	record["commands"][0] = "";
}

Engine::~Engine()
{
}

state::State &Engine::getState()
{
    state::State &refState = currentState;
    return refState;
}

void Engine::addPassiveCommands()
{
    int priority = 0;
    // if currentCommands isnt empty
    if (currentCommands.size() > 0)
        // find largest priority
        priority = currentCommands.rbegin()->first + 1;

    // passive commands...
    unique_ptr<Command> ptr_cw(new CheckWinnerCommand());
    addCommand(move(ptr_cw), priority++);
    
}

void Engine::setEnableRecord(bool enableRecord){
    this->enableRecord = enableRecord;
}

bool Engine::getEnableRecord() const
{
    return enableRecord;
}

Json::Value Engine::getRecord() 
{
	return record;
}

void Engine::addCommand(std::unique_ptr<Command> ptr_cmd, int priority)
{
    if (priority == -1)
    {
        // find largest priority
        if (currentCommands.size() > 0)
            priority = currentCommands.rbegin()->first + 1;
        else
        {
            priority = 0;
        }
    }
    if (enableRecord && ptr_cmd->getCommandID() != CHECK_WINNER){
		Json::Value newCommand = ptr_cmd->serialize();
		record["CommandArray"][record["Size"].asUInt()] = newCommand;
		record["Size"] = record["Size"].asUInt() + 1;

	}
    currentCommands[priority] = move(ptr_cmd);
}

void Engine::update()
{
    if (!currentState.getEnd())
    {
        cout << "Adding passive commands ..." << endl;
        addPassiveCommands();
        cout << "Executing commands from turn " << currentState.getTurn() << endl;
        //default event
        StateEvent stateEvent(ALLCHANGED);
        bool endTurn = false;
        for (size_t i = 0; i < currentCommands.size(); i++)
        {
            currentCommands[i]->execute(currentState);
            currentState.notifyObservers(stateEvent, currentState);
            if(currentState.getMode() == "engine"){
                usleep(200 * 1000);
            }
        }
        // clean using iterator
        map<int, std::unique_ptr<Command>>::iterator iterator;
        for (iterator = currentCommands.begin(); iterator != currentCommands.end(); iterator++)
        {
            currentCommands.erase(iterator);
        }
    }
    else
    {

        cout << "The game is ended, we have a winner" << endl;
    }
}

std::map<int, std::unique_ptr<Command>>& Engine::getCurrentCommands (){
    return currentCommands;
}
