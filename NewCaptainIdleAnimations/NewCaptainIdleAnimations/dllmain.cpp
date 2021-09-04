// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore/Anim/IAnimManager.h>
#include <Spore/Anim/AnimatedCreature.h>
#include <Spore/Simulator/cCreatureBase.h>

void Initialize()
{
	// This method is executed when the game starts, before the user interface is shown
	// Here you can do things such as:
	//  - Add new cheats
	//  - Add new simulator classes
	//  - Add new game modes
	//  - Add new space tools
	//  - Change materials
}

void Dispose()
{
	// This method is called when the game is closing
}
using namespace Anim;



virtual_detour(idlelookaround_detour, AnimatedCreature, AnimatedCreature, void(uint32_t, int*)) {    // Changes the animation being loaded while the avatar is idle, provided certain conditions are met.
	void detoured(uint32_t animID, int* pChoice) {
		auto avatar = GameNounManager.GetAvatar();
		if (avatar && this == avatar->mpAnimatedCreature.get() && animID == 0x027844AD && (Simulator::IsScenarioMode() || Simulator::IsSpaceGame())) {    // Checks that the object requesting the animation is the avatar, 
																																						  // that the animation ID is for "idle_lookaround", and that the current stage is either Space or GA. 
			return original_function(this, 0xe95aea61, pChoice);		// If the conditions are met, load the custom animation ID from "lisk_newcaptainidle.tlsa".
		}
		else {
			return original_function(this, animID, pChoice);			// Otherwise, return the original ID and load the animation as usual.
		} 
	}

};

/* Unused function. Might use it for the future. Is otherwise the same as the above detour but for a different function, AnimatedCreature::PlayAnimation.
virtual_detour(idlelookaround_detour_play, AnimatedCreature, AnimatedCreature, void(uint32_t, int*)) {
	void detoured(uint32_t animID, int* pChoice) {
		if (this == GameNounManager.GetAvatar()->mpAnimatedCreature.get() && animID == 0x027844AD && (Simulator::IsScenarioMode() || Simulator::IsSpaceGame())) {
			return original_function(this, 0xe95aea61, pChoice);
		}
		else {
			return original_function(this, animID, pChoice);
		}
	}

};*/

void AttachDetours()
{
	idlelookaround_detour::attach(Address(ModAPI::ChooseAddress(0xA0C5D0, 0xA0C5D0)));		  // Attach the AnimatedCreature::LoadAnimation detour
//	idlelookaround_detour_play::attach(Address(ModAPI::ChooseAddress(0xA0C5A0, 0xA0C5A0)));   // Attach the AnimatedCreature::PlayAnimation detour 
	// Call the attach() method on any detours you want to add
	// For example: cViewer_SetRenderType_detour::attach(GetAddress(cViewer, SetRenderType));

}




// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

