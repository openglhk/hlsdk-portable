#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CSCOUT : public CBasePlayerWeapon
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

    void SCOUTFire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireScout;
};

TYPEDESCRIPTION CSCOUT::m_SaveData[] =
{
    // Scout 核心狀態已由基底類別繼承處理，此處保留武器類別標記空間
    DEFINE_FIELD(CSCOUT, m_iShell, FIELD_INTEGER),
};

LINK_ENTITY_TO_CLASS(weapon_scout, CSCOUT);

void CSCOUT::Precache(void)
{
    PRECACHE_MODEL("models/v_scout.mdl");
    PRECACHE_MODEL("models/w_scout.mdl");

    PRECACHE_SOUND("weapons/scout_fire-1.wav");
    PRECACHE_SOUND("weapons/scout_bolt.wav");
    PRECACHE_SOUND("weapons/scout_clipin.wav");
    PRECACHE_SOUND("weapons/scout_clipout.wav");
    PRECACHE_SOUND("weapons/zoom.wav");

    m_iShell = PRECACHE_MODEL("models/rshell_big.mdl");
    m_iShellId = m_iShell;
    m_usFireScout = PRECACHE_EVENT(1, "events/scout.sc");
}

void CSCOUT::Spawn(void)
{
    Precache();

    m_iId = WEAPON_SCOUT;
    SET_MODEL(ENT(pev), "models/w_scout.mdl");

    m_iDefaultAmmo = 10;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CSCOUT::Deploy(void)
{
    return DefaultDeploy("models/v_scout.mdl", "models/p_scout.mdl", 4, "rifle", UseDecrement() != FALSE);
}

int CSCOUT::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "762Nato";
    p->iMaxAmmo1 = 90;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 10;
    p->iSlot = 0;
    p->iPosition = 9;
    p->iFlags = 0;
    p->iId = WEAPON_SCOUT;
    p->iWeight = 30;
    p->bIsAccessory = false;

    m_iId = WEAPON_SCOUT;

    return 1;
}

float CSCOUT::GetMaxSpeed(void)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        return 225.0f;
    }

    return 198.0f;
}

int CSCOUT::iItemSlot(void)
{
    return 1;
}

void CSCOUT::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        if (m_pPlayer->m_iFOV != 90)
        {
            m_pPlayer->m_bResumeZoom = true;
            m_pPlayer->m_iLastZoom = m_pPlayer->m_iFOV;
            m_pPlayer->m_iFOV = 90;
        }

        if (m_iClip > 0)
        {
            m_iClip--;
            m_pPlayer->SetAnimation(PLAYER_ATTACK1);

            Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
            UTIL_MakeVectors(vecAngles);

            m_pPlayer->m_flEjectBrass = gpGlobals->time + 0.56f;
            m_pPlayer->m_iWeaponVolume = 800;
            m_pPlayer->m_iWeaponFlash = 256;

            Vector vecAiming = gpGlobals->v_forward;
            Vector vecSrc = m_pPlayer->GetGunPosition();

            FireBullets3(vecSrc, vecAiming, 0.2f, 8192.0f, 1, BULLET_PLAYER_762MM, 50, m_pPlayer->random_seed, m_pPlayer->pev);

            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireScout, 0.0f, g_vecZero, m_pPlayer->pev->angles, 0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

            m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.25f;

            if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
            {
                m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
            }

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;
            m_pPlayer->pev->punchangle.x -= 2.0f;
            return;
        }

        if (m_fFireOnEmpty)
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
        }
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 170.0f)
        {
            SCOUTFire(0.075f, 1.25f, FALSE);
        }
        else if (m_pPlayer->pev->flags & FL_DUCKING)
        {
            SCOUTFire(0.0f, 1.25f, FALSE);
        }
        else
        {
            SCOUTFire(0.007f, 1.25f, FALSE);
        }
    }
}

void CSCOUT::Reload(void)
{
    if (DefaultReload(10, 3, 2.0f))
    {
        if (m_pPlayer->m_iFOV != 90)
        {
            m_pPlayer->m_iFOV = 90;
            m_pPlayer->pev->fov = 90.0f;
        }
        m_pPlayer->SetAnimation(PLAYER_RELOAD);
    }
}

void CSCOUT::SCOUTFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    if (m_pPlayer->m_iFOV == 90)
    {
        flSpread += 0.025f;
    }
    else
    {
        m_pPlayer->m_bResumeZoom = true;
        m_pPlayer->m_iLastZoom = m_pPlayer->m_iFOV;
        m_pPlayer->m_iFOV = 90;
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
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    m_pPlayer->m_flEjectBrass = gpGlobals->time + 0.56f;
    m_pPlayer->m_iWeaponVolume = 800;
    m_pPlayer->m_iWeaponFlash = 256;

    Vector vecAiming = gpGlobals->v_forward;
    Vector vecSrc = m_pPlayer->GetGunPosition();

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_762MM, 50, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireScout, 0.0f, g_vecZero, m_pPlayer->pev->angles, 0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;
    m_pPlayer->pev->punchangle.x -= 2.0f;
}

void CSCOUT::SecondaryAttack(void)
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
    else if (m_pPlayer->m_iFOV == 15)
    {
        m_pPlayer->m_iFOV = 90;
        m_pPlayer->pev->fov = 90.0f;
    }

    EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2f, ATTN_NORM, 0, PITCH_NORM);
    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

BOOL CSCOUT::UseDecrement(void)
{
    return TRUE;
}

void CSCOUT::WeaponIdle(void)
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
