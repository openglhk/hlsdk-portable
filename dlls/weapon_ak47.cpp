#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CAK47 : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    BOOL Deploy(void) override;
	int  Save( CSave *save ) override;
	int  Restore( CRestore *restore ) override;
    float GetMaxSpeed( void ) override;
	int  iItemSlot( void ) override;
    void PrimaryAttack(void) override;
	void SecondaryAttack( void ) override;
	BOOL UseDecrement( void ) override;
    void Reload(void) override;
    void WeaponIdle(void) override;
public:
    int            iShellOn;
    int            m_iShell;
    unsigned short m_usFireAK47;
    int            m_iPrimaryAmmoType;
public:
    static TYPEDESCRIPTION m_SaveData[];
};

TYPEDESCRIPTION CAK47::m_SaveData[] =
{
    DEFINE_FIELD( CAK47, iShellOn, FIELD_INTEGER ),
};

int CAK47::Save( CSave *save )
{
    if ( !CBasePlayerWeapon::Save( save ) )
    {
        return 0;
    }

    return save->WriteFields( "CAK47", this, m_SaveData, ARRAYSIZE( m_SaveData ) );
}

int CAK47::Restore( CRestore *restore )
{
    if ( !CBasePlayerWeapon::Restore( restore ) )
    {
        return 0;
    }

    return restore->ReadFields( "CAK47", this, m_SaveData, ARRAYSIZE( m_SaveData ) );
}

void CAK47::Precache( void )
{
    PRECACHE_MODEL( "models/v_ak47.mdl" );
    PRECACHE_MODEL( "models/w_ak47.mdl" );
    PRECACHE_MODEL( "models/p_ak47.mdl" );

    PRECACHE_SOUND( "weapons/ak47-1.wav" );
    PRECACHE_SOUND( "weapons/ak47-2.wav" );
    PRECACHE_SOUND( "weapons/ak47_clipout.wav" );
    PRECACHE_SOUND( "weapons/ak47_clipin.wav" );
    PRECACHE_SOUND( "weapons/ak47_boltpull.wav" );

    m_iShell = PRECACHE_MODEL( "models/rshell.mdl" );
    m_usFireAK47 = PRECACHE_EVENT( 1, "events/ak47.sc" );
}

BOOL CAK47::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;

    BOOL bHasAmmo = TRUE;
    if (!HasAmmo())
    {
        bHasAmmo = FALSE;
    }

    return DefaultDeploy("models/v_ak47.mdl", nullptr, 2, "ak47", bHasAmmo);
}

int CAK47::GetItemInfo( ItemInfo *p )
{
    p->pszName = STRING( pev->classname );
    p->pszAmmo1 = "762Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 1;
    p->m_iId = 21;
    p->iId = 21;
    p->iWeight = 25;
    p->iFlags = 0;
    p->bIsAccessory = 0;

    return 1;
}

float CAK47::GetMaxSpeed( void )
{
    return 199.0f;
}

int CAK47::iItemSlot( void )
{
    return 1;
}

void CAK47::AK47Fire( float flSpread, float flCycleTime, BOOL fUseAutoAim )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    m_bDelayFire = true;
    m_iShotsFired++;

    float flIdealAccuracy = (float)( ( m_iShotsFired * m_iShotsFired * m_iShotsFired ) / 200 ) + 0.35f;
    if ( flIdealAccuracy > 1.25f )
    {
        flIdealAccuracy = 1.25f;
    }
    m_flAccuracy = flIdealAccuracy;

    if ( m_iClip < 1 )
    {
        if ( m_fFireOnEmpty != 0 )
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
            return;
        }
    }
    else
    {
        m_iClip--;

        entvars_t *pPlayerPev = m_pPlayer->pev;
        pPlayerPev->effects |= EF_MUZZLEFLASH;

        m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

        Vector vecAngles = pPlayerPev->v_angle + pPlayerPev->punchangle;
        UTIL_MakeVectors( vecAngles );

        Vector vecSrc = m_pPlayer->GetGunPosition();
        Vector vecAiming;
        m_pPlayer->GetAutoaimVector( vecAiming, 0.17364818f );

        FireBullets3( vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_762MM, 30, 0.98f, pPlayerPev, m_pPlayer->random_seed );

        int iPunchX = (int)( pPlayerPev->punchangle.x * 100.0f );
        int iPunchY = (int)( pPlayerPev->punchangle.y * 100.0f );

        PLAYBACK_EVENT_FULL( 0, m_pPlayer->edict(), m_usFireAK47, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, vecAngles.x, iPunchX, iPunchY, 0, 0 );

        m_pPlayer->m_iWeaponVolume = 1500;
        m_pPlayer->m_iWeaponFlash  = 512;

        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
        m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

        if ( m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
        {
            m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
        }

        float flVelocityX = pPlayerPev->velocity.x;
        float flVelocityY = pPlayerPev->velocity.y;
        float flSpeed = sqrtf( ( flVelocityX * flVelocityX ) + ( flVelocityY * flVelocityY ) );

        if ( flSpeed <= 0.0f )
        {
            if ( !( pPlayerPev->flags & FL_ONGROUND ) )
            {
                KickBack( 2.0f, 1.0f, 0.5f, 0.35f, 9.0f, 6.0f, 5 );
            }
            else if ( !( pPlayerPev->flags & FL_DUCKING ) )
            {
                KickBack( 1.0f, 0.375f, 0.175f, 0.0375f, 5.75f, 1.75f, 8 );
            }
            else
            {
                KickBack( 0.9f, 0.35f, 0.15f, 0.025f, 5.5f, 1.5f, 9 );
            }
        }
        else
        {
            KickBack( 1.5f, 0.45f, 0.225f, 0.05f, 6.5f, 2.5f, 7 );
        }
    }
}

