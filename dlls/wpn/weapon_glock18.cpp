#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

#define WPNSTATE_GLOCK18_BURST (1 << 1)

class CGLOCK18 : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
	float GetMaxSpeed( void ) override;
    int  iItemSlot( void ) override;
    BOOL Deploy(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;
    void SecondaryAttack(void) override;
    BOOL UseDecrement(void) override;

    void GLOCK18Fire(float flSpread, float flCycleTime, BOOL bFireBurst);

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

public:
    int m_iShell;                     // 模型 pshell.mdl 的索引
    int m_iShellId;                   // 武器基類的彈殼 ID 備份
    unsigned short m_usFireGlock18;   // 註冊客戶端網絡事件的 ID
    
    BOOL m_bBurstFire;                // 是否處於三連發狀態
    int m_iGlock18ShotsFired;         // 三連發模式下已連續射擊的子彈計數
    float m_flGlock18Shoot;           // 三連發模式下下一發子彈的連射時間戳
    
    int m_iShotsFired;                // 單發模式下的開火計數
    float m_flAccuracy;               // 彈道動態精準度因子
    float m_flLastFire;               // 上一次開火的時間點
	
public:
    static TYPEDESCRIPTION m_SaveData[];
};

LINK_ENTITY_TO_CLASS(weapon_glock18, CGLOCK18);

TYPEDESCRIPTION CGLOCK18::m_SaveData[] =
{
    DEFINE_FIELD(CGLOCK18, m_iWeaponState, FIELD_INTEGER),
    DEFINE_FIELD(CGLOCK18, m_bBurstFire, FIELD_BOOLEAN),
    DEFINE_FIELD(CGLOCK18, m_iGlock18ShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CGLOCK18, m_flGlock18Shoot, FIELD_TIME),
    DEFINE_FIELD(CGLOCK18, m_iShotsFired, FIELD_INTEGER),
    DEFINE_FIELD(CGLOCK18, m_flAccuracy, FIELD_FLOAT),
};

void CGLOCK18::Precache(void)
{
    PRECACHE_MODEL("models/v_glock18.mdl");
    PRECACHE_MODEL("models/w_glock18.mdl");

    PRECACHE_SOUND("weapons/glock18-1.wav");
    PRECACHE_SOUND("weapons/glock18-2.wav");
    PRECACHE_SOUND("weapons/clipout1.wav");
    PRECACHE_SOUND("weapons/clipin1.wav");
    PRECACHE_SOUND("weapons/sliderelease1.wav");
    PRECACHE_SOUND("weapons/slideback1.wav");
    PRECACHE_SOUND("weapons/357_cock1.wav");
    PRECACHE_SOUND("weapons/de_clipin.wav");
    PRECACHE_SOUND("weapons/de_clipout.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");
    m_iShellId = m_iShell;

    m_usFireGlock18 = PRECACHE_EVENT(1, "events/glock18.sc");
}

void CGLOCK18::Spawn(void)
{
    Precache();
    m_iId = WEAPON_GLOCK18;
    SET_MODEL(ENT(pev), "models/w_glock18.mdl");

    m_iGlock18ShotsFired = 0;
    m_iDefaultAmmo = 20;
    m_bBurstFire = false;
    m_flGlock18Shoot = 0.0f;
    m_flAccuracy = 0.9f;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CGLOCK18::Deploy(void)
{
    m_bBurstFire = false;
    m_iGlock18ShotsFired = 0;
    m_flGlock18Shoot = 0.0f;
    m_flAccuracy = 0.9f;

    if (RANDOM_LONG(0, 1) != 0)
    {
        return DefaultDeploy("models/v_glock18.mdl", nullptr, 8, "onehanded", UseDecrement() != FALSE);
    }

    return DefaultDeploy("models/v_glock18.mdl", nullptr, 11, "onehanded", UseDecrement() != FALSE);
}

int CGLOCK18::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "9mm";
    p->iMaxAmmo1 = 120;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 20;
    p->iSlot = 1;
    p->iPosition = 2;
    p->iFlags = 0;
    p->iId = WEAPON_GLOCK18;
    p->iWeight = 5;
    p->bIsAccessory = false;

    m_iId = WEAPON_GLOCK18;

    return 1;
}

float CGLOCK18::GetMaxSpeed( void )
{
    return 225.0f;
}

int CGLOCK18::iItemSlot( void )
{
    return 2;
}

