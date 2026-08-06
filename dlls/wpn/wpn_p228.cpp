#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CP228 : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
	float GetMaxSpeed( void ) override;
    int  iItemSlot( void ) override;
    BOOL Deploy(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;
    BOOL UseDecrement(void) override;

    void P228Fire(float flSpread, float flCycleTime, BOOL fUseSemi);

public:
    float m_flAccuracy;
    float m_flLastFire;
    int m_iShotsFired;
    unsigned short m_usFireP228;
};

LINK_ENTITY_TO_CLASS(weapon_p228, CP228);

void CP228::Precache( void )
{
    PRECACHE_MODEL( "models/v_p228.mdl" );
    PRECACHE_MODEL( "models/w_p228.mdl" );

    PRECACHE_SOUND( "weapons/p228-1.wav" );
    PRECACHE_SOUND( "weapons/p228_clipout.wav" );
    PRECACHE_SOUND( "weapons/p228_clipin.wav" );
    PRECACHE_SOUND( "weapons/p228_sliderelease.wav" );
    PRECACHE_SOUND( "weapons/p228_slidepull.wav" );

    m_iShell = PRECACHE_MODEL( "models/pshell.mdl" );
    m_usFireP228 = PRECACHE_EVENT( 1, "events/p228.sc" );
}

void CP228::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_p228");
    Precache();
    m_iId = WEAPON_P228;
    SET_MODEL(ENT(pev), "models/w_p228.mdl");

    m_iDefaultAmmo = 13;
    m_flAccuracy = 0.9f;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CP228::Deploy(void)
{
    m_flAccuracy = 0.9f;

    return DefaultDeploy("models/v_p228.mdl", "models/p_p228.mdl", 6, "onehanded", UseDecrement() != FALSE);
}

int CP228::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "357SIG";
    p->iMaxAmmo1 = 52;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 13;
    p->iSlot = 1;
    p->iPosition = 3;
    p->iFlags = 0;
    p->iId = WEAPON_P228;
    p->iWeight = 5;
    p->bIsAccessory = false;

    m_iId = WEAPON_P228;

    return 1;
}

float CP228::GetMaxSpeed( void )
{
    return 225.0f;
}

int CP228::iItemSlot( void )
{
    return 2;
}

void CP228::P228Fire(float flSpread, float flCycleTime, BOOL fUseSemi)
{
    m_iShotsFired++;

    if (m_iShotsFired < 2)
    {
        if (m_flLastFire == 0.0f)
        {
            m_flLastFire = gpGlobals->time;
        }
        else
        {
            m_flAccuracy = m_flAccuracy - (0.325f - (gpGlobals->time - m_flLastFire)) * 0.3f;

            if (m_flAccuracy <= 0.9f)
            {
                if (m_flAccuracy < 0.6f)
                    m_flAccuracy = 0.6f;
            }
            else
            {
                m_flAccuracy = 0.9f;
            }

            m_flLastFire = gpGlobals->time;
        }

        if (m_iClip > 0)
        {
            m_iClip--;

            m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

            m_pPlayer->SetAnimation(PLAYER_ATTACK1);

            Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
            UTIL_MakeVectors(vecAngles);

            m_pPlayer->m_iWeaponVolume = 500;
            m_pPlayer->m_iWeaponFlash = 128;

            Vector vecAiming;
            m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

            Vector vecSrc = m_pPlayer->GetGunPosition();

            FireBullets3(vecSrc, vecAiming, flSpread, 4096.0f, 1, BULLET_PLAYER_357SIG, 20, m_pPlayer->random_seed, m_pPlayer->pev);

            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireP228, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

            m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread - 0.05f;

            if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
            {
                m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
            }

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

            m_pPlayer->pev->punchangle.x -= 2.0f;
            return;
        }

        if (m_fFireOnEmpty)
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
        }
    }
}

void CP228::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        m_iShotsFired++;

        if (m_iShotsFired < 2)
        {
            if (m_flLastFire == 0.0f)
            {
                m_flLastFire = gpGlobals->time;
            }
            else
            {
                m_flAccuracy = m_flAccuracy - (0.325f - (gpGlobals->time - m_flLastFire)) * 0.3f;

                if (m_flAccuracy <= 0.9f)
                {
                    if (m_flAccuracy < 0.6f)
                        m_flAccuracy = 0.6f;
                }
                else
                {
                    m_flAccuracy = 0.9f;
                }

                m_flLastFire = gpGlobals->time;
            }

            if (m_iClip < 1)
            {
                if (m_fFireOnEmpty)
                {
                    PlayEmptySound();
                    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
                }
            }
            else
            {
                m_iClip--;

                m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

                m_pPlayer->SetAnimation(PLAYER_ATTACK1);

                Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
                UTIL_MakeVectors(vecAngles);

                m_pPlayer->m_iWeaponVolume = 500;
                m_pPlayer->m_iWeaponFlash = 128;

                Vector vecAiming;
                m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

                Vector vecSrc = m_pPlayer->GetGunPosition();

                float flSpread = (1.0f - m_flAccuracy) * 1.5f;

                FireBullets3(vecSrc, vecAiming, flSpread, 4096.0f, 1, BULLET_PLAYER_357SIG, 20, m_pPlayer->random_seed, m_pPlayer->pev);

                PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireP228, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

                m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.15f;

                if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
                {
                    m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
                }

                m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

                m_pPlayer->pev->punchangle.x -= 2.0f;
            }
        }
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 0.0f)
        {
            P228Fire((1.0f - m_flAccuracy) * 0.255f, 0.2f, FALSE);
        }
        else
        {
            float flSpreadFactor = 1.0f - m_flAccuracy;

            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                P228Fire(flSpreadFactor * 0.15f, 0.2f, FALSE);
            }
            else
            {
                P228Fire(flSpreadFactor * 0.075f, 0.2f, FALSE);
            }
        }
    }
}

void CP228::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(13, 5, 2.7f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.9f;
        }
    }
}

BOOL CP228::UseDecrement(void)
{
    return TRUE;
}

void CP228::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip != 0)
    {
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0625f;
        SendWeaponAnim(0, UseDecrement() != FALSE);
    }
}