void CAK47::PrimaryAttack( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    entvars_t *pPlayerPev = m_pPlayer->pev;
    float flSpreadModifier;

    if ( !( pPlayerPev->flags & FL_ONGROUND ) )
    {
        flSpreadModifier = 0.4f;
        AK47Fire( flSpreadModifier * m_flAccuracy + 0.04f, 0.0955f, FALSE );
    }
    else
    {
        float flVelocityX = pPlayerPev->velocity.x;
        float floatVelocityY = pPlayerPev->velocity.y;
        float flSpeed = sqrtf( ( flVelocityX * flVelocityX ) + ( floatVelocityY * floatVelocityY ) );

        if ( flSpeed <= 140.0f )
        {
            AK47Fire( m_flAccuracy * 0.0275f, 0.0955f, FALSE );
        }
        else
        {
            flSpreadModifier = 0.07f;
            AK47Fire( flSpreadModifier * m_flAccuracy + 0.04f, 0.0955f, FALSE );
        }
    }
}

void CAK47::SecondaryAttack( void )
{
    return;
}

void CAK47::Reload( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
    {
        return;
    }

    if ( DefaultReload( 30, 1, 2.45f ) )
    {
        m_pPlayer->SetAnimation( PLAYER_RELOAD );
        m_flAccuracy = 0.2f;
        m_iShotsFired = 0;
        m_bDelayFire = FALSE;
    }
}

void CAK47::WeaponIdle( void )
{
    CBasePlayerWeapon::WeaponIdle();

    if ( m_pPlayer == nullptr )
    {
        return;
    }

    Vector vecAutoAim;
    m_pPlayer->GetAutoaimVector( vecAutoAim, 0.17364818f );

    if ( UTIL_WeaponTimeBase() < m_flTimeWeaponIdle )
    {
        return;
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;

    BOOL bHasAmmo = TRUE;
    if ( !UseDecrement() )
    {
        bHasAmmo = FALSE;
    }

    SendWeaponAnim( 0, bHasAmmo );
}

BOOL CAK47::UseDecrement( void )
{
    return TRUE;
}

extern "C" void weapon_ak47(entvars_t *pev)
{
    if (pev == nullptr)
    {
        edict_t *pNewEdict = CREATE_ENTITY();
        if (pNewEdict != nullptr)
        {
            pev = &pNewEdict->v;
        }
        else
        {
            return;
        }
    }

    edict_t *pEdict = pev->pContainingEntity;
    if (pEdict != nullptr && pEdict->pvPrivateData != nullptr)
    {
        return;
    }

    CAK47 *pAK47 = (CAK47 *)pfnPvAllocEntPrivateData(pEdict, 284);
    if (pAK47 == nullptr)
    {
        return;
    }

    pAK47->CAK47::CAK47();

    pev->classname = ALLOC_STRING("weapon_ak47");
    pEdict->pvPrivateData = pAK47;
    pAK47->pev = pev;
}
LINK_ENTITY_TO_FUNCPTR(weapon_ak47, weapon_ak47);
