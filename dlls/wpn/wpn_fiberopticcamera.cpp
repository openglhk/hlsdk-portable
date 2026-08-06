#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"

#define WEAPON_FIBEROPTICCAMERA 26 // 0x1a

class CFiberOpticCamera : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    
    int GetItemInfo(ItemInfo *p) override;
    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    
    void PrimaryAttack(void) override;
    void SecondaryAttack(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;
    
    int iItemSlot(void) override;
    float GetMaxSpeed(void) override;
    
    void SetWeaponState(ushort weaponState) override;
    
    int Save(CSave &save) override;
    int Restore(CRestore &restore) override;

    static TYPEDESCRIPTION m_SaveData[];

public:
    unsigned short m_usFireFiberOpticCamera;
    BOOL m_bIsZoomed;
};

TYPEDESCRIPTION CFiberOpticCamera::m_SaveData[] =
{
    DEFINE_FIELD(CFiberOpticCamera, m_bIsZoomed, FIELD_CHARACTER),
};

LINK_ENTITY_TO_CLASS(weapon_fiberopticcamera, CFiberOpticCamera);

void CFiberOpticCamera::Precache(void)
{
    PRECACHE_MODEL("models/p_focamera.mdl");
    PRECACHE_MODEL("models/v_focamera.mdl");
    PRECACHE_MODEL("models/v_nothing.mdl");
    PRECACHE_MODEL("models/w_focamera.mdl");
    PRECACHE_SOUND("weapons/camera-1.wav");
    PRECACHE_SOUND("weapons/camera_clipout.wav");
    PRECACHE_SOUND("weapons/camera_clipin.wav");
    PRECACHE_SOUND("weapons/camera_sliderelease.wav");
    PRECACHE_SOUND("weapons/camera_slidepull.wav");

    m_usFireFiberOpticCamera = PRECACHE_EVENT(1, "events/fiberopticcamera.sc");
}

void CFiberOpticCamera::Spawn(void)
{
    SET_MODEL(ENT(pev), "models/w_focamera.mdl");
    pev->frame = 0.0f;
    pev->body = 3;
    pev->sequence = 0;
    pev->framerate = 0.0f;
    pev->classname = ALLOC_STRING("weapon_fiberopticcamera");

    Precache();

    m_iId = WEAPON_FIBEROPTICCAMERA; // 0x1a = 26
    m_iDefaultAmmo = -1;
    m_iClip = -1;
    m_bIsAccessory = TRUE;

    FallInit();
}

BOOL CFiberOpticCamera::Deploy(void)
{
    return DefaultDeploy("models/v_focamera.mdl", "models/p_focamera.mdl", 1, "onehanded", 0);
}

int CFiberOpticCamera::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = nullptr;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = -1;
    p->iSlot = 4;
    p->iPosition = 12; // 0x0c = 12
    p->iId = WEAPON_FIBEROPTICCAMERA; // 0x1a = 26
    p->iWeight = 5;
    p->iFlags = 0;
    p->bIsAccessory = TRUE;

    m_iId = WEAPON_FIBEROPTICCAMERA;
    m_iPlayEmptySound = 1;

    return 1;
}

float CFiberOpticCamera::GetMaxSpeed(void)
{
    if (m_WeaponState > 1)
    {
        if (m_WeaponState > 3)
        {
            return 0.0f;
        }
        return 1.0f;
    }

    return 225.0f;
}

void CFiberOpticCamera::Holster(int skiplocal)
{
    m_pPlayer->m_iFOV = 90; // 0x5a = 90

    if (m_WeaponState == 3 && m_pPlayer->m_toolTrigger != nullptr)
    {
        m_pPlayer->m_toolTrigger->FireUseTarget(m_pPlayer, nullptr, USE_SET, 0.0f);
    }

    m_pPlayer->pev->iuser3 = 0;

    SendWeaponAnim(1, skiplocal);
    m_WeaponState = 1;

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
    m_pPlayer->m_flNextAttack = 1.0f;
}

