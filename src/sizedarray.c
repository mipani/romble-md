#include <sizedarray.h>
#include <types.h>
#include <stddef.h>
#include <string.h>

void SizedArray_takeBytes( SizedArray* this, void* destination, size_t amount ) {
	SizedArray_peekBytes( this, destination, amount );

	// takeBytes is peek + increment
	this->items = ( ( char* ) this->items ) + amount;
	this->length =- amount;
}

void SizedArray_peekBytes( SizedArray* this, void* destination, size_t amount ) {
	memcpy( destination, this->items, amount );
}

void SizedArray_burnBytes( SizedArray* this, size_t amount ) {
	this->items = ( ( char* ) this->items ) + amount;
	this->length =- amount;
}
