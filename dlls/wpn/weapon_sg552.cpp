#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CSG552 : public CBasePlayerWeapon
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
    void SecondaryAttack(void) override;
    BOOL UseDecrement(void) override;

    void SG552Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireSG552;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CSG552::m_SaveData[] =
{
    DEFINE_FIELD(CSG552, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CSG552, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CSG552, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_sg552, CSG552);

void CSG552::Precache(void)
{
    PRECACHE_MODEL("models/v_sg552.mdl");
    PRECACHE_MODEL("models/w_sg552.mdl");

    PRECACHE_SOUND("weapons/sg552-1.wav");
    PRECACHE_SOUND("weapons/sg552-2.wav");
    PRECACHE_SOUND("weapons/sg552_clipout.wav");
    PRECACHE_SOUND("weapons/sg552_clipin.wav");
    PRECACHE_SOUND("weapons/sg552_boltpull.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_usFireSG552 = PRECACHE_EVENT(1, "events/sg552.sc");
}

void CSG552::Spawn(void)
{
    Precache();

    m_iId = WEAPON_SG552;
    SET_MODEL(ENT(pev), "models/w_sg552.mdl");

    m_iShotsFired = 0;
    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.2f;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CSG552::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;

    return DefaultDeploy("models/v_sg552.mdl", "models/p_sg552.mdl", 2, "mp5", UseDecrement() != FALSE);
}

int CSG552::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "556Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 10;
    p->iFlags = 0;
    p->iId = WEAPON_SG552;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_SG552;

    return 1;
}

float CSG552::GetMaxSpeed(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        return 212.0f;
    }

    return 180.0f;
}

int CSG552::iItemSlot(void)
{
    return 1;
}

void CSG552::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        SG552Fire(0.45f * m_flAccuracy + 0.035f, 0.0825f, FALSE);
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed <= 140.0f)
        {
            float flCycleTime;

            if (m_pPlayer->m_iFOV == 90)
            {
                flCycleTime = 0.0825f;
            }
            else
            {
                flCycleTime = 0.135f;
            }

            SG552Fire(m_flAccuracy * 0.02f, flCycleTime, FALSE);
        }
        else
        {
            SG552Fire(0.075f * m_flAccuracy + 0.035f, 0.0825f, FALSE);
        }
    }
}

void CSG552::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(30, 1, 3.0f))
        {
            if (m_pPlayer->m_iFOV != 90)
            {
                m_pPlayer->m_iFOV = 90;
                m_pPlayer->pev->fov = 90.0f;
                m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
            }

            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.2f;
            m_iShotsFired = 0;
            m_bDelayFire = false;
        }
    }
}

int CSG552::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CSG552", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CSG552::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CSG552", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CSG552::SecondaryAttack(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        m_pPlayer->m_iFOV = 55;
        m_pPlayer->pev->fov = 55.0f;
    }
    else
    {
        m_pPlayer->m_iFOV = 90;
        m_pPlayer->pev->fov = 90.0f;
    }

    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

void CSG552::SG552Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220) + 0.3f;
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

    m_pPlayer->m_iWeaponVolume = 1500;
    m_pPlayer->m_iWeaponFlash = 512;

    Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_556MM, 33, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireSG552, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 5, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

    float flSpeed = m_pPlayer->pev->velocity.Length2D();
    if (flSpeed <= 0.0f)
    {
        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(1.25f, 0.45f, 0.22f, 0.18f, 6.0f, 4.0f, 5);
        }
        else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            KickBack(0.625f, 0.375f, 0.25f, 0.0125f, 4.0f, 2.25f, 9);
        }
        else
        {
            KickBack(0.6f, 0.35f, 0.2f, 0.0125f, 3.7f, 2.0f, 10);
        }
    }
    else
    {
        KickBack(1.0f, 0.45f, 0.28f, 0.04f, 4.25f, 2.5f, 7);
    }
}

BOOL CSG552::UseDecrement(void)
{
    return TRUE;
}

void CSG552::WeaponIdle(void)
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

