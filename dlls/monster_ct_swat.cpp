#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "schedule.h"

struct ScheduleEntry {
    float activeFlag;      
    float percentChance;   
};

// =========================================================================
// 1. CMonsterCtSwat 類別宣告 (五國部隊王牌大滿貫完全體)
// =========================================================================

class CMonsterCtSwat : public CBaseMonster
{
public:
    void Spawn( void ) override;
    void Precache( void ) override; 
    int  Classify( void ) override { return CLASS_PLAYER_ALLY; } 

    void SpawnInit( void ); 
    void ActivateTableEntry( int entry );
    BOOL CanFollow( void ); 
    int  CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel ) override;

    // ?? 完美還原：原廠 DWARF 規格的戰術 AI 彈藥量檢查與換彈信號注入函式！
    void CheckAmmo( void ) override;

    BOOL CheckMeleeAttack1( float flDot, float flDist ) override;
    void FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

    CMonsterCtSwat();

private:
    int            m_ctBehavior;       
    float          m_flNextGrenadeCheck;
    float          m_flNextPainTime;
    int            m_iSentence;
    int            m_fFirstEncounter;
    float          m_healthMultiplier; 

    int            m_ctType;         
    string_t       m_language;       

    ScheduleEntry  scheduleTable;
    int            m_cantMove;       

    CBaseEntity*   m_pBeam;          
    CBaseEntity*   m_pLaserGlow;      

    int            m_voicePitch;
    int            m_iBrassShell;
    int            m_iShotgunShell;
    unsigned short m_usFireM60;
    unsigned short m_usFireUSP;
    unsigned short m_usFireAK47;
    unsigned short m_usFireM4A1;
    unsigned short m_usFireScout;
    unsigned short m_usFireMAC10;
    unsigned short m_usFireShotgun;
    unsigned short m_usFireMP5;
};

LINK_ENTITY_TO_CLASS( monster_ct_swat, CMonsterCtSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_sniperrifle, CMonsterCtSwat );
LINK_ENTITY_TO_CLASS( monster_ct_gign, CMonsterCtSwat ); 
LINK_ENTITY_TO_CLASS( monster_ct_gsg9, CMonsterCtSwat ); 
LINK_ENTITY_TO_CLASS( monster_ct_sas, CMonsterCtSwat ); 
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz, CMonsterCtSwat ); 

// =========================================================================
// 2. 核心 AI 狀態機：100% 對照原廠動態條件遮罩與 COND_NO_AMMO_LOADED 神還原 (CheckAmmo)
// =========================================================================

void CMonsterCtSwat::CheckAmmo( void )
{
    // ?? 完美還原 1：檢查當前手持彈匣內是否還有子彈，有則直接返回繼續開火搜敵
    if ( m_cAmmoLoaded > 0 )
    {
        return;
    }

    // ?? 完美還原 2：子彈打光，對照原廠 `*piVar1 | 1` 的遮罩操作
    // 在標準 SDK 中，`bits_COND_NO_AMMO_LOADED` 的巨集常數值固定就是 1 (0x00000001)。
    // 透過位元或（Bitwise OR）將此條件標記強行注入 AI 環境狀態，激活連鎖換彈匣事件！
    m_afConditions |= bits_COND_NO_AMMO_LOADED; 
}

// =========================================================================
// 3. 其餘成員方法實作
// =========================================================================

int CMonsterCtSwat::CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel )
{
    if ( this->m_ctBehavior == 4 ) {
        if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
        if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
    }
    return CBaseMonster::CanPlaySequence( fDisregardMonsterState, interruptLevel );
}

BOOL CMonsterCtSwat::CanFollow( void )
{
    if ( !IsAlive() ) return FALSE;
    if ( m_hTargetEnt != nullptr && m_hTargetEnt->IsPlayer() ) return FALSE;
    return TRUE; 
}

void CMonsterCtSwat::ActivateTableEntry( int entry )
{
    if ( entry == 0x12 || entry == 0x32 || entry == 0x40 || entry == 0x42 ) {
        if ( this->m_cantMove != 0 ) return;
    }
    if ( entry > 0x49 ) return;
    this->scheduleTable[entry].activeFlag = 1.0f;
}

CMonsterCtSwat::CMonsterCtSwat()
{
    m_ctBehavior = 0; m_flNextGrenadeCheck = 0.0f; m_flNextPainTime = 0.0f; m_iSentence = -1; m_fFirstEncounter = 1; m_healthMultiplier = 1.0f; 
    m_ctType = 0; m_language = 0; m_cantMove = 0;
    m_pBeam = nullptr; m_pLaserGlow = nullptr;
    m_voicePitch = 100; m_iBrassShell = m_iShotgunShell = 0;
    m_usFireM60 = m_usFireUSP = m_usFireAK47 = m_usFireM4A1 = m_usFireScout = m_usFireMAC10 = m_usFireShotgun = m_usFireMP5 = 0;
    memset(scheduleTable, 0, sizeof(scheduleTable));
    if ( pev ) pev->nextthink = 0.0f;
}