void CGLOCK18::GLOCK18Fire(float flSpread, float flCycleTime, BOOL bFireBurst)
{
    if (bFireBurst == 1)
    {
        m_iGlock18ShotsFired = 0;
    }
    else
    {
        m_iShotsFired++;
        if (m_iShotsFired > 1)
        {
            return;
        }
        flCycleTime = flCycleTime - 0.05f;
    }

    if (m_flLastFire == 0.0f)
    {
        m_flLastFire = gpGlobals->time;
    }
    else
    {
        m_flAccuracy = m_flAccuracy - (0.325f - (gpGlobals->time - m_flLastFire)) * 0.275f;

        if (m_flAccuracy <= 0.9f)
        {
            if (m_flAccuracy < 0.6f)
                m_flAccuracy = 0.6f;
        }
        else
        {
            m_flAccuracy = 0.9f;
        }

        m_flLastFire = gpGlobals->time;
    }

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

        m_pPlayer->m_iWeaponVolume = 500;
        m_pPlayer->m_iWeaponFlash = 256;

        Vector vecAiming;
        m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

        Vector vecSrc = m_pPlayer->GetGunPosition();

        FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);

        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireGlock18, 0.0f, g_vecZero, g_vecZero, flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), (int)(m_pPlayer->pev->punchangle.y * 100.0f), (m_iClip == 0), 0);

        m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flSpread;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;

        if (flCycleTime == 0.0f)
        {
            m_flGlock18Shoot = gpGlobals->time + 0.1f;
            m_iGlock18ShotsFired++;
        }
    }
}

void CGLOCK18::PrimaryAttack(void)
{
    if ((m_iWeaponState & WPNSTATE_GLOCK18_BURST) == 0)
    {
        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            GLOCK18Fire(1.0f - m_flAccuracy, 0.2f, FALSE);
        }
        else
        {
            float flSpeed = m_pPlayer->pev->velocity.Length2D();

            if (flSpeed > 0.0f)
            {
                GLOCK18Fire((1.0f - m_flAccuracy) * 0.165f, 0.2f, FALSE);
                return;
            }

            float flSpreadFactor = 1.0f - m_flAccuracy;

            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                GLOCK18Fire(flSpreadFactor * 0.1f, 0.2f, FALSE);
                return;
            }

            GLOCK18Fire(flSpreadFactor * 0.075f, 0.2f, FALSE);
        }
    }
    else
    {
        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            GLOCK18Fire((1.0f - m_flAccuracy) * 1.2f, 0.5f, TRUE);
            return;
        }

        float flSpeed = m_pPlayer->pev->velocity.Length2D();

        if (flSpeed > 0.0f)
        {
            GLOCK18Fire((1.0f - m_flAccuracy) * 0.185f, 0.5f, TRUE);
            return;
        }

        float flSpreadFactor = 1.0f - m_flAccuracy;

        if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
        {
            GLOCK18Fire(flSpreadFactor * 0.3f, 0.5f, TRUE);
        }
        else
        {
            GLOCK18Fire(flSpreadFactor * 0.095f, 0.5f, TRUE);
        }
    }
}

void CGLOCK18::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        int iResult;

        if (RANDOM_LONG(0, 1) == 0)
        {
            iResult = DefaultReload(20, 12, 2.2f);
        }
        else
        {
            iResult = DefaultReload(20, 7, 2.2f);
        }

        if (iResult != 0)
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.9f;
        }
    }
}

int CGLOCK18::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CGLOCK18", this, m_SaveData, 1);
}

int CGLOCK18::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CGLOCK18", this, m_SaveData, 1);
}

void CGLOCK18::SecondaryAttack(void)
{
    if ((m_iWeaponState & WPNSTATE_GLOCK18_BURST) == 0)
    {
        ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Switch_To_BurstFire");
        m_iWeaponState |= WPNSTATE_GLOCK18_BURST;
    }
    else
    {
        ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Switch_To_SemiAuto");
        m_iWeaponState &= ~WPNSTATE_GLOCK18_BURST;
    }

    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

BOOL CGLOCK18::UseDecrement(void)
{
    return TRUE;
}

void CGLOCK18::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip != 0)
    {
        int iAnim;
        float flRand = RANDOM_FLOAT(0.0f, 1.0f);

        if (flRand <= 0.3f)
        {
            iAnim = 2;
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0625f;
        }
        else if (flRand > 0.6f)
        {
            iAnim = 1;
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
        }
        else
        {
            iAnim = 0;
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.75f;
        }

        SendWeaponAnim(iAnim, UseDecrement() != FALSE);
    }
}

