#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CUSP : public CBasePlayerWeapon
{
public:
    void Spawn( void ) override;
    void Precache( void ) override;
    int  GetItemInfo( ItemInfo *p ) override;
    BOOL Deploy( void ) override;
    float GetMaxSpeed( void ) override;
    int  iItemSlot( void ) override;
    void PrimaryAttack( void ) override;
    void SecondaryAttack( void ) override;
    void Reload( void ) override;
    void WeaponIdle( void ) override;
    BOOL UseDecrement( void ) override;

    // CZDS 核心射擊函數
    void USPFire( float flSpread, float flCycleTime, BOOL fUseSemi );

public:
    int            m_iWeaponState;
    int            m_iShell;
    unsigned short m_usFireUSP;

    // 註：基底類別 CBasePlayerWeapon 已包含 m_iShotsFired, m_flAccuracy, m_flLastFire 等變數，在此不重複宣告
};

LINK_ENTITY_TO_CLASS(weapon_usp, CUSP);

void CUSP::Precache( void )
{
    PRECACHE_MODEL( "models/v_usp.mdl" );
    PRECACHE_MODEL( "models/w_usp.mdl" );

    PRECACHE_SOUND( "weapons/usp1.wav" );
    PRECACHE_SOUND( "weapons/usp2.wav" );
    PRECACHE_SOUND( "weapons/usp_unsil-1.wav" );
    PRECACHE_SOUND( "weapons/usp_clipout.wav" );
    PRECACHE_SOUND( "weapons/usp_clipin.wav" );
    PRECACHE_SOUND( "weapons/usp_silencer_on.wav" );
    PRECACHE_SOUND( "weapons/usp_silencer_off.wav" );
    PRECACHE_SOUND( "weapons/usp_sliderelease.wav" );
    PRECACHE_SOUND( "weapons/usp_slideback.wav" );

    m_iShell = PRECACHE_MODEL( "models/pshell.mdl" );
    m_usFireUSP = PRECACHE_EVENT( 1, "events/usp.sc" );
}

void CUSP::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_usp");
    Precache();
    m_iId = 16;
    SET_MODEL(ENT(pev), "models/w_usp.mdl");
    m_iDefaultAmmo = 12;
    m_flAccuracy = 0.92f;
    m_bIsAccessory = 0;
    m_iPrimaryAmmoType = m_iId;
    FallInit();
}

BOOL CUSP::Deploy( void )
{
    m_flAccuracy = 0.92f;

    if ( ( m_iWeaponState & 1 ) != 0 )
    {
        return DefaultDeploy( "models/v_usp.mdl", nullptr, 6, "onehanded", UseDecrement() != FALSE);
    }

    return DefaultDeploy( "models/v_usp.mdl", nullptr, 14, "onehanded", UseDecrement() != FALSE);
}

int CUSP::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "45ACP";
    p->iMaxAmmo1 = 100;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 12;
    p->iSlot = 1;
    p->iPosition = 4;
    p->iFlags = 0;
    p->iId = m_iId = 16;
    p->iWeight = 5;
    p->bIsAccessory = 0;

    return 1;
}

float CUSP::GetMaxSpeed( void )
{
    return 225.0f;
}

int CUSP::iItemSlot( void )
{
    return 2;
}

void CUSP::PrimaryAttack( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    entvars_t *pPlayerPev = m_pPlayer->pev;

    if ( ( m_iWeaponState & 1 ) == 0 )
    {
        if ( !( pPlayerPev->flags & FL_ONGROUND ) )
        {
            USPFire( ( 1.0f - m_flAccuracy ) * 1.2f, 0.225f, 0 );
        }
        else
        {
            float flVelocityX = pPlayerPev->velocity.x;
            float flVelocityY = pPlayerPev->velocity.y;
            float flSpeed = sqrtf( ( flVelocityX * flVelocityX ) + ( flVelocityY * flVelocityY ) );

            if ( flSpeed > 0.0f )
            {
                USPFire( ( 1.0f - m_flAccuracy ) * 0.225f, 0.225f, 0 );
                return;
            }

            float flSpreadModifier = 1.0f - m_flAccuracy;

            if ( !( pPlayerPev->flags & FL_DUCKING ) )
            {
                USPFire( flSpreadModifier * 0.1f, 0.225f, 0 );
                return;
            }

            USPFire( flSpreadModifier * 0.08f, 0.225f, 0 );
        }
    }
    else
    {
        if ( !( pPlayerPev->flags & FL_ONGROUND ) )
        {
            USPFire( ( 1.0f - m_flAccuracy ) * 1.3f, 0.225f, 0 );
            return;
        }

        float flVelocityX = pPlayerPev->velocity.x;
        float flVelocityY = pPlayerPev->velocity.y;
        float flSpeed = sqrtf( ( flVelocityX * flVelocityX ) + ( flVelocityY * flVelocityY ) );

        if ( flSpeed > 0.0f )
        {
            USPFire( ( 1.0f - m_flAccuracy ) * 0.25f, 0.225f, 0 );
            return;
        }

        float flSpreadModifier = 1.0f - m_flAccuracy;

        if ( !( pPlayerPev->flags & FL_DUCKING ) )
        {
            USPFire( flSpreadModifier * 0.15f, 0.225f, 0 );
        }
        else
        {
            USPFire( flSpreadModifier * 0.125f, 0.225f, 0 );
        }
    }
}

