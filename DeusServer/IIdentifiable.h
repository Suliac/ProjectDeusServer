#pragma once
#include <memory>
namespace DeusServer
{
	class IIdentifiable
	{
	public:
		IIdentifiable();

		uint32_t GetId() const { return m_uniqueIdentifier; }
	protected:
		uint32_t m_uniqueIdentifier;

	private:
		static uint32_t m_nextId;
		uint32_t GetNewId();
	};
}

