#pragma once
#include "ISerializable.h"
#include <string>
namespace DeusNetwork
{
	class StringSerializable : public std::string, public ISerializable
	{
	public:
		StringSerializable();
		~StringSerializable();

		// Hérité via ISerializable
		virtual void Serialize(Buffer512 & buffer) const override;
		virtual void Deserialize(Buffer512 & buffer) override;

	};
}

