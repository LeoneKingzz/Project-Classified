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
}
