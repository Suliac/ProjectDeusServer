#pragma once
namespace DeusNetwork
{
	static const int SIZE_BUFFER = 512;

	template<int N>
	struct Buffer
	{
		unsigned char data[N];	// pointer to buffer data
		int size;									// size of the buffer data (in bytes)
		int index;									// index of next byte to be read
	};

	using Buffer512 = Buffer<SIZE_BUFFER>;
}

