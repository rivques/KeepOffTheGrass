#include "pch.h"
#include "KeepOffTheGrass.h"


std::string KeepOffTheGrass::GetPluginName() {
	return "KeepOffTheGrass";
}

void KeepOffTheGrass::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> KeepOffTheGrass
void KeepOffTheGrass::RenderSettings() {
	ImGui::Checkbox("Enabled", &enabled);
	if (enabled) {
		ImGui::Checkbox("Hurt Blue", &enabledBlue);
		ImGui::Checkbox("Hurt Orange", &enabledOrange);
	}
	ImGui::Separator();
	ImGui::Checkbox("Draw Debug Rendering", &drawDebug);
}


/*
// Do ImGui rendering here
void KeepOffTheGrass::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string KeepOffTheGrass::GetMenuName()
{
	return "KeepOffTheGrass";
}

// Title to give the menu
std::string KeepOffTheGrass::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void KeepOffTheGrass::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool KeepOffTheGrass::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool KeepOffTheGrass::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void KeepOffTheGrass::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void KeepOffTheGrass::OnClose()
{
	isWindowOpen_ = false;
}
*/
