#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"

#define WEAPON_RADIOCONTROLLEDBOMB 20 // 0x14
#define HAS_RADIOCONTROLLEDBOMB    0x00100000

class CBomb;

class CRadioControlledBomb : public CBasePlayerWeapon
{
public:
    CRadioControlledBomb();

    void Spawn(void) override;
    void Precache(void) override;
    void KeyValue(KeyValueData *pkvd) override;
    
    int GetItemInfo(ItemInfo *p) override;
    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    
    void PrimaryAttack(void) override;
    void SecondaryAttack(void) override;
    void Reload(void) override;
    void WeaponIdle(void) override;
    void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
    
    int iItemSlot(void) override;
    float GetMaxSpeed(void) override;
    void SetWeaponState(ushort weaponState) override;

    // Custom single player logic
    BOOL PlantBomb(void);
    void DetonateBombs(void);
    BOOL AddToBombArray(CBomb *bomb);
    BOOL BombArrayIsEmpty(void);
    void ClearBombArray(void);
    void Retract(void); // Matches virtual index 0x61 called during state 5 empty check

    int Save(CSave &save) override;
    int Restore(CRestore &restore) override;

    static TYPEDESCRIPTION m_SaveData[];

public:
    CBomb *m_BombList[20]; // Structure block array optimized into 0x50 bytes
};

TYPEDESCRIPTION CRadioControlledBomb::m_SaveData[] =
{
    DEFINE_ARRAY(CRadioControlledBomb, m_BombList, FIELD_CLASSPTR, 20),
};

LINK_ENTITY_TO_CLASS(weapon_radiocontrolledbomb, CRadioControlledBomb);

void CRadioControlledBomb::Precache(void)
{
    PRECACHE_MODEL("models/v_rcontrolbomb.mdl");
    PRECACHE_MODEL("models/w_rcontrolbomb.mdl");
    UTIL_PrecacheOther("bomb");
    PRECACHE_SOUND("weapons/rc_detonate.wav");
    PRECACHE_SOUND("weapons/rc_draw_detonator.wav");
}

void CRadioControlledBomb::Spawn(void)
{
    SET_MODEL(ENT(pev), "models/w_rcontrolbomb.mdl");
    pev->frame = 0.0f;
    pev->body = 3;
    pev->sequence = 0;
    pev->framerate = 0.0f;
    pev->classname = ALLOC_STRING("weapon_radiocontrolledbomb");

    Precache();

    m_iId = WEAPON_RADIOCONTROLLEDBOMB; // 0x14 = 20
    m_iDefaultAmmo = 5;

    if (pev->targetname == 0)
    {
        m_bIsAccessory = TRUE;
        FallInit();
        SetThink(&CBasePlayerItem::FallThink);
        pev->nextthink = UTIL_WeaponTimeBase() + 0.1f;
        return;
    }

    pev->effects |= EF_NODRAW; // 0x80 = EF_NODRAW
    DROP_TO_FLOOR(ENT(pev));
}

