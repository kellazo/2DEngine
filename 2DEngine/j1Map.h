#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


//primer va la estructura de Tileset abans que la de Mapdata pk farem una llista.
// Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{

	/*firstgid: The first global tile ID of this tileset(this global ID maps to the first tile in this tileset).
	source : If this tileset is stored in an external TSX(Tile Set XML) file, this attribute refers to that file.That TSX file has the same structure as the <tileset> element described here. (There is the firstgid attribute missing and this source attribute is also not there.These two attributes are kept in the TMX map, since they are map specific.)
	name : The name of this tileset.
	tilewidth : The(maximum) width of the tiles in this tileset.
	tileheight : The(maximum) height of the tiles in this tileset.
	spacing : The spacing in pixels between the tiles in this tileset(applies to the tileset image).
	margin : The margin around the tiles in this tileset(applies to the tileset image).
	tilecount : The number of tiles in this tileset(since 0.13)
	columns : The number of tile columns in the tileset.For image collection tilesets it is editable and is used when displaying the tileset. (since 0.15)*/

	//TileSet() {} //constructor
	p2SString name;
	uint firstgid;
	//uchar name;
	uint tilewidth;
	uint tileheight;
	uint spacing;
	uint margin;
	uint tilecount;
	//uint columns;
	SDL_Texture* texture;
	//relacionat amb la texture
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;


	//~TileSet() {} //destructor
};

enum orientation
{
	UNKNOWN = 0,
	ORTHOGONAL,
	ISOMETRIC,
	STAGGERED,
	HEXAGONAL
};

enum renderorder
{
	NOTHING = 0,
	RIGHT_DOWN,
	RIGHT_UP,
	LEFT_DOWN,
	LEFT_UP
};

// Create a struct needed to hold the information to Map node
struct Map
{
	/*version: The TMX format version, generally 1.0.
	orientation : Map orientation.Tiled supports "orthogonal", "isometric", "staggered" (since 0.9) and "hexagonal" (since 0.11).
	renderorder : The order in which tiles on tile layers are rendered.Valid values are right - down(the default), right - up, left - down and left - up.In all cases, the map is drawn row - by - row. (since 0.10, but only supported for orthogonal maps at the moment)
	width : The map width in tiles.
	height : The map height in tiles.
	tilewidth : The width of a tile.
	tileheight : The height of a tile.
	hexsidelength : Only for hexagonal maps.Determines the width or height(depending on the staggered axis) of the tile's edge, in pixels.
	staggeraxis : For staggered and hexagonal maps, determines which axis("x" or "y") is staggered. (since 0.11)
	staggerindex : For staggered and hexagonal maps, determines whether the "even" or "odd" indexes along the staggered axis are shifted. (since 0.11)
	backgroundcolor : The background color of the map. (since 0.9, optional, may include alpha value since 0.15 in the form #AARRGGBB)
	nextobjectid : Stores the next available ID for new objects.This number is stored to prevent reuse of the same ID after objects have been removed. (since 0.11)*/
	
	
	
	//Map() {} //constructor
	
	

	float version;
	orientation	mapOrientaton;
	renderorder mapOrderer;
	uint width;
	uint height;
	uint tilewidth;
	uint tileheight;
	//uint hexsidelength;
	//uchar staggeraxis;
	//uchar staggerindex;
	//uchar backgroundcolor;
	uint nextobjectid;

	//com que podem carregar mes dun tileset en el TMX i necesitem agafar la info que contenem. fem una llista de punters a l'estrucutra Tileset que conte tota la info parsejada
	p2List<TileSet*>	tilesets;

	//~Map() {} //destructor
};


// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

private:
	//fill mapa data to variables mapdata
	bool LoadMap();
	//fill all tilesets information
	bool LoadTileset();
	//fill all tileset information data tag
	bool LoadTilesetData(pugi::xml_node& tileset_node, TileSet* set);
	//fill all tileset information image tag
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	


public:

	//Add your struct for map info as public for now
	Map MapData;
	//TileSet TilesetData;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	p2SString			fullPathMap;


	p2SString           orientation;
	p2SString			renderorder;
};

#endif // __j1MAP_H__