#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	bool ret = true;

	pugi::xml_node name = node.child("map");

	map = name.text().get();
	//pugi::xml_node name = node.child("name");
	//map.create(name.child_value());

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	//img = App->tex->Load("game_test/textures/test.png");
	//App->audio->PlayMusic("game_test/audio/music/music_sadpiano.ogg");
	volume = App->audio->volume;
	//App->map->Load(map.GetString());
	App->map->Load(map);
	//App->map->Load("hello.tmx");
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{

	/*Call load / save methods when pressing l / s”*/
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		App->LoadGame("SaveGame.xml");
		LOG("Scene: You are pressed L key(load game)");
		
		
	}
		

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		App->SaveGame("SaveGame.xml");
		LOG("Scene: You are pressed S key(save game)");
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y += 1;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x -= 1;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x += 1;
	
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		volume == App->audio->ChangeVolume(volume);
		LOG("Scene: volume - pressed");
		volume--;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN)
	{
		volume == App->audio->ChangeVolume(volume);
		LOG("Scene: volume + pressed");
		volume++;
	}
	App->map->Draw();

	//App->render->Blit(img, 300, 0);
	
	MapInfo.create("Map: %d x %d Tiles: %d x %d Tilesets: %d", App->map->MapData.width, App->map->MapData.height, App->map->MapData.tilewidth, App->map->MapData.tileheight, App->map->MapData.tilesets.count());
	App->win->SetTitle(MapInfo.GetString());
	
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
