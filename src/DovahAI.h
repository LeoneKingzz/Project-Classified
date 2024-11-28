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
		static void SendRandomAnimationEvent(RE::Actor *a_actor, int I, std::string AnimEvent01, std::string AnimEvent02, std::string AnimEvent03, std::string AnimEvent04);
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
		static void TalonSmashScene(RE::Actor *a_actor);
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
		RE::TESObjectREFR *Get_Box(RE::Actor *a_actor);
		static void Shout(RE::Actor *a_actor);
		static void Shout1(RE::Actor *a_actor);
		static void CastAreaEffect(RE::Actor *a_actor, RE::MagicItem *a_spell, RE::BSFixedString a_node);
		std::tuple<int, std::vector<int>, std::vector<int>, std::vector<int>> Get_AttackList(RE::Actor *a_actor);
		static void ControlDistanceRiddenAI(RE::Actor *a_actor);
		static void ControlDistanceRiddenAI1(RE::Actor *a_actor);
		static void TakeoffCombatAI(RE::Actor *a_actor);
		static int HoverWaitTime(RE::Actor *a_actor);
		static void ToHoverAttackScene(RE::Actor *a_actor);
		static void ToHoverAttackScene1(RE::Actor *a_actor);
		static void ToGroundAttackScene(RE::Actor *a_actor);
		static void GroundAttackScene(RE::Actor *a_actor);
		static void PassByCombatAI(RE::Actor *a_actor);
		static void GroundCombatAI(RE::Actor *a_actor);
		static void GroundCombatAI_front(RE::Actor *a_actor);
		static void GroundCombatAI_back(RE::Actor *a_actor);
		static void GroundCombatAI_left(RE::Actor *a_actor);
		static void GroundCombatAI_right(RE::Actor *a_actor);
		static void MoveControllShout(RE::Actor *a_actor);
		static void MoveControllShout1(RE::Actor *a_actor);
		static void MoveControllShout2(RE::Actor *a_actor);
		static void AddBehavior(RE::Actor *a_actor);
		static bool GetFuzzy(float value, float min, float max);
		static int Random(std::vector<int> List);
		static void MagicSelector(RE::Actor *a_actor, int value);
		static void MagicSelector1(RE::Actor *a_actor);
		static void StartParry(RE::Actor *a_actor);
		static void LandingCombatAI(RE::Actor *a_actor);
		static void LandingCombatAI1(RE::Actor *a_actor);
		static void ControlDistanceAIFly(RE::Actor *a_actor);
		static void ControlDistanceAIFly1(RE::Actor *a_actor);
		static void ControlDistanceAIGround(RE::Actor *a_actor);
		static void StartShout(RE::Actor *a_actor);
		static void TripleShout(RE::Actor *a_actor);
		static void CombatStylePref(RE::Actor *a_actor);
		static void OneMoreTailTurn(RE::Actor *a_actor);

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


