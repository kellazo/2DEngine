#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Config.xml: Loading Map Parser");
	bool ret = true;
	// see node map, inside map there are a node that his name is folder, so put in folder var the text there are between folder node that is (maps/)
	folder.create(config.child("folder").child_value());
	Load("hello.tmx");
	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;
	// Iterate all tilesets and draw all their 
	// images in 0,0 (you should have only one tileset for now)

	p2List_item<TileSet*>* tileset = MapData.tilesets.start;
	while (tileset != NULL)
	{
		// accedim a la info a traves d'un punter que apunta a la informacio, cada vegada
		//TileSet* TileData = tile->data;

		//aqui pintem el tileset a la posicio 0,0
		//App->render->Blit(TileData->texture, 0, 0);

		//un cop hem mostrat la primera tile, passem a la seguent i anem iterant
		//App->render->Blit(tileset->data->texture, 0, 0);

		for (int y = 0; y < MapData.height; y++)
		{
			for (int x = 0; x < MapData.width; x++)
			{

				
				
				App->render->Blit(tileset->data->texture, 0, 0);
				
			}
		}



		tileset = tileset->next;
	}


	


}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// TODO 2: Make sure you clean up any memory allocated
	// from tilesets / map
	//MapData.~Map();
	//TilesetData.~TileSet();

	// Remove all tilesets
	/*p2List_item<TileSet*>* tile;
	tile = MapData.tilesets.start;

	while (tile != NULL)
	{
		RELEASE(tile->data);
		tile = tile->next;
	}
	MapData.tilesets.clear();*/


	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	//fullPathMap var is the full path text inside node folder: maps/hello.tmx
	fullPathMap.create("%s%s", folder.GetString(), file_name);
	//load tmx in buffer for read
	char* buffer;
	LOG("PhysFS: Loading tmx:( %s ) in buffer...", fullPathMap.GetString());
	int size = App->fs->Load(fullPathMap.GetString(), &buffer);
	//check if buffer there are something to read
	if (size > 0)
	{
		LOG("Pugi: Loading tmx:( %s ) in buffer to parse...", fullPathMap.GetString());
		pugi::xml_parse_result result = map_file.load_buffer(buffer, size);
		//release memory xml
		RELEASE(buffer);

		if (result == NULL)
		{
			LOG("Pugi_XML: Could not load tmx file %s. pugi error: %s", file_name, result.description());
			ret = false;
		}

		if (ret == true)
		{
			LOG("Pugi_XML: tmx %s parsed without errors.\n TMX: Starting loading map tag data....\n", file_name);
			//Create and call a private function to load and fill
			// all your map data
			ret = LoadMap();
			LOG("Pugi_XML:...finished loading map tag data.\n");
		}

		
		if (ret == true)
		{
			LOG("TMX: Starting loading tileset information....\n");
			// TODO 4: Create and call a private function to load a tileset
			// remember to support more any number of tilesets!
			ret = LoadTileset();
			LOG("TMX:...finished loading tileset information.\n");
		}
		

		// TODO 5: LOG all the data loaded
		// iterate all tilesets and LOG everything
		if (ret == true)
		{
			LOG("Successfully put all info of (%s)TMX file into variables: \n", file_name);
			LOG("Map --------------");
			LOG("Size of map: width: %d in tiles x height: %d in tiles.", MapData.width, MapData.height);
			LOG("Orientation: %s map (%d).",orientation.GetString(), MapData.mapOrientaton);
			LOG("Render orderer: %s map (%d).", renderorder.GetString(), MapData.mapOrderer);
			LOG("Size of each tile: Width: %d of a tile Height: %d of a tile.", MapData.tilewidth, MapData.tileheight);
			LOG("Tilesets --------------");


			//Aqui iterem la llista de tilesets per que poguem llegir totes les tiles
			p2List_item<TileSet*>* tile = MapData.tilesets.start;
			while (tile != NULL)
			{
				// accedim a la info a traves d'un punter que apunta a la informacio, cada vegada
				TileSet* TileData = tile->data;
				LOG("Tileset ----");
				LOG("Name: %s. Firstgid: %d", TileData->name.GetString(), TileData->firstgid);

				LOG("Width: %d (The(maximum) width of the tiles in this tileset.)", TileData->tilewidth);
				LOG("Height: %d (The(maximum) height of the tiles in this tileset.)", TileData->tileheight);
				LOG("Total tiles in this tileset %d", TileData->tilecount);
				LOG("Spacing: %d Margin: %d", TileData->spacing, TileData->margin);

				//un cop hem mostrat la primera tile, passem a la seguent i anem iterant
				tile = tile->next;
			}

			/*LOG("Name: %s. Firstgid: %d", TilesetData.name.GetString(), TilesetData.firstgid);
			LOG("Width: %d (The(maximum) width of the tiles in this tileset.)",TilesetData.tilewidth);
			LOG("Height: %d (The(maximum) height of the tiles in this tileset.)", TilesetData.tileheight);
			LOG("Total tiles in this tileset %d", TilesetData.tilecount);
			LOG("Spacing: %d Margin: %d", TilesetData.spacing, TilesetData.margin);*/
		}
	}
	else
		LOG("PhysFS: Could not load %s TMX file in buffer.\n", fullPathMap.GetString());
	
	map_loaded = ret;

	return ret;
}

