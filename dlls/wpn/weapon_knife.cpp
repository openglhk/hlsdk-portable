#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CKnife : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override { return 3; }
    float GetMaxSpeed(void) override { return 225.0f; }
    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    BOOL CanDrop(void) override { return FALSE; }
    BOOL UseDecrement(void) override { return TRUE; }
    void WeaponAnimation(int iAnimation);
    void WeaponIdle(void) override;
    void PrimaryAttack(void) override;
    void SecondaryAttack(void) override;
    int Swing(int fFirst);
    int Stab(int fFirst);
    void SwingAgain(void);
    void Smack(void);
    void SetWeaponState(unsigned short weaponState);

public:
    int m_iSwing;                     // 揮刀次數計數器 (用於左鍵切換 slash1 / slash2 動態切換)
    unsigned short m_WeaponState;     // 當前武器自定義動畫狀態機
    TraceResult m_trHit;              // 儲存當前命中的 Trace 結構，供 Smack 延遲判定使用
    unsigned short m_usKnife;         // 註冊客戶端事件 (events/knife.sc) 的索引 ID
};

LINK_ENTITY_TO_CLASS(weapon_knife, CKnife);

void CKnife::Precache(void)
{
    PRECACHE_MODEL("models/v_knife.mdl");
    PRECACHE_MODEL("models/w_knife.mdl");

    PRECACHE_SOUND("weapons/knife_deploy1.wav");
    PRECACHE_SOUND("weapons/knife_hit1.wav");
    PRECACHE_SOUND("weapons/knife_hit2.wav");
    PRECACHE_SOUND("weapons/knife_hit3.wav");
    PRECACHE_SOUND("weapons/knife_hit4.wav");
    PRECACHE_SOUND("weapons/knife_slash1.wav");
    PRECACHE_SOUND("weapons/knife_slash2.wav");
    PRECACHE_SOUND("weapons/knife_stab.wav");
    PRECACHE_SOUND("weapons/knife_hitwall1.wav");

    m_usKnife = PRECACHE_EVENT(1, "events/knife.sc");
}

void CKnife::Spawn(void)
{
    Precache();

    m_iId = WEAPON_KNIFE;
    SET_MODEL(ENT(pev), "models/w_knife.mdl");

    m_iClip = WEAPON_NOCLIP;
    m_bIsAccessory = true;

    FallInit();
}

BOOL CKnife::CanDrop(void)
{
    return FALSE;
}

BOOL CKnife::Deploy(void)
{
    EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/knife_deploy1.wav", 0.3, ATTN_NORM, 0, PITCH_NORM);

    m_iSwing = 0;

    return DefaultDeploy("models/v_knife.mdl", "models/p_knife.mdl", 3, "knife", UseDecrement() != FALSE);
}

int CKnife::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = nullptr;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = -1;
    p->iSlot = 2;
    p->iPosition = 1;
    p->iId = WEAPON_KNIFE;
    p->iWeight = 0;
    p->bIsAccessory = true;

    return 1;
}

float CKnife::GetMaxSpeed(void)
{
    return 225.0f;
}

void CKnife::Holster(int skiplocal)
{
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;
}

int CKnife::iItemSlot(void)
{
    return 3;
}

void CKnife::PrimaryAttack(void)
{
    Swing(TRUE);
}

void CKnife::SecondaryAttack(void)
{
    Stab(TRUE);
    pev->nextthink = UTIL_WeaponTimeBase() + 0.35f;
}

void CKnife::SetWeaponState(unsigned short weaponState)
{
    float duration = 0.0f;
    float nextAttack = 0.0f;
    float nextSecondaryAttack = 0.0f;

    if (weaponState < 8)
    {
        duration = CSWTCH_118[weaponState];
        nextAttack = CSWTCH_119[weaponState];
        nextSecondaryAttack = CSWTCH_120[weaponState];
    }

    SendWeaponAnim(weaponState, UseDecrement() != FALSE);
    m_WeaponState = weaponState;

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + duration;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + nextAttack;
    m_flNextSecondaryAttack = nextSecondaryAttack;
}

void CKnife::Smack(void)
{
    return;
}

