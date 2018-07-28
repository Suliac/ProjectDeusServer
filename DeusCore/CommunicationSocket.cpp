#include "CommunicationSocket.h"

bool DeusCore::CommunicationSocket::Send(const Packet & packet, int & byteSent)
{
	Buffer512 buffer;
	Packet::Serialize(buffer, packet);

	return Send(buffer, byteSent);
}

bool DeusCore::CommunicationSocket::Send(const Buffer512 & buffer, int & byteSent)
{
	return SendDatas((const char*)buffer.Data(), buffer.GetIndex(), byteSent);
}

bool DeusCore::CommunicationSocket::Recv(std::unique_ptr<Packet>& p_packetReceived, int & byteRecv)
{
	p_packetReceived.reset();
	Buffer512 buffer;

	if (!Recv(buffer, byteRecv))
		return false;

	if (byteRecv > 0)
		p_packetReceived = Packet::Deserialize(buffer);

	return true;
}

bool DeusCore::CommunicationSocket::Recv(Buffer512 & buffer, int & byteRecv)
{
	char tmpBuffer[SIZE_BUFFER];

	if (!RecvDatas(tmpBuffer, SIZE_BUFFER, byteRecv))
		return false; // nothing to read ! 

	buffer.Set((const unsigned char*)tmpBuffer, byteRecv);
	return true;
}
