#include "p2Defs.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1FileSystem.h"
#include "SDL/include/SDL_rwops.h"
#include "SDL/include/SDL_filesystem.h"
#include "PhysFS/include/physfs.h"
//#include "p2DynArray.h"
//#include "p2SString.h"


#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

j1FileSystem::j1FileSystem(const char* game_path) : j1Module()
{
	name.create("file_system");

	// TODO 1: Init PhysFS lib
	// need to be created before Awake so other modules can use it
	char* base_path = SDL_GetBasePath();
	int physfs =PHYSFS_init(base_path);
	SDL_free(base_path);
	/*char* base_path = SDL_GetBasePath();
	int physfs = PHYSFS_init(base_path);
	SDL_free(base_path);*/
	if (physfs == 0)
	{ 
		LOG("PhysFS could not initialize! PhysFS_ERROR: %s\n", PHYSFS_getLastError());
		
	}
	else
	{
		LOG("PhysFS initializated\n");
		
	}
		

	// TODO 2: Mount directory ".", then mount "data.zip"
	// files in the folder should take precendence over the zip file!

	//const char* newDir = ".";
	IsDir(game_path);
	Mount(game_path);
	
	const char* newDir2 = "data.zip";
	Exist(newDir2);
	IsDir(newDir2);
	Mount(newDir2);
	
	
		
}

// Destructor
j1FileSystem::~j1FileSystem()
{
	// TODO 1: stop PhysFS lib
	PHYSFS_deinit();

}

// Called before render is available
bool j1FileSystem::Awake(pugi::xml_node& node)
{
	LOG("Loading Physic File System");
	bool ret = true;

	// This is a good place to setup the write dir (homework)
	const char* write_path = SetWritingDir(App->GetOrganization(), App->GetApplicationName());
	// We add the writing directory as a reading directory too with speacial mount point
	IsDir(write_path);
	Mount(write_path, GetSaveDirectory());
	IsDir(GetSaveDirectory());

	//CreateDir("Mapoos",write_path);
	//Delete("Mapoos",write_path);
	
	
	//p2SString texte = "Test for save purpose";
	//BufferToFile("testSave.txt", texte.GetString(), texte.Length());

	

	
	return ret;
}

