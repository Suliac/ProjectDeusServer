#pragma once
#include "Buffer.h"

namespace DeusNetwork
{
	class Message
	{
	public:
		virtual void Write(Buffer512 &buffer) const = 0;
		virtual void Read(const Buffer512 &buffer) = 0;

	protected:

		template<typename T>
		void SerializeData(Buffer512& buffer, const T& value) const;


		template<typename T>
		T DeserializeData(const Buffer512& buffer) const;
	};

}

