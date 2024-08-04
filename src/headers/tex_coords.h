#ifndef TEX_COORDS_H
#define TEX_COORDS_H

// grass block texture coordinates
const int GRASS_TEX_COORDS[] = {
	// front
	0,  0,
	16, 0,
	0,  16,
	16, 16,

	// back
	16, 0,
	0,  0,
	16, 16,
	0,  16,

	// left
	0,  0,
	16, 0,
	0,  16,
	16, 16,

	// right
	0,  0,
	16, 0,
	0,  16,
	16, 16,

	// bottom
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// top
	32, 0,
	48, 0,
	32, 16,
	48, 16,
};
// dirt block texture coordinates
const int DIRT_TEX_COORDS[] = {
	// front
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// back
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// left
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// right
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// bottom
	16, 0,
	32, 0,
	16, 16,
	32, 16,

	// top
	16, 0,
	32, 0,
	16, 16,
	32, 16,
};
// stone block texture coordinates
const int STONE_TEX_COORDS[] = {
	// front
	48, 0,
	64, 0,
	48, 16,
	64, 16,

	// back
	48, 0,
	64, 0,
	48, 16,
	64, 16,

	// left
	48, 0,
	64, 0,
	48, 16,
	64, 16,

	// right
	48, 0,
	64, 0,
	48, 16,
	64, 16,

	// bottom
	64, 0,
	80, 0,
	64, 16,
	80, 16,

	// top
	80, 0,
	96, 0,
	80, 16,
	96, 16,
};
// SAND block texture coordinates
const int SAND_TEX_COORDS[] = {
	// front
	96, 0,
	112, 0,
	96, 16,
	112, 16,

	// back
	96, 0,
	112, 0,
	96, 16,
	112, 16,

	// left
	96, 0,
	112, 0,
	96, 16,
	112, 16,

	// right
	96, 0,
	112, 0,
	96, 16,
	112, 16,

	// bottom
	96, 0,
	112, 0,
	96, 16,
	112, 16,

	// top
	96, 0,
	112, 0,
	96, 16,
	112, 16,
};
// water block texture coordinates
const int WATER_TEX_COORDS[] = {
	// front
   240, 224,
	256, 224,
	240, 240,
	256, 240,

	// back
   240, 224,
	256, 224,
	240, 240,
	256, 240,

	// left
   240, 224,
	256, 224,
	240, 240,
	256, 240,

	// right
   240, 224,
	256, 224,
	240, 240,
	256, 240,

	// bottom
   240, 224,
	256, 224,
	240, 240,
	256, 240,

	// top
	112, 0,
	128, 0,
	112, 16,
	128, 16,
};
// wood log texture coordinates
const int LOG_TEX_COORDS[] = {
	// front
   32, 16,
	48, 16,
	32, 32,
	48, 32,

	// back
   32, 16,
	48, 16,
	32, 32,
	48, 32,

	// left
   32, 16,
	48, 16,
	32, 32,
	48, 32,

	// right
   32, 16,
	48, 16,
	32, 32,
	48, 32,

	// bottom
   48, 16,
	64, 16,
	48, 32,
	64, 32,

	// top
   48, 16,
	64, 16,
	48, 32,
	64, 32,
};
// leaves texture coordinates
const int LEAVES_TEX_COORDS[] = {
	// front
   64, 16,
	80, 16,
	64, 32,
	80, 32,

	// back
   64, 16,
	80, 16,
	64, 32,
	80, 32,

	// left
   64, 16,
	80, 16,
	64, 32,
	80, 32,

	// right
   64, 16,
	80, 16,
	64, 32,
	80, 32,

	// bottom
   64, 16,
	80, 16,
	64, 32,
	80, 32,

	// top
   64, 16,
	80, 16,
	64, 32,
	80, 32,
};

#endif
