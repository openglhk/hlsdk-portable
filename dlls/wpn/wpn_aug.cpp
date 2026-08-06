#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CAUG : public CBasePlayerWeapon
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

    void AUGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireAug;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CAUG::m_SaveData[] =
{
    DEFINE_FIELD(CAUG, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CAUG, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CAUG, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_aug, CAUG);

void CAUG::Precache(void)
{
    PRECACHE_MODEL("models/v_aug.mdl");
    PRECACHE_MODEL("models/w_aug.mdl");

    PRECACHE_SOUND("weapons/aug-1.wav");
    PRECACHE_SOUND("weapons/aug_clipout.wav");
    PRECACHE_SOUND("weapons/aug_clipin.wav");
    PRECACHE_SOUND("weapons/aug_boltpull.wav");
    PRECACHE_SOUND("weapons/aug_boltslap.wav");
    PRECACHE_SOUND("weapons/aug_forearm.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_usFireAug = PRECACHE_EVENT(1, "events/aug.sc");
}

void CAUG::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_aug");
    Precache();

    m_iId = WEAPON_AUG;
    SET_MODEL(ENT(pev), "models/w_aug.mdl");

    m_iShotsFired = 0;
    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.2f;
    m_bIsAccessory = false;

    FallInit();
}

void CAUG::AUGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 215) + 0.3f;
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

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_556MM, 32, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireAug, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

    float flSpeed = m_pPlayer->pev->velocity.Length2D();
    if (flSpeed <= 0.0f)
    {
        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(1.25f, 0.45f, 0.22f, 0.18f, 5.5f, 4.0f, 5);
        }
        else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            KickBack(0.625f, 0.375f, 0.25f, 0.0125f, 3.5f, 2.25f, 8);
        }
        else
        {
            KickBack(0.575f, 0.325f, 0.2f, 0.011f, 3.25f, 2.0f, 8);
        }
    }
    else
    {
        KickBack(1.0f, 0.45f, 0.275f, 0.05f, 4.0f, 2.5f, 7);
    }
}

BOOL CAUG::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;

    return DefaultDeploy("models/v_aug.mdl", "models/p_aug.mdl", 2, "carbine", UseDecrement() != FALSE);
}

int CAUG::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "556Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 14;
    p->iFlags = 0;
    p->iId = WEAPON_AUG;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_AUG;

    return 1;
}

float CAUG::GetMaxSpeed(void)
{
    return 216.0f;
}

int CAUG::iItemSlot( void )
{
    return 1;
}

void CAUG::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        AUGFire(0.4f * m_flAccuracy + 0.035f, 0.0825f, FALSE);
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

            AUGFire(m_flAccuracy * 0.02f, flCycleTime, FALSE);
        }
        else
        {
            AUGFire(0.07f * m_flAccuracy + 0.035f, 0.0825f, FALSE);
        }
    }
}

void CAUG::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(30, 1, 3.3f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);

            if (m_pPlayer->m_iFOV != 90)
            {
                m_pPlayer->m_iFOV = 90;
                m_pPlayer->pev->fov = 90.0f;
                m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
            }

            m_flAccuracy = 0.0f;
            m_iShotsFired = 0;
            m_bDelayFire = false;
        }
    }
}

int CAUG::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CAUG", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CAUG::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CAUG", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CAUG::SecondaryAttack(void)
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

BOOL CAUG::UseDecrement(void)
{
    return TRUE;
}

void CAUG::WeaponIdle(void)
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