int CFiberOpticCamera::iItemSlot(void)
{
    return 5;
}

void CFiberOpticCamera::PrimaryAttack(void)
{
    if (m_WeaponState != 0)
    {
        if (m_WeaponState == 3)
        {
            if (m_pPlayer->m_toolTrigger != nullptr)
            {
                m_pPlayer->m_toolTrigger->FireUseTarget(m_pPlayer, nullptr, USE_SET, 0.0f);
            }

            m_pPlayer->m_iFOV = 90;
            m_pPlayer->ResetMaxSpeed();
            m_pPlayer->pev->iuser3 = 0;

            DefaultDeploy("models/v_focamera.mdl", "models/p_focamera.mdl", 1, "onehanded", 0);
            SendWeaponAnim(0, 0);
            m_WeaponState = 0;

            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
            m_pPlayer->m_flNextAttack = 0.0f;
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
        }
        return;
    }

    if (m_pPlayer->m_toolTrigger != nullptr && m_pPlayer->m_toolTrigger->IsPlayerInPositionUsingProperTool())
    {
        SetWeaponState(2);
        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
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

    UTIL_HudMessageAll(&hudTextParms, "#FIBERCAMERA_MESSAGE");
}

void CFiberOpticCamera::Reload(void)
{
    return;
}

int CFiberOpticCamera::Restore(CRestore &restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore.ReadFields("CFiberOpticCamera", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CFiberOpticCamera::Save(CSave &save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save.WriteFields("CFiberOpticCamera", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CFiberOpticCamera::SecondaryAttack(void)
{
    if (m_WeaponState != 3)
    {
        return;
    }

    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.2f;

    if (m_bIsZoomed)
    {
        m_bIsZoomed = FALSE;
        m_pPlayer->m_iFOV = 120; // 0x78 = 120 (Wide View / Reset)
    }
    else
    {
        m_bIsZoomed = TRUE;
        m_pPlayer->m_iFOV = 60;   // 0x3c = 60 (Zoomed In)
    }
}

void CFiberOpticCamera::SetWeaponState(ushort weaponState)
{
    float duration = 0.0f;
    float nextAttack = 0.0f;

    switch (weaponState)
    {
    case 1:
        duration = 1.0f;
        nextAttack = 1.0f;
        break;

    case 0:
        m_pPlayer->m_iFOV = 90;
        m_pPlayer->ResetMaxSpeed();
        m_pPlayer->pev->iuser3 = 0;
        DefaultDeploy("models/v_focamera.mdl", "models/p_focamera.mdl", 1, "onehanded", 0);
        duration = 20.0f;
        nextAttack = 0.0f;
        break;

    case 2:
        m_pPlayer->SetMaxSpeed(1.0f);
        duration = 1.166f;
        nextAttack = 1.166f;
        break;

    case 3:
        m_bIsZoomed = FALSE;
        if (m_pPlayer->m_toolTrigger != nullptr)
        {
            DefaultDeploy("models/v_nothing.mdl", "models/v_nothing.mdl", 0, "onehanded", 0);
            m_pPlayer->m_toolTrigger->FireUseTarget(m_pPlayer, nullptr, USE_SET, 0.0f);
        }
        m_pPlayer->m_iFOV = 120;
        duration = 20.0f;
        nextAttack = 0.1f;
        break;

    default:
        duration = 0.0f;
        nextAttack = 0.0f;
        break;
    }

    SendWeaponAnim(weaponState, 0);
    m_WeaponState = weaponState;

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + duration;
    m_pPlayer->m_flNextAttack = nextAttack;
}

void CFiberOpticCamera::WeaponIdle(void)
{
    if (m_WeaponState == 1 || m_WeaponState == 0)
    {
        if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
        {
            SetWeaponState(0);
        }
    }
    else if (m_WeaponState == 2)
    {
        if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
        {
            m_pPlayer->pev->iuser3 = 1;
            m_pPlayer->pev->flags |= FL_FROZEN; // 0x4000 = FL_FROZEN
            SetWeaponState(3);
        }
    }
}
