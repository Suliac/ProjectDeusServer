#pragma once
#include <unordered_map>

#include "DeusCore/EventManager.h"
#include "DeusCore/Packets.h"
#include "DeusCore/DeusVector2.h"

#include "GameObject.h"

using Id = uint32_t;
namespace DeusServer
{
	const uint32_t NUMBER_CELLS = 1;
	const uint32_t DEFAULT_CELL_ID = 1;

	using PlayerId = Id;
	using GameObjectId = Id;


	using PlayerInfos = std::unordered_map<PlayerId, GameObjectId>; // id of the player connection with id of the 
	using ServerGameObjects = std::unordered_map<GameObjectId, std::shared_ptr<GameObject>>;
	using ServerCells = std::map<Id, ServerGameObjects>;


	class GameLogicServer : public IExecutable
	{
	public:
		GameLogicServer(Id gameId);
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
		bool AddObject(std::shared_ptr<GameObject> p_objectToAdd, Id cellId);
		bool RemoveObject(uint32_t objectToDeleteId);

		/////////////////////////// 
		//       DELEGUATES      // 
		/////////////////////////// 
		void InterpretPacket(DeusCore::DeusEventSPtr p_packet);
		void StartNewGame(const std::vector<uint32_t>& playerIds);
		void PlayerDisconnected(Id clientId);
		void UpdatePlayerDirection(Id clientId, Id componentId, DeusCore::DeusVector2 newDir);
		void PlayerUseSkill(Id clientId, std::shared_ptr<DeusCore::PacketUseSkillRequest> p_packet);
	private:
		const float SPEED_SEC = 5;
		const float SPEED_MS = SPEED_SEC / 1000.f;

		Id GetCellIdOfGameObject(Id objectId);
		void GetGameObjectOnChangeCells(Id playerId, Id cellLeavedId, Id cellEnteredId, std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered);
		
		std::shared_ptr<GameObjectComponent> GetObjectComponent(Id clientId, Id componentId, Id& cellId);
		std::shared_ptr<GameObject> GetObject(Id clientId, Id& cellId);

		ServerCells m_cellsGameObjects;
		PlayerInfos m_playerWithGameObject;

		std::string m_name;
		Id m_gameId;

		std::recursive_mutex m_cellLock;
		//std::mutex m_gameObjLocker[NUMBER_CELLS];
		std::mutex m_playersLocker;
	};
}

