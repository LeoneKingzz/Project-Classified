#include "GeneralFunctions.h"

namespace GFunc_Space{
	
	void GFunc::Set_iFrames(RE::Actor *actor)
	{
		actor->SetGraphVariableBool("bIframeActive", true);
		actor->SetGraphVariableBool("bInIframe", true);
	}

	void GFunc::Reset_iFrames(RE::Actor *actor)
	{
		actor->SetGraphVariableBool("bIframeActive", false);
		actor->SetGraphVariableBool("bInIframe", false);
	}

	void GFunc::dispelEffect(RE::MagicItem *spellForm, RE::Actor *a_target)
	{
		const auto targetActor = a_target->AsMagicTarget();
		if (targetActor->HasMagicEffect(spellForm->effects[0]->baseEffect))
		{
			auto activeEffects = targetActor->GetActiveEffectList();
			for (const auto &effect : *activeEffects)
			{
				if (effect->spell == spellForm)
				{
					effect->Dispel(true);
				}
			}
		}
	}

	float GFunc::get_angle_he_me(RE::Actor *me, RE::Actor *he, RE::BGSAttackData *attackdata)
	{
		auto he_me = PolarAngle(me->GetPosition() - he->GetPosition());
		auto head = PolarAngle(he->GetHeading(false) * 180.0f / PI);
		if (attackdata)
			head = head.add(attackdata->data.attackAngle);
		auto angle = he_me.sub(head).to_normangle();
		return angle;
	}