void CMonsterCtSwat::Precache( void )
{
    if ( pev->model != 0 && STRING(pev->model) != nullptr ) {
        PRECACHE_MODEL( (char*)STRING(pev->model) );
    } else {
        if ( strcasecmp( STRING(pev->classname), "monster_ct_gign" ) == 0 )         PRECACHE_MODEL( "models/gign.mdl" );
        else if ( strcasecmp( STRING(pev->classname), "monster_ct_gsg9" ) == 0 )    PRECACHE_MODEL( "models/gsg9.mdl" ); 
        else if ( strcasecmp( STRING(pev->classname), "monster_ct_sas" ) == 0 )     PRECACHE_MODEL( "models/sas.mdl" ); 
        else if ( strcasecmp( STRING(pev->classname), "monster_ct_spetsnaz" ) == 0 ) PRECACHE_MODEL( "models/spetsnaz.mdl" ); 
        else                                                                        PRECACHE_MODEL( "models/seal.mdl" );
    }
    PRECACHE_SOUND( "common/wpn_hudoff.wav" ); 
    PRECACHE_SOUND("weapons/reload_shotgun.wav"); PRECACHE_SOUND("weapons/pistol_reload.wav"); PRECACHE_SOUND("weapons/reload_mp5.wav");
    PRECACHE_SOUND("weapons/glauncher.wav"); PRECACHE_SOUND("weapons/sbarrel1.wav"); PRECACHE_SOUND("weapons/knife_slash1.wav");
    m_iBrassShell = PRECACHE_MODEL("models/shell.mdl"); m_iShotgunShell = PRECACHE_MODEL("models/shotgunshell.mdl");
    PRECACHE_MODEL("sprites/laserbeam.spr"); PRECACHE_MODEL("sprites/flare3.spr");
    m_usFireM60 = PRECACHE_EVENT(1, "events/m60.sc"); m_usFireUSP = PRECACHE_EVENT(1, "events/usp.sc"); m_usFireAK47 = PRECACHE_EVENT(1, "events/ak47.sc"); m_usFireM4A1 = PRECACHE_EVENT(1, "events/m4a1.sc");
    m_usFireScout = PRECACHE_EVENT(1, "events/scout.sc"); m_usFireMAC10 = PRECACHE_EVENT(1, "events/mac10.sc"); m_usFireShotgun = PRECACHE_EVENT(1, "events/xm1014.sc"); m_usFireMP5 = PRECACHE_EVENT(1, "events/mp5n.sc");
}

void CMonsterCtSwat::SpawnInit( void )
{
    bool bIsGIGN     = ( strcasecmp( STRING(pev->classname), "monster_ct_gign" ) == 0 );
    bool bIsGSG9     = ( strcasecmp( STRING(pev->classname), "monster_ct_gsg9" ) == 0 );
    bool bIsSpetsnaz = ( strcasecmp( STRING(pev->classname), "monster_ct_spetsnaz" ) == 0 );
    bool bIsSAS      = ( strcasecmp( STRING(pev->classname), "monster_ct_sas" ) == 0 );

    if ( pev->model == 0 ) {
        if ( bIsGIGN )         SET_MODEL( ENT(pev), "models/gign.mdl" );
        else if ( bIsGSG9 )     SET_MODEL( ENT(pev), "models/gsg9.mdl" );
        else if ( bIsSpetsnaz ) SET_MODEL( ENT(pev), "models/spetsnaz.mdl" );
        else if ( bIsSAS )     SET_MODEL( ENT(pev), "models/sas.mdl" );
        else                   SET_MODEL( ENT(pev), "models/seal.mdl" );
    } else {
        SET_MODEL( ENT(pev), STRING(pev->model) );
    }

    if ( bIsGIGN ) { this->m_ctType = 1; if ( this->m_language == 0 ) this->m_language = ALLOC_STRING("FR"); }
    else if ( bIsGSG9 ) { this->m_ctType = 2; if ( this->m_language == 0 ) this->m_language = ALLOC_STRING("GE"); }
    else if ( bIsSpetsnaz ) { this->m_ctType = 3; if ( this->m_language == 0 ) this->m_language = ALLOC_STRING("RU"); }
    else if ( bIsSAS ) { this->m_ctType = 4; if ( this->m_language == 0 ) this->m_language = ALLOC_STRING("BR"); }
    else { this->m_ctType = 0; if ( this->m_language == 0 ) this->m_language = ALLOC_STRING("AM"); }
}

void CMonsterCtSwat::Spawn( void )
{
    Precache();
    UTIL_SetSize( pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 72.0f) );
    pev->solid = SOLID_SLIDEBOX; pev->movetype = MOVETYPE_STEP; pev->effects = 0;
    pev->health = 100.0f * m_healthMultiplier; m_bloodColor = BLOOD_COLOR_RED;
    m_flFieldOfView = 0.1f; m_MonsterState = MONSTERSTATE_NONE; m_afCapability = 0x748;
    m_flNextGrenadeCheck = gpGlobals->time + 1.0f; m_flNextPainTime = gpGlobals->time; m_iSentence = -1; m_fFirstEncounter = 1;
    m_HackedGunPos = Vector( 0.0f, 0.0f, 55.0f );
    if ( pev->body > 0 ) SetBodygroup( 1, pev->body );
    SpawnInit(); MonsterInit(); SetActivity( ACT_IDLE );
    SetUse( &CMonsterCtSwat::FollowerUse );
}

BOOL CMonsterCtSwat::CheckMeleeAttack1( float flDot, float flDist )
{
    if ( m_hEnemy != nullptr ) {
        CBaseMonster *pEnemyMonster = m_hEnemy->MyMonsterPointer();
        if ( pEnemyMonster == nullptr ) return FALSE;
    }
    if ( flDist <= 64.0f && flDot >= 0.7f ) return TRUE;
    return FALSE;
}

void CMonsterCtSwat::FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    if ( pActivator && pActivator->IsPlayer() ) {
        if ( !CanFollow() ) return;
        ALERT(at_console, "CZDS_AI: Counter-Terrorist 多國部隊隊員與玩家完成戰術跟隨互動！\n");
        EMIT_SOUND(ENT(pev), CHAN_VOICE, "common/wpn_hudoff.wav", 1.0, ATTN_NORM);
    }
}
