//#include "PrecisionAPI.h"
#include "SKSE/Trampoline.h"
#include <SimpleIni.h>
#include <iterator>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#pragma warning(disable: 4100)
#pragma warning(disable : 4189)
//using std::string;
static float& g_deltaTime = (*(float*)RELOCATION_ID(523660, 410199).address());

namespace hooks
{
	// static float& g_deltaTime = (*(float*)RELOCATION_ID(523660, 410199).address());
	using uniqueLocker = std::unique_lock<std::shared_mutex>;
	using sharedLocker = std::shared_lock<std::shared_mutex>;
	using VM = RE::BSScript::Internal::VirtualMachine;
	using StackID = RE::VMStackID;
#define STATIC_ARGS [[maybe_unused]] VM *a_vm, [[maybe_unused]] StackID a_stackID, RE::StaticFunctionTag *
#define PI 3.14159265358979323846f

	using EventResult = RE::BSEventNotifyControl;

	using tActor_IsMoving = bool (*)(RE::Actor* a_this);
	//static REL::Relocation<tActor_IsMoving> IsMoving{ REL::VariantID(36928, 37953, 0x6116C0) };

	typedef float (*tActor_GetReach)(RE::Actor* a_this);
	static REL::Relocation<tActor_GetReach> Actor_GetReach{ RELOCATION_ID(37588, 38538) };

	union ConditionParam
	{
		char c;
		std::int32_t i;
		float f;
		RE::TESForm *form;
	};

	bool GetshouldHelp(const RE::Actor *p_ally, const RE::Actor *a_actor);
	void StartCombat(const RE::Actor *subject, const RE::Actor *target);
	bool Has_Magiceffect_Keyword(const RE::Actor *a_actor, const RE::BGSKeyword *a_key);
	bool HasBoundWeaponEquipped(const RE::Actor *a_actor, RE::MagicSystem::CastingSource type);

	class animEventHandler
	{
	private:
		template <class Ty>
		static Ty SafeWrite64Function(uintptr_t addr, Ty data)
		{
			DWORD oldProtect;
			void* _d[2];
			memcpy(_d, &data, sizeof(data));
			size_t len = sizeof(_d[0]);

			VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
			Ty olddata;
			memset(&olddata, 0, sizeof(Ty));
			memcpy(&olddata, (void*)addr, len);
			memcpy((void*)addr, &_d[0], len);
			VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
			return olddata;
		}

		typedef RE::BSEventNotifyControl (animEventHandler::*FnProcessEvent)(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* dispatcher);

		RE::BSEventNotifyControl HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src);

		static void HookSink(uintptr_t ptr)
		{
			FnProcessEvent fn = SafeWrite64Function(ptr + 0x8, &animEventHandler::HookedProcessEvent);
			fnHash.insert(std::pair<uint64_t, FnProcessEvent>(ptr, fn));
		}

	public:
		static animEventHandler* GetSingleton()
		{
			static animEventHandler singleton;
			return &singleton;
		}

		/*Hook anim event sink*/
		static void Register(bool player, bool NPC)
		{
			if (player) {
				logger::info("Sinking animation event hook for player");
				REL::Relocation<uintptr_t> pcPtr{ RE::VTABLE_PlayerCharacter[2] };
				HookSink(pcPtr.address());
			}
			if (NPC) {
				logger::info("Sinking animation event hook for NPC");
				REL::Relocation<uintptr_t> npcPtr{ RE::VTABLE_Character[2] };
				HookSink(npcPtr.address());
			}
			logger::info("Sinking complete.");
		}

		static void RegisterForPlayer()
		{
			Register(true, false);
		}