bool j1Map::LoadMap()
{
	bool ret = true;
	//parsing node map inside tmx file
	pugi::xml_node map = map_file.child("map");
	if (map == NULL)
	{
		LOG("Pugi: Error parsing map tmx file: Cannot find 'map' node.");
		ret = false;
	}
	else
	{

		MapData.width = map.attribute("width").as_uint();
		MapData.height = map.attribute("height").as_uint();
		MapData.nextobjectid = map.attribute("nextobjectid").as_uint();
		MapData.tileheight = map.attribute("tileheight").as_uint();
		MapData.tilewidth = map.attribute("tilewidth").as_uint();
		MapData.version = map.attribute("version").as_float();

		
		orientation.create(map.attribute("orientation").as_string());
		
		if (orientation == "orthogonal")
		{
			MapData.mapOrientaton = ORTHOGONAL;
		}
			
		else if (orientation == "isometric")
		{
			MapData.mapOrientaton = ISOMETRIC;
		}
			
		else if (orientation == "staggered")
		{
			MapData.mapOrientaton = STAGGERED;
		}
			
		else if (orientation == "hexagonal")
		{
			MapData.mapOrientaton = HEXAGONAL;
		}
			
		else
		{
			MapData.mapOrientaton = UNKNOWN;
		}

		
		renderorder.create(map.attribute("renderorder").as_string());

		if (orientation == "right-down")
		{
			MapData.mapOrderer = RIGHT_DOWN;
		}

		else if (orientation == "right-up")
		{
			MapData.mapOrderer = RIGHT_UP;
		}

		else if (orientation == "left-down")
		{
			MapData.mapOrderer = LEFT_DOWN;
		}

		else if (orientation == "left-up")
		{
			MapData.mapOrderer = LEFT_UP;
		}

		else
		{
			MapData.mapOrderer = NOTHING;
		}

			
	}


	return ret;
}

bool j1Map::LoadTileset()
{
	bool ret = true;
	
	
	//iterem totes les tileset
	for (pugi::xml_node tileset = map_file.child("map").child("tileset"); tileset; tileset = tileset.next_sibling("tileset"))
	{
		//creem punter a Tileset 
		TileSet* set = new TileSet();
		
		//omplim
		if (ret == true)
		{
			ret = LoadTilesetData(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		MapData.tilesets.add(set);
	}
	


	return ret;
}

bool j1Map::LoadTilesetData(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	LOG("TMX: Starting loading tileset data tag....\n");
	set->firstgid = tileset_node.attribute("firstgid").as_uint();
	set->name.create(tileset_node.attribute("name").as_string());
	set->tileheight = tileset_node.attribute("tileheight").as_uint();
	set->tilewidth = tileset_node.attribute("tilewidth").as_uint();
	set->spacing = tileset_node.attribute("spacing").as_uint();
	set->margin = tileset_node.attribute("margin").as_uint();
	set->tilecount = tileset_node.attribute("tilecount").as_uint();

	//si no haguessim fet servir punters sempre s'ens maxacaria i perdriem la informacio dels tileset que hi han al tmx. Nomes es guardaria l'ultim.
	//TilesetData.firstgid = tileset.attribute("firstgid").as_uint(); //for identificate which tileset is, because we can load all tilesets that we want
	//TilesetData.name.create(tileset.attribute("name").as_string());
	//TilesetData.tileheight = tileset.attribute("tileheight").as_uint();
	//TilesetData.tilewidth = tileset.attribute("tilewidth").as_uint();
	//TilesetData.spacing = tileset.attribute("spacing").as_uint();
	//TilesetData.margin = tileset.attribute("margin").as_uint();
	//TilesetData.tilecount = tileset.attribute("tilecount").as_uint();

	//Finalment afegim la info a la llista que tenim a map, d'aquesta manera tindrem un punter i podrem accediri.
	MapData.tilesets.add(set);

	LOG("TMX: Finish loading tileset data tag....\n");

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	LOG("TMX: Starting loading image data tag....\n");
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Pugi: Error parsing map tmx file: Cannot find 'image' node.");
		ret = false;
	}
	else
	{
		p2SString path;
		path.create("%s%s", folder.GetString(), image.attribute("source").as_string());
		
		set->texture = App->tex->Load(path.GetString());
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tilewidth;
		set->num_tiles_height = set->tex_height / set->tileheight;
	}
	LOG("TMX: Finish loading image data tag....\n");
	return ret;
}
