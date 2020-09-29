# Darkstar Extender
This project allows for new code to be added to various Darkstar engine games, either using C++ or Python, without needing the source code of the original games (which have been for all official accounts lost or not publically available).

C++ Builder builder is used for ABI compatilbitiy and embeds Python to make the extension process fully dynamic.

Visual Studio is required for other proxy dlls because its linker supports forward exports. Read the document below for more information.

For some more technical information of the project, check this document: https://github.com/matthew-rindel/darkstar-hook/blob/master/progress-and-roadmap.md

## Code Structure
### src/darkstar
#### Structure Summary
The main dll project for the project which makes use of C++ Builder. It is 32-bit only because the games are all 32-bit.

C++ 17 is used, for the features that do work.

All of the types which map out engine data structures are in _src/darkstar/Core_.

All of the types which wrap the engine types are present in _src/darkstar/Hook_.

All of the types which are used to interact with Python are present in _src/darkstar/Python_.

The Darkstar engine is a precursor to the Torque engine, and most of the documentation for Torque applies to Darkstar for a few topics. These documents are:
* http://docs.garagegames.com/torque-3d/official/index.html?content/documentation/Scripting/Overview/Introduction.html
* http://www.garagegames.com/community/resources/view/20937
* http://wiki.torque3d.org/

_src/Darkstar/functions.json_ contains all of the memory addresses for some of the functions defined in _src/darkstar/Core/EngineFunctions.hpp_.

The ultimate goal is to have addresses for every binary using the Darkstar engine. Currently only Starsiege 1.003 is supported.
#### Brief Engine Description
The _src/Darkstar/Core/EngineTypes.hpp_ file defines all of the data types present in each binary of each Darkstar game.

All of the function pointers defined in _src/darkstar/Core/EngineFunctions.hpp_ interact with these data types directly.

The object structure is something like this:
* GameRoot is used to represent a global object representing the game itself.
    * It contains all the other objects then used to interact with the game, find specifc objects or communicate with either the client or server.
* GameConsole is a child of GameRoot which allows for direct interaction with the scripting engine, which is called CScript (the precursor to TorqueScript).
    * The game console is where most of the magic happens for the hook, as it allows for new code to be added to the game dynamically, which is then accessible from the scripting engine.
* GameObject is a base class for every entity in a game. 
    * If we are talking about Tribes, then every soldier jetpacking around is a GameObject derivative. 
    * If we are talking about Starsiege, then every herc or tank is a GameObject derivative.
* GameManager represents either a client or server which manages many child GameObjects, especially everything relating to the game world, all of the level assets, static objects, NPCs, etc.
* The ConsoleConsumer is an extension point for GameConsole, which allows for any console messages to be sent to the consumer to handle somehow (usually to log or display on another window or something else). _AddConsoleConsumer_ in _darkstar/Core/EngineFunctions.hpp_ is used to add a new consumer.
* The ConsoleCallback is another extension point for GameConsole, and is what is used when new native code is to be excuted for by script funciton. _AddConsoleCallbackFunc_ or _AddConsoleCallback_ in _darkstar/Core/EngineFunctions.hpp_ is used to add a new callback.
* GamePlugin is an extended ConsoleCallback which is an extension point of GameRoot. These plugins can also be notified of frame render events during the render cycle. _AddGamePlugin_ in _darkstar/Core/EngineFunctions.hpp_ is used to add a new plugin.

### src/mem
mem.dll is a special dll that Starsiege and Tribes are compiled to check for, in oder to replace their internal calls to malloc/free/calloc/realloc.

The project in src/mem is a Visual Studio project which compiles a proxy dll, and all it does is forward those functions to the implementation found in darkstar.dll

The _mem.cpp_ file is deliberately empty for there to be a translation unit to compile.

The real work of this project is done by _mem.def_, which is used to describe the functions to forward.

See https://github.com/matthew-rindel/darkstar-hook/blob/master/progress-and-roadmap.md for more information and links about how this works.

## Dependencies
### Tools
* C++ Builder 10.3.3 or newer: https://www.embarcadero.com/products/cbuilder/starter
* A disassembler or decompiler for the relevant game. I use Binary Ninja: https://binary.ninja/
### Libraries
* Python 3.8.3 or newer (32-bit version only)
* Pybind11 2.5.0
* nlohmann_json 3.8.0

## Project Setup
Conan is required to install the project dependencies.

If you don't already have it, just run:

``pip install conan``

Once everything is installed, run the following in the main checkout directory of the project.

``conan install . --profile ./local-profile.ini``

All installed packages are copied into the _packages_ folder. This includes the Python include files from the local Python installation.

To compile the project, simply use:

``conan build .``

TODO add more information about the build targets.

## Setup with Game
TODO

## Notes
* Everything is work in progress, including the documentation.
    * More detailed explanations of how this module works and the exposed API will be added over time.
* All of the exe details for each game get stored in functions.json. Right now, only Starsiege 1.0003r has been added.
* Why C++ Builder? Why not Visual C++ or MinGW?
    * Most Darkstar games were compiled with the predecessor to C++ Builder and use a calling convention called __fastcall
    * The meaning of __fastcall in C++ Builder is different to that of Visual C++, thus meaning raw assembly code would be needed to interface with the game code if Visual C++ is to be used
    * So far, object structures (like vtables) are compatible between C++ Builder and Borland C++, which makes it possible extend the game in new ways based on disassembled information  
* What can be done with this?
    * The built-in game interpreter can be extended with new functions either from C++ or Python
    * Existing built-in game functions can also be removed, replaced or decorated
    * Examples of ideas:
        * Updating the screenshot function of the game to convert BMP files to PNG
        * Adding support for remote controlling of dedicated servers through a browser
        * Discord integration with the chat engine in the game
        * Much more

### Related Projects
https://github.com/matthew-rindel/darkstar-projects