void CUSP::Reload( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    if ( m_pPlayer->ammo_45acp <= 0 || m_iClip == 12 )
    {
        return;
    }

    int iAnim = ( ( m_iWeaponState & 1 ) == 0 ) ? 13 : 5;

    if ( DefaultReload( 12, iAnim, 2.7f ) )
    {
        m_pPlayer->SetAnimation( PLAYER_RELOAD );
        m_flAccuracy = 0.92f;
    }
}

void CUSP::SecondaryAttack( void )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    int iAnim;

    if ( ( m_iWeaponState & 1 ) == 0 )
    {
        m_iWeaponState |= 1;
        iAnim = 7; 
    }
    else
    {
        m_iWeaponState &= ~1;
        iAnim = 15;
    }

    SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0 );
    
    strcpy( m_pPlayer->m_szAnimExtention, "onehanded" );

    float flTime = UTIL_WeaponTimeBase();
    m_flNextPrimaryAttack = flTime + 3.0f;
    m_flNextSecondaryAttack = flTime + 3.0f;
    m_flTimeWeaponIdle = flTime + 3.0f;
}

BOOL CUSP::UseDecrement( void )
{
    return TRUE;
}

void CUSP::USPFire( float flSpread, float flCycleTime, BOOL fUseSemi )
{
    if ( m_pPlayer == nullptr )
    {
        return;
    }

    m_iShotsFired++;

    if ( m_iShotsFired < 2 )
    {
        if ( m_flLastFire == 0.0f )
        {
            m_flLastFire = gpGlobals->time;
        }
        else
        {
            float flNewAcc = m_flAccuracy - ( 0.3f - ( gpGlobals->time - m_flLastFire ) ) * 0.275f;
            if ( flNewAcc <= 0.92f )
            {
                m_flAccuracy = ( flNewAcc < 0.6f ) ? 0.6f : flNewAcc;
            }
            else
            {
                m_flAccuracy = 0.92f;
            }
            m_flLastFire = gpGlobals->time;
        }

        if ( m_iClip > 0 )
        {
            m_iClip--;

            float flNextAttackTime = UTIL_WeaponTimeBase() + ( flCycleTime - 0.075f );
            m_flNextPrimaryAttack = flNextAttackTime;
            m_flNextSecondaryAttack = flNextAttackTime;

            m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

            if ( ( m_iWeaponState & 1 ) == 0 )
            {
                m_pPlayer->m_iWeaponVolume = 500;
                m_pPlayer->m_iWeaponFlash = 128;
                m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
            }
            else
            {
                m_pPlayer->m_iWeaponVolume = 100;
                m_pPlayer->m_iWeaponFlash = 128;
            }

            Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
            UTIL_MakeVectors( vecAngles );

            Vector vecSrc = m_pPlayer->GetGunPosition();
            Vector vecAiming;
            m_pPlayer->GetAutoaimVector( vecAiming, 0.17364818f );

            int iBulletDamage = ( ( m_iWeaponState & 1 ) == 0 ) ? 34 : 30;

            m_pPlayer->FireBullets3( vecSrc, vecAiming, flSpread, 4096.0f, 1, BULLET_PLAYER_45ACP, iBulletDamage, 0.79f, m_pPlayer->pev, m_pPlayer->random_seed );

            PLAYBACK_EVENT_FULL( 0, m_pPlayer->edict(), m_usFireUSP, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 0, ( m_iClip == 0 ) ? 1 : 0, ( m_iWeaponState & 1 ) ? 1 : 0 );

            if ( m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
            {
                m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
            }

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
            m_pPlayer->pev->punchangle.x -= 2.0f;
            return;
        }

        if ( m_fFireOnEmpty != 0 )
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
        }
    }
}

void CUSP::WeaponIdle( void )
{
    CBasePlayerWeapon::WeaponIdle();

    if ( m_pPlayer == nullptr )
    {
        return;
    }

    Vector vecAutoAim;
    m_pPlayer->GetAutoaimVector( vecAutoAim, 0.17364818f );

    if ( m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip != 0 )
    {
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;

        int iAnim = ( ( m_iWeaponState & 1 ) == 0 ) ? 8 : 0;

        SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0 );
    }
}



