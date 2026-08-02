#include "genericitem.h"

// 🚀 核心註冊：將 Hammer 地圖編輯器中的類名 "item_generic" 綁定到這個 C++ 類別上
LINK_ENTITY_TO_CLASS( item_generic, CGenericItem );

// 完美對齊：CGenericItem 權威 6 欄位存檔數據表 (0x06)
TYPEDESCRIPTION CGenericItem::m_SaveData[] =
{
    DEFINE_FIELD( CGenericItem, m_bCanSpawn, FIELD_BOOLEAN ),
    DEFINE_FIELD( CGenericItem, m_iSequence, FIELD_STRING ),
    DEFINE_FIELD( CGenericItem, m_iLightValue, FIELD_INTEGER ),
    DEFINE_FIELD( CGenericItem, m_removeOnUse, FIELD_BOOLEAN ),
    DEFINE_FIELD( CGenericItem, m_bSetPVSDefault, FIELD_BOOLEAN ),
    DEFINE_FIELD( CGenericItem, m_nopvs, FIELD_BOOLEAN ),
};

void CGenericItem::Precache( void )
{
    // 100% 鏡像對齊 Linux server.so 的底層行為，直接將模型路徑打入引擎快取
    PRECACHE_MODEL( (char *)STRING( pev->model ) );
}

