#pragma once
#include <unordered_map>

#include "DeusCore/EventManager.h"
#include "DeusCore/Packets.h"

#include "GameObject.h"
namespace DeusServer
{
	using PlayerId = uint32_t;
	using GameObjectId = uint32_t;

	using PlayerInfos = std::unordered_map<PlayerId, GameObjectId>; // id of the player connection with id of the 
	using ServerGameObjects = std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>;
	
	
	class GameLogicServer : public IExecutable
	{
	public:
		GameLogicServer(int gameId);
		~GameLogicServer();

	protected:
		/////////////////////////// 
		//         EXECUTE       // 
		/////////////////////////// 
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStart() override;
		virtual void OnStop() override;
		
		/////////////////////////// 
		//     OBJECTS MNGMT     // 
		/////////////////////////// 
		bool AddObject(std::shared_ptr<GameObject> p_objectToAdd);
		bool RemoveObject(uint32_t objectToDeleteId);

		/////////////////////////// 
		//       DELEGUATES      // 
		/////////////////////////// 
		void InterpretPacket(DeusCore::DeusEventSPtr p_packet);
		void StartNewGame(const std::vector<uint32_t>& playerIds);
	private:
		ServerGameObjects m_gameObjects;
		PlayerInfos m_playerWithGameObject;

		std::mutex m_gameObjLocker;
		int m_gameId;
		std::string m_name;
	};
}

