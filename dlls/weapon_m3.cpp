#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CM3 : public CBasePlayerWeapon
{
public:
    void Spawn( void ) override;
    void Precache( void ) override;
    int  GetItemInfo( ItemInfo *p ) override;
    BOOL Deploy( void ) override;
	void Holster( int skiplocal = 0 ) override;
	int  Save( CSave *save ) override;
    int  Restore( CRestore *restore ) override;
    float GetMaxSpeed( void ) override;
    int  iItemSlot( void ) override;
    void PrimaryAttack( void ) override;
	void SecondaryAttack( void ) override;
	BOOL UseDecrement( void ) override;
    void Reload( void ) override;
    void WeaponIdle( void ) override;
public:
	int m_iShell;
    unsigned short m_usFireM3;
    float          m_flPumpTime;
public:
    static TYPEDESCRIPTION m_SaveData[];
};

LINK_ENTITY_TO_CLASS(weapon_m3, CM3);

TYPEDESCRIPTION CM3::m_SaveData[] =
{
    DEFINE_FIELD( CM3, m_fInSpecialReload, FIELD_INTEGER ),
    DEFINE_FIELD( CM3, m_flPumpTime, FIELD_TIME ),
    DEFINE_FIELD( CM3, m_flNextReload, FIELD_TIME ),
};

int CM3::Save( CSave *save )
{
    if ( !CBasePlayerWeapon::Save( save ) )
    {
        return 0;
    }

    return save->WriteFields( "CM3", this, m_SaveData, ARRAYSIZE( m_SaveData ) );
}

int CM3::Restore( CRestore *restore )
{
    if ( !CBasePlayerWeapon::Restore( restore ) )
    {
        return 0;
    }

    return restore->ReadFields( "CM3", this, m_SaveData, ARRAYSIZE( m_SaveData ) );
}

void CM3::Precache( void )
{
    PRECACHE_MODEL( "models/v_m3.mdl" );
    PRECACHE_MODEL( "models/w_m3.mdl" );

    m_iShell = PRECACHE_MODEL( "models/shotgunshell.mdl" );
    m_iShellId = m_iShell;

    PRECACHE_SOUND( "weapons/m3-1.wav" );
    PRECACHE_SOUND( "weapons/m3_insertshell.wav" );
    PRECACHE_SOUND( "weapons/m3_pump.wav" );
    PRECACHE_SOUND( "weapons/reload1.wav" );
    PRECACHE_SOUND( "weapons/reload3.wav" );

    m_usFireM3 = PRECACHE_EVENT( 1, "events/m3.sc" );
}

BOOL CM3::Deploy( void )
{
    m_pPlayer->m_iHideHUD &= ~HIDEHUD_WEAPONS;

    BOOL bHasAmmo = TRUE;
    if ( !HasAmmo() )
    {
        bHasAmmo = FALSE;
    }

    return DefaultDeploy( "models/v_m3.mdl", nullptr, 6, "shotgun", bHasAmmo );
}

int CM3::GetItemInfo( ItemInfo *p )
{
    p->pszName = STRING( pev->classname );
    p->pszAmmo1 = "buckshot";
    p->iMaxAmmo1 = 32;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 8;
    p->iSlot = 0;
    p->iPosition = 5;
    p->m_iId = 15;
    p->iId = 15;
    p->iWeight = 20;
    p->iFlags = 0;
    p->bIsAccessory = 0;

    return 1;
}

float CM3::GetMaxSpeed( void )
{
    return 207.0f;
}

void CM3::Holster( int skiplocal )
{
    m_pPlayer->m_iHideHUD |= HIDEHUD_WEAPONS;
}

int CM3::iItemSlot( void )
{
    return 1;
}

