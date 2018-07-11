#pragma once
namespace DeusNetwork
{
	static const int SIZE_BUFFER = 512;

	template<int N>
	struct Buffer
	{
		const int size = N;		// size of the buffer data (in bytes)
		unsigned char data[N];	// pointer to buffer data
		int index = 0;			// index of next byte to be read
	};

	using Buffer512 = Buffer<SIZE_BUFFER>;
}

