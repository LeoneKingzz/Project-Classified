#include "hook.h"

void MessageHandler(SKSE::MessagingInterface::Message *a_msg)
{
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:
		hooks::animEventHandler::Register(false, true);
		hooks::OnMeleeHitHook::install();
		// hooks::InputEventHandler::SinkEventHandlers();
		//hooks::Settings::GetSingleton()->Load();
		//hooks::OnMeleeHitHook::Patch_Spell_List();

		break;

	default:

		break;
	}
}

void Init()
{
	hooks::OnMeleeHitHook::install_pluginListener();
	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", MessageHandler);
}

void Load(){
	//hooks::OnMeleeHitHook::install_protected();
}