	protected:
		static std::unordered_map<uint64_t, FnProcessEvent> fnHash;
	};

	class OnMeleeHitHook
	{
	public:

		static OnMeleeHitHook* GetSingleton()
		{
			static OnMeleeHitHook avInterface;
			return &avInterface;
		}

		static void install();
		static void install_pluginListener();
		static void install_protected(){
			Install_Update();
		}
		void init();

		static bool BindPapyrusFunctions(VM* vm);
		static void Set_iFrames(RE::Actor* actor);
		static void Reset_iFrames(RE::Actor* actor);
		static void dispelEffect(RE::MagicItem *spellForm, RE::Actor *a_target);

		static void InterruptAttack(RE::Actor *a_actor);

		static void EquipfromInvent(RE::Actor *a_actor, RE::FormID a_formID);

		static bool isPowerAttacking(RE::Actor *a_actor);
		static bool IsCasting(RE::Actor *a_actor);
		static void UpdateCombatTarget(RE::Actor* a_actor);
		static bool isHumanoid(RE::Actor *a_actor);
		static std::vector<RE::TESForm*> GetEquippedForm(RE::Actor* actor);
		static bool IsWeaponOut(RE::Actor* actor);
		void Update(RE::Actor* a_actor, float a_delta);
		float AV_Mod(RE::Actor *a_actor, int a_aggression, float input, float mod);
		int GenerateRandomInt(int value_a, int value_b);
	    float GenerateRandomFloat(float value_a, float value_b);
		static bool IsMeleeOnly(RE::Actor *a_actor);
		static void Patch_Spell_List();
		void UnequipAll(RE::Actor* a_actor);
		void Re_EquipAll(RE::Actor *a_actor);
		static std::pair<bool, RE::EffectSetting *> GetEffect_FF_Aimed(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_FF_TA(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_FF_Self(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_CC_TA(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_CC_Aimed(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_CC_Self(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_FF_TL(RE::SpellItem *a_spell);
		static std::pair<bool, RE::EffectSetting *> GetEffect_CC_TL(RE::SpellItem *a_spell);
		static void Execute_Cast(RE::Actor *a_actor, RE::SpellItem *a_spell, bool lefthand, bool FF_aimed = false, bool FF_TA = false, bool FF_TL = false, bool CC_aimed = false, bool CC_TA = false, bool CC_TL = false);
		static void GetAttackSpell(RE::Actor *a_actor, bool lefthand = false);
		RE::BGSAttackData *get_attackData(RE::Actor *a);
		static void Scan_Teammates_NeedHealing(RE::Actor *a_actor);
		static void Scan_Teammates_HasWeapon(RE::Actor *a_actor);
		static void Scan_Teammates_NeedBoundWeapon(RE::Actor *a_actor);
		static void Scan_Teammates_NeedWard(RE::Actor *a_actor);
		static void Scan_Teammates_NeedInvisibility(RE::Actor *a_actor);
		static void Scan_Teammates_NeedSurvival(RE::Actor *a_actor);
		static void Scan_Teammates_NeedCloak(RE::Actor *a_actor);
		static void Scan_Teammates_NeedArmour(RE::Actor *a_actor);
		static void Scan_Teammates_Undead(RE::Actor *a_actor);
		static void Scan_Teammates_CommandedActor(RE::Actor *a_actor);
		static void Cast_PotentialRune(RE::Actor *actor, bool lefthand = false);
		static void Cast_Rune(RE::Actor *actor, RE::SpellItem *a_spell, RE::BGSProjectile* a_projectile, bool lefthand);
		void scan_activeRunes(RE::Actor *a_actor, RE::Projectile *a_rune, bool insert = false, bool clear = false, bool clear_all = false);
		float confidence_threshold(RE::Actor *a_actor, int confidence, bool inverse = false);
		float get_personal_threatRatio(RE::Actor *protagonist, RE::Actor *combat_target);
		float get_personal_survivalRatio(RE::Actor *protagonist, RE::Actor *combat_target);

		struct PolarAngle
		{
			float alpha;
			operator float() const { return alpha; }
			PolarAngle(float x = 0.0f) : alpha(x)
			{
				while (alpha > 360.0f)
					alpha -= 360.0f;
				while (alpha < 0.0f)
					alpha += 360.0f;
			}
			PolarAngle(const RE::NiPoint3 &p)
			{
				const float y = p.y;
				if (y == 0.0)
				{
					if (p.x <= 0.0)
						alpha = PI * 1.5f;
					else
						alpha = PI * 0.5f;
				}
				else
				{
					alpha = atanf(p.x / y);
					if (y < 0.0)
						alpha += PI;
				}
				alpha = alpha * 180.0f / PI;
			}
			PolarAngle add(const PolarAngle &r) const
			{
				float ans = alpha + r.alpha;
				if (ans >= 360.0f)
					return {ans - 360.0f};
				else
					return {ans};
			}
			PolarAngle sub(const PolarAngle &r) const
			{
				return this->add({360.0f - r.alpha});
			}
			float to_normangle() const
			{
				if (alpha > 180.0f)
					return alpha - 360.0f;
				else
					return alpha;
			}
			float to_normangle_abs() const
			{
				return abs(to_normangle());
			}
			static bool ordered(PolarAngle alpha, PolarAngle beta, PolarAngle gamma)
			{
				gamma = gamma.sub(alpha);
				beta = beta.sub(alpha);
				return gamma >= beta;
			}
			static float dist(float r, PolarAngle alpha)
			{
				auto ans = r * (alpha) / 180.0f * PI;
				return ans;
			}
		};

		float get_angle_he_me(RE::Actor *me, RE::Actor *he, RE::BGSAttackData *attackdata);

		template <class T>
		static std::vector<T*> get_all(const std::vector<RE::BGSKeyword*>& a_keywords)
		{
			std::vector<T*> result;

			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				for (const auto& form : dataHandler->GetFormArray<T>()) {
					if (!form || !a_keywords.empty() && !form->HasKeywordInArray(a_keywords, false)) {
						continue;
					}
					result.push_back(form);
				}
			}

			return result;
		}

		template <class T>
		static std::vector<T*> get_in_mod(const RE::TESFile* a_modInfo, const std::vector<RE::BGSKeyword*>& a_keywords)
		{
			std::vector<T*> result;

			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				for (const auto& form : dataHandler->GetFormArray<T>()) {
					if (!form || !a_modInfo->IsFormInMod(form->formID) || !a_keywords.empty() && !form->HasKeywordInArray(a_keywords, false)) {
						continue;
					}
					result.push_back(form);
				}
			}

			return result;
		}

		static std::vector<const RE::TESFile*> LookupMods(const std::vector<std::string>& modInfo_List)
		{
			std::vector<const RE::TESFile*> result;

			for (auto limbo_mod : modInfo_List) {
				if (!limbo_mod.empty()){
					const auto dataHandler = RE::TESDataHandler::GetSingleton();
					const auto modInfo = dataHandler ? dataHandler->LookupModByName(limbo_mod) : nullptr;

					if (modInfo){
						result.push_back(modInfo);
					}
				}
			}

			return result;
		}

		static std::vector<RE::BGSKeyword*> LookupKeywords(const std::vector<std::string>& keyword_List)
		{
			std::vector<RE::BGSKeyword*> result;

			for (auto limbo_key : keyword_List) {
				if (!limbo_key.empty()) {
					const auto key = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(limbo_key);

					if (key) {
						result.push_back(key);
					}
				}
			}

			return result;
		}

		template <class T>
		static std::vector<T*> get_valid_spellList(const std::vector<const RE::TESFile*>& exclude_modInfo_List, const std::vector<RE::BGSKeyword*>& exclude_keywords, bool whiteList_approach)
		{
			std::vector<T*> result;

			if (whiteList_approach){
				if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler)
				{
					for (const auto &form : dataHandler->GetFormArray<T>())
					{
						if (!form)
						{
							continue;
						}
						bool valid = false;
						for (const auto a_modInfo : exclude_modInfo_List)
						{
							if (a_modInfo && a_modInfo->IsFormInMod(form->formID))
							{
								valid = true;
								break;
							}
						}
						if (form->HasKeywordInArray(exclude_keywords, false))
						{
							valid = true;
						}
						if (valid)
						{
							result.push_back(form);
						}
					}
				}
			}else{

				if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler)
				{
					for (const auto &form : dataHandler->GetFormArray<T>())
					{
						if (!form)
						{
							continue;
						}
						bool invalid = false;
						for (const auto a_modInfo : exclude_modInfo_List)
						{
							if (a_modInfo && a_modInfo->IsFormInMod(form->formID))
							{
								invalid = true;
								break;
							}
						}
						if (form->HasKeywordInArray(exclude_keywords, false))
						{
							invalid = true;
						}
						if (!invalid)
						{
							result.push_back(form);
						}
					}
				}
			}

			return result;
		}

	private:
		OnMeleeHitHook() = default;
		OnMeleeHitHook(const OnMeleeHitHook&) = delete;
		OnMeleeHitHook(OnMeleeHitHook&&) = delete;
		~OnMeleeHitHook() = default;

		OnMeleeHitHook& operator=(const OnMeleeHitHook&) = delete;
		OnMeleeHitHook& operator=(OnMeleeHitHook&&) = delete;

		std::random_device rd;
		//PRECISION_API::IVPrecision1* _precision_API;
		//static void PrecisionWeaponsCallback_Post(const PRECISION_API::PrecisionHitData& a_precisionHitData, const RE::HitData& a_hitdata);
		std::unordered_map<RE::Actor*, std::vector<RE::TESBoundObject*>> _Inventory;
		std::unordered_map<RE::Actor *, std::vector<RE::Projectile *>> _RunesCast;
		std::shared_mutex mtx_RunesCast;
		std::shared_mutex mtx_Inventory;

	protected:

		struct Actor_Update
		{
			static void thunk(RE::Actor* a_actor, float a_delta)
			{
				func(a_actor, a_delta);
				GetSingleton()->Update(a_actor, g_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install_Update(){
			stl::write_vfunc<RE::Character, 0xAD, Actor_Update>();
		}

	};

	class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
	{
	public:

		static InputEventHandler*	GetSingleton();

		virtual EventResult			ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

		static void SinkEventHandlers();

	private:
		enum : uint32_t
		{
			kInvalid = static_cast<uint32_t>(-1),
			kKeyboardOffset = 0,
			kMouseOffset = 256,
			kGamepadOffset = 266
		};

		InputEventHandler() = default;
		InputEventHandler(const InputEventHandler&) = delete;
		InputEventHandler(InputEventHandler&&) = delete;
		virtual ~InputEventHandler() = default;

		InputEventHandler& operator=(const InputEventHandler&) = delete;
		InputEventHandler& operator=(InputEventHandler&&) = delete;

		std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);
	};


	class util
	{
	private:
		static int soundHelper_a(void *manager, RE::BSSoundHandle *a2, int a3, int a4) // sub_140BEEE70
		{
			using func_t = decltype(&soundHelper_a);
			REL::Relocation<func_t> func{RELOCATION_ID(66401, 67663)};
			return func(manager, a2, a3, a4);
		}

		static void soundHelper_b(RE::BSSoundHandle *a1, RE::NiAVObject *source_node) // sub_140BEDB10
		{
			using func_t = decltype(&soundHelper_b);
			REL::Relocation<func_t> func{RELOCATION_ID(66375, 67636)};
			return func(a1, source_node);
		}

		static char __fastcall soundHelper_c(RE::BSSoundHandle *a1) // sub_140BED530
		{
			using func_t = decltype(&soundHelper_c);
			REL::Relocation<func_t> func{RELOCATION_ID(66355, 67616)};
			return func(a1);
		}

		static char set_sound_position(RE::BSSoundHandle *a1, float x, float y, float z)
		{
			using func_t = decltype(&set_sound_position);
			REL::Relocation<func_t> func{RELOCATION_ID(66370, 67631)};
			return func(a1, x, y, z);
		}

		std::random_device rd;

	public:
		static void playSound(RE::Actor *a, RE::BGSSoundDescriptorForm *a_descriptor)
		{
			//logger::info("starting voicing....");

			RE::BSSoundHandle handle;
			handle.soundID = static_cast<uint32_t>(-1);
			handle.assumeSuccess = false;
			*(uint32_t *)&handle.state = 0;

			soundHelper_a(RE::BSAudioManager::GetSingleton(), &handle, a_descriptor->GetFormID(), 16);

			if (set_sound_position(&handle, a->data.location.x, a->data.location.y, a->data.location.z))
			{
				soundHelper_b(&handle, a->Get3D());
				soundHelper_c(&handle);
				//logger::info("FormID {}"sv, a_descriptor->GetFormID());
				//logger::info("voicing complete");
			}
		}

		static RE::BGSSoundDescriptor *GetSoundRecord(const char* description)
		{

			auto Ygr = RE::TESForm::LookupByEditorID<RE::BGSSoundDescriptor>(description);

			return Ygr;
		}

		static util *GetSingleton()
		{
			static util singleton;
			return &singleton;
		}

		float GenerateRandomFloat(float value_a, float value_b)
		{
			std::mt19937 generator(rd());
			std::uniform_real_distribution<float> dist(value_a, value_b);
			return dist(generator);
		}
	};

	class Settings
	{
	public:
		static Settings* GetSingleton()
		{
			static Settings avInterface;
			return &avInterface;
		}

		void Load();

		struct General_Settings
		{
			void Load(CSimpleIniA &a_ini);
			bool bWhiteListApproach = false;

		} general;

		struct Exclude_AllSpells_inMods
		{
			void Load(CSimpleIniA& a_ini);

			std::string exc_mods_joined = "Heroes of Yore.esp|VampireLordSeranaAssets.esp|VampireLordSerana.esp|TheBeastWithin.esp|TheBeastWithinHowls.esp";

			std::vector<std::string> exc_mods;

		} exclude_spells_mods;

		struct Exclude_AllSpells_withKeywords
		{
			void Load(CSimpleIniA& a_ini);
			std::string exc_keywords_joined = "HoY_MagicShoutSpell|LDP_MagicShoutSpell|NSV_CActorSpell_Exclude";

			std::vector<std::string> exc_keywords;

		} exclude_spells_keywords;

		struct Include_AllSpells_inMods
		{
			void Load(CSimpleIniA &a_ini);

			std::string inc_mods_joined = "Skyrim.esm|Dawnguard.esm|Dragonborn.esm";

			std::vector<std::string> inc_mods;

		} include_spells_mods;

		struct Include_AllSpells_withKeywords
		{
			void Load(CSimpleIniA &a_ini);
			std::string inc_keywords_joined = "DummyKey|ImposterKey";

			std::vector<std::string> inc_keywords;

		} include_spells_keywords;

	private:
		Settings() = default;
		Settings(const Settings&) = delete;
		Settings(Settings&&) = delete;
		~Settings() = default;

		Settings& operator=(const Settings&) = delete;
		Settings& operator=(Settings&&) = delete;
	};
};

constexpr uint32_t hash(const char* data, size_t const size) noexcept
{
	uint32_t hash = 5381;

	for (const char* c = data; c < data + size; ++c) {
		hash = ((hash << 5) + hash) + (unsigned char)*c;
	}

	return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
	return hash(str, size);
}