int CKnife::Stab(int fFirst)
{
    int fDidHit = FALSE;
    TraceResult tr;

    UTIL_MakeVectors(m_pPlayer->pev->v_angle);

    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecEnd = vecSrc + gpGlobals->v_forward * 32.0f;

    UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr);

    if (tr.flFraction >= 1.0f)
    {
        UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, m_pPlayer->edict(), &tr);
        if (tr.flFraction < 1.0f)
        {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (pHit == nullptr || pHit->IsBSPModel())
            {
                FindHullIntersection(vecSrc, tr, VEC_DUST_MIN, VEC_DUST_MAX, m_pPlayer->edict());
            }
            vecEnd = tr.vecEndPos;
        }
    }

    if (tr.flFraction < 1.0f)
    {
        fDidHit = TRUE;
        SetWeaponState(4);

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.189f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.189f;
        m_flNextSecondaryAttack = 1.189f;

        CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        ClearMultiDamage();

        pEntity->TraceAttack(m_pPlayer->pev, 65.0f, gpGlobals->v_forward, &tr, DMG_BULLET | DMG_NEVERGIB);
        ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

        if (pEntity == nullptr || !pEntity->Classify())
        {
            TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, 0);

            int iPitch = 98 + RANDOM_LONG(0, 3);
            EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_hitwall1.wav", 1.0f, ATTN_NORM, 0, iPitch);
        }
        else
        {
            int iPitch = 94;
            const char *sample = "weapons/knife_stab.wav";

            EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, sample, 1.0f, ATTN_NORM, 0, iPitch);
            m_pPlayer->SetAnimation(PLAYER_ATTACK1);
            return 0;
        }
    }

    return fDidHit;
}

int CKnife::Swing(int fFirst)
{
    int fDidHit = FALSE;
    TraceResult tr;

    UTIL_MakeVectors(m_pPlayer->pev->v_angle);
    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecEnd = vecSrc + gpGlobals->v_forward * 48.0f;

    UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, m_pPlayer->edict(), &tr);

    if (tr.flFraction >= 1.0f)
    {
        UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, m_pPlayer->edict(), &tr);
        if (tr.flFraction < 1.0f)
        {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (pHit == nullptr || pHit->IsBSPModel())
            {
                FindHullIntersection(vecSrc, tr, VEC_DUST_MIN, VEC_DUST_MAX, m_pPlayer->edict());
            }
            vecEnd = tr.vecEndPos;
        }
    }

    if (tr.flFraction >= 1.0f)
    {
        m_iSwing++;
        
        if (m_iSwing % 2 == 0)
        {
            SetWeaponState(6);
        }
        else
        {
            SetWeaponState(7);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.675f;
        m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.675f;
        m_flNextSecondaryAttack = 0.825f;

        const char *sample = (RANDOM_LONG(0, 1) == 0) ? "weapons/knife_slash2.wav" : "weapons/knife_slash1.wav";
        EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, sample, 1.0f, ATTN_NORM, 0, 94);
        
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);
        return 0;
    }

    m_iSwing++;
    
    if (m_iSwing % 2 == 0)
    {
        SetWeaponState(6);
    }
    else
    {
        SetWeaponState(7);
    }

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.675f;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.675f;
    m_flNextSecondaryAttack = 0.825f;

    CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    ClearMultiDamage();

    float flDamage = (UTIL_WeaponTimeBase() <= m_flNextPrimaryAttack + 0.4f) ? 15.0f : 20.0f;

    pEntity->TraceAttack(m_pPlayer->pev, flDamage, gpGlobals->v_forward, &tr, DMG_BULLET | DMG_NEVERGIB);
    ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

    float flVol = 1.0f;

    if (pEntity == nullptr || !pEntity->Classify())
    {
        TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_TYPEMG);

        if (RANDOM_LONG(0, 1) <= 1)
        {
            int iPitch = 98 + RANDOM_LONG(0, 3);
            EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapons/knife_hitwall1.wav", 1.0f, ATTN_STATIC, 0, iPitch);
        }
    }
    else
    {
        const char *sample = nullptr;
        switch (RANDOM_LONG(0, 3))
        {
            case 0: sample = "weapons/knife_hit1.wav"; break;
            case 1: sample = "weapons/knife_hit2.wav"; break;
            case 2: sample = "weapons/knife_hit3.wav"; break;
            case 3: sample = "weapons/knife_hit4.wav"; break;
        }
        
        EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, sample, 1.0f, ATTN_NORM, 0, 100);
        m_pPlayer->m_iWeaponVolume = 128;
        
        if (!pEntity->IsAlive())
        {
            return 1;
        }
        
        flVol = 0.1f;
    }

    m_trHit = tr;
    m_pfnThink = &CKnife::Smack;
    pev->nextthink = UTIL_WeaponTimeBase() + 0.2f;
    m_pPlayer->m_iWeaponVolume = (int)(flVol * 512.0f);

    return 1;
}

void CKnife::SwingAgain(void)
{
    Swing(FALSE);
}

BOOL CKnife::UseDecrement(void)
{
    return TRUE;
}

void CKnife::WeaponAnimation(int iAnimation)
{
    return;
}

void CKnife::WeaponIdle(void)
{
    if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
    {
        switch (m_WeaponState)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            SetWeaponState(0);
            break;
        default:
            break;
        }
    }
}