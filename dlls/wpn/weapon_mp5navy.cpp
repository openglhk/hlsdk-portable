#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CMP5N : public CBasePlayerWeapon
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
    void Holster(int skiplocal = 0) override;

    void MP5NFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireMP5N;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CMP5N::m_SaveData[] =
{
    DEFINE_FIELD(CMP5N, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CMP5N, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CMP5N, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_mp5navy, CMP5N);

void CMP5N::Precache(void)
{
    PRECACHE_MODEL("models/v_mp5.mdl");
    PRECACHE_MODEL("models/w_mp5.mdl");

    PRECACHE_SOUND("weapons/mp5-1.wav");
    PRECACHE_SOUND("weapons/mp5-2.wav");
    PRECACHE_SOUND("weapons/mp5_clipout.wav");
    PRECACHE_SOUND("weapons/mp5_clipin.wav");
    PRECACHE_SOUND("weapons/mp5_slideback.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_usFireMP5N = PRECACHE_EVENT(1, "events/mp5n.sc");
}

void CMP5N::Spawn(void)
{
    Precache();

    m_iId = WEAPON_MP5NAVY;
    SET_MODEL(ENT(pev), "models/w_mp5.mdl");

    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.0f;
    m_bDelayFire = false;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CMP5N::Deploy(void)
{
    iShellOn = 1;
    m_flAccuracy = 0.0f;
    m_bDelayFire = false;

    m_pPlayer->m_iHideHUD &= ~HIDEHUD_WEAPONS;

    return DefaultDeploy("models/v_mp5.mdl", "models/p_mp5.mdl", 2, "mp5", UseDecrement() != FALSE);
}

int CMP5N::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "9mm";
    p->iMaxAmmo1 = 120;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 7;
    p->iFlags = 0;
    p->iId = WEAPON_MP5NAVY;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_MP5NAVY;

    return 1;
}

float CMP5N::GetMaxSpeed(void)
{
    return 225.0f;
}

void CMP5N::Holster(int skiplocal)
{
    m_pPlayer->m_iHideHUD |= HIDEHUD_WEAPONS;
}

int CMP5N::iItemSlot(void)
{
    return 1;
}

void CMP5N::MP5NFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)(m_iShotsFired * m_iShotsFired) / 220.1f + 0.45f;
    if (flAccuracy > 0.75f)
    {
        flAccuracy = 0.75f;
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
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 26, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireMP5N, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

    m_pPlayer->m_iWeaponVolume = 800;
    m_pPlayer->m_iWeaponFlash = 128;

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        KickBack(0.9f, 0.475f, 0.35f, 0.0425f, 5.0f, 3.0f, 6);
        return;
    }

    float flSpeed = m_pPlayer->pev->velocity.Length2D();
    if (flSpeed > 0.0f)
    {
        KickBack(0.5f, 0.275f, 0.2f, 0.03f, 3.0f, 2.0f, 10);
    }
    else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
    {
        KickBack(0.25f, 0.175f, 0.125f, 0.02f, 2.25f, 1.25f, 10);
    }
    else
    {
        KickBack(0.225f, 0.15f, 0.1f, 0.015f, 2.0f, 1.0f, 10);
    }
}

void CMP5N::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        MP5NFire(m_flAccuracy * 0.2f, 0.075f, FALSE);
        return;
    }

    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)(m_iShotsFired * m_iShotsFired) / 220.1f + 0.45f;
    if (flAccuracy > 0.75f)
    {
        flAccuracy = 0.75f;
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
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors(vecAngles);

        Vector vecAiming;
        m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

        Vector vecSrc = m_pPlayer->GetGunPosition();
        float flSpread = m_flAccuracy * 0.04f;

        FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 26, m_pPlayer->random_seed, m_pPlayer->pev);

        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireMP5N, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

        m_pPlayer->m_iWeaponVolume = 800;
        m_pPlayer->m_iWeaponFlash = 128;

        m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.075f;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(0.9f, 0.475f, 0.35f, 0.0425f, 5.0f, 3.0f, 6);
            return;
        }

        float flSpeed = m_pPlayer->pev->velocity.Length2D();
        if (flSpeed <= 0.0f)
        {
            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                KickBack(0.25f, 0.175f, 0.125f, 0.02f, 2.25f, 1.25f, 10);
            }
            else
            {
                KickBack(0.225f, 0.15f, 0.1f, 0.015f, 2.0f, 1.0f, 10);
            }
        }
        else
        {
            KickBack(0.5f, 0.275f, 0.2f, 0.03f, 3.0f, 2.0f, 10);
        }
    }
}

void CMP5N::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(30, 1, 2.63f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.0f;
            m_iShotsFired = 0;
        }
    }
}

int CMP5N::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CMP5N", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CMP5N::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CMP5N", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CMP5N::SecondaryAttack(void)
{
    if (m_pPlayer == nullptr)
    {
        return;
    }

    if (!m_pPlayer->FlashlightIsOn())
    {
        m_pPlayer->FlashlightTurnOn();
    }
    else
    {
        m_pPlayer->FlashlightTurnOff();
    }

    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

BOOL CMP5N::UseDecrement(void)
{
    return TRUE;
}

void CMP5N::WeaponIdle(void)
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

