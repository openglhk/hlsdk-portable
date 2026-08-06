#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CSG550 : public CBasePlayerWeapon
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

    void SG550Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireSG550;
};

TYPEDESCRIPTION CSG550::m_SaveData[] =
{
    DEFINE_FIELD(CSG550, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CSG550, m_flLastFire, FIELD_TIME),
};

LINK_ENTITY_TO_CLASS(weapon_sg550, CSG550); 

void CSG550::Precache(void)
{
    PRECACHE_MODEL("models/v_sg550.mdl");
    PRECACHE_MODEL("models/w_sg550.mdl");

    PRECACHE_SOUND("weapons/sg550-1.wav");
    PRECACHE_SOUND("weapons/sg550_boltpull.wav");
    PRECACHE_SOUND("weapons/sg550_clipin.wav");
    PRECACHE_SOUND("weapons/sg550_clipout.wav");
    PRECACHE_SOUND("weapons/zoom.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_iShellId = m_iShell;
    m_usFireSG550 = PRECACHE_EVENT(1, "events/sg550.sc");
}

void CSG550::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_sg550");
    Precache();

    m_iId = WEAPON_SG550;
    SET_MODEL(ENT(pev), "models/w_sg550.mdl");

    m_iDefaultAmmo = 30;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CSG550::Deploy(void)
{
    m_flAccuracy = 0.2f;

    return DefaultDeploy("models/v_sg550.mdl", "models/p_sg550.mdl", 4, "rifle", UseDecrement() != FALSE);
}

int CSG550::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "556Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 0;
    p->iPosition = 16;
    p->iFlags = 0;
    p->iId = WEAPON_SG550;
    p->iWeight = 20;
    p->bIsAccessory = false;

    m_iId = WEAPON_SG550;

    return 1;
}

float CSG550::GetMaxSpeed(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        return 189.0f;
    }

    return 135.0f;
}

int CSG550::iItemSlot(void)
{
    return 1;
}

void CSG550::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        SG550Fire((1.0f - m_flAccuracy) * 0.45f, 0.25f, FALSE);
        return;
    }

    float flSpeed = m_pPlayer->pev->velocity.Length2D();

    if (flSpeed > 0.0f)
    {
        SG550Fire(0.15f, 0.25f, FALSE);
        return;
    }

    float flSpreadModifier = 1.0f - m_flAccuracy;

    if (m_pPlayer->pev->flags & FL_DUCKING)
    {
        SG550Fire(flSpreadModifier * 0.04f, 0.25f, FALSE);
    }
    else
    {
        SG550Fire(flSpreadModifier * 0.05f, 0.25f, FALSE);
    }
}

void CSG550::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
    {
        return;
    }

    if (DefaultReload(30, 3, 3.35f))
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

void CSG550::SecondaryAttack(void)
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

void CSG550::SG550Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
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
        float flIdealAccuracy = (gpGlobals->time - m_flLastFire) * 0.35f + 0.65f;
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

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 3, BULLET_PLAYER_556MM, 70, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireSG550, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 5, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;

    float flKickUp = UTIL_SharedRandomFloat(m_pPlayer->random_seed + 4, 1.5f, 1.75f);
    m_pPlayer->pev->punchangle.x -= (m_pPlayer->pev->punchangle.x * 0.25f + flKickUp);

    float flKickSide = UTIL_SharedRandomFloat(m_pPlayer->random_seed + 5, -1.0f, 1.0f);
    m_pPlayer->pev->punchangle.y += flKickSide;
}

BOOL CSG550::UseDecrement(void)
{
    return TRUE;
}

void CSG550::WeaponIdle(void)
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

