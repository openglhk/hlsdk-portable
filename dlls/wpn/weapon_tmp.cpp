#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CTMP : public CBasePlayerWeapon
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

    void TMPFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireTMP;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CTMP::m_SaveData[] =
{
    DEFINE_FIELD(CTMP, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CTMP, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CTMP, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_tmp, CTMP);

void CTMP::Precache(void)
{
    PRECACHE_MODEL("models/v_tmp.mdl");
    PRECACHE_MODEL("models/w_tmp.mdl");

    PRECACHE_SOUND("weapons/tmp-1.wav");
    PRECACHE_SOUND("weapons/tmp-2.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_usFireTMP = PRECACHE_EVENT(1, "events/tmp.sc");
}

void CTMP::Spawn(void)
{
    Precache();

    m_iId = WEAPON_TMP;
    SET_MODEL(ENT(pev), "models/w_tmp.mdl");

    m_iShotsFired = 0;
    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.2f;
    m_bDelayFire = false;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CTMP::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;
    m_bDelayFire = false;

    return DefaultDeploy("models/v_tmp.mdl", "models/p_tmp.mdl", 2, "onehanded", UseDecrement() != FALSE);
}

int CTMP::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "9mm";
    p->iMaxAmmo1 = 120;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 11;
    p->iFlags = 0;
    p->iId = WEAPON_TMP;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_TMP;

    return 1;
}

float CTMP::GetMaxSpeed(void)
{
    return 225.0f;
}

int CTMP::iItemSlot(void)
{
    return 1;
}

void CTMP::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        TMPFire(m_flAccuracy * 0.25f, 0.07f, FALSE);
        return;
    }

    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.55f;
    if (flAccuracy > 1.4f)
    {
        flAccuracy = 1.4f;
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

        m_pPlayer->m_iWeaponVolume = 500;

        Vector vecAiming;
        m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

        Vector vecSrc = m_pPlayer->GetGunPosition();
        float flSpread = m_flAccuracy * 0.03f;

        FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 20, m_pPlayer->random_seed, m_pPlayer->pev);

        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireTMP, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 5, 0);

        m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.07f;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(1.1f, 0.5f, 0.35f, 0.045f, 4.5f, 3.5f, 6);
            return;
        }

        float flSpeed = m_pPlayer->pev->velocity.Length2D();
        if (flSpeed <= 0.0f)
        {
            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                KickBack(0.725f, 0.375f, 0.15f, 0.025f, 2.75f, 2.25f, 9);
            }
            else
            {
                KickBack(0.7f, 0.35f, 0.125f, 0.025f, 2.5f, 2.0f, 10);
            }
        }
        else
        {
            KickBack(0.8f, 0.4f, 0.2f, 0.03f, 3.0f, 2.5f, 7);
        }
    }
}

void CTMP::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(30, 1, 2.12f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.2f;
            m_iShotsFired = 0;
        }
    }
}

int CTMP::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CTMP", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CTMP::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CTMP", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CTMP::TMPFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.55f;
    if (flAccuracy > 1.4f)
    {
        flAccuracy = 1.4f;
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

    m_pPlayer->m_iWeaponVolume = 500;

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 20, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireTMP, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 5, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        KickBack(1.1f, 0.5f, 0.35f, 0.045f, 4.5f, 3.5f, 6);
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 0.0f)
        {
            KickBack(0.8f, 0.4f, 0.2f, 0.03f, 3.0f, 2.5f, 7);
        }
        else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            KickBack(0.725f, 0.375f, 0.15f, 0.025f, 2.75f, 2.25f, 9);
        }
        else
        {
            KickBack(0.7f, 0.35f, 0.125f, 0.025f, 2.5f, 2.0f, 10);
        }
    }
}

BOOL CTMP::UseDecrement(void)
{
    return TRUE;
}

void CTMP::WeaponIdle(void)
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

