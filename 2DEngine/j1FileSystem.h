#ifndef __j1FILESYSTEM_H__
#define __j1FILESYSTEM_H__

#include "j1Module.h"

struct SDL_RWops;

int close_sdl_rwops(SDL_RWops *rw);

class j1FileSystem : public j1Module
{
public:

	j1FileSystem(const char* game_path);

	// Destructor
	virtual ~j1FileSystem();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Open for Read/Write
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	//utility methods
	unsigned int Exist(const char* file) const;
	unsigned int Size(const char* fileptr) const;
	bool CreateDir(const char* directory, const char* writeDirectory) const;
	unsigned int IsDir(const char* fname) const;
	bool Delete(const char* file_or_dir, const char* writeDirectory) const;
	bool Mount(const char* newDir, const char* mountPoint = NULL) const;
	const char* SetWritingDir(const char* organization, const char* application) const;
	unsigned int BufferToFile(const char* file, const char* buffer, unsigned int size) const;
	const char* GetSaveDirectory() const
	{
		return "save/";
	}

	

private:

};

#endif // __j1FILESYSTEM_H__