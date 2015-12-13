#ifndef PAL_MANAGER
#define PAL_MANAGER

#include <types.h>
#include <res/globals.h>
#include <vdp.h>

#define		VDPManager_TILE_USERINDEX	TILE_USERINDEX
#define		VDPManager_TILE_MAX			0x01FF

typedef u16  VDPManager_TileIndex;
typedef u32* VDPManager_Tiles;
typedef u16* VDPManager_Palette;

typedef enum {
	VDPManager_Palette_INVALID = -1,
	VDPManager_Palette_PAL0 = PAL0,
	VDPManager_Palette_PAL1,
	VDPManager_Palette_PAL2,
	VDPManager_Palette_PAL3
} VDPManager_PaletteIndex;

typedef struct VDPManager_VDPRamSegment {
	VDPManager_Tiles tileData;
	VDPManager_TileIndex index;
	u16 length;
} VDPManager_VDPRamSegment;

typedef struct VDPManager {
	u16* palettes[ 3 ];
	VDPManager_VDPRamSegment* freeVDPSegments;
	u16 usedSegmentCount;
} VDPManager;

void VDPManager_ctor( VDPManager* this );
void VDPManager_dtor( VDPManager* this );

/**
 * Load tiles into VDP RAM. The VDP manager will select a contiguous region and return
 * the numeric index of the item stored in VDP RAM.
 */
VDPManager_TileIndex VDPManager_loadTiles( VDPManager* this, VDPManager_Tiles tiles, u16 count );

/**
 * Search for the region of tiles and remove it, marking this region of VDP RAM free to use.
 */
void VDPManager_unloadTiles( VDPManager* this, VDPManager_TileIndex index );

/**
 * Load a palette into VDP. The VDP manager will select one of the four palettes
 * to use, and return the index used (from 0-3). If all palettes are in use, the palette
 * is unable to be stored, and this will return VDPManager_Palette_INVALID.
 */
VDPManager_PaletteIndex VDPManager_loadPalette( VDPManager* this, VDPManager_Palette palette );

/**
 * Marks a selected palette as free for use.
 */
void VDPManager_unloadPalette( VDPManager* this, VDPManager_PaletteIndex palette );

/**
 * Comparator for the qsort() function, used to sort VDPManager_VDPRamSegments
 */
int VDPManager_qsortComparator( const void* firstItem, const void* secondItem );
#endif