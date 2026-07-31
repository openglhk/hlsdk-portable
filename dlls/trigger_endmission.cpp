#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h" 

class CTriggerEndMission : public CBaseToggle
{
public:
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    
    void EndMissionUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    void EndMissionTouch( CBaseEntity *pOther );
    void ChangeLevelThink( void ); 

    CTriggerEndMission();

private:
    char m_szNextMap[64]; 
    int  m_mp3Name; // DWARF 符號證實的 mp3Name 成員變數 (在 HLSDK 中字串通常儲存為 string_t / int)

    // ?? 完美還原：原廠 DWARF 暴露的自訂變數成員
    float duration;   // 黑幕淡出秒數 (對應 0x40a00000 = 5.0f)
    float holdtime;   // 黑幕停留秒數 (對應 0x40400000 = 3.0f)
    float alphaamt;   // 黑幕最高 Alpha 透明度 (對應 0x437f0000 = 255.0f)
    float r, g, b;    // 黑幕顏色 RGB 欄位
    
    unsigned short m_usHolsterEvent; // 銲槍收回事件快取
};

LINK_ENTITY_TO_CLASS( trigger_endmission, CTriggerEndMission );

CTriggerEndMission::CTriggerEndMission()
{
    memset(m_szNextMap, 0, sizeof(m_szNextMap));
    m_mp3Name = 0;
    
    // 完美還原：原廠反編譯中揭露的硬編碼初始數值
    duration = 5.0f;    
    holdtime = 3.0f;    
    alphaamt = 255.0f; 
    r = g = b = 0.0f; 
    m_usHolsterEvent = 0;
    if ( pev ) pev->nextthink = 0.0f;
}

void CTriggerEndMission::KeyValue( KeyValueData *pkvd )
{
    if ( strcasecmp(pkvd->szKeyName, "map") == 0 || strcasecmp(pkvd->szKeyName, "nextmap") == 0 ) {
        strncpy(m_szNextMap, pkvd->szValue, sizeof(m_szNextMap) - 1);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "mp3") == 0 || strcasecmp(pkvd->szKeyName, "mp3name") == 0 ) {
        m_mp3Name = ALLOC_STRING(pkvd->szValue); 
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "duration") == 0 ) {
        duration = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "holdtime") == 0 ) {
        holdtime = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else {
        CBaseToggle::KeyValue( pkvd );
    }
}

void CTriggerEndMission::Spawn( void )
{
    // ?? 完美還原：`g_pGameRules->_vptr.CGameRules[7]` (多人連線檢測)
    // 如果當前是多人模式，調用 pfnRemoveEntity 銷毀自身並直接返回
    if ( g_pGameRules && g_pGameRules->IsMultiplayer() ) {
        g_engfuncs.pfnRemoveEntity( ENT(pev) );
        return;
    }

    // ?? 完美還原：原廠對物理屬性、固體型態與特效清零的設定
    pev->solid    = SOLID_NOT;     // 對應 solid = 0
    pev->movetype = MOVETYPE_NONE;  // 對應 movetype = 0
    pev->effects  = 0;             // 對應 effects = 0
    pev->frame    = 0.0f;           // 對應 frame = 0.0

    if ( pev->model ) {
        SET_MODEL( ENT(pev), STRING(pev->model) );
    }

    // ?? 完美還原：極具價值的「借殼上市變數指派」核心！完全與原廠二進位檔對齊！
    pev->dmg_take   = this->duration;
    pev->dmg_save   = this->holdtime;
    pev->renderamt  = this->alphaamt;
    pev->rendercolor.x = this->r;
    pev->rendercolor.y = this->g;
    pev->rendercolor.z = this->b;

    if (m_szNextMap[0] == '\0' && pev->netname != 0) {
        strncpy(m_szNextMap, STRING(pev->netname), sizeof(m_szNextMap) - 1);
    }

    // 預載入單人模式特有的收回切割銲槍事件
    m_usHolsterEvent = PRECACHE_EVENT(1, "events/blowtorchholster.sc");

    // ?? 完美還原：動態掛載 Use 函數指標到 EndMissionUse
    SetUse( &CTriggerEndMission::EndMissionUse );

    // ?? 完美還原：如果地圖編輯器沒有勾選 spawnflags & 1，則動態掛載 Touch 進入點
    if ( (pev->spawnflags & 1) == 0 ) {
        SetTouch( &CTriggerEndMission::EndMissionTouch );
    }
}

// =========================================================================
// 4. 核心點火執行器 (EndMissionUse & Touch)
// =========================================================================

void CTriggerEndMission::EndMissionTouch( CBaseEntity *pOther )
{
    if ( pOther && pOther->IsPlayer() ) {
        EndMissionUse( pOther, this, USE_TOGGLE, 0.0f );
    }
}

void CTriggerEndMission::EndMissionUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 斷開點火器，防止玩家重複觸發
    SetUse( nullptr );

    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict || FNullEnt(pPlayerEdict)) return;

    // 完美還原：動態發送 mp3 play 語法給單人玩家客戶端控制台
    if ( m_mp3Name != 0 && STRING(m_mp3Name) != nullptr ) 
    {
        char cmdBuf[256];
        snprintf(cmdBuf, sizeof(cmdBuf), "mp3 play %s\n", STRING(m_mp3Name));
        g_engfuncs.pfnClientCommand(pPlayerEdict, cmdBuf);
    }
    // ?? 修正：直接傳入數字 1 (對應協定中的 FFADE_OUT 標記)，徹底根治巨集缺失問題！
    UTIL_ScreenFade(pActivator, pev->rendercolor, pev->dmg_take, pev->dmg_save, (int)pev->renderamt, 1);

    // 完美還原：播放戰術切割銲槍強制收回事件，清理玩家手持狀態
    if ( m_usHolsterEvent > 0 ) 
    {
        PLAYBACK_EVENT_FULL(0, pPlayerEdict, m_usHolsterEvent, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0);
    }

    // 完美還原：掛載動態 Think 定時器，時間一到正式引密切換關卡
    SetThink( &CTriggerEndMission::ChangeLevelThink );
    pev->nextthink = (gpGlobals->time + pev->dmg_save + pev->dmg_take) - 0.1f;
}

// =========================================================================
// 5. 時間軸終點：真正切換地圖 (ChangeLevelThink)
// =========================================================================
void CTriggerEndMission::ChangeLevelThink( void )
{
    if ( m_szNextMap[0] != '\0' ) 
    {
        g_engfuncs.pfnChangeLevel(m_szNextMap, nullptr);
    } 
    else if (gpGlobals->mapname != 0) 
    {
        g_engfuncs.pfnChangeLevel((char*)STRING(gpGlobals->mapname), nullptr);
    }
}