// Read a whole file and put it in a new buffer
unsigned int j1FileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	Exist(file);
	IsDir(file);
	// TODO 3: Read a file and allocate needed bytes to buffer and write all data into it
	// return the size of the data
	PHYSFS_File* fileptr = PHYSFS_openRead(file);
	if (fileptr != NULL)
	{
		if (fileptr == 0)
		{
			LOG("PhysFS: Filename not open for Read.(%s) :%s \n", file,PHYSFS_getLastError());
		}
		else
			LOG("PhysFS: Filename open for Read. (%s) \n",file);

		PHYSFS_sint64 file_size = (PHYSFS_sint64)Size((const char*)fileptr);
		
		if (file_size > 0)
		{
			*buffer = new char[(uint)file_size];

			PHYSFS_sint64 load_in_memory = PHYSFS_read(fileptr, *buffer, 1, (PHYSFS_sint32)file_size);
			if (load_in_memory != file_size)
			{
				LOG("PhysFS: error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE(buffer);
			}
			else
			{
				ret = (uint)load_in_memory;
				LOG("PhysFS: File (%s) with size: %d bytes, read in memory[Allocated in memory(buffer=Size of file)]\n", file,ret);
			}
			
		}
		if (PHYSFS_close(fileptr) == 0)
			LOG("PhysFS: error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("PhysFS: error while opening file %s: %s\n", file, PHYSFS_getLastError());
	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* j1FileSystem::Load(const char* file) const
{
	// TODO 4: Using our previous Load method
	// create a sdl rwops using SDL_RW_From_ConstMem()
	// and return it if everything goes well

	//create a new buffer
	char* buffer;
	//pass the new buffer to load function
	unsigned int size = Load(file, &buffer);

	if (size > 0)
	{	//Use this function to prepare a read-only memory buffer for use with RWops.
		SDL_RWops* read_in_memory = SDL_RWFromConstMem(buffer, size);
		
		if (read_in_memory != NULL)
		{
			LOG("PhysFS with RWops: File %s is free automatically.\n", file);
			read_in_memory->close = close_sdl_rwops;  //wtf free memory?¿?¿?

			
		}
		LOG("PhysFS with RWops: File %s read in memory.\n", file);
		return read_in_memory;
	
	}
	else
		LOG("PhysFS with RWops: error while reading in memory %s: %s\n", file, PHYSFS_getLastError());
		return NULL;
	
}

// TODO 4: you will need to use this small helper function
// to make sure all data is freed automatically
int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

//Utility methods

// Check if a file exists
unsigned int j1FileSystem::Exist(const char* file) const
{
	unsigned int exist;

	exist = PHYSFS_exists(file);
	if (exist > 0)
	{
		LOG("PhysFS: File %s, exists!\n", file);
		return exist;
	}
	else
		LOG("PhysFS: error, file %s doesn't exists: %s\n", file, PHYSFS_getLastError());
	return exist;
	
}

// Calculate the size of file
unsigned int j1FileSystem::Size(const char* fileptr) const
{
	PHYSFS_sint64 file_size = PHYSFS_fileLength((PHYSFS_File*)fileptr);
	if (file_size == -1)
	{
		LOG("PhysFS: File size can't be determined :%s \n", PHYSFS_getLastError());
		return (uint)file_size;
	}
	else
		LOG("PhysFS: Size of file. (%d Bytes) \n", file_size);
	return (uint)file_size;
}

//Create a directory.
//This is specified in platform - independent notation in relation to the write dir.All missing parent directories are also created if they don't exist.
//So if you've got the write dir set to "C:\mygame\writedir" and call PHYSFS_mkdir("downloads/maps") then the directories
//"C:\mygame\writedir\downloads" and "C:\mygame\writedir\downloads\maps" will be created if possible. If the creation of "maps"
//fails after we have successfully created "downloads", then the function leaves the created directory behind and reports failure.
//Parameters:dirName -New dir to create.
//Returns :nonzero on success, zero on error.
bool j1FileSystem::CreateDir(const char* dirName, const char* writeDirectory) const
{
	bool ret = true;

	int directory = PHYSFS_mkdir(dirName);

	if (directory > 0)
	{
		LOG("PhysFS: New directory with name '%s' is a created in the write directory: %s\n", dirName, writeDirectory);
		return ret;
	}
	else
	{
		LOG("PhysFS: Error creating directory '%s' in %s ERROR: %s.\n", dirName, writeDirectory, PHYSFS_getLastError());
		ret = false;
	}
		
	return ret;

}

// Check if a file is a directory
//Determine if a file in the search path is really a directory. non-zero if filename exists and is a directory. zero otherwise.
unsigned int j1FileSystem::IsDir(const char* fname) const
{
	unsigned int isDirectory = PHYSFS_isDirectory(fname);
	
	if (isDirectory > 0)
	{
		LOG("PhysFS:( %s ) Is a directory.\n", fname);
		return isDirectory;
	}
	else
		LOG("PhysFS: (%s) is not a directory. \n", fname);
	return isDirectory;

}

//Delete a file or directory on the writedirectory A directory must be empty before this call can delete it, filename not
bool j1FileSystem::Delete(const char* file_or_dir, const char* writeDirectory) const
{
	bool ret = true;

	int fileDir = PHYSFS_delete(file_or_dir);

	if (fileDir > 0)
	{
		LOG("PhysFS: Filename/Directory with name '%s' was deleted in the write directory: %s\n", file_or_dir, writeDirectory);
		return ret;
	}
	else
	{
		LOG("PhysFS: Error deleting directory/filename '%s' in %s ERROR: %s.\n", file_or_dir, writeDirectory, PHYSFS_getLastError());
		ret = false;
	}

	return ret;
}

//Add an archive or directory to the search path.
//newDir 	directory or archive to add to the path, in platform - dependent notation.
//mountPoint 	Location in the interpolated tree that this archive will be "mounted", in platform - independent notation.NULL or "" is equivalent to "/".
//appendToPath 	nonzero to append to search path, zero to prepend.
bool j1FileSystem::Mount(const char* newDir, const char* mountPoint) const
{
	bool ret = false;
	
	if (PHYSFS_mount(newDir, mountPoint, 1) == 0)

		LOG("PhysFS: File System error while adding a path or zip(%s): %s\n", newDir, PHYSFS_getLastError());

	else

		LOG("PhysFS mounted in path/file: (%s) \n", newDir);
		ret = true;

	return ret;

}

//Path to write files 
const char* j1FileSystem::SetWritingDir(const char* organization, const char* application) const
{
	
	organization = App->GetOrganization();
	application = App->GetApplicationName();
	// Ask SDL for a write dir
	/*Use this function to get the "pref dir".This is meant to be where the application 
	can write personal files(Preferences and save games, etc.) that are specific to the application.This directory is unique per user and per application.*/
	char* write_path = SDL_GetPrefPath(organization, application);
	/*Tell PhysicsFS where it may write files.
	Set a new write dir.This will override the previous setting.
	This call will fail(and fail to change the write dir) if the current write dir still has files open in it.
	Parameters: newDir 	The new directory to be the root of the write dir, specified in platform - dependent notation.
	Setting to NULL disables the write dir, so no files can be opened for writing via PhysicsFS.
	Returns : non - zero on success, zero on failure.*/
	if (PHYSFS_setWriteDir(write_path) == 0)
		LOG("PhysFS: File System error while creating write dir in: %s Error:\n",write_path, PHYSFS_getLastError());
	else
	{
		LOG("PhysFS: Writing directory is %s\n", write_path);
		
	}

	SDL_free(write_path);

	return write_path;

}

//Write data to a PhysicsFS filehandle.(Save a whole buffer to disk)
unsigned int j1FileSystem::BufferToFile(const char* file, const char* buffer, unsigned int size) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if (fs_file != NULL)
	{
		PHYSFS_sint64 written = PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
			LOG("PhysFS: File System error while writing to file %s: %s\n", file, PHYSFS_getLastError());
		else
			LOG("PhysFS: File '%s' written(saved) in disk. Lenght of Buffer saved to disk %d chars\n", file, written);
			ret = (uint)written;

		if (PHYSFS_close(fs_file) == 0)
			LOG("PhysFS: File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("PhysFS: File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}