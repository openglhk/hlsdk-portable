#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

// =========================================================================
// 1. CTriggerHud 類別宣告 (繼承自 Android NDK 最相容的 CBaseToggle)
// =========================================================================

class CTriggerHud : public CBaseToggle
{
public:
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    void HudUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

    CTriggerHud();

private:
    int m_iHudState; // 用來儲存地圖設定的 HUD 預設狀態
};

LINK_ENTITY_TO_CLASS( trigger_hud, CTriggerHud );

// =========================================================================
// 2. 建構子與地圖參數讀取
// =========================================================================

CTriggerHud::CTriggerHud()
{
    m_iHudState = 1; // 預設為 1 (代表原本的 HUD 顯示狀態)
    if ( pev ) pev->nextthink = 0.0f;
}

void CTriggerHud::KeyValue( KeyValueData *pkvd )
{
    if ( strcasecmp(pkvd->szKeyName, "hud_state") == 0 || strcasecmp(pkvd->szKeyName, "state") == 0 )
    {
        m_iHudState = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
        return;
    }
    CBaseToggle::KeyValue( pkvd );
}

// =========================================================================
// 3. 誕生初始化 (Spawn)
// =========================================================================

void CTriggerHud::Spawn( void )
{
    pev->solid    = SOLID_NOT;     // 對應固體類型 0
    pev->movetype = MOVETYPE_NONE;  // 對應移動類型 0

    if ( pev->model ) {
        SET_MODEL( ENT(pev), STRING(pev->model) );
    }

    // 動態綁定 Use 觸發函式
    SetUse( &CTriggerHud::HudUse );
}

// =========================================================================
// 4. HUD 控制核心執行器 (HudUse)
// =========================================================================

// ?? 修正核心：引用標準 SDK 自帶的動態分配全域訊息 ID 變數。
// 這樣可以完全避免任何硬編碼產生的動態分配錯亂與伺服器崩潰問題。
extern int gmsgHideWeapon;

void CTriggerHud::HudUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 獲取單人劇情模式下的主玩家指標 (EntIndex 1 固定為玩家)
    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict || FNullEnt(pPlayerEdict) || pPlayerEdict->pvPrivateData == nullptr)
        return;

    // 根據當前實體被觸發的狀態（或 targetname）來決定是否隱藏 HUD
    bool bHideHud = (strcasecmp(STRING(pev->targetname), "hudOff") == 0) || (m_iHudState == 0);

    // ?? 安全防護：確保地圖在載入完成、ID 已被正常註冊的情況下才進行發送
    if (gmsgHideWeapon > 0)
    {
        // 100% 符合標準 SDK 封包發送規範，徹底抹平任何 NDK 編譯成員缺失錯誤
        MESSAGE_BEGIN(MSG_ONE, gmsgHideWeapon, nullptr, pPlayerEdict);
            WRITE_BYTE(bHideHud ? 1 : 0); // 1 = 隱藏 HUD (關閉), 0 = 恢復顯示 HUD (開啟)
        MESSAGE_END();

        // 翻轉開關狀態
        m_iHudState = bHideHud ? 1 : 0;
    }
}
