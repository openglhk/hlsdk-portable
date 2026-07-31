#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h" // ?? 修正 4-7：必須包含 player.h 才能讓編譯器認得 CBasePlayer 的成員 (pev->flags 等)

// =========================================================================
// 1. CTriggerFreezePlayer 類別宣告
// =========================================================================

// ?? 修正 1：Android NDK 編譯器提示將 CBaseTrigger 改為標準的 CBaseToggle
// 在 hlsdk-portable 中，Brush 觸發器物件最安全的直接基底類別是 CBaseToggle
class CTriggerFreezePlayer : public CBaseToggle
{
public:
    void Spawn( void ) override;
    void FreezeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

    CTriggerFreezePlayer()
    {
        if ( pev ) pev->nextthink = 0.0f;
    }
};

LINK_ENTITY_TO_CLASS( trigger_freezeplayer, CTriggerFreezePlayer );

// =========================================================================
// 2. 誕生初始化 (Spawn)
// =========================================================================

void CTriggerFreezePlayer::Spawn( void )
{
    if ( pev->angles != g_vecZero )
    {
        SetMovedir( pev );
    }

    pev->solid    = SOLID_TRIGGER;   // 對應固體類型 1
    pev->movetype = MOVETYPE_NONE;   // 對應移動類型 0

    SET_MODEL( ENT(pev), STRING(pev->model) );

    if ( CVAR_GET_FLOAT("showtriggers") == 0.0f )
    {
        pev->effects |= EF_NODRAW; // 0x80 強制隱形
    }

    // 動態綁定 Use 函式
    SetUse( &CTriggerFreezePlayer::FreezeUse );
}

// =========================================================================
// 3. 定身/解凍狀態切換控制 (FreezeUse)
// =========================================================================

void CTriggerFreezePlayer::FreezeUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    // ?? 修正 2-3：標準 SDK 的 CBaseToggle 沒有 m_iQueryTarget 與 CheckConditions，
    // 這屬於 Ritual 特有擴充，在此直接移除，以相容標準 Android NDK 編譯。

    // 獲取單人模式下的第一個主玩家對象 (EntIndex 1)
    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict || FNullEnt(pPlayerEdict) || pPlayerEdict->pvPrivateData == nullptr)
        return;

    CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;

    // 根據 pev->solid 狀態進行定身/解凍狀態切換
    if (pev->solid == SOLID_NOT) 
    {
        // ---------- 解凍 / 釋放玩家 ----------
        pev->solid = SOLID_TRIGGER; 

        // 移除玩家的定身標記，恢復移動
        pPlayer->pev->flags &= ~FL_FROZEN;
        
        gpGlobals->force_retouch += 1.0f; 
    }
    else 
    {
        // ---------- 定身 / 鎖死玩家 ----------
        pev->solid = SOLID_NOT; 

        // 為玩家加上金源引擎內建的定身標記
        pPlayer->pev->flags |= FL_FROZEN;
        
        // 強制將速度壓回零，確保 100% 卡死無法位移
        pPlayer->pev->velocity = g_vecZero;
        pPlayer->pev->maxspeed = 0.00001f;
    }

    UTIL_SetOrigin(pev, pev->origin);
}
