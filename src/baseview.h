#ifndef BASEVIEW
#define BASEVIEW

#include <types.h>
#include <vdp.h>
#include <res/globals.h>

struct BaseView;

typedef struct {
	void ( *BaseView )( struct BaseView*, s16, s16, s16, s16 );
	void ( *destroy )( struct BaseView* );
	void ( *render )( struct BaseView* );
	void ( *position )( struct BaseView* );
	void ( *renderChildren )( struct BaseView* );
	void ( *addChildView )( struct BaseView*, struct BaseView* );
	void ( *setPlane )( struct BaseView*, u16 );
	void ( *placeTile )( struct BaseView*, s16, s16, u8, u16, bool, bool );
	void ( *placeTileSeries )( struct BaseView*, s16, s16, s16, s16, u8, u16, bool );
	bool ( *checkTileBoundary )( struct BaseView*, s16, s16 );
} BaseView_vtable;

typedef struct BaseView {
	u16 id;
	
	s16 x;
	s16 y;

	s16 width;
	s16 height;

	s16 absX;
	s16 absY;

	s16 scrollX;
	s16 scrollY;

	u8 numChildren;
	u16 plane;

	struct BaseView* parent;
	struct BaseView** children;

	vtable functions;
} BaseView;

void BaseView_ctor( BaseView* this, s16 x, s16 y, s16 width, s16 height );
void BaseView_dtor( BaseView* this );

u8 BaseView_testa( BaseView* this );

void BaseView_render( BaseView* this );
void BaseView_position( BaseView* this );
void BaseView_renderChildren( BaseView* this );
void BaseView_addChildView( BaseView* this, BaseView* childView );

void BaseView_setPlane( BaseView* this, u16 plane );
void BaseView_placeTile( BaseView* this, s16 x, s16 y, u8 pal, u16 tileIndex, bool flipV, bool flipH );

void BaseView_placeTileSeries( BaseView* this, s16 x, s16 y, s16 w, s16 h, u8 pal, u16 tileIndex, bool autoInc );

bool BaseView_checkTileBoundary( BaseView* this, s16 x, s16 y );

#endif
