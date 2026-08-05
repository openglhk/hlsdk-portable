#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CAWP : public CBasePlayerWeapon
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

    void AWPFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireAWP;
};

TYPEDESCRIPTION CAWP::m_SaveData[] =
{
    // AWP 開鏡及彈夾的核心狀態已由基底類別 CBasePlayerWeapon 自動繼承處理
    DEFINE_FIELD(CAWP, m_iShell, FIELD_INTEGER),
};
//try use this?
//IMPLEMENT_SAVERESTORE(CSG550, CBasePlayerWeapon);
LINK_ENTITY_TO_CLASS(weapon_awp, CAWP);

void CAWP::Precache(void)
{
    PRECACHE_MODEL("models/v_awp.mdl");
    PRECACHE_MODEL("models/w_awp.mdl");
    PRECACHE_MODEL("models/p_awp.mdl");

    PRECACHE_SOUND("weapons/awp1.wav");
    PRECACHE_SOUND("weapons/boltpull1.wav");
    PRECACHE_SOUND("weapons/boltup.wav");
    PRECACHE_SOUND("weapons/boltdown.wav");
    PRECACHE_SOUND("weapons/zoom.wav");
    PRECACHE_SOUND("weapons/awp_deploy.wav");
    PRECACHE_SOUND("weapons/awp_clipin.wav");
    PRECACHE_SOUND("weapons/awp_clipout.wav");

    m_iShell = PRECACHE_MODEL("models/rshell_big.mdl");
    m_iShellId = m_iShell;
    m_usFireAWP = PRECACHE_EVENT(1, "events/awp.sc");
}

void CAWP::Spawn(void)
{
    Precache();

    m_iId = WEAPON_AWP;
    SET_MODEL(ENT(pev), "models/w_awp.mdl");

    m_iDefaultAmmo = 10;
    m_bIsAccessory = false;

    FallInit();
}

void CAWP::AWPFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        flSpread += 0.08f;
    }
    else
    {
        m_pPlayer->m_bResumeZoom = true;
        m_pPlayer->m_iLastZoom = m_pPlayer->m_iFOV;
        m_pPlayer->m_iFOV = 90;
        m_pPlayer->pev->fov = 90.0f;
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

    m_pPlayer->m_flEjectBrass = gpGlobals->time + 0.55f;
    m_pPlayer->m_iWeaponVolume = 2250;
    m_pPlayer->m_iWeaponFlash = 256;

    Vector vecAiming = gpGlobals->v_forward;
    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 3, BULLET_PLAYER_338MAG, 115, m_pPlayer->random_seed, m_pPlayer->pev);

    int iPunchX = (int)(m_pPlayer->pev->punchangle.x * 100.0f);
    int iPunchY = (int)(m_pPlayer->pev->punchangle.y * 100.0f);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireAWP, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, iPunchX, iPunchY, 0, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
    m_pPlayer->pev->punchangle.x -= 2.0f;
}

BOOL CAWP::Deploy(void)
{
    return DefaultDeploy("models/v_awp.mdl", "models/p_awp.mdl", 5, "rifle", UseDecrement() != FALSE);
}

int CAWP::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "338Magnum";
    p->iMaxAmmo1 = 30;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 10;
    p->iSlot = 0;
    p->iPosition = 2;
    p->iFlags = 0;
    p->iId = WEAPON_AWP;
    p->iWeight = 30;
    p->bIsAccessory = false;

    m_iId = WEAPON_AWP;

    return 1;
}

float CAWP::GetMaxSpeed(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        return 189.0f;
    }

    return 135.0f;
}

int CAWP::iItemSlot(void)
{
    return 1;
}

void CAWP::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        AWPFire(0.85f, 1.45f, FALSE);
        return;
    }

    float flSpeed = m_pPlayer->pev->velocity.Length2D();

    if (flSpeed > 140.0f)
    {
        AWPFire(0.25f, 1.45f, FALSE);
    }
    else if (flSpeed <= 10.0f)
    {
        if (m_pPlayer->pev->flags & FL_DUCKING)
        {
            AWPFire(0.0f, 1.45f, FALSE);
        }
        else
        {
            AWPFire(0.001f, 1.45f, FALSE);
        }
    }
    else
    {
        AWPFire(0.1f, 1.45f, FALSE);
    }
}

void CAWP::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
    {
        return;
    }

    if (DefaultReload(10, 4, 2.5f))
    {
        if (m_pPlayer->m_iFOV != 90)
        {
            m_pPlayer->m_iFOV = 90;
            m_pPlayer->pev->fov = 90.0f;
        }
        m_pPlayer->SetAnimation(PLAYER_RELOAD);
    }
}

void CAWP::SecondaryAttack(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        m_pPlayer->m_iFOV = 40;
        m_pPlayer->pev->fov = 40.0f;
    }
    else if (m_pPlayer->m_iFOV == 40)
    {
        m_pPlayer->m_iFOV = 10;
        m_pPlayer->pev->fov = 10.0f;
    }
    else
    {
        m_pPlayer->m_iFOV = 90;
        m_pPlayer->pev->fov = 90.0f;
    }

    EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2f, ATTN_NORM, 0, PITCH_NORM);
    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

BOOL CAWP::UseDecrement(void)
{
    return TRUE;
}

void CAWP::WeaponIdle(void)
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
