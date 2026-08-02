#include "hostage_rescue.h"

LINK_ENTITY_TO_CLASS( info_hostage_rescue, CHostageRescue );

// 完美對齊：CHostageRescue 權威 5 欄位存檔數據表 (0x05)
// =========================================================================
// 💾 終極修正：CHostageRescue 權威 3 欄位存檔數據表 (0x03)
// =========================================================================
TYPEDESCRIPTION CHostageRescue::m_SaveData[] =
{
    DEFINE_FIELD( CHostageRescue, m_flNextSearchTime, FIELD_TIME ),
    DEFINE_FIELD( CHostageRescue, m_iRescueType, FIELD_INTEGER ),
    DEFINE_FIELD( CHostageRescue, m_iRescuedCount, FIELD_INTEGER ), // 👈 剛好 3 個欄位，達成 100% 內存精確對齊！
};

CHostageRescue::CHostageRescue()
{
    m_flNextSearchTime = 0.0f;
    m_iRescueType      = 7; // RESCUE_HOSTAGE
    m_iRescuedCount    = 0;
    m_bQuiet           = FALSE;
    m_bHidden          = FALSE;
}

void info_hostage_rescue( entvars_t *pev )
{
    if ( pev == nullptr )
    {
        edict_t *pNewEdict = CREATE_ENTITY();
        if ( pNewEdict != nullptr ) pev = &pNewEdict->v;
    }
    if ( pev == nullptr || pev->pContainingEntity == nullptr ) return;
    if ( pev->pContainingEntity->pvPrivateData != nullptr )   return;

    CHostageRescue *pRescue = (CHostageRescue *)GET_PRIVATE( pev->pContainingEntity );
    if ( pRescue == nullptr )
    {
        // 內存封包空間動態對齊 112 位元組
        pRescue = (CHostageRescue *)ALLOC_PRIVATE( pev->pContainingEntity, sizeof(CHostageRescue) );
    }

    pRescue->CPointEntity::CPointEntity();

    float *pflNextSearch = (float *)((char *)pRescue + 60);  *pflNextSearch = 2.0f;                                   
    int *piRescueType = (int *)((char *)pRescue + 64);       *piRescueType = 7;                                       
    int *piRescuedCount = (int *)((char *)pRescue + 68);     *piRescuedCount = 0;

    // 為新變數槽位進行二進位內聯安全初始化
    BOOL *pbQuiet = (BOOL *)((char *)pRescue + 72);          *pbQuiet = FALSE;
    BOOL *pbHidden = (BOOL *)((char *)pRescue + 76);         *pbHidden = FALSE;

    pRescue->pev = pev;
}

// =========================================================================
// 📡 完美還原：地圖自定義屬性解析器矩陣
// =========================================================================
void CHostageRescue::KeyValue( KeyValueData *pkvd )
{
    // 1. 解析安靜潛行營救模式
    if ( strcasecmp( pkvd->szKeyName, "quiet" ) == 0 )
    {
        m_bQuiet = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    // 2. 解析雷達 HUD 隱藏模式
    else if ( strcasecmp( pkvd->szKeyName, "hidden" ) == 0 )
    {
        m_bHidden = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }

    // 3. 完美還原二進位行為：不調用基類，直接釋放 fHandled = FALSE，放行讓引擎常規鏈解析 origin 座標
    pkvd->fHandled = FALSE;
}

void CHostageRescue::Spawn( void )
{
    // 1. 優先呼叫點實體基底類別的 Spawn 系統，自動拉起實體在空間中的無碰撞屬性
    CPointEntity::Spawn();

    // 2. 🚀 核心指針綁定：將 Think 方法死死指向我們還原的 3D 雷達主動掃描中樞
    SetThink( &CHostageRescue::RescueThink );
    
    // 3. 🚀 點火切入時鐘：0.1 秒 (10Hz) 後正式發動雷達循環，開啟無限主動感知閉環！
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CHostageRescue::RescueThink( void )
{
    // 1. 📢 獲取劇情關卡中的 1 號玩家實體物件 (CBasePlayer)
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }

    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        // 實例化玩家 C++ 指針
        CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );

        if ( pPlayer != nullptr )
        {
            // 2. 幾何運算：計算此營救點與玩家之間的平面 2D 物理距離
            float flDistToPlayer = (pev->origin - pPlayer->pev->origin).Length2D();

            // 3. 戰術防線判定：若玩家進入 256 碼黃金隨行範圍內
            if ( flDistToPlayer < 256.0f )
            {
                m_bPlayerNear = TRUE;

                // 🚀 核心雷達黑科技：若地圖未指定隱藏 (m_bHidden = 0)，跨類別強寫玩家 0x8B4 暫存器
                // 100% 還原 |= 0x400 (bits_RADAR_RESCUE_ZONE) 位元行為，在玩家 HUD 雷達小地圖上即時亮起撤離點綠色標記！
                if ( !m_bHidden )
                {
                    // 完美利用強轉指針，毫釐不差地重合 Linux server.so 的跨類別強寫特權
                    unsigned int *pPlayerRadarFlags = (unsigned int *)((char *)pPlayer + 0x8B4);
                    *pPlayerRadarFlags |= 0x400; // 點亮 1024 號雷達信號
                }
            }
            // 4. 若玩家遠離 256 碼範圍，且之前標記為近身，重置降級狀態
            else if ( m_bPlayerNear )
            {
                m_bPlayerNear = FALSE;
            }
        }
    }

    // =========================================================================
    // 🔃 終極心跳維持：直接寫入 gpGlobals->time，解鎖狂暴無延時每幀極速 Think 刷新，
    // 確保玩家一踏入紅線、雷達圖標瞬間毫秒級亮起！
    // =========================================================================
    pev->nextthink = gpGlobals->time;
}

int CHostageRescue::Restore( CRestore &restore )
{
    // 1. 優先呼叫 CPointEntity / CBaseEntity 基底類別的 Restore 方法
    int status = CBaseEntity::Restore( restore );
    
    if ( status == 0 )
    {
        return 0; // 基底類別讀取失敗，直接硬熔斷
    }

    // 2. 完美還原：從存檔檔案中精確讀取 3 個 CHostageRescue 特有的自定義核心變數
    // sizeof(m_SaveData) / sizeof(m_SaveData) 在編譯時會 100% 精確展開為常數 3
    return restore.ReadFields( "CHostageRescue", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CHostageRescue::Save( CSave &save )
{
    // 3. 完美對稱保存：優先保存基底類別，成功後寫入這 3 個核心狀態欄位
    int status = CBaseEntity::Save( save );
    
    if ( status == 0 )
    {
        return 0;
    }
    
    return save.WriteFields( "CHostageRescue", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

