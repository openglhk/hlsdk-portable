#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CP90 : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;
    float GetMaxSpeed(void) override;

    BOOL Deploy(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;
    BOOL UseDecrement(void) override;

    void P90Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireP90;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CP90::m_SaveData[] =
{
    DEFINE_FIELD(CP90, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CP90, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CP90, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_p90, CP90);

void CP90::Precache(void)
{
    PRECACHE_MODEL("models/v_p90.mdl");
    PRECACHE_MODEL("models/w_p90.mdl");

    PRECACHE_SOUND("weapons/p90-1.wav");
    PRECACHE_SOUND("weapons/p90_clipout.wav");
    PRECACHE_SOUND("weapons/p90_clipin.wav");
    PRECACHE_SOUND("weapons/p90_boltpull.wav");
    PRECACHE_SOUND("weapons/p90_cliprelease.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_usFireP90 = PRECACHE_EVENT(1, "events/p90.sc");
}

void CP90::Spawn(void)
{
    pev->classname = MAKE_STRING("weapon_p90");
    Precache();

    m_iId = WEAPON_P90;
    SET_MODEL(ENT(pev), "models/w_p90.mdl");

    m_iShotsFired = 0;
    m_iDefaultAmmo = 50;
    m_flAccuracy = 0.2f;
    m_bDelayFire = false;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CP90::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;
    m_bDelayFire = false;

    return DefaultDeploy("models/v_p90.mdl", "models/p_p90.mdl", 2, "carbine", UseDecrement() != FALSE);
}

int CP90::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "57mm";
    p->iMaxAmmo1 = 100;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 50;
    p->iSlot = 0;
    p->iPosition = 8;
    p->iFlags = 0;
    p->iId = WEAPON_P90;
    p->iWeight = 26;
    p->bIsAccessory = false;

    m_iId = WEAPON_P90;

    return 1;
}

float CP90::GetMaxSpeed(void)
{
    return 221.0f;
}

int CP90::iItemSlot(void)
{
    return 1;
}

void CP90::P90Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired) / 175) + 0.45f;
    if (flAccuracy > 1.0f)
    {
        flAccuracy = 1.0f;
    }
    m_flAccuracy = flAccuracy;

    if (m_iClip < 1)
    {
        if (m_fFireOnEmpty)
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
        }
        return;
    }

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

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_57MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireP90, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 5, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        KickBack(0.9f, 0.45f, 0.35f, 0.04f, 5.25f, 3.5f, 4);
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 0.0f)
        {
            KickBack(0.45f, 0.3f, 0.2f, 0.0275f, 4.0f, 2.25f, 7);
        }
        else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            KickBack(0.3f, 0.225f, 0.125f, 0.02f, 3.25f, 1.25f, 8);
        }
        else
        {
            KickBack(0.275f, 0.2f, 0.125f, 0.02f, 3.0f, 1.0f, 9);
        }
    }
}

void CP90::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        m_bDelayFire = true;
        m_iShotsFired++;

        float flAccuracy = (float)((m_iShotsFired * m_iShotsFired) / 175) + 0.45f;
        if (flAccuracy > 1.0f)
        {
            flAccuracy = 1.0f;
        }
        m_flAccuracy = flAccuracy;

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
            float flSpread = m_flAccuracy * 0.3f;

            FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_57MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);

            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireP90, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 5, 0);

            m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.066f;

            if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
            {
                m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
            }

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

            if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
            {
                KickBack(0.9f, 0.45f, 0.35f, 0.04f, 5.25f, 3.5f, 4);
                return;
            }

            float flSpeed = m_pPlayer->pev->velocity.Length2D();
            if (flSpeed <= 0.0f)
            {
                if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
                {
                    KickBack(0.3f, 0.225f, 0.125f, 0.02f, 3.25f, 1.25f, 8);
                }
                else
                {
                    KickBack(0.275f, 0.2f, 0.125f, 0.02f, 3.0f, 1.0f, 9);
                }
            }
            else
            {
                KickBack(0.45f, 0.3f, 0.2f, 0.0275f, 4.0f, 2.25f, 7);
            }
        }
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 170.0f)
        {
            P90Fire(m_flAccuracy * 0.115f, 0.066f, FALSE);
        }
        else
        {
            P90Fire(m_flAccuracy * 0.045f, 0.066f, FALSE);
        }
    }
}

void CP90::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(50, 1, 3.4f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.2f;
            m_iShotsFired = 0;
        }
    }
}

int CP90::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CP90", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CP90::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CP90", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

BOOL CP90::UseDecrement(void)
{
    return TRUE;
}

void CP90::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    if (UTIL_WeaponTimeBase() < m_flTimeWeaponIdle)
    {
        return;
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
    SendWeaponAnim(0, UseDecrement() != FALSE);
}

