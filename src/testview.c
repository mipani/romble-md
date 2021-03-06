#include <testview.h>
#include <baseview.h>
#include <boxview.h>
#include <simpletextview.h>
#include <types.h>
#include <res/globals.h>
#include <romble.h>
#include <lang.h>
#include <stdlib.h>
#include <vdp.h>
#include <joymanager.h>
#include <joy.h>
#include <buttonview.h>
#include <sizedarray.h>
#include <bit_trie.h>
#include <utility.h>
#include <vdpmanager.h>
#include <tags.h>
#include <eventmanager.h>
#include <log.h>

static char* TestView_HEADER = "Romble Event Tester";

static char* TestView_STRING_HI = "Hi!               ";
static char* TestView_STRING_BYE = "Bye!              ";
static char* TestView_STRING_DEFAULT = "(Press any button)";
static char* TestView_STRING_OBSCENE = "Watch ur language!";

static char* TestView_SAY_HI = "Say Hi";
static char* TestView_SAY_BYE = "Say Bye";
static char* TestView_SAY_OBSCENE = "OH SHIT!";
static char* TestView_SAY_RESET = "Clear Text";

static char* TestView_BUTTON_LOG = "debug action-check event log";

TestView_vtable TestView_table = {
	TestView_dtor,
	TestView_render,
	BaseView_position,
	BaseView_renderChildren,
	BaseView_addChildView,
	BaseView_placeTile,
	BaseView_placeTileSeries,

	BaseView_listenToView,
	BaseView_stopListeningView,
	BaseView_trigger,

	BaseView_getChildById,
	BaseView_getChildByTag,
	BaseView_removeChild,

	BaseView_checkTileBoundary,

	TestView_setupChildren
};

void TestView_ctor( TestView* this, s16 x, s16 y, s16 width, s16 height ) {
	BaseView_ctor( ( BaseView* ) this, x, y, width, height );
	CLASS( BaseView, this )->functions = &TestView_table;

	// Determine this->buttonStyle by searching for tile tag TILES_BOX_DRAWING_BETA
	// If it's not in the VDPManager, load it into the VDPManager
	// TILES_BOX_DRAWING_BETA uses the system palette
	this->buttonStyle = VDPManager_getTilesByTag( vdpManager, TILES_BOX_DRAWING_BETA );
	if( this->buttonStyle == VDPManager_INDEX_NULL ) {
		// Need to load the tiles
		this->buttonStyle = VDPManager_loadTiles( vdpManager, BoxDrawingCharacters, 3, TILES_BOX_DRAWING_BETA );
	}

	CLASS( BaseView, this )->events = Romble_alloc_d( sizeof( EventManager ), TRUE, FILE_LINE() );
	EventManager_ctor( CLASS( BaseView, this )->events );

	FUNCTIONS( TestView, BaseView, this )->setupChildren( this );
}

void TestView_dtor( TestView* this ) {

}

void TestView_render( TestView* this ) {

	Log_fmessage( Log_Level_DEBUG, FILE_LINE(), "Rendering view %p...", this );

	FUNCTIONS( TestView, BaseView, this )->position( CLASS( BaseView, this ) );

	// Get system palette
	VDPManager_PaletteIndex systemPalette = VDPManager_getPaletteByTag( vdpManager, PAL_SYSTEM );
	if( systemPalette == VDPManager_Palette_INVALID ) {
		// Need to load the system palette - Ordinarily, this should never be done.
		Log_message( Log_Level_WARNING, FILE_LINE(), "system palette wasn't loaded when initialising TestView" );
		systemPalette = VDPManager_loadPalette( vdpManager, StandardColours, PAL_SYSTEM );
	}

	// Clear VIEW
	FUNCTIONS( TestView, BaseView, this )->placeTileSeries(
		CLASS( BaseView, this ),
		VDPManager_LAYER_BASE,
		0,
		0,
		CLASS( BaseView, this )->width,
		CLASS( BaseView, this )->height,
		systemPalette,
		0x0001,
		FALSE
	);
	FUNCTIONS( TestView, BaseView, this )->placeTileSeries(
		CLASS( BaseView, this ),
		VDPManager_LAYER_TRANSPARENT,
		0,
		0,
		CLASS( BaseView, this )->width,
		CLASS( BaseView, this )->height,
		systemPalette,
		0x0000,
		FALSE
	);

	FUNCTIONS( TestView, BaseView, this )->renderChildren( CLASS( BaseView, this ) );

	// Refresh the JoyManager and prepare it for a new set of clickables
	JoyManager_unregisterAll( joyManager );
	FUNCTIONS( ButtonView, BaseView, this->hi )->setClickable( this->hi, TRUE );
	FUNCTIONS( ButtonView, BaseView, this->bye )->setClickable( this->bye, TRUE );
	FUNCTIONS( ButtonView, BaseView, this->obscenity )->setClickable( this->obscenity, TRUE );
	FUNCTIONS( ButtonView, BaseView, this->clear )->setClickable( this->clear, TRUE );
	FUNCTIONS( ButtonView, BaseView, this->allPurpose )->setClickable( this->allPurpose, TRUE );
	JoyManager_setDefaultCurrentElement( joyManager );
	JoyManager_displayCursor( joyManager, TRUE );

	Log_fmessage( Log_Level_DEBUG, FILE_LINE(), "Done rendering testview located at %p.", this );
}

