#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CUMP45 : public CBasePlayerWeapon
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

    void UMP45Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireUMP45;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CUMP45::m_SaveData[] =
{
    DEFINE_FIELD(CUMP45, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CUMP45, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CUMP45, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_ump45, CUMP45);

void CUMP45::Precache(void)
{
    PRECACHE_MODEL("models/v_ump45.mdl");
    PRECACHE_MODEL("models/w_ump45.mdl");

    PRECACHE_SOUND("weapons/ump45-1.wav");
    PRECACHE_SOUND("weapons/ump45_clipout.wav");
    PRECACHE_SOUND("weapons/ump45_clipin.wav");
    PRECACHE_SOUND("weapons/ump45_boltslap.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_usFireUMP45 = PRECACHE_EVENT(1, "events/ump45.sc");
}

void CUMP45::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_ump45");
    Precache();

    m_iId = WEAPON_UMP45;
    SET_MODEL(ENT(pev), "models/w_ump45.mdl");

    m_iDefaultAmmo = 25;
    m_flAccuracy = 0.0f;
    m_bDelayFire = false;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CUMP45::Deploy(void)
{
    iShellOn = 1;
    m_flAccuracy = 0.0f;
    m_bDelayFire = false;

    return DefaultDeploy("models/v_ump45.mdl", "models/p_ump45.mdl", 2, "carbine", UseDecrement() != FALSE);
}

int CUMP45::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "45ACP";
    p->iMaxAmmo1 = 100;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 25;
    p->iSlot = 0;
    p->iPosition = 15;
    p->iFlags = 0;
    p->iId = WEAPON_UMP45;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_UMP45;

    return 1;
}

float CUMP45::GetMaxSpeed(void)
{
    return 225.0f;
}

int CUMP45::iItemSlot(void)
{
    return 1;
}

void CUMP45::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        UMP45Fire(m_flAccuracy * 0.24f, 0.1f, FALSE);
        return;
    }

    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired) / 210) + 0.5f;
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
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors(vecAngles);

        Vector vecAiming;
        m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

        Vector vecSrc = m_pPlayer->GetGunPosition();
        float flSpread = m_flAccuracy * 0.04f;

        FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_45ACP, 30, m_pPlayer->random_seed, m_pPlayer->pev);

        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireUMP45, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

        m_pPlayer->m_iWeaponVolume = 800;
        m_pPlayer->m_iWeaponFlash = 128;

        m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1f;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(0.125f, 0.65f, 0.55f, 0.0475f, 5.5f, 4.0f, 10);
            return;
        }

        float flSpeed = m_pPlayer->pev->velocity.Length2D();
        if (flSpeed <= 0.0f)
        {
            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                KickBack(0.275f, 0.2f, 0.15f, 0.0225f, 2.5f, 1.5f, 10);
            }
            else
            {
                KickBack(0.25f, 0.175f, 0.125f, 0.02f, 2.25f, 1.25f, 10);
            }
        }
        else
        {
            KickBack(0.55f, 0.3f, 0.225f, 0.03f, 3.5f, 2.5f, 10);
        }
    }
}

void CUMP45::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(25, 1, 3.5f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.0f;
            m_iShotsFired = 0;
        }
    }
}

int CUMP45::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CUMP45", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CUMP45::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CUMP45", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CUMP45::UMP45Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired) / 210) + 0.5f;
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
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_45ACP, 30, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireUMP45, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

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
        KickBack(0.125f, 0.65f, 0.55f, 0.0475f, 5.5f, 4.0f, 10);
        return;
    }

    float flSpeed = m_pPlayer->pev->velocity.Length2D();
    if (flSpeed > 0.0f)
    {
        KickBack(0.55f, 0.3f, 0.225f, 0.03f, 3.5f, 2.5f, 10);
    }
    else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
    {
        KickBack(0.275f, 0.2f, 0.15f, 0.0225f, 2.5f, 1.5f, 10);
    }
    else
    {
        KickBack(0.25f, 0.175f, 0.125f, 0.02f, 2.25f, 1.25f, 10);
    }
}

BOOL CUMP45::UseDecrement(void)
{
    return TRUE;
}

void CUMP45::WeaponIdle(void)
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

