#include "cbase.h"
#include "c_weapon__stubs.h"
#include "c_basehlcombatweapon.h"

#define SECRET_MUZZLE		1

class C_WeaponSecret : public C_BaseHLCombatWeapon
{
	DECLARE_CLASS( C_WeaponSecret, C_BaseHLCombatWeapon );
public:
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	void	ReceiveMessage( int classID, bf_read &msg );
};

STUB_WEAPON_CLASS_IMPLEMENT( weapon_secret, C_WeaponSecret );

IMPLEMENT_CLIENTCLASS_DT( C_WeaponSecret, DT_WeaponSecret, CWeaponSecret )
END_RECV_TABLE()

void C_WeaponSecret::ReceiveMessage( int classID, bf_read &msg )
{
	if ( classID != GetClientClass()->m_ClassID )
	{
		// Message is for subclass
		BaseClass::ReceiveMessage( classID, msg );
		return;
	}

	int messageType = msg.ReadByte();

	C_BasePlayer *pOwner = C_BasePlayer::GetLocalPlayer();
	if ( !pOwner || !pOwner->IsPlayer() )
	{
		BaseClass::ReceiveMessage( classID, msg );
		return;
	}

	switch ( messageType )
	{
	case SECRET_MUZZLE:
		{
			unsigned char nAttachment = msg.ReadByte();

			Vector vecEndPos;
			msg.ReadBitVec3Coord( vecEndPos );
		
			C_BaseViewModel* pViewModel = pOwner->GetViewModel( 0 );

			if ( pViewModel )
			{
				C_BaseAnimating *pAnim = GetBaseAnimating();
				Vector vecStart;
				pAnim->GetAttachment( nAttachment, vecStart );

				CNewParticleEffect *pEffect = ParticleProp()->Create( "instagib_new", PATTACH_POINT_FOLLOW, nAttachment );
				if (pEffect)
				{
					pEffect->SetControlPoint( 0, vecStart );
					pEffect->SetControlPoint( 1, vecEndPos );
				}
			}
		}
		break;
	default:
		AssertMsg1( false, "Received unknown message %d", messageType );
	}
}
