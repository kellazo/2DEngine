#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CreateQuad(int x, int y) const;
	bool Collision(SDL_Rect obj1, SDL_Rect obj2);

	unsigned int volume;
	p2SString MapInfo;
	//p2SString map;
	const char* map;


	
private:
	SDL_Texture* img;
	SDL_Rect rectangle;
	SDL_Rect rectangleverde;
};

#endif // __j1SCENE_H__