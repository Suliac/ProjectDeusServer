#include "SkillTimeLineComponent.h"

#include "DeusCore/FastDelegate.h"

namespace DeusServer
{
	SkillTimeLineComponent::SkillTimeLineComponent(DeusCore::Id gameId)
		:	TimeLineComponent(EComponentType::SkillComponent),
			m_gameId(gameId)
	{		
	}

	SkillTimeLineComponent::~SkillTimeLineComponent()
	{
	}

	void SkillTimeLineComponent::OnStart()
	{
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &SkillTimeLineComponent::InterpretPacket);
		
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectEnter);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectLeave);
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
							// 1 - Check if there is someone in the area

							// 2 - Deduce life

							// 3 - Send packets to client concerned

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

	void SkillTimeLineComponent::OnStop()
	{
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &SkillTimeLineComponent::InterpretPacket);

		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectEnter);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectLeave);
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
		// TODO : Update player GO list
	}
}
