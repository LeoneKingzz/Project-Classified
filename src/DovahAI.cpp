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
        float result = 0.0f;

        if (GetBoolVariable(a_actor, "Injured") || GetBoolVariable(a_actor, "IsEnraging"))
        {
            result = 1.25f;
        }else{

        }

        return result;
    }

    void DovahAI::DeathRadollCrashLand_1(RE::Actor *a_actor)
    {
        auto first_position = a_actor->GetPosition();
        DeferredKill(a_actor, true);
    }

    int main()
    {
        
        // auto t0 = GFunc_Space::Time::now();
        // auto t1 = Time::now();
        // fsec fs = t1 - t0;
        // ms d = std::chrono::duration_cast<ms>(fs);
        // std::cout << fs.count() << "s\n";
        // std::cout << d.count() << "ms\n";

        //std::tuple<bool, GFunc_Space::Time::time_point, GFunc_Space::ms, std::string> bar();
    }
}
