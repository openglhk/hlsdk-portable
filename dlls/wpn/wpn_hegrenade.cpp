#pragma once

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CHEGrenade : public CBasePlayerWeapon
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

public:
    // 高爆手榴彈專屬全域網路事件（對齊 field_0x10e 記憶體偏移量）
    unsigned short m_usCreateExplosion; 
};

LINK_ENTITY_TO_CLASS(weapon_hegrenade, CHEGrenade); 

void CHEGrenade::Precache(void)
{
    PRECACHE_MODEL("models/v_hegrenade.mdl");
    PRECACHE_MODEL("models/w_hegrenade.mdl");

    PRECACHE_SOUND("weapons/hegrenade-1.wav");
    PRECACHE_SOUND("weapons/hegrenade-2.wav");
    PRECACHE_SOUND("weapons/he_bounce-1.wav");
    PRECACHE_SOUND("weapons/pinpull.wav");

    m_usCreateExplosion = PRECACHE_EVENT(1, "events/createexplo.sc");
}

void CHEGrenade::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_hegrenade");
    Precache();

    m_iId = WEAPON_HEGRENADE;
    SET_MODEL(ENT(pev), "models/w_hegrenade.mdl");

    pev->dmg = 4.0f;
    m_iDefaultAmmo = 1;
    m_flStartThrow = 0.0f;
    m_flReleaseThrow = -1.0f;
    m_bIsAccessory = true;

    FallInit();
}

BOOL CHEGrenade::CanDeploy(void)
{
    return m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0;
}

BOOL CHEGrenade::CanDrop(void)
{
    return FALSE;
}

BOOL CHEGrenade::CanHolster(void)
{
    if (m_flStartThrow != 0.0f)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CHEGrenade::Deploy(void)
{
    m_flStartThrow = 0.0f;
    m_flReleaseThrow = -1.0f;

    return DefaultDeploy("models/v_hegrenade.mdl", "models/p_hegrenade.mdl", 3, "grenade", UseDecrement() != FALSE);
}

int CHEGrenade::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "HEGrenade";
    p->iMaxAmmo1 = 3;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = WEAPON_NOCLIP;
    p->iSlot = 3;
    p->iPosition = 1;
    p->iId = WEAPON_HEGRENADE;
    p->iWeight = 2;
    p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
    p->bIsAccessory = true;

    m_iId = WEAPON_HEGRENADE;

    return 1;
}

float CHEGrenade::GetMaxSpeed(void)
{
    return 225.0f;
}

void CHEGrenade::Holster(int skiplocal)
{
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;

    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
    {
        m_pPlayer->m_iAccessoryBits &= ~ACCESSORY_HEGRENADE; // 0xffffffdf (1 << 5) 清除高爆手榴彈狀態
        DestroyItem();
    }

    m_flReleaseThrow = 0.0f;
    m_flStartThrow = 0.0f;

    EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "common/null.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);
}

int CHEGrenade::iItemSlot(void)
{
    return 4;
}

void CHEGrenade::PrimaryAttack(void)
{
    if (m_flStartThrow == 0.0f && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        m_flReleaseThrow = 0.0f;
        m_flStartThrow = gpGlobals->time;

        SendWeaponAnim(1, UseDecrement() != FALSE);
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5f;
    }
}

BOOL CHEGrenade::UseDecrement(void)
{
    return TRUE;
}

void CHEGrenade::WeaponIdle(void)
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
                    SendWeaponAnim(0, UseDecrement() != FALSE);
                    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10.0f, 15.0f);
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

            // 呼叫破片手榴彈專屬的定時拋體生成函數，並傳入儲存在 field_0x10e 的 m_usCreateExplosion 事件 ID 與陣營
            CGrenade::ShootTimed2(m_pPlayer->pev, vecSrc, vecThrow, 1.5f, m_pPlayer->m_iTeam, m_usCreateExplosion);

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

