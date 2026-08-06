#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"

#define WEAPON_RADIO 11 // 0x0b

class CRadio : public CBasePlayerWeapon
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
    
    void SetWeaponState(ushort weaponState) override;
    void Retract(void); // Matches virtual index 0x61 called during state 2 idle

public:
    unsigned short m_usFireRadio;
};

LINK_ENTITY_TO_CLASS(weapon_radio, CRadio);

void CRadio::Precache(void)
{
    PRECACHE_MODEL("models/v_radio.mdl");
    PRECACHE_MODEL("models/w_radio.mdl");
    PRECACHE_SOUND("weapons/radio_activate.wav");
    PRECACHE_SOUND("weapons/radio_deploy.wav");

    m_usFireRadio = PRECACHE_EVENT(1, "events/radio.sc");
}

void CRadio::Spawn(void)
{
    SET_MODEL(ENT(pev), "models/w_radio.mdl");
    pev->frame = 0.0f;
    pev->body = 3;
    pev->sequence = 0;
    pev->framerate = 0.0f;
    pev->classname = ALLOC_STRING("weapon_radio");

    Precache();

    m_iId = WEAPON_RADIO; // 0x0b = 11
    m_iDefaultAmmo = -1;
    m_iClip = -1;
    m_bIsAccessory = TRUE;

    FallInit();

    pev->nextthink = UTIL_WeaponTimeBase() + 0.1f;
}

BOOL CRadio::Deploy(void)
{
    return DefaultDeploy("models/v_radio.mdl", nullptr, 1, "onehanded", 0);
}

int CRadio::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = nullptr;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = -1;
    p->iSlot = 4;
    p->iPosition = 11; // 0x0b = 11
    p->iId = WEAPON_RADIO; // 0x0b = 11
    p->iWeight = 5;
    p->iFlags = 0;
    p->bIsAccessory = TRUE;

    m_iId = WEAPON_RADIO;
    m_iPlayEmptySound = 1;

    return 1;
}

float CRadio::GetMaxSpeed(void)
{
    if (m_WeaponState > 2)
    {
        if (m_WeaponState > 6)
        {
            return 0.0f;
        }
        return 1.0f;
    }

    return 225.0f;
}

void CRadio::Holster(int skiplocal)
{
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0f;

    SendWeaponAnim(1, skiplocal);
    m_WeaponState = 1;

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
}

int CRadio::iItemSlot(void)
{
    return 5;
}

void CRadio::PrimaryAttack(void)
{
    if (m_WeaponState != 0)
    {
        return;
    }

    if (m_pPlayer->m_toolTrigger == nullptr || !m_pPlayer->m_toolTrigger->IsPlayerInPositionUsingProperTool())
    {
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

        UTIL_HudMessageAll(&hudTextParms, "#RADIO_MESSAGE");
        return;
    }

    SetWeaponState(3);

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.733f;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2.733f;

    if (m_pPlayer != nullptr && m_pPlayer->m_toolTrigger != nullptr)
    {
        m_pPlayer->m_toolTrigger->FireUseTarget(m_pPlayer, nullptr, USE_SET, 0.0f);
    }
}

void CRadio::Reload(void)
{
    return;
}

void CRadio::SetWeaponState(ushort weaponState)
{
    float duration = 0.0f;
    float nextAttack = 0.0f;

    switch (weaponState)
    {
    case 0: // 閒置狀態 (Idle)
        duration = 20.0f;
        nextAttack = 0.0f;
        break;

    case 1: // 收槍狀態 (Holster)
        duration = 1.0f;
        nextAttack = 1.0f;
        break;

    case 2: // 部署狀態 (Deploy)
        duration = 1.0f;
        nextAttack = 1.0f;
        break;

    case 3: // 呼叫/使用中狀態 (Primary Attack Active)
        duration = 2.733f;
        nextAttack = 2.733f;
        break;

    case 4: // 特殊廣播狀態 1
        duration = 3.0f;
        nextAttack = 3.0f;
        break;

    case 5: // 特殊廣播狀態 2
        duration = 4.0f;
        nextAttack = 4.0f;
        break;

    case 6: // 特殊廣播狀態 3
        duration = 5.0f;
        nextAttack = 5.0f;
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

void CRadio::WeaponIdle(void)
{
    if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
    {
        return;
    }

    if (m_WeaponState == 2)
    {
        Retract();
        return;
    }

    if (m_WeaponState == 1 || m_WeaponState == 0 || m_WeaponState == 3)
    {
        SendWeaponAnim(0, 0);
        m_WeaponState = 0;
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.0f;
    }
}

