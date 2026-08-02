#include "trigger_changekeyvalue.h"

LINK_ENTITY_TO_CLASS( trigger_changekeyvalue, CTriggerChangeKeyValue );

// 完美對齊 35 個通道的數據存檔表 (1 + 1 + 1 + 16 + 16 = 35)
TYPEDESCRIPTION CTriggerChangeKeyValue::m_SaveData[] =
{
    DEFINE_FIELD( CTriggerChangeKeyValue, m_changeTargetName, FIELD_STRING ),
    DEFINE_FIELD( CTriggerChangeKeyValue, m_cTargets, FIELD_INTEGER ),
    DEFINE_ARRAY( CTriggerChangeKeyValue, m_iKey, FIELD_STRING, 16 ),
    DEFINE_ARRAY( CTriggerChangeKeyValue, m_iValue, FIELD_STRING, 16 ),
};

CTriggerChangeKeyValue::CTriggerChangeKeyValue()
{
    m_changeTargetName = 0;
    m_cTargets         = 0;
    memset( m_iKey, 0, sizeof(m_iKey) );
    memset( m_iValue, 0, sizeof(m_iValue) );
}

void CTriggerChangeKeyValue::Spawn( void )
{
    pev->solid    = SOLID_NOT;
    pev->movetype = MOVETYPE_NONE;
}

// =========================================================================
// 📡 完美還原：自適應通配符多重屬性批次動態解碼矩陣
// =========================================================================
void CTriggerChangeKeyValue::KeyValue( KeyValueData *pkvd )
{
    // 1. 安全防線過濾：若是引擎原生的 4 大最底層通用變數，降級放行，絕對不進行誤捕獲
    if ( strcasecmp( pkvd->szKeyName, "origin" ) == 0 ||
         strcasecmp( pkvd->szKeyName, "target" ) == 0 ||
         strcasecmp( pkvd->szKeyName, "targetname" ) == 0 ||
         strcasecmp( pkvd->szKeyName, "classname" ) == 0 )
    {
        CBaseDelay::KeyValue( pkvd ); // 降級派發給母體類別
        return;
    }

    // 2. 解析主要的目標物件名字群組
    if ( strcasecmp( pkvd->szKeyName, "changetarget" ) == 0 )
    {
        m_changeTargetName = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }

    // 3. 🚀 通配符大暴發：若屬於其餘自選自定義屬性，且未超出 16 組容量極限
    if ( m_cTargets < 16 )
    {
        char szCleanToken[128];

        // 100% 還原二進位行為：洗淨 KeyName 的前後空格引號並註冊進字串池
        UTIL_StripToken( pkvd->szKeyName, szCleanToken, sizeof(szCleanToken) );
        m_iKey[m_cTargets] = ALLOC_STRING( szCleanToken );

        // 洗淨 Value 數值字串並註冊進字串池
        UTIL_StripToken( pkvd->szValue, szCleanToken, sizeof(szCleanToken) );
        m_iValue[m_cTargets] = ALLOC_STRING( szCleanToken );

        // 遞增陣列指針
        m_cTargets++;
        pkvd->fHandled = TRUE;
    }
}

int CTriggerChangeKeyValue::Restore( CRestore &restore )
{
    // 1. 優先呼叫母體類別 CBaseDelay 的 Restore 方法，恢復標準的延時與 Target 鏈
    int status = CBaseDelay::Restore( restore );
    
    if ( status == 0 )
    {
        return 0; // 母體類別讀取失敗，直接硬熔斷退出
    }

    // 2. 完美還原：從存檔檔案中讀取 4 個自定義黃金通道 (sizeof(m_SaveData) 剛好展開為 4)
    return restore.ReadFields( "CTriggerChangeKeyValue", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CTriggerChangeKeyValue::Save( CSave &save )
{
    // 3. 完美鏡像對稱保存通道
    if ( !CBaseDelay::Save( save ) )
    {
        return 0;
    }
        
    return save.WriteFields( "CTriggerChangeKeyValue", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CTriggerChangeKeyValue::ObjectCaps( void )
{
    // 完美還原二進位行為：不具備跨關卡轉移權限，亦不接受玩家 E 鍵脈衝直接互動
    // 此實體屬於純粹的地圖局部邏輯物件，切換地圖時由伺服器核心自動從內存中安全註銷
    return 0;
}

void CTriggerChangeKeyValue::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    if ( pev->target == 0 )
    {
        return;
    }

    CBaseEntity *pStartEntity = nullptr;

    // 1. 全圖搜捕名字與當前改寫器 pev->target 一致的媒介實體群組
    while ( (pStartEntity = UTIL_FindEntityByString( pStartEntity, "targetname", STRING(pev->target) )) != nullptr )
    {
        // 2. 🚀 批次注入矩陣：若大腦中加載了合法的自定義屬性
        if ( m_cTargets > 0 )
        {
            for ( int i = 0; i < m_cTargets; i++ )
            {
                char szKeyBuf[128];
                char szValueBuf[128];

                // 安全提取快取陣列中的 Key 與 Value 字串 (對齊 0x80 = 128字元緩衝)
                strncpy( szKeyBuf, STRING(m_iKey[i]), sizeof(szKeyBuf) - 1 );
                strncpy( szValueBuf, STRING(m_iValue[i]), sizeof(szValueBuf) - 1 );

                // 100% 還原二進位 operator new(0x10) 16位元組 Heap 區動態分配行為
                KeyValueData *pkvd = new KeyValueData();

                pkvd->szKeyName   = szKeyBuf;
                pkvd->szValue     = szValueBuf;
                pkvd->fHandled    = FALSE;
                pkvd->szClassName = (char *)STRING( pStartEntity->pev->classname );

                // 🔥 核心重砲：呼叫引擎最高 Vanilla 接口，強行將改寫屬性實時派發注入目標 Edict！
                DispatchKeyValue( pStartEntity->pev->pContainingEntity, pkvd );

                // 實時銷毀記憶體，防堵內存洩漏
                delete pkvd;
            }
        }

        // 3. 🎯 終極主權移交：屬性注入完畢後，強行將該實體的下游 target 目標重新定位、修改為新劇本線！
        if ( m_changeTargetName != 0 )
        {
            pStartEntity->pev->target = m_changeTargetName;
        }
    }
}