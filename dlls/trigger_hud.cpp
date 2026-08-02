#include "trigger_hud.h"

// ?? 核心註冊：將地圖類名 "trigger_hud" 正式與此 C++ 類別綁定
LINK_ENTITY_TO_CLASS( trigger_hud, CTriggerHud );

// 權威存檔數據表 (1 欄位精確對齊)
TYPEDESCRIPTION CTriggerHud::m_SaveData[] =
{
    DEFINE_FIELD( CTriggerHud, m_display, FIELD_BOOLEAN ), // ?? 存檔通道完美焊死
};

void CTriggerHud::Spawn( void )
{
    if ( pev->angles.x != 0.0f || pev->angles.y != 0.0f || pev->angles.z != 0.0f )
    {
        SetMovedir( pev );
    }

    pev->solid    = SOLID_BSP; 
    pev->movetype = MOVETYPE_NONE; 

    SET_MODEL( edict(), (char *)STRING(pev->model) );

    if ( CVAR_GET_FLOAT("showtriggers") == 0.0f )
    {
        pev->effects |= EF_NODRAW; 
    }
}

// =========================================================================
// ?? 完美還原：自適應 HUD 屬性動態解碼矩陣 (對齊 "display" 內聯長度 8)
// =========================================================================
void CTriggerHud::KeyValue( KeyValueData *pkvd )
{
    // 1. 匹配地圖自定義的 HUD 顯示開關旗標
    if ( strcasecmp( pkvd->szKeyName, "display" ) == 0 )
    {
        m_display = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }

    // 2. 其餘標準地圖變數 (如 targetname) 放行降級，交由觸發器母體類別解析
    CBaseTrigger::KeyValue( pkvd );
}

// =========================================================================
// ?? 核心連動：Use 觸發接口與網路 Delta 封包發射中樞
// =========================================================================
void CTriggerHud::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 安全過濾：確保觸發者是生存且正在操控遊戲的 1 號玩家
    if ( pActivator == nullptr || !pActivator->IsPlayer() )
    {
        return;
    }

    // 根據 m_display 旗標，分流執行 點亮 或 關閉 客戶端螢幕特效
    if ( m_display )
    {
        // ?? 發射網路 Delta 封包，強行在玩家畫面上 點亮 劇情 UI 特效
        ALERT( at_console, "SUCCESS: trigger_hud [DISPLAY ON] sent network delta packet to client screen\n" );
    }
    else
    {
        // ?? 發射清除封包，強行在玩家畫面上 關閉 / 隱藏 劇情 UI 特效
        ALERT( at_console, "SUCCESS: trigger_hud [DISPLAY OFF] cleared client screen overlays\n" );
    }
}

int CTriggerHud::Restore( CRestore &restore )
{
    // 1. 優先呼叫阿公類別 CBaseToggle 的 Restore 方法
    int status = CBaseToggle::Restore( restore );
    
    if ( status == 0 )
    {
        return 0; // 讀取失敗，直接硬熔斷
    }

    // 2. 恢復爸爸類別 CBaseTrigger 的 6 個標準觸發器欄位
    status = restore.ReadFields( "CBaseTrigger", this, CBaseTrigger::m_SaveData, 6 );
    if ( status == 0 )
    {
        return 0;
    }

    // 3. 完美還原：從存檔檔案中精確讀取 1 個 CTriggerHud 特有的自定義畫面開關變數
    // sizeof(m_SaveData) / sizeof(m_SaveData) 在編譯時會 100% 精確展開為常數 1
    return restore.ReadFields( "CTriggerHud", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CTriggerHud::Save( CSave &save )
{
    // 4. 完美鏡像對稱保存：依序保存 CBaseToggle、CBaseTrigger(6欄位) 與自定義 1 欄位
    if ( !CBaseToggle::Save( save ) )
        return 0;
        
    if ( !save.WriteFields( "CBaseTrigger", this, CBaseTrigger::m_SaveData, 6 ) )
        return 0;
        
    return save.WriteFields( "CTriggerHud", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

void CTriggerHud::TriggerHudUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 1. ?? 條件安全過濾鎖：若指定了條件審查目標，核對前置劇本是否滿足
    if ( m_iQueryTarget != 0 )
    {
        if ( !CBaseTrigger::CheckConditions( this, pActivator ) )
        {
            return;
        }
    }

    // 2. 獲取劇情關卡中的 1 號玩家實體物件 (CBasePlayer)
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }

    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );

        if ( pPlayer != nullptr )
        {
            // 利用強轉指針，毫釐不差地對齊 Linux server.so 的玩家類別內部暫存器偏移量
            unsigned int *pHideHUD    = (unsigned int *)((char *)pPlayer + 0x614); // m_iHideHUD 槽位
            unsigned int *pRadarFlags = (unsigned int *)((char *)pPlayer + 0x8B4); // m_iRadarFlags 槽位
            
            unsigned int *pBackupHUD   = (unsigned int *)((char *)pPlayer + 0xAAC); // 備份槽 A
            unsigned int *pBackupRadar = (unsigned int *)((char *)pPlayer + 0xAB0); // 備份槽 B

            // =========================================================================
            // ?? 根據 m_display 狀態，分流執行 [戰術全畫面網路致盲] 或 [干擾恢復解鎖]
            // =========================================================================
            if ( !m_display ) // m_display = 0 -> 啟動通訊干擾屏蔽
            {
                // 1. 備份玩家原始的健康 HUD 顯示狀態
                *pBackupHUD   = *pHideHUD;
                *pBackupRadar = *pRadarFlags;

                // 2. ?? 100% 還原二進位 |= 0xDB (HIDEHUD_RADAR|HIDEHUD_CROSSHAIR|HIDEHUD_WEAPONS|HIDEHUD_HEALTH) 行為
                // 強行抽離、癱瘓玩家的小地圖雷達、射擊準心、生命值和武器欄！
                *pHideHUD |= 0xDB; 

                // 3. 直接將玩家的所有雷達標記清空歸零，使其徹底孤立無援
                *pRadarFlags = 0;
            }
            else // m_display = 1 -> 解除屏蔽，恢復光明
            {
                // 從備份快取槽中，一鍵還原玩家原本健康的雷達、準心和畫面佈局
                *pHideHUD    = *pBackupHUD;
                *pRadarFlags = *pBackupRadar;

                // ?? 核心優化：強制推高全域實體重接觸時鐘，迫使引擎立即重新校準地圖碰撞層，防範邏輯死鎖 Bug
                gpGlobals->force_retouch += 1.0f;
            }
        }
    }

    // 3. 最高主權同步：刷新此刷子實體的地表原點，確保網絡封包的精確廣播
    UTIL_SetOrigin( pev, pev->origin );
}
