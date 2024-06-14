#include <GarrysMod/Lua/Interface.h>
#include <thread>
#include <chrono>
#include <cstring>
#include "socket.hpp"

using namespace std::chrono_literals;

struct serversock::gameStateData object_data;
serversock::gameStateData *pointer = &object_data;

LUA_FUNCTION(RegisterGameState)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);
	LUA->CheckType(2, GarrysMod::Lua::Type::Table);
	LUA->CheckType(3, GarrysMod::Lua::Type::Table);

	struct serversock::gameStateData game_state_data{};
	LUA->GetField(1, "map_name");
	strcpy(game_state_data.map_name, LUA->GetString(-1));
	LUA->Pop();

	LUA->GetField(1, "time_spent");
	game_state_data.time_spent = LUA->GetNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "player_status");
	game_state_data.player_status = LUA->GetNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "incoming_damage_direction");
	game_state_data.incoming_damage_direction = LUA->GetNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "incoming_damage_amount");
	game_state_data.incoming_damage_amount = LUA->GetNumber(-1);
	LUA->Pop();

	for (int i = 0; i < VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y; i++)
	{
		// Retrieve all view array distance values
		LUA->PushNumber(i + 1); // Key
		LUA->GetTable(2);		// Pops the key
		double distance = LUA->GetNumber(-1);
		game_state_data.view_trace_distance_array[i] = distance;
	}

	for (int i = 0; i < VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y; i++)
	{
		// Retrieve all view array classname values
		LUA->PushNumber(i + 1); // Key
		LUA->GetTable(3);		// Pops the key
		const char *classname = LUA->GetString(-1);
		// game_state_data.view_trace_classname_array[i] = classname;
	}

	// serversock::readValues(pointer);
	// std::this_thread::sleep_for(1000ms);
	serversock::writeValues(&game_state_data);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Push the global table
	LUA->GetField(-1, "print");						// Get the print function
	LUA->PushString("Game State Saved");			// Push our argument
	LUA->Call(1, 0);								// Call the function
	LUA->Pop();										// Pop the global table off the stack

	return 0; // How many values we are returning
}

GMOD_MODULE_OPEN()
{
	serversock::createConnection();

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Push the global table
	LUA->PushCFunction(RegisterGameState);			// Push our function
	LUA->SetField(-2, "RegisterGameState");			// Set RegisterGameState in lua to our C++ function
	LUA->Pop();										// Pop the global table off the stack

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}