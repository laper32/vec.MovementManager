#include "extension.h"

// We uses these functions below here:
// - GetPlayerMaxSpeed
// - IsInWorld
// - SetLocalVelocity
// - ApplyAbsVelocityImpulse

float GetPlayerMaxSpeed(CBasePlayer* player);
void SetLocalVelocity(CBaseEntity* entity);
bool IsInWorld(CBaseEntity* entity);
void ApplyAbsVelocityImpulse(CBaseEntity* entity);

Extension g_Extension;
SMEXT_LINK(&g_Extension);

CGlobalVars* gpGlobals = nullptr;
IGameConfig* g_pGameConf = nullptr;
inline IBinTools* g_pBinTools = nullptr;
inline ISDKTools* g_pSDKTools = nullptr;

inline int FindOffset(const char* name)
{
	int offset;
	if (!g_pGameConf->GetOffset(name, &offset))
		throw std::runtime_error("hook : sig not found - " + std::string(name));
	return offset;
}
inline void* FindSig(const char* name)
{
	void* addr;
	if (!g_pGameConf->GetMemSig(name, &addr))
		throw std::runtime_error("hook : sig not found - " + std::string(name));
	return addr;
}
sp_nativeinfo_t g_Natives[] = {
	{nullptr, nullptr}
};

bool Extension::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	// Load SDK
	sm::SDK_OnLoad(error, maxlength, late);

	// Start what we want to do
	char conf_error[256] = {};
	if (!gameconfs->LoadGameConfigFile("MovementManager.games", &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		smutils->LogError(myself, "Unable to find MovementManager.games, diasabling this module.");
		return false;
	}
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf, conf_error, sizeof(conf_error)))
		return false;
	sharesys->AddDependency(myself, "bintools.ext", true, true);
	sharesys->AddDependency(myself, "sdktools.ext", true, true);

	SM_GET_IFACE(BINTOOLS, g_pBinTools);
	SM_GET_IFACE(SDKTOOLS, g_pSDKTools);
	
	// retrving convar here...
	if (!sm::icvar->FindVar("sv_clamp_unsafe_velocities")->IsRegistered())
	{
		smutils->LogError(myself, "Cannot find sv_clamp_unsafe_velocities, disabling.");
		return false;
	}

	sm::icvar->FindVar("sv_clamp_unsafe_velocities")->SetValue(0);

	return SDKExtension::SDK_OnLoad(error, maxlength, late);
}

void Extension::SDK_OnUnload()
{
	sm::SDK_OnUnload();
	gameconfs->CloseGameConfigFile(g_pGameConf);
	return SDKExtension::SDK_OnUnload();
}

bool Extension::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	sm::SDK_OnMetamodLoad(ismm, error, maxlen, late);
	return SDKExtension::SDK_OnMetamodLoad(ismm, error, maxlen, late);
}

float GetPlayerMaxSpeed(CBasePlayer* player)
{
	static sm::VFuncCaller<float(CBasePlayer::*)()> caller(g_pBinTools, FindOffset("GetPlayerMaxSpeed"));
	return caller(player);
}
void SetLocalVelocity(CBaseEntity* entity)
{
	static sm::MemFuncCaller<void(CBaseEntity::*)()> caller(g_pBinTools, FindSig("SetLocalVelocity"));
	return caller(entity);
}
bool IsInWorld(CBaseEntity* entity)
{
	static sm::MemFuncCaller<bool(CBaseEntity::*)()> caller(g_pBinTools, FindSig("SetLocalVelocity"));
	return caller(entity);
}
void ApplyAbsVelocityImpulse(CBaseEntity* entity)
{
	static sm::MemFuncCaller<void(CBaseEntity::*)()> caller(g_pBinTools, FindSig("ApplyAbsVelocityImpulse"));
	return caller(entity);
}
namespace API
{
	static cell_t GetBasicSpeed(IPluginContext* pContext, const cell_t* params)
	{

	}
	static cell_t SetBasicSpeed(IPluginContext* pContext, const cell_t* params)
	{

	}
	static cell_t GetMaxSpeed(IPluginContext* pContext, const cell_t* params)
	{
		return GetPlayerMaxSpeed(sm::ent_cast<CBasePlayer*>(params[1]));
	}
	static cell_t SetMaxSpeed(IPluginContext* pContext, const cell_t* params)
	{

	}
}