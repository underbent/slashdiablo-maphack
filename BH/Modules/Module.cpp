#include "Module.h"
#include "../BH.h"

Module::Module(string name) : name(name), active(false) {
	BH::moduleManager->Add(this);
}

Module::~Module() {
	//Unload();
	//BH::moduleManager->Remove(this);
}

void Module::Load() {
	if (IsActive())
		return;

	// Hook up all the events
	
	__hook(&ModuleManager::OnDraw, BH::moduleManager, &Module::OnDraw, this);
	__hook(&ModuleManager::OnAutomapDraw, BH::moduleManager, &Module::OnAutomapDraw, this);
	__hook(&ModuleManager::OnOOGDraw, BH::moduleManager, &Module::OnOOGDraw, this);

	__hook(&ModuleManager::OnGameJoin, BH::moduleManager, &Module::OnGameJoin, this);
	__hook(&ModuleManager::OnGameExit, BH::moduleManager, &Module::OnGameExit, this);

	__hook(&ModuleManager::OnLoop, BH::moduleManager, &Module::OnLoop, this);

	__hook(&ModuleManager::OnLeftClick, BH::moduleManager, &Module::OnLeftClick, this);
	__hook(&ModuleManager::OnRightClick, BH::moduleManager, &Module::OnRightClick, this);
	__hook(&ModuleManager::OnKey, BH::moduleManager, &Module::OnKey, this);

	__hook(&ModuleManager::OnChatPacketRecv, BH::moduleManager, &Module::OnChatPacketRecv, this);
	__hook(&ModuleManager::OnRealmPacketRecv, BH::moduleManager, &Module::OnRealmPacketRecv, this);
	__hook(&ModuleManager::OnGamePacketRecv, BH::moduleManager, &Module::OnGamePacketRecv, this);

	__hook(&ModuleManager::OnChatMsg, BH::moduleManager, &Module::OnChatMsg, this);
	__hook(&Module::UserInput, this, &Module::OnUserInput, this);

	active = true;
	OnLoad();
}

void Module::Unload() {
	if (!IsActive())
		return;

	// Unhook all events
	__unhook(&ModuleManager::OnDraw, BH::moduleManager, &Module::OnDraw, this);
	__unhook(&ModuleManager::OnAutomapDraw, BH::moduleManager, &Module::OnAutomapDraw, this);
	__unhook(&ModuleManager::OnOOGDraw, BH::moduleManager, &Module::OnOOGDraw, this);

	__unhook(&ModuleManager::OnGameJoin, BH::moduleManager, &Module::OnGameJoin, this);
	__unhook(&ModuleManager::OnGameExit, BH::moduleManager, &Module::OnGameExit, this);

	__unhook(&ModuleManager::OnLoop, BH::moduleManager, &Module::OnLoop, this);

	__unhook(&ModuleManager::OnLeftClick, BH::moduleManager, &Module::OnLeftClick, this);
	__unhook(&ModuleManager::OnRightClick, BH::moduleManager, &Module::OnRightClick, this);
	__unhook(&ModuleManager::OnKey, BH::moduleManager, &Module::OnKey, this);

	__unhook(&ModuleManager::OnChatPacketRecv, BH::moduleManager, &Module::OnChatPacketRecv, this);
	__unhook(&ModuleManager::OnRealmPacketRecv, BH::moduleManager, &Module::OnRealmPacketRecv, this);
	__unhook(&ModuleManager::OnGamePacketRecv, BH::moduleManager, &Module::OnGamePacketRecv, this);

	__unhook(&ModuleManager::OnChatMsg, BH::moduleManager, &Module::OnChatMsg, this);
	//__unhook(&Module::UserInput, this, &Module::OnUserInput, this);

	active = false;
	OnUnload();
}