BOOL CRadioControlledBomb::AddToBombArray(CBomb *bomb)
{
    if (bomb == nullptr)
    {
        return FALSE;
    }

    for (int i = 0; i < 20; i++) // 0x14 = 20
    {
        if (m_BombList[i] == nullptr)
        {
            m_BombList[i] = bomb;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CRadioControlledBomb::BombArrayIsEmpty(void)
{
    for (int i = 0; i < 20; i++)
    {
        if (m_BombList[i] != nullptr)
        {
            return FALSE;
        }
    }
    return TRUE;
}

void CRadioControlledBomb::ClearBombArray(void)
{
    memset(m_BombList, 0, sizeof(m_BombList));
}

CRadioControlledBomb::CRadioControlledBomb()
{
    deathfadedelay = 2.0f;
    fadespeed = 7;
    shotPlayerRecently = 0;
    shotPlayerEver = 0;
    shotConeTime = 0.0f;
    leapConeFromFull = 0;
    m_classtype = 0;

    memset(m_BombList, 0, sizeof(m_BombList));
}

BOOL CRadioControlledBomb::Deploy(void)
{
    for (int i = 0; i < 20; i++)
    {
        if (m_BombList[i] != nullptr)
        {
            SetWeaponState(1);
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
            m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.0f;

            return DefaultDeploy("models/v_rcontrolbomb.mdl", nullptr, 3, "radiocontrolledbomb", 0);
        }
    }

    return DefaultDeploy("models/v_rcontrolbomb.mdl", nullptr, 2, "radiocontrolledbomb", 0);
}

void CRadioControlledBomb::DetonateBombs(void)
{
    for (int i = 0; i < 20; i++)
    {
        if (m_BombList[i] != nullptr)
        {
            m_BombList[i]->SetThink(&CGrenade::Detonate3);
            m_BombList[i]->pev->nextthink = gpGlobals->time + 0.1f;
        }
    }

    memset(m_BombList, 0, sizeof(m_BombList));
}

int CRadioControlledBomb::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "bomb";
    p->iMaxAmmo1 = 5;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = -1;
    p->iSlot = 4;
    p->iPosition = 6;
    p->iId = WEAPON_RADIOCONTROLLEDBOMB; // 0x14 = 20
    p->iWeight = 3;
    p->iFlags = ITEM_FLAG_SELECTONEMPTY | ITEM_FLAG_NOAUTORELOAD; // 0x18 = ITEM_FLAG_SELECTONEMPTY(0x08) | ITEM_FLAG_NOAUTORELOAD(0x10)
    p->bIsAccessory = TRUE;

    m_iId = WEAPON_RADIOCONTROLLEDBOMB;
    m_iPlayEmptySound = 1;

    return 1;
}

float CRadioControlledBomb::GetMaxSpeed(void)
{
    if (m_WeaponState == 6)
    {
        return 1.0f;
    }

    if (m_WeaponState > 5 && m_WeaponState != 7)
    {
        return 0.0f;
    }

    return 225.0f;
}

void CRadioControlledBomb::Holster(int skiplocal)
{
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;

    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0)
    {
        BOOL bHasBombs = FALSE;
        for (int i = 0; i < 20; i++)
        {
            if (m_BombList[i] != nullptr)
            {
                bHasBombs = TRUE;
                break;
            }
        }

        if (!bHasBombs)
        {
            m_pPlayer->m_iAccessoryBits &= ~HAS_RADIOCONTROLLEDBOMB; // 0xffefffff removes bit 0x00100000
            DestroyItem();
        }
    }

    SetWeaponState(2);

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0f;

    EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}

int CRadioControlledBomb::iItemSlot(void)
{
    return 5;
}

void CRadioControlledBomb::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "detonatedelay"))
    {
        pev->speed = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "detonatetarget"))
    {
        pev->noise1 = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "defusetarget"))
    {
        pev->target = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else
    {
        pkvd->fHandled = FALSE;
    }
}

BOOL CRadioControlledBomb::PlantBomb(void)
{
    UTIL_MakeVectors(m_pPlayer->pev->v_angle);

    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecOrigin = vecSrc + (gpGlobals->v_forward * 16.0f) + (gpGlobals->v_right * 8.0f) + (gpGlobals->v_up * -8.0f);
    Vector vecAngles = m_pPlayer->pev->v_angle;

    CBomb *pBomb = CBomb::CreateBomb(vecOrigin, vecAngles, m_pPlayer);
    if (pBomb == nullptr)
    {
        return FALSE;
    }

    for (int i = 0; i < 20; i++)
    {
        if (m_BombList[i] == nullptr)
        {
            m_BombList[i] = pBomb;
            break;
        }
    }

    m_pPlayer->m_rgAmmo[CBasePlayer::GetAmmoIndex("bomb")]--;

    if (m_pPlayer->m_toolTrigger != nullptr)
    {
        m_pPlayer->m_toolTrigger->FireUseTarget(m_pPlayer, nullptr, USE_SET, 0.0f);
    }

    return TRUE;
}

