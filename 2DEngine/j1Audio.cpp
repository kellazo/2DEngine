#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1FileSystem.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	
	name.create("audio");
}

// Destructor
j1Audio::~j1Audio()
{
}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& node)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = true;
	}

	bool music = PlayMusic(node.child("music").child_value());
	volume = Mix_VolumeMusic(node.child("music").attribute("volume").as_uint());
	LOG("Volume default was: %d \n", volume);
	LOG("Volume default is now: %d \n", volume = Mix_VolumeMusic(-1));

	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music_memory != NULL)
	{
		Mix_FreeMusic(music_memory);
	}

	p2List_item<Mix_Chunk*>* item;
	for(item = fx.start; item != NULL; item = item->next)
		Mix_FreeChunk(item->data);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(music_memory != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music_memory);
	}

	// TODO 6: Same as with the textures, use the proper RW loading function
	
	//music = Mix_LoadMUS(path); //from HD
	music_memory = Mix_LoadMUS_RW(App->fs->Load(path), 1); //from memory
	LOG("Load music %s from memory.\n", path);

	if(music_memory == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music_memory, -1, int(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music_memory, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;
	/*This can load WAVE, AIFF, RIFF, OGG, and VOC formats*/
	audiofx_memory = Mix_LoadWAV_RW(App->fs->Load(path),1);

	if(audiofx_memory == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.add(audiofx_memory);
		ret = fx.count();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if(id > 0 && id <= fx.count())
	{
		Mix_PlayChannel(-1, fx[id - 1], repeat);
	}

	return ret;
}

//change volume
bool j1Audio::ChangeVolume(unsigned int volumen)
{
	bool ret = false;

	volume = Mix_VolumeMusic(volumen);
	LOG("Volume was: %d \n", volume);
	LOG("Volume is now: %d \n", volume = Mix_VolumeMusic(-1));

	return ret;
}


// TODO 6: Create a method to load the state
// for now it will be camera's x and y
bool j1Audio::LoadGame(pugi::xml_node& node)
{
	bool ret = true;
	

	volume = node.child("volume").attribute("value").as_int();
	Mix_VolumeMusic(volume);
	LOG("XML read volume value: %d", volume = Mix_VolumeMusic(-1));
	

	return ret;
}

//Create a method to save the state of the renderer”
//Use append_child and append_attribute
bool j1Audio::SaveGame(pugi::xml_node& node) const
{
	bool ret = true;

	pugi::xml_node volumeNode;
	//create node camera inside node renderer
	volumeNode = node.append_child("volume");
	//create attribute x  inside node camera with int value
	volumeNode.append_attribute("value") = volume;
	LOG("XML save volume value: %d", volume);


	return ret;
}