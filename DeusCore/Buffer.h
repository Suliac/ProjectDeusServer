#pragma once
#include <assert.h>
#include <vector>
namespace DeusNetwork
{
	static const int SIZE_BUFFER = 512;

	template<int N>
	class Buffer
	{
	public:
		// Insert datas 'datasToInsert' of size 'dataSize' into buffer from current buffer's index
		void Insert(const unsigned char* datasToInsert, size_t dataSize);

		// Get datas from buffer of size 'dataSize' into param 'datas' from current buffer's index
		void Select(unsigned char* datas, size_t dataSize);

		// Get datas from buffer of size 'dataSize'
		std::vector<unsigned char> Select(size_t dataSize);

		// Set our buffer's data from an offseted index (0 by default)
		void Set(const unsigned char* datas, size_t dataSize, size_t offset = 0);

		// Get buffer's datas
		const unsigned char* Data() const { return m_data; };

		// Get current buffer's index
		const size_t GetIndex() const { return m_index; };
		
		// Set current buffer's index
		void SetIndex(size_t value) { m_index = value; };

		const size_t size = N;		// size of the buffer data (in bytes)
	protected:
		unsigned char m_data[N];	// pointer to buffer data
		size_t m_index = 0;			// index of next byte to be write or read
	};

	using Buffer512 = Buffer<SIZE_BUFFER>;

	template<int N>
	inline void Buffer<N>::Insert(const unsigned char* datasToInsert, size_t dataSize)
	{
		assert(m_index + dataSize <= size);
		memcpy(m_data + m_index, datasToInsert, dataSize);
		m_index += dataSize;
	}

	template<int N>
	inline void Buffer<N>::Set(const unsigned char* datas, size_t dataSize, size_t offset)
	{
		m_index = offset;
		memcpy(m_data + m_index, datas, dataSize);
		m_index += dataSize;
	}

	template<int N>
	inline void Buffer<N>::Select(unsigned char* datasSelected, size_t dataSize)
	{
		assert(m_index + dataSize <= size);
		memcpy(datasSelected, m_data + m_index, dataSize);
		m_index += dataSize;

	}

	template<int N>
	inline std::vector<unsigned char> Buffer<N>::Select(size_t dataSize)
	{
		assert(m_index + dataSize <= size);

		std::vector<unsigned char> tmpBuff;
		tmpBuff.resize(dataSize);

		memcpy(tmpBuff.data(), m_data + m_index, dataSize);
		m_index += dataSize;

		return tmpBuff;
	}

}

