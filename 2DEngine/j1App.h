#ifndef __j1APP_H__
#define __j1APP_H__

#include "p2List.h"
#include "j1Module.h"

//parser data lib http://pugixml.org/docs/quickstart.html
#include "PugiXml\src\pugixml.hpp"



// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1FileSystem;
class j1Scene;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	int GetArgc() const;
	const char* GetArgv(int index) const;

	//to read private var through public methods from other sites, for example writepath
	const char* GetOrganization() const;
	const char* GetApplicationName() const;
	

private:

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

public:

	uint		frames;
	float		dt;

	// Modules
	j1Window*		win;
	j1Input*		input;
	j1Render*		render;
	j1Textures*		tex;
	j1Audio*		audio;
	j1Scene*		scene;
	j1FileSystem*	fs;

private:

	p2List<j1Module*>	modules;
	int					argc;
	char**				args;

	//for write directori through PhysFS using "pref dir" of SDL_GetPrefPath
	p2SString			organization;
	p2SString			applicationName;
	
};

extern j1App* App;

#endif