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
            a_actor->SetGraphVariableInt("iLDP_StormCall_Type", 1); // "Vul Vaaz Daan";
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

        if (a_actor->HasKeywordString("FireUnitedKey") && !a_actor->HasKeywordString("FrostUnitedKey") && !a_actor->HasKeywordString("ShockUnitedKey") && !a_actor->HasKeywordString("BloodCloakKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_StormCall_Type", 2); // "Gaar Toor Nah";

        }else if (!a_actor->HasKeywordString("FireUnitedKey") && a_actor->HasKeywordString("FrostUnitedKey") && !a_actor->HasKeywordString("ShockUnitedKey") && !a_actor->HasKeywordString("DeceiverKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_StormCall_Type", 3); // "Jiid So Daan";

        }else if (!a_actor->HasKeywordString("FireUnitedKey") && !a_actor->HasKeywordString("FrostUnitedKey") && a_actor->HasKeywordString("ShockUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_StormCall_Type", 4); // "Strun Bah Qo";

        }else if (a_actor->HasKeywordString("BloodCloakKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_StormCall_Type", 5); // "Lun So Haas";

        }else if (a_actor->HasKeywordString("DeceiverKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_StormCall_Type", 6); // "Fiik Lo Sah";
        }
    }

    void DovahAI::Enrage_state(RE::Actor *a_actor)
    {
        a_actor->SetGraphVariableBool("bLDP_IsEnraging", true);
        a_actor->SetGraphVariableInt("iLDP_Enrage_Count", 0);
        a_actor->NotifyAnimationGraph("Enrage");
        a_actor->SetGraphVariableBool("bNoStagger", true);
        //auto H = RE::TESDataHandler::GetSingleton();
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDEnrageSpell"), true, a_actor, 1, false, 0.0, a_actor); // LDP_aaaUDEnrageSpell "Enrage Effect" [SPEL:FE172805]

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
            //auto H = RE::TESDataHandler::GetSingleton();
            box->MoveToNode(a_actor, "NPC Head MagicNode [Hmag]");
            const auto caster = box->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
            caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_DragonVoiceDismayArea"), true, nullptr, 1, false, 0.0, a_actor); // FearExplosionSpell
        }else{
            GetSingleton()->Set_Box(a_actor);
            if (auto box = GetSingleton()->Get_Box(a_actor))
            {
                //auto H = RE::TESDataHandler::GetSingleton();
                box->MoveToNode(a_actor, "NPC Head MagicNode [Hmag]");
                const auto caster = box->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_DragonVoiceDismayArea"), true, nullptr, 1, false, 0.0, a_actor); // FearExplosionSpell
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
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "TSS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);

            }else{
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
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
        Physical_Impact(a_actor, "LDP_Talon", 10.0f);
        RE::NiPoint3 Tx;
        Tx.x = -1.0f;
        // FXDragonLandingImpactSet [IPDS:0002BD39]
        GFunc_Space::GFunc::PlayImpactEffect(a_actor, H->LookupForm<RE::BGSImpactDataSet>(0x2BD39, "Skyrim.esm"), "", Tx, 512.0f, false, false);

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
            while (i <= HoverWaitTime(a_actor) && GFunc_Space::IsAllowedToFly(a_actor, 1.0f) && ct && !ct->IsDead() && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 2.5f)
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
        a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
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
                if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr)) <= 45.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 1.13f) //front
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_front_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
                else if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr)) >= 135.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 1.13f)//back
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_back_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
                else if (GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) < -45.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 1.25f)//left
                {
                    std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                    GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1400ms, "GC_left_AI_Update");
                    GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                }
                else if (GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr) < 45.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 1.25f)//right
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
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
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
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
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
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
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
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            // 2 == takeoff_fast
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

            if (GetIntVariable(a_actor, "iLDP_TakeOff_Faction") == 2 && !GetBoolVariable(a_actor, "bLDP_PreventFlyingTalonSmash"))
            {
                if (a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 1.75f)
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
                                GroundAttackVerticalScene(a_actor);
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

            if (a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 2.5f)
            {
                if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 83.0f && GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
                {
                    if (ct->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                    {
                        if (GetActorValuePercent(a_actor, RE::ActorValue::kHealth) > 0.35f)
                        {
                            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= GetFloatVariable(a_actor, "fLDP_HoverAttackChance") && !GetBoolVariable(a_actor, "IsShouting") && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 2.0f)
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
                                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "PBC_AI_Update");
                                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                            }
                            else if (!GetBoolVariable(a_actor, "IsShouting") && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 2.0f)
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

    void DovahAI::ControlDistanceAIFly(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();
            int i = 0;
            while (i <= HoverWaitTime(a_actor) && GFunc_Space::IsAllowedToFly(a_actor, 1.0f) && ct && !ct->IsDead() && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 2.5f)
            {
                i += 1;
                std::jthread waitThread(wait, 1000);
            }
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1000ms, "ControlDistanceFly_AI_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
    }

    void DovahAI::ControlDistanceAIGround(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (a_actor->GetPosition().GetDistance(ct->GetPosition()) > 500.0f * 3.0f && GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 65.0f)
            {
                if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
            }
            else if (a_actor->GetPosition().GetDistance(ct->GetPosition()) > 500.0f * 3.0f)
            {
                while (a_actor->GetPosition().GetDistance(ct->GetPosition()) > 500.0f * 3.0f && a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                {
                    std::jthread waitThread(wait, 350);
                }
                if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
                
            }
            else
            {
                while (a_actor->GetPosition().GetDistance(ct->GetPosition()) < 500.0f * 3.0f && a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                {
                    std::jthread waitThread(wait, 350);
                }
                if (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
                {
                    if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
                        {
                        case 1:
                            a_actor->NotifyAnimationGraph("Takeoff");
                            break;

                        case 2:
                            a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                            break;

                        default:
                            break;
                        }
                    }
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
                }
            }
        }
    }

    void DovahAI::ControlDistanceAIFly1(RE::Actor *a_actor)
    {
        if (GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
        {
            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 83.0f)
            {
                if (GetActorValuePercent(a_actor, RE::ActorValue::kHealth) > 0.35f)
                {
                    ToGroundAttackScene(a_actor);
                }
                else
                {
                    GroundAttackScene(a_actor);
                }
            }
            else
            {
                while (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kHovering)
                {
                    GetSingleton()->ResetAI(a_actor);
                }
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "ControlDistanceFly_AI2_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
        }
        else
        {
            while (a_actor->AsActorState()->GetFlyState() == RE::FLY_STATE::kHovering)
            {
                GetSingleton()->ResetAI(a_actor);
            }
            a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "ControlDistanceFly_AI2_Update");
            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
        }
    }

    void DovahAI::ToHoverAttackScene(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            a_actor->NotifyAnimationGraph("FlyStartHover");
            auto ct = targethandle.get();
            int i = 0;
            while (i <= HoverWaitTime(a_actor) && GFunc_Space::IsAllowedToFly(a_actor, 1.0f) && ct && !ct->IsDead() && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 2.5f)
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
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "HAS_AI2_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
        }else{
            GetSingleton()->ResetAI(a_actor);
            a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
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
        else if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= GetFloatVariable(a_actor, "fLDP_TalonAttackChance"))
        {
            GFunc_Space::GFunc::Set_iFrames(a_actor);
            TalonSmashScene(a_actor);
        }
    }

    void DovahAI::CombatStylePref(RE::Actor *a_actor)
    {
        if (a_actor->HasKeywordString("AlduinUnitedKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_PreferCombatStyle", 1);
        }
        else if (a_actor->HasKeywordString("MasterPaarthurnaxKey"))
        {
            a_actor->SetGraphVariableInt("iLDP_PreferCombatStyle", 1);
        }
        else
        {
            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 25.0f)
            {
                a_actor->SetGraphVariableInt("iLDP_PreferCombatStyle", 1);
                a_actor->SetGraphVariableFloat("fLDP_TalonAttackChance", 25.0);
                a_actor->SetGraphVariableFloat("fLDP_HoverAttackChance", 38.0);
            }
            else if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
            {
                a_actor->SetGraphVariableInt("iLDP_PreferCombatStyle", 2);
                a_actor->SetGraphVariableFloat("fLDP_TalonAttackChance", 6.0);
                a_actor->SetGraphVariableFloat("fLDP_HoverAttackChance", 13.0);
            }
            else
            {
                a_actor->SetGraphVariableInt("iLDP_PreferCombatStyle", 0);
                a_actor->SetGraphVariableFloat("fLDP_TalonAttackChance", 13.0);
                a_actor->SetGraphVariableFloat("fLDP_HoverAttackChance", 25.0);
            }
        }
    }

    void DovahAI::OneMoreTailTurn(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (!GetBoolVariable(a_actor, "bTailTurnOneMore"))
            {
                a_actor->SetGraphVariableBool("bTailTurnOneMore", true);
                return;
            }

            auto HeadAngle = GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr);

            if(!GetFuzzy(HeadAngle, 30.0, 90.0)){
                if (HeadAngle < -30.0 && HeadAngle > -180.0)
                {
                    a_actor->NotifyAnimationGraph("SwingShout");
                }
                else if (HeadAngle > 30.0 && HeadAngle < 180.0)
                {
                    a_actor->NotifyAnimationGraph("SwingShout");
                }
                a_actor->SetGraphVariableBool("bTailTurnOneMore", false);
            }
        }
    }

    void DovahAI::MoveControllShout(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            auto degrees = abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr));
            a_actor->SetGraphVariableFloat("fLDP_MoveCtrlShout_angle", degrees);

            if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 15.0f && a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 1500.0f && degrees < 80.0f)
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

    void DovahAI::MoveControllShout1(RE::Actor *a_actor)
    {
        auto trackspeed = pow(GetFloatVariable(a_actor, "fLDP_MoveCtrlShout_angle") * (1.0 + (GetIntVariable(a_actor, "iLDP_Lvl_Rank") / 50.0)) / 10.0, 3.0) / 10000.0;
        a_actor->SetGraphVariableFloat("BSLookAtModifier_m_onGain_Shouting", trackspeed);

        int i = 0;
        while ((GetBoolVariable(a_actor, "IsShouting")) && i < 12)
        {
            std::jthread waitThread(wait, 250);
            i += 1;
        }
        a_actor->SetGraphVariableFloat("BSLookAtModifier_m_onGain_Combat", 0.075);
        a_actor->SetGraphVariableFloat("BSLookAtModifier_m_onGain_Shouting", 0.25);

        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 100ms, "MovShout2_AI_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::MoveControllShout2(RE::Actor *a_actor)
    {
        auto yt = GetSingleton()->Get_AttackList(a_actor);
        std::get<2>(yt)[1] = 0;
        AddBehavior(a_actor);
    }

    void DovahAI::LandingCombatAI(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (a_actor->GetPosition().GetDistance(ct->GetPosition()) <= 500.0f * 3.0f)
            {
                if (GetIntVariable(a_actor, "iLDP_PreferCombatStyle") != 2)
                {
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 35.0f)
                    {
                        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 1000ms, "LandingCombat_AI_Update");
                        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                    }
                }
            }
            
        }
    }

    void DovahAI::LandingCombatAI1(RE::Actor *a_actor)
    {
        if (GetActorValuePercent(a_actor, RE::ActorValue::kStamina) >= 1.0f && GFunc_Space::Has_Magiceffect_Keyword(a_actor, RE::TESForm::LookupByEditorID<RE::BGSKeyword>("a_spell"), 0.0f) && !(GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging") || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell")) || a_actor->HasSpell(RE::TESForm::LookupByEditorID<RE::SpellItem>("a_spell"))))
        {
            switch (GetIntVariable(a_actor, "iLDP_TakeOff_Faction"))
            {
            case 1:
                a_actor->NotifyAnimationGraph("Takeoff");
                break;

            case 2:
                a_actor->NotifyAnimationGraph("Takeoff_Vertical");
                break;

            default:
                break;
            }
        }
        a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", false);
    }

    bool DovahAI::GetFuzzy(float value, float min, float max)
    {
        bool result = false;

        if(value < min){
            result = true;

        }else if(value <= max){
            float grade = (value - min) / (max - min);
            if(GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 1.0f) > grade){
                result = true;
            }
        }
        return result;
    }

    int DovahAI::Random(std::vector<int> List)
    {
        //auto yt = GetSingleton()->Get_AttackList(a_actor);
       // std::get<2>(yt)[1] = 0;
       int i = 0;
	   int sum = 0;
       while (i < List.size())
       {
           sum += List[i];
           i += 1;
       }
       int RandomResult = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, sum);
       i = 0;

       while (i < List.size() && RandomResult > 0)
       {
           RandomResult -= List[i];
           i += 1;
       }
       i -= 1;
       return i;
    }

    void DovahAI::StartShout(RE::Actor *a_actor)
    {
        int RNG = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 2);
        auto H = RE::TESDataHandler::GetSingleton();
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
        {
        case 1:
        case 2:
        case 3:
            switch (RNG)
            {
            case 1:
                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); //fire
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDSwingFireShout"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            case 2:
                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); //frost
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDSwingFrostShout"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            default:
                break;
            }
            break;

        case 4:
        case 6:
            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
            caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDSwingFireShout"), true, nullptr, 1, false, 0.0, a_actor);
            break;
        case 5:
        case 7:
            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
            caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_aaaUDSwingFrostShout"), true, nullptr, 1, false, 0.0, a_actor);
            break;

        default:
            break;
        }
    }

    void DovahAI::TripleShout(RE::Actor *a_actor)
    {
        int RNG = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 2);
        int RNGy = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 3);
        auto H = RE::TESDataHandler::GetSingleton();
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
        {
        case 1:
        case 2:
            switch (RNGy)
            {
            case 1:
               // fire
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFireBreath3"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            case 2:
                // frost
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFrostBreath3"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            case 3:
                // shock
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_DragonReverbAreaNormal"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            default:
                break;
            }
            break;

        case 3:
            switch (RNG)
            {
            case 1:
                // fire
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFireBreath3"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            case 2:
                // frost
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFrostBreath3"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            default:
                break;
            }
            break;

        case 4:
            switch (RNG)
            {
            case 1:
                // fire
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFireBreath3"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            case 2:
                // shock
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_DragonReverbAreaNormal"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            default:
                break;
            }
            break;

        case 5:
            switch (RNG)
            {
            case 1:
                // shock
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_DragonReverbAreaNormal"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            case 2:
                // frost
                caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFrostBreath3"), true, nullptr, 1, false, 0.0, a_actor);
                break;

            default:
                break;
            }
            break;

        case 6:
            // fire
            caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFireBreath3"), true, nullptr, 1, false, 0.0, a_actor);
            break;

        case 7:
            // frost
            caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("VoiceFrostBreath3"), true, nullptr, 1, false, 0.0, a_actor);
            break;

        case 8:
            // shock
            caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("LDP_DragonReverbAreaNormal"), true, nullptr, 1, false, 0.0, a_actor);
            break;

        default:
            break;
        }
    }

    void DovahAI::MagicSelector(RE::Actor *a_actor, int value)
    {
        int RNGf = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 3);
        int RNGfx = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 2);

        switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
        {
        case 1:
        case 2:
            switch (RNGf)
            {
            case 2:
                value += 1;
                break;

            case 3:
                value += 2;
                break;

            default:
                break;
            }
            break;
        case 3:
            switch (RNGfx)
            {
            case 2:
                value += 1;
                break;

            default:
                break;
            }
            break;
        case 4:
            switch (RNGfx)
            {
            case 2:
                value += 1;
                break;

            default:
                break;
            }
            break;
        case 5:
            switch (RNGf)
            {
            case 2:
                value += 1;
                break;

            case 3:
                value += 2;
                break;

            default:
                break;
            }
            break;

        case 7:
            switch (RNGfx)
            {
            case 2:
                value += 1;
                break;

            default:
                break;
            }
            break;

        case 8:
            switch (RNGfx)
            {
            case 2:
                value += 2;
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        if(value > 10){
            return;
        }

        a_actor->SetGraphVariableInt("iLDP_MagicSelector", value);
        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 10ms, "MagicSelector_AI_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::MagicSelector1(RE::Actor *a_actor)
    {
        const auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        caster->CastSpellImmediate(RE::TESForm::LookupByEditorID<RE::MagicItem>("a_spell"), true, a_actor, 1, false, 0.0, a_actor);
    }

    void DovahAI::StartParry(RE::Actor *a_actor)
    {
        if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 75.0f)
        {
            a_actor->NotifyAnimationGraph("ParryAttack");
        }else{
            a_actor->NotifyAnimationGraph("Parry");
        }
        a_actor->NotifyAnimationGraph("RecoilLargeStart");

        std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
        GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 200ms, "startparry_AI_Update");
        GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
    }

    void DovahAI::AddBehavior(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();
            int i = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 100);
            auto LvRank = GetIntVariable(a_actor, "iLDP_Lvl_Rank");
            auto yt = GetSingleton()->Get_AttackList(a_actor);
            if (i >= 80 + LvRank)
            {
                return;
            }
            auto distance = a_actor->GetPosition().GetDistance(ct->GetPosition());
            auto headingAngle = GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, ct, nullptr);

            if(i <= std::get<0>(yt)){
                switch (Random(std::get<1>(yt)))
                {
                case 0:
                    a_actor->NotifyAnimationGraph("Step");
                    break;

                case 4:
                    switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 6:
                        MagicSelector(a_actor, 10);
                        break;
                    
                    default:
                        break;
                    }
                    break;

                default:
                    break;
                }
            }
            else if (GetFuzzy(distance, 530.0, 700.0))
            {
                switch (Random(std::get<2>(yt)))
                {
                case 1:
                    if (headingAngle <= 180.000 && headingAngle > 0.000000)
                    {
                    }else{

                    }
                    break;

                case 2:
                    switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                    {
                    case 0:
                        break;

                    default:
                        MagicSelector(a_actor, 1);
                        break;
                    }
                    break;

                case 3:
                    switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                    {
                    case 0:
                    case 8:
                        break;

                    default:
                        a_actor->NotifyAnimationGraph("MarkedForDeath");
                        break;
                    }
                    break;

                case 4:
                    if (GetFuzzy(abs(headingAngle), 80.0000, 90.0000))
                    {
                        switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                        {
                        case 0:
                        case 8:
                            break;

                        default:
                            a_actor->NotifyAnimationGraph("SwingShout");
                            break;
                        }
                    }
                    break;

                case 5:
                    if (GetFuzzy(abs(headingAngle), 80.0000, 90.0000))
                    {
                        if(!GetBoolVariable(a_actor, "bLDP_InParry_State")){
                            a_actor->SetGraphVariableBool("bLDP_InParry_State", true);
                            a_actor->NotifyAnimationGraph("parrywait");
                        }
                    }
                    break;

                default:
                    break;
                }
            }
            else if (GetFuzzy(abs(headingAngle), 80.0, 90.0))
            {
                int RNG = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 2);
                int RNGy = GFunc_Space::GFunc::GetSingleton()->GenerateRandomInt(1, 3);
                auto H = RE::TESDataHandler::GetSingleton();
                switch (Random(std::get<3>(yt)))
                {
                case 0:
                    a_actor->NotifyAnimationGraph("Step");
                    break;

                case 1:
                    a_actor->NotifyAnimationGraph("ThreeTimesShout");
                    break;

                case 2:
                    switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                    {
                    case 0:
                        break;

                    default:
                        MagicSelector(a_actor, 4);
                        break;
                    }
                    break;

                case 3:
                    switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                    {
                    case 0:
                        break;

                    default:
                        MagicSelector(a_actor, 7);
                        break;
                    }
                    break;

                case 4:
                    switch (GetIntVariable(a_actor, "iLDP_Dragon_Type"))
                    {
                    case 1:
                        //Alduin voice
                        switch (RNGy)
                        {
                        case 1:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                            break;

                        case 2:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // shock
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 2);
                            break;

                        case 3:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                            break;

                        default:
                            break;
                        }
                        break;
                    case 2:
                        //Dragon Voice
                        switch (RNGy)
                        {
                        case 1:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                            break;

                        case 2:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // shock
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 2);
                            break;

                        case 3:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                            break;

                        default:
                            break;
                        }
                        break;

                    case 3:
                        if (a_actor->HasKeywordString("MasterPaarthurnaxKey"))
                        {
                            switch (RNG)
                            {
                            case 1:
                                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                                a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                                break;

                            case 2:
                                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                                a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                                break;

                            default:
                                break;
                            }
                        }
                        else if (a_actor->HasKeywordString("OdahviingKey"))
                        {
                            switch (RNG)
                            {
                            case 1:
                                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                                a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                                break;

                            case 2:
                                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                                a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                                break;

                            default:
                                break;
                            }
                        }
                        else
                        {
                            switch (RNG)
                            {
                            case 1:
                                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                                a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                                break;

                            case 2:
                                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                                a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                                break;

                            default:
                                break;
                            }
                        }
                        break;

                    case 4:
                        //fireshock
                        switch (RNG)
                        {
                        case 1:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // shock
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 2);
                            break;

                        case 2:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                            break;

                        default:
                            break;
                        }
                        break;

                    case 5:
                        //frostshock
                        switch (RNG)
                        {
                        case 1:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                            break;

                        case 2:
                            GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // shock
                            a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 2);
                            break;

                        default:
                            break;
                        }
                        break;

                    case 6:
                        GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // fire
                        a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 3);
                        break;

                    case 7:
                        GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // frost
                        a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 1);
                        break;

                    case 8:
                        GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // shock
                        a_actor->SetGraphVariableInt("iLDP_Shout_to_Cast", 2);
                        break;

                    default:
                        break;
                    }
                    break;

                default:
                    break;
                }
                a_actor->NotifyAnimationGraph("ShoutFireBallStart");
            }
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
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "GAS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);

            }else{
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "GAS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
        }
    }

    void DovahAI::GroundAttackVerticalScene(RE::Actor *a_actor)
    {
        if (auto targethandle = a_actor->GetActorRuntimeData().currentCombatTarget.get(); targethandle)
        {
            auto ct = targethandle.get();

            if (ct->AsActorState()->GetFlyState() == RE::FLY_STATE::kNone)
            {
                if (GFunc_Space::IsAllowedToFly(a_actor, 1.0f))
                {
                    GetSingleton()->SetLandingMarker(a_actor);
                    a_actor->SetGraphVariableInt("iLDP_Landing_Faction", 2); // vertical landing faction
                    a_actor->SetGraphVariableBool("bLDP_DragonFlightlessCombat", true);
                    switch (GetIntVariable(a_actor, "iLDP_PreferCombatStyle"))
                    {
                    case 0:
                    case 1:
                        if (a_actor)
                        {
                            std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                            GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 12000ms, "GAS_Flight_Update");
                            GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
                        }
                        break;

                    case 2:
                        if (a_actor)
                        {
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
                    GFunc_Space::GFunc::SetForcedLandingMarker(a_actor, nullptr);
                }
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "GAVS_AI_Update");
                GFunc_Space::GFunc::GetSingleton()->RegisterforUpdate(a_actor, data);
            }
            else
            {
                a_actor->SetGraphVariableBool("bLDP_AIControl_doOnce", true);
                std::tuple<bool, std::chrono::steady_clock::time_point, GFunc_Space::ms, std::string> data;
                GFunc_Space::GFunc::set_tupledata(data, true, std::chrono::steady_clock::now(), 8100ms, "GAVS_AI_Update");
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
                    if (a_actor->GetPosition().GetDistance(Enemy->GetPosition()) <= 500.0f)
                    {
                        DovahAI_Space::DovahAI::DamageTarget(a_actor, Enemy);
                        if (!(Enemy->IsBlocking() && GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(Enemy, a_actor, nullptr) <= 45.0f))
                        {
                            if (!Enemy->HasKeywordString("ActorTypeDragon"))
                            {
                                GFunc_Space::GFunc::PushActorAway(a_actor, Enemy, p_force);
                            }
                        }
                    }
                }
            }
        }
    }

    void DovahAI::BiteAttack_Impact(RE::Actor *a_actor, RE::Actor *a_target)
    {
        auto H = RE::TESDataHandler::GetSingleton();
        if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_target, a_actor, nullptr)) <= 45.0f)
        {
            if (GetBoolVariable(a_target, "IsBlocking"))
            {
                return;
            }
            else if (GetBoolVariable(a_target, "IsAttacking"))
            {
                a_target->NotifyAnimationGraph("recoilLargeStart");
                switch (GFunc_Space::GFunc::GetEquippedItemType(a_target, false))
                {
                case 1:
                case 2:
                case 5:
                    GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashblade
                    break;

                case 4:
                case 6:
                    GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashblunt
                    break;

                case 3:
                    GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashaxe
                    break;

                default:
                    break;
                }
            }
            else
            {
                a_target->NotifyAnimationGraph("staggerStop");
                a_target->SetGraphVariableFloat("staggerDirection", 0.0);
                a_target->SetGraphVariableFloat("staggerMagnitude", 1.0);
                a_target->NotifyAnimationGraph("staggerStart");
            }
        }else{
            a_target->NotifyAnimationGraph("staggerStop");
            a_target->SetGraphVariableFloat("staggerDirection", 0.0);
            a_target->SetGraphVariableFloat("staggerMagnitude", 1.0);
            a_target->NotifyAnimationGraph("staggerStart");
        }
    }

    void DovahAI::LeftWingAttack_Impact(RE::Actor *a_actor, RE::Actor *a_target)
    {
        if (GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, a_target, nullptr) < -45.0f)
        {
            auto H = RE::TESDataHandler::GetSingleton();
            if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_target, a_actor, nullptr)) <= 45.0f)
            {
                if (GetBoolVariable(a_target, "IsBlocking"))
                {
                    return;
                }
                else if (GetBoolVariable(a_target, "IsAttacking"))
                {
                    a_target->NotifyAnimationGraph("recoilLargeStart");
                    switch (GFunc_Space::GFunc::GetEquippedItemType(a_target, false))
                    {
                    case 1:
                    case 2:
                    case 5:
                        GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashblade
                        break;

                    case 4:
                    case 6:
                        GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashblunt
                        break;

                    case 3:
                        GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashaxe
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    GFunc_Space::GFunc::PushActorAway(a_actor, a_target, 13.0);
                }
            }
            else
            {
                GFunc_Space::GFunc::PushActorAway(a_actor, a_target, 13.0);
            }
        }
    }

    void DovahAI::RightWingAttack_Impact(RE::Actor *a_actor, RE::Actor *a_target)
    {
        if (GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_actor, a_target, nullptr) > 45.0f)
        {
            auto H = RE::TESDataHandler::GetSingleton();
            if (abs(GFunc_Space::GFunc::GetSingleton()->get_angle_he_me(a_target, a_actor, nullptr)) <= 45.0f)
            {
                if (GetBoolVariable(a_target, "IsBlocking"))
                {
                    return;
                }
                else if (GetBoolVariable(a_target, "IsAttacking"))
                {
                    a_target->NotifyAnimationGraph("recoilLargeStart");
                    switch (GFunc_Space::GFunc::GetEquippedItemType(a_target, false))
                    {
                    case 1:
                    case 2:
                    case 5:
                        GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashblade
                        break;

                    case 4:
                    case 6:
                        GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashblunt
                        break;

                    case 3:
                        GFunc_Space::GFunc::playSound(a_target, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // bashaxe
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    GFunc_Space::GFunc::PushActorAway(a_actor, a_target, 13.0);
                }
            }
            else
            {
                GFunc_Space::GFunc::PushActorAway(a_actor, a_target, 13.0);
            }
        }
    }

    void DovahAI::Unarm_effect(RE::Actor *a_actor)
    {
        GFunc_Space::GFunc::InterruptAttack(a_actor);

        auto form_list = GFunc_Space::GFunc::GetEquippedForm(a_actor);

        if (!form_list.empty())
        {
            for (auto form : form_list)
            {
                if (form)
                {
                    GFunc_Space::GFunc::EquipfromInvent(a_actor, form->formID, true);
                }
                continue;
            }
        }
    }

    void DovahAI::Random_TakeOffandDeath_Anims(RE::Actor *a_actor)
    {
        if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
        {
            a_actor->SetGraphVariableInt("iLDP_TakeOff_Faction", 1); // TakeOffDefault
        }else{
            a_actor->SetGraphVariableInt("iLDP_TakeOff_Faction", 2); // TakeOffVertical
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
                GFunc_Space::GFunc::playSound(a_actor, (H->LookupForm<RE::BGSSoundDescriptorForm>(0x802, "LeoneDragonProject.esp"))); // k/s_NPCDragonKillMove
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

    void DovahAI::GetEquippedShout_Alduin(RE::Actor *actor, bool SpellFire){
        if (auto limboshout = actor->GetActorRuntimeData().selectedPower) {
            if (limboshout->Is(RE::FormType::Shout)){
                auto data = RE::TESDataHandler::GetSingleton();
                std::string Lsht = (clib_util::editorID::get_editorID(limboshout));

                switch (hash(Lsht.c_str(), Lsht.size())){
                case "LDP_UnrelentingForceShout"_h:
                case "LDP_DragonUnrelentingForceAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF06, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF05, "Skyrim.esm")));
                    }
                    break;

                case "LDP_ChainSuppressionShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AA, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A9, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_BackfireRecoilShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AC, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AB, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DevourMoonLightShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C0, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BF, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_MarkedForDeathShout_Area"_h:
                case "LDP_DragonMarkedforDeathShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B2, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B1, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_SlowTimeShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC7, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC6, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDisarmShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF00, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FEFF, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonBecomeEtherealShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A6, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A5, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDismayAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF04, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF03, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonDrainVitalityShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B4, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B3, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonMoonBlastShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C4, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C3, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonSoulTearShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BA, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B9, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonIceFormShout"_h:
                case "LDP_FrostNuke_FrostForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF02, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10FF01, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonCycloneShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B6, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B5, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DLC2BendWillShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA81, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AD, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "L_DragonFireBreathShout"_h:
                case "L_DragonFireBallShout"_h:
                    if (SpellFire)
                    {
                        if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 100.0f) <= 50.0f)
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A0, "Leone Dragon Project Voicing.esp")));
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
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8C, "Leone Dragon Project Voicing.esp")));
                        }
                        else
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10F562, "Skyrim.esm")));
                        }
                    }
                    break;

                case "L_DragonFrostBreathShout"_h:
                case "L_DragonFrostIceStormShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A4, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A3, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonShockBreathShout"_h:
                case "LDP_DragonShockBallShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B8, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B7, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_zz005AlduinLightningstorms"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BC, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BB, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_zz005AlduinFroststorms"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C2, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C1, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_zz05AlduinFirestorms"_h:
                case "dunCGDragonStormCallShout"_h:
                case "MQ206AlduinFirestormShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A8, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A7, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_MistofSouls_Alduin"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC5, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC4, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_zz005DevourSoul_Alduin"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BE, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BD, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireConc_UniqueBreathShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F9, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F8, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FrostConc_VolleyBreathShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F7, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F6, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireNuke_FireForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x909, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x908, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_ShockNuke_ShockForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FB, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FA, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_StaggerSpikesShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B0, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AF, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    void DovahAI::GetEquippedShout_Odahviing(RE::Actor *actor, bool SpellFire){
        if (auto limboshout = actor->GetActorRuntimeData().selectedPower) {
            if (limboshout->Is(RE::FormType::Shout)){
                auto data = RE::TESDataHandler::GetSingleton();
                std::string Lsht = (clib_util::editorID::get_editorID(limboshout));

                switch (hash(Lsht.c_str(), Lsht.size())){
                case "LDP_UnrelentingForceShout"_h:
                case "LDP_DragonUnrelentingForceAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x874, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x873, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_ChainSuppressionShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x884, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x883, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_BackfireRecoilShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x886, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x885, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DevourMoonLightShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C6, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C5, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_MarkedForDeathShout_Area"_h:
                case "LDP_DragonMarkedforDeathShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88C, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88B, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_SlowTimeShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x888, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x887, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDisarmShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87A, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x879, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonBecomeEtherealShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x880, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87F, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDismayAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x876, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x875, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDrainVitalityShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88E, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88D, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonIceFormShout"_h:
                case "LDP_FrostNuke_FrostForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x878, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x877, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonCycloneShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x890, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88F, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "L_DragonFireBreathShout"_h:
                case "L_DragonFireBallShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87C, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87B, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "L_DragonFrostBreathShout"_h:
                case "L_DragonFrostIceStormShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87E, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87D, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireConc_UniqueBreathShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x903, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x902, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FrostConc_VolleyBreathShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x901, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x900, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireNuke_FireForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x905, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x904, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_StaggerSpikesShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88A, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x889, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    void DovahAI::GetEquippedShout_Paarthurnax(RE::Actor *actor, bool SpellFire)
    {
        if (auto limboshout = actor->GetActorRuntimeData().selectedPower) {
            if (limboshout->Is(RE::FormType::Shout)){
                auto data = RE::TESDataHandler::GetSingleton();
                std::string Lsht = (clib_util::editorID::get_editorID(limboshout));

                switch (hash(Lsht.c_str(), Lsht.size())){
                case "LDP_UnrelentingForceShout"_h:
                case "LDP_DragonUnrelentingForceAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E27, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E26, "Skyrim.esm")));
                    }
                    break;

                case "LDP_ChainSuppressionShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D7, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D6, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_BackfireRecoilShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D9, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D8, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_SlowTimeShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DB, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DA, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDisarmShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E44, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E3D, "Skyrim.esm")));
                    }
                    break;
                
                case "LDP_DragonBecomeEtherealShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D5, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D4, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDismayAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E29, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E28, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonIceFormShout"_h:
                case "LDP_FrostNuke_FrostForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E3C, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E2A, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonCycloneShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DF, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DE, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "L_DragonFireBreathShout"_h:
                case "L_DragonFireBallShout"_h:
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
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D3, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D2, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_PaarthurnaxClearSkies"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E5, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E4, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_PaarthurnaxElementalFury"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E3, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E2, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_PaarthurnaxBattleFury"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90D, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90C, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_PaarthurnaxWhirlwindTempest"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E1, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8E0, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireConc_UniqueBreathShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FF, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FE, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FrostConc_VolleyBreathShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FD, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FC, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireNuke_FireForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x907, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x906, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_StaggerSpikesShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DD, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DC, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    void DovahAI::GetEquippedShout_Dragon(RE::Actor *actor, bool SpellFire)
    {
        if (auto limboshout = actor->GetActorRuntimeData().selectedPower) {
            if (limboshout->Is(RE::FormType::Shout)){
                auto data = RE::TESDataHandler::GetSingleton();
                std::string Lsht = (clib_util::editorID::get_editorID(limboshout));

                switch (hash(Lsht.c_str(), Lsht.size())){
                case "LDP_UnrelentingForceShout"_h:
                case "LDP_DragonUnrelentingForceAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E53, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E50, "Skyrim.esm")));
                    }
                    break;

                case "LDP_ChainSuppressionShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x826, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x825, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_BackfireRecoilShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82A, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x829, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DevourMoonLightShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C9, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C8, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_MarkedForDeathShout_Area"_h:
                case "LDP_DragonMarkedforDeathShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82C, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82B, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDisarmShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E7A, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E79, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonBecomeEtherealShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x834, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x833, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonDismayAreaShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E69, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E59, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonDrainVitalityShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x820, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81F, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonMoonBlastShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D1, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D0, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonSoulTearShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x822, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x821, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonIceFormShout"_h:
                case "LDP_FrostNuke_FrostForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E6F, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E6B, "Skyrim.esm")));
                    }
                    break;

                case "LDP_DragonCycloneShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x837, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x836, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "L_DragonFireBreathShout"_h:
                case "L_DragonFireBallShout"_h:
                case "LDP_005InfernoDragonFireBreathShout01"_h:
                case "LDP_005BlackDragonFireBreathShout01"_h:
                case "LDP_005AncientDragonFireShout01"_h:
                case "LDP_005HighRedDragonFireBreathShout01"_h:
                case "LDP_005GoldDragonFireBreathShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16ED0, "Skyrim.esm")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x16E91, "Skyrim.esm")));
                    }
                    break;

                case "L_DragonFrostBreathShout"_h:
                case "L_DragonFrostIceStormShout"_h:
                case "LDP_005BlueDragonFrostBreathShout01"_h:
                case "LDP_005AncientDragonFrostShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81B, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81A, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_DragonShockBreathShout"_h:
                case "LDP_DragonShockBallShout"_h:
                case "LDP_005YellowDragonLightningBreathShout01"_h:
                case "LDP_005StormDragonLightningBreathShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x830, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82F, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_SoulCairnSummonShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81D, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81C, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireConc_UniqueBreathShouts"_h:
                case "LDP_005ViinturuthMeteorBallConcShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F3, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F2, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FrostConc_VolleyBreathShouts"_h:
                case "LDP_005DragonIceStormConcShout02"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F1, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F0, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_FireNuke_FireForgetShouts"_h:
                case "LDP_005GoldDragonFireBallConcShout06"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90B, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x90A, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_ShockNuke_ShockForgetShouts"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F5, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F4, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005StormShoutsALL"_h:
                    switch (GetIntVariable(actor, "iLDP_StormCall_Type"))
                    {
                    case 1:
                        //ALLD
                        if (SpellFire)
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CB, "Leone Dragon Project Voicing.esp")));
                        }
                        else
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CA, "Leone Dragon Project Voicing.esp")));
                        }
                        break;

                    case 2:
                        //Fire
                        if (SpellFire)
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x859, "Leone Dragon Project Voicing.esp")));
                        }
                        else
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x858, "Leone Dragon Project Voicing.esp")));
                        }
                        break;

                    case 3:
                        //Frost
                        if (SpellFire)
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CF, "Leone Dragon Project Voicing.esp")));
                        }
                        else
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CE, "Leone Dragon Project Voicing.esp")));
                        }
                        break;

                    case 4:
                        //Shock
                        if (SpellFire)
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x832, "Leone Dragon Project Voicing.esp")));
                        }
                        else
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x831, "Leone Dragon Project Voicing.esp")));
                        }
                        break;

                    case 6:
                        //Decieve
                        if (SpellFire)
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x9CC, "Leone Dragon Project Voicing.esp")));
                        }
                        else
                        {
                            GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82D, "Leone Dragon Project Voicing.esp")));
                        }
                        break;

                    default:
                        break;
                    }

                    break;

                case "LDP_005ViljoDarkBallShout01"_h:
                    // 5; Blood
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x949, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x948, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005SahloknirStormShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82E, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82D, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_SkyShiftShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CD, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8CC, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005GreenDragonPoisonBreathShout01b"_h:
                case "LDP_005BronzeDragonPoisonBreathShout01"_h:
                case "LDP_005PinkPoisonBreathShout01"_h:
                case "LDP_005PurplePoisonBreathShout03"_h:
                case "LDP_005ApocryphaPoisonBreathShout06"_h:
                case "LDP_005SerpentDragonPoisonBreathShout07"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8F, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8E, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005AquaDragonWaterBreathShout01"_h:
                case "LDP_005WhiteDragonWaterBreathShout01"_h:
                case "LDP_005LakeDragonWaterBreathShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA94, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA93, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005DesertDragonSandBreathShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA92, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA91, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005AcidDragonAcidBreathShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA96, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA95, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005DragonWindBlastConcShout01"_h:
                case "LDP_005DragonTornadeConcShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA98, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA97, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005SerpentDragonParlyzeBallShoutConc"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9A, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA99, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005FatDragonGreatWatrerConcShout05"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9C, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9B, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005DragonRockBallConcShout01"_h:
                case "LDP_005DragonSandBallConcShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9E, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9D, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_005DragonPoisonBallConcShout01"_h:
                case "LDP_005DragonAcidBallConcShout01"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAA0, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA9F, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                case "LDP_StaggerSpikesShout"_h:
                    if (SpellFire)
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x824, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x823, "Leone Dragon Project Voicing.esp")));
                    }
                    break;

                default:
                    break;
                }
            }
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
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A0, "Leone Dragon Project Voicing.esp")));
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
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8C, "Leone Dragon Project Voicing.esp")));
                    }
                    else
                    {
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x10F562, "Skyrim.esm")));
                    }
                }
                break;

            case "005UniqueBreathShouts"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F3, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F2, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "005UniqueBreathShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F9, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F8, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "005UniqueBreathShoutsODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x903, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x902, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "005UniqueBreathShoutsPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FF, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FE, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "005VolleyBreathShouts"_h:
            
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F1, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F0, "Leone Dragon Project Voicing.esp")));
                }
                break;
            case "005VolleyBreathShoutsALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F7, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8F6, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "005VolleyBreathShoutsODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x901, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x900, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "005VolleyBreathShoutsPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FD, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8FC, "Leone Dragon Project Voicing.esp")));
                }
                break;


            case "ks_DragonMoonBlast"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D1, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D0, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "ks_DragonMoonBlastALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C4, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C3, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "zz05AlduinFirestorms"_h:
            case "zz05AlduinFirestormsSERIO"_h:
            case "dunCGDragonStormCallShout"_h:
            case "MQ206AlduinFirestormShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A8, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A7, "Leone Dragon Project Voicing.esp")));
                }
                break;


            case "Serio_EDR_GravityBlastShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x828, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x827, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AE, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AD, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x888, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x887, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_GravityBlastShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DB, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8DA, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82A, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x829, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AC, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AB, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x886, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x885, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_BackfireRecoilShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D9, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D8, "Leone Dragon Project Voicing.esp")));
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
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x874, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x873, "Leone Dragon Project Voicing.esp")));
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
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x878, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x877, "Leone Dragon Project Voicing.esp")));
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
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87A, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x879, "Leone Dragon Project Voicing.esp")));
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
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A0, "Leone Dragon Project Voicing.esp")));
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
                        GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xA8C, "Leone Dragon Project Voicing.esp")));
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
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87C, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x87B, "Leone Dragon Project Voicing.esp")));
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
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81B, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x81A, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x826, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x825, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8AA, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8A9, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x884, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x883, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_ChainSuppressionShoutPAAR"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D7, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8D6, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "ks_DragonMarkedforDeathShout"_h:
            case "Serio_EDR_MarkedForDeathShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82C, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82B, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "ks_DragonMarkedforDeathShoutALDUIN"_h:
            case "Serio_EDR_MarkedForDeathShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B2, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B1, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "ks_DragonMarkedforDeathShoutODAH"_h:
            case "Serio_EDR_MarkedForDeathShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88C, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x88B, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "L_DragonShockBreathShout"_h:
            case "L_DragonShockBallShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x830, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x82F, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "L_DragonShockBreathShoutALDUIN2"_h:
            case "L_DragonShockBallShoutALDUIN2"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B8, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B7, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "ks_DragonSoulTearShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x822, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x821, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "ks_DragonSoulTearShout_ALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BA, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8B9, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_DevourMoonLightShout"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C9, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C8, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_DevourMoonLightShoutALDUIN"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C0, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8BF, "Leone Dragon Project Voicing.esp")));
                }
                break;

            case "Serio_EDR_DevourMoonLightShoutODAH"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C6, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0x8C5, "Leone Dragon Project Voicing.esp")));
                }
                break;


            case "KS_SlowTime_Alduin"_h:
                if (SpellFire)
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC7, "Leone Dragon Project Voicing.esp")));
                }
                else
                {
                    GFunc_Space::GFunc::playSound(actor, (data->LookupForm<RE::BGSSoundDescriptorForm>(0xAC6, "Leone Dragon Project Voicing.esp")));
                }
                break;

            default:

                break;
            }
        }
    }
}
