#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "monsters.h"

// =========================================================================
// 1. CMonsterCtRepel 類別宣告 (對齊 DWARF 變數空間)
// =========================================================================

class CMonsterCtRepel : public CBaseDelay
{
public:
    void Spawn( void ) override;
    void Precache( void ) override;
    void KeyValue( KeyValueData *pkvd ) override; // ?? 終極還原：經二進位檔 Fact Check 核實的真實屬性解析
    
    void RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    void SlideThink( void ); 

    int Save( CSave &save ) override;
    int Restore( CRestore &restore ) override;
    static TYPEDESCRIPTION m_SaveData[];

    CMonsterCtRepel();

private:
    string_t m_classname;        // 落地衍生兵種名 (由 monstertype 賦值)
    int      m_iSpriteTexture;   // 繩索材質索引
    float    m_flSlideSpeed;     // 垂降速度
    
    // ?? 完美還原：原廠自訂欄位
    int      m_iHead;            // 隊員頭部骨骼 (由 repelhead 賦值)
    int      m_iSkin;            // 隊員皮膚網格 (由 repelskin 賦值)
    CBaseEntity* m_pSpawnedCt;   // 當前正在向下索降的真實 AI 實體指標
};

LINK_ENTITY_TO_CLASS( monster_ct_repel, CMonsterCtRepel );

// =========================================================================
// 2. 存讀檔持久化映射表 (3 欄位空間對齊)
// =========================================================================

TYPEDESCRIPTION CMonsterCtRepel::m_SaveData[] =
{
    DEFINE_FIELD( CMonsterCtRepel, m_classname, FIELD_STRING ),
    DEFINE_FIELD( CMonsterCtRepel, m_iSpriteTexture, FIELD_INTEGER ),
    DEFINE_FIELD( CMonsterCtRepel, m_flSlideSpeed, FIELD_FLOAT ),
};

int CMonsterCtRepel::Save( CSave &save ) {
    if ( !CBaseDelay::Save( save ) ) return 0;
    return save.WriteFields( "CCounterTerroristRepel", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CMonsterCtRepel::Restore( CRestore &restore ) {
    int status = CBaseDelay::Restore( restore );
    if ( status == 0 ) return 0;
    return restore.ReadFields( "CCounterTerroristRepel", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

// =========================================================================
// 3. 資源加載與建構子初始化
// =========================================================================

CMonsterCtRepel::CMonsterCtRepel()
{
    m_classname = 0; m_iSpriteTexture = 0; m_flSlideSpeed = 250.0f;
    m_iHead = m_iSkin = 0; m_pSpawnedCt = nullptr;
    if ( pev ) pev->nextthink = 0.0f;
}

// ?? 完美還原：100% 對照原廠 DWARF 彙編的 KeyValue 欄位分割
void CMonsterCtRepel::KeyValue( KeyValueData *pkvd )
{
    if ( strcasecmp(pkvd->szKeyName, "repelskin") == 0 ) 
    {
        this->m_iSkin = strtol(pkvd->szValue, nullptr, 10);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "repelhead") == 0 ) 
    {
        this->m_iHead = strtol(pkvd->szValue, nullptr, 10);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "monstertype") == 0 ) 
    {
        // 確核：原廠官方實名欄位為 monstertype，由此指派變身兵種！
        this->m_classname = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "speed") == 0 ) 
    {
        m_flSlideSpeed = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else 
    {
        // 回退呼叫 CBaseMonster/父類別 KeyValue
        CBaseDelay::KeyValue( pkvd );
    }
}

void CMonsterCtRepel::Precache( void )
{
    if ( m_classname == 0 ) {
        m_classname = ALLOC_STRING("monster_ct_sas"); 
    }
    UTIL_PrecacheOther( STRING(m_classname) );
    PRECACHE_SOUND("common/body_zipdrop.wav");
    PRECACHE_SOUND("common/body_ziploop.wav");
    m_iSpriteTexture = PRECACHE_MODEL("sprites/rope.spr");
    PRECACHE_MODEL("models/sas.mdl");
}

// =========================================================================
// 4. 誕生初始化 (Spawn)
// =========================================================================

void CMonsterCtRepel::Spawn( void )
{
    Precache();

    pev->solid        = SOLID_NOT;     
    pev->movetype     = MOVETYPE_NONE;  
    pev->effects      = EF_NODRAW;     

    SET_MODEL( ENT(pev), "models/sas.mdl" );
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 72) );

    SetUse( &CMonsterCtRepel::RepelUse );
}

