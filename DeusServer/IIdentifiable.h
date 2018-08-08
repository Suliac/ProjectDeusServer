#pragma once
#include <memory>
using Id = uint32_t;
namespace DeusServer
{
	class IIdentifiable
	{
	public:
		Id GetId() const { return m_uniqueIdentifier; }
	protected:
		Id m_uniqueIdentifier;

	};
}

