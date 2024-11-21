#include "GeneralFunctions.h"

namespace DovahAI_Space{


	class DovahAI
	{
	public:
		static DovahAI *GetSingleton()
		{
			static DovahAI avInterface;
			return &avInterface;
		}

		static void DeferredKill(RE::Actor *a_actor, bool start = false);
		static void DeathRadollPerch(RE::Actor *a_actor);
		static bool GetBoolVariable(RE::Actor *a_actor, std::string a_string);
		static int GetIntVariable(RE::Actor *a_actor, std::string a_string);
		static float GetFloatVariable(RE::Actor *a_actor, std::string a_string);
		static void DeathRadollCrashLand(RE::Actor *a_actor);
		void wait(int a_duration);
		float PercentageHealthAction(RE::Actor *a_actor);
		void SetLandingMarker(RE::Actor *a_actor);
		void DeathRadollFly(RE::Actor *a_actor);
		void ResetAI(RE::Actor *a_actor);
		void SendRandomAnimationEvent(RE::Actor *a_actor, int I, std::string AnimEvent01, std::string AnimEvent02, std::string AnimEvent03, std::string AnimEvent04);
		static void Others(RE::Actor *a_actor);
		static void TalonSmash(RE::Actor *a_actor);
		static bool IsMQ206CutsceneDragons(RE::Actor *a_actor);
		static void DamageTarget(RE::Actor *a_actor, float percentage);
		static void Physical_Impact(RE::Actor *a_actor, std::string a_spell, float p_force);
		static void Random_TakeOffandDeath_Anims(RE::Actor *a_actor);
		static void GetEquippedShout(RE::Actor *actor, bool SpellFire = false);

	private:
		DovahAI() = default;
		DovahAI(const DovahAI &) = delete;
		DovahAI(DovahAI &&) = delete;
		~DovahAI() = default;

		DovahAI &operator=(const DovahAI &) = delete;
		DovahAI &operator=(DovahAI &&) = delete;

		

	protected:
	};
}


