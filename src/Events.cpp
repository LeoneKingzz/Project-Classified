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

			if (!a_actor)
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			// if (a_actor->IsPlayerRef()){
			// 	Events::GetSingleton()->scan_activeRunes(nullptr, nullptr, false, false, true);
			// }else{
			// 	Events::GetSingleton()->scan_activeRunes(a_actor, nullptr, false, true);
			// }

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

			logger::info("recieved KID finished event"sv);

			//Settings::GetSingleton()->Load();
			//Events::Patch_Spell_List();

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* event, RE::BSTEventSource<RE::TESCombatEvent>*){
			auto a_actor = event->actor->As<RE::Actor>();

			if (!a_actor || a_actor->IsPlayerRef()) {
				return RE::BSEventNotifyControl::kContinue;
			}

			switch (event->newState.get()) {
			case RE::ACTOR_COMBAT_STATE::kCombat:
				a_actor->SetGraphVariableBool("bNSV_IsinCombat", true);
				break;
			case RE::ACTOR_COMBAT_STATE::kSearching:
				a_actor->SetGraphVariableBool("bNSV_IsinCombat", false);
				break;

			case RE::ACTOR_COMBAT_STATE::kNone:
				a_actor->SetGraphVariableBool("bNSV_IsinCombat", false);
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

		RE::Actor* actor = const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>();
		switch (hash(a_event.tag.c_str(), a_event.tag.size())) {
		case "BeginCastLeft"_h:
		    if (!actor->IsPlayerRef()){
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{

				}
			}
			break;

		case "BeginCastRight"_h:
			if (!actor->IsPlayerRef())
			{
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{

				}
			}
			break;

		case "MLh_SpellFire_Event"_h:
			if (!actor->IsPlayerRef())
			{
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{

				}
			}
			break;

		case "MRh_SpellFire_Event"_h:
			if (!actor->IsPlayerRef())
			{
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{

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
			// auto bNSV_IsinCombat = false;
			// if ((a_actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
			// {
			// 	Events::Scan_ForHealers(a_actor);
			// }else{
			// 	Events::Unregister_Healer(a_actor);
			// }
		}
	}

	// void Events::init()
	// {
	// 	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	// 	if (_precision_API) {
	// 		_precision_API->AddPostHitCallback(SKSE::GetPluginHandle(), PrecisionWeaponsCallback_Post);
	// 		logger::info("Enabled compatibility with Precision");
	// 	}
	// }

	// void Events::PrecisionWeaponsCallback_Post(const PRECISION_API::PrecisionHitData& a_precisionHitData, const RE::HitData& a_hitdata)
	// {
	// 	if (!a_precisionHitData.target || !a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
	// 		return;
	// 	}
	// 	return;
	// }




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