void CGenericItem::KeyValue( KeyValueData *pkvd )
{
    // 1. PVS 網路優化旗標首次安全初始化
    if ( !m_bSetPVSDefault )
    {
        pev->iuser2 = 0;
        m_bSetPVSDefault = TRUE;
    }

    // =========================================================================
    // 🏢 地圖自定義屬性自適應字串匹配矩陣 (不區分大小寫)
    // =========================================================================
    
    // 1. 動態動畫序列字串名稱
    if ( strcasecmp( pkvd->szKeyName, "sequencename" ) == 0 )
    {
        m_iSequence = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 2. 道具貼圖外觀紋理索引
    else if ( strcasecmp( pkvd->szKeyName, "skin" ) == 0 )
    {
        pev->skin = atof( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 3. 道具子部件組模型索引
    else if ( strcasecmp( pkvd->szKeyName, "body" ) == 0 )
    {
        pev->body = atoi( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 4. 戰術外殼螢光特效乘法器 (100% 還原二進位行為：浮點數乘以 10 壓縮為整數)
    else if ( strcasecmp( pkvd->szKeyName, "lightmultiplier" ) == 0 )
    {
        m_iLightValue = (int)( atof( pkvd->szValue ) * 10.0f );
        pkvd->fHandled = TRUE;
        return;
    }
    // 5. 可見性優化裁剪開關
    else if ( strcasecmp( pkvd->szKeyName, "nopvs" ) == 0 )
    {
        pev->iuser2 = atoi( pkvd->szValue );
        m_nopvs     = ( pev->iuser2 != 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    // 6. 按 E 鍵互動使用後是否自動銷毀
    else if ( strcasecmp( pkvd->szKeyName, "removeonuse" ) == 0 )
    {
        m_removeOnUse = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }

    // =========================================================================
    // 2. 其餘標準底層屬性 (如 targetname, origin) 降級交由母體類別方法解析
    // =========================================================================
    CBaseMonster::KeyValue( pkvd );
}

// 貼上我們上一個回合完美還原的 Spawn() 函數
void CGenericItem::Spawn( void )
{
    BOOL bValidRules = ( g_pGameRules != nullptr );
    m_bCanSpawn = bValidRules;

    if ( bValidRules )
    {
        pev->solid    = SOLID_NOT;
        pev->movetype = MOVETYPE_NONE;
        pev->effects  = 0;
        pev->frame    = 0.0f;
        pev->iuser1   = 0;

        pev->iuser3   = pev->spawnflags;
        pev->flags   |= FL_DONTTHINK; 

        Precache();
        SET_MODEL( edict(), STRING(pev->model) );

        if ( m_iSequence == 0 )
        {
            // 靜態道具
        }
        else
        {
            SetThink( &CGenericItem::StartItem );
            pev->nextthink = gpGlobals->time + 0.1f;
        }

        if ( pev->spawnflags & 1 ) // SF_ITEM_DROP_TO_FLOOR
        {
            if ( DROP_TO_FLOOR( edict() ) == 0 )
            {
                ALERT( at_error, "Item %s fell out of level at %f,%f,%f", 
                       STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z );
                UTIL_Remove( this );
                return;
            }
        }

        if ( pev->spawnflags & 2 ) // SF_ITEM_SOLID_FROM_ANIM
        {
            pev->solid = SOLID_SLIDEBOX;
            int iSeqIndex = LookupSequence( STRING(m_iSequence) );
            
            if ( iSeqIndex == -1 )
            {
                ALERT( at_console, "ERROR! FIX ME: item generic: %s, model: %s, does not have animation: %s\n",
                       STRING(pev->targetname), STRING(pev->model), STRING(m_iSequence) );
                iSeqIndex = 0;
            }

            ExtractBbox( iSeqIndex, pev->mins, pev->maxs );
            UTIL_SetSize( pev, pev->mins, pev->maxs );
            UTIL_SetOrigin( pev, pev->origin );
        }

        if ( m_iLightValue != 0 )
        {
            pev->renderfx = kRenderFxGlowShell;
            pev->iuser4   = m_iLightValue;
        }

        if ( pev->rendermode == kRenderNormal && pev->renderamt == 0.0f )
        {
            pev->renderamt  = 255.0f;
            pev->rendermode = kRenderTransTexture;
            return;
        }
    }
    else
    {
        SetThink( &CGenericItem::SUB_Remove );
        pev->nextthink = gpGlobals->time + 1.0f;
    }
}

void CGenericItem::StartItem( void )
{
    // 1. 清空殘留特效旗標，確保乾淨的動畫渲染起手式
    pev->effects = 0;

    // 2. 根據指定的動畫字串名稱，反查模型內部的真實 Sequence ID 序列號索引
    pev->sequence = LookupSequence( STRING(m_iSequence) );

    // 3. 🛡️ 防崩潰安全防線：若地圖製作者拼錯名字找不到動作 (-1)
    if ( pev->sequence == -1 )
    {
        pev->sequence = 0; // 強行降級回填為 0 號基準 Idle 動作，防止引擎崩潰
        
        // 噴出主控台警告，通知開發者修正地圖名字
        ALERT( at_console, "ERROR! FIX ME: item generic: %s, model: %s, does not have animation: %s\n",
               STRING(pev->targetname), STRING(pev->model), STRING(m_iSequence) );
    }

    // 4. 初始化動畫幀，刷新 StudioMdl 骨骼動畫速率、長度數據頭
    pev->frame = 0.0f;
    ResetSequenceInfo();

    // 5. 🚀 大腦思維轉移：正式將 Think 驅動指向 AnimateThink，開啟 10Hz 每幀動畫前推循環
    SetThink( &CGenericItem::AnimateThink );
    pev->nextthink = gpGlobals->time + 0.1f;

    // 二次確保首幀為 0.0 起始點
    pev->frame = 0.0f;
}

int CGenericItem::Save( CSave &save )
{
    // 1. 優先呼叫母體類別怪物的 Save 方法，保存世界基礎空間座標與角度變數
    int status = CBaseMonster::Save( save );
    
    if ( status == 0 )
    {
        return 0; // 基底類別保存失敗，直接熔斷退出
    }

    // 2. 完美還原：將 6 個 CGenericItem 特有的自定義屬性變數寫入存檔檔案
    // sizeof(m_SaveData) / sizeof(m_SaveData) 在編譯時會 100% 精確展開為常數 6 (0x06)
    return save.WriteFields( "CGenericItem", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CGenericItem::Restore( CRestore &restore )
{
    // 1. 優先呼叫母體類別怪物的 Restore 方法，恢復基礎尋路和血量變數
    int status = CBaseMonster::Restore( restore );
    
    if ( status == 0 )
    {
        return 0; // 母體類別讀取失敗，直接熔斷退出
    }

    // 2. 完美還原：從存檔檔案中讀取 6 個 CGenericItem 特有的自定義屬性變數
    // sizeof(m_SaveData) / sizeof(m_SaveData[0]) 在編譯時會 100% 精確展開為常數 6
    int iRet = restore.ReadFields( "CGenericItem", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData[0]) );

    // 3. 🚀 網路同步回填：讀檔成功後，即時將 PVS 裁剪狀態同步寫入網路暫存器 iuser2
    pev->iuser2 = m_nopvs;

    // 4. 🚀 特效同步回填：若具備螢光特效，即時將螢光數值同步寫入網路暫存器 iuser4，防止特效丟失 Bug
    if ( m_iLightValue != 0 )
    {
        pev->iuser4 = m_iLightValue;
    }

    return iRet;
}

void CGenericItem::AnimateThink( void )
{
    // 1. 動態派發目前動畫幀中嵌入的所有腳本/音效事件 (0.1秒步進)
    DispatchAnimEvents( 0.1f );

    // 2. 驅動引擎核心 StudioMdl 骨骼引擎，前推、刷新模型的 3D 頂點幀數
    StudioFrameAdvance( 0.0f ); // 傳入 0.0f 觸發引擎自動時間增量計算

    // 3. 大閉環重置：若是一段不循環的單次動作，且目前已經播到了最後一幀
    if ( m_fSequenceFinished && !m_fSequenceLoops )
    {
        pev->frame = 0.0f; // 強制將動畫幀重設回起始點
        ResetSequenceInfo(); // 重新整理動畫速率頭數據
    }

    // 4. 設定下一次 Think 的刷新時鐘 (10Hz 頻率，即每 0.1 秒刷新一次動畫)
    pev->nextthink = gpGlobals->time + 0.1f;
    
    // 同步刷新最後一次的動畫時間戳
    m_lastTime = gpGlobals->time;
}

void CGenericItem::Restart( void )
{
    // 1. 📢 核心重置：無條件洗掉隱形旗標 (~EF_NODRAW = ~0x80)，讓道具在新回合重新顯形
    pev->effects &= ~EF_NODRAW; 

    // 2. 重新拉起動畫 Think 時鐘鏈
    if ( m_iSequence != 0 )
    {
        SetThink( &CGenericItem::StartItem );
        pev->nextthink = gpGlobals->time + 0.1f;
    }

    // =========================================================================
    // 🏢 旗標 1 重置：重新垂直落地下榻到固體地面上
    // =========================================================================
    if ( pev->spawnflags & 1 ) // SF_ITEM_DROP_TO_FLOOR
    {
        if ( DROP_TO_FLOOR( edict() ) == 0 )
        {
            // 防崩潰兜底：重置時若發現掉出地圖外，直接安全註銷銷毀
            ALERT( at_error, "Item %s fell out of level at %f,%f,%f", 
                   STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z );
            UTIL_Remove( this );
            return;
        }
    }

    // =========================================================================
    // 📦 旗標 2 重置：重新從 3D 模型 MDL 頭部動態抽回物理包圍盒
    // =========================================================================
    if ( pev->spawnflags & 2 ) // SF_ITEM_SOLID_FROM_ANIM
    {
        pev->solid = SOLID_SLIDEBOX; // 3

        Vector vecMins, vecMaxs;
        int iSeqIndex = LookupSequence( STRING(m_iSequence) );
        
        if ( iSeqIndex == -1 )
        {
            ALERT( at_console, "ERROR! FIX ME: item generic: %s, model: %s, does not have animation: %s\n",
                   STRING(pev->targetname), STRING(pev->model), STRING(m_iSequence) );
            iSeqIndex = 0;
        }

        // 重新抽取並刷新世界空間物理象限尺寸
        ExtractBbox( iSeqIndex, vecMins, vecMaxs );
        UTIL_SetSize( pev, vecMins, vecMaxs );
        UTIL_SetOrigin( pev, pev->origin );
    }
}

void CGenericItem::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 1. 戰術硬熔斷：一旦道具被觸發使用，立即切斷並註銷 Think 時鐘鏈，停止骨骼動畫步進
    SetThink( nullptr );

    // =========================================================================
    // 🏢 根據 removeonuse 旗標，分流執行 銷毀 或 隱形重置
    // =========================================================================
    if ( !m_removeOnUse )
    {
        // 2. 常駐互動分支：關閉物理碰撞體盒 (SOLID_NOT = 0)，防止卡死玩家
        pev->solid = SOLID_NOT;

        // 3. 打上隱形旗標 (EF_NODRAW = 0x80)，使其在世界中隱形，等待下一回合自適應 Restart() 顯形
        pev->effects |= EF_NODRAW;
        return;
    }

    // 4. 一次性道具分支：直接呼叫引擎核心方法，將道具從世界與記憶體中徹底銷毀移除
    UTIL_Remove( this );
}