// =========================================================================
// 5. 索降點火執行器 (RepelUse)
// =========================================================================

void CMonsterCtRepel::RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    TraceResult tr;
    UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, -4096.0f ), ignore_monsters, ENT(pev), &tr );

    CBaseEntity *pNewEnt = CBaseEntity::Create( (char*)STRING(m_classname), pev->origin, pev->angles, nullptr );
    if ( !pNewEnt ) {
        ALERT(at_console, "NULL Ent in Create!\n");
        return;
    }

    m_pSpawnedCt = pNewEnt;

    pNewEnt->pev->owner  = nullptr;
    pNewEnt->pev->origin = pev->origin;
    pNewEnt->pev->angles = pev->angles;
    pNewEnt->pev->health = 200.0f;          
    pNewEnt->pev->movetype = MOVETYPE_FLY;   
    pNewEnt->pev->solid    = SOLID_NOT;

    float flRandomZSpeed = RANDOM_FLOAT( -196.0f, -128.0f );
    pNewEnt->pev->velocity = Vector( 0.0f, 0.0f, flRandomZSpeed );
    
    // ?? 完美還原：將地圖讀取到的皮膚與頭部外觀傳遞給向下索降的真實 AI 實體！
    pNewEnt->pev->skin     = m_iSkin;
    pNewEnt->pev->body     = m_iHead; 

    CBaseMonster *pMonster = pNewEnt->MyMonsterPointer();
    if ( pMonster )
    {
        pMonster->m_Activity = ACT_GLIDE;
        int iSequence = pMonster->LookupActivity( ACT_GLIDE );
        if ( iSequence < 0 ) {
            ALERT( at_console, "%s has no sequence for act:%d\n", STRING(pNewEnt->pev->classname), 0x33 );
            pNewEnt->pev->sequence = 0;
        } else {
            pNewEnt->pev->frame = 0.0f;
            pNewEnt->pev->sequence = iSequence;
            pMonster->ResetSequenceInfo();
            pNewEnt->pev->yaw_speed = 90.0f;
        }
    }

    EMIT_SOUND( ENT(pNewEnt->pev), CHAN_STATIC, "common/body_ziploop.wav", 1.0, ATTN_NORM );
    SetThink( &CMonsterCtRepel::SlideThink );
    pev->nextthink = gpGlobals->time;
}

// =========================================================================
// 6. 著地變身檢測狀態機 (SlideThink)
// =========================================================================

void CMonsterCtRepel::SlideThink( void )
{
    if ( m_pSpawnedCt == nullptr || m_pSpawnedCt->pev == nullptr ) {
        SetThink( nullptr );
        return;
    }

    TraceResult tr;
    UTIL_TraceLine( m_pSpawnedCt->pev->origin, m_pSpawnedCt->pev->origin + Vector( 0, 0, -32.0f ), ignore_monsters, ENT(m_pSpawnedCt->pev), &tr );

    if ( tr.flFraction == 1.0f ) {
        pev->nextthink = gpGlobals->time + 0.1f;
        return;
    }

    STOP_SOUND( ENT(m_pSpawnedCt->pev), CHAN_STATIC, "common/body_ziploop.wav" );
    EMIT_SOUND( ENT(m_pSpawnedCt->pev), CHAN_VOICE, "common/body_zipdrop.wav", 1.0, ATTN_NORM );

    m_pSpawnedCt->pev->movetype = MOVETYPE_STEP;  
    m_pSpawnedCt->pev->solid    = SOLID_SLIDEBOX; 
    m_pSpawnedCt->pev->velocity = g_vecZero;
    m_pSpawnedCt->pev->targetname = pev->targetname; 

    CBaseMonster *pMonster = m_pSpawnedCt->MyMonsterPointer();
    if ( pMonster ) {
        pMonster->m_Activity = ACT_IDLE;
        pMonster->SetActivity( ACT_IDLE ); 
    }

    SetThink( &CBaseEntity::SUB_Remove );
    pev->nextthink = gpGlobals->time;
}
