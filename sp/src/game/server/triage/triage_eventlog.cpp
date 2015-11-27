//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "../EventLog.h"
#include "KeyValues.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CTriageEventLog : public CEventLog
{
private:
	typedef CEventLog BaseClass;

public:
	virtual char const *Name() { return "CTriageEventLog"; }

	virtual ~CTriageEventLog() {};

public:
	bool PrintEvent( IGameEvent * event )	// override virtual function
	{
		if ( BaseClass::PrintEvent( event ) )
		{
			return true;
		}
	
		if ( Q_strcmp(event->GetName(), "triage_") == 0 )
		{
			return PrintTriageEvent( event );
		}

		return false;
	}

protected:

	bool PrintTriageEvent( IGameEvent * event )	// print Mod specific logs
	{
	//	const char * name = event->GetName() + Q_strlen("triage_"); // remove prefix

		return false;
	}

};

static CTriageEventLog s_TriageEventLog;

//-----------------------------------------------------------------------------
// Singleton access
//-----------------------------------------------------------------------------
IGameSystem* GameLogSystem()
{
	return &s_TriageEventLog;
}

