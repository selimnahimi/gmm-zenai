#include <GarrysMod/Lua/Interface.h>
#include <thread>
#include <chrono>
#include <cstring>
#include <cmath>

#include "socket.hpp"
#include "crc32b.hpp"

using namespace std::chrono_literals;

struct serversock::gameStateData object_data;
serversock::gameStateData *pointer = &object_data;

LUA_FUNCTION(RegisterGameState)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::Table);
	LUA->CheckType(2, GarrysMod::Lua::Type::Table);
	LUA->CheckType(3, GarrysMod::Lua::Type::Table);

	struct serversock::gameStateData game_state_data
	{
	};

	// Get map_name field, hash it and store it in game_state_data
	LUA->GetField(1, "map_name");
	const char *map_name = LUA->GetString(-1);
	game_state_data.map_name = hash(map_name);
	LUA->Pop();
	
	LUA->GetField(1, "time_spent");
	game_state_data.time_spent = LUA->GetNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "player_status");
	game_state_data.player_status = LUA->GetNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "equipped_weapon");
	const char *equipped_weapon = LUA->GetString(-1);
	game_state_data.equipped_weapon = hash(equipped_weapon);
	LUA->Pop();

	LUA->GetField(1, "incoming_damage_direction");
	game_state_data.incoming_damage_direction = LUA->GetNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "incoming_damage_amount");
	game_state_data.incoming_damage_amount = LUA->GetNumber(-1);
	LUA->Pop();

	// Retrieve all view array distance values
	for (int i = 0; i < VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y; i++)
	{
		LUA->PushNumber(i + 1); // Key
		LUA->GetTable(2);		// Pops the key
		double distance = LUA->GetNumber(-1);
		game_state_data.view_trace_distance_array[i] = distance;
	}

	// Retrieve all view array classname values
	for (int i = 0; i < VIEW_GRID_SIZE_X * VIEW_GRID_SIZE_Y; i++)
	{
		LUA->PushNumber(i + 1); // Key
		LUA->GetTable(3);		// Pops the key
		const char *classname = LUA->GetString(-1);
		game_state_data.view_trace_classname_array[i] = hash(classname);
	}

	// serversock::readValues(pointer);
	// std::this_thread::sleep_for(1000ms);
	serversock::writeValues(&game_state_data); // Send data to Python socket server

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Push the global table
	LUA->GetField(-1, "print");						// Get the print function
	LUA->PushString("Game State Saved");			// Push our argument
	LUA->Call(1, 0);								// Call the function
	LUA->Pop();										// Pop the global table off the stack

	return 0; // How many values we are returning
}

GMOD_MODULE_OPEN()
{
	serversock::createConnection(); // Connect to Python socket server

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

// Hash string to float between 0-1
// Source: https://stackoverflow.com/a/42909410
float hash(const char *str)
{
	return crc32b(str) / pow(2, 32);
}