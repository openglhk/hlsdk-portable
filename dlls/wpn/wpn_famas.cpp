#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

#define WPNSTATE_FAMAS_BURST (1 << 4)

class CFamas : public CBasePlayerWeapon
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

    void FamasFire(float flSpread, float flCycleTime, BOOL fUseAutoAim, BOOL bFireBurst);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    
    int m_iShotsFired;
    float m_flAccuracy;
    BOOL m_bDelayFire;
    int iShellOn;

    float m_flFamasShoot;
    int m_iFamasShotsFired;
    float m_fBurstSpread;
};

TYPEDESCRIPTION CFamas::m_SaveData[] =
{
    DEFINE_FIELD(CFamas, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CFamas, m_flAccuracy, FIELD_FLOAT),
    DEFINE_FIELD(CFamas, m_bDelayFire, FIELD_BOOLEAN),
    DEFINE_FIELD(CFamas, m_flFamasShoot, FIELD_TIME),
    DEFINE_FIELD(CFamas, m_iFamasShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CFamas, m_fBurstSpread, FIELD_FLOAT),
};

LINK_ENTITY_TO_CLASS(weapon_famas, CFamas);

void CFamas::Precache(void)
{
    PRECACHE_MODEL("models/v_famas_r.mdl");
    PRECACHE_MODEL("models/w_famas.mdl");

    PRECACHE_SOUND("weapons/famas-1.wav");
    PRECACHE_SOUND("weapons/famas-2.wav");
    PRECACHE_SOUND("weapons/famas_clipout.wav");
    PRECACHE_SOUND("weapons/famas_clipin.wav");
    PRECACHE_SOUND("weapons/famas_boltpull.wav");
    PRECACHE_SOUND("weapons/famas_boltslap.wav");
    PRECACHE_SOUND("weapons/famas_forearm.wav");
    PRECACHE_SOUND("weapons/famas-burst.wav");

    m_iShell = PRECACHE_MODEL("models/rshell.mdl");
    m_usFireFamas = PRECACHE_EVENT(1, "events/famas.sc");
}

void CFamas::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_famas");
    Precache();

    m_iId = WEAPON_FAMAS;
    SET_MODEL(ENT(pev), "models/w_famas.mdl");

    m_iDefaultAmmo = 25;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CFamas::Deploy(void)
{
    m_iShotsFired = 0;
    iShellOn = 1;
    m_flAccuracy = 0.2f;

    return DefaultDeploy("models/v_famas_r.mdl", "models/p_famas.mdl", 2, "carbine", UseDecrement() != FALSE);
}

void CFamas::FamasFire(float flSpread, float flCycleTime, BOOL fUseAutoAim, BOOL bFireBurst)
{
    if (bFireBurst == FALSE)
    {
        flSpread = flSpread + 0.01f;
    }
    else
    {
        m_iFamasShotsFired = 0;
    }

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

    int iBulletType = (bFireBurst == FALSE) ? BULLET_PLAYER_556MM : BULLET_PLAYER_556MM; 
    int iDamage = (bFireBurst == FALSE) ? 30 : 34; // 根據散佈判定映射對應 CS 傷害

    FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 2, iBulletType, iDamage, m_pPlayer->random_seed, m_pPlayer->pev);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireFamas, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 10000000.0f), (int)(m_pPlayer->pev->punchangle.y * 10000000.0f), 0, bFireBurst);

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

    if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
    {
        m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1f;

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

    if (bFireBurst == TRUE)
    {
        m_flFamasShoot = gpGlobals->time + 0.05f;
        m_iFamasShotsFired = 1;
        m_fBurstSpread = flSpread;
    }
}

int CFamas::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "556Nato";
    p->iMaxAmmo1 = 75;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 25;
    p->iSlot = 0;
    p->iPosition = 18;
    p->iFlags = 0;
    p->iId = WEAPON_FAMAS;
    p->iWeight = 75;
    p->bIsAccessory = false;

    m_iId = WEAPON_FAMAS;

    return 1;
}

float CFamas::GetMaxSpeed(void)
{
    return 216.0f;
}

int CFamas::iItemSlot( void )
{
    return 1;
}

void CFamas::PrimaryAttack(void)
{
    if (m_pPlayer->pev->waterlevel == 3)
    {
        PlayEmptySound();
        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
        return;
    }

    BOOL bFireBurst = (m_iWeaponState & WPNSTATE_FAMAS_BURST) ? TRUE : FALSE;

    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        FamasFire(0.3f * m_flAccuracy + 0.035f, 0.0825f, FALSE, bFireBurst);
    }
    else
    {
        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed <= 140.0f)
        {
            FamasFire(m_flAccuracy * 0.025f, 0.0825f, FALSE, bFireBurst);
        }
        else
        {
            FamasFire(0.07f * m_flAccuracy + 0.035f, 0.0825f, FALSE, bFireBurst);
        }
    }
}

void CFamas::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(25, 1, 3.3f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);

            m_flAccuracy = 0.0f;
            m_iShotsFired = 0;
            m_bDelayFire = false;
        }
    }
}

int CFamas::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CFamas", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CFamas::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CFamas", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CFamas::SecondaryAttack(void)
{
    if ((m_iWeaponState & WPNSTATE_FAMAS_BURST) == 0)
    {
        ClientPrint(m_pPlayer->pev, HUD_PRINTcenter, "#Switch_To_BurstFire");
        m_iWeaponState |= WPNSTATE_FAMAS_BURST;
    }
    else
    {
        ClientPrint(m_pPlayer->pev, HUD_PRINTcenter, "#Switch_To_FullAuto");
        m_iWeaponState &= ~WPNSTATE_FAMAS_BURST;
    }

    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

BOOL CFamas::UseDecrement(void)
{
    return TRUE;
}

void CFamas::WeaponIdle(void)
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

