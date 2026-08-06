#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

// 狀態機巨集定義（對齊 ReGameDLL_CS）
#define WPNSTATE_ELITE_LEFT (1 << 3)

class CELITE : public CBasePlayerWeapon
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
    BOOL UseDecrement(void) override;

    void ELITEFire(float flSpread, float flCycleTime, BOOL fUseSemi);

public:
    int m_iShell;
    unsigned short m_usFireELITE_LEFT;
    unsigned short m_usFireELITE_RIGHT;
    
    int m_iShotsFired;
    float m_flAccuracy;
    float m_flLastFire;
};

LINK_ENTITY_TO_CLASS(weapon_elite, CELITE);

void CELITE::Precache(void)
{
    PRECACHE_MODEL("models/v_elite.mdl");
    PRECACHE_MODEL("models/w_elite.mdl");

    PRECACHE_SOUND("weapons/elite_fire.wav");
    PRECACHE_SOUND("weapons/elite_reloadstart.wav");
    PRECACHE_SOUND("weapons/elite_leftclipin.wav");
    PRECACHE_SOUND("weapons/elite_clipout.wav");
    PRECACHE_SOUND("weapons/elite_sliderelease.wav");
    PRECACHE_SOUND("weapons/elite_rightclipin.wav");
    PRECACHE_SOUND("weapons/elite_deploy.wav");

    m_iShell = PRECACHE_MODEL("models/pshell.mdl");

    m_usFireELITE_LEFT = PRECACHE_EVENT(1, "events/elite_left.sc");
    m_usFireELITE_RIGHT = PRECACHE_EVENT(1, "events/elite_right.sc");
}

void CELITE::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_elite");
    Precache();

    m_iId = WEAPON_ELITE;
    SET_MODEL(ENT(pev), "models/w_elite.mdl");

    m_iDefaultAmmo = 30;
    m_flAccuracy = 0.88f;
    m_bIsAccessory = false;

    FallInit();
}

BOOL CELITE::Deploy(void)
{
    m_flAccuracy = 0.88f;

    if ((m_iClip & 1) == 0)
    {
        m_iWeaponState |= WPNSTATE_ELITE_LEFT;
    }

    return DefaultDeploy("models/v_elite.mdl", nullptr, 15, "dualpistols", UseDecrement() != FALSE);
}

void CELITE::ELITEFire(float flSpread, float flCycleTime, BOOL fUseSemi)
{
    m_iShotsFired++;

    if (m_iShotsFired < 2)
    {
        if (m_flLastFire == 0.0f)
        {
            m_flLastFire = gpGlobals->time;
        }
        else
        {
            m_flAccuracy = m_flAccuracy - (0.325f - (gpGlobals->time - m_flLastFire)) * 0.275f;

            if (m_flAccuracy <= 0.88f)
            {
                if (m_flAccuracy < 0.55f)
                    m_flAccuracy = 0.55f;
            }
            else
            {
                m_flAccuracy = 0.88f;
            }

            m_flLastFire = gpGlobals->time;
        }

        if (m_iClip > 0)
        {
            m_iClip--;

            m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime - 0.125f;

            m_pPlayer->m_iWeaponVolume = 500;
            m_pPlayer->m_iWeaponFlash = 128;

            Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
            UTIL_MakeVectors(vecAngles);

            m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

            Vector vecAiming;
            m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

            Vector vecSrc = m_pPlayer->GetGunPosition();
            unsigned short usEvent;

            if ((m_iClip & 1) == 0)
            {
                m_pPlayer->SetAnimation(PLAYER_ATTACK1);
                m_iWeaponState &= ~WPNSTATE_ELITE_LEFT;

                vecSrc = vecSrc - gpGlobals->v_right * 5.0f;

                FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);
                usEvent = m_usFireELITE_LEFT;
            }
            else
            {
                m_pPlayer->SetAnimation(PLAYER_ATTACK2);
                m_iWeaponState |= WPNSTATE_ELITE_LEFT;

                vecSrc = vecSrc + gpGlobals->v_right * 5.0f;

                FireBullets3(vecSrc, vecAiming, flSpread, 8192.0f, 1, BULLET_PLAYER_9MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);
                usEvent = m_usFireELITE_RIGHT;
            }

            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), usEvent, 0.0f, g_vecZero, g_vecZero, flSpread, 0.0f, (int)(vecAngles.x * 100.0f), m_iClip, 0, 0);

            if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
            {
                m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
            }

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
            m_pPlayer->pev->punchangle.x -= 2.0f;
            return;
        }

        if (m_fFireOnEmpty)
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
        }
    }
}

