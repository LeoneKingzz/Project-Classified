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

	void OnMeleeHitHook::Patch_Spell_List()
	{
		auto DS = Settings::GetSingleton();

		auto spellList = DS->general.bWhiteListApproach ? get_valid_spellList<RE::SpellItem>(LookupMods(Settings::GetSingleton()->include_spells_mods.inc_mods), LookupKeywords(Settings::GetSingleton()->include_spells_keywords.inc_keywords), true) : get_valid_spellList<RE::SpellItem>(LookupMods(Settings::GetSingleton()->exclude_spells_mods.exc_mods), LookupKeywords(Settings::GetSingleton()->exclude_spells_keywords.exc_keywords), false);

		// static auto fireKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFire");
		// static auto frostKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFrost");
		// static auto shockKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageShock");
		// static auto healKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRestoreHealth");

		// const auto NSV_Aimed_FF_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Self_FF_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TA_FF_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TL_FF_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_TL_FF_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Aimed_FF_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_nonHostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Self_FF_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_nonHostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TA_FF_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_nonHostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TL_FF_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_TL_FF_nonHostile_Effect")->As<RE::EffectSetting>();

		// const auto NSV_Aimed_CC_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Self_CC_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TA_CC_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TL_CC_Hostile_Effect = RE::TESForm::LookupByEditorID("NSV_TL_CC_Hostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Aimed_CC_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_nonHostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Self_CC_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_nonHostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TA_CC_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_nonHostile_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TL_CC_nonHostile_Effect = RE::TESForm::LookupByEditorID("NSV_TL_CC_nonHostile_Effect")->As<RE::EffectSetting>();

		// const auto NSV_Aimed_FF_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Self_FF_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TA_FF_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TL_FF_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_TL_FF_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Aimed_CC_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_Self_CC_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TA_CC_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_Heal_Effect")->As<RE::EffectSetting>();
		// const auto NSV_TL_CC_Heal_Effect = RE::TESForm::LookupByEditorID("NSV_TL_CC_Heal_Effect")->As<RE::EffectSetting>();

		for (auto indv_spell : spellList) {
			if (indv_spell) {
				for (auto indv_effect : indv_spell->effects) {
					if (indv_effect && indv_effect->baseEffect) {
						if (indv_effect->baseEffect->data.associatedForm) {
							auto limboform = indv_effect->baseEffect->data.associatedForm;
							if (limboform && limboform->Is(RE::FormType::Spell)) {
								auto a_spell = limboform->As<RE::SpellItem>();
								if (a_spell) {
									std::vector<RE::SpellItem*>::iterator position = std::find(spellList.begin(), spellList.end(), a_spell);
									if (position != spellList.end()) {
										spellList.erase(position);
									}
								}
							}
						}
					}
				}
			}
		}

		for (auto indv_spell : spellList) {
			if (indv_spell && indv_spell->GetSpellType() == RE::MagicSystem::SpellType::kSpell && indv_spell->GetDelivery() != RE::MagicSystem::Delivery::kTouch 
			&& indv_spell->GetCastingType() != RE::MagicSystem::CastingType::kScroll && indv_spell->GetCastingType() != RE::MagicSystem::CastingType::kConstantEffect) {

				bool invalid_spell = false;
				// bool hostile_flag = false;
				// bool det_flag = false;
				// bool fire_flag = false;
				// bool frost_flag = false;
				// bool shock_flag = false;
				// bool heal_flag = false;
				// bool valuemod_flag = false;
				// bool health_AV_flag = false;

				for (auto indv_effect : indv_spell->effects) {
					if (indv_effect && indv_effect->baseEffect) {
						auto Archy = indv_effect->baseEffect->data.archetype;
						// auto pAV = indv_effect->baseEffect->data.primaryAV;
						// auto sAV = indv_effect->baseEffect->data.secondaryAV;

						if (Archy == RE::EffectSetting::Archetype::kValueAndParts || Archy == RE::EffectSetting::Archetype::kTelekinesis || Archy == RE::EffectSetting::Archetype::kConcussion
						|| Archy == RE::EffectSetting::Archetype::kSlowTime || Archy == RE::EffectSetting::Archetype::kDisguise || Archy == RE::EffectSetting::Archetype::kVampireLord 
						|| Archy == RE::EffectSetting::Archetype::kGrabActor || Archy == RE::EffectSetting::Archetype::kWerewolfFeed|| Archy == RE::EffectSetting::Archetype::kCureAddiction
						|| Archy == RE::EffectSetting::Archetype::kLock|| Archy == RE::EffectSetting::Archetype::kOpen || Archy == RE::EffectSetting::Archetype::kWerewolf
						|| Archy == RE::EffectSetting::Archetype::kSpawnScriptedRef || Archy == RE::EffectSetting::Archetype::kCureDisease|| Archy == RE::EffectSetting::Archetype::kNightEye
						|| Archy == RE::EffectSetting::Archetype::kGuide || Archy == RE::EffectSetting::Archetype::kLight || Archy == RE::EffectSetting::Archetype::kDarkness 
						|| Archy == RE::EffectSetting::Archetype::kDetectLife) {
							invalid_spell = true;
						}

						// if (indv_effect->baseEffect->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kHostile)) {
						// 	hostile_flag = true;
						// }
						// if (indv_effect->baseEffect->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kDetrimental)) {
						// 	det_flag = true;
						// }
						// if (indv_effect->baseEffect->HasKeyword(fireKeyword)) {
						// 	fire_flag = true;
						// }
						// if (indv_effect->baseEffect->HasKeyword(frostKeyword)) {
						// 	frost_flag = true;
						// }
						// if (indv_effect->baseEffect->HasKeyword(shockKeyword)) {
						// 	shock_flag = true;
						// }
						// if (indv_effect->baseEffect->HasKeyword(healKeyword)) {
						// 	heal_flag = true;
						// }
						// if (Archy == RE::EffectSetting::Archetype::kValueModifier || Archy == RE::EffectSetting::Archetype::kDualValueModifier 
						// || Archy == RE::EffectSetting::Archetype::kPeakValueModifier) {
						// 	valuemod_flag = true;
						// }
						// if (pAV == RE::ActorValue::kHealth || sAV == RE::ActorValue::kHealth) {
						// 	health_AV_flag = true;
						// }
					}
				}

				if (invalid_spell){
					continue;
				}

				RE::Effect* effect = new RE::Effect;
				effect->cost = 0.0f;
				effect->effectItem.area = 0;
				effect->effectItem.duration = 0;
				effect->effectItem.magnitude = 0.0f;
				// effect->conditions.head = new RE::TESConditionItem;
				// effect->conditions.head->data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetRandomPercent;
				// effect->conditions.head->data.object = RE::CONDITIONITEMOBJECT::kSelf;
				// bool less_than_or = false;
				// if (OnMeleeHitHook::GetSingleton()->GenerateRandomInt(0/ 100) <= 50)
				// {
				// 	less_than_or = true;
				// 	effect->conditions.head->data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kLessThanOrEqualTo;
				// }else{
				// 	effect->conditions.head->data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kGreaterThanOrEqualTo;
				// }

				// if (less_than_or){
				// 	effect->conditions.head->data.comparisonValue.f = 2.0f;
				// }else{
				// 	effect->conditions.head->data.comparisonValue.f = 97.0f;
				// }

				auto check_CC_TA = GetEffect_CC_TA(indv_spell);
				auto check_CC_TL = GetEffect_CC_TL(indv_spell);
				auto check_CC_Self = GetEffect_CC_Self(indv_spell);
				auto check_CC_Aimed = GetEffect_CC_Aimed(indv_spell);

				auto check_FF_TA = GetEffect_FF_TA(indv_spell);
				auto check_FF_TL = GetEffect_FF_TL(indv_spell);
				auto check_FF_Self = GetEffect_FF_Self(indv_spell);
				auto check_FF_Aimed = GetEffect_FF_Aimed(indv_spell);

				bool unrecognized_spell = false;

				switch (indv_spell->GetCastingType()) {
				case RE::MagicSystem::CastingType::kFireAndForget:

					switch (indv_spell->GetDelivery()) {
					case RE::MagicSystem::Delivery::kAimed:
                        if (check_FF_Aimed.first){
							effect->baseEffect = check_FF_Aimed.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_Aimed_FF_Hostile_Effect;

						// }else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag) )){
						// 	effect->baseEffect = NSV_Aimed_FF_Heal_Effect;

						}else{
							//effect->baseEffect = NSV_Aimed_FF_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kSelf:
					    if (check_FF_Self.first){
							effect->baseEffect = check_FF_Self.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_Self_FF_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_Self_FF_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_Self_FF_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetActor:
					    if (check_FF_TA.first){
							effect->baseEffect = check_FF_TA.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_TA_FF_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_TA_FF_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_TA_FF_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetLocation:
					    if (check_FF_TL.first){
							effect->baseEffect = check_FF_TL.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_TL_FF_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_TL_FF_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_TL_FF_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					default:
						break;
					}

					break;

				case RE::MagicSystem::CastingType::kConcentration:

					switch (indv_spell->GetDelivery()) {
					case RE::MagicSystem::Delivery::kAimed:
					    if (check_CC_Aimed.first){
							effect->baseEffect = check_CC_Aimed.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_Aimed_CC_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_Aimed_CC_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_Aimed_CC_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kSelf:
					    if (check_CC_Self.first){
							effect->baseEffect = check_CC_Self.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_Self_CC_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_Self_CC_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_Self_CC_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetActor:
						if (check_CC_TA.first){
							effect->baseEffect = check_CC_TA.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_TA_CC_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_TA_CC_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_TA_CC_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					case RE::MagicSystem::Delivery::kTargetLocation:
					    if (check_CC_TL.first){
							effect->baseEffect = check_CC_TL.second;
						// }
						// else if (hostile_flag || fire_flag || frost_flag || shock_flag) {
						// 	effect->baseEffect = NSV_TL_CC_Hostile_Effect;

						// } else if (!det_flag && (heal_flag || (valuemod_flag && health_AV_flag))) {
						// 	effect->baseEffect = NSV_TL_CC_Heal_Effect;

						} else {
							//effect->baseEffect = NSV_TL_CC_nonHostile_Effect;
							unrecognized_spell = true;
						}
						break;

					default:
						break;
					}

					break;

				default:
					break;
				}

				if (unrecognized_spell){
					continue;
				}

				indv_spell->effects.push_back(effect);
			}
			continue;
		}
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

			if (a_actor->IsPlayerRef()){
				OnMeleeHitHook::GetSingleton()->scan_activeRunes(nullptr, nullptr, false, false, true);
			}else{
				OnMeleeHitHook::GetSingleton()->scan_activeRunes(a_actor, nullptr, false, true);
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
						std::vector<RE::BGSKeyword *> valid_keywordList;
						auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
						valid_keywordList.push_back(NSV_Magic_Heal);

						bool lefthand = false;


						if (a_spell->HasKeywordInArray(valid_keywordList, false)){

							auto limbospell = a_actor->GetActorRuntimeData().currentProcess;

							if (limbospell)
							{
								auto eSpell = limbospell->GetEquippedLeftHand();
								if (eSpell && eSpell->Is(RE::FormType::Spell) && eSpell->As<RE::SpellItem>() == a_spell)
								{
									lefthand = true;
								}

								switch (a_spell->GetCastingType())
								{
								case RE::MagicSystem::CastingType::kConcentration:
									switch (a_spell->GetDelivery())
									{
									case RE::MagicSystem::Delivery::kAimed:
										
										break;

									case RE::MagicSystem::Delivery::kTargetActor:
										
										break;

									case RE::MagicSystem::Delivery::kTargetLocation:
										
										break;

									default:
										break;
									}
									break;

								case RE::MagicSystem::CastingType::kFireAndForget:
									switch (a_spell->GetDelivery())
									{
									case RE::MagicSystem::Delivery::kAimed:
										
										break;

									case RE::MagicSystem::Delivery::kTargetActor:
										
										break;

									case RE::MagicSystem::Delivery::kTargetLocation:
										
										break;

									default:
										break;
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

		RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* event, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
		{
			auto Ename = event->eventName;

			if (Ename != "KID_KeywordDistributionDone")
			{
				return RE::BSEventNotifyControl::kContinue;
			}

			logger::info("recieved KID finished event"sv);

			Settings::GetSingleton()->Load();
			OnMeleeHitHook::Patch_Spell_List();

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

			OnMeleeHitHook::GetSingleton()->scan_activeRunes(nullptr, nullptr, false, false, true);

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

	void OnMeleeHitHook::Scan_Teammates_NeedHealing(RE::Actor *a_actor)
	{
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
		
		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_needheals = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					auto cur_health = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
					auto perm_health = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
					if (perm_health == 0 || cur_health == 0){
						continue;
					}
					if ((cur_health / perm_health) < 1.0f)
					{
						refr->SetGraphVariableBool("bNSV_Teammate_healtarget", true);
						team_needheals = true;

					}else{
						refr->SetGraphVariableBool("bNSV_Teammate_healtarget", false);
					}
				}
				continue;
			}
			if (team_needheals){
				if (combat_target){
					combat_target->SetGraphVariableBool("bNSV_Teammate_needhealing", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needhealing", true);
			}else{
				if (combat_target){
					combat_target->SetGraphVariableBool("bNSV_Teammate_needhealing", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needhealing", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_HasWeapon(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (IsWeaponOut(refr))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isWeaponOut", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isWeaponOut", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_hasWeapon_member", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasWeapon_member", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_hasWeapon_member", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasWeapon_member", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_Undead(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (refr->HasKeywordString("ActorTypeUndead"))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isUndead", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isUndead", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_hasUndead_member", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasUndead_member", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_hasUndead_member", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasUndead_member", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_CommandedActor(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (refr->IsCommandedActor())
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isSummoned", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_isSummoned", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_hasSummoned_member", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasSummoned_member", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_hasSummoned_member", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_hasSummoned_member", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_NeedBoundWeapon(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (HasBoundWeaponEquipped(refr, RE::MagicSystem::CastingSource::kLeftHand) && HasBoundWeaponEquipped(refr, RE::MagicSystem::CastingSource::kRightHand))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_boundweapontarget", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_boundweapontarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_needBoundWeapon_member", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_needBoundWeapon_member", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Team_needBoundWeapon_member", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Team_needBoundWeapon_member", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_NeedWard(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (Has_Magiceffect_Keyword(refr, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicWard")))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_wardtarget", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_wardtarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needward", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needward", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needward", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needward", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_NeedInvisibility(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if ((refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kInvisibility) == 0) && Has_Magiceffect_Keyword(refr, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicInvisibility")) && Has_Magiceffect_Keyword(refr, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicCloak"))) //
					{
						refr->SetGraphVariableBool("bNSV_Teammate_invisibilitytarget", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_invisibilitytarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needinvisibility", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needinvisibility", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needinvisibility", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needinvisibility", false);
			}
		}
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
	float OnMeleeHitHook::confidence_threshold(RE::Actor *a_actor, int confidence, bool inverse)
	{
		float result = 0.0f;

		if (inverse)
		{
			switch (confidence)
			{
			case 0:
				result = 0.1f;
				break;

			case 1:
				result = 0.3f;
				break;

			case 2:
				result = 0.5f;
				break;

			case 3:
				result = 0.7f;
				break;

			case 4:
				result = 0.9f;
				break;

			default:
				break;
			}
		}
		else
		{
			switch (confidence)
			{
			case 0:
				result = 1.25f;
				break;

			case 1:
				result = 1.0f;
				break;

			case 2:
				result = 0.75f;
				break;

			case 3:
				result = 0.5f;
				break;

			case 4:
				result = 0.25f;
				break;

			default:
				break;
			}
		}
		return result;
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_NeedSurvival(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_needheals = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}
					auto cur_health = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
					auto perm_health = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
					auto cur_stamina = refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
					auto perm_stamina = refr->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina);
					if (perm_health == 0 || cur_health == 0)
					{
						continue;
					}
					auto refr_target = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!refr_target){
						continue;
					}
					auto R = GetSingleton();
					auto personal_survival = R->get_personal_survivalRatio(refr, refr_target);
					auto personal_threat = R->get_personal_threatRatio(refr, refr_target);
					auto confidence = static_cast<int>(refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kConfidence));

					if ((personal_survival <= R->confidence_threshold(refr, confidence)) || (personal_threat <= R->confidence_threshold(a_actor, confidence)) 
					|| ((cur_health / perm_health) < 0.35f) || ((cur_stamina / perm_stamina) < 0.05f) || (refr->AsActorState()->GetKnockState() != RE::KNOCK_STATE_ENUM::kNormal) 
					|| (refr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kParalysis) > 0))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_survivaltarget", true);
						team_needheals = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_survivaltarget", false);
					}
				}
				continue;
			}
			if (team_needheals)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needsurvival", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needsurvival", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needsurvival", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needsurvival", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_NeedCloak(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (Has_Magiceffect_Keyword(refr, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicCloak")))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_cloaktarget", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_cloaktarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needcloak", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needcloak", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needcloak", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needcloak", false);
			}
		}
	}

	void hooks::OnMeleeHitHook::Scan_Teammates_NeedArmour(RE::Actor *a_actor){
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			bool team_hasweapon = false;
			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor *refr = ally.get();

					if (refr == a_actor || refr->IsDead())
					{
						continue;
					}

					if (Has_Magiceffect_Keyword(refr, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicArmorSpell")))
					{
						refr->SetGraphVariableBool("bNSV_Teammate_armourtarget", true);
						team_hasweapon = true;
					}
					else
					{
						refr->SetGraphVariableBool("bNSV_Teammate_armourtarget", false);
					}
				}
				continue;
			}
			if (team_hasweapon)
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needarmour", true);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needarmour", true);
			}
			else
			{
				if (combat_target)
				{
					combat_target->SetGraphVariableBool("bNSV_Teammate_needarmour", false);
				}
				a_actor->SetGraphVariableBool("bNSV_Self_Teammate_needarmour", false);
			}
		}
	}

	void OnMeleeHitHook::Execute_Cast(RE::Actor *a_actor, RE::SpellItem *a_spell, bool lefthand, bool FF_aimed, bool FF_TA, bool FF_TL, bool CC_aimed, bool CC_TA, bool CC_TL)
	{
		auto combat_target = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();

		if (const auto combatGroup = a_actor->GetCombatGroup())
		{
			const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
			const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
			const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
			const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
			const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
			const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();
			const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
			const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
			const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
			const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();
			const auto MagicRune = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRune");

			bool successful_cast = false;
			bool trap = false;
			const auto caster = lefthand ? a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand) : a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand);
			RE::Actor *focus_ally = nullptr;

			for (auto &memberData : combatGroup->members)
			{
				if (auto ally = memberData.memberHandle.get(); ally)
				{
					RE::Actor* refr = ally.get();

					if (refr == a_actor){
						if (!(FF_TL || CC_TL))
						{
							continue;
						}
					}
					bool hasLOS = false;
					if (a_actor->HasLineOfSight(refr, hasLOS) && !hasLOS)
					{
						if (!(FF_TL || CC_TL))
						{
							continue;
						}
					}
					RE::BGSAttackData *attackdata = GetSingleton()->get_attackData(a_actor);
					auto angle = GetSingleton()->get_angle_he_me(refr, a_actor, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 25.0f;

					if  (FF_aimed || CC_aimed){
						if (attackAngle > 10.0f){
							attackAngle = 10.0f;
						}
					}
					if (abs(angle) > attackAngle)
					{
						if (!(FF_TL || CC_TL))
						{
							continue;
						}
					}

					if (!(FF_TL || CC_TL)){
						if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
						{
							auto bNSV_Teammate_cloaktarget = false;
							if ((refr->GetGraphVariableBool("bNSV_Teammate_cloaktarget", bNSV_Teammate_cloaktarget) && !bNSV_Teammate_cloaktarget))
							{
								continue;
							}
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
								
							}
							
						}
						if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
						{
							auto bNSV_Teammate_armourtarget = false;
							if ((refr->GetGraphVariableBool("bNSV_Teammate_armourtarget", bNSV_Teammate_armourtarget) && !bNSV_Teammate_armourtarget))
							{
								continue;
							}
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
								
							}
						}
						if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
						{
							auto bNSV_Teammate_healtarget = false;
							if ((refr->GetGraphVariableBool("bNSV_Teammate_healtarget", bNSV_Teammate_healtarget) && !bNSV_Teammate_healtarget))
							{
								continue;
							}
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
							}
						}
						if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
						{
							auto bNSV_Teammate_invisibilitytarget = false;
							if ((refr->GetGraphVariableBool("bNSV_Teammate_invisibilitytarget", bNSV_Teammate_invisibilitytarget) && !bNSV_Teammate_invisibilitytarget))
							{
								continue;
							}
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
								
							}
						}
						if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
						{
							if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
							{
								auto bNSV_Teammate_boundweapontarget = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_boundweapontarget", bNSV_Teammate_boundweapontarget) && !bNSV_Teammate_boundweapontarget))
								{
									continue;
								}
								if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
								{
									auto bNSV_Teammate_isUndead = false;
									if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
									{
										continue;
									}
								}
								else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
								{
									auto bNSV_Teammate_isSummoned = false;
									if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
									{
										continue;
									}
								}
							}
						}
						if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
						{
							auto bNSV_Teammate_wardtarget = false;
							if ((refr->GetGraphVariableBool("bNSV_Teammate_wardtarget", bNSV_Teammate_wardtarget) && !bNSV_Teammate_wardtarget))
							{
								continue;
							}
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
								
							}
						}
						if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
						{
							if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
							{
								return;
							}
							else
							{
								if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
								{
									auto bNSV_Teammate_isUndead = false;
									if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
									{
										continue;
									}
								}
								else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
								{
									auto bNSV_Teammate_isSummoned = false;
									if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
									{
										continue;
									}
								}
							}
						}
						if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
						{
							auto bNSV_Teammate_survivaltarget = false;
							if ((refr->GetGraphVariableBool("bNSV_Teammate_survivaltarget", bNSV_Teammate_survivaltarget) && !bNSV_Teammate_survivaltarget))
							{
								continue;
							}
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
								
							}
						}
						if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
						{
							if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
							{
								auto bNSV_Teammate_isUndead = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isUndead", bNSV_Teammate_isUndead) && !bNSV_Teammate_isUndead))
								{
									continue;
								}
							}
							else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
							{
								auto bNSV_Teammate_isSummoned = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isSummoned", bNSV_Teammate_isSummoned) && !bNSV_Teammate_isSummoned))
								{
									continue;
								}
								
							}
							else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
							{
								auto bNSV_Teammate_isWeaponOut = false;
								if ((refr->GetGraphVariableBool("bNSV_Teammate_isWeaponOut", bNSV_Teammate_isWeaponOut) && !bNSV_Teammate_isWeaponOut))
								{
									continue;
								}
							}
						
						}
					}else{
						if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
						{
							if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
							{
								return;
								
							}
							else if (NSV_Tag_Weapon && !a_spell->HasKeyword(NSV_Tag_Weapon))
							{
								return;
							}
						}

						if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap)){
							trap = true;
							auto Effect_List = a_spell->effects;
							for (auto Effect : Effect_List)
							{
								if (Effect && Effect->baseEffect)
								{
									if (Effect->baseEffect->HasKeyword(MagicRune))
									{
										caster->PrepareSound(RE::MagicSystem::SoundID::kRelease, a_spell->As<RE::MagicItem>());
										break;
									}
								}
								continue;
							}
						}
					}
					
					successful_cast = true;
					if (!(FF_TL || CC_TL))
					{
						focus_ally = refr;
					}else{
						if (trap){
							focus_ally = a_actor;
						}else if(combat_target){
							focus_ally = combat_target;
						}
					}
					break;
				}
				continue;
			}
			if (successful_cast){
				caster->desiredTarget = focus_ally;
				if (!(FF_TL || CC_TL))
				{
					a_actor->GetActorRuntimeData().currentCombatTarget = focus_ally->GetHandle();
					if (a_actor->GetActorRuntimeData().currentProcess)
					{
						a_actor->GetActorRuntimeData().currentProcess->SetHeadtrackTarget(focus_ally, focus_ally->data.location);
					}
				}
			}else{
				caster->InterruptCastImpl(false);
				a_actor->NotifyAnimationGraph("InterruptCast");
			}
		}
	}

	void OnMeleeHitHook::scan_activeRunes([[maybe_unused]] RE::Actor *a_actor, [[maybe_unused]] RE::Projectile *a_rune, bool insert, bool clear, bool clear_all)
	{
		uniqueLocker lock(mtx_RunesCast);
		if (insert){
			auto itt = _RunesCast.find(a_actor);
			if (itt == _RunesCast.end()){
				std::vector<RE::Projectile *> Hen;
				_RunesCast.insert({a_actor, Hen});
			}
		}
		
		for (auto it = _RunesCast.begin(); it != _RunesCast.end(); ++it){
			if (insert){
				if (it->first == a_actor){
					if (!it->second.empty()){
						for (auto rune : it->second){
							if (rune){
								rune->GetProjectileRuntimeData().explosion = nullptr;
								rune->GetProjectileRuntimeData().avEffect = nullptr;
								rune->GetProjectileRuntimeData().spell = nullptr;
								rune->GetProjectileRuntimeData().flags = 1 << 5;
								rune->GetProjectileRuntimeData().transparency = 1000000.0f;
								rune->GetProjectileRuntimeData().scale = 0.0f;
								rune->GetProjectileRuntimeData().livingTime = 0.0f;
								rune = nullptr;
							}
						}
						it->second.clear();
					}
					it->second.push_back(a_rune);
					break;
				}
			}
			if (clear){
				if (it->first == a_actor){
					if (!it->second.empty()){
						for (auto rune : it->second){
							if (rune){
								rune->GetProjectileRuntimeData().explosion = nullptr;
								rune->GetProjectileRuntimeData().avEffect = nullptr;
								rune->GetProjectileRuntimeData().spell = nullptr;
								rune->GetProjectileRuntimeData().flags = 1 << 5;
								rune->GetProjectileRuntimeData().transparency = 1000000.0f;
								rune->GetProjectileRuntimeData().scale = 0.0f;
								rune->GetProjectileRuntimeData().livingTime = 0.0f;
								rune = nullptr;
							}
						}
						it->second.clear();
					}
					_RunesCast.erase(it);
					break;
				}
			}
			if (clear_all){
				if (it->first){
					if (!it->second.empty()){
						for (auto rune : it->second){
							if (rune){
								rune->GetProjectileRuntimeData().explosion = nullptr;
								rune->GetProjectileRuntimeData().avEffect = nullptr;
								rune->GetProjectileRuntimeData().spell = nullptr;
								rune->GetProjectileRuntimeData().flags = 1 << 5;
								rune->GetProjectileRuntimeData().transparency = 1000000.0f;
								rune->GetProjectileRuntimeData().scale = 0.0f;
								rune->GetProjectileRuntimeData().livingTime = 0.0f;
								rune = nullptr;
							}
						}
						it->second.clear();
					}
					_RunesCast.erase(it);
				}
			}
			continue;
		}
	}

	void hooks::OnMeleeHitHook::Cast_Rune(RE::Actor *actor, RE::SpellItem *a_spell, RE::BGSProjectile *a_projectile, bool lefthand){
		auto Rune = actor->PlaceObjectAtMe(a_projectile, false).get()->AsProjectile();
		if (Rune){
			GetSingleton()->scan_activeRunes(actor, Rune, true);
			Rune->SetActorCause(actor->GetActorCause());
			Rune->GetProjectileRuntimeData().shooter = actor;
			Rune->GetProjectileRuntimeData().spell = a_spell->As<RE::MagicItem>();
			Rune->GetProjectileRuntimeData().avEffect = a_spell->As<RE::MagicItem>()->avEffectSetting;
			//Rune->GetProjectileRuntimeData().flags = 1 << 19;
			auto combat_target = actor->GetActorRuntimeData().currentCombatTarget.get().get();
			if (combat_target){
				Rune->GetProjectileRuntimeData().desiredTarget = combat_target;
			}
			actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kMagicka, -(a_spell->As<RE::MagicItem>()->CalculateMagickaCost(actor)));
		}
	}

	void OnMeleeHitHook::Cast_PotentialRune(RE::Actor *actor, bool lefthand)
	{
		auto limbospell = actor->GetActorRuntimeData().currentProcess;
		const auto MagicRune = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicRune");
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NSV_Magic_Trap");

		if (limbospell)
		{
			if (lefthand)
			{
				auto eSpell = limbospell->GetEquippedLeftHand();
				if (eSpell && eSpell->Is(RE::FormType::Spell))
				{
					auto rSpell = eSpell->As<RE::SpellItem>();
					if (rSpell->HasKeyword(NSV_Magic_Trap) && rSpell->GetDelivery() == RE::MagicSystem::Delivery::kTargetLocation)
					{
						auto Effect_List = rSpell->effects;
						for (auto Effect : Effect_List)
						{
							if (Effect && Effect->baseEffect)
							{
								if (Effect->baseEffect->HasKeyword(MagicRune))
								{
									auto IsCastingDual = false;
									if ((actor->GetGraphVariableBool("IsCastingDual", IsCastingDual) && IsCastingDual))
									{
										if (Effect->baseEffect->data.dualCastData && Effect->baseEffect->data.dualCastData->data.pProjectile)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.dualCastData->data.pProjectile, lefthand);
										}else{
											if (Effect->baseEffect->data.projectileBase)
											{
												Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
											}
										}
									}else{
										if (Effect->baseEffect->data.projectileBase)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
										}
									}
									break;
								}
							}
							continue;
						}
					}
				}
			}
			else
			{
				auto eSpell = limbospell->GetEquippedRightHand();
				if (eSpell && eSpell->Is(RE::FormType::Spell))
				{
					auto rSpell = eSpell->As<RE::SpellItem>();
					if (rSpell->HasKeyword(NSV_Magic_Trap) && rSpell->GetDelivery() == RE::MagicSystem::Delivery::kTargetLocation)
					{
						auto Effect_List = rSpell->effects;
						for (auto Effect : Effect_List)
						{
							if (Effect && Effect->baseEffect)
							{
								if (Effect->baseEffect->HasKeyword(MagicRune))
								{
									auto IsCastingDual = false;
									if ((actor->GetGraphVariableBool("IsCastingDual", IsCastingDual) && IsCastingDual))
									{
										if (Effect->baseEffect->data.dualCastData && Effect->baseEffect->data.dualCastData->data.pProjectile)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.dualCastData->data.pProjectile, lefthand);
										}else{
											if (Effect->baseEffect->data.projectileBase)
											{
												Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
											}
										}
									}else{
										if (Effect->baseEffect->data.projectileBase)
										{
											Cast_Rune(actor, rSpell, Effect->baseEffect->data.projectileBase, lefthand);
										}
									}
									break;
								}
							}
							continue;
						}
					}
				}
			}
		}
	}

	void OnMeleeHitHook::GetAttackSpell(RE::Actor *a_actor, bool lefthand)
	{
		OnMeleeHitHook::Scan_Teammates_NeedHealing(a_actor);
		OnMeleeHitHook::Scan_Teammates_NeedWard(a_actor);
		OnMeleeHitHook::Scan_Teammates_NeedBoundWeapon(a_actor);
		OnMeleeHitHook::Scan_Teammates_CommandedActor(a_actor);
		OnMeleeHitHook::Scan_Teammates_HasWeapon(a_actor);
		OnMeleeHitHook::Scan_Teammates_NeedArmour(a_actor);
		OnMeleeHitHook::Scan_Teammates_NeedCloak(a_actor);
		OnMeleeHitHook::Scan_Teammates_NeedInvisibility(a_actor);
		OnMeleeHitHook::Scan_Teammates_NeedSurvival(a_actor);
		OnMeleeHitHook::Scan_Teammates_Undead(a_actor);

		auto limbospell = a_actor->GetActorRuntimeData().currentProcess;

		if (limbospell)
		{
			auto key_list = RE::TESForm::LookupByEditorID("NSV_Keywords_FormList")->As<RE::BGSListForm>();
			std::vector<RE::BGSKeyword *> valid_keywordList;

			for (auto a_key : key_list->forms)
			{
				if (a_key && a_key->Is(RE::FormType::Keyword))
				{
					valid_keywordList.push_back(a_key->As<RE::BGSKeyword>());
				}
			}
			if (lefthand)
			{
				auto eSpell = limbospell->GetEquippedLeftHand();
				if (eSpell && eSpell->Is(RE::FormType::Spell))
				{
					auto a_spell = eSpell->As<RE::SpellItem>();
					if (a_spell)
					{
						if (a_spell->HasKeywordInArray(valid_keywordList, false))
						{
							switch (a_spell->GetCastingType())
							{
							case RE::MagicSystem::CastingType::kConcentration:
								switch (a_spell->GetDelivery())
								{
								case RE::MagicSystem::Delivery::kAimed:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, false, true);
									break;

								case RE::MagicSystem::Delivery::kTargetActor:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, true);
									break;

								case RE::MagicSystem::Delivery::kTargetLocation:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, false, true);
									break;

								default:
									break;
								}
								break;

							case RE::MagicSystem::CastingType::kFireAndForget:
								switch (a_spell->GetDelivery())
								{
								case RE::MagicSystem::Delivery::kAimed:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, true);
									break;

								case RE::MagicSystem::Delivery::kTargetActor:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, true);
									break;

								case RE::MagicSystem::Delivery::kTargetLocation:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, true);
									break;

								default:
									break;
								}
								break;

							default:
								break;
							}
						}
					}
				}
			}
			else
			{
				auto eSpell = limbospell->GetEquippedRightHand();
				if (eSpell && eSpell->Is(RE::FormType::Spell))
				{
					auto a_spell = eSpell->As<RE::SpellItem>();
					if (a_spell)
					{
						if (a_spell->HasKeywordInArray(valid_keywordList, false))
						{
							switch (a_spell->GetCastingType())
							{
							case RE::MagicSystem::CastingType::kConcentration:
								switch (a_spell->GetDelivery())
								{
								case RE::MagicSystem::Delivery::kAimed:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, false, true);
									break;

								case RE::MagicSystem::Delivery::kTargetActor:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, true);
									break;

								case RE::MagicSystem::Delivery::kTargetLocation:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, false, false, false, true);
									break;

								default:
									break;
								}
								break;

							case RE::MagicSystem::CastingType::kFireAndForget:
								switch (a_spell->GetDelivery())
								{
								case RE::MagicSystem::Delivery::kAimed:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, true);
									break;

								case RE::MagicSystem::Delivery::kTargetActor:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, true);
									break;

								case RE::MagicSystem::Delivery::kTargetLocation:
									OnMeleeHitHook::Execute_Cast(a_actor, a_spell, lefthand, false, false, true);
									break;

								default:
									break;
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
					OnMeleeHitHook::GetAttackSpell(actor, true);
				}
			}
			break;

		case "BeginCastRight"_h:
			if (!actor->IsPlayerRef())
			{
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{
					OnMeleeHitHook::GetAttackSpell(actor);
				}
			}
			break;

		case "MLh_SpellFire_Event"_h:
			if (!actor->IsPlayerRef())
			{
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{
					OnMeleeHitHook::Cast_PotentialRune(actor, true);
				}
			}
			break;

		case "MRh_SpellFire_Event"_h:
			if (!actor->IsPlayerRef())
			{
				auto bNSV_IsinCombat = false;
				if ((actor->GetGraphVariableBool("bNSV_IsinCombat", bNSV_IsinCombat) && bNSV_IsinCombat))
				{
					OnMeleeHitHook::Cast_PotentialRune(actor);
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

	InputEventHandler* InputEventHandler::GetSingleton()
	{
		static InputEventHandler singleton;
		return std::addressof(singleton);
	}

	RE::BSEventNotifyControl InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
	{
		// using EventType = RE::INPUT_EVENT_TYPE;
		// using DeviceType = RE::INPUT_DEVICE;

		if (!a_event) {
			return RE::BSEventNotifyControl::kContinue;
		}

		// for (auto event = *a_event; event; event = event->next) {
		// 	if (event->eventType != EventType::kButton) {
		// 		continue;
		// 	}

		// 	auto button = static_cast<RE::ButtonEvent*>(event);
		// 	if (!button) {
		// 		continue;
		// 	}
		// 	auto key = button->idCode;
		// 	if (!key) {
		// 		continue;
		// 	}

		// 	switch (button->device.get()) {
		// 	case DeviceType::kMouse:
		// 		key += kMouseOffset;
		// 		break;
		// 	case DeviceType::kKeyboard:
		// 		key += kKeyboardOffset;
		// 		break;
		// 	case DeviceType::kGamepad:
		// 		key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
		// 		break;
		// 	default:
		// 		continue;
		// 	}

		// 	if (key == 274)
		// 	{
		// 		auto Playerhandle = RE::PlayerCharacter::GetSingleton();
		// 		if (button->IsDown() || button->IsHeld() || button->IsPressed()){
		// 			Playerhandle->SetGraphVariableBool("bPSV_Toggle_PowerSprintAttack", true);
		// 		}
		// 		if(button->IsUp()){
		// 			Playerhandle->SetGraphVariableBool("bPSV_Toggle_PowerSprintAttack", false);
		// 		}
		// 		break;
		// 	}
		// }

		return RE::BSEventNotifyControl::kContinue;
	}

	std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		std::uint32_t index;
		switch (a_key) {
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalid;
			break;
		}

		return index != kInvalid ? index + kGamepadOffset : kInvalid;
	}

	void InputEventHandler::SinkEventHandlers()
	{
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(InputEventHandler::GetSingleton());
		logger::info("Added input event sink");
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

	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_CC_TA(RE::SpellItem *a_spell)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_CloakUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_CloakSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_CloakBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ArmourUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ArmourSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ArmourBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ControlUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ControlSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ControlWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_ControlBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_HealUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_HealSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_HealBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_InvisibilityUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_InvisibilitySummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_InvisibilityBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_CalmSeduceBase")->As<RE::EffectSetting>();
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_FearUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_FearSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_FearBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_SummonWeaponUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_SummonWeaponSummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_SummonWeapon")->As<RE::EffectSetting>();
				}
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_WardUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_WardSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_WardBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_AbsorbBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DamageBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TeleportEnemyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TeleportEnemySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TeleportEnemy")->As<RE::EffectSetting>();
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TeleportAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TeleportAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TeleportAlly")->As<RE::EffectSetting>();
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_SurviveUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_SurviveSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_SurviveBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_BuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_BuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_BuffWeapon")->As<RE::EffectSetting>();
			}else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_BuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DebuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DebuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DebuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_DebuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TrapUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TrapSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TrapWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_CC_TrapBase")->As<RE::EffectSetting>();
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_FF_Aimed(RE::SpellItem *a_spell)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_CloakUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_CloakSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_CloakBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ArmourUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ArmourSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ArmourBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ControlUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ControlSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ControlWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_ControlBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_HealUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_HealSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_HealBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_InvisibilityUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_InvisibilitySummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_InvisibilityBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_CalmSeduceBase")->As<RE::EffectSetting>();
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_FearUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_FearSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_FearBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SummonWeaponUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SummonWeaponSummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SummonWeapon")->As<RE::EffectSetting>();
				}
			}else if(NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally)){
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SummonAlly")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_WardUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_WardSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_WardBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_AbsorbBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DamageBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TeleportEnemyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TeleportEnemySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TeleportEnemy")->As<RE::EffectSetting>();
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TeleportAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TeleportAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TeleportAlly")->As<RE::EffectSetting>();
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SurviveUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SurviveSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_SurviveBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_BuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_BuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_BuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_BuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DebuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DebuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DebuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_DebuffBase")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TrapUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TrapSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TrapWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_FF_TrapBase")->As<RE::EffectSetting>();
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_CC_Aimed(RE::SpellItem *a_spell)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_CloakUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_CloakSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_CloakBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ArmourUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ArmourSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ArmourBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ControlUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ControlSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ControlWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_ControlBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_HealUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_HealSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_HealBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_InvisibilityUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_InvisibilitySummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_InvisibilityBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_CalmSeduceBase")->As<RE::EffectSetting>();
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_FearUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_FearSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_FearBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_SummonWeaponUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_SummonWeaponSummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_SummonWeapon")->As<RE::EffectSetting>();
				}
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_WardUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_WardSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_WardBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_AbsorbBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DamageBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TeleportEnemyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TeleportEnemySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TeleportEnemy")->As<RE::EffectSetting>();
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TeleportAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TeleportAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TeleportAlly")->As<RE::EffectSetting>();
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_SurviveUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_SurviveSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_SurviveBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_BuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_BuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_BuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_BuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DebuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DebuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DebuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_DebuffBase")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TrapUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TrapSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TrapWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Aimed_CC_TrapBase")->As<RE::EffectSetting>();
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}
	std::pair<bool, RE::EffectSetting *> OnMeleeHitHook::GetEffect_FF_TA(RE::SpellItem *a_spell)
	{
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_CloakUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_CloakSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_CloakBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ArmourUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ArmourSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ArmourBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ControlUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ControlSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ControlWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_ControlBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_HealUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_HealSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_HealBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_InvisibilityUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_InvisibilitySummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_InvisibilityBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_CalmSeduceBase")->As<RE::EffectSetting>();
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_FearUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_FearSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_FearBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SummonWeaponUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SummonWeaponSummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SummonWeapon")->As<RE::EffectSetting>();
				}
			}else if(NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally)){
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SummonAlly")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_WardUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_WardSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_WardBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_AbsorbBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DamageBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TeleportEnemyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TeleportEnemySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TeleportEnemy")->As<RE::EffectSetting>();
				}
			}
			else
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TeleportAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TeleportAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TeleportAlly")->As<RE::EffectSetting>();
				}
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SurviveUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SurviveSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_SurviveBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_BuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_BuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_BuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_BuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DebuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DebuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DebuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_DebuffBase")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TrapUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TrapSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TrapWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TA_FF_TrapBase")->As<RE::EffectSetting>();
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_FF_Self(RE::SpellItem *a_spell){
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally)){
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_CloakAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_CloakAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_CloakAlly")->As<RE::EffectSetting>();
				}
			}else{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_CloakBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ArmourAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ArmourAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ArmourAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ArmourBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ControlUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ControlSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ControlWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_ControlBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_HealAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_HealAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_HealAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_HealBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_InvisibilityAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_InvisibilityAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_InvisibilityAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_InvisibilityBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_CalmSeduceBase")->As<RE::EffectSetting>();
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_FearUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_FearSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_FearBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon)){

					if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
					{
						a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SummonAllyWeaponUndead")->As<RE::EffectSetting>();
					}
					else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
					{
						a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SummonAllyWeaponSummoned")->As<RE::EffectSetting>();
					}
					else
					{
						a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SummonAllyWeapon")->As<RE::EffectSetting>();
					}
				}else{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SummonAlly")->As<RE::EffectSetting>();
				}
				
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SummonWeapon")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_WardAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_WardAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_WardAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_WardBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_AbsorbBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DamageBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportEnemyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportEnemySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportEnemy")->As<RE::EffectSetting>();
				}
			}
			else if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportAlly")->As<RE::EffectSetting>();
				}
			}else{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TeleportBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SurviveAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SurviveAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SurviveAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_SurviveBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_BuffAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_BuffAllySummoned")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_BuffAllyWeapon")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_BuffAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_BuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DebuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DebuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DebuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_DebuffBase")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TrapUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TrapSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TrapWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_FF_TrapBase")->As<RE::EffectSetting>();
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_CC_Self(RE::SpellItem *a_spell){
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Cloak && a_spell->HasKeyword(NSV_Magic_Cloak))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_CloakAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_CloakAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_CloakAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_CloakBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Armour && a_spell->HasKeyword(NSV_Magic_Armour))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ArmourAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ArmourAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ArmourAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ArmourBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Control && a_spell->HasKeyword(NSV_Magic_Control))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ControlUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ControlSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ControlWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_ControlBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Heal && a_spell->HasKeyword(NSV_Magic_Heal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_HealAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_HealAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_HealAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_HealBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Invisibility && a_spell->HasKeyword(NSV_Magic_Invisibility))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_InvisibilityAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_InvisibilityAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_InvisibilityAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_InvisibilityBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_CalmSeduce && a_spell->HasKeyword(NSV_Magic_CalmSeduce))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_CalmSeduceBase")->As<RE::EffectSetting>();
		}
		if (NSV_Magic_Fear && a_spell->HasKeyword(NSV_Magic_Fear))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_FearUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_FearSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_FearBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
				{

					if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
					{
						a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SummonAllyWeaponUndead")->As<RE::EffectSetting>();
					}
					else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
					{
						a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SummonAllyWeaponSummoned")->As<RE::EffectSetting>();
					}
					else
					{
						a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SummonAllyWeapon")->As<RE::EffectSetting>();
					}
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SummonAlly")->As<RE::EffectSetting>();
				}
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SummonWeapon")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Ward && a_spell->HasKeyword(NSV_Magic_Ward))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_WardAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_WardAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_WardAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_WardBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Magic_Absorb && a_spell->HasKeyword(NSV_Magic_Absorb))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_AbsorbBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Damage && a_spell->HasKeyword(NSV_Magic_Damage))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageSummoned")->As<RE::EffectSetting>();
			}
			else
			{
				if (const auto perk_list = RE::TESForm::LookupByEditorID("NSV_Perks_FormList")->As<RE::BGSListForm>())
				{
					if (a_spell->data.castingPerk && perk_list->HasForm(a_spell->data.castingPerk))
					{
						switch (GetSingleton()->GenerateRandomInt(1, 99))
						{
						case 1:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_1")->As<RE::EffectSetting>();
							break;

						case 2:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_2")->As<RE::EffectSetting>();
							break;

						case 3:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_3")->As<RE::EffectSetting>();
							break;

						case 4:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_4")->As<RE::EffectSetting>();
							break;

						case 5:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_5")->As<RE::EffectSetting>();
							break;

						case 6:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_6")->As<RE::EffectSetting>();
							break;

						case 7:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_7")->As<RE::EffectSetting>();
							break;

						case 8:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_8")->As<RE::EffectSetting>();
							break;

						case 9:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_9")->As<RE::EffectSetting>();
							break;

						case 10:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_10")->As<RE::EffectSetting>();
							break;

						case 11:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_11")->As<RE::EffectSetting>();
							break;

						case 12:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_12")->As<RE::EffectSetting>();
							break;

						case 13:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_13")->As<RE::EffectSetting>();
							break;

						case 14:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_14")->As<RE::EffectSetting>();
							break;

						case 15:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_15")->As<RE::EffectSetting>();
							break;

						case 16:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_16")->As<RE::EffectSetting>();
							break;

						case 17:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_17")->As<RE::EffectSetting>();
							break;

						case 18:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_18")->As<RE::EffectSetting>();
							break;

						case 19:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_19")->As<RE::EffectSetting>();
							break;

						case 20:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_20")->As<RE::EffectSetting>();
							break;

						case 21:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_21")->As<RE::EffectSetting>();
							break;

						case 22:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_22")->As<RE::EffectSetting>();
							break;

						case 23:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_23")->As<RE::EffectSetting>();
							break;

						case 24:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_24")->As<RE::EffectSetting>();
							break;

						case 25:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_25")->As<RE::EffectSetting>();
							break;

						case 26:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_26")->As<RE::EffectSetting>();
							break;

						case 27:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_27")->As<RE::EffectSetting>();
							break;

						case 28:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_28")->As<RE::EffectSetting>();
							break;

						case 29:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_29")->As<RE::EffectSetting>();
							break;

						case 30:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_30")->As<RE::EffectSetting>();
							break;

						case 31:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_31")->As<RE::EffectSetting>();
							break;

						case 32:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_32")->As<RE::EffectSetting>();
							break;

						case 33:
							a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DamageBase_33")->As<RE::EffectSetting>();
							break;

						default:
							break;
						}
					}
				}
				
			}
		}
		if (NSV_Magic_Teleport && a_spell->HasKeyword(NSV_Magic_Teleport))
		{
			if (NSV_Tag_Enemy && a_spell->HasKeyword(NSV_Tag_Enemy))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportEnemyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportEnemySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportEnemy")->As<RE::EffectSetting>();
				}
			}
			else if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TeleportBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Survival_Lethal && a_spell->HasKeyword(NSV_Survival_Lethal))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SurviveAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SurviveAllySummoned")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SurviveAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_SurviveBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Buff && a_spell->HasKeyword(NSV_Generic_Buff))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_BuffAllyUndead")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_BuffAllySummoned")->As<RE::EffectSetting>();
				}
				else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_BuffAllyWeapon")->As<RE::EffectSetting>();
				}
				else
				{
					a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_BuffAlly")->As<RE::EffectSetting>();
				}
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_BuffBase")->As<RE::EffectSetting>();
			}
		}
		if (NSV_Generic_Debuff && a_spell->HasKeyword(NSV_Generic_Debuff))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DebuffUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DebuffSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DebuffWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_DebuffBase")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			if (NSV_Tag_Undead && a_spell->HasKeyword(NSV_Tag_Undead))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TrapUndead")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Summoned && a_spell->HasKeyword(NSV_Tag_Summoned))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TrapSummoned")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TrapWeapon")->As<RE::EffectSetting>();
			}
			else
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_Self_CC_TrapBase")->As<RE::EffectSetting>();
			}
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_FF_TL(RE::SpellItem *a_spell){
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TL_FF_SummonAlly")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TL_FF_SummonWeapon")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_TL_FF_TrapBase")->As<RE::EffectSetting>();
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}

	std::pair<bool, RE::EffectSetting *> hooks::OnMeleeHitHook::GetEffect_CC_TL(RE::SpellItem *a_spell){
		RE::EffectSetting *a_effect = nullptr;
		bool detected_key = false;

		const auto NSV_Magic_Cloak = RE::TESForm::LookupByEditorID("NSV_Magic_Cloak")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Armour = RE::TESForm::LookupByEditorID("NSV_Magic_Armour")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Control = RE::TESForm::LookupByEditorID("NSV_Magic_Control")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Heal = RE::TESForm::LookupByEditorID("NSV_Magic_Heal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Invisibility = RE::TESForm::LookupByEditorID("NSV_Magic_Invisibility")->As<RE::BGSKeyword>();
		const auto NSV_Magic_CalmSeduce = RE::TESForm::LookupByEditorID("NSV_Magic_CalmSeduce")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Fear = RE::TESForm::LookupByEditorID("NSV_Magic_Fear")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Summon = RE::TESForm::LookupByEditorID("NSV_Magic_Summon")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Ward = RE::TESForm::LookupByEditorID("NSV_Magic_Ward")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Absorb = RE::TESForm::LookupByEditorID("NSV_Magic_Absorb")->As<RE::BGSKeyword>();

		const auto NSV_Magic_Damage = RE::TESForm::LookupByEditorID("NSV_Magic_Damage")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Teleport = RE::TESForm::LookupByEditorID("NSV_Magic_Teleport")->As<RE::BGSKeyword>();
		const auto NSV_Survival_Lethal = RE::TESForm::LookupByEditorID("NSV_Survival_Lethal")->As<RE::BGSKeyword>();
		const auto NSV_Magic_Trap = RE::TESForm::LookupByEditorID("NSV_Magic_Trap")->As<RE::BGSKeyword>();

		const auto NSV_Generic_Buff = RE::TESForm::LookupByEditorID("NSV_Generic_Buff")->As<RE::BGSKeyword>();
		const auto NSV_Generic_Debuff = RE::TESForm::LookupByEditorID("NSV_Generic_Debuff")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Undead = RE::TESForm::LookupByEditorID("NSV_Tag_Undead")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Vampiric = RE::TESForm::LookupByEditorID("NSV_Tag_Vampiric")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Summoned = RE::TESForm::LookupByEditorID("NSV_Tag_Summoned")->As<RE::BGSKeyword>();

		const auto NSV_Tag_Ally = RE::TESForm::LookupByEditorID("NSV_Tag_Ally")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Enemy = RE::TESForm::LookupByEditorID("NSV_Tag_Enemy")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Weapon = RE::TESForm::LookupByEditorID("NSV_Tag_Weapon")->As<RE::BGSKeyword>();
		const auto NSV_Tag_Area = RE::TESForm::LookupByEditorID("NSV_Tag_Area")->As<RE::BGSKeyword>();

		if (NSV_Magic_Summon && a_spell->HasKeyword(NSV_Magic_Summon))
		{
			if (NSV_Tag_Ally && a_spell->HasKeyword(NSV_Tag_Ally))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TL_CC_SummonAlly")->As<RE::EffectSetting>();
			}
			else if (NSV_Tag_Weapon && a_spell->HasKeyword(NSV_Tag_Weapon))
			{
				a_effect = RE::TESForm::LookupByEditorID("NSV_TL_CC_SummonWeapon")->As<RE::EffectSetting>();
			}
		}

		if (NSV_Magic_Trap && a_spell->HasKeyword(NSV_Magic_Trap))
		{
			a_effect = RE::TESForm::LookupByEditorID("NSV_TL_CC_TrapBase")->As<RE::EffectSetting>();
		}

		if (a_effect)
		{
			detected_key = true;
		}
		return {detected_key, a_effect};
	}
}




