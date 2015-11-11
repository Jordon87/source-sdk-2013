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

class C1187EventLog : public CEventLog
{
private:
	typedef CEventLog BaseClass;

public:
	virtual char const *Name() { return "C1187EventLog"; }

	virtual ~C1187EventLog() {};

public:
	bool PrintEvent( IGameEvent * event )	// override virtual function
	{
		if ( BaseClass::PrintEvent( event ) )
		{
			return true;
		}
	
		if ( Q_strcmp(event->GetName(), "1187_") == 0 )
		{
			return Print1187Event( event );
		}

		return false;
	}

protected:

	bool Print1187Event( IGameEvent * event )	// print Mod specific logs
	{
	//	const char * name = event->GetName() + Q_strlen("1187_"); // remove prefix

		return false;
	}

};

static C1187EventLog s_1187EventLog;

//-----------------------------------------------------------------------------
// Singleton access
//-----------------------------------------------------------------------------
IGameSystem* GameLogSystem()
{
	return &s_1187EventLog;
}

