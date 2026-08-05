#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CGalil : public CBasePlayerWeapon
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

    void GalilFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireGalil;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;
};

TYPEDESCRIPTION CGalil::m_SaveData[] =
{
    DEFINE_FIELD(CGalil, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CGalil, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CGalil, m_bDelayFire, FIELD_BOOLEAN),
};

LINK_ENTITY_TO_CLASS(weapon_galil, CGalil);

void CGalil::Precache(void)
{
    PRECACHE_MODEL("models/v_galil_r.mdl");
    PRECACHE_MODEL("models/w_galil.mdl");

    PRECACHE_SOUND("weapons/galil-1.wav");
    PRECACHE_SOUND("weapons/galil-2.wav");
    PRECACHE_SOUND("weapons/galil_clipout.wav");
    PRECACHE_SOUND("weapons/galil_clipin.wav");
    PRECACHE_SOUND("weapons/galil_boltpull.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_usFireGalil = PRECACHE_EVENT(1, "events/galil.sc");
}

void CGalil::Spawn(void)
{
    pev->classname = MAKE_STRING("weapon_galil");

    Precache();

    m_iId = WEAPON_GALIL;
    SET_MODEL(ENT(pev), "models/w_galil.mdl");

    m_iDefaultAmmo = 35;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CGalil::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;

    return DefaultDeploy("models/v_galil_r.mdl", "models/p_galil.mdl", 2, "ak47", UseDecrement() != FALSE);
}

void CGalil::GalilFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    float flAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.35f;
    if (flAccuracy > 1.25f)
    {
        flAccuracy = 1.25f;
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

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, BULLET_PLAYER_556MM, 30, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireGalil, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 10000000.0f), (int)(m_pPlayer->pev->punchangle.y * 10000000.0f), 0, 0);

    m_pPlayer->m_iWeaponVolume = 1500;
    m_pPlayer->m_iWeaponFlash = 512;

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.28f;

    float flSpeed = m_pPlayer->pev->velocity.Length2D();
    if (flSpeed <= 0.0f)
    {
        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(1.2f, 0.5f, 0.23f, 0.15f, 5.5f, 3.5f, 6);
        }
        else if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            KickBack(0.65f, 0.35f, 0.25f, 0.015f, 3.5f, 2.25f, 7);
        }
        else
        {
            KickBack(0.6f, 0.3f, 0.2f, 0.0125f, 3.25f, 2.0f, 7);
        }
    }
    else
    {
        KickBack(1.0f, 0.45f, 0.28f, 0.045f, 3.75f, 3.0f, 7);
    }
}

int CGalil::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "556Nato";
    p->iMaxAmmo1 = 70;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 35;
    p->iSlot = 0;
    p->iPosition = 17;
    p->iFlags = 0;
    p->iId = WEAPON_GALIL;
    p->iWeight = 25;
    p->bIsAccessory = false;

    m_iId = WEAPON_GALIL;

    return 1;
}

float CGalil::GetMaxSpeed(void)
{
    return 216.0f;
}

int CGalil::iItemSlot( void )
{
    return 1;
}

void CGalil::PrimaryAttack(void)
{
    if (m_pPlayer->pev->waterlevel == 3)
    {
        PlayEmptySound();
        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
        return;
    }

    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        GalilFire(0.3f * m_flAccuracy + 0.04f, 0.0875f, FALSE);
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed <= 140.0f)
        {
            GalilFire(m_flAccuracy * 0.0375f, 0.0875f, FALSE);
        }
        else
        {
            GalilFire(0.07f * m_flAccuracy + 0.04f, 0.0875f, FALSE);
        }
    }
}

void CGalil::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(35, 1, 2.45f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.2f;
            m_iShotsFired = 0;
            m_bDelayFire = false;
        }
    }
}

int CGalil::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CGalil", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CGalil::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CGalil", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CGalil::SecondaryAttack(void)
{
    return;
}

BOOL CGalil::UseDecrement(void)
{
    return TRUE;
}

void CGalil::WeaponIdle(void)
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