void TestView_setupChildren( TestView* this ) {
	NEW_OBJECT( ButtonView, this->hi, this->buttonStyle, TestView_BOX_FILL_TILE, 0, 0, 10 );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->hi ) );
	FUNCTIONS( ButtonView, BaseView, this->hi )->setText( this->hi, TestView_SAY_HI, FALSE, FALSE );

	NEW_OBJECT( ButtonView, this->bye, this->buttonStyle, TestView_BOX_FILL_TILE, 0, 4, 10 );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->bye ) );
	FUNCTIONS( ButtonView, BaseView, this->bye )->setText( this->bye, TestView_SAY_BYE, FALSE, FALSE );

	NEW_OBJECT( ButtonView, this->obscenity, this->buttonStyle, TestView_BOX_FILL_TILE, 0, 8, 10 );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->obscenity ) );
	FUNCTIONS( ButtonView, BaseView, this->obscenity )->setText( this->obscenity, TestView_SAY_OBSCENE, FALSE, FALSE );
	CLASS( BaseView, this->obscenity )->tag = 0xF0CC;

	NEW_OBJECT( ButtonView, this->displayedText, this->buttonStyle, TestView_BOX_FILL_TILE, 12, 4, 20 );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->displayedText ) );
	FUNCTIONS( ButtonView, BaseView, this->displayedText )->setText( this->displayedText, TestView_STRING_DEFAULT, FALSE, FALSE );

	NEW_OBJECT( ButtonView, this->clear, this->buttonStyle, TestView_BOX_FILL_TILE, 12, 8, 20 );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->clear ) );
	FUNCTIONS( ButtonView, BaseView, this->clear )->setText( this->clear, TestView_SAY_RESET, FALSE, FALSE );

	NEW_OBJECT( ButtonView, this->allPurpose, this->buttonStyle, TestView_BOX_FILL_TILE, 0, 12, 32 );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->allPurpose ) );
	FUNCTIONS( ButtonView, BaseView, this->allPurpose )->setText( this->allPurpose, TestView_BUTTON_LOG, FALSE, FALSE );

	NEW_OBJECT( SimpleTextView, this->textView, TestView_HEADER, 12, 1, FALSE );
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, this->textView ) );

	// Send this ButtonView in with only the tag 0x7175
	ButtonView* lostReference;
	NEW_OBJECT( ButtonView, lostReference, this->buttonStyle, TestView_BOX_FILL_TILE, 0, 16, 32 );
	CLASS( BaseView, lostReference )->tag = 0x7175;
	FUNCTIONS( TestView, BaseView, this )->addChildView( CLASS( BaseView, this ), CLASS( BaseView, lostReference ) );
	FUNCTIONS( ButtonView, BaseView, lostReference )->setText( lostReference, "This button will be deleted...", FALSE, TRUE );

	// Setup event listeners
	FUNCTIONS( TestView, BaseView, this )->listenToView( CLASS( BaseView, this ), CLASS( BaseView, this->hi ), EVENT_CLICK, TestView_onButtonClick );
	FUNCTIONS( TestView, BaseView, this )->listenToView( CLASS( BaseView, this ), CLASS( BaseView, this->bye ), EVENT_CLICK, TestView_onButtonClick );
	FUNCTIONS( TestView, BaseView, this )->listenToView( CLASS( BaseView, this ), CLASS( BaseView, this->obscenity ), EVENT_CLICK, TestView_onButtonClick );
	FUNCTIONS( TestView, BaseView, this )->listenToView( CLASS( BaseView, this ), CLASS( BaseView, this->clear ), EVENT_CLICK, TestView_onButtonClick );
	FUNCTIONS( TestView, BaseView, this )->listenToView( CLASS( BaseView, this ), CLASS( BaseView, this->allPurpose ), EVENT_CLICK, TestView_onButtonClick );
}

void TestView_onButtonClick( void* instance, void* payload ) {
	TestView* this = instance;
	ButtonView* button = payload;

	Log_fmessage( Log_Level_DEBUG, FILE_LINE(), "testview received button click from %p", button );

	if( button == this->hi ) {
		FUNCTIONS( ButtonView, BaseView, this->displayedText )->setText( this->displayedText, TestView_STRING_HI, TRUE, FALSE );
	} else if( button == this->bye ) {
		FUNCTIONS( ButtonView, BaseView, this->displayedText )->setText( this->displayedText, TestView_STRING_BYE, TRUE, FALSE );
	} else if( button == this->obscenity ) {
		FUNCTIONS( ButtonView, BaseView, this->displayedText )->setText( this->displayedText, TestView_STRING_OBSCENE, TRUE, FALSE );
	} else if( button == this->clear ) {
		FUNCTIONS( ButtonView, BaseView, this->displayedText )->setText( this->displayedText, TestView_STRING_DEFAULT, TRUE, FALSE );
	} else if( button == this->allPurpose ) {
		Log_message( Log_Level_DEBUG, FILE_LINE(), "The all-purpose button was clicked." );
		BaseView* lostReference = FUNCTIONS( TestView, BaseView, this )->getChildByTag( CLASS( BaseView, this ), 0x7175 );
		Log_fmessage( Log_Level_DEBUG, FILE_LINE(), "Got: %p, removing child...", lostReference );
		FUNCTIONS( TestView, BaseView, this )->removeChild( CLASS( BaseView, this ), lostReference, TRUE );
	} else {
		Log_message( Log_Level_WARNING, FILE_LINE(), "Invalid payload for event listener TestView_onButtonClick" );
	}
}
