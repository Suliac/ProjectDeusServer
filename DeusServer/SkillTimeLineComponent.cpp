#include "SkillTimeLineComponent.h"
#include "HealthTimeLineComponent.h"

#include "DeusCore/FastDelegate.h"
#include "DeusCore/PacketUpdateHealth.h"
#include "DeusCore/EventManagerHandler.h"

namespace DeusServer
{
	SkillTimeLineComponent::SkillTimeLineComponent(DeusCore::Id gameId, Id parentObjectId)
		: TimeLineComponent(EComponentType::SkillComponent, parentObjectId),
		m_gameId(gameId)
	{
	}

	SkillTimeLineComponent::~SkillTimeLineComponent()
	{
	}

	void SkillTimeLineComponent::OnStart()
	{
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &SkillTimeLineComponent::InterpretPacket);

		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectChangeCell);
	}

	void SkillTimeLineComponent::OnUpdate(double deltatimeMs)
	{
		uint32_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + DELAY_MS;
		std::shared_ptr<const DeusCore::SkillInfos> p_currentSkill = GetValue(currentTime);
		if (p_currentSkill)
		{
			// Update state
			DeusCore::ESkillState state = DeusCore::ESkillState::NotLaunched;

			if (p_currentSkill->GetLaunchTime() > currentTime) // spell is at least casting
			{
				state = DeusCore::ESkillState::Casting;
				if (p_currentSkill->GetLaunchTime() + p_currentSkill->GetCastTime() > currentTime) // spell is at least launched
				{
					state = DeusCore::ESkillState::Launched;

					if (p_currentSkill->GetTotalTime() > currentTime && m_effectIndex >= p_currentSkill->GetEffects().size())
						state = DeusCore::ESkillState::Finished;
				}
			}

			// Apply effects/damages
			if (m_lastState != state || state == DeusCore::ESkillState::Launched)
			{
				switch (state)
				{
				case DeusCore::NotLaunched:
					// just wait
					break;
				case DeusCore::Casting:
					// just wait
					break;
				case DeusCore::Launched:

					if (p_currentSkill->GetEffects().size() <= m_effectIndex)
					{
						// check if we can apply damage, otherwise juste wait
						if ((p_currentSkill->GetLaunchTime() + p_currentSkill->GetCastTime() + p_currentSkill->GetEffects()[m_effectIndex]->GetDuration() + m_effectDurationDone) * 1000 < currentTime) // can apply effect
						{
							// Apply damage
							for (int i = 0; i < m_objectsInPerimeters.size(); ++i)
							{
								std::shared_ptr<PositionTimeLineComponent> p_positionComponent = std::dynamic_pointer_cast<PositionTimeLineComponent>(m_objectsInPerimeters[i]->GetFirstComponent(GameObjectComponent::EComponentType::PositionComponent));
								if (p_positionComponent)
								{
									// ******************** 1 - Check if there is someone in the range of the skill
									std::shared_ptr<const DeusCore::DeusVector2> p_objPos = p_positionComponent->GetValue(currentTime);
									std::shared_ptr<const DeusCore::DeusVector2> p_myPod = m_currentPosition->GetValue(currentTime);

									if (!p_objPos || !p_myPod)
										break;

									// check range
									if (DeusCore::DeusVector2::SqrtMagnitude(*p_objPos, *p_myPod) <= p_currentSkill->GetMaxScope() * p_currentSkill->GetMaxScope())
									{

										// Check if target has life
										std::shared_ptr<HealthTimeLineComponent> p_healthComponent = std::dynamic_pointer_cast<HealthTimeLineComponent>(m_objectsInPerimeters[i]->GetFirstComponent(GameObjectComponent::EComponentType::HealthComponent));
										if (p_healthComponent)
										{
											// ******************** 2 - Deduce life
											std::shared_ptr<const int> p_life = p_healthComponent->GetValue(currentTime);
											int newHealth = *p_life - p_currentSkill->GetEffects()[m_effectIndex]->GetDamage();
											p_healthComponent->InsertData(std::make_shared<int>(newHealth), currentTime);

											// ******************** 3 - Send packets to client concerned
											//DeusCore::PacketSPtr p_packetHealth = std::shared_ptr<DeusCore::PacketUpdateHealth>(new DeusCore::PacketUpdateHealth(m_objectsInPerimeters[i], p_healthComponent->GetId(), newHealth, currentTime));
											//DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, 0, p_packetHealth);										}
										}
									}


								}

								m_effectDurationDone += p_currentSkill->GetEffects()[m_effectIndex]->GetDuration();
								m_effectIndex++;
							}
						}
						// else all effects are done

						break;
				case DeusCore::Finished:
					// Delete or réinit
					break;
				default:
					break;
					}
				}
			}
		}
	}

	void SkillTimeLineComponent::OnStop()
	{
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &SkillTimeLineComponent::InterpretPacket);

		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectChangeCell);
	}

	std::shared_ptr<DeusCore::SkillInfos> SkillTimeLineComponent::Interpolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, const DeusCore::SkillInfos & afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const
	{
		return nullptr;
	}

	std::shared_ptr<DeusCore::SkillInfos> SkillTimeLineComponent::Extrapolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const
	{
		return nullptr;
	}

	void SkillTimeLineComponent::InterpretPacket(DeusCore::DeusEventSPtr p_packet)
	{
		if (p_packet->second->GetType() == DeusCore::Packet::EMessageType::ObjectChangeCell)
		{
			ManageChangeCellPacket(std::dynamic_pointer_cast<PacketObjectChangeCell>(p_packet->second));
		}
	}

	void SkillTimeLineComponent::ManageChangeCellPacket(std::shared_ptr<PacketObjectChangeCell> p_packetChangeCell)
	{
		// Is this me ? then i need to update my info
		if (p_packetChangeCell->GetGameObject()->GetId() == m_parentObjectId)
		{
			m_currentCellId = p_packetChangeCell->GetEnteredCellId();

		}
		else
		{
			if (p_packetChangeCell->GetEnteredCellId() == m_currentCellId) // new object entered our perimeter
				m_objectsInPerimeters.insert(std::make_pair(p_packetChangeCell->GetGameObject()->GetId(), p_packetChangeCell->GetGameObject()));

			if (p_packetChangeCell->GetLeftCellId() == m_currentCellId) // object just left perimeter
			{
				std::map<Id, std::shared_ptr<const GameObject>>::iterator  objIt = m_objectsInPerimeters.find(p_packetChangeCell->GetGameObject()->GetId());
				if (objIt != m_objectsInPerimeters.end())
					m_objectsInPerimeters.erase(objIt);
			}
		}
	}
}
