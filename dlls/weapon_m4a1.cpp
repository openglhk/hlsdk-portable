#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

#define M4A1_SILENCED (1 << 2) // 位移標記值 (4)

class CM4A1 : public CBasePlayerWeapon
{
public:
    void Spawn( void ) override;
    void Precache( void ) override;
    int  GetItemInfo( ItemInfo *p ) override;
    float GetMaxSpeed( void ) override;
    int  iItemSlot( void ) override;
    void M4A1Fire( float flSpread, float flCycleTime, BOOL fUseAutoAim );
    BOOL Deploy( void ) override;
    void PrimaryAttack( void ) override;
    void SecondaryAttack( void ) override;
    void Reload( void ) override;
    void WeaponIdle( void ) override;
    int  Save( CSave *save ) override;
    int  Restore( CRestore *restore ) override;
    BOOL UseDecrement( void ) override;

public:
    int            m_iWeaponState;
    int            iShellOn;
    int            m_iShell;
    unsigned short m_usFireM4A1;

TYPEDESCRIPTION CM4A1::m_SaveData[] =
{
    DEFINE_FIELD( CM4A1, m_iShotsFired, FIELD_INTEGER ),
    DEFINE_FIELD( CM4A1, m_flAccuracy, FIELD_FLOAT ),
    DEFINE_FIELD( CM4A1, m_bDelayFire, FIELD_BOOLEAN ),
    DEFINE_FIELD( CM4A1, m_iWeaponState, FIELD_INTEGER ),
};

LINK_ENTITY_TO_CLASS( weapon_m4a1, CM4A1 );

TYPEDESCRIPTION CM4A1::m_SaveData[] =
{
    DEFINE_FIELD( CM4A1, m_iWeaponState, FIELD_INTEGER )
};

void CM4A1::Spawn( void )
{
    Precache();
    m_iId = 22;
    SET_MODEL( ENT( pev ), "models/w_m4a1.mdl" );
    m_iShotsFired = 0;
    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.2f;
    m_bDelayFire = TRUE;
    m_bIsAccessory = FALSE;
    
    // 初始化彈藥索引，防止扣除彈藥時崩潰
    m_iPrimaryAmmoType = m_iId; 
    
    FallInit();
}

void CM4A1::Precache( void )
{
    PRECACHE_MODEL( "models/v_m4a1.mdl" );
    PRECACHE_MODEL( "models/w_m4a1.mdl" );
    PRECACHE_MODEL( "models/p_m4a1.mdl" );

    PRECACHE_SOUND( "weapons/m4a1-1.wav" );
    PRECACHE_SOUND( "weapons/m4a1_unsil-1.wav" );
    PRECACHE_SOUND( "weapons/m4a1_unsil-2.wav" );
    PRECACHE_SOUND( "weapons/m4a1_clipin.wav" );
    PRECACHE_SOUND( "weapons/m4a1_clipout.wav" );
    PRECACHE_SOUND( "weapons/m4a1_boltpull.wav" );
    PRECACHE_SOUND( "weapons/m4a1_deploy.wav" );
    PRECACHE_SOUND( "weapons/m4a1_silencer_on.wav" );
    PRECACHE_SOUND( "weapons/m4a1_silencer_off.wav" );

    m_iShell = PRECACHE_MODEL( "models/rshell.mdl" );
    m_usFireM4A1 = PRECACHE_EVENT( 1, "events/m4a1.sc" );
}

BOOL CM4A1::Deploy( void )
{
    m_iShotsFired = 0;
    m_flAccuracy = 0.2f;
    m_bDelayFire = TRUE;
    iShellOn = 1;

    int iAnim = ( ( m_iWeaponState & M4A1_SILENCED ) != 0 ) ? 5 : 12;

    return DefaultDeploy( "models/v_m4a1.mdl", "models/p_m4a1.mdl", iAnim, "rifle", UseDecrement() ? 1 : 0 );
}

int CM4A1::GetItemInfo( ItemInfo *p )
{
    p->pszName = STRING( pev->classname );
    p->pszAmmo1 = "556Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 6;
    p->iFlags = 0;
    p->iId = m_iId = WEAPON_M4A1;
    p->iWeight = 25;
    p->bIsAccessory = FALSE;

    return 1;
}

float CM4A1::GetMaxSpeed( void )
{
    return 207.0f;
}

int CM4A1::iItemSlot( void )
{
    return 1;
}

void CM4A1::M4A1Fire( float flSpread, float flCycleTime, BOOL fUseAutoAim )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    m_bDelayFire = TRUE;
    m_iShotsFired++;

    float flAcc = (float)(( m_iShotsFired * m_iShotsFired * m_iShotsFired ) / 220.0f ) + 0.3f;
    if ( flAcc > 1.0f )
    {
        flAcc = 1.0f;
    }
    m_flAccuracy = flAcc;

