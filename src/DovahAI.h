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
		static void DeathRadollCrashLand(RE::Actor *a_actor);

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


