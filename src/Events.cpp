#include "Events.h"

namespace Events_Space
{
	class OurEventSink :
		public RE::BSTEventSink<RE::TESSwitchRaceCompleteEvent>,
		public RE::BSTEventSink<RE::TESEquipEvent>,
		public RE::BSTEventSink<RE::TESCombatEvent>,
		public RE::BSTEventSink<RE::TESActorLocationChangeEvent>,
		public RE::BSTEventSink<RE::TESSpellCastEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<SKSE::ModCallbackEvent>,
        public RE::BSTEventSink<RE::TESHitEvent>,
		public RE::BSTEventSink<RE::TESMagicEffectApplyEvent>
	{
		OurEventSink() = default;
		OurEventSink(const OurEventSink&) = delete;
		OurEventSink(OurEventSink&&) = delete;
		OurEventSink& operator=(const OurEventSink&) = delete;
		OurEventSink& operator=(OurEventSink&&) = delete;

	public:
		static OurEventSink* GetSingleton()
		{
			static OurEventSink singleton;
			return &singleton;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSwitchRaceCompleteEvent* event, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>*)
		{
			auto a_actor = event->subject->As<RE::Actor>();

			if (!a_actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent *event, RE::BSTEventSource<RE::TESDeathEvent> *)
		{
			auto a_actor = event->actorDying->As<RE::Actor>();

			if (!a_actor || !a_actor->HasKeywordString("ActorTypeDragon"))
			{
				return RE::BSEventNotifyControl::kContinue;
			}
			a_actor->SetGraphVariableFloat("playbackSpeed", 1.0f);

			if (!DovahAI_Space::DovahAI::IsMQ206CutsceneDragons(a_actor))
			{
				DovahAI_Space::DovahAI::DeathWaitRagdoll(a_actor);
			}

			DovahAI_Space::DovahAI::GetSingleton()->RemoveAttackList(a_actor);

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* event, RE::BSTEventSource<RE::TESEquipEvent>*){
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (a_actor->IsPlayerRef())
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			
			auto item = event->originalRefr;
			if (item && event->equipped)
			{
				auto form = RE::TESForm::LookupByID<RE::TESForm>(item);
				if (form && form->Is(RE::FormType::Spell)){
					auto a_spell = form->As<RE::SpellItem>();
					if (a_spell)
					{
					}
				}
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* event, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
		{
			auto Ename = event->eventName;

			if (Ename != "KID_KeywordDistributionDone")
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			//logger::info("recieved KID finished event"sv);

			//Settings::GetSingleton()->Load();

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* event, RE::BSTEventSource<RE::TESCombatEvent>*){
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor || !a_actor->HasKeywordString("ActorTypeDragon")) {
				return RE::BSEventNotifyControl::kContinue;
			}

			switch (event->newState.get()) {
			case RE::ACTOR_COMBAT_STATE::kCombat:
				a_actor->SetGraphVariableBool("bLDP_IsinCombat", true);
				a_actor->SetGraphVariableBool("bLDP_CrashLand_Faction", true);
				DovahAI_Space::DovahAI::OnInitHPCalc(a_actor);
				DovahAI_Space::DovahAI::DragonType(a_actor);
				DovahAI_Space::DovahAI::CalcLevelRank(a_actor);
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_SetValues_Dragon"))
				{
					DovahAI_Space::DovahAI::SetValuesDragon(a_actor);
				}
				DovahAI_Space::DovahAI::GetSingleton()->CreateAttackList(a_actor);
				break;
			case RE::ACTOR_COMBAT_STATE::kSearching:
				a_actor->SetGraphVariableBool("bLDP_IsinCombat", false);
				break;

			case RE::ACTOR_COMBAT_STATE::kNone:
				a_actor->SetGraphVariableBool("bLDP_IsinCombat", false);
				a_actor->SetGraphVariableInt("iLDP_DownFlyRate", 0);
				DovahAI_Space::DovahAI::GetSingleton()->RemoveAttackList(a_actor);
				break;

			default:
				break;
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESActorLocationChangeEvent* event, RE::BSTEventSource<RE::TESActorLocationChangeEvent>*)
		{
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor || !a_actor->IsPlayerRef()) {
				return RE::BSEventNotifyControl::kContinue;
			}


			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent *event, RE::BSTEventSource<RE::TESHitEvent> *)
		{
			auto a_actor = event->target->As<RE::Actor>();

			if (!a_actor)
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			if (a_actor->HasKeywordString("ActorTypeDragon"))
			{
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat") || DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_Busy_State"))
				{
					return RE::BSEventNotifyControl::kContinue;
				}

				if (auto enemy = event->cause->As<RE::Actor>())
				{
					if (enemy->IsHostileToActor(a_actor))
					{
						a_actor->SetGraphVariableBool("bLDP_Busy_State", true);

						if (GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("StaggerSpikes_CoolKey"), 1.0))
						{
							switch (GFunc_Space::GFunc::GetEquippedItemType(enemy, false))
							{
							case 7:
							case 8:
								//do nothing
								break;

							default:
								if (const auto caster = enemy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant))
								{
									caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_StaggerHitSpell"), true, enemy, 1, false, 1.0, enemy);
								}
								break;
							}
						}

						if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
						{
							auto DiffHp = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Pre_HP") - (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth));
							float hitAngle = GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, enemy, nullptr);

							if (abs(hitAngle) <= 45.0f && DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Front_HP") > 0)
							{
								auto val = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Front_HP");
								a_actor->SetGraphVariableInt("iLDP_Front_HP", val -= DiffHp);
								if (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Front_HP") <= 0)
								{
									// headbroken flag
									if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_BleedOut_State"))
									{
										DovahAI_Space::DovahAI::BleedOut_state(a_actor);
									}
								}
							}
							else if (abs(hitAngle) >= 135.0f && DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Back_HP") > 0)
							{
								auto val = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Back_HP");
								a_actor->SetGraphVariableInt("iLDP_Back_HP", val -= DiffHp);
								if (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Back_HP") <= 0)
								{
									// tailbroken flag
									// additem (dragon bonemeal)
									if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_BleedOut_State"))
									{
										DovahAI_Space::DovahAI::BleedOut_state(a_actor);
									}
								}
							}
							else if (hitAngle < -45.0f && hitAngle > -135.0f && DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Left_HP") > 0)
							{
								auto val = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Left_HP");
								a_actor->SetGraphVariableInt("iLDP_Left_HP", val -= DiffHp);
								if (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Left_HP") <= 0)
								{
									// additem (dragon claw)
									switch (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_DownFlyRate"))
									{
									case 0:
										a_actor->SetGraphVariableInt("iLDP_DownFlyRate", 1);
										break;
									case 1:
										a_actor->SetGraphVariableInt("iLDP_DownFlyRate", 2);
										break;

									default:
										break;
									}

									if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_BleedOut_State"))
									{
										DovahAI_Space::DovahAI::BleedOut_state(a_actor);
									}
								}
							}
							else if (hitAngle > 45.0f && hitAngle < 135.0f && DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Right_HP") > 0)
							{
								auto val = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Right_HP");
								a_actor->SetGraphVariableInt("iLDP_Right_HP", val -= DiffHp);
								if (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Right_HP") <= 0)
								{
									// additem (dragon claw)
									switch (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_DownFlyRate"))
									{
									case 0:
										a_actor->SetGraphVariableInt("iLDP_DownFlyRate", 1);
										break;
									case 1:
										a_actor->SetGraphVariableInt("iLDP_DownFlyRate", 2);
										break;

									default:
										break;
									}

									if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_BleedOut_State"))
									{
										DovahAI_Space::DovahAI::BleedOut_state(a_actor);
									}
								}
							}

							a_actor->SetGraphVariableInt("iLDP_Pre_HP", a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth));

							auto rt = DovahAI_Space::DovahAI::GetFloatVariable(a_actor, "fLDP_Ratio_HP");

							if (DovahAI_Space::DovahAI::GetActorValuePercent(a_actor, RE::ActorValue::kHealth) <= rt && rt > 0.0f)
							{
								DovahAI_Space::DovahAI::Enrage(a_actor, 3);
								a_actor->SetGraphVariableInt("iLDP_Right_HP", rt -= 0.1f);
							}

							if (event->flags && (event->flags.all(RE::TESHitEvent::Flag::kPowerAttack) || event->flags.all(RE::TESHitEvent::Flag::kBashAttack)))
							{
								if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsStaggering"))
								{
									DovahAI_Space::DovahAI::Enrage(a_actor, 1);
								}
							}
						}

						std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
						GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 100ms, "BusyState_Update");
						GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
					}
				}

			}else{
				if (auto enemy = event->cause->As<RE::Actor>())
				{
					if (DovahAI_Space::DovahAI::GetBoolVariable(enemy, "bLDP_IsinCombat") && enemy->IsHostileToActor(a_actor))
					{
						if (auto form = RE::TESForm::LookupByID<RE::TESForm>(event->source))
						{
							if (form)
							{
								switch (form->GetFormType())
								{
								case RE::FormType::Spell:
									if (auto a_spell = form->As<RE::SpellItem>())
									{
										std::string Lsht = (clib_util::editorID::get_editorID(a_spell));
										switch (hash(Lsht.c_str(), Lsht.size()))
										{
										case "BiteAttack_LDP"_h:
											DovahAI_Space::DovahAI::BiteAttack_Impact(enemy, a_actor);
											break;

										case "LeftWingAttack_LDP"_h:
											DovahAI_Space::DovahAI::LeftWingAttack_Impact(enemy, a_actor);
											break;

										case "RightWingAttack_LDP"_h:
											DovahAI_Space::DovahAI::RightWingAttack_Impact(enemy, a_actor);
											break;

										case "LDP_aaaUDTailTurnSpell"_h:
											DovahAI_Space::DovahAI::UDPhysical_Impact(enemy, a_actor, true);
											break;

										case "LDP_aaaUDStampSpell"_h:
											DovahAI_Space::DovahAI::UDPhysical_Impact(enemy, a_actor);
											break;

										default:
											break;
										}
									}
									break;
								
								default:
									break;
								}
							}
						}
						
					}
				}
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESMagicEffectApplyEvent *event, RE::BSTEventSource<RE::TESMagicEffectApplyEvent> *)
		{
			auto a_actor = event->target->As<RE::Actor>();

			if (!a_actor)
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			if (auto enemy = event->caster->As<RE::Actor>()){
				if (DovahAI_Space::DovahAI::GetBoolVariable(enemy, "bLDP_IsinCombat")){
					if (auto form = RE::TESForm::LookupByID<RE::TESForm>(event->magicEffect)){
						switch (form->GetFormType())
						{
						case RE::FormType::MagicEffect:
							if (auto a_effect = form->As<RE::EffectSetting>())
							{
								std::string Lsht = (clib_util::editorID::get_editorID(a_effect));
								switch (hash(Lsht.c_str(), Lsht.size()))
								{
								case "BiteEffect_LDP"_h:
									DovahAI_Space::DovahAI::BiteAttack_Impact(enemy, a_actor);
									break;

								case "LeftWingAttackEffect_LDP"_h:
									DovahAI_Space::DovahAI::LeftWingAttack_Impact(enemy, a_actor);
									break;

								case "RightWingAttackEffect_LDP"_h:
									DovahAI_Space::DovahAI::RightWingAttack_Impact(enemy, a_actor);
									break;

								case "LDP_DragonVoiceDisarmOrUnarmEffect2"_h:
									DovahAI_Space::DovahAI::Unarm_effect(a_actor);
									break;

								default:
									break;
								}
							}
							break;

						default:
							break;
						}
					}
				}
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* event, RE::BSTEventSource<RE::TESSpellCastEvent>*)
		{
			auto a_actor = event->object->As<RE::Actor>();

			if (!a_actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (a_actor->IsPlayerRef()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			auto H = RE::TESDataHandler::GetSingleton();
			const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);

			if (auto form = RE::TESForm::LookupByID(event->spell)){

				switch (form->GetFormType())
				{
				case RE::FormType::Spell:
					if (auto a_spell = form->As<RE::SpellItem>())
					{
						std::string Lsht = (clib_util::editorID::get_editorID(a_spell));
						switch (hash(Lsht.c_str(), Lsht.size()))
						{
						case "LDP_005VijoDarkBallAbsorbSpell"_h:
							caster->CastSpellImmediate(H->LookupForm<RE::SpellItem>(0x9C4, "Leone Dragon Project Shouts 2.esp"), true, a_actor, 1, false, 50.0, a_actor); // Blood Cloak
							break;

						default:
							break;
						}
					}
					break;

				default:
					break;
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}
	};

	RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src)
	{
		FnProcessEvent fn = fnHash.at(*(uint64_t*)this);

		if (!a_event.holder) {
			return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
		}
		auto data = RE::TESDataHandler::GetSingleton();
		RE::Actor* a_actor = const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>();
		switch (hash(a_event.tag.c_str(), a_event.tag.size())) {
		case "footfront"_h:
		case "footleft"_h:
		case "footright"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				GFunc_Space::shakeCamera(0.5f, a_actor->GetPosition(), 0.0f);
			}
			break;

		case "DragonForcefulLandEffect"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				RE::NiPoint3 Tx;
				Tx.x = -1.0f;
				// FXDragonTakeoffImpactSet [IPDS:00019A01]
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0x19A01, "Skyrim.esm"), "NPC Pelvis", Tx, 512.0f, false, false);
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
				{
					DovahAI_Space::DovahAI::LandingCombatAI(a_actor);
				}
			}
		case "DragonLandEffect"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_PreventFlyingTalonSmash"))
				{
					DovahAI_Space::DovahAI::Physical_Impact(a_actor, "LimboSpell", 10.0f);
				}
				if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 1.0f) <= 0.5f)
				{
					GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xF1B1F, "Skyrim.esm"))); // NPCDragonKillMove [SNDR:000F1B1F]
				}
				if (!DovahAI_Space::DovahAI::IsMQ206CutsceneDragons(a_actor))
				{
					DovahAI_Space::DovahAI::Random_TakeOffandDeath_Anims(a_actor);
				}
			}
			break;

		case "DragonTakeoffEffect"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				GFunc_Space::shakeCamera(1.0f, a_actor->GetPosition(), 0.0f);
				if(auto process = a_actor->GetActorRuntimeData().currentProcess){
					process->KnockExplosion(a_actor, a_actor->GetPosition(), 1.0f);
				}
				a_actor->SetGraphVariableInt("iLDP_tailAttack_counter", 0);
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_PreventFlyingTalonSmash") && !DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_RunOnce_TakeOffeffect"))
				{
					DovahAI_Space::DovahAI::Physical_Impact(a_actor, "LimboSpell", 13.0f);
					a_actor->SetGraphVariableBool("bLDP_RunOnce_TakeOffeffect", true);
				}
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
				{
					DovahAI_Space::DovahAI::TakeoffCombatAI(a_actor);
				}
			}
			break;

		case "DragonPassByEffect"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (auto process = a_actor->GetActorRuntimeData().currentProcess)
				{
					process->KnockExplosion(a_actor, a_actor->GetPosition(), 1.0f);
				}
				RE::NiPoint3 Tx;
				Tx.x = -1.0f;
				// FXDragonTakeoffImpactSet [IPDS:00019A01]
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0x19A01, "Skyrim.esm"), "", Tx, 512.0f, false, false);
				DovahAI_Space::DovahAI::Physical_Impact(a_actor, "LimboSpell", 10.0f);

				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
				{
					DovahAI_Space::DovahAI::PassByCombatAI(a_actor);
				}
			}
			break;

		case "weaponSwing"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (a_actor->IsBeingRidden() && RE::PlayerCharacter::GetSingleton()->IsOnMount())
				{
					if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
					{
						if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
						{
							if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
							{
								DovahAI_Space::DovahAI::MoveControllShout(a_actor);
							}else{
								DovahAI_Space::DovahAI::GroundCombatAI(a_actor);
							}
							
						}
					}
				}
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnceOther"))
				{
					if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
					{
						a_actor->SetGraphVariableBool("bLDP_AIControl_doOnceOther", true);
						DovahAI_Space::DovahAI::ControlDistanceAIGround(a_actor);
						a_actor->SetGraphVariableBool("bLDP_AIControl_doOnceOther", false);
					}
				}
			}
			break;

		case "DragonTailAttackEffect"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (auto process = a_actor->GetActorRuntimeData().currentProcess)
				{
					process->KnockExplosion(a_actor, a_actor->GetPosition(), 1.0f);
				}
				RE::NiPoint3 Tx;
				Tx.x = -1.0f;
				// FXDragonTakeoffImpactSet [IPDS:00019A01]
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0x19A01, "Skyrim.esm"), "", Tx, 512.0f, false, false);
				GFunc_Space::shakeCamera(1.0f, a_actor->GetPosition(), 0.0f);
				if (auto var = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_tailAttack_counter"))
				{
					if (var < 2){
						a_actor->SetGraphVariableInt("iLDP_tailAttack_counter", var += 1);
					}else{
						a_actor->SetGraphVariableInt("iLDP_tailAttack_counter", 0);
						// FXDragonTakeoffImpactSet [IPDS:00019A01]
						GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0x19A01, "Skyrim.esm"), "", Tx, 512.0f, false, false);
						// FXDragonLandingImpactSet [IPDS:0002BD39]
						GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0x2BD39, "Skyrim.esm"), "NPC Tail8", Tx, 512.0f, false, false);

						GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x3F1F3, "Skyrim.esm"))); // NPCDragonLandCrashLong2DSD [SNDR:0003F1F3]
						GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xF1B1F, "Skyrim.esm"))); // NPCDragonKillMove [SNDR:000F1B1F]

						if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_TripleThreat_Faction"))
						{
							a_actor->SetGraphVariableBool("bLDP_TripleThreat_Faction", true);
						}
					}
				}
				DovahAI_Space::DovahAI::Physical_Impact(a_actor, "LimboSpell", 13.0f);
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_TripleThreat_Faction"))
				{
					std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
					GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "TATripleThreat_Update");
					GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
				}
			}
			break;

		case "DragonLeftWingAttackEffect"_h:
		case "DragonRightWingAttackEffect"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (auto process = a_actor->GetActorRuntimeData().currentProcess)
				{
					process->KnockExplosion(a_actor, a_actor->GetPosition(), 1.0f);
				}
				if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 1.0f) <= 0.5f)
				{
					GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xF1B1F, "Skyrim.esm"))); // NPCDragonKillMove [SNDR:000F1B1F]
				}
			}
			break;

		case "FlapThrustBegin"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				GFunc_Space::shakeCamera(0.5f, a_actor->GetPosition(), 0.0f);
				if (auto process = a_actor->GetActorRuntimeData().currentProcess)
				{
					process->KnockExplosion(a_actor, a_actor->GetPosition(), 1.0f);
				}
			}
			break;

		case "BeginCastVoice"_h:
		    if(a_actor->HasKeywordString("AlduinUnitedKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Alduin(a_actor);
			}
			if(a_actor->HasKeywordString("MasterPaarthurnaxKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Paarthurnax(a_actor);
			}
			if(a_actor->HasKeywordString("OdahviingKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Odahviing(a_actor);
			}
			if(a_actor->HasKeywordString("DragonVoiceKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Dragon(a_actor);
			}
			break;

		case "Voice_SpellFire_Event"_h:
			if(a_actor->HasKeywordString("AlduinUnitedKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Alduin(a_actor, true);
			}
			if(a_actor->HasKeywordString("MasterPaarthurnaxKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Paarthurnax(a_actor, true);
			}
			if(a_actor->HasKeywordString("OdahviingKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Odahviing(a_actor, true);
			}
			if(a_actor->HasKeywordString("DragonVoiceKey")){
				DovahAI_Space::DovahAI::GetEquippedShout_Dragon(a_actor, true);
			}
			
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				GFunc_Space::shakeCamera(0.5f, a_actor->GetPosition(), 0.0f);

				if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kHovering || a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kPerching)
				{
					if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
					{
						if (a_actor->IsBeingRidden() && RE::PlayerCharacter::GetSingleton()->IsOnMount())
						{
							DovahAI_Space::DovahAI::ControlDistanceRiddenAI(a_actor);
						}
						
					}else{
						DovahAI_Space::DovahAI::ControlDistanceAIFly(a_actor);
					}
				}
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnceOther"))
				{
					if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
					{
						a_actor->SetGraphVariableBool("bLDP_AIControl_doOnceOther", true);
						DovahAI_Space::DovahAI::ControlDistanceAIGround(a_actor);
						a_actor->SetGraphVariableBool("bLDP_AIControl_doOnceOther", false);
					}
				}
			}
			break;

		case "endGroundAttack"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
				{
					auto yt = DovahAI_Space::DovahAI::GetSingleton()->Get_AttackList(a_actor);
					std::get<2>(yt)[1] = 3;
					DovahAI_Space::DovahAI::AddBehavior(a_actor);
				}
			}
			break;

		case "EnrageEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat") && DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
			{
				DovahAI_Space::DovahAI::Enrage_start(a_actor);
			}
			break;

		case "SoundPlay.NPCDragonKillMove"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_AIControl_doOnce"))
				{
					DovahAI_Space::DovahAI::TripleShout(a_actor);
				}
			}
			break;

		case "StampHitEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (auto process = a_actor->GetActorRuntimeData().currentProcess)
				{
					process->KnockExplosion(a_actor, a_actor->GetPosition(), 1.0f);
				}
				RE::NiPoint3 Tx;
				Tx.x = -1.0f;
				// FXDragonTakeoffImpactSet [IPDS:00019A01]
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0x19A01, "Skyrim.esm"), "NPC Pelvis", Tx, 512.0f, false, false);
				GFunc_Space::shakeCamera(0.5f, a_actor->GetPosition(), 0.0f);
				if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 1.0f) <= 0.5f)
				{
					GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xF1B1F, "Skyrim.esm"))); // NPCDragonKillMove [SNDR:000F1B1F]
				}
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					auto Node = DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsRight")? "NPC RLegToe1" : "NPC LLegFoot";
					DovahAI_Space::DovahAI::CastAreaEffect(a_actor, RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDStampSpell"), Node);
				}
			}
			break;

		case "TailTurnHitEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					auto Node = DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsRight") ? "NPC RLegToe1" : "NPC LLegFoot";
					DovahAI_Space::DovahAI::CastAreaEffect(a_actor, RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDTailTurnSpell"), Node);
				}
			}
			break;

		case "TailTurnEndEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					DovahAI_Space::DovahAI::OneMoreTailTurn(a_actor);
				}
			}
			break;

		case "ShoutFireBallEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
					{
						auto ct = targethandle.get();
						const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
						switch (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Shout_to_Cast"))
						{
						case 1:
							caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_VoiceDragonFrostBall01SPELLSHOUT"), true, ct, 1, false, 0.0, a_actor);
							break;

						case 2:
							caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_ShockBallSpellShoutversion"), true, ct, 1, false, 0.0, a_actor);
							break;

						case 3:
							caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("L_VoiceDragonFireBall01"), true, ct, 1, false, 0.0, a_actor);
							break;

						default:
							break;
						}
					}
					
				}
			}
			break;

		case "MarkedForDeathEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					//DovahAI_Space::DovahAI::OneMoreTailTurn(a_actor);
				}
			}
			break;

		case "SwingShoutEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					DovahAI_Space::DovahAI::StartShout(a_actor);
				}
			}
			break;

		case "SwingShoutEndEvent"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					// sound.StopInstance(instanceID);
				}
			}
			break;

		case "InterruptCast"_h:
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "IsUDAnimation"))
				{
					a_actor->InterruptCast(false);
				}
			}
			break;

		case "preHitframe"_h:
			if (const auto combatGroup = a_actor->GetCombatGroup())
			{
				for (auto &targetData : combatGroup->targets)
				{
					if (auto target = targetData.targetHandle.get())
					{
						RE::Actor *Enemy = target.get();
						if (DovahAI_Space::DovahAI::GetBoolVariable(Enemy, "bLDP_IsinCombat") && DovahAI_Space::DovahAI::GetBoolVariable(Enemy, "bLDP_InParry_State"))
						{
							if (Enemy->GetPosition().GetDistance(a_actor->GetPosition()) <= 530.0f && abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(Enemy, a_actor, nullptr)) < 90.0f)
							{
								DovahAI_Space::DovahAI::StartParry(Enemy);
							}
							break;
						}
					}
				}
			}
			break;

		case "BowRelease"_h:
			if (GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("Backfire_CoolKey"), 1.0))
			{
				const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
				caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_StaggerHitSpell"), true, a_actor, 1, false, 0.0, a_actor);
			}

		default:
			break;
		}

		return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
	}

	std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;

	void Events::install(){

		auto eventSink = OurEventSink::GetSingleton();

		// ScriptSource
		auto* eventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		// eventSourceHolder->AddEventSink<RE::TESSwitchRaceCompleteEvent>(eventSink);
		//eventSourceHolder->AddEventSink<RE::TESEquipEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESCombatEvent>(eventSink);
		//eventSourceHolder->AddEventSink<RE::TESActorLocationChangeEvent>(eventSink);
		// eventSourceHolder->AddEventSink<RE::TESSpellCastEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESDeathEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESHitEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESMagicEffectApplyEvent>(eventSink);
	}

	void Events::install_pluginListener(){
		auto eventSink = OurEventSink::GetSingleton();
		SKSE::GetModCallbackEventSource()->AddEventSink(eventSink);
	}

	

	bool Events::BindPapyrusFunctions(VM* vm)
	{
		//vm->RegisterFunction("XXXX", "XXXXX", XXXX);
		return true;
	}


	void Events::Update(RE::Actor* a_actor, [[maybe_unused]] float a_delta)
	{
		if (a_actor->GetActorRuntimeData().currentProcess && a_actor->GetActorRuntimeData().currentProcess->InHighProcess() && a_actor->Is3DLoaded()){
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				Singleton<GSub>().Process_Updates(a_actor, std::chrono::steady_clock::now());

				DovahAI_Space::DovahAI::Others(a_actor);

				if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_RunOnce_TakeOffeffect") && a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kCruising)
				{
					a_actor->SetGraphVariableBool("bLDP_RunOnce_TakeOffeffect", false);
				}

				if (!DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsEnraging") && DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_Enrage_Count") >= 10 && a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
				{
					DovahAI_Space::DovahAI::Enrage_state(a_actor);
				}
			}
		}
	}

	void GSub::Process_Updates(RE::Actor *a_actor, GFunc_Space::Time::time_point time_now)
	{
		uniqueLocker lock(mtx_Timer);
		for (auto it = _Timer.begin(); it != _Timer.end(); ++it)
		{
			if (it->first == a_actor)
			{
				if (!it->second.empty())
				{
					for (auto data : it->second)
					{
						auto update = std::get<0>(data);
						if (update)
						{
							auto time_initial = std::get<1>(data);
							auto time_required = std::get<2>(data);
							if (duration_cast<std::chrono::milliseconds>(time_now - time_initial).count() >= time_required.count())
							{
								std::get<0>(data) = false;
								auto function = std::get<3>(data);
								auto H = RE::TESDataHandler::GetSingleton();
								switch (hash(function.c_str(), function.size()))
								{
								case "TATripleThreat_Update"_h:
									a_actor->SetGraphVariableBool("bLDP_TripleThreat_Faction", false);
									break;

								case "DeathTimeTravel_Update"_h:
									GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0xF1B1F, "Skyrim.esm"))); // NPCDragonKillMove [SNDR:000F1B1F]

								case "DeathDefault_Update"_h:
								case "DeathAgony_Update"_h:
								case "DeathInjured_Update"_h:
								case "DeathBleedout_Update"_h:
									a_actor->NotifyAnimationGraph("Ragdoll");
									break;

								case "TalonSmash_Update"_h:
									DovahAI_Space::DovahAI::TalonSmash1(a_actor);
									break;

								case "TalonSmash1_Update"_h:
									DovahAI_Space::DovahAI::TalonSmash2(a_actor);
									break;

								case "TalonSmash2_Update"_h:
									a_actor->SetGraphVariableBool("Injured", true);
									break;

								case "EnrageState_Update"_h:
									a_actor->SetGraphVariableBool("bNoStagger", false);
									a_actor->SetGraphVariableBool("bLDP_IsEnraging", false);		
									break;

								case "BusyState_Update"_h:
									a_actor->SetGraphVariableBool("bLDP_Busy_State", false);
									break;

								case "BleedOutState_Update"_h:
									DovahAI_Space::DovahAI::BleedOut_state1(a_actor);
									break;

								case "BleedOutState1_Update"_h:
									DovahAI_Space::DovahAI::Enrage(a_actor, 1);
									a_actor->SetGraphVariableBool("bLDP_BleedOut_State", false);
									break;

								case "Shout_Update"_h:
									DovahAI_Space::DovahAI::Shout1(a_actor);
									break;

								case "EnrageStart_Update"_h:
									a_actor->SetGraphVariableBool("IsEnraging", false);
									DovahAI_Space::DovahAI::GetSingleton()->CreateAttackList(a_actor);
									break;

								case "CDR_AI_Update"_h:
									DovahAI_Space::DovahAI::ControlDistanceRiddenAI1(a_actor);
									break;

								case "CDR_AI2_Update"_h:
								case "HAS_AI2_Update"_h:
								case "GAS_AI_Update"_h:
								case "TSS_AI_Update"_h:
								case "PBC_AI_Update"_h:
								case "ControlDistanceFly_AI2_Update"_h:
								case "GAVS_AI_Update"_h:
									a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", false);
									break;

								case "TakeOffCombat_AI_Update"_h:
									a_actor->SetGraphVariableBool("bVoiceReady", false);
									a_actor->SetGraphVariableBool("bVoiceReady", true);
									DovahAI_Space::DovahAI::ToHoverAttackScene(a_actor);
									break;

								case "HAS_AI_Update"_h:
									DovahAI_Space::DovahAI::ToHoverAttackScene1(a_actor);
									break;

								case "GAS_Flight_Update"_h:
									a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
									break;

								case "GC_front1_AI_Update"_h:
								case "GC_back1_AI_Update"_h:
									a_actor->NotifyAnimationGraph("attackStartTail");
									break;

								case "GC_front_AI_Update"_h:
									DovahAI_Space::DovahAI::GroundCombatAI_front(a_actor);
									break;

								case "GC_back_AI_Update"_h:
									DovahAI_Space::DovahAI::GroundCombatAI_back(a_actor);
									break;

								case "GC_left_AI_Update"_h:
									DovahAI_Space::DovahAI::GroundCombatAI_left(a_actor);
									break;

								case "GC_right_AI_Update"_h:
									DovahAI_Space::DovahAI::GroundCombatAI_right(a_actor);
									break;

								case "MovShout_AI_Update"_h:
									DovahAI_Space::DovahAI::MoveControllShout1(a_actor);
									break;

								case "MovShout2_AI_Update"_h:
									DovahAI_Space::DovahAI::MoveControllShout2(a_actor);
									break;

								case "MagicSelector_AI_Update"_h:
									DovahAI_Space::DovahAI::MagicSelector1(a_actor);
									break;

								case "startparry_AI_Update"_h:
								    GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x3C7C5, "Skyrim.esm"))); // WPNBashBladeSD [SNDR:0003C7C5]
									a_actor->SetGraphVariableBool("bLDP_InParry_State", false);
									break;

								case "ControlDistanceFly_AI_Update"_h:
									DovahAI_Space::DovahAI::ControlDistanceAIFly1(a_actor);
									break;

								case "LandingCombat_AI_Update"_h:
									DovahAI_Space::DovahAI::LandingCombatAI1(a_actor);
									break;

								default:
									break;
								}
								std::vector<std::tuple<bool, GFunc_Space::Time::time_point, GFunc_Space::ms, std::string>>::iterator position = std::find(it->second.begin(), it->second.end(), data);
								if (position != it->second.end())
								{
									it->second.erase(position);
								}
							}
						}
					}
				}
				else
				{
					_Timer.erase(it);
				}
				break;
			}
		}
	}

	void Events::init()
	{
		_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
		if (_precision_API) {
			_precision_API->AddPostHitCallback(SKSE::GetPluginHandle(), PrecisionWeaponsCallback_Post);
			logger::info("Enabled compatibility with Precision");
		}
	}

	void Events::PrecisionWeaponsCallback_Post(const PRECISION_API::PrecisionHitData& a_precisionHitData, const RE::HitData& a_hitdata)
	{
		if (!a_precisionHitData.target || !a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
			return;
		}
		return;
	}




	void Settings::Load(){
		constexpr auto path = "Data\\SKSE\\Plugins\\NPCSpellVariance.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		general.Load(ini);

		include_spells_mods.Load(ini);
		include_spells_keywords.Load(ini);
		exclude_spells_mods.Load(ini);
		exclude_spells_keywords.Load(ini);

		ini.SaveFile(path);
	}

	void Settings::General_Settings::Load(CSimpleIniA &a_ini)
	{
		static const char *section = "General_Settings";

		auto DS = GetSingleton();

		DS->general.bWhiteListApproach = a_ini.GetBoolValue(section, "bWhiteListApproach", DS->general.bWhiteListApproach);

		a_ini.SetBoolValue(section, "bWhiteListApproach", DS->general.bWhiteListApproach, ";If set to true, only the include mods and keywords are considered. Else only the exclude approach is used");
		//
	}

	void Settings::Include_AllSpells_withKeywords::Load(CSimpleIniA &a_ini)
	{
		static const char *section = "Include_AllSpells_withKeywords";

		auto DS = GetSingleton();

		DS->include_spells_keywords.inc_keywords_joined = a_ini.GetValue(section, "inc_keywords", DS->include_spells_keywords.inc_keywords_joined.c_str());

		std::istringstream f(DS->include_spells_keywords.inc_keywords_joined);
		std::string s;
		while (getline(f, s, '|'))
		{
			DS->include_spells_keywords.inc_keywords.push_back(s);
		}

		a_ini.SetValue(section, "inc_keywords", DS->include_spells_keywords.inc_keywords_joined.c_str(), ";Enter keywords for which all associated spells are included. Seperate keywords with | ");
	}

	void Settings::Include_AllSpells_inMods::Load(CSimpleIniA& a_ini){
		static const char* section = "Include_AllSpells_inMods";

		auto DS = GetSingleton();

		DS->include_spells_mods.inc_mods_joined = a_ini.GetValue(section, "inc_mods", DS->include_spells_mods.inc_mods_joined.c_str());

		std::istringstream f(DS->include_spells_mods.inc_mods_joined);
		std::string  s;
		while (getline(f, s, '|')) {
			DS->include_spells_mods.inc_mods.push_back(s);
		}

		a_ini.SetValue(section, "inc_mods", DS->include_spells_mods.inc_mods_joined.c_str(), ";Enter Mod Names of which all spells within are included. Seperate names with | ");
		//
	}

	void Settings::Exclude_AllSpells_withKeywords::Load(CSimpleIniA& a_ini)
	{
		static const char* section = "Exclude_AllSpells_withKeywords";

		auto DS = GetSingleton();

		DS->exclude_spells_keywords.exc_keywords_joined = a_ini.GetValue(section, "exc_keywords", DS->exclude_spells_keywords.exc_keywords_joined.c_str());

		std::istringstream f(DS->exclude_spells_keywords.exc_keywords_joined);
		std::string  s;
		while (getline(f, s, '|')) {
			DS->exclude_spells_keywords.exc_keywords.push_back(s);
		}

		a_ini.SetValue(section, "exc_keywords", DS->exclude_spells_keywords.exc_keywords_joined.c_str(), ";Enter keywords for which all associated spells are excluded. Seperate keywords with | ");
	}

	void Settings::Exclude_AllSpells_inMods::Load(CSimpleIniA &a_ini)
	{
		static const char *section = "Exclude_AllSpells_inMods";

		auto DS = GetSingleton();

		DS->exclude_spells_mods.exc_mods_joined = a_ini.GetValue(section, "exc_mods", DS->exclude_spells_mods.exc_mods_joined.c_str());

		std::istringstream f(DS->exclude_spells_mods.exc_mods_joined);
		std::string s;
		while (getline(f, s, '|'))
		{
			DS->exclude_spells_mods.exc_mods.push_back(s);
		}

		a_ini.SetValue(section, "exc_mods", DS->exclude_spells_mods.exc_mods_joined.c_str(), ";Enter Mod Names of which all spells within are excluded. Seperate names with | ");
		//
	}

	
}