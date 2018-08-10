#pragma once
#include "NetworkServer.h"
#include "PacketObjectChangeCell.h"
#include "PacketObjectEnter.h"
#include "PacketObjectLeave.h"
#include "GameLogicServer.h"

#include "DeusCore/EventManagerHandler.h"

namespace DeusServer
{
	struct DeusPlayerInfos {
	public:
		enum EPlayerState {
			NotReady = 0,
			Ready = 1,
			InGame = 2,
		};

		Id GameObjectId;
		EPlayerState State;
		std::vector<Id> ObjectsIdsFollowed;
		std::string Nickname;

		DeusPlayerInfos()
		{
			GameObjectId = 0;
			State = EPlayerState::NotReady;
		}

		DeusPlayerInfos(std::string nickname)
		{
			Nickname = nickname;
			GameObjectId = 0;
			State = EPlayerState::NotReady;
		}
	};

	using CellId = Id;
	using CellsWithListenersIds = std::map<CellId, std::vector<Id>>;
	using PlayersInfos = std::map<Id, DeusPlayerInfos>;

	class GameNetworkServer : public NetworkServer
	{
	public:
		GameNetworkServer(int gameId);
		~GameNetworkServer();

	protected:
		/////////////////////////// 
		//         SERVER        // 
		///////////////////////////
		virtual void OnStart() override;
		virtual void OnStop() override;
		virtual void OnDisconnectClient(Id clientId) override;

		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
		void InterpretPacket(DeusCore::DeusEventSPtr p_packet);
		void LeaveGame(Id clientId);
		void TryDeleteGame();
		void PlayerReady(Id clientId);
		void PlayerNotReady(Id clientId);
		bool NewPlayer(Id clientId, DeusClientSPtr clientConnection);
		void ObjectChangedCell(std::shared_ptr<PacketObjectChangeCell> p_packetReceived);
	private:
		bool CanStartGame();


		/////////////////////////// 
		//  INTEREST MANAGEMENT  // 
		///////////////////////////

		bool IsPlayerFollowingObject(Id clientId, Id objectId);
		bool IsObjectEnteringInnerArea(Id clientId, CellId source, CellId destination);
		bool IsObjectLeavingOutterArea(Id clientId, CellId source, CellId destination);

		void UpdateCellSubscription(Id clientId, CellId cellLeavedId, CellId cellEnteredId);
		void UpdateGameObjectIdForPlayer(Id clientId, Id objectId);
		void ManageLeftCell(CellId cellLeftId, CellId cellEnteredId, Id objectId, const std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft);
		void ManageEnteredCell(CellId cellLeftId, CellId cellEnteredId, std::shared_ptr<const GameObject> object, const std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered, bool notifyAllPlayer);

		void ObjectEnter(Id clientId, std::shared_ptr<const GameObject> gameObject);
		void ObjectLeft(Id objectId, Id clientId);

		//////////////////////////////////////////////////////

		unsigned int m_gameId;
		unsigned int m_stopped;

		CellsWithListenersIds m_cells;
		std::recursive_mutex m_cellLocker[NUMBER_CELLS];

		PlayersInfos m_playersInfos;
		std::mutex m_playersLocker;
	};
	using GameNetworkServerUPtr = std::unique_ptr<GameNetworkServer>;
	using GameNetworkServerSPtr = std::shared_ptr<GameNetworkServer>;
}

