#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CFiveSeven : public CBasePlayerWeapon
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

    void FiveSevenFire(float flSpread, float flCycleTime, BOOL fUseSemi);

public:
    int m_iShell;
    unsigned short m_usFireFiveSeven;
    
    int m_iShotsFired;
    float m_flAccuracy;
    float m_flLastFire;
};

LINK_ENTITY_TO_CLASS(weapon_fiveseven, CFiveSeven);

void CFiveSeven::Precache(void)
{
    PRECACHE_MODEL("models/v_fiveseven.mdl");
    PRECACHE_MODEL("models/w_fiveseven.mdl");
    PRECACHE_MODEL("models/p_fiveseven.mdl");

    PRECACHE_SOUND("weapons/fiveseven-1.wav");
    PRECACHE_SOUND("weapons/fiveseven_clipout.wav");
    PRECACHE_SOUND("weapons/fiveseven_clipin.wav");
    PRECACHE_SOUND("weapons/fiveseven_sliderelease.wav");
    PRECACHE_SOUND("weapons/fiveseven_slidepull.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_usFireFiveSeven = PRECACHE_EVENT(1, "events/fiveseven.sc");
}

void CFiveSeven::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_fiveseven");
    Precache();

    m_iId = WEAPON_FIVESEVEN;
    SET_MODEL(ENT(pev), "models/w_fiveseven.mdl");

    m_iDefaultAmmo = 20;
    m_flAccuracy = 0.92f;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CFiveSeven::Deploy(void)
{
    m_flAccuracy = 0.92f;

    return DefaultDeploy("models/v_fiveseven.mdl", "models/p_fiveseven.mdl", 5, "onehanded", UseDecrement() != FALSE);
}

void CFiveSeven::FiveSevenFire(float flSpread, float flCycleTime, BOOL fUseSemi)
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
            m_flAccuracy = m_flAccuracy - (0.275f - (gpGlobals->time - m_flLastFire)) * 0.25f;

            if (m_flAccuracy <= 0.92f)
            {
                if (m_flAccuracy < 0.725f)
                    m_flAccuracy = 0.725f;
            }
            else
            {
                m_flAccuracy = 0.92f;
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

            FireBullets3(vecSrc, vecAiming, flSpread, 4096.0f, 1, BULLET_PLAYER_57MM, 14, m_pPlayer->random_seed, m_pPlayer->pev);

            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireFiveSeven, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

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

int CFiveSeven::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "57mm";
    p->iMaxAmmo1 = 100;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 20;
    p->iSlot = 1;
    p->iPosition = 6;
    p->iFlags = 0;
    p->iId = WEAPON_FIVESEVEN;
    p->iWeight = 5;
    p->bIsAccessory = false;

    m_iId = WEAPON_FIVESEVEN;

    return 1;
}

float CFiveSeven::GetMaxSpeed( void )
{
    return 225.0f;
}

int CFiveSeven::iItemSlot( void )
{
    return 2;
}

void CFiveSeven::PrimaryAttack(void)
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
                m_flAccuracy = m_flAccuracy - (0.275f - (gpGlobals->time - m_flLastFire)) * 0.25f;

                if (m_flAccuracy <= 0.92f)
                {
                    if (m_flAccuracy < 0.725f)
                        m_flAccuracy = 0.725f;
                }
                else
                {
                    m_flAccuracy = 0.92f;
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

                FireBullets3(vecSrc, vecAiming, flSpread, 4096.0f, 1, BULLET_PLAYER_57MM, 14, m_pPlayer->random_seed, m_pPlayer->pev);

                PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireFiveSeven, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

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
            FiveSevenFire((1.0f - m_flAccuracy) * 0.255f, 0.2f, FALSE);
        }
        else
        {
            float flSpreadFactor = 1.0f - m_flAccuracy;

            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                FiveSevenFire(flSpreadFactor * 0.15f, 0.2f, FALSE);
            }
            else
            {
                FiveSevenFire(flSpreadFactor * 0.075f, 0.2f, FALSE);
            }
        }
    }
}

void CFiveSeven::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(20, 4, 2.7f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.92f;
        }
    }
}

BOOL CFiveSeven::UseDecrement(void)
{
    return TRUE;
}

void CFiveSeven::WeaponIdle(void)
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


