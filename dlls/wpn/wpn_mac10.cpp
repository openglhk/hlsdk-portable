#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CMAC10 : public CBasePlayerWeapon
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

    void MAC10Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireMAC10;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CMAC10::m_SaveData[] =
{
    DEFINE_FIELD(CMAC10, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CMAC10, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CMAC10, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_mac10, CMAC10);

void CMAC10::Precache(void)
{
    PRECACHE_MODEL("models/v_mac10.mdl");
    PRECACHE_MODEL("models/w_mac10.mdl");

    PRECACHE_SOUND("weapons/mac10-1.wav");
    PRECACHE_SOUND("weapons/mac10_clipout.wav");
    PRECACHE_SOUND("weapons/mac10_clipin.wav");
    PRECACHE_SOUND("weapons/mac10_boltpull.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_usFireMAC10 = PRECACHE_EVENT(1, "events/mac10.sc");
}

void CMAC10::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_mac10");
    Precache();

    m_iId = WEAPON_MAC10;
    SET_MODEL(ENT(pev), "models/w_mac10.mdl");

    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.15f;
    m_bDelayFire = false;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CMAC10::Deploy(void)
{
    iShellOn = 1;
    m_flAccuracy = 0.15f;
    m_bDelayFire = false;

    return DefaultDeploy("models/v_mac10.mdl", "models/p_mac10.mdl", 2, "onehanded", UseDecrement() != FALSE);
}

int CMAC10::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "45acp";
    p->iMaxAmmo1 = 100;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 13;
    p->iFlags = 0;
    p->iId = WEAPON_MAC10;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_MAC10;

    return 1;
}

float CMAC10::GetMaxSpeed(void)
{
    return 225.0f;
}

int CMAC10::iItemSlot(void)
{
    return 1;
}

void CMAC10::MAC10Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.6f;
    if (flAccuracy > 1.65f)
    {
        flAccuracy = 1.65f;
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

    m_pPlayer->m_iWeaponVolume = 800;
    m_pPlayer->m_iWeaponFlash = 128;

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_45ACP, 29, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireMAC10, 0.0f, g_vecZero, g_vecZero, 0.0f, local_18.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        KickBack(1.3f, 0.55f, 0.4f, 0.05f, 4.75f, 3.75f, 5);
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 0.0f)
        {
            KickBack(0.9f, 0.45f, 0.25f, 0.035f, 3.5f, 2.75f, 7);
        }
        else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            KickBack(0.775f, 0.425f, 0.2f, 0.03f, 3.0f, 2.75f, 9);
        }
        else
        {
            KickBack(0.75f, 0.4f, 0.175f, 0.03f, 2.75f, 2.5f, 10);
        }
    }
}

void CMAC10::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        MAC10Fire(m_flAccuracy * 0.375f, 0.07f, FALSE);
        return;
    }

    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.6f;
    if (flAccuracy > 1.65f)
    {
        flAccuracy = 1.65f;
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

        m_pPlayer->m_iWeaponVolume = 800;
        m_pPlayer->m_iWeaponFlash = 128;

        Vector vecAiming;
        m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

        Vector vecSrc = m_pPlayer->GetGunPosition();
        float flSpread = m_flAccuracy * 0.03f;

        FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_45ACP, 29, m_pPlayer->random_seed, m_pPlayer->pev);

        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireMAC10, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

        m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.07f;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(1.3f, 0.55f, 0.4f, 0.05f, 4.75f, 3.75f, 5);
            return;
        }

        float flSpeed = m_pPlayer->pev->velocity.Length2D();
        if (flSpeed <= 0.0f)
        {
            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                KickBack(0.775f, 0.425f, 0.2f, 0.03f, 3.0f, 2.75f, 9);
            }
            else
            {
                KickBack(0.75f, 0.4f, 0.175f, 0.03f, 2.75f, 2.5f, 10);
            }
        }
        else
        {
            KickBack(0.9f, 0.45f, 0.25f, 0.035f, 3.5f, 2.75f, 7);
        }
    }
}

void CMAC10::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(30, 1, 3.15f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.0f;
            m_iShotsFired = 0;
        }
    }
}

int CMAC10::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CMAC10", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CMAC10::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CMAC10", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

BOOL CMAC10::UseDecrement(void)
{
    return TRUE;
}

void CMAC10::WeaponIdle(void)
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
