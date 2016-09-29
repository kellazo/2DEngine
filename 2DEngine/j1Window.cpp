#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"

#include "SDL/include/SDL.h"


j1Window::j1Window() : j1Module()
{
	window = NULL;
	screen_surface = NULL;
	name.create("window");
}

// Destructor
j1Window::~j1Window()
{
}

// Called before render is available
bool j1Window::Awake(pugi::xml_node& node)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		//title.create(WINDOW_TITLE);

		//changing the program values to xml values
		Uint32 flags = SDL_WINDOW_SHOWN;
		width = node.child("resolution").attribute("width").as_uint();
		height = node.child("resolution").attribute("height").as_uint();
		scale = node.child("resolution").attribute("scale").as_uint();

		if(node.child("fullscreen").attribute("value").as_bool() == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(node.child("borderless").attribute("value").as_bool() == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(node.child("resizable").attribute("value").as_bool() == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(node.child("fullscreen_window").attribute("value").as_bool() == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			/*Read the title of the app from the XML
			and set directly the window title using SetTitle()*/
			//if we put public the app var of xml
			//pugi::xml_node root = App->xmlrootnode();
			//for (App->root = App->document.child("config"); App->root; App->root = App->root.next_sibling("config"))
			//{
			//	// %s for read strings in console

			//	LOG("Nom: %s", App->root.child("name").text().as_string());
			//	SetTitle(App->root.child("name").text().as_string());
			//}	
			//SetTitle(APP);
			//nodeconfig.name();
			//if we use the awake argument from hereditage aplicattion app:
			SetTitle(node.child("window_title").child_value());
		}
	}

	return ret;
}

// Called before quitting
bool j1Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void j1Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	SDL_SetWindowTitle(window, new_title);
}

void j1Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}

uint j1Window::GetScale() const
{
	return scale;
}