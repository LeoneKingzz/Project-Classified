#include "GeneralFunctions.h"

namespace DovahAI_Space{
	using uniqueLocker = std::unique_lock<std::shared_mutex>;
	using sharedLocker = std::shared_lock<std::shared_mutex>;

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
		static void OnInitHPCalc(RE::Actor *a_actor);
		static float GetActorValuePercent(RE::Actor *a_actor, RE::ActorValue a_value);
		static void DragonType(RE::Actor *a_actor);
		static void Enrage(RE::Actor *a_actor, int count);
		static void Enrage_state(RE::Actor *a_actor);
		static void Enrage_start(RE::Actor *a_actor);
		static void BleedOut_state(RE::Actor *a_actor);
		static void BleedOut_state1(RE::Actor *a_actor);
		static void TalonSmash(RE::Actor *a_actor);
		static void TalonSmash1(RE::Actor *a_actor);
		static void TalonSmash2(RE::Actor *a_actor);
		static bool IsMQ206CutsceneDragons(RE::Actor *a_actor);
		static bool IsUnSafeDragon(RE::Actor *a_actor);
		static void DamageTarget(RE::Actor *a_actor, RE::Actor *enemy);
		static void CalcLevelRank(RE::Actor *a_actor);
		void CreateAttackList(RE::Actor *a_actor);
		void RemoveAttackList(RE::Actor *a_actor);
		static void SetValuesDragon(RE::Actor *a_actor);
		static void Physical_Impact(RE::Actor *a_actor, std::string a_spell, float p_force);
		static void Random_TakeOffandDeath_Anims(RE::Actor *a_actor);
		static void GetEquippedShout(RE::Actor *actor, bool SpellFire = false);
		static void DeathWaitRagdoll(RE::Actor *a_actor);
		void Set_Box(RE::Actor *actor);
		void scan_activeBoxes(RE::Actor *a_actor, RE::TESObjectREFR *a_box, bool insert = false, bool clear = false, bool clear_all = false);

	private:
		DovahAI() = default;
		DovahAI(const DovahAI &) = delete;
		DovahAI(DovahAI &&) = delete;
		~DovahAI() = default;

		DovahAI &operator=(const DovahAI &) = delete;
		DovahAI &operator=(DovahAI &&) = delete;

		

	protected:
		std::unordered_map<RE::Actor *, std::tuple<int, std::vector<int>, std::vector<int>, std::vector<int>>> _attackList;
		std::shared_mutex mtx_attackList;
		std::unordered_map<RE::Actor *, std::vector<RE::TESObjectREFR *>> _Boxes;
		std::shared_mutex mtx_Boxes;
	};
}


