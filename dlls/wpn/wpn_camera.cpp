#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"

#define WEAPON_CAMERA 18 // 0x12

class CCamera : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    
    int GetItemInfo(ItemInfo *p) override;
    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    
    void PrimaryAttack(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;
    
    int iItemSlot(void) override;
    float GetMaxSpeed(void) override;
    
    // CZDS 專屬虛擬函式或自訂狀態覆寫
    void SetWeaponState(ushort weaponState) override;
    void Retract(void); // 對應 WeaponIdle 中狀態 3 呼叫的收回/結束動作

public:
    unsigned short m_usFireCamera; // DWARF 提示的重疊/專屬事件 ID 欄位 (用於武器事件特效)
};

LINK_ENTITY_TO_CLASS(weapon_camera, CCamera);

void CCamera::Precache(void)
{
    PRECACHE_MODEL("models/v_camera.mdl");
    PRECACHE_MODEL("models/w_camera.mdl");
    PRECACHE_SOUND("weapons/camera-1.wav");
    PRECACHE_SOUND("weapons/camera_clipout.wav");
    PRECACHE_SOUND("weapons/camera_clipin.wav");
    PRECACHE_SOUND("weapons/camera_sliderelease.wav");
    PRECACHE_SOUND("weapons/camera_slidepull.wav");

    m_usFireCamera = PRECACHE_EVENT(1, "events/camera.sc");
}

void CCamera::Spawn(void)
{
    SET_MODEL(ENT(pev), "models/w_camera.mdl");
    pev->frame = 0.0f;
    pev->body = 3;
    pev->sequence = 0;
    pev->framerate = 0.0f;
    pev->classname = ALLOC_STRING("weapon_camera");

    Precache();

    m_iId = WEAPON_CAMERA; // 0x12 = 18
    m_iDefaultAmmo = -1;
    m_iClip = -1;
    m_bIsAccessory = TRUE;

    FallInit();

    pev->nextthink = UTIL_WeaponTimeBase() + 0.1f;
}

BOOL CCamera::Deploy(void)
{
    return DefaultDeploy("models/v_camera.mdl", nullptr, 2, "onehanded", 0);
}

int CCamera::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = nullptr;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 20; // 0x14 = 20
    p->iSlot = 4;
    p->iPosition = 4;
    p->iFlags = 0;
    p->iId = WEAPON_CAMERA; // 0x12 = 18
    p->iWeight = 5;
    p->bIsAccessory = TRUE;

    m_iId = WEAPON_CAMERA;
    m_iPlayEmptySound = 1;

    return 1;
}

float CCamera::GetMaxSpeed(void)
{
    if (m_WeaponState == 1)
    {
        return 1.0f;
    }

    if (m_WeaponState != 0 && m_WeaponState > 3)
    {
        return 0.0f;
    }

    return 225.0f;
}

void CCamera::Holster(int skiplocal)
{
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0f;

    if (m_pPlayer->m_iWeaponState & 1)
    {
        m_pPlayer->m_iWeaponState &= ~1;
    }

    SendWeaponAnim(2, skiplocal);
    m_WeaponState = 2;
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
}

int CCamera::iItemSlot(void)
{
    return 5;
}

void CCamera::PrimaryAttack(void)
{
    if (m_WeaponState != 0)
    {
        return;
    }

    if (m_pPlayer->m_toolTrigger != nullptr && m_pPlayer->m_toolTrigger->IsPlayerInPositionUsingProperTool())
    {
        g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 1.0f);
        SendWeaponAnim(1, 0);
        m_WeaponState = 1;

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.794f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.794f;
        return;
    }

    hudtextparms_t hudTextParms;
    hudTextParms.effect = 0;
    hudTextParms.fadeinTime = 0.25f;
    hudTextParms.fadeoutTime = 0.25f;
    hudTextParms.holdTime = 2.0f;
    hudTextParms.fxTime = 0.0f;
    hudTextParms.x = -1.0f;
    hudTextParms.y = 0.01f;
    hudTextParms.r1 = 0;
    hudTextParms.g1 = 0;
    hudTextParms.b1 = 255;
    hudTextParms.a1 = 255;
    hudTextParms.r2 = 255;
    hudTextParms.g2 = 255;
    hudTextParms.b2 = 255;
    hudTextParms.a2 = 255;
    hudTextParms.channel = 0;

    UTIL_HudMessageAll(&hudTextParms, "#CAMERA_MESSAGE");
}

void CCamera::Reload(void)
{
    return;
}

void CCamera::SetWeaponState(ushort weaponState)
{
    float duration = 0.0f;
    float nextAttack = 0.0f;

    switch (weaponState)
    {
    case 1:
        g_engfuncs.pfnSetClientMaxspeed(m_pPlayer->edict(), 1.0f);
        duration = 1.794f;
        nextAttack = 1.794f;
        break;

    case 0:
        duration = 20.0f;
        nextAttack = 0.0f;
        break;

    case 2:
        duration = 1.0f;
        nextAttack = 1.0f;
        break;

    case 3:
        duration = 0.7f;
        nextAttack = 0.7f;
        break;

    default:
        duration = 0.0f;
        nextAttack = 0.0f;
        break;
    }

    SendWeaponAnim(weaponState, 0);
    m_WeaponState = weaponState;

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + duration;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + nextAttack;
}

void CCamera::WeaponIdle(void)
{
    if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
    {
        return;
    }

    if (m_WeaponState == 1)
    {
        if (m_pPlayer->m_toolTrigger != nullptr)
        {
            m_pPlayer->m_toolTrigger->FireUseTarget(m_pPlayer, nullptr, USE_SET, 0.0f);
        }

        SendWeaponAnim(0, 0);
        m_WeaponState = 0;
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.0f;

        m_pPlayer->ResetMaxSpeed();
        return;
    }

    if (m_WeaponState == 0 || m_WeaponState == 2)
    {
        SendWeaponAnim(0, 0);
        m_WeaponState = 0;
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.0f;
    }
    else if (m_WeaponState == 3)
    {
        Retract();
    }
}