void CRadioControlledBomb::PrimaryAttack(void)
{
    if (m_WeaponState == 0)
    {
        if (m_pPlayer != nullptr && m_pPlayer->m_toolTrigger != nullptr &&
            m_pPlayer->m_toolTrigger->IsPlayerInPositionUsingProperTool() &&
            m_pPlayer->m_toolTrigger->m_bombPlanted == 0)
        {
            SetWeaponState(6);
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.06f;
            m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.06f;
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

        UTIL_HudMessageAll(&hudTextParms, "#RCBOMB_MESSAGE");
        return;
    }

    if (m_WeaponState == 1)
    {
        for (int i = 0; i < 20; i++)
        {
            if (m_BombList[i] != nullptr)
            {
                SetWeaponState(7);
                m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
                m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5f;
                return;
            }
        }
        
        Reload();
    }
}

int CRadioControlledBomb::Save(CSave &save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    return save.WriteFields("CRadioControlledBomb", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

int CRadioControlledBomb::Restore(CRestore &restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    return restore.ReadFields("CRadioControlledBomb", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CRadioControlledBomb::SecondaryAttack(void)
{
    if (m_WeaponState == 0)
    {
        SetWeaponState(4);
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.666f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.666f;
    }
    else if (m_WeaponState == 1)
    {
        if (m_pPlayer->m_rgAmmo[CBasePlayer::GetAmmoIndex("bomb")] > 0)
        {
            SetWeaponState(5);
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.2f;
            m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.2f;
        }
    }
}

void CRadioControlledBomb::SetWeaponState(ushort weaponState)
{
    float duration = 0.0f;
    float nextAttack = 0.0f;

    switch (weaponState)
    {
    case 0:
    case 1:
        duration = 20.0f;
        nextAttack = 0.0f;
        break;

    case 2:
        duration = 1.0f;
        nextAttack = 1.0f;
        break;

    case 3:
        duration = 1.333f;
        nextAttack = 1.333f;
        break;

    case 4:
        duration = 0.666f;
        nextAttack = 0.666f;
        break;

    case 5:
        duration = 1.2f;
        nextAttack = 1.2f;
        break;

    case 6:
        duration = 1.06f;
        nextAttack = 1.06f;
        break;

    case 7:
        {
            BOOL bHasBombs = FALSE;
            for (int i = 0; i < 20; i++)
            {
                if (m_BombList[i] != nullptr)
                {
                    bHasBombs = TRUE;
                    break;
                }
            }

            if (!bHasBombs)
            {
                Reload();
                return;
            }

            duration = 1.5f;
            nextAttack = 1.5f;
        }
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

void CRadioControlledBomb::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    return;
}

void CRadioControlledBomb::WeaponIdle(void)
{
    if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
    {
        return;
    }

    switch (m_WeaponState)
    {
    case 0:
    case 2:
        SetWeaponState(0);
        break;

    case 1:
    case 3:
        SetWeaponState(1);
        break;

    case 4:
        SetWeaponState(3);
        break;

    case 5:
        if (m_pPlayer->m_rgAmmo[CBasePlayer::GetAmmoIndex("bomb")] < 1)
        {
            Retract();
            return;
        }
        SetWeaponState(2);
        break;

    case 6:
        if (PlantBomb())
        {
            SetWeaponState(3);
        }
        else
        {
            SetWeaponState(0);
        }
        break;

    case 7:
        for (int i = 0; i < 20; i++)
        {
            if (m_BombList[i] != nullptr)
            {
                m_BombList[i]->SetThink(&CGrenade::Detonate3);
                m_BombList[i]->pev->nextthink = gpGlobals->time + 0.1f;
            }
        }
        memset(m_BombList, 0, sizeof(m_BombList));
        SetWeaponState(5);
        break;
    }
}