int CELITE::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "9mm";
    p->iMaxAmmo1 = 120;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 30;
    p->iSlot = 1;
    p->iPosition = 5;
    p->iFlags = 0;
    p->iId = WEAPON_ELITE;
    p->iWeight = 5;
    p->bIsAccessory = false;

    m_iId = WEAPON_ELITE;

    return 1;
}

float CELITE::GetMaxSpeed( void )
{
    return 225.0f;
}

int CELITE::iItemSlot( void )
{
    return 2;
}

void CELITE::PrimaryAttack(void)
{
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        m_iShotsFired++;

        if (m_iShotsFired < 2)
        {
            if (m_flLastFire == 0.0f)
            {
                m_flLastFire = gpGlobals->time;
            }
            else
            {
                m_flAccuracy = m_flAccuracy - (0.325f - (gpGlobals->time - m_flLastFire)) * 0.275f;

                if (m_flAccuracy <= 0.88f)
                {
                    if (m_flAccuracy < 0.55f)
                        m_flAccuracy = 0.55f;
                }
                else
                {
                    m_flAccuracy = 0.88f;
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

                m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.075f;

                m_pPlayer->m_iWeaponVolume = 500;
                m_pPlayer->m_iWeaponFlash = 128;

                Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
                UTIL_MakeVectors(vecAngles);

                m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

                Vector vecAiming;
                m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

                Vector vecSrc = m_pPlayer->GetGunPosition();
                unsigned short usEvent;

                if ((m_iClip & 1) == 0)
                {
                    m_pPlayer->SetAnimation(PLAYER_ATTACK1);
                    m_iWeaponState &= ~WPNSTATE_ELITE_LEFT;

                    vecSrc = vecSrc - gpGlobals->v_right * 5.0f;

                    FireBullets3(vecSrc, vecAiming, 1.0f - m_flAccuracy, 8192.0f, 1, BULLET_PLAYER_9MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);
                    usEvent = m_usFireELITE_LEFT;
                }
                else
                {
                    m_pPlayer->SetAnimation(PLAYER_ATTACK2);
                    m_iWeaponState |= WPNSTATE_ELITE_LEFT;

                    vecSrc = vecSrc + gpGlobals->v_right * 5.0f;

                    FireBullets3(vecSrc, vecAiming, 1.0f - m_flAccuracy, 8192.0f, 1, BULLET_PLAYER_9MM, 21, m_pPlayer->random_seed, m_pPlayer->pev);
                    usEvent = m_usFireELITE_RIGHT;
                }

                PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), usEvent, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, (int)(vecAngles.x * 100.0f), m_iClip, 0, 0);

                if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
                {
                    m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
                }

                m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
                m_pPlayer->pev->punchangle.x -= 2.0f;
            }
        }
    }
}

void CELITE::Reload(void)
{
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        if (DefaultReload(30, 14, 4.5f))
        {
            m_pPlayer->SetAnimation(PLAYER_RELOAD);
            m_flAccuracy = 0.88f;
        }
    }
}

BOOL CELITE::UseDecrement(void)
{
    return TRUE;
}

void CELITE::WeaponIdle(void)
{
    ResetEmptySound();

    Vector vecAiming;
    m_pPlayer->GetAutoaimVector(vecAiming, 0.17364818f);

    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip != 0)
    {
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
        
        int iAnim = (m_iClip == 1) ? 1 : 0;
        SendWeaponAnim(iAnim, UseDecrement() != FALSE);
    }
}