    if ( m_iClip < 1 )
    {
        if ( m_fFireOnEmpty )
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
            return;
        }
    }
    else
    {
        m_iClip--;
        m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors( vecAngles );

        Vector vecSrc = m_pPlayer->GetGunPosition();
        Vector vecDirAndAim;
        m_pPlayer->GetAutoaimVector( vecDirAndAim, 0.17364818f );

        if ( ( m_iWeaponState & M4A1_SILENCED ) == 0 )
        {
            m_pPlayer->m_iWeaponVolume = 1500; // 修正逆向跳轉：沒消音大聲
            m_pPlayer->m_iWeaponFlash = 512;
            m_pPlayer->FireBullets3( vecSrc, vecDirAndAim, flSpread, 8192.0f, 2, BULLET_PLAYER_556MM, 24, 0.97f, m_pPlayer->random_seed, FALSE );
            m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
        }
        else
        {
            m_pPlayer->m_iWeaponVolume = 120;  // 修正逆向跳轉：消音幽靜
            m_pPlayer->m_iWeaponFlash = 0;
            m_pPlayer->FireBullets3( vecSrc, vecDirAndAim, flSpread, 8192.0f, 2, BULLET_PLAYER_556MM, 22, 0.95f, m_pPlayer->random_seed, FALSE );
        }

        //m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
        PLAYBACK_EVENT_FULL( 0, m_pPlayer->edict(), m_usFireM4A1, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), ( m_iWeaponState & M4A1_SILENCED ) ? 1 : 0, 0 );

        float flTime = UTIL_WeaponTimeBase();
        m_flNextPrimaryAttack = flTime + flCycleTime;
        m_flNextSecondaryAttack = flTime + flCycleTime;
        m_flTimeWeaponIdle = flTime + 1.5f;

        if ( m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
        {
            m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
        }

        float flSpeedSq = ( m_pPlayer->pev->velocity.x * m_pPlayer->pev->velocity.x ) + ( m_pPlayer->pev->velocity.y * m_pPlayer->pev->velocity.y );
        float flSpeed = sqrtf( flSpeedSq );

        if ( flSpeed <= 0.0f )
        {
            unsigned int uiFlags = m_pPlayer->pev->flags;
            if ( !( uiFlags & FL_ONGROUND ) )
            {
                KickBack( 1.2f, 0.5f, 0.23f, 0.15f, 5.5f, 3.5f, 6 );
            }
            else if ( !( uiFlags & FL_DUCKING ) )
            {
                KickBack( 0.65f, 0.35f, 0.25f, 0.015f, 3.5f, 2.25f, 7 );
            }
            else
            {
                KickBack( 0.6f, 0.3f, 0.2f, 0.0125f, 3.25f, 2.0f, 7 );
            }
        }
        else
        {
            KickBack( 1.0f, 0.45f, 0.28f, 0.045f, 3.75f, 3.0f, 7 );
        }
    }
}

void CM4A1::PrimaryAttack( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    float flSpeed = m_pPlayer->pev->velocity.Length2D();

    if ( ( m_iWeaponState & M4A1_SILENCED ) == 0 )
    {
        if ( !( m_pPlayer->pev->flags & FL_ONGROUND ) )
        {
            M4A1Fire( m_flAccuracy * 0.4f + 0.04f, 0.0875f, FALSE );
            return;
        }

        if ( flSpeed <= 140.0f )
        {
            M4A1Fire( m_flAccuracy * 0.02f, 0.0875f, FALSE );
            return;
        }
    }
    else
    {
        if ( !( m_pPlayer->pev->flags & FL_ONGROUND ) )
        {
            M4A1Fire( m_flAccuracy * 0.4f + 0.04f, 0.0875f, FALSE );
            return;
        }

        if ( flSpeed <= 140.0f )
        {
            M4A1Fire( m_flAccuracy * 0.025f, 0.0875f, FALSE );
            return;
        }
    }

    M4A1Fire( m_flAccuracy * 0.07f + 0.035f, 0.0875f, FALSE );
}

void CM4A1::Reload( void )
{
    if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == 30 )
    {
        return;
    }

    int iAnim = ( ( m_iWeaponState & M4A1_SILENCED ) == 0 ) ? 11 : 4;

    if ( DefaultReload( 30, iAnim, 3.05f ) )
    {
        m_pPlayer->SetAnimation( PLAYER_RELOAD );
        m_flAccuracy = 0.2f;
        m_iShotsFired = 0;
        m_bDelayFire = FALSE;
    }
}

void CM4A1::SecondaryAttack( void )
{
    int iAnim;

    if ( ( m_iWeaponState & M4A1_SILENCED ) == 0 )
    {
        m_iWeaponState |= M4A1_SILENCED;
        iAnim = 6; // 拆下消音器
    }
    else
    {
        m_iWeaponState &= ~M4A1_SILENCED;
        iAnim = 13; // 裝上消音器
    }

    SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0 );

    float flTime = UTIL_WeaponTimeBase();
    m_flNextPrimaryAttack = flTime + 2.0f;
    m_flNextSecondaryAttack = flTime + 2.0f;
    m_flTimeWeaponIdle = flTime + 2.0f;
}

void CM4A1::WeaponIdle( void )
{
    if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
    {
        return;
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;

    int iAnim = ( ( m_iWeaponState & M4A1_SILENCED ) != 0 ) ? 0 : 7;
    SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0 );
}

int CM4A1::Save( CSave *save )
{
    if ( !CBasePlayerWeapon::Save( save ) )
    {
        return 0;
    }
    return save->WriteFields( "CM4A1", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

int CM4A1::Restore( CRestore *restore )
{
    if ( !CBasePlayerWeapon::Restore( restore ) )
    {
        return 0;
    }
    return restore->ReadFields( "CM4A1", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

BOOL CM4A1::UseDecrement( void )
{
    return TRUE;
}
