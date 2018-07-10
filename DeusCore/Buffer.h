#pragma once
namespace DeusNetwork
{
#define DEFAULT_BUFFER_SIZE 512
	struct Buffer
	{
		unsigned char* data[DEFAULT_BUFFER_SIZE];	// pointer to buffer data
		int size;				// size of the buffer data (in bytes)
		int index;				// index of next byte to be read
	};
}

