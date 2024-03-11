#include "cbase.h"
#include "proxyentity.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/imaterialsystem.h"
#include "KeyValues.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar loadscreen("loadscreen","0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE | FCVAR_HIDDEN);
ConVar loadscreen_max("loadscreen_max","1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE | FCVAR_HIDDEN);

void cc_loadscreen_force()
{
	random->SetSeed(gpGlobals->curtime);
	loadscreen.SetValue(RandomInt(0, loadscreen_max.GetInt()));
}

class CLoadingScreenProxy : public CEntityMaterialProxy
{
public:
	CLoadingScreenProxy();
	virtual ~CLoadingScreenProxy();
	virtual bool Init(IMaterial* pMaterial, KeyValues* pKeyValues);
	virtual void OnBind(C_BaseEntity* pEnt);

	virtual IMaterial* GetMaterial();
private:
	IMaterialVar* m_LoadingScreenVar;
};

CLoadingScreenProxy::CLoadingScreenProxy()
{
	m_LoadingScreenVar = NULL;
}

CLoadingScreenProxy::~CLoadingScreenProxy()
{
}

bool CLoadingScreenProxy::Init(IMaterial* pMaterial, KeyValues* pKeyValues)
{
	char const* pTextureVarName = pKeyValues->GetString("texture");
	if (!pTextureVarName)
		return false;
	
	bool foundVar;
	m_LoadingScreenVar = pMaterial->FindVar(pTextureVarName, &foundVar, false);
	if (!foundVar)
		return false;

	return true;
}

void CLoadingScreenProxy::OnBind(C_BaseEntity* pEnt)
{
	IMaterial* pMaterial = NULL;

	char loadingscreenbuffer[128];
	if (m_LoadingScreenVar)
	{
		V_snprintf(loadingscreenbuffer, sizeof(loadingscreenbuffer), "VGUI/loading/screen%d", loadscreen.GetInt());

		pMaterial = materials->FindMaterial(loadingscreenbuffer, false);
	
		if (!pMaterial)
			pMaterial = materials->FindMaterial("VGUI/loading/error", false);
	
		m_LoadingScreenVar->SetMaterialValue(pMaterial);
	}
}

IMaterial* CLoadingScreenProxy::GetMaterial()
{
	if (!m_LoadingScreenVar)
		return NULL;

	return m_LoadingScreenVar->GetOwningMaterial();
}

EXPOSE_INTERFACE(CLoadingScreenProxy, IMaterialProxy, "1187Loading" IMATERIAL_PROXY_INTERFACE_VERSION);