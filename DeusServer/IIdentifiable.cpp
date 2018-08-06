#include "IIdentifiable.h"

namespace DeusServer
{
	uint32_t IIdentifiable::m_nextId = 1;

	IIdentifiable::IIdentifiable()
	{
		m_uniqueIdentifier = GetNewId();
	}

	uint32_t IIdentifiable::GetNewId()
	{
		uint32_t newId = m_nextId;
		m_nextId++;
		return newId;
	}
}


