#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CDEAGLE : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int  iItemSlot( void ) override;
    BOOL Deploy(void) override;
    BOOL Deploy(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;
    BOOL UseDecrement(void) override;

    void DEAGLEFire(float flSpread, float flCycleTime, BOOL fUseSemi);

public:
    int m_iShell;
    unsigned short m_usFireDeagle;
    
    int m_iShotsFired;
    float m_flAccuracy;
    float m_flLastFire;
};

LINK_ENTITY_TO_CLASS(weapon_deagle, CDEAGLE);

void CDEAGLE::Precache(void)
{
    PRECACHE_MODEL("models/v_deagle.mdl");
    PRECACHE_MODEL("models/w_deagle.mdl");

    PRECACHE_SOUND("weapons/deagle-1.wav");
    PRECACHE_SOUND("weapons/deagle-2.wav");
    PRECACHE_SOUND("weapons/de_clipout.wav");
    PRECACHE_SOUND("weapons/de_clipin.wav");
    PRECACHE_SOUND("weapons/de_deploy.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_usFireDeagle = PRECACHE_EVENT(1, "events/deagle.sc");
}

void CDEAGLE::Spawn(void)
{
    Precache();

    m_iId = WEAPON_DEAGLE;
    SET_MODEL(ENT(pev), "models/w_deagle.mdl");

    m_iDefaultAmmo = 7;
    m_flAccuracy = 0.9f;
    m_bIsAccessory = false;

    FallInit();
}

void CDEAGLE::DEAGLEFire(float flSpread, float flCycleTime, BOOL fUseSemi)
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
            m_flAccuracy = m_flAccuracy - (0.4f - (gpGlobals->time - m_flLastFire)) * 0.35f;

            if (m_flAccuracy <= 0.9f)
            {
                if (m_flAccuracy < 0.55f)
                    m_flAccuracy = 0.55f;
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

            m_pPlayer->m_iWeaponVolume = 1500;
            m_pPlayer->m_iWeaponFlash = 512;

            Vector vecAiming;
            m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

            Vector vecSrc = m_pPlayer->GetGunPosition();

            FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_50AE, 54, m_pPlayer->random_seed, m_pPlayer->pev);

            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireDeagle, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

            m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread - 0.075f;

            if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
            {
                m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
            }

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;

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

BOOL CDEAGLE::Deploy(void)
{
    m_flAccuracy = 0.9f;

    return DefaultDeploy("models/v_deagle.mdl", "models/p_deagle.mdl", 5, "onehanded", UseDecrement() != FALSE);
}

int CDEAGLE::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "50AE";
    p->iMaxAmmo1 = 35;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 7;
    p->iSlot = 1;
    p->iPosition = 1;
    p->iFlags = 0;
    p->iId = WEAPON_DEAGLE;
    p->iWeight = 7;
    p->bIsAccessory = false;

    m_iId = WEAPON_DEAGLE;

    return 1;
}

float CDEAGLE::GetMaxSpeed( void )
{
    return 225.0f;
}

int CDEAGLE::iItemSlot( void )
{
    return 2;
}

void CDEAGLE::PrimaryAttack(void)
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
                m_flAccuracy = m_flAccuracy - (0.4f - (gpGlobals->time - m_flLastFire)) * 0.35f;

                if (m_flAccuracy <= 0.9f)
                {
                    if (m_flAccuracy < 0.55f)
                        m_flAccuracy = 0.55f;
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

                m_pPlayer->m_iWeaponVolume = 1500;
                m_pPlayer->m_iWeaponFlash = 512;

                Vector vecAiming;
                m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

                Vector vecSrc = m_pPlayer->GetGunPosition();

                float flSpread = (1.0f - m_flAccuracy) * 1.5f;

                FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_50AE, 54, m_pPlayer->random_seed, m_pPlayer->pev);

                PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireDeagle, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

                m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.225f;

                if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
                {
                    m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
                }

                m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;

                m_pPlayer->pev->punchangle.x -= 2.0f;
            }
        }
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 0.0f)
        {
            DEAGLEFire((1.0f - m_flAccuracy) * 0.25f, 0.3f, FALSE);
        }
        else
        {
            float flSpreadFactor = 1.0f - m_flAccuracy;

            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                DEAGLEFire(flSpreadFactor * 0.13f, 0.3f, FALSE);
            }
            else
            {
                DEAGLEFire(flSpreadFactor * 0.115f, 0.3f, FALSE);
            }
        }
    }
}

void CDEAGLE::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(7, 4, 2.2f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.9f;
        }
    }
}

BOOL CDEAGLE::UseDecrement(void)
{
    return TRUE;
}

void CDEAGLE::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip != 0)
    {
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.75f;
        SendWeaponAnim(0, UseDecrement() != FALSE);
    }
}
