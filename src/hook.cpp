#include "hook.h"

namespace hooks
{
	void OnMeleeHitHook::Set_iFrames(RE::Actor* actor)
	{
		actor->SetGraphVariableBool("bIframeActive", true);
		actor->SetGraphVariableBool("bInIframe", true);
	}

	void OnMeleeHitHook::Reset_iFrames(RE::Actor* actor)
	{
		actor->SetGraphVariableBool("bIframeActive", false);
		actor->SetGraphVariableBool("bInIframe", false);
	}

	void OnMeleeHitHook::dispelEffect(RE::MagicItem* spellForm, RE::Actor* a_target)
	{
		const auto targetActor = a_target->AsMagicTarget();
		if (targetActor->HasMagicEffect(spellForm->effects[0]->baseEffect)) {
			auto activeEffects = targetActor->GetActiveEffectList();
			for (const auto& effect : *activeEffects) {
				if (effect->spell == spellForm) {
					effect->Dispel(true);
				}
			}
		}
	}

	bool Has_Magiceffect_Keyword(const RE::Actor *a_actor, const RE::BGSKeyword *a_key)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kHasMagicEffectKeyword;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
		cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = 0.0f; });

		ConditionParam cond_param;
		cond_param.form = const_cast<RE::BGSKeyword *>(a_key->As<RE::BGSKeyword>());
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool HasBoundWeaponEquipped(const RE::Actor *a_actor, RE::MagicSystem::CastingSource type)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kHasBoundWeaponEquipped;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = 0.0f; });

		ConditionParam cond_param;
		cond_param.i = static_cast<int32_t>(type);
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool OnMeleeHitHook::isHumanoid(RE::Actor* a_actor)
	{
		auto bodyPartData = a_actor->GetRace() ? a_actor->GetRace()->bodyPartData : nullptr;
		return bodyPartData && bodyPartData->GetFormID() == 0x1d;
	}


	void OnMeleeHitHook::UnequipAll(RE::Actor* a_actor)
	{
		uniqueLocker lock(mtx_Inventory);
		auto         itt = _Inventory.find(a_actor);
		if (itt == _Inventory.end()) {
			std::vector<RE::TESBoundObject*> Hen;
			_Inventory.insert({ a_actor, Hen });
		}

		for (auto it = _Inventory.begin(); it != _Inventory.end(); ++it) {
			if (it->first == a_actor) {
				auto inv = a_actor->GetInventory();
				for (auto& [item, data] : inv) {
					const auto& [count, entry] = data;
					if (count > 0 && entry->IsWorn()) {
						RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, item);
						it->second.push_back(item);
					}
				}
				break;
			}
			continue;
		}
	}

	void OnMeleeHitHook::Re_EquipAll(RE::Actor* a_actor)
	{
		uniqueLocker lock(mtx_Inventory);
		for (auto it = _Inventory.begin(); it != _Inventory.end(); ++it) {
			if (it->first == a_actor) {
				for (auto item : it->second) {
					RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, item);
				}
				_Inventory.erase(it);
				break;
			}
			continue;
		}
	}

	bool OnMeleeHitHook::isPowerAttacking(RE::Actor* a_actor)
	{
		auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
		if (currentProcess) {
			auto highProcess = currentProcess->high;
			if (highProcess) {
				auto attackData = highProcess->attackData;
				if (attackData) {
					auto flags = attackData->data.flags;
					return flags.any(RE::AttackData::AttackFlag::kPowerAttack);
				}
			}
		}
		return false;
	}

	void OnMeleeHitHook::UpdateCombatTarget(RE::Actor* a_actor){
		auto CTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
		if (!CTarget) {
			auto combatGroup = a_actor->GetCombatGroup();
			if (combatGroup) {
				for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
					if (it->targetHandle && it->targetHandle.get().get()) {
						a_actor->GetActorRuntimeData().currentCombatTarget = it->targetHandle.get().get();
						break;
					}
					continue;
				}
			}
		}
		//a_actor->UpdateCombat();
	}


	bool OnMeleeHitHook::IsCasting(RE::Actor* a_actor)
	{
		bool result = false;

		auto IsCastingRight = false;
		auto IsCastingLeft = false;
		auto IsCastingDual = false;

		if ((a_actor->GetGraphVariableBool("IsCastingRight", IsCastingRight) && IsCastingRight) 
		|| (a_actor->GetGraphVariableBool("IsCastingLeft", IsCastingLeft) && IsCastingLeft) 
		|| (a_actor->GetGraphVariableBool("IsCastingDual", IsCastingDual) && IsCastingDual)) {
			result = true;
		}
		
		return result;
	}

	void OnMeleeHitHook::InterruptAttack(RE::Actor* a_actor){
		a_actor->NotifyAnimationGraph("attackStop");
		a_actor->NotifyAnimationGraph("recoilStop");
		a_actor->NotifyAnimationGraph("bashStop");
		a_actor->NotifyAnimationGraph("blockStop");
		a_actor->NotifyAnimationGraph("staggerStop");
	}

	std::vector<RE::TESForm*> OnMeleeHitHook::GetEquippedForm(RE::Actor* actor)
	{
		std::vector<RE::TESForm*> Hen;

		auto limboform = actor->GetActorRuntimeData().currentProcess;

		if (limboform && limboform->GetEquippedLeftHand()) {
			Hen.push_back(limboform->GetEquippedLeftHand());
		}
		if (limboform && limboform->GetEquippedRightHand()) {
			Hen.push_back(limboform->GetEquippedRightHand());
		}

		return Hen;
	}

	bool OnMeleeHitHook::IsWeaponOut(RE::Actor *actor)
	{
		bool result = false;
		auto form_list = GetEquippedForm(actor);

		if (!form_list.empty()) {
			for (auto form : form_list) {
				if (form) {
					switch (*form->formType) {
					case RE::FormType::Weapon:
						if (const auto equippedWeapon = form->As<RE::TESObjectWEAP>()) {
							switch (equippedWeapon->GetWeaponType()) {
							case RE::WEAPON_TYPE::kOneHandSword:
							case RE::WEAPON_TYPE::kOneHandDagger:
							case RE::WEAPON_TYPE::kOneHandAxe:
							case RE::WEAPON_TYPE::kOneHandMace:
							case RE::WEAPON_TYPE::kTwoHandSword:
							case RE::WEAPON_TYPE::kTwoHandAxe:
							case RE::WEAPON_TYPE::kBow:
							case RE::WEAPON_TYPE::kCrossbow:
								result = true;
								break;
							default:
								break;
							}
						}
						break;
					case RE::FormType::Armor:
						if (auto equippedShield = form->As<RE::TESObjectARMO>()) {
							result = true;
						}
						break;
					default:
						break;
					}
					if (result) {
						break;
					}
				}
				continue;
			}
		}
		return result;
	}

	bool OnMeleeHitHook::IsMeleeOnly(RE::Actor* a_actor)
	{
		using TYPE = RE::CombatInventoryItem::TYPE;

		auto result = false;

		auto combatCtrl = a_actor->GetActorRuntimeData().combatController;
		auto CombatInv = combatCtrl ? combatCtrl->inventory : nullptr;
		if (CombatInv) {
			for (const auto item : CombatInv->equippedItems) {
				if (item.item) {
					switch (item.item->GetType()) {
					case TYPE::kMelee:
						result = true;
						break;
					default:
						break;
					}
				}
				if (result){
					break;
				}
			}
		}

		return result;
	}

	void OnMeleeHitHook::EquipfromInvent(RE::Actor* a_actor, RE::FormID a_formID)
	{
		auto inv = a_actor->GetInventory();
		for (auto& [item, data] : inv) {
			const auto& [count, entry] = data;
			if (count > 0 && entry->object->formID == a_formID) {
				RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, entry->object);
				break;
			}
			continue;
		}
	}

	float OnMeleeHitHook::AV_Mod(RE::Actor* a_actor, int actor_value, float input, float mod)
	{
		if (actor_value > 0){
			int k;
			for (k = 0; k <= actor_value; k += 1) {
				input += mod;
			}
		}

		return input;
	}

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
			// 	OnMeleeHitHook::GetSingleton()->scan_activeRunes(nullptr, nullptr, false, false, true);
			// }else{
			// 	OnMeleeHitHook::GetSingleton()->scan_activeRunes(a_actor, nullptr, false, true);
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
			//OnMeleeHitHook::Patch_Spell_List();

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

	

	float OnMeleeHitHook::get_angle_he_me(RE::Actor *me, RE::Actor *he, RE::BGSAttackData *attackdata){
		auto he_me = PolarAngle(me->GetPosition() - he->GetPosition());
		auto head = PolarAngle(he->GetHeading(false) * 180.0f / PI);
		if (attackdata)
			head = head.add(attackdata->data.attackAngle);
		auto angle = he_me.sub(head).to_normangle();
		return angle;
	}

	RE::BGSAttackData* OnMeleeHitHook::get_attackData(RE::Actor *a){
		if (!a->GetActorRuntimeData().currentProcess || !a->GetActorRuntimeData().currentProcess->high)
			return nullptr;
		return a->GetActorRuntimeData().currentProcess->high->attackData.get();
	}

	

	float OnMeleeHitHook::get_personal_survivalRatio(RE::Actor *protagonist, RE::Actor *combat_target)
	{
		float result = 0.0f;
		float MyTeam_total_threat = 0.0f;
		float EnemyTeam_total_threat = 0.0f;
		float personal_threat = 0.0f;

		if (const auto combatGroup = protagonist->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (auto value = memberData.threatValue; value)
					{
						MyTeam_total_threat += value;
						if (ally.get() == protagonist)
						{
							personal_threat += value;
						}
					}
				}
				continue;
			}
		}

		if (const auto combatGroup = combat_target->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (auto value = memberData.threatValue; value)
					{
						EnemyTeam_total_threat += value;
					}
				}
				continue;
			}
		}

		if (MyTeam_total_threat > 0 && EnemyTeam_total_threat > 0 && personal_threat > 0)
		{

			auto personal_survival = personal_threat / EnemyTeam_total_threat;
			auto Enemy_groupSurvival = EnemyTeam_total_threat / MyTeam_total_threat;

			result = personal_survival / Enemy_groupSurvival;
		}

		return result;
	}
	float OnMeleeHitHook::get_personal_threatRatio(RE::Actor *protagonist, RE::Actor *combat_target)
	{
		float result = 0.0f;
		float personal_threat = 0.0f;
		float CTarget_threat = 0.0f;

		if (const auto combatGroup = protagonist->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (ally.get() == protagonist)
					{
						if (auto value = memberData.threatValue; value)
						{
							personal_threat += value;
							break;
						}
					}
				}
				continue;
			}
		}

		if (const auto combatGroup = combat_target->GetCombatGroup())
		{
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					if (ally.get() == combat_target)
					{
						if (auto value = memberData.threatValue; value)
						{
							CTarget_threat += value;
							break;
						}
					}
				}
				continue;
			}
		}

		if (personal_threat > 0 && CTarget_threat > 0)
		{
			result = personal_threat / CTarget_threat;
		}

		return result;
	}
	

	

	bool GetshouldHelp(const RE::Actor *p_ally, const RE::Actor *a_actor)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetShouldHelp;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        cond.data.comparisonValue.f     = 1.0f; });

		ConditionParam cond_param;
		cond_param.form = const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>());
		cond.data.functionData.params[0] = std::bit_cast<void *>(cond_param);

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(p_ally->As<RE::TESObjectREFR>()),
										const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()));
		return cond(params);
	}

	void StartCombat(const RE::Actor *subject, const RE::Actor *target)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kStartCombat;});

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(subject->As<RE::TESObjectREFR>()),
										const_cast<RE::TESObjectREFR *>(target->As<RE::TESObjectREFR>()));
		//return cond(params);
		params.unk20;
	}

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

	void OnMeleeHitHook::install(){

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

	void OnMeleeHitHook::install_pluginListener(){
		auto eventSink = OurEventSink::GetSingleton();
		SKSE::GetModCallbackEventSource()->AddEventSink(eventSink);
	}

	

	bool OnMeleeHitHook::BindPapyrusFunctions(VM* vm)
	{
		//vm->RegisterFunction("XXXX", "XXXXX", XXXX);
		return true;
	}

	int OnMeleeHitHook::GenerateRandomInt(int value_a, int value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_int_distribution<int> dist(value_a, value_b);
		return dist(generator);
	}

	float OnMeleeHitHook::GenerateRandomFloat(float value_a, float value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_real_distribution<float> dist(value_a, value_b);
		return dist(generator);
	}


	void OnMeleeHitHook::Update(RE::Actor* a_actor, [[maybe_unused]] float a_delta)
	{
		if (a_actor->GetActorRuntimeData().currentProcess && a_actor->GetActorRuntimeData().currentProcess->InHighProcess() && a_actor->Is3DLoaded()){
			// auto bNSV_IsinCombat = false;
			// if ((a_actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
			// {
			// 	OnMeleeHitHook::Scan_ForHealers(a_actor);
			// }else{
			// 	OnMeleeHitHook::Unregister_Healer(a_actor);
			// }
		}
	}

	// void OnMeleeHitHook::init()
	// {
	// 	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	// 	if (_precision_API) {
	// 		_precision_API->AddPostHitCallback(SKSE::GetPluginHandle(), PrecisionWeaponsCallback_Post);
	// 		logger::info("Enabled compatibility with Precision");
	// 	}
	// }

	// void OnMeleeHitHook::PrecisionWeaponsCallback_Post(const PRECISION_API::PrecisionHitData& a_precisionHitData, const RE::HitData& a_hitdata)
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