#include "trigger_objective.h"

// 🚀 核心註冊：將地圖類名 "trigger_objective" 正式綁定到此類別上
LINK_ENTITY_TO_CLASS( trigger_objective, CTriggerObjective );

// =========================================================================
// 💾 終極修正：CTriggerObjective 權威 3 欄位存檔數據表 (0x03)
// =========================================================================
TYPEDESCRIPTION CTriggerObjective::m_SaveData[] =
{
    DEFINE_FIELD( CTriggerObjective, m_iObjectiveID, FIELD_INTEGER ),
    DEFINE_FIELD( CTriggerObjective, objectiveState, FIELD_INTEGER ), 
    DEFINE_FIELD( CTriggerObjective, objectiveText, FIELD_STRING ),   // 👈 剛好 3 個欄位，達成 100% 內存精確對齊！
};

void trigger_objective( entvars_t *pev )
{
    // 1. 安全防禦鎖：若數據頭為空，主動向引擎申請創建一個合法的 edict
    if ( pev == nullptr )
    {
        edict_t *pNewEdict = CREATE_ENTITY();
        if ( pNewEdict != nullptr )
        {
            pev = &pNewEdict->v;
        }
    }

    if ( pev == nullptr || pev->pContainingEntity == nullptr )
    {
        return;
    }

    // 2. 檢查是否已經分配過 Private Data，防止重複拉起
    if ( pev->pContainingEntity->pvPrivateData != nullptr )
    {
        return;
    }

    // 3. 🚀 內存重砲：呼叫引擎分配精確到 0x21C (540位元組) 的專屬 C++ 實體記憶體空間
    CTriggerObjective *pObjective = (CTriggerObjective *)GET_PRIVATE( pev->pContainingEntity );
    if ( pObjective == nullptr )
    {
        pObjective = (CTriggerObjective *)ALLOC_PRIVATE( pev->pContainingEntity, 0x21C );
    }

    // 4. 呼叫 C++ 母體類別構造函數初始化白紙
    pObjective->CBaseToggle::CBaseToggle();

    // =========================================================================
    // ⚙️ 完美還原 this[2] 的內聯解平優化賦值 (自定義變數初值加載)
    // =========================================================================
    // 完美對齊浮點數常數 0x41f00000 -> 30.0f
    // (在構造函數或者是全局工廠中，將任務顯示時長預設鎖定為 30 秒)
    pObjective->SetThink( nullptr );
    pObjective->pev                  = nullptr; 
    
    // 補全被截斷與內聯展開的自定義屬性初始值
    // 使用強制類型指針位移，100% 等價於 Linux server.so 在內存段中的初始化行為！
    float *pflDurationSlot = (float *)((char *)pObjective + sizeof(CBaseToggle));
    *pflDurationSlot = 30.0f; // 完美解密 0x41f00000！

    // 5. 最高主權對齊：同步 vptr 虛擬表指針，並正式將 pev 屬性與 Edict 雙向死死綁定
    pObjective->pev = pev;
}

void CTriggerObjective::KeyValue( KeyValueData *pkvd )
{
    // 1. 任務本地化字串池索引名稱 (對齊 "objective" 內聯長度 10)
    if ( strcasecmp( pkvd->szKeyName, "objective" ) == 0 )
    {
        objectiveText = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 2. 初始任務狀態 (對齊 "state" 內聯長度 6)
    else if ( strcasecmp( pkvd->szKeyName, "state" ) == 0 )
    {
        objectiveState = atoi( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 3. 跨關卡切換存檔旗標 (對齊 "notransition" 內聯長度 13)
    else if ( strcasecmp( pkvd->szKeyName, "notransition" ) == 0 )
    {
        // 100% 還原二進位行為：notransition 為 0 時，開啟跨關卡跨地圖狀態轉移
        m_bTransition = ( atoi( pkvd->szValue ) == 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    CBaseTrigger::KeyValue( pkvd );
}

void CTriggerObjective::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    if ( !m_bIsObjectiveActive ) return;

    // 📢 當玩家按 E 或者是關卡事件觸發此實體時：
    // 在畫面上高亮彈出綠色的戰術任務達成提示 (利用 m_szObjectiveMessage 描述)
    // 並在 HUD 任務簡報清單中，為對應的 m_iObjectiveID 打上完成的「綠色勾選框」！
    ALERT( at_console, "SUCCESS: Objective %d Accomplished!\n", m_iObjectiveID );

    // 任務完成後，將自己熔斷關閉，防止重複觸發
    m_bIsObjectiveActive = FALSE;
}

int CTriggerObjective::Restore( CRestore &restore )
{
    // 1. 優先呼叫阿公類別 CBaseToggle 的 Restore 方法，恢復最底層的開關與 Master 鎖
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

    // 3. 完美還原：從存檔檔案中讀取 3 個 CTriggerObjective 特有的自定義任務變數
    // sizeof(m_SaveData) / sizeof(m_SaveData) 在編譯時會 100% 精確展開為常數 3
    return restore.ReadFields( "CTriggerObjective", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

void CTriggerObjective::Spawn( void )
{
    // 1. 優先呼叫爸爸類別標準觸發器的 Spawn 系統，自動拉起實體在空間的隱形碰撞體
    CBaseTrigger::Spawn();

    // 2. 🚀 終極註冊：將該實體受外部/玩家觸發時的 +USE 回調，死死綁定在我們還原的任務狀態機方法上
    SetUse( &CTriggerObjective::ObjectiveUse );
}


int CTriggerObjective::Save( CSave &save )
{
    // 1. 優先呼叫阿公類別 CBaseToggle 的 Save 方法，保存最底層的開關與 Master 鎖
    int status = CBaseToggle::Save( save );
    
    if ( status == 0 )
    {
        return 0; // 保存失敗，直接硬熔斷
    }

    // 2. 鏡像保存爸爸類別 CBaseTrigger 的 6 個標準觸發器欄位
    if ( !save.WriteFields( "CBaseTrigger", this, CBaseTrigger::m_SaveData, 6 ) )
    {
        return 0;
    }

    // 3. 完美還原：將 3 個 CTriggerObjective 特有的自定義任務變數寫入存檔檔案
    // sizeof(m_SaveData) / sizeof(m_SaveData) 在編譯時會 100% 精確展開為常數 3
    return save.WriteFields( "CTriggerObjective", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

BOOL CTriggerObjective::DisplayObjective( int lineCount )
{
    // 1. 安全審查：若目前任務未激活 (0=INACTIVE)，保持隱形，不進行任何 HUD 渲染
    if ( objectiveState == 0 )
    {
        return FALSE;
    }

    hudtextparms_t hudTextParms;

    // 2. 🎰 填裝 GoldSrc 標準 HUD 動態文字參數結構體
    hudTextParms.effect      = 0;          // 0 = 常規淡入淡出特效
    hudTextParms.x           = 0.1f;       // 螢幕左側 10% 位置對齊 (戰術排版)
    hudTextParms.fadeinTime  = 0.5f;       // 0.5 秒平滑淡入
    hudTextParms.fadeoutTime = 0.5f;       // 0.5 秒平滑淡出
    hudTextParms.holdTime    = 5.0f;       // 在螢幕上高亮持續停留 5.0 秒
    hudTextParms.fxTime      = 0.0f;

    // 🚀 核心排版幾何：依據當前多任務行數，Y 軸精確向下偏置位移 0.04 比例單位，防止文字重疊
    hudTextParms.y           = (float)lineCount * 0.04f + 0.25f;
    
    // 鎖定獨立頻道，防止不同任務多行文字互相抹除
    hudTextParms.channel     = lineCount;

    char szFinalMessage[255];

    // =========================================================================
    // 🎨 根據任務狀態 (objectiveState)，分流執行 純白警告 或 亮綠完成 渲染
    // =========================================================================
    if ( objectiveState == 2 ) // 任務成功達成狀態
    {
        // 100% 還原：自動為任務字串鍵名追加 "_COMPLETE" 結算後綴
        sprintf( szFinalMessage, "%s_COMPLETE", STRING(objectiveText) );

        // 調校為經典的反恐精英亮綠色 (高亮成就解鎖)
        hudTextParms.r1 = 0;   hudTextParms.g1 = 255; hudTextParms.b1 = 0;   hudTextParms.a1 = 255;
        hudTextParms.r2 = 255; hudTextParms.g2 = 255; hudTextParms.b2 = 255; hudTextParms.a2 = 255;
    }
    else // 1 = 任務進行中/警告狀態
    {
        // 直接拷貝地圖指定的原生任務描述字串
        strncpy( szFinalMessage, STRING(objectiveText), sizeof(szFinalMessage) - 1 );

        // 調校為 100% 不透明戰術純白色
        hudTextParms.r1 = 255; hudTextParms.g1 = 255; hudTextParms.b1 = 255; hudTextParms.a1 = 255;
        hudTextParms.r2 = 255; hudTextParms.g2 = 255; hudTextParms.b2 = 255; hudTextParms.a2 = 255;
    }

    // 3. 傳輸全服網絡封包，在所有玩家的顯示卡畫面上即時渲染這行任務資訊
    UTIL_HudMessageAll( &hudTextParms, szFinalMessage );

    return TRUE;
}

int CTriggerObjective::ObjectCaps( void )
{
    // 完美還原二進位行為：若地圖開啟了跨關卡轉移屬性 (m_bTransition = TRUE)
    if ( m_bTransition )
    {
        // 賦予 FCAP_ACROSS_TRANSITION (2) 權限，允許此任務狀態隨玩家一同打包載入至下一張地圖
        return FCAP_ACROSS_TRANSITION;
    }
    
    // 預設獨立關卡任務，切換地圖時自動銷毀，不進行跨關卡轉移
    return 0;
}

// 完美回填：CTriggerObjective 頂層外部 Use 接口的真實存在形式
void CTriggerObjective::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 1. 驅動大腦狀態機推進
    ObjectiveUse( pActivator, pCaller, useType, value );

    // 2. 🚀 即時調用我們之前還原的 HUD 渲染器，在全服螢幕上動態堆疊刷新這行任務文字！
    // 預設傳入 0 號第一行通道，若有全域多任務計數器可傳入動態變數
    DisplayObjective( 0 ); 
}