void CM3::PrimaryAttack( void )
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
            if ( m_fInSpecialReload == 0 )
            {
                PlayEmptySound();
            }
            UpdateWeaponSound();
            return;
        }
    }

    m_iClip--;

    entvars_t *pPlayerPev = m_pPlayer->pev;
    pPlayerPev->effects |= EF_MUZZLEFLASH;

    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

    Vector vecAngles = pPlayerPev->v_angle + pPlayerPev->punchangle;
    UTIL_MakeVectors( vecAngles );

    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecAiming;
    m_pPlayer->GetAutoaimVector( vecAiming, 0.087155744f );

    Vector vecSpread = Vector( 0.0675f, 0.0675f, 0.0675f );

    FireBullets( 9, vecSrc, vecAiming, vecSpread, 2048.0f, BULLET_PLAYER_BUCKSHOT, 0, m_pPlayer->random_seed );

    PLAYBACK_EVENT_FULL( 0, m_pPlayer->edict(), m_usFireM3, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, gpGlobals->v_forward.x, gpGlobals->v_forward.y, (int)( pPlayerPev->punchangle.x * 100.0f ), (int)( pPlayerPev->punchangle.y * 100.0f ), 0, 0 );

    m_pPlayer->m_iWeaponVolume = 2250;
    m_pPlayer->m_iWeaponFlash  = 512;

    float flWeaponTime = UTIL_WeaponTimeBase();
    m_flNextPrimaryAttack = flWeaponTime + 0.875f;
    m_flNextSecondaryAttack = flWeaponTime + 0.875f;
    m_flTimeWeaponIdle = flWeaponTime + 1.9f;

    if ( m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
    {
        m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
    }
}

void CM3::Reload( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == 8 )
    {
        return;
    }

    if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
    {
        return;
    }

    if ( m_fInSpecialReload == 0 )
    {
        m_pPlayer->SetAnimation( PLAYER_RELOAD );
        m_fInSpecialReload = 1;

        BOOL bHasAmmo = TRUE;
        if ( !UseDecrement() )
        {
            bHasAmmo = FALSE;
        }

        SendWeaponAnim( 5, bHasAmmo );

        float flWeaponTime = UTIL_WeaponTimeBase();
        m_pPlayer->m_flNextAttack = flWeaponTime + 0.55f;
        m_flNextPrimaryAttack = flWeaponTime + 0.55f;
        m_flNextSecondaryAttack = flWeaponTime + 0.55f;
        m_flTimeWeaponIdle = flWeaponTime + 0.55f;
    }
    else
    {
        if ( m_fInSpecialReload == 1 )
        {
            if ( m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() )
            {
                m_fInSpecialReload = 2;

                BOOL bHasAmmo = TRUE;
                if ( !UseDecrement() )
                {
                    bHasAmmo = FALSE;
                }

                SendWeaponAnim( 3, bHasAmmo );

                float flWeaponTime = UTIL_WeaponTimeBase();
                m_flNextReload = flWeaponTime + 0.45f;
                m_flTimeWeaponIdle = flWeaponTime + 0.45f;
            }
        }
        else
        {
            if ( m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() )
            {
                m_fInSpecialReload = 1;
                m_iClip++;
                m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
                m_pPlayer->ammo_buckshot--;
            }
        }
    }
}

void CM3::SecondaryAttack( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    if ( !m_pPlayer->FlashlightIsOn() )
    {
        m_pPlayer->FlashlightTurnOn();
    }
    else
    {
        m_pPlayer->FlashlightTurnOff();
    }

    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

void CM3::Spawn( void )
{
    Precache();
    m_iId = 15;
    SET_MODEL( edict(), "models/w_m3.mdl" );
    m_iDefaultAmmo = 8;
    m_bIsAccessory = 0;
    FallInit();
}

BOOL CM3::UseDecrement( void )
{
    return TRUE;
}

void CM3::WeaponIdle( void )
{
    CBasePlayerWeapon::WeaponIdle();

    if ( m_pPlayer == nullptr )
    {
        return;
    }

    Vector vecAutoAim;
    m_pPlayer->GetAutoaimVector( vecAutoAim, 0.087155744f );

    if ( m_flPumpTime != 0.0f && m_flPumpTime < UTIL_WeaponTimeBase() )
    {
        m_flPumpTime = 0.0f;
    }

    if ( UTIL_WeaponTimeBase() <= m_flTimeWeaponIdle )
    {
        return;
    }

    BOOL bHasAmmo = TRUE;
    if ( !UseDecrement() )
    {
        bHasAmmo = FALSE;
    }

    if ( m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0 )
    {
        Reload();
        return;
    }

    if ( m_fInSpecialReload == 0 )
    {
        SendWeaponAnim( 0, bHasAmmo );
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
        return;
    }

    if ( m_iClip == 8 || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0 )
    {
        SendWeaponAnim( 4, bHasAmmo );
        m_fInSpecialReload = 0;
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
        return;
    }

    Reload();
}