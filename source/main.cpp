#include <GarrysMod/Lua/Interface.h>
#include <thread>
#include <chrono>
#include "socket.hpp"

using namespace std::chrono_literals;

struct serversock::objectData object_data;
serversock::objectData *pointer = &object_data;

LUA_FUNCTION( MyFirstFunction )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::Number ); // Make sure a number is the first argument
    LUA->CheckType( 2, GarrysMod::Lua::Type::Table );
	LUA->CheckType( 3, GarrysMod::Lua::Type::Table );

	int array_size = LUA->GetNumber( 1 );
	int* view_distance_array = new int[array_size];
	const char** view_classname_array = new const char*[array_size];

	for (int i = 0; i < array_size; i++)
	{
		// Retrieve all view array distance values
		LUA->PushNumber(i + 1); // Key
		LUA->GetTable(2); // Pops the key
		double distance = LUA->GetNumber(-1);
		view_distance_array[i] = distance;
	}

	for (int i = 0; i < array_size; i++)
	{
		// Retrieve all view array classname values
		LUA->PushNumber(i + 1); // Key
		LUA->GetTable(3); // Pops the key
		const char* classname = LUA->GetString(-1);
		view_classname_array[i] = classname;
	}

	serversock::readValues(pointer);
	// std::this_thread::sleep_for(1000ms);

	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB ); // Push the global table
		LUA->GetField( -1, "print" ); // Get the print function
		LUA->PushNumber( pointer->value ); // Push our argument
		LUA->Call( 1, 0 ); // Call the function
	LUA->Pop(); // Pop the global table off the stack

    return 0; // How many values we are returning
}

GMOD_MODULE_OPEN()
{
	serversock::createConnection();

	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB ); // Push the global table
		LUA->PushCFunction( MyFirstFunction ); // Push our function
		LUA->SetField( -2, "MyFirstFunction" ); // Set MyFirstFunction in lua to our C++ function
	LUA->Pop(); // Pop the global table off the stack

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}