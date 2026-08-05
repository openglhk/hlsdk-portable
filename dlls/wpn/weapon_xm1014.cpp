#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CXM1014 : public CBasePlayerWeapon
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

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    static TYPEDESCRIPTION m_SaveData[];

public:
    int m_iShell;
    unsigned short m_usFireXM1014;
    
    float m_flPumpTime;
};

TYPEDESCRIPTION CXM1014::m_SaveData[] =
{
    DEFINE_FIELD(CXM1014, m_fInSpecialReload, FIELD_INTEGER),
    DEFINE_FIELD(CXM1014, m_flPumpTime, FIELD_TIME),
};

LINK_ENTITY_TO_CLASS(weapon_xm1014, CXM1014);

void CXM1014::Precache(void)
{
    PRECACHE_MODEL("models/v_xm1014.mdl");
    PRECACHE_MODEL("models/w_xm1014.mdl");

    m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl");
    m_iShellId = m_iShell;

    PRECACHE_SOUND("weapons/xm1014-1.wav");
    PRECACHE_SOUND("weapons/reload1.wav");
    PRECACHE_SOUND("weapons/reload3.wav");

    m_usFireXM1014 = PRECACHE_EVENT(1, "events/xm1014.sc");
}

void CXM1014::Spawn(void)
{
    Precache();

    m_iId = WEAPON_XM1014;
    pev->classname = MAKE_STRING("weapon_xm1014");
    SET_MODEL(ENT(pev), "models/w_xm1014.mdl");

    m_iDefaultAmmo = 7;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CXM1014::Deploy(void)
{
    return DefaultDeploy("models/v_xm1014.mdl", nullptr, 6, "m249", UseDecrement() != FALSE);
}

int CXM1014::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "buckshot";
    p->iMaxAmmo1 = 32;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 7;
    p->iSlot = 0;
    p->iPosition = 12;
    p->iFlags = 0;
    p->iId = WEAPON_XM1014;
    p->iWeight = 20;
    p->bIsAccessory = false;

    m_iId = WEAPON_XM1014;

    return 1;
}

float CXM1014::GetMaxSpeed(void)
{
    return 216.0f;
}

int CXM1014::iItemSlot(void)
{
    return 1;
}

void CXM1014::PrimaryAttack(void)
{
    if (m_pPlayer->pev->waterlevel == 3)
    {
        PlayEmptySound();
        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
        return;
    }

    m_pPlayer->m_iWeaponVolume = 2250;
    m_pPlayer->m_iWeaponFlash = 512;

    m_iClip--;

    m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.087155744f);

    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecSpread = Vector(0.0725f, 0.0725f, 0.0f);

    FireBullets(6, vecSrc, gpGlobals->v_forward, vecSpread, 2048.0f, BULLET_PLAYER_BUCKSHOT, 0, (int)gSkillData.plrDmgBuckshot, m_pPlayer->pev, m_pPlayer->random_seed);

    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireXM1014, 0.0f, g_vecZero, g_vecZero, m_vVecAiming.x, m_vVecAiming.y, 7, (int)(m_vVecAiming.x * 100.0f), (m_iClip == 0), 0);

    if (m_iClip != 0 || (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 && (m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0), m_iClip != 0)))
    {
        m_flPumpTime = UTIL_WeaponTimeBase() + 0.125f;
    }

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.25f;

    if (m_iClip == 0)
    {
        m_flTimeWeaponIdle = 0.75f;
    }
    else
    {
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.25f;
    }

    m_fInSpecialReload = 0;

    int iMinRecoil, iMaxRecoil;
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        iMinRecoil = 7;
        iMaxRecoil = 10;
    }
    else
    {
        iMinRecoil = 3;
        iMaxRecoil = 5;
    }

    int iRecoil = UTIL_SharedRandomLong(m_pPlayer->random_seed + 1, iMinRecoil, iMaxRecoil);
    m_pPlayer->pev->punchangle.x -= (float)iRecoil;
}

void CXM1014::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 && m_iClip != 7)
    {
        if (m_flNextPrimaryAttack <= UTIL_WeaponTimeBase())
        {
            if (m_fInSpecialReload == 0)
            {
                m_pPlayer->SetAnimation(PLAYER_RELOAD);
                SendWeaponAnim(5, UseDecrement() != FALSE);
                m_fInSpecialReload = 1;

                m_pPlayer->m_flNextAttack = m_flTimeWeaponIdle = m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.55f;
            }
            else
            {
                if (m_fInSpecialReload == 1)
                {
                    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
                    {
                        m_fInSpecialReload = 2;

                        const char *sample;
                        if (RANDOM_LONG(0, 1) == 0)
                        {
                            sample = "weapons/reload3.wav";
                        }
                        else
                        {
                            sample = "weapons/reload1.wav";
                        }

                        int pitch = 85 + RANDOM_LONG(0, 31);
                        EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, sample, 1.0f, ATTN_NORM, 0, pitch);

                        SendWeaponAnim(3, UseDecrement() != FALSE);
                        m_flNextReload = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.3f;
                    }
                }
                else
                {
                    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
                    {
                        m_fInSpecialReload = 1;
                        m_iClip++;
                        m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
                        m_pPlayer->ammo_buckshot--;
                    }
                }
            }
        }
    }
}

int CXM1014::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CXM1014", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CXM1014::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CXM1014", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

BOOL CXM1014::UseDecrement(void)
{
    return TRUE;
}

void CXM1014::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.087155744f);

    if (m_flPumpTime != 0.0f && m_flPumpTime < UTIL_WeaponTimeBase())
    {
        m_flPumpTime = 0.0f;
    }

    if (UTIL_WeaponTimeBase() <= m_flTimeWeaponIdle)
    {
        return;
    }

    if (m_iClip == 0)
    {
        if (m_fInSpecialReload == 0)
        {
            if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
            {
                SendWeaponAnim(0, UseDecrement() != FALSE);
                m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
                return;
            }

            Reload();
            return;
        }
    }
    else
    {
        if (m_fInSpecialReload == 0)
        {
            SendWeaponAnim(0, UseDecrement() != FALSE);
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
            return;
        }

        if (m_iClip == 7)
        {
            SendWeaponAnim(4, UseDecrement() != FALSE);
            m_fInSpecialReload = 0;
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
            return;
        }
    }

    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
    {
        SendWeaponAnim(4, UseDecrement() != FALSE);
        m_fInSpecialReload = 0;
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
        return;
    }

    Reload();
}

