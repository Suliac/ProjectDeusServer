#pragma once
#include "Buffer.h"

namespace DeusNetwork
{
	class Message
	{
	public:
		virtual void Write(Buffer &buffer) = 0;
		virtual void Read(Buffer &buffer) = 0;

	protected:
		
		template<typename T> 
		void SerializeData(Buffer& buffer, const T& value);


		template<typename T>
		T DeserializeData(Buffer& buffer);
	};

}