// bool OnMeleeHitHook::AddCondition(BGSKeyword* a_keyword)
// {
// 	if (!GetKeywordIndex(a_keyword)) {
// 		std::vector<BGSKeyword*> copiedData{ keywords, keywords + numKeywords };
// 		copiedData.push_back(a_keyword);
// 		CopyKeywords(copiedData);
// 		return true;
// 	}
// 	return false;
// }

// void OnMeleeHitHook::CopyEffect(const std::vector<RE::Effect*>& a_copiedData)
// {
// 	const auto oldData = keywords;

// 	const auto newSize = a_copiedData.size();
// 	const auto newData = calloc<RE::Effect*>(newSize);
// 	std::ranges::copy(a_copiedData, newData);

// 	numKeywords = static_cast<std::uint32_t>(newSize);
// 	keywords = newData;

// 	free(oldData);
// }

// auto spelldata = a_actor->As<RE::TESNPC>()->GetSpellList();
// auto numSpells = spelldata->numSpells;
// auto spells = spelldata->spells;
// std::vector<RE::SpellItem*> spellList{ spells, spells + numSpells };

// a_value = a_ini.GetAllValues(a_section, a_key, a_value);
// bool commented = false;
// for (auto it = a_value.begin(); it != a_value.end(); ++it) {
// 	if (it) {
// 		if (!commented) {
// 			commented = true;
// 			a_ini.SetValue(a_section, a_key, it, a_comment);
// 		} else {
// 			a_ini.SetValue(a_section, a_key, it);
// 		}
// 	}
// }

// RE::BSTArray<RE::TESForm *>::iterator position = std::find(a_active_list_1->forms.begin(), a_active_list_1->forms.end(), limbospell);
// if (position == a_active_list_1->forms.end())
// {
// 	a_active_list_1->AddForm(limbospell);
// }

// if (caster->GetCastingSource() == RE::MagicSystem::CastingSource::kLeftHand)
// {
// 	a_actor->NotifyAnimationGraph("BeginCastLeft");
// 	// a_actor->NotifyAnimationGraph("MLh_SpellFire_Event");
// }
// else
// {
// 	a_actor->NotifyAnimationGraph("BeginCastRight");
// 	// a_actor->NotifyAnimationGraph("MRh_SpellFire_Event");
// }

// if (a_actor->GetActorRuntimeData().currentProcess)
// {
// 	a_actor->GetActorRuntimeData().currentProcess->SetHeadtrackTarget(refr, refr->data.location);
// }
// caster->SpellCast(true, 1, a_spell);