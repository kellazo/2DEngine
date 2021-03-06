#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
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
	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;
	
	// Iterate all tilesets and draw all their 
	// images in 0,0 (you should have only one tileset for now)

	// accedim a la info a traves d'un punter que apunta a la informacio, cada vegada
	//TileSet* TileData = tile->data;
	
	//aqui pintem el tileset a la posicio 0,0
	//App->render->Blit(TileData->texture, 0, 0);

	//un cop hem mostrat la primera tile, passem a la seguent i anem iterant

	//si volem iterar tots els tilesets ho hem de fer a traves de la llsita de punters.
	p2List_item<TileSet*>* tileset = MapData.tilesets.start;

	//***????? punter a layer que apunta a la primera direccio de memoria de la llista de layers amb la info
	//Layer* layer = MapData.layers.start->data;
	
	//Make sure we draw all the layers and not just the first one
	p2List_item<Layer*>* layers = MapData.layers.start;
	//creem punter buit per guardar la info de la llista de layers
	Layer* layer = NULL;

	int coordenate;
	iPoint posWorld;
	iPoint posMap;
	//try to add custom properties to map
	if (MapData.properties.GetValue("Nodraw") != 0)
	{
		while (tileset != NULL)
		{
			//iterem les layers
			while (layers)
			{

				//iterem i guardem la info en la que esta al punter de layer
				layer = layers->data;

				for (int y = 0; y < MapData.height; y++)
				{
					for (int x = 0; x < MapData.width; x++)
					{


						coordenate = layer->Get(x, y);
						//be sure to ignore tiles of id == 0
						if (coordenate > 0)
						{	//which tileset belongs this cordenate?
							TileSet* tilesetpng = GetTilesetFromTileId(coordenate);

							if (tilesetpng != NULL)
							{

								SDL_Rect tilesetposition = tilesetpng->GetTileRect(coordenate);
								posWorld = MapToWorld(x, y);
								//posMap = WorldToMap(x, y);

								//Custom properties: Now use it to ask each layer if your �Draw� property is true.
								if (layer->properties.GetValue("Nodraw") != 0)
								{
									//pintem
									App->render->Blit(tilesetpng->texture, posWorld.x, posWorld.y, &tilesetposition);
								}

							}
						}

					}
				}
				// i passem al seguent node de la llista i aixins fins arribar al final
				layers = layers->next;
			}

			tileset = tileset->next;
		}
	}


	


}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Make sure you clean up any memory allocated
	// from tilesets / map

	// Remove all tilesets
	p2List_item<TileSet*>* tile;
	tile = MapData.tilesets.start;

	while (tile != NULL)
	{
		RELEASE(tile->data);
		tile = tile->next;
	}
	MapData.tilesets.clear();
	// Remove all layers
	p2List_item<Layer*>* layer;
	layer = MapData.layers.start;

	while (layer != NULL)
	{
		RELEASE(layer->data);
		layer = layer->next;
	}
	MapData.layers.clear();

	// Clean up the pugui tree
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
		if (ret == true)
		{
			LOG("TMX: Starting loading Layer information....\n");
			ret = LoadLayer();
			LOG("TMX:...finished loading Layer information.\n");
		}

		//load custom properties
		if (ret == true)
		{
			LOG("TMX: Starting loading properties information....\n");
			ret = LoadLayer();
			LOG("TMX:...finished loading properties information.\n");
		}

		//  LOG all the data loaded
		// iterate all tilesets and LOG everything
		if (ret == true)
		{
			LOG("Successfully put all info of (%s)TMX file into variables: \n", file_name);
			LOG("Map --------------");
			LOG("Size of map: width: %d in tiles x height: %d in tiles.", MapData.width, MapData.height);
			LOG("Orientation: %s map (%d).",orientation.GetString(), MapData.mapOrientaton);
			LOG("Render orderer: %s map (%d).", renderorder.GetString(), MapData.mapOrderer);
			LOG("Size of each tile: Width: %d of a tile Height: %d of a tile.", MapData.tilewidth, MapData.tileheight);
			LOG("Tilesets ---------------------");


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
			LOG("Layers -----------------------");
			p2List_item<Layer*>* layer = MapData.layers.start;
			while (layer != NULL)
			{
				
				Layer* layerData = layer->data;
				LOG("Layer ----");
				LOG("Name: %s ", layerData->name.GetString());
				LOG("Opacity: %f", layerData->opacity);
				LOG("Visible: %d", layerData->visible);
				LOG("Offset X: %d", layerData->offsetx);
				LOG("Offset Y: %d", layerData->offsety);
				LOG("Tile Width: %d", layerData->width);
				LOG("Tile Height: %d", layerData->height);
				layer = layer->next;
			}

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


		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		MapData.background_color.r = 0;
		MapData.background_color.g = 0;
		MapData.background_color.b = 0;
		MapData.background_color.a = 0;

		if (bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) MapData.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) MapData.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) MapData.background_color.b = v;
		}

		LoadProperties(map, MapData.properties);
			
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

	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}


	//si no haguessim fet servir punters sempre s'ens maxacaria i perdriem la informacio dels tileset que hi han al tmx. Nomes es guardaria l'ultim.
	//TilesetData.firstgid = tileset.attribute("firstgid").as_uint(); //for identificate which tileset is, because we can load all tilesets that we want
	//TilesetData.name.create(tileset.attribute("name").as_string());
	//TilesetData.tileheight = tileset.attribute("tileheight").as_uint();
	//TilesetData.tilewidth = tileset.attribute("tilewidth").as_uint();
	//TilesetData.spacing = tileset.attribute("spacing").as_uint();
	//TilesetData.margin = tileset.attribute("margin").as_uint();
	//TilesetData.tilecount = tileset.attribute("tilecount").as_uint();

	//Finalment afegim la info a la llista que tenim a map, d'aquesta manera tindrem un punter i podrem accediri.
	//MapData.tilesets.add(set);

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
		//les tiles en width
		set->num_tiles_width = set->tex_width / set->tilewidth;
		//les tiles en height
		set->num_tiles_height = set->tex_height / set->tileheight;
	}
	LOG("TMX: Finish loading image data tag....\n");
	return ret;
}

