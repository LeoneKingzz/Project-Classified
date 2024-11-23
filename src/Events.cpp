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
		public RE::BSTEventSink<SKSE::ModCallbackEvent>
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
				break;
			case RE::ACTOR_COMBAT_STATE::kSearching:
				a_actor->SetGraphVariableBool("bLDP_IsinCombat", false);
				break;

			case RE::ACTOR_COMBAT_STATE::kNone:
				a_actor->SetGraphVariableBool("bLDP_IsinCombat", false);
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

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* event, RE::BSTEventSource<RE::TESSpellCastEvent>*)
		{
			auto a_actor = event->object->As<RE::Actor>();

			if (!a_actor) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (a_actor->IsPlayerRef()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			auto eSpell = RE::TESForm::LookupByID(event->spell);

			if (eSpell && eSpell->Is(RE::FormType::Spell)) {
				auto rSpell = eSpell->As<RE::SpellItem>();
				switch (rSpell->GetSpellType()) {
				case RE::MagicSystem::SpellType::kSpell:
				    
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
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "NPC Pelvis", Tx, 512.0f, false, false);
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
					GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp")));
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
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "", Tx, 512.0f, false, false);
				DovahAI_Space::DovahAI::Physical_Impact(a_actor, "LimboSpell", 10.0f);
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
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "", Tx, 512.0f, false, false);
				GFunc_Space::shakeCamera(1.0f, a_actor->GetPosition(), 0.0f);
				if (auto var = DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_tailAttack_counter"))
				{
					if (var < 2){
						a_actor->SetGraphVariableInt("iLDP_tailAttack_counter", var += 1);
					}else{
						a_actor->SetGraphVariableInt("iLDP_tailAttack_counter", 0);
						GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "", Tx, 512.0f, false, false);
						GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "NPC Tail8", Tx, 512.0f, false, false);

						GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // ks_NPCDragonTripleThreat
						GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // ks_NPCDragonKillMove

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
					GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // ks_NPCDragonKillMove
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
		    if(a_actor->HasKeywordString("ActorTypeDragon")){
				DovahAI_Space::DovahAI::GetEquippedShout(a_actor);
			}
			break;

		case "Voice_SpellFire_Event"_h:
			if (a_actor->HasKeywordString("ActorTypeDragon"))
			{
				DovahAI_Space::DovahAI::GetEquippedShout(a_actor);
			}
			if (DovahAI_Space::DovahAI::GetBoolVariable(a_actor, "bLDP_IsinCombat"))
			{
				GFunc_Space::shakeCamera(0.5f, a_actor->GetPosition(), 0.0f);
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
				GFunc_Space::GFunc::PlayImpactEffect(a_actor, data->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "NPC Pelvis", Tx, 512.0f, false, false);
				GFunc_Space::shakeCamera(0.5f, a_actor->GetPosition(), 0.0f);
				if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 1.0f) <= 0.5f)
				{
					GFunc_Space::GFunc::playSound(a_actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // ks_NPCDragonKillMove
				}
			}
			break;

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
								auto function = std::get<3>(data);
								auto H = RE::TESDataHandler::GetSingleton();
								switch (hash(function.c_str(), function.size()))
								{
								case "TATripleThreat_Update"_h:
									a_actor->SetGraphVariableBool("bLDP_TripleThreat_Faction", false);
									break;

								case "DeathTimeTravel_Update"_h:
									GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // ks_NPCDragonKillMove

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