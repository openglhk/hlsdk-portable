#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CG3SG1 : public CBasePlayerWeapon
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

    void G3SG1Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireG3SG1;
};

TYPEDESCRIPTION CG3SG1::m_SaveData[] =
{
    DEFINE_FIELD(CG3SG1, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CG3SG1, m_flLastFire, FIELD_TIME),
};

LINK_ENTITY_TO_CLASS(weapon_g3sg1, CG3SG1);

void CG3SG1::Precache(void)
{
    PRECACHE_MODEL("models/v_g3sg1.mdl");
    PRECACHE_MODEL("models/w_g3sg1.mdl");

    PRECACHE_SOUND("weapons/g3sg1-1.wav");
    PRECACHE_SOUND("weapons/g3sg1_slide.wav");
    PRECACHE_SOUND("weapons/g3sg1_clipin.wav");
    PRECACHE_SOUND("weapons/g3sg1_clipout.wav");
    PRECACHE_SOUND("weapons/zoom.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_iShellId = m_iShell;
    m_usFireG3SG1 = PRECACHE_EVENT(1, "events/g3sg1.sc");
}

void CG3SG1::Spawn(void)
{
    Precache();

    m_iId = WEAPON_G3SG1;
    SET_MODEL(ENT(pev), "models/w_g3sg1.mdl");

    m_iDefaultAmmo = 20;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CG3SG1::Deploy(void)
{
    m_flAccuracy = 0.2f;

    return DefaultDeploy("models/v_g3sg1.mdl", "models/p_g3sg1.mdl", 4, "mp5", UseDecrement() != FALSE);
}

void CG3SG1::G3SG1Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        flSpread += 0.025f;
    }

    if (m_flLastFire == 0.0f)
    {
        m_flLastFire = gpGlobals->time;
    }
    else
    {
        float flIdealAccuracy = (gpGlobals->time - m_flLastFire) * 0.3f + 0.55f;
        if (flIdealAccuracy > 0.98f)
        {
            flIdealAccuracy = 0.98f;
        }
        m_flAccuracy = flIdealAccuracy;
        m_flLastFire = gpGlobals->time;
    }

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

    m_pPlayer->m_iWeaponVolume = 1500;
    m_pPlayer->m_iWeaponFlash = 256;

    Vector vecAiming = gpGlobals->v_forward;
    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 3, BULLET_PLAYER_762MM, 80, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireG3SG1, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;

    float flKickUp = UTIL_SharedRandomFloat(m_pPlayer->random_seed + 4, 2.75f, 3.25f);
    m_pPlayer->pev->punchangle.x -= (m_pPlayer->pev->punchangle.x * 0.25f + flKickUp);

    float flKickSide = UTIL_SharedRandomFloat(m_pPlayer->random_seed + 5, -1.25f, 1.5f);
    m_pPlayer->pev->punchangle.y += flKickSide;
}

int CG3SG1::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "762Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 20;
    p->iSlot = 0;
    p->iPosition = 3;
    p->iFlags = 0;
    p->iId = WEAPON_G3SG1;
    p->iWeight = 20;
    p->bIsAccessory = false;

    m_iId = WEAPON_G3SG1;

    return 1;
}

float CG3SG1::GetMaxSpeed(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        return 189.0f;
    }

    return 135.0f;
}

int CG3SG1::iItemSlot(void)
{
    return 1;
}

void CG3SG1::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        G3SG1Fire((1.0f - m_flAccuracy) * 0.45f, 0.25f, FALSE);
        return;
    }

    float flSpeed = m_pPlayer->pev->velocity.Length2D();

    if (flSpeed > 0.0f)
    {
        G3SG1Fire(0.15f, 0.25f, FALSE);
        return;
    }

    float flSpreadModifier = 1.0f - m_flAccuracy;

    if (m_pPlayer->pev->flags & FL_DUCKING)
    {
        G3SG1Fire(flSpreadModifier * 0.035f, 0.25f, FALSE);
    }
    else
    {
        G3SG1Fire(flSpreadModifier * 0.055f, 0.25f, FALSE);
    }
}

void CG3SG1::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
    {
        return;
    }

    if (DefaultReload(20, 3, 3.5f))
    {
        m_flAccuracy = 0.2f;

        if (m_pPlayer->m_iFOV != 90)
        {
            m_pPlayer->m_iFOV = 90;
            m_pPlayer->pev->fov = 90.0f;
        }
        m_pPlayer->SetAnimation(PLAYER_RELOAD);
    }
}

void CG3SG1::SecondaryAttack(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        m_pPlayer->m_iFOV = 40;
        m_pPlayer->pev->fov = 40.0f;
    }
    else if (m_pPlayer->m_iFOV == 40)
    {
        m_pPlayer->m_iFOV = 15;
        m_pPlayer->pev->fov = 15.0f;
    }
    else
    {
        m_pPlayer->m_iFOV = 90;
        m_pPlayer->pev->fov = 90.0f;
    }

    EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2f, ATTN_NORM, 0, PITCH_NORM);
    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

BOOL CG3SG1::UseDecrement(void)
{
    return TRUE;
}

void CG3SG1::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip != 0)
    {
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
        SendWeaponAnim(0, UseDecrement() != FALSE);
    }
}