bool j1Map::LoadLayer()
{
	bool ret = true;

	for (pugi::xml_node layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		Layer* set = new Layer();

		if (ret == true)
		{
			ret = LoadLayerInfo(layer, set);
		}

		if (ret == true)
		{
			ret = LoadLayerData(layer, set);
		}
		
		MapData.layers.add(set);
	}

	return ret;
}

bool j1Map::LoadLayerInfo(pugi::xml_node& layer_node, Layer* set)
{
	bool ret = true;
	LOG("TMX: Starting loading layer data tag....\n");

	set->name.create(layer_node.attribute("name").as_string());
	set->visible = layer_node.attribute("visible").as_bool();
	set->opacity = layer_node.attribute("opacity").as_float();
	set->offsetx = layer_node.attribute("offsetx").as_uint();
	set->offsety = layer_node.attribute("offsety").as_uint();
	set->width = layer_node.attribute("width").as_uint();
	set->height = layer_node.attribute("height").as_uint();

	//Add the calls to this method so we load custom properties for each layer
	LoadProperties(layer_node, set->properties);


	LOG("TMX: Finish loading layer data tag....\n");
	return ret;
}

bool j1Map::LoadLayerData(pugi::xml_node& layer_node, Layer* set)
{
	bool ret = true;
	LOG("TMX: Starting loading data tag inside node layer....\n");

	pugi::xml_node data = layer_node.child("data");

	if (data == NULL)
	{
		LOG("Pugi: Error parsing map tmx file: Cannot find 'data' node.");
		ret = false;
	}
	else
	{
	//encoding: The encoding used to encode the tile layer data.When used, it can be "base64" and "csv" at the moment.

		p2SString type_encoding;
		type_encoding.create("%s",data.attribute("encoding").as_string());

		/*What will be the size of that array ? � how many do you have in the TMX
		file ?*/
		uint size = MapData.width*MapData.height;

		set->datainfo = new uint[size];

		//Once the array is allocated, use memset to fill it with zeroes
		memset(set->datainfo, 0, size);

		if (type_encoding == "csv")
		{
			
			
			// to read content
			const char* string = data.text().get();
			//p2SString sizet;
			//sizet.create(string);
			//p2SString size_string;
			//size_string.create(string);
			//to separate words by tokens, function atoi is for pas char to int
			char* token = NULL;
			char* next_token = NULL;
			char  delims[] = " ,\t\n";

			token = strtok_s((char*)string, delims, &next_token);
			int i = 0;
			
		
			while (token != NULL)
			{

				if (token != NULL)
				{
					

					for (i; i < size; i++)
					{
						token = strtok_s(NULL, delims, &next_token);
						if (token)
						{
							set->datainfo[i] = atoi(token);
							//LOG("%d", set->datainfo[i]);
						}
						else
						{
							LOG("Token is null");
						}
							

					}
				}
				else
					LOG("ERROR with token");
			}
			
			ret = true;

		}

		else if (type_encoding == "base64")
		{
			
			ret = true;
		}
		else if (type_encoding == "undefined")
		{
			LOG("TMX: type_encoding is undefined....\n");
			ret = false;
		}
		else
		{
			int i = 0;
			for (pugi::xml_node tile = data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				set->datainfo[i++] = tile.attribute("gid").as_int(0);
			}
			ret = true;

		}
			
	}

	LOG("TMX: Finish loading data tag inside node layer....\n");
	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect position;
	//firstgid sempre es 1, i el id es 0 en la primera iteraci�. Per tal de tenir el Id del tileset.
	int idTileset = id - firstgid;
	// ample de la imatge del tileset
	position.w = tilewidth;
	// al�ada de la imatge del tileset
	position.h = tileheight;
	//The last one, modulo operator, represented by a percentage sign (%), gives the remainder of a division of two values. For example:
	//x = 11 % 3;
	//results in variable x containing the value 2, since dividing 11 by 3 results in 3, with a remainder of 2.
	//We use this because we want our value of x to reset back to 0 at the start of each row, so we draw the respective tile on the far left :
	
	position.x = margin + (position.w + spacing) * (idTileset % num_tiles_width);
	//As for y, we take the floor() of i / w(that is, we round that result down) because we only want y to increase once we have reached the end of each row and moved down one level :https://gamedevelopment.tutsplus.com/tutorials/an-introduction-to-creating-a-tile-map-engine--gamedev-10900
	position.y = margin + (position.h + spacing) * (idTileset / num_tiles_width);


	return position;
}
//�Create a method that translates x,y coordinates from map positions to world positions�
iPoint j1Map::MapToWorld(int x, int y) const
{
	
	iPoint worldPoint;
	//You only really need the size of the tiles
	if (MapData.mapOrientaton == ORTHOGONAL)
	{
		worldPoint.x = x * MapData.tilewidth;
		worldPoint.y = y * MapData.tileheight;
		
	}
	// Add isometric map to world coordinates
	else if (MapData.mapOrientaton == ISOMETRIC)
	{
		worldPoint.x = (x - y) * (MapData.tilewidth * 0.5f);
		worldPoint.y = (x + y) * (MapData.tileheight * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		worldPoint.x = x; worldPoint.y = y;
	}
	

	return worldPoint;
}
//(x and y ) are screen position values, there are coordenates of screen
iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint mapPoint(0, 0);
	// Add orthographic world to map coordinates It is just the inverse math from MapToWorld
	if (MapData.mapOrientaton == ORTHOGONAL)
	{
		mapPoint.x = x / MapData.tilewidth;
		mapPoint.y = y / MapData.tileheight;

	}
	// Add the case for isometric maps to WorldToMap(world to map coordinates)
	else if (MapData.mapOrientaton == ISOMETRIC)
	{
		float half_width = MapData.tilewidth * 0.5f;
		float half_height = MapData.tileheight * 0.5f;

		mapPoint.x = int((x / half_width + y / half_height) / 2);
		mapPoint.y = int((y / half_height - (x / half_width)) / 2);
		
	}
	else
	{
		LOG("Unknown map type");
		mapPoint.x = x; mapPoint.y = y;
	}

	return mapPoint;
}

//Which tileset belongs this tile?
TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	// Complete this method so we pick the right
	// Tileset based on a tile id

	//first iterate through the tilesets with lists
	p2List_item<TileSet*>* tile = MapData.tilesets.start;
	//this is the tileset that we want
	TileSet* set = NULL;
	while (tile)
	{
		if (id < tile->data->firstgid)
		{
			//LOG("tileset");
			set = tile->prev->data;
			//LOG("tilesettete");
			break;
		}
		set = tile->data; //?
		tile = tile->next;
	}
	
	

	return set;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& property, Properties& properties)
{
	bool ret = false;
	//  Fill in the method to fill the custom properties from 
	// an xml_node
	pugi::xml_node data = property.child("properties");

	LOG("TMX: Starting loading properties tag....\n");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();
			LOG("TMX: Starting loading property tag....\n");
			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();


			properties.properties.add(p);
			LOG("TMX: Finish loading property tag....\n");
		}
	}
	


	LOG("TMX: Finish loading properties tag....\n");

	return ret;
}

// Add a method to custom properties struct.It should be to request any value. It must accept a default value in case it is not found(in the lines of pugui xml).
int Properties::GetValue(const char* value, int default_value) const
{
	p2List_item<Property*>* item = properties.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}