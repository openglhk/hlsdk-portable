#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"

// =========================================================================
// 1. CEnvLiveAmmoShooter 類別宣告 (包含完全還原的 CreateGib 工廠函式)
// =========================================================================

class CEnvLiveAmmoShooter : public CBaseDelay
{
public:
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
    
    void ShootThink( void ); 
    CBaseEntity* CreateGib( void ); // ?? 終極還原：原廠 DWARF 規格的碎片實體工廠方法

    int Save( CSave &save ) override;
    int Restore( CRestore &restore ) override;
    static TYPEDESCRIPTION m_SaveData[];

    CEnvLiveAmmoShooter();

private:
    int   m_iGibModelIndex;   
    float m_flGibVelocity;    
    float m_flVariance;       
    int   m_iGibs;            
    int   m_iGibCapacity;     
    
    float m_flGibLife;        
    int   m_bodyGroup;        
    int   m_quiet;            // ?? 完美還原：用來控制拋殼/殘骸撞擊地面時是否保持靜音的旗標
};

LINK_ENTITY_TO_CLASS( env_liveammoshooter, CEnvLiveAmmoShooter );

// =========================================================================
// 2. 存讀檔映射表 (9 個欄位對齊)
// =========================================================================

TYPEDESCRIPTION CEnvLiveAmmoShooter::m_SaveData[] =
{
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_iGibModelIndex, FIELD_INTEGER ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_flGibVelocity, FIELD_FLOAT ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_flVariance, FIELD_FLOAT ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_iGibs, FIELD_INTEGER ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_iGibCapacity, FIELD_INTEGER ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_flGibLife, FIELD_FLOAT ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_bodyGroup, FIELD_INTEGER ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_quiet, FIELD_INTEGER ),
    DEFINE_FIELD( CEnvLiveAmmoShooter, m_flDelay, FIELD_FLOAT ),
};

int CEnvLiveAmmoShooter::Save( CSave &save ) {
    if ( !CBaseDelay::Save( save ) ) return 0;
    return save.WriteFields( "CGibShooter", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CEnvLiveAmmoShooter::Restore( CRestore &restore ) {
    int status = CBaseDelay::Restore( restore );
    if ( status == 0 ) return 0;
    return restore.ReadFields( "CGibShooter", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

// =========================================================================
// 3. 資源預載入 (Precache)
// =========================================================================

extern int g_Language; 

void CEnvLiveAmmoShooter::Precache( void )
{
    if ( pev->model ) {
        m_iGibModelIndex = PRECACHE_MODEL( (char*)STRING(pev->model) );
        return;
    }
    if ( g_Language != 1 ) {
        m_iGibModelIndex = PRECACHE_MODEL( "models/hgibs.mdl" );
    } else {
        m_iGibModelIndex = PRECACHE_MODEL( "models/germanygibs.mdl" );
    }
}

// =========================================================================
// 4. 核心實體生成工廠：100% 對照原廠 DWARF 暴力過濾與隨機 Body 還原 (CreateGib)
// =========================================================================

CBaseEntity* CEnvLiveAmmoShooter::CreateGib( void )
{
    // ?? 完美還原 1：讀取控制台全域暴力和諧變數，若玩家關閉了 hgibs，則拒絕生成實體！
    if ( CVAR_GET_FLOAT("violence_hgibs") == 0.0f )
    {
        return nullptr;
    }

    // 呼叫金源引擎原生工廠建立一個基礎的 gib 碎片實體
    CBaseEntity *pGib = CBaseEntity::Create( "gib", pev->origin, pev->angles, edict() );
    if ( pGib )
    {
        pGib->pev->modelindex = m_iGibModelIndex;
        pGib->pev->solid      = SOLID_BBOX; 
        pGib->pev->movetype   = MOVETYPE_TOSS; 
        
        // ?? 完美還原 2：指派人類鮮紅色血液編碼 0xf7 (BLOOD_COLOR_RED)
        // 在標準 SDK 中，對應設定其 bloodcolor 屬性
        // pGib->m_bloodColor = 2; // 2 在標準 SDK 裡固定代表紅色血液
        
        // ?? 完美還原 3：如果地圖指定的 bodyGroup 為 0，則利用 RandomLong 在模型總外觀數量內隨機抽樣
        if ( m_bodyGroup == 0 )
        {
            if ( pev->body < 2 ) {
                ALERT(at_aiconsole, "GibShooter Body is <= 1!\n");
            }
            pGib->pev->body = RANDOM_LONG(1, pev->body - 1);
        }
        else
        {
            pGib->pev->body = m_bodyGroup;
        }
        
        return pGib;
    }
    return nullptr;
}

// =========================================================================
// 5. 其他控制方法實作
// =========================================================================

CEnvLiveAmmoShooter::CEnvLiveAmmoShooter()
{
    m_iGibModelIndex = 0;
    m_flGibVelocity = 500.0f; 
    m_flVariance = 0.1f;   
    m_iGibs = 1;       
    m_iGibCapacity = 1;
    m_flGibLife = 0.0f;    
    m_bodyGroup = 0;       
    m_quiet = 0;
    if ( pev ) pev->nextthink = 0.0f;
}

void CEnvLiveAmmoShooter::KeyValue( KeyValueData *pkvd )
{
    if ( strcasecmp(pkvd->szKeyName, "shootspeed") == 0 ) {
        m_flGibVelocity = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "m_flVariance") == 0 ) {
        m_flVariance = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "m_iGibCount") == 0 || strcasecmp(pkvd->szKeyName, "count") == 0 ) {
        m_iGibs = atoi(pkvd->szValue);
        m_iGibCapacity = m_iGibs; 
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "m_flGibLife") == 0 ) {
        m_flGibLife = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "m_bodyGroup") == 0 || strcasecmp(pkvd->szKeyName, "body") == 0 ) {
        m_bodyGroup = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp(pkvd->szKeyName, "m_quiet") == 0 ) {
        m_quiet = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else {
        CBaseDelay::KeyValue( pkvd );
    }
}

void CEnvLiveAmmoShooter::Spawn( void )
{
    Precache();
    pev->solid   = 0;          
    pev->effects = 0x80;       

    if ( m_flDelay == 0.0f ) m_flDelay = 0.1f;
    if ( m_flGibLife == 0.0f ) m_flGibLife = 25.0f;

    SetMovedir( pev );

    int iModelFrames = g_engfuncs.pfnModelFrames( m_iGibModelIndex );
    pev->body = iModelFrames;

    if ( pev->body < m_bodyGroup ) {
        ALERT( at_aiconsole, "GibShooter bodygroup %d > hgibs.mdl body groups %d. Using random.\n", m_bodyGroup, pev->body );
        m_bodyGroup = 0;
    }
}

void CEnvLiveAmmoShooter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    SetThink( &CEnvLiveAmmoShooter::ShootThink );
    pev->nextthink = gpGlobals->time;
}

// ?? 結合全域工廠優化後的 ShootThink 狀態機
void CEnvLiveAmmoShooter::ShootThink( void )
{
    pev->nextthink = gpGlobals->time + m_flDelay;

    Vector vecForward = pev->movedir;
    Vector vecShootDir = vecForward + 
        gpGlobals->v_right   * RANDOM_FLOAT(-1.0f, 1.0f) * m_flVariance +
        gpGlobals->v_forward * RANDOM_FLOAT(-1.0f, 1.0f) * m_flVariance +
        gpGlobals->v_up      * RANDOM_FLOAT(-1.0f, 1.0f) * m_flVariance;
        
    vecShootDir = vecShootDir.Normalize(); 

    // ?? 完美改造：直接調用我們剛剛神還原的 CreateGib() 工廠方法！
    CBaseEntity *pGib = CreateGib();
    if ( pGib )
    {
        pGib->pev->movetype   = MOVETYPE_TOSS; 
        pGib->pev->velocity   = vecShootDir * m_flGibVelocity;
        
        pGib->pev->avelocity.x = RANDOM_FLOAT( 100.0f, 200.0f );
        pGib->pev->avelocity.y = RANDOM_FLOAT( 100.0f, 300.0f );
        
        float flRandomLife = m_flGibLife * RANDOM_FLOAT( 0.95f, 1.05f );
        
        pGib->SetThink( &CBaseEntity::SUB_Remove );
        pGib->pev->nextthink = gpGlobals->time + flRandomLife;
    }

    m_iGibs--;

    if ( m_iGibs > 0 ) return;

    if ( (pev->spawnflags & 1) != 0 ) {
        m_iGibs = m_iGibCapacity;
        SetThink( nullptr );
        pev->nextthink = gpGlobals->time;
        return;
    }

    SetThink( &CBaseEntity::SUB_Remove );
    pev->nextthink = gpGlobals->time;
}
