#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CFlashbang : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;
    float GetMaxSpeed(void) override;

    BOOL CanDeploy(void) override;
    BOOL CanDrop(void) override;
    BOOL CanHolster(void) override;

    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;
    BOOL UseDecrement(void) override;
};

LINK_ENTITY_TO_CLASS(weapon_flashbang, CFlashbang); 

void CFlashbang::Precache(void)
{
    PRECACHE_MODEL("models/v_flashbang.mdl");
    PRECACHE_MODEL("models/w_flashbang.mdl");

    PRECACHE_SOUND("weapons/flashbang-1.wav");
    PRECACHE_SOUND("weapons/flashbang-2.wav");
    PRECACHE_SOUND("weapons/pinpull.wav");
}

void CFlashbang::Spawn(void)
{
    Precache();

    m_iId = WEAPON_FLASHBANG;
    SET_MODEL(ENT(pev), "models/w_flashbang.mdl");

    pev->dmg = 4.0f;
    m_iDefaultAmmo = 1;
    m_flStartThrow = 0.0f;
    m_flReleaseThrow = -1.0f;
    m_bIsAccessory = true;

    FallInit();
}

BOOL CFlashbang::CanDeploy(void)
{
    return m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0;
}

BOOL CFlashbang::CanDrop(void)
{
    return FALSE;
}

BOOL CFlashbang::CanHolster(void)
{
    return TRUE;
}

BOOL CFlashbang::Deploy(void)
{
    m_flStartThrow = 0.0f;
    m_flReleaseThrow = -1.0f;

    return DefaultDeploy("models/v_flashbang.mdl", "models/p_flashbang.mdl", 3, "grenade", UseDecrement() != FALSE);
}

int CFlashbang::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "Flashbang";
    p->iMaxAmmo1 = 3;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = WEAPON_NOCLIP;
    p->iSlot = 3;
    p->iPosition = 2;
    p->iId = WEAPON_FLASHBANG;
    p->iWeight = 1;
    p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
    p->bIsAccessory = true;

    m_iId = WEAPON_FLASHBANG;

    return 1;
}

float CFlashbang::GetMaxSpeed(void)
{
    return 225.0f;
}

void CFlashbang::Holster(int skiplocal)
{
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;

    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
    {
        m_pPlayer->m_iAccessoryBits &= ~ACCESSORY_FLASHBANG; // 0xffffffef (1 << 4) 清除投擲物狀態欄
        DestroyItem();
    }

    m_flReleaseThrow = 0.0f;
    m_flStartThrow = 0.0f;

    EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "common/null.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);
}

int CFlashbang::iItemSlot(void)
{
    return 4;
}

void CFlashbang::PrimaryAttack(void)
{
    if (m_flStartThrow == 0.0f && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        m_flReleaseThrow = 0.0f;
        m_flStartThrow = gpGlobals->time;

        SendWeaponAnim(1, UseDecrement() != FALSE);
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5f;
    }
}

BOOL CFlashbang::UseDecrement(void)
{
    return TRUE;
}

void CFlashbang::WeaponIdle(void)
{
    if (m_flReleaseThrow == 0.0f && m_flStartThrow != 0.0f)
    {
        m_flReleaseThrow = gpGlobals->time;
    }

    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
    {
        if (m_flStartThrow == 0.0f)
        {
            if (m_flReleaseThrow <= 0.0f)
            {
                if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0)
                {
                    if (UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0.0f, 1.0f) > 0.75f)
                    {
                        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
                    }
                    else
                    {
                        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10.0f, 15.0f);
                    }

                    SendWeaponAnim(0, UseDecrement() != FALSE);
                }
            }
            else
            {
                m_flStartThrow = 0.0f;

                if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0)
                {
                    SendWeaponAnim(3, UseDecrement() != FALSE);
                    m_flReleaseThrow = -1.0f;
                    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10.0f, 15.0f);
                    return;
                }

                RetractWeapon();
            }
        }
        else
        {
            Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

            if (angThrow.x < 0.0f)
            {
                angThrow.x = angThrow.x * 0.8888889f;
            }
            else
            {
                angThrow.x = angThrow.x * 1.1111112f;
            }

            angThrow.x -= 10.0f;
            float flVel = (90.0f - angThrow.x) * 6.0f;

            if (flVel > 750.0f)
            {
                flVel = 750.0f;
            }

            UTIL_MakeVectors(angThrow);

            Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16.0f;
            Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

            // 呼叫手榴彈基底類別的核心計時拋體生成函數
            CGrenade::ShootTimed(m_pPlayer->pev, vecSrc, vecThrow, 1.5f);

            SendWeaponAnim(2, UseDecrement() != FALSE);
            m_pPlayer->SetAnimation(PLAYER_ATTACK1);

            m_flStartThrow = 0.0f;
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5f;
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75f;

            m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

            if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
            {
                m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5f;
                return;
            }
        }
    }
}
