#include "DovahAI.h"

namespace DovahAI_Space{

    void DovahAI::DeferredKill(RE::Actor *a_actor, bool start)
    {
        if (auto AIprocess = a_actor->GetActorRuntimeData().currentProcess)
        {
            if (start){
                AIprocess->middleHigh->inDeferredKill = true;
            }else{
                AIprocess->middleHigh->inDeferredKill = false;
            }
        }
    }

    void DovahAI::DeathRadollPerch(RE::Actor *a_actor)
    {
        DeferredKill(a_actor, true);
        a_actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth, -9999.0f);
        DeferredKill(a_actor, false);
        a_actor->NotifyAnimationGraph("Ragdoll");
    }

    bool DovahAI::GetBoolVariable(RE::Actor *a_actor, std::string a_string)
    {
        auto result = false;
        a_actor->GetGraphVariableBool(a_string, result);
        return result;
    }

    int DovahAI::GetIntVariable(RE::Actor *a_actor, std::string a_string)
    {
        auto result = 0;
        a_actor->GetGraphVariableInt(a_string, result);
        return result;
    }

    float DovahAI::GetFloatVariable(RE::Actor *a_actor, std::string a_string)
    {
        auto result = 0.0f;
        a_actor->GetGraphVariableFloat(a_string, result);
        return result;
    }

    bool DovahAI::IsMQ206CutsceneDragons(RE::Actor *a_actor)
    {
        auto result = false;
        if (auto AB = a_actor->GetActorBase())
        {
            if (AB == RE::TESForm::LookupByEditorID<RE::TESNPC>("MQ206AncientAlduin"))
            {
                result = true;
            }
        }
        return result;
    }

    bool DovahAI::IsUnSafeDragon(RE::Actor *a_actor)
    {
        auto result = false;
        if (auto AB = a_actor->GetActorBase())
        {
            if (AB == RE::TESForm::LookupByEditorID<RE::TESNPC>("MQ206AncientAlduin"))
            {
                result = true;
            }
        }
        return result;
    }

    void DovahAI::wait(int a_duration)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(a_duration));
    }

    void DovahAI::DeathRadollCrashLand(RE::Actor *a_actor)
    {
        auto first_position = a_actor->GetPosition();
        DeferredKill(a_actor, true);
        std::jthread waitThread(wait, 100);
        auto second_position = a_actor->GetPosition();
        DeferredKill(a_actor);
        DeferredKill(a_actor, true);
        a_actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth, -9999.0f);
        DeferredKill(a_actor);
        a_actor->NotifyAnimationGraph("Ragdoll");
        GFunc_Space::GFunc::GetSingleton()->ApplyHavokImpulse(a_actor, second_position.x - first_position.x, second_position.y - first_position.y, second_position.z - first_position.z, 50000.0f);
    }

    float DovahAI::PercentageHealthAction(RE::Actor *a_actor)
    {
        float result = 1.0f;

        if (GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging"))
        {
            result = 1.25f;
        }else{

            auto cur_health = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
            auto perm_health = a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
            if (perm_health != 0 && cur_health != 0)
            {
                if (auto percentHealth = (cur_health / perm_health)){
                    if (percentHealth > 0.87 && percentHealth <= 1.0)
                    {
                        result = 1.25f;
                    }
                    else if (percentHealth > 0.74 && percentHealth <= 0.87)
                    {
                        result = 1.2f;
                    }
                    else if (percentHealth > 0.61 && percentHealth <= 0.74)
                    {
                        result = 1.15f;
                    }
                    else if (percentHealth > 0.48 && percentHealth <= 0.61)
                    {
                        result = 1.1f;
                    }
                    else if (percentHealth > 0.35 && percentHealth <= 0.48)
                    {
                        result = 1.05f;
                    }else{
                        result = 1.0f;
                    }
                }
            }
        }

        return result;
    }

    void DovahAI::SetLandingMarker(RE::Actor *a_actor)
    {
        if (auto combat_target_handle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); combat_target_handle)
        {
            GFunc_Space::GFunc::SetForcedLandingMarker(a_actor, combat_target_handle.get());
        }
    }

    void DovahAI::DeathRadollFly(RE::Actor *a_actor)
    {
        DeferredKill(a_actor, true);
        auto first_position = a_actor->GetPosition();
        auto first_time = std::chrono::steady_clock::now();
        std::jthread waitThread(wait, 400);
        auto second_position = a_actor->GetPosition();
        auto second_time = std::chrono::steady_clock::now();
        auto time_secs = (std::chrono::duration_cast<std::chrono::seconds>(second_time - first_time).count()) * 10.0;
        auto total_val = (abs(second_position.x - first_position.x) + abs(second_position.y - first_position.y) + abs(second_position.z - first_position.z))/time_secs;
        if (total_val > 80000.0)
        {
            total_val = 80000.0;
        }
        a_actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth, -9999.0f);
        DeferredKill(a_actor);
        a_actor->NotifyAnimationGraph("Ragdoll");
        GFunc_Space::GFunc::GetSingleton()->ApplyHavokImpulse(a_actor, second_position.x - first_position.x, second_position.y - first_position.y, second_position.z - first_position.z, total_val);
    }

    void DovahAI::ResetAI(RE::Actor *a_actor)
    {
        auto data = RE::TESDataHandler::GetSingleton();
        a_actor->AddSpell(data->LookupForm<RE::SpellItem>(0xA342E7, "LeoneDragonProject.esp"));
        std::jthread waitThread(wait, 350);
        a_actor->RemoveSpell(data->LookupForm<RE::SpellItem>(0xA342E7, "LeoneDragonProject.esp"));
    }

    void DovahAI::SendRandomAnimationEvent(RE::Actor *a_actor, int I, std::string AnimEvent01, std::string AnimEvent02, std::string AnimEvent03, std::string AnimEvent04)
    {
        switch (I)
        {
        case 0:
            a_actor->NotifyAnimationGraph(AnimEvent01);
            break;

        case 1:
            a_actor->NotifyAnimationGraph(AnimEvent02);
            break;

        case 2:
            a_actor->NotifyAnimationGraph(AnimEvent03);
            break;

        case 3:
            a_actor->NotifyAnimationGraph(AnimEvent04);
            break;

        default:
            break;
        }
    }

    void DovahAI::OnInitHPCalc(RE::Actor *a_actor)
    {
        auto perm_health = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
        a_actor->SetGraphVariableInt("iLDP_Pre_HP", static_cast<int>(perm_health));
        a_actor->SetGraphVariableInt("iLDP_Front_HP", static_cast<int>(perm_health * 0.4));
        a_actor->SetGraphVariableInt("iLDP_Back_HP", static_cast<int>(perm_health * 0.25));
        a_actor->SetGraphVariableInt("iLDP_Left_HP", static_cast<int>(perm_health * 0.3));
        a_actor->SetGraphVariableInt("iLDP_Right_HP", static_cast<int>(perm_health * 0.3));
    }

    float DovahAI::GetActorValuePercent(RE::Actor *a_actor, RE::ActorValue a_value)
    {
        auto result = 0.0f;
        auto cur_value = a_actor->AsActorValueOwner()->GetActorValue(a_value);
        auto perm_value = a_actor->AsActorValueOwner()->GetPermanentActorValue(a_value);

        if(perm_value != 0.0f){
            result = cur_value/perm_value;
        }
        return result;
    }

    void DovahAI::Enrage(RE::Actor *a_actor, int count)
    {
        if (!GetBoolVariable(a_actor, "bLDP_IsEnraging"))
        {
            auto val = GetIntVariable(a_actor, "iLDP_Enrage_Count");
            a_actor->SetGraphVariableInt("iLDP_Enrage_Count", val += 1);
        }
    }

    void DovahAI::DragonType(RE::Actor *a_actor)
    {
        if (a_actor->HasKeywordString("AlduinUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 1); // "Alduin";
        }
        else if (a_actor->HasKeywordString("FireUnitedKey") && a_actor->HasKeywordString("FrostUnitedKey") && a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 2); // "AllD";
        }
        else if (a_actor->HasKeywordString("FireUnitedKey") && a_actor->HasKeywordString("FrostUnitedKey") && !a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 3); // "FireFrostD";
        }
        else if (a_actor->HasKeywordString("FireUnitedKey") && !a_actor->HasKeywordString("FrostUnitedKey") && a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 4); // "FireShockD";
        }
        else if (!a_actor->HasKeywordString("FireUnitedKey") && a_actor->HasKeywordString("FrostUnitedKey") && a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 5); // "FrostShockD";
        }
        else if (a_actor->HasKeywordString("FireUnitedKey") && !a_actor->HasKeywordString("FrostUnitedKey") && !a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 6); // "Fire";
        }
        else if (!a_actor->HasKeywordString("FireUnitedKey") && a_actor->HasKeywordString("FrostUnitedKey") && !a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 7); // "Frost";
        }
        else if (!a_actor->HasKeywordString("FireUnitedKey") && !a_actor->HasKeywordString("FrostUnitedKey") && a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_Dragon_Type", 8); // "Shock";
        }
    }

    void DovahAI::Enrage_state(RE::Actor *a_actor)
    {
        a_actor->SetGraphVariableBool("bLDP_IsEnraging", true);
        a_actor->SetGraphVariableInt("iLDP_Enrage_Count", 0);
        a_actor->NotifyAnimationGraph("Enrage");
        a_actor->SetGraphVariableBool("bNoStagger", true);
        auto H = RE::TESDataHandler::GetSingleton();
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        caster->CastSpellImmediate(H->LookupForm<RE::SpellItem>(0xA342E7, "LeoneDragonProject.esp"), true, a_actor, 1, false, 0.0, a_actor); // EnrageSpell

        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 2000ms, "EnrageState_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::Enrage_start(RE::Actor *a_actor)
    {
        Shout(a_actor);
        a_actor->SetGraphVariableBool("IsEnraging", true);
        auto yt = GetSingleton()->Get_AttackList(a_actor);
        std::get<0>(yt) = 4;
        std::get<1>(yt)[0] = 0;
        std::get<2>(yt)[2] = 0;
        std::get<2>(yt)[4] = 0;
        std::get<2>(yt)[5] = 6;
        std::get<3>(yt)[0] = 0;
        std::get<3>(yt)[2] = 1;
        std::get<3>(yt)[4] = 18;

        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 60000ms, "EnrageStart_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::Set_Box(RE::Actor *actor)
    {
        auto H = RE::TESDataHandler::GetSingleton();
        auto Box = actor->PlaceObjectAtMe(H->LookupForm<RE::TESObjectACTI>(0xA342E7, "LeoneDragonProject.esp"), false).get(); //fxactivator
        if (Box)
        {
            GetSingleton()->scan_activeBoxes(actor, Box, true);
        }
    }

    void DovahAI::scan_activeBoxes([[maybe_unused]] RE::Actor *a_actor, [[maybe_unused]] RE::TESObjectREFR *a_box, bool insert, bool clear, bool clear_all)
    {
		uniqueLocker lock(mtx_Boxes);
		if (insert){
			auto itt = _Boxes.find(a_actor);
			if (itt == _Boxes.end()){
                std::vector<RE::TESObjectREFR *> Hen;
                _Boxes.insert({a_actor, Hen});
			}
		}
		
		for (auto it = _Boxes.begin(); it != _Boxes.end(); ++it){
			if (insert){
				if (it->first == a_actor){
					if (!it->second.empty()){
						for (auto box : it->second){
							if (box){
								box = nullptr;
							}
						}
						it->second.clear();
					}
					it->second.push_back(a_box);
					break;
				}
			}
			if (clear){
				if (it->first == a_actor){
					if (!it->second.empty()){
						for (auto box : it->second){
							if (box){
								box = nullptr;
							}
						}
						it->second.clear();
					}
					_Boxes.erase(it);
					break;
				}
			}
			if (clear_all){
				if (it->first){
					if (!it->second.empty()){
						for (auto box : it->second){
							if (box){
								box = nullptr;
							}
						}
						it->second.clear();
					}
					_Boxes.erase(it);
				}
			}
			continue;
		}
	}

    RE::TESObjectREFR* DovahAI::Get_Box(RE::Actor *a_actor)
    {
        uniqueLocker lock(mtx_Boxes);
        RE::TESObjectREFR* result = nullptr;
        for (auto it = _Boxes.begin(); it != _Boxes.end(); ++it){
            if (it->first == a_actor) {
                if (!it->second.empty()) {
                    for (auto box : it->second) {
                        if (box)
                        {
                            result = box;
                            break;
                        }
                    }
                }
            }
            continue;
        }
        return result;
    }

    std::tuple<int, std::vector<int>, std::vector<int>, std::vector<int>> DovahAI::Get_AttackList(RE::Actor *a_actor)
    {
        uniqueLocker lock(mtx_attackList);
        std::tuple<int, std::vector<int>, std::vector<int>, std::vector<int>>  result ;
        for (auto it = _attackList.begin(); it != _attackList.end(); ++it)
        {
            if (it->first == a_actor)
            {
                result = it->second;
                break;
            }
            continue;
        }
        return result;
    }

    void DovahAI::Shout(RE::Actor *a_actor)
    {
        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 66ms, "Shout_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::Shout1(RE::Actor *a_actor)
    {
        if (auto box = GetSingleton()->Get_Box(a_actor))
        {
            auto H = RE::TESDataHandler::GetSingleton();
            box->MoveToNode(a_actor, "NPC Head MagicNode [Hmag]");
            const auto caster = box->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
            caster->CastSpellImmediate(H->LookupForm<RE::MagicItem>(0xA342E7, "LeoneDragonProject.esp"), true, nullptr, 1, false, 0.0, a_actor); // EnrageSpell
        }else{
            GetSingleton()->Set_Box(a_actor);
            if (auto box = GetSingleton()->Get_Box(a_actor))
            {
                auto H = RE::TESDataHandler::GetSingleton();
                box->MoveToNode(a_actor, "NPC Head MagicNode [Hmag]");
                const auto caster = box->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
                caster->CastSpellImmediate(H->LookupForm<RE::MagicItem>(0xA342E7, "LeoneDragonProject.esp"), true, nullptr, 1, false, 0.0, a_actor); // EnrageSpell
            }
        }
    }

    void DovahAI::CastAreaEffect(RE::Actor *a_actor, RE::MagicItem* a_spell, RE::BSFixedString a_node)
    {
        if (auto box = GetSingleton()->Get_Box(a_actor))
        {
            box->MoveToNode(a_actor, a_node);
            const auto caster = box->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
            caster->CastSpellImmediate(a_spell, true, nullptr, 1, false, 0.0, a_actor);
        }else{
            GetSingleton()->Set_Box(a_actor);
            if (auto box = GetSingleton()->Get_Box(a_actor))
            {
                box->MoveToNode(a_actor, a_node);
                const auto caster = box->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
                caster->CastSpellImmediate(a_spell, true, nullptr, 1, false, 0.0, a_actor);
            }
        }
    }

    void DovahAI::BleedOut_state(RE::Actor *a_actor)
    {
        if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
        {
            a_actor->SetGraphVariableBool("bLDP_BleedOut_State", true);
            a_actor->NotifyAnimationGraph("BleedoutStart");
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 5000ms, "BleedOutState_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
    }

    void DovahAI::BleedOut_state1(RE::Actor *a_actor)
    {
        a_actor->NotifyAnimationGraph("BleedOutStop");
        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 500ms, "BleedOutState1_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::Others(RE::Actor *a_actor)
    {
        if (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kSpeedMult) != 100.0f){
            a_actor->AsActorValueOwner()->SetActorValue(RE::ActorValue::kSpeedMult, 100.0f);
            a_actor->SetGraphVariableFloat("playbackSpeed", GetSingleton()->PercentageHealthAction(a_actor));
        }
    }
     void DovahAI::TalonSmashScene(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (ct->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
            {
                if (GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
                {
                    GetSingleton()->SetLandingMarker(a_actor);
                    a_actor->SetGraphVariableBool("bLDP_Talon_Faction", true);
                    TalonSmash(a_actor);

                    while (GetBoolVariable(a_actor, "bLDP_PreventFlyingTalonSmash"))
                    {
                        std::jthread waitThread(wait, 500);
                    }
                    a_actor->SetGraphVariableBool("bLDP_Talon_Faction", false);
                    GFunc_Space::GFunc::SetForcedLandingMarker(a_actor, nullptr);

                    while (ct->AsActorState()->GetFlyState() != RE::FLY_STATE::kCruising)
                    {
                        std::jthread waitThread1(wait, 500);
                    }
                    GFunc_Space::GFunc::Reset_iFrames(a_actor);
                }
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "TSS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);

            }else{
                GFunc_Space::GFunc::Reset_iFrames(a_actor);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "TSS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
        }
    }

    void DovahAI::TalonSmash(RE::Actor *a_actor)
    {
        a_actor->SetGraphVariableBool("bLDP_PreventFlyingTalonSmash", true);

        while (GetBoolVariable(a_actor, "bLDP_PreventFlyingTalonSmash") 
        && (a_actor->AsActorState()->GetFlyState() > RE::FLY_STATE::kNone && a_actor->AsActorState()->GetFlyState() <= RE::FLY_STATE::kPerching) 
        && !GetBoolVariable(a_actor, "Injured"))
        {
            std::jthread waitThread(wait, 100);
        }
        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 800ms, "TalonSmash_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::TalonSmash1(RE::Actor *a_actor)
    {
        a_actor->NotifyAnimationGraph("to_Flight_Kill_Grab_Action");

        auto H = RE::TESDataHandler::GetSingleton();
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        caster->CastSpellImmediate(H->LookupForm<RE::SpellItem>(0xA342E7, "LeoneDragonProject.esp"), true, a_actor, 1, false, 0.0, a_actor); // talonAOE
        RE::NiPoint3 Tx;
        Tx.x = -1.0f;
        GFunc_Space::GFunc::PlayImpactEffect(a_actor, H->LookupForm<RE::BGSImpactDataSet>(0xA342E7, "LeoneDragonProject.esp"), "", Tx, 512.0f, false, false);

        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 900ms, "TalonSmash1_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::TalonSmash2(RE::Actor *a_actor)
    {
        if (GetBoolVariable(a_actor, "Injured"))
        {
            a_actor->SetGraphVariableBool("bLDP_PreventFlyingTalonSmash", false);
            a_actor->SetGraphVariableBool("Injured", false);
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 4500ms, "TalonSmash2_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
        else
        {
            a_actor->SetGraphVariableBool("bLDP_PreventFlyingTalonSmash", false);
        }
    }

    void DovahAI::DamageTarget(RE::Actor *a_actor, RE::Actor *enemy)
    {
        if (const auto base = a_actor->GetActorBase())
        {
            auto level = base->GetLevel();
            auto damage = 25.0f;

            if (level > 10 && level <= 20)
            {
                damage = 50.0f;
            }
            else if (level > 20 && level <= 30)
            {
                damage = 100.0f;
            }
            else if (level > 30 && level <= 40)
            {
                damage = 150.0f;
            }
            else if (level > 40 && level <= 50)
            {
                damage = 200.0f;
            }
            else if (level > 50 && level <= 62)
            {
                damage = 250.0f;
            }
            else if (level > 62)
            {
                damage = 300.0f;
            }
            enemy->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth, -(damage));
        }
    }

    void DovahAI::SetValuesDragon(RE::Actor *a_actor)
    {
        auto LvRank = GetIntVariable(a_actor, "iLDP_Lvl_Rank");
        a_actor->AsActorValueOwner()->SetActorValue(RE::ActorValue::kDamageResist, ((LvRank * 45.0) / (1.0 + abs(3.0 - log(RE::PlayerCharacter::GetSingleton()->GetLevel())) * pow(2.718, (-1.0 * LvRank)))));
        a_actor->SetGraphVariableBool("bLDP_SetValues_Dragon", true);
    }

    void DovahAI::CalcLevelRank(RE::Actor *a_actor)
    {
        if (const auto base = a_actor->GetActorBase())
        {
            auto level = base->GetLevel();
            int rank = 0;
            while (rank * 10 < level && rank < 7)
            {
                rank += 1;
            }
            a_actor->SetGraphVariableInt("iLDP_Lvl_Rank", rank);
        }
    }

    void DovahAI::CreateAttackList(RE::Actor *a_actor)
    {
        uniqueLocker lock(mtx_attackList);

        int NoDistRate = 8;
        std::vector<int> NoDistList = {10 - GetIntVariable(a_actor, "iLDP_Lvl_Rank"), 0, 0, 0, 0};
        std::vector<int> MeleeList = {14 - GetIntVariable(a_actor, "iLDP_Lvl_Rank"), 14, GetIntVariable(a_actor, "iLDP_Lvl_Rank"), 0, 6, 6};
        std::vector<int> RangedList = {10 - GetIntVariable(a_actor, "iLDP_Lvl_Rank"), 9, GetIntVariable(a_actor, "iLDP_Lvl_Rank"), GetIntVariable(a_actor, "iLDP_Lvl_Rank"), 12};

        switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
        {
        case 1:
        case 2:
            NoDistList[3] = 8;
            NoDistList[4] = 12;
            break;

        case 5:
        case 7:
            NoDistList[1] = 8;
            break;

        default:
            NoDistList[2] = 8;
            break;
        }
        if (GetIntVariable(a_actor, "iLDP_Lvl_Rank") >= 4)
        {
            MeleeList[3] = 6;
        }

        auto itt = _attackList.find(a_actor);
        if (itt == _attackList.end())
        {
            std::tuple<int, std::vector<int>, std::vector<int>, std::vector<int>> Hen;
            std::get<0>(Hen) = NoDistRate;
            std::get<1>(Hen) = NoDistList;
            std::get<2>(Hen) = MeleeList;
            std::get<3>(Hen) = RangedList;
            _attackList.insert({a_actor, Hen});
        }else{
            std::get<0>(itt->second) = NoDistRate;
            std::get<1>(itt->second) = NoDistList;
            std::get<2>(itt->second) = MeleeList;
            std::get<3>(itt->second) = RangedList;
        }
    }

    void DovahAI::RemoveAttackList(RE::Actor *a_actor)
    {
        uniqueLocker lock(mtx_attackList);
        for (auto it = _attackList.begin(); it != _attackList.end(); ++it)
        {
            if (it->first == a_actor)
            {
                _attackList.erase(it);

                break;
            }
            continue;
        }
    }

    void DovahAI::ControlDistanceRiddenAI(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();
            int i = 0;
            while (i <= HoverWaitTime(a_actor) && GFunc_Space::IsAllowedToFly(a_actor, 1.0f) && ct && !ct->IsDead() && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 2.5f)
            {
                i += 1;
                std::jthread waitThread(wait, 1000);
            }
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1000ms, "CDR_AI_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
    }

    void DovahAI::ControlDistanceRiddenAI1(RE::Actor *a_actor)
    {
        while (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kHovering)
        {
            GetSingleton()->ResetAI(a_actor);
        }

        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "CDR_AI2_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::GroundCombatAI(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 75.0f)
            {
                if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) <= 45.0f) && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 1.13f) //front
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_front_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
                else if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) >= 135.0f) && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 1.13f)//back
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_back_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
                else if (GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) < -45.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 1.25f)//left
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_left_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
                else if (GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) < 45.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 1.25f)//right
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_right_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
            }
        }
    }

    void DovahAI::GroundCombatAI_front(RE::Actor *a_actor)
    {
        switch (GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 3))
        {
        case 0:
            a_actor->NotifyAnimationGraph("attackStartTail");
            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
            {
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_front1_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
            break;

        case 1:
            SendRandomAnimationEvent(a_actor, GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 1), "attackStartWingLeft", "attackStartWingRight", "None", "None");
            break;

        case 2:
            SendRandomAnimationEvent(a_actor, GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 1), "attackStartStampLeft", "attackStartStampRight", "None", "None");
            break;

        case 3:
            if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 2)
            {
                if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 35.0f)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) 
                    && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 0:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
            }
            break;

        default:
            break;
        }
    }

    void DovahAI::GroundCombatAI_back(RE::Actor *a_actor)
    {
        switch (GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 2))
        {
        case 0:
            a_actor->NotifyAnimationGraph("attackStartTail");
            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
            {
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_back1_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
            break;

        case 1:
            a_actor->NotifyAnimationGraph("attackStartTailWhip");
            break;

        case 2:
            if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 2)
            {
                if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 35.0f)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 0:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
            }
            break;

        default:
            break;
        }
    }

    void DovahAI::GroundCombatAI_left(RE::Actor *a_actor)
    {
        switch (GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 2))
        {
        case 0:
            SendRandomAnimationEvent(a_actor, GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 1), "attackStartWingLeft", "attackStartTailLeft", "None", "None");
            break;

        case 1:
            a_actor->NotifyAnimationGraph("attackStartStampLeft");
            break;

        case 2:
            if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 2)
            {
                if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 35.0f)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 0:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
            }
            break;

        default:
            break;
        }
    }

    void DovahAI::GroundCombatAI_right(RE::Actor *a_actor)
    {
        switch (GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 2))
        {
        case 0:
            SendRandomAnimationEvent(a_actor, GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 1), "attackStartWingRight", "attackStartTailRight", "None", "None");
            break;

        case 1:
            a_actor->NotifyAnimationGraph("attackStartStampRight");
            break;

        case 2:
            if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 2)
            {
                if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 35.0f)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 0:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
            }
            break;

        default:
            break;
        }
    }

    void DovahAI::TakeoffCombatAI(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (GetIntVariable(a_actor, "iLDP_TakeOff_Faction") == 1 && !GetBoolVariable(a_actor, "bLDP_PreventFlyingTalonSmash"))
            {
                if (a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 1.75f)
                {
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
                    {
                        if (GetActorValuePercent(a_actor, RE::ActorValue::kHealth) > 0.35f)
                        {
                            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= GetFloatVariable(a_actor, "fLDP_HoverAttackChance"))
                            {
                                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 850ms, "TakeOffCombat_AI_Update");
                                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                            }
                            else if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 1)
                            {
                                //ground attack vertical scene
                            }
                        }
                    }
                }
            }
        }
    }

    void DovahAI::PassByCombatAI(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 2.5f)
            {
                if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 83.0f && GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
                {
                    if (ct->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                    {
                        if (GetActorValuePercent(a_actor, RE::ActorValue::kHealth) > 0.35f)
                        {
                            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= GetFloatVariable(a_actor, "fLDP_HoverAttackChance") && !GetBoolVariable(a_actor, "IsShouting") && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 2.0f)
                            {
                                a_actor->SetGraphVariableBool("bVoiceReady", false);
                                a_actor->SetGraphVariableBool("bVoiceReady", true);
                                ToHoverAttackScene(a_actor);
                            }
                            else
                            {
                                ToGroundAttackScene(a_actor);
                            }
                        }
                        else
                        {
                            GroundAttackScene(a_actor);
                        }

                    }else{
                        if (GetActorValuePercent(a_actor, RE::ActorValue::kHealth) > 0.35f)
                        {
                            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 35.0f)
                            {
                                SendRandomAnimationEvent(a_actor, GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 1), "to_Flight_Kill_Grab_Action", "to_Flight_Kill_Grab_Action_Failed", "None", "None");
                                GetSingleton()->ResetAI(a_actor);
                                GetSingleton()->ResetAI(a_actor);
                                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "PBC_AI_Update");
                                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                            }
                            else if (!GetBoolVariable(a_actor, "IsShouting") && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 2.0f)
                            {
                                a_actor->SetGraphVariableBool("bVoiceReady", false);
                                a_actor->SetGraphVariableBool("bVoiceReady", true);
                                DovahAI_Space::DovahAI::ToHoverAttackScene(a_actor);
                            }
                        }
                    }
                    
                }
            }
        }
    }

    void DovahAI::ToHoverAttackScene(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            a_actor->NotifyAnimationGraph("FlyStartHover");
            auto ct = targethandle.get();
            int i = 0;
            while (i <= HoverWaitTime(a_actor) && GFunc_Space::IsAllowedToFly(a_actor, 1.0f) && ct && !ct->IsDead() && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 150.0f * 2.5f)
            {
                i += 1;
                std::jthread waitThread(wait, 1000);
            }
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1000ms, "HAS_AI_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
    }

    void DovahAI::ToHoverAttackScene1(RE::Actor *a_actor)
    {
        a_actor->NotifyAnimationGraph("HoverStopVertical");
        if (GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
        {
            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 83.0f)
            {
                DovahAI_Space::DovahAI::ToGroundAttackScene(a_actor);
            }else{
                GetSingleton()->ResetAI(a_actor);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "HAS_AI2_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
        }else{
            GetSingleton()->ResetAI(a_actor);
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "HAS_AI2_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
    }

    void DovahAI::ToGroundAttackScene(RE::Actor *a_actor)
    {
        if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 1)
        {
            GroundAttackScene(a_actor);
        }
        else if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= GetFloatVariable(a_actor, "fLDP_HoverAttackChance"))
        {
            GFunc_Space::GFunc::Set_iFrames(a_actor);
            TalonSmashScene(a_actor);
        }
    }

    void DovahAI::MoveControllShout(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 15.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 1500.0f && abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) < 80.0f))
            {
                switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                {
                case 0:
                case 8:
                    //do nothing
                    break;
                
                default:
                    a_actor->NotifyAnimationGraph("SwingShout");
                    break;
                }
            }
            a_actor->SetGraphVariableFloat("BSLookAtModifier_m_onGain_Shouting", 0.03);
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1500ms, "MovShout_AI_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
        
    }

    void DovahAI::GroundAttackScene(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (ct->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
            {
                if (GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
                {
                    a_actor->SetGraphVariableInt("iLDP_Landing_Faction", 1); //default landing faction
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", true);
                    switch (GetIntVariable(a_actor, "iLDP_PreferCombatStyle"))
                    {
                    case 0:
                    case 1:
                        if(a_actor){
                            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 12000ms, "GAS_Flight_Update");
                            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                        }
                        break;

                    case 2:
                        if(a_actor){
                            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 128000ms, "GAS_Flight_Update");
                            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                        }
                        break;

                    default:
                        break;
                    }
                    while (a_actor->AsActorState()->GetFlyState() > RE::FLY_STATE::kNone && GetBoolVariable(a_actor, "bLDP_DragonFlightlessCombat"))
                    {
                        std::jthread waitThread(wait, 1000);
                    }
                    a_actor->SetGraphVariableInt("iLDP_Landing_Faction", 0);
                }
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "GAS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);

            }else{
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "GAS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
        }
    }

    int DovahAI::HoverWaitTime(RE::Actor *a_actor)
    {
        int result = 0;

        switch (GetIntVariable(a_actor, "iLDP_PreferCombatStyle"))
        {
        case 0:
            result = 4;
            break;

        case 1:
            result = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(6, 8);
            break;

        case 2:
            result = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(4, 6);
            break;

        default:
            break;
        }
        
        return result;
    }

    void DovahAI::Physical_Impact(RE::Actor *a_actor, std::string a_spell, float p_force)
    {
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>(a_spell), true, a_actor, 1, false, 0.0, a_actor);
        if (const auto combatGroup = a_actor->GetCombatGroup())
        {
            for (auto &targetData : combatGroup->targets)
            {
                if (auto target = targetData.targetHandle.get())
                {
                    RE::Actor *Enemy = target.get();
                    if (a_actor->GetPosition().GetDistance(Enemy->GetPosition()) <= 150.0f)
                    {
                        DovahAI_Space::DovahAI::DamageTarget(a_actor, Enemy);
                        if (!(Enemy->IsBlocking() && GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(Enemy, a_actor, nullptr) <= 45.0f))
                        {
                            GFunc_Space::GFunc::PushActorAway(a_actor, Enemy, p_force);
                        }
                    }
                }
            }
        }
    }

    void DovahAI::Random_TakeOffandDeath_Anims(RE::Actor *a_actor)
    {
        if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
        {
            a_actor->SetGraphVariableInt("iLDP_TakeOff_Faction", 1); // TakeOffVertical
        }

        switch (GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(0, 5))
        {
        case 0:
            a_actor->SetGraphVariableInt("iLDP_DeathAnim_Faction", 0); // DeathDefault
            break;

        case 1:
            a_actor->SetGraphVariableInt("iLDP_DeathAnim_Faction", 1); // DeathRagdoll
            break;

        case 2:
            a_actor->SetGraphVariableInt("iLDP_DeathAnim_Faction", 2); // DeathAgony
            break;

        case 3:
            a_actor->SetGraphVariableInt("iLDP_DeathAnim_Faction", 3); // DeathInjured
            break;

        case 4:
            a_actor->SetGraphVariableInt("iLDP_DeathAnim_Faction", 4); // DeathBleedout
            break;

        case 5:
            a_actor->SetGraphVariableInt("iLDP_DeathAnim_Faction", 5); // DeathTimeTravel
            break;

        default:
            break;
        }
    }

    void DovahAI::DeathWaitRagdoll(RE::Actor *a_actor)
    {
        switch (GetIntVariable(a_actor, "iLDP_DeathAnim_Faction"))
        {
        case 0:
            if(a_actor){
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1600ms, "DeathDefault_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
            // DeathDefault
            break;

        case 1:
            // if(a_actor){
            //     std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            //     GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1600ms, "DeathRagdoll_Update");
            //     GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            // } 
            // DeathRagdoll
            break;

        case 2:
            if(a_actor){
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 25000ms, "DeathAgony_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            } 
            // DeathAgony
            break;

        case 3:
            if(a_actor){
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 2600ms, "DeathInjured_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            } 
            // DeathInjured
            break;

        case 4:
            if(a_actor){
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1200ms, "DeathBleedout_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            } 
            // DeathBleedout
            break;

        case 5:
            if(a_actor){
                auto H = RE::TESDataHandler::GetSingleton();
                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // ks_NPCDragonKillMove
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 6200ms, "DeathTimeTravel_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            } 
            // DeathTimeTravel
            break;

        default:
            break;
        }
    }

    void DovahAI::GetEquippedShout(RE::Actor *actor, bool SpellFire)
    {
        auto limboshout = actor->GetActorRuntimeData().selectedPower;

        if (limboshout && limboshout->Is(RE::FormType::Shout))
        {
            auto data = RE::TESDataHandler::GetSingleton();

            std::string Lsht = (clib_util::editorID::get_editorID(limboshout));

            switch (hash(Lsht.c_str(), Lsht.size()))
            {
            case "ks_DragonFlameWaveShoutALDUIN"_h:
                if (SpellFire)
                {
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A0, "Dragons shout with voice - KS2 Patch.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10F563, "Skyrim.esm")));
                    }
                }
                else
                {
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8C, "Dragons shout with voice - KS2 Patch.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10F562, "Skyrim.esm")));
                    }
                }
                break;

            case "005SummonShouts"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81C, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005StormShoutsDECIEVE"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x9CC, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005SahloknirStormShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82E, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005StormShoutsBLOOD"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x949, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x948, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005UniqueBreathShouts"_h:
            case "005GoldenFireballConcSpell06"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F3, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F2, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005UniqueBreathShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F9, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F8, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005UniqueBreathShoutsODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x903, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x902, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005UniqueBreathShoutsPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FF, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FE, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005VolleyBreathShouts"_h:
            case "005DragonIceStormConcSpell02"_h:
            case "005DragonIceStormConcSpell03"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F1, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F0, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;
            case "005VolleyBreathShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F7, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F6, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005VolleyBreathShoutsODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x901, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x900, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005VolleyBreathShoutsPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FD, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FC, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "PaarthurnaxElementalFury"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E3, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E2, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "PaarthurnaxWhirlwindTempest"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E1, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E0, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonMoonBlast"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D1, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D0, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonMoonBlastALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C3, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005StormShoutsFROST"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CF, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CE, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;
            case "zz005AlduinFroststorms"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C2, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C1, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "zz05AlduinFirestorms"_h:
            case "zz05AlduinFirestormsSERIO"_h:
            case "dunCGDragonStormCallShout"_h:
            case "MQ206AlduinFirestormShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A8, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A7, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005StormShoutsFIRE"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x859, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x858, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "zz005DevourSoul_Alduin"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BE, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BD, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005StormShoutsSHOCK"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x832, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x831, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "zz005AlduinLightningstorms"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BC, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BB, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonCycloneShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x837, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x836, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonCycloneShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B6, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonCycloneShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x890, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88F, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonCycloneShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DF, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DE, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonDrainVitalityShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x820, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81F, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonDrainVitalityShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B3, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonDrainVitalityShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88E, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "KS_DLC2BendWillShout_Alduin"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA81, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AD, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x828, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x827, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AE, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AD, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x888, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x887, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DB, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DA, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82A, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x829, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AC, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AB, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x886, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x885, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D9, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D8, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonBecomeEtherealShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x834, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x833, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonBecomeEtherealShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A6, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonBecomeEtherealShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x880, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87F, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonBecomeEtherealShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_UnrelentingForceShout"_h:
            case "ks_DragonUnrelentingForceAreaShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E53, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E50, "Skyrim.esm")));
                }
                break;

            case "Serio_EDR_UnrelentingForceShoutALDUIN"_h:
            case "MQ101DragonUnrelentingForceShout"_h:
            case "ks_DragonUnrelentingForceAreaShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF06, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF05, "Skyrim.esm")));
                }
                break;

            case "Serio_EDR_UnrelentingForceShoutODAH"_h:
            case "ks_DragonUnrelentingForceAreaShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x874, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x873, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_UnrelentingForceShoutPAAR"_h:
            case "ks_DragonUnrelentingForceAreaShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E27, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E26, "Skyrim.esm")));
                }
                break;

            case "ks_DragonDismayAreaShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E69, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E59, "Skyrim.esm")));
                }
                break;

            case "ks_DragonDismayAreaShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF04, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF03, "Skyrim.esm")));
                }
                break;

            case "ks_DragonDismayAreaShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x876, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x875, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonDismayAreaShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E29, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E28, "Skyrim.esm")));
                }
                break;

            case "ks_DragonIceFormShout"_h:
            case "005FrostForgetShouts"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E6F, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E6B, "Skyrim.esm")));
                }
                break;

            case "ks_DragonIceFormShoutALDUIN"_h:
            case "005FrostForgetShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF02, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF01, "Skyrim.esm")));
                }
                break;

            case "ks_DragonIceFormShoutODAH"_h:
            case "005FrostForgetShoutsODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x878, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x877, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonIceFormShoutPAAR"_h:
            case "005FrostForgetShoutsPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E3C, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E2A, "Skyrim.esm")));
                }
                break;

            case "ks_DragonDisarmShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E7A, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E79, "Skyrim.esm")));
                }
                break;

            case "ks_DragonDisarmShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF00, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FEFF, "Skyrim.esm")));
                }
                break;

            case "ks_DragonDisarmShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87A, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x879, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonDisarmShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E44, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E3D, "Skyrim.esm")));
                }
                break;

            case "L_DragonFireBreathShoutDRAGON"_h:
            case "L_DragonFireBallShout"_h:
            case "ks_DragonFlameWaveShout"_h:
            case "005InfernoDragonVoiceDragonFire05"_h:
            case "005BlackDragonVoiceDragonFire01a"_h:
            case "005AncientDragonFire01"_h:
            case "005HighRedDragonVoiceDragonFire05"_h:
            case "005GoldDragonVoiceDragonFire"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16ED0, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E91, "Skyrim.esm")));
                }
                break;

            case "L_DragonFireBreathShoutALDUIN2"_h:
            case "L_DragonFireBallShoutALDUIN2"_h:
                if (SpellFire)
                {
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A0, "Dragons shout with voice - KS2 Patch.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10F563, "Skyrim.esm")));
                    }
                }
                else
                {
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8C, "Dragons shout with voice - KS2 Patch.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10F562, "Skyrim.esm")));
                    }
                }
                break;

            case "L_DragonFireBreathShoutODAH"_h:
            case "L_DragonFireBallShoutODAH"_h:
            case "ks_DragonFlameWaveShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87C, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87B, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "L_DragonFireBreathShoutPAAR"_h:
            case "L_DragonFireBallShoutPAAR"_h:
            case "ks_DragonFlameWaveShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E4E, "Skyrim.esm")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E46, "Skyrim.esm")));
                }
                break;

            case "L_DragonFrostBreathShout"_h:
            case "L_DragonFrostIceStormShout"_h:
            case "005BlueDragonVoiceDragonFrost01"_h:
            case "005AncientDragonFrost01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81B, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81A, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "L_DragonFrostBreathShoutALDUIN"_h:
            case "L_DragonFrostIceStormShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A3, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "L_DragonFrostBreathShoutODAH"_h:
            case "L_DragonFrostIceStormShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87E, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "L_DragonFrostBreathShoutPAAR"_h:
            case "L_DragonFrostIceStormShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D3, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D2, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x826, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x825, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AA, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A9, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x884, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x883, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D7, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D6, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_StaggerSpikesShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x824, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x823, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_StaggerSpikesShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B0, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AF, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_StaggerSpikesShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88A, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x889, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_StaggerSpikesShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DD, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DC, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonMarkedforDeathShout"_h:
            case "Serio_EDR_MarkedForDeathShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82C, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82B, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonMarkedforDeathShoutALDUIN"_h:
            case "Serio_EDR_MarkedForDeathShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B2, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B1, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonMarkedforDeathShoutODAH"_h:
            case "Serio_EDR_MarkedForDeathShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88C, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88B, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "L_DragonShockBreathShout"_h:
            case "L_DragonShockBallShout"_h:
            case "005YellowDragonVoiceDragonLightningBreath01"_h:
            case "005StormDragonVoiceDragonLightningBreath01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x830, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82F, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "L_DragonShockBreathShoutALDUIN2"_h:
            case "L_DragonShockBallShoutALDUIN2"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B8, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B7, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonSoulTearShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x822, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x821, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "ks_DragonSoulTearShout_ALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BA, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B9, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_DevourMoonLightShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C9, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C8, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_DevourMoonLightShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C0, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BF, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_DevourMoonLightShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C6, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005StormShoutsALLD"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CB, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CA, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "Serio_EDR_StormBarrageShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CD, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CC, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "PaarthurnaxClearSkies"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "PaarthurnaxBattleFury"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90C, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005ShockForgetShouts"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005ShockForgetShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FB, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FA, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005FireForgetShouts"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90B, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90A, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005FireForgetShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x909, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x908, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005FireForgetShoutsODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x905, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x904, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005FireForgetShoutsPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x907, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x906, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005GreenDragonVoiceDragonPoisonBreath01B"_h:
            case "005BronzeDragonPoisonBreath01"_h:
            case "005PinkPoisonBreath01"_h:
            case "005PurplePoisonBreath03"_h:
            case "005PurplePoisonBreath07"_h:
            case "005PurplePoisonBreath06"_h:
            case "005ApoGreenPoisonBreath60"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8F, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8E, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005SandAshBreath01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA92, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA91, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005AcidDragonBreathSpray01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA96, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA95, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005AquaDragonVoiceDragonWaterBreath01"_h:
            case "005WhiteDragonVoiceDragonWaterBreath01"_h:
            case "005LakeDragonVoiceDragonWaterBreath01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA94, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA93, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005DragonWindBlastConcSpell01"_h:
            case "005DragonTornadeConcSpell01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA98, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA97, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005SerpentDragonMassParryzeConc"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9A, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA99, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005DragonGreatWaterBallConcSpell05"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9C, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9B, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005DragonRockballConcSpell01"_h:
            case "005DragonSandballConcSpell01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9E, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9D, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "005DragonPoisonballConcSpell01"_h:
            case "005DragonAcidballConcSpell01"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAA0, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9F, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "KS_MistofSouls_Alduin"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC5, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC4, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            case "KS_SlowTime_Alduin"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC7, "Dragons shout with voice - KS2 Patch.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC6, "Dragons shout with voice - KS2 Patch.esp")));
                }
                break;

            default:

                break;
            }
        }
    }
}
