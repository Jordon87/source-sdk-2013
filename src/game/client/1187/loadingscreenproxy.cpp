#include "cbase.h"
#include "materialsystem/imaterialproxy.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar loadscreen("loadscreen","0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );
ConVar loadscreen_max("loadscreen_max","1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );

static void cc_loadscreen_force()
{
	random->SetSeed(gpGlobals->curtime);
	loadscreen.SetValue(RandomInt(0, loadscreen_max.GetInt()));
}

static ConCommand loadscreen_force("loadscreen_force", cc_loadscreen_force, "", FCVAR_CLIENTDLL);

class CLoadingScreenProxy : public IMaterialProxy
{
public:
	CLoadingScreenProxy();
	virtual ~CLoadingScreenProxy();
	virtual bool Init(IMaterial* pMaterial, KeyValues* pKeyValues);
	virtual void OnBind(void *pEnt);
	virtual void Release(void) { delete this; }
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
	m_LoadingScreenVar = NULL;
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

void CLoadingScreenProxy::OnBind(void* pEnt)
{
	ITexture* pTexture;

	char loadingscreenbuffer[128];
	if (m_LoadingScreenVar)
	{
		V_snprintf(loadingscreenbuffer, sizeof(loadingscreenbuffer), "VGUI/loading/screen%d", loadscreen.GetInt());

		pTexture = materials->FindTexture(loadingscreenbuffer, 0, false);
	
		if (!pTexture)
			pTexture = materials->FindTexture("VGUI/loading/error", 0, false);
	
		m_LoadingScreenVar->SetTextureValue(pTexture);
	}
}

IMaterial* CLoadingScreenProxy::GetMaterial()
{
	return m_LoadingScreenVar->GetOwningMaterial();;
}

EXPOSE_INTERFACE(CLoadingScreenProxy, IMaterialProxy, "1187Loading" IMATERIAL_PROXY_INTERFACE_VERSION);