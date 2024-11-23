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
            GFunc_Space::GFunc::GetSingleton()->SetForcedLandingMarker(a_actor, combat_target_handle.get());
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

    void DovahAI::Enrage(RE::Actor *a_actor, int count)
    {
        if (!GetBoolVariable(a_actor, "bLDP_IsEnraging"))
        {
            auto val = GetIntVariable(a_actor, "iLDP_Enrage_Count");
            a_actor->SetGraphVariableInt("iLDP_Enrage_Count", val += 1);
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

    void DovahAI::Others(RE::Actor *a_actor)
    {
        if (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kSpeedMult) != 100.0f){
            a_actor->AsActorValueOwner()->SetActorValue(RE::ActorValue::kSpeedMult, 100.0f);
            a_actor->SetGraphVariableFloat("playbackSpeed", GetSingleton()->PercentageHealthAction(a_actor));
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

    void DovahAI::DamageTarget(RE::Actor *a_actor, float percentage)
    {
        auto perm_health = a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
        a_actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kHealth, -(perm_health * percentage));
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
                        DovahAI_Space::DovahAI::DamageTarget(Enemy, 0.25f);
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
        if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0f, 1.0f) <= 0.5f)
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
        switch (DovahAI_Space::DovahAI::GetIntVariable(a_actor, "iLDP_DeathAnim_Faction"))
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
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0, 1.0) <= 0.5f)
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
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0, 1.0) <= 0.5f)
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
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0, 1.0) <= 0.5f)
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
                    if (GFunc_Space::GFunc::GetSingleton()->GenerateRandomFloat(0.0, 1.0) <= 0.5f)
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
