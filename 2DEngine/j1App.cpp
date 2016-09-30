#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1FileSystem.h"

#include "j1App.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;
	
	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	fs = new j1FileSystem(".");
	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(scene);

	// render last to swap buffer
	AddModule(render);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	bool ret = false;
	//char* buffer;
	//App->fs->Load("config.txt",&buffer); without RWops
	//RELEASE(buffer);

	//App->fs->Load("config.txt"); with RWops



	/*-- - load config file-- -
	Load "config.xml" file to a buffer, then send the data to
	pugi using load_buffer() method. If everything goes well, load
	the top tag inside the xml_node property created in the last TODO*/
	LOG("XML: Loading config file to buffer through PhysFS\n");
	root = LoadConfigXml(document);
	LOG("XML: Config file succsesfully loaded in buffer.\n");

	if (root.empty() == false)
	{
		//for write directori through PhysFS using "pref dir" of SDL_GetPrefPath using DEFINES var of P2Defs
		//organization.create(ORGANIZATION);
		//applicationName.create(APP);
		//self-config also we can put data from xml file to program with xml functions to read:

		ret = true;
		appNode = root.child("app");
		organization.create(appNode.child("organization").child_value());
		applicationName.create((appNode.child("title").child_value()));
		LOG("XML: Config.xml file parsed: Name of Organization: %s", organization.GetString());
		LOG("XML: Config.xml file parsed: Name of Application: %s", applicationName.GetString());

	}
		//accessing data xml to read
		//for (root = document.child("config"); root; root = root.next_sibling("config"))
		//{
		//	// %s for read strings in console
		//	// read the name of title aplication
		//	LOG("Nom: %s", root.child("name").text().as_string());

		//}

	if(ret == true)
	{

		p2List_item<j1Module*>* item;
		item = modules.start;

		while (item != NULL && ret == true)
		{
			// TODO 7: Add a new argument to the Awake method to receive a pointer to a xml node.
			// If the section with the module name exist in config.xml, fill the pointer with the address of a valid xml_node
			// that can be used to read all variables from that section. Send nullptr if the section does not exist in config.xml
			LOG("App: Awakening module %s ...\n", item->data->name.GetString());
			ret = item->data->Awake(root.child(item->data->name.GetString()));
			LOG("App: ....finished Awake module %s \n", item->data->name.GetString());
			item = item->next;
			
		}
	}
	LOG("App: Finish Awake() Function of all modules. \n");
	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}


// ---------------------------------------------
pugi::xml_node j1App::LoadConfigXml(pugi::xml_document& documentxml) const
{
	pugi::xml_node ret;


	char* buffer;
	int size = App->fs->Load("config.xml", &buffer);

	//load xml in buffer for read
	pugi::xml_parse_result result = documentxml.load_buffer(buffer, size);
	//release memory xml
	RELEASE(buffer);

	if (result)
	{
		LOG("XML %d parsed without errors, attr value:[ %d]\n", result, document.child("node").attribute("attr").value());
		//root = document.child("config");
		//appNode = root.child("app");
		//ret = true;
		ret = documentxml.child("config");
	}
	else
	{
		LOG("XML %s parsed with errors, attr value:[ %d ]\n", result, document.child("node").attribute("attr").value());
		LOG("ERROR description: %s \n", result.description());
		LOG("ERROR offset: %d , (error at [.. %d ] \n", result.offset, result + result.offset);

		
	}


	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	//This is a good place to call load / Save functions

	if (want_to_save == true)
	{
		SavegameNow();
	}
	if (want_to_load == true)
	{
		LoadGameNow();
	}
	
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// to read private vars from public methods outside
const char* j1App::GetApplicationName() const
{
	
		return applicationName.GetString();
}

// to read private vars from public methods outside
const char* j1App::GetOrganization() const
{
	
	return organization.GetString();
}


// to xml node to read
//const pugi::xml_node j1App::xmlrootnode() const
//{
//
//	return root;
//}

void j1App::Load()
{

}
void j1App::Save() const
{

}

bool j1App::LoadGameNow()
{
	Load();
	return true;
}
bool j1App::SavegameNow() const
{
	Save();
	return true;
}

//: Create a simulation of the xml file to read 
