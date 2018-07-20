#pragma once
#include "ISerializable.h"
#include <string>
namespace DeusCore
{
	class StringSerializable : public std::string, public ISerializable
	{
	public:
		StringSerializable();
		~StringSerializable();

		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual void OnDeserialize(Buffer512 & buffer) override;
	};
}