	bool Has_Magiceffect_Keyword(const RE::Actor *a_actor, const RE::BGSKeyword *a_key, float a_comparison_value)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kHasMagicEffectKeyword;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
		cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = a_comparison_value; });

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

	bool IsAllowedToFly(const RE::Actor *a_actor, float a_comparison_value)
	{
		static RE::TESConditionItem cond;
		static std::once_flag flag;
		std::call_once(flag, [&]()
					   {
        cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kIsAllowedToFly;
        cond.data.flags.opCode          = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        cond.data.object                = RE::CONDITIONITEMOBJECT::kSelf;
        cond.data.comparisonValue.f     = a_comparison_value; });

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(a_actor->As<RE::TESObjectREFR>()), nullptr);
		return cond(params);
	}

	bool GFunc::isHumanoid(RE::Actor *a_actor)
	{
		auto bodyPartData = a_actor->GetRace() ? a_actor->GetRace()->bodyPartData : nullptr;
		return bodyPartData && bodyPartData->GetFormID() == 0x1d;
	}

	void GFunc::UnequipAll(RE::Actor *a_actor)
	{
		uniqueLocker lock(mtx_Inventory);
		auto itt = _Inventory.find(a_actor);
		if (itt == _Inventory.end())
		{
			std::vector<RE::TESBoundObject *> Hen;
			_Inventory.insert({a_actor, Hen});
		}

		for (auto it = _Inventory.begin(); it != _Inventory.end(); ++it)
		{
			if (it->first == a_actor)
			{
				auto inv = a_actor->GetInventory();
				for (auto &[item, data] : inv)
				{
					const auto &[count, entry] = data;
					if (count > 0 && entry->IsWorn())
					{
						RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, item);
						it->second.push_back(item);
					}
				}
				break;
			}
			continue;
		}
	}

	void GFunc::RegisterforUpdate(RE::Actor *a_actor, std::tuple<bool, GFunc_Space::Time::time_point, GFunc_Space::ms, std::string> data)
	{
		uniqueLocker lock(mtx_Timer);
		auto itt = _Timer.find(a_actor);
		if (itt == _Timer.end())
		{
			std::vector<std::tuple<bool, GFunc_Space::Time::time_point, GFunc_Space::ms, std::string>> Hen;
			Hen.push_back(data);
			_Timer.insert({a_actor, Hen});
		}else{
			itt->second.push_back(data);
		}
	}

	void GFunc::set_tupledata(std::tuple<bool, GFunc_Space::Time::time_point, GFunc_Space::ms, std::string> data, bool a, GFunc_Space::Time::time_point b, GFunc_Space::ms c, std::string d)
	{
		std::get<0>(data) = a;
		std::get<1>(data) = b;
		std::get<2>(data) = c;
		std::get<3>(data) = d;
	}

	void GFunc::Re_EquipAll(RE::Actor *a_actor)
	{
		uniqueLocker lock(mtx_Inventory);
		for (auto it = _Inventory.begin(); it != _Inventory.end(); ++it)
		{
			if (it->first == a_actor)
			{
				for (auto item : it->second)
				{
					RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, item);
				}
				_Inventory.erase(it);
				break;
			}
			continue;
		}
	}

	bool GFunc::isPowerAttacking(RE::Actor *a_actor)
	{
		auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
		if (currentProcess)
		{
			auto highProcess = currentProcess->high;
			if (highProcess)
			{
				auto attackData = highProcess->attackData;
				if (attackData)
				{
					auto flags = attackData->data.flags;
					return flags.any(RE::AttackData::AttackFlag::kPowerAttack);
				}
			}
		}
		return false;
	}

	void GFunc::UpdateCombatTarget(RE::Actor *a_actor)
	{
		auto CTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
		if (!CTarget)
		{
			auto combatGroup = a_actor->GetCombatGroup();
			if (combatGroup)
			{
				for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it)
				{
					if (it->targetHandle && it->targetHandle.get().get())
					{
						a_actor->GetActorRuntimeData().currentCombatTarget = it->targetHandle.get().get();
						break;
					}
					continue;
				}
			}
		}
		// a_actor->UpdateCombat();
	}

	bool GFunc::IsCasting(RE::Actor *a_actor)
	{
		bool result = false;

		auto IsCastingRight = false;
		auto IsCastingLeft = false;
		auto IsCastingDual = false;

		if ((a_actor->GetGraphVariableBool("IsCastingRight", IsCastingRight) && IsCastingRight) || (a_actor->GetGraphVariableBool("IsCastingLeft", IsCastingLeft) && IsCastingLeft) || (a_actor->GetGraphVariableBool("IsCastingDual", IsCastingDual) && IsCastingDual))
		{
			result = true;
		}

		return result;
	}

	void GFunc::InterruptAttack(RE::Actor *a_actor)
	{
		a_actor->NotifyAnimationGraph("attackStop");
		a_actor->NotifyAnimationGraph("recoilStop");
		a_actor->NotifyAnimationGraph("bashStop");
		a_actor->NotifyAnimationGraph("blockStop");
		a_actor->NotifyAnimationGraph("staggerStop");
	}

	std::vector<RE::TESForm *> GFunc::GetEquippedForm(RE::Actor *actor, bool right, bool left)
	{
		std::vector<RE::TESForm *> Hen;

		auto limboform = actor->GetActorRuntimeData().currentProcess;

		if(right){
			if (limboform && limboform->GetEquippedRightHand())
			{
				Hen.push_back(limboform->GetEquippedRightHand());
			}
		}else if(left){
			if (limboform && limboform->GetEquippedLeftHand())
			{
				Hen.push_back(limboform->GetEquippedLeftHand());
			}
		}else{
			if (limboform && limboform->GetEquippedLeftHand())
			{
				Hen.push_back(limboform->GetEquippedLeftHand());
			}
			if (limboform && limboform->GetEquippedRightHand())
			{
				Hen.push_back(limboform->GetEquippedRightHand());
			}
		}
		return Hen;
	}

	int GFunc::GetEquippedItemType(RE::Actor *actor, bool lefthand)
	{
		using TYPE = RE::CombatInventoryItem::TYPE;
		int result = -1;
		auto form_list = lefthand ? GetEquippedForm(actor, false, true) : GetEquippedForm(actor, true);

		if (!form_list.empty())
		{
			for (auto form : form_list)
			{
				if (form)
				{
					switch (*form->formType)
					{
					case RE::FormType::Weapon:
						if (const auto equippedWeapon = form->As<RE::TESObjectWEAP>())
						{
							switch (equippedWeapon->GetWeaponType())
							{
							case RE::WEAPON_TYPE::kHandToHandMelee:
								result = 0;
								break;
							case RE::WEAPON_TYPE::kOneHandSword:
								result = 1;
								break;
							case RE::WEAPON_TYPE::kOneHandDagger:
								result = 2;
								break;
							case RE::WEAPON_TYPE::kOneHandAxe:
								result = 3;
								break;
							case RE::WEAPON_TYPE::kOneHandMace:
								result = 4;
								break;
							case RE::WEAPON_TYPE::kTwoHandSword:
								result = 5;
								break;
							case RE::WEAPON_TYPE::kTwoHandAxe:
								result = 6;
								break;
							case RE::WEAPON_TYPE::kBow:
								result = 7;
								break;
							case RE::WEAPON_TYPE::kStaff:
								result = 8;
								break;
							case RE::WEAPON_TYPE::kCrossbow:
								result = 12;
								break;
							default:
								break;
							}
						}
						break;

					default:
						break;
					}
					if (result != -1)
					{
						break;
					}
				}
				continue;
			}
		}

		if (result == -1)
		{
			auto combatCtrl = actor->GetActorRuntimeData().combatController;
			auto CombatInv = combatCtrl ? combatCtrl->inventory : nullptr;
			if (CombatInv)
			{
				for (const auto item : CombatInv->equippedItems)
				{
					if (item.item)
					{
						switch (item.item->GetType())
						{
						case TYPE::kTorch:
							result = 11;
							break;

						case TYPE::kShield:
							result = 10;
							break;

						case TYPE::kScroll:
						case TYPE::kMagic:
							result = 9;
							break;

						default:
							break;
						}
					}
					if (result != -1)
					{
						break;
					}
				}
			}
		}

		return result;
	}

	bool GFunc::IsWeaponOut(RE::Actor *actor)
	{
		bool result = false;
		auto form_list = GetEquippedForm(actor);

		if (!form_list.empty())
		{
			for (auto form : form_list)
			{
				if (form)
				{
					switch (*form->formType)
					{
					case RE::FormType::Weapon:
						if (const auto equippedWeapon = form->As<RE::TESObjectWEAP>())
						{
							switch (equippedWeapon->GetWeaponType())
							{
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
						if (auto equippedShield = form->As<RE::TESObjectARMO>())
						{
							result = true;
						}
						break;
					default:
						break;
					}
					if (result)
					{
						break;
					}
				}
				continue;
			}
		}
		return result;
	}

	bool GFunc::IsMeleeOnly(RE::Actor *a_actor)
	{
		using TYPE = RE::CombatInventoryItem::TYPE;

		auto result = false;

		auto combatCtrl = a_actor->GetActorRuntimeData().combatController;
		auto CombatInv = combatCtrl ? combatCtrl->inventory : nullptr;
		if (CombatInv)
		{
			for (const auto item : CombatInv->equippedItems)
			{
				if (item.item)
				{
					switch (item.item->GetType())
					{
					case TYPE::kMelee:
						result = true;
						break;
					default:
						break;
					}
				}
				if (result)
				{
					break;
				}
			}
		}

		return result;
	}

	void GFunc::EquipfromInvent(RE::Actor *a_actor, RE::FormID a_formID, bool unequip)
	{
		auto inv = a_actor->GetInventory();
		for (auto &[item, data] : inv)
		{
			const auto &[count, entry] = data;
			if (count > 0 && entry->object->formID == a_formID)
			{
				if (unequip){
					RE::ActorEquipManager::GetSingleton()->UnequipObject(a_actor, entry->object);
				}else{
					RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, entry->object);
				}
				break;
			}
			continue;
		}
	}

	float GFunc::AV_Mod(RE::Actor *a_actor, int actor_value, float input, float mod)
	{
		if (actor_value > 0)
		{
			int k;
			for (k = 0; k <= actor_value; k += 1)
			{
				input += mod;
			}
		}

		return input;
	}

	RE::BGSAttackData *GFunc::get_attackData(RE::Actor *a)
	{
		if (!a->GetActorRuntimeData().currentProcess || !a->GetActorRuntimeData().currentProcess->high)
			return nullptr;
		return a->GetActorRuntimeData().currentProcess->high->attackData.get();
	}

	float GFunc::get_personal_survivalRatio(RE::Actor *protagonist, RE::Actor *combat_target)
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
	float GFunc::get_personal_threatRatio(RE::Actor *protagonist, RE::Actor *combat_target)
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
					   { cond.data.functionData.function = RE::FUNCTION_DATA::FunctionID::kStartCombat; });

		RE::ConditionCheckParams params(const_cast<RE::TESObjectREFR *>(subject->As<RE::TESObjectREFR>()),
										const_cast<RE::TESObjectREFR *>(target->As<RE::TESObjectREFR>()));
		// return cond(params);
		params.unk20;
	}

	int GFunc::GenerateRandomInt(int value_a, int value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_int_distribution<int> dist(value_a, value_b);
		return dist(generator);
	}

	float GFunc::GenerateRandomFloat(float value_a, float value_b)
	{
		std::mt19937 generator(rd());
		std::uniform_real_distribution<float> dist(value_a, value_b);
		return dist(generator);
	}

	void GFunc::PushActorAway(RE::Actor *causer, RE::Actor *target, float magnitude)
	{
		if (!target->HasKeywordString("ActorTypeDragon"))
		{
			auto targetPoint = causer->GetNodeByName(causer->GetActorRuntimeData().race->bodyPartData->parts[0]->targetName.c_str());
			RE::NiPoint3 vec = targetPoint->world.translate;
			// RE::NiPoint3 vec = causer->GetPosition();
			pushActorAway(causer->GetActorRuntimeData().currentProcess, target, vec, magnitude);
		}
	}

	
}
