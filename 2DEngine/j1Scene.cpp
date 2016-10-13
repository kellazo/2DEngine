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



	rectangle = App->render->camera;

	
	App->win->GetWindowSize((uint&)rectangle.x, (uint&)rectangle.y);

	rectangleverde = { 0,0,700,550 };

	

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	img = App->tex->Load("game_test/textures/test.png");
	//App->audio->PlayMusic("game_test/audio/music/music_sadpiano.ogg");
	volume = App->audio->volume;
	//App->map->Load(map.GetString());
	App->map->Load(map);
	//App->map->Load("hello.tmx");
	//App->map->Load("iso.tmx");


	
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

	
	
	
	
	
	


	iPoint mousepoint;
	App->input->GetMousePosition(mousepoint.x,mousepoint.y);
	int offsetcam_x = App->render->camera.x;
	int offsetcam_y = App->render->camera.y;
	iPoint map_coordinates = App->map->WorldToMap(mousepoint.x - offsetcam_x, mousepoint.y - offsetcam_y);

	MapInfo.create("Map: %d x %d Tiles: %d x %d Tilesets: %d WorldToMapCoordinates in Tiles: %d,%d", App->map->MapData.width, App->map->MapData.height, App->map->MapData.tilewidth,
	App->map->MapData.tileheight, App->map->MapData.tilesets.count(),map_coordinates.x, map_coordinates.y);
	App->win->SetTitle(MapInfo.GetString());





	if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_REPEAT)
		rectangle.y -= 4;
		
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_REPEAT)
		rectangle.y += 4;
		
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT)
		rectangle.x -= 4;
		
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT)
		rectangle.x += 4;
		
	
	
	App->map->Draw();
	
	


	//App->render->DrawQuad({ rectangleverde.x , rectangleverde.y, rectangleverde.w, rectangleverde.h }, 0, 255, 0, 255, true);
	//CreateQuad(rectangle.x, rectangle.y);

	
	//Collision(rectangle,rectangleverde);

	//int rect1Bottom = rectangle.y/4 + rectangle.h/2;
	//int rect2Top = rectangleverde.x + rectangleverde.w;
	//int rect1Top = rectangle.x/4 + rectangle.w/2;
	//int rect2Bottom = rectangleverde.y + rectangleverde.h;
	//int rect1Left = rectangle.x/4 + rectangle.y/4;
	//int rect2Right = rectangleverde.w + rectangleverde.h;
	//int rect1Right = rectangle.w/2 + rectangle.h/2;
	//int rect2Left = rectangleverde.x + rectangleverde.y;

	//int OutsideBottom = rect1Bottom < rect2Top;
	//int OutsideTop = rect1Top > rect2Bottom;
	//int OutsideLeft = rect1Left > rect2Right;
	//int OutsideRight = rect1Right < rect2Left;

	///*if (!OutsideBottom || !OutsideTop || !OutsideLeft || !OutsideRight)
	//{
	//	LOG("NO DETECTADP");
	//}
	//else
	//{
	//	LOG("DETECTED");
	//	App->map->Draw();
	//}*/
	//

	//if ((rectangle.x < rect2Top) &&(rect1Top > rectangleverde.x) && (rectangle.y < rect2Bottom) &&(rect1Bottom > rectangleverde.y))
	//{
	//	LOG("DETECTED COLLISION");
	//}
	//else
	//	LOG("NO COLLISION");



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

void j1Scene::CreateQuad(int x, int y) const
{
	//SDL_Rect cam = App->render->camera;

	//cam.x = x;
	//cam.y = y;
	//App->win->GetWindowSize((uint&)cam.x, (uint&)cam.y);
	
	App->render->DrawQuad({ x / 4 , y / 4, App->render->camera.w / 2, App->render->camera.h / 2 }, 255, 0, 0, 255, false);
}




//To keep the following code simple, I(naively) assume that sprite_2 is the same size or larger than sprite_1.
//To make it detect cases where sprite_2 is smaller, run the same tests again but swap sprite_2 and sprite_1.
bool j1Scene::Collision(SDL_Rect obj1, SDL_Rect obj2) {
	bool ret = false;

	//Let's examine the x axis first:
	//If the leftmost or rightmost point of the first sprite lies somewhere inside the second, continue.
	if ((obj1.x/4 >= obj2.x && obj1.x/4 <= (obj2.x + obj2.w)) ||
		((obj1.x/4 + obj1.w/2) >= obj2.x/2 && (obj1.x/4 + obj1.w/2) <= (obj2.x + obj2.w))) {
		//Now we look at the y axis:
		if ((obj1.y/4 >= obj2.y && obj1.y/4 <= (obj2.y + obj2.h)) ||
			((obj1.y/4 + obj1.h/2) >= obj2.y && (obj1.y/4 + obj1.h/2) <= (obj2.y + obj2.h))) {
			//The sprites appear to overlap.

			LOG("OVERLAPING");
			App->map->Draw();
			ret = true;
		}
	}
	//The sprites do not overlap:
	LOG("NO OVERLAPING");
	return ret;
}