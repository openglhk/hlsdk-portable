#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "cterrorist.h"

TYPEDESCRIPTION CCounterTerrorist::m_SaveData[] =
{
    DEFINE_FIELD( CCounterTerrorist, m_ctBehavior, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_flNextGrenadeCheck, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_flNextPainTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_flStopTalkTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_flStareTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_hTalkTarget, FIELD_EHANDLE ),
    DEFINE_FIELD( CCounterTerrorist, m_behaviorType, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_canWander, FIELD_BOOLEAN ),
    DEFINE_FIELD( CCounterTerrorist, m_wanderOrigin, FIELD_POSITION_VECTOR ),
    DEFINE_FIELD( CCounterTerrorist, m_wanderTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_fPreferedRange, FIELD_FLOAT ),
    DEFINE_FIELD( CCounterTerrorist, m_weaponAccuracy, FIELD_VECTOR ),
    DEFINE_FIELD( CCounterTerrorist, m_iShellId, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_iAssaultRifleType, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_iShellModelIndex, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_iBrassShell, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_iShotgunShell, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_iSentence, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_voicePitch, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_pBeam, FIELD_CLASSPTR ),
    DEFINE_FIELD( CCounterTerrorist, m_pLaserGlow, FIELD_CLASSPTR ),
    DEFINE_FIELD( CCounterTerrorist, m_laserBrightness, FIELD_FLOAT ),
    DEFINE_FIELD( CCounterTerrorist, m_glowBrightness, FIELD_FLOAT ),
    DEFINE_FIELD( CCounterTerrorist, m_idleAnim, FIELD_STRING ),
    DEFINE_FIELD( CCounterTerrorist, m_deathAnim, FIELD_STRING ),
    DEFINE_FIELD( CCounterTerrorist, m_bStanding, FIELD_BOOLEAN ),
    DEFINE_FIELD( CCounterTerrorist, m_scheduleList, FIELD_POINTER ),
    DEFINE_FIELD( CCounterTerrorist, scheduleTable, FIELD_POINTER )
};

int  g_fCTQuestion = 0;

CCounterTerrorist::CCounterTerrorist( void )
{
    m_ctBehavior = -1;
    m_flNextGrenadeCheck = 0.0f;
    m_flNextPainTime = 0.0f;
    m_flStopTalkTime = 0.0f;
    m_flStareTime = 0.0f;
    m_hTalkTarget = nullptr;
    m_behaviorType = 0;
    m_canWander = FALSE;
    m_wanderOrigin = g_vecZero;
    m_wanderTime = 0.0f;
    m_fPreferedRange = 256.0f;
    m_weaponAccuracy = g_vecZero;
    m_iShellId = 0;
    m_iAssaultRifleType = 0;
    m_iShellModelIndex = 0;
    m_iBrassShell = 0;
    m_iShotgunShell = 0;
    
    m_iSentence = -1;
    m_voicePitch = RANDOM_LONG( 95, 105 );

    m_pBeam = nullptr;
    m_pLaserGlow = nullptr;
    m_laserBrightness = 0.0f;
    m_glowBrightness = 0.0f;
    
    m_idleAnim = iStringNull;
    m_deathAnim = iStringNull;
    m_bStanding = TRUE;

    InitScheduleTable();
}

void CCounterTerrorist::Precache( void )
{
    PRECACHE_SOUND( "weapons/reload_shotgun.wav" );
    PRECACHE_SOUND( "weapons/pistol_reload.wav" );
    PRECACHE_SOUND( "weapons/reload_mp5.wav" );
    PRECACHE_SOUND( "weapons/glauncher.wav" );
    PRECACHE_SOUND( "weapons/sbarrel1.wav" );
    PRECACHE_SOUND( "weapons/knife_slash1.wav" );
    m_voicePitch = 100;
    m_iBrassShell   = PRECACHE_MODEL( "models/shell.mdl" );         // 黃銅小彈殼模型
    m_iShotgunShell = PRECACHE_MODEL( "models/shotgunshell.mdl" );  // 紅色塑料散彈大殼模型
    PRECACHE_MODEL( "sprites/laserbeam.spr" );                     // 狙擊雷射線射線
    PRECACHE_MODEL( "sprites/flare3.spr" );                        // 槍口光暈精靈
    m_usFireM60     = PRECACHE_EVENT( 1, "events/m60.sc" );
    m_usFireUSP     = PRECACHE_EVENT( 1, "events/usp.sc" );
    m_usFireAK47    = PRECACHE_EVENT( 1, "events/ak47.sc" );
    m_usFireM4A1    = PRECACHE_EVENT( 1, "events/m4a1.sc" );
    m_usFireScout   = PRECACHE_EVENT( 1, "events/scout.sc" );
    m_usFireMAC10   = PRECACHE_EVENT( 1, "events/mac10.sc" );
    m_usFireShotgun = PRECACHE_EVENT( 1, "events/xm1014.sc" );
    m_usFireMP5     = PRECACHE_EVENT( 1, "events/mp5n.sc" );
}

void CCounterTerrorist::SpawnInit( void )
{
    if ( pev->model != 0 )
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
        return;
    }
    SET_MODEL( edict(), "models/hgrunt.mdl" );
}

void CCounterTerrorist::Spawn( void )
{
    CSquadMonster::Spawn();
    InitSquadData();
    m_ctBehavior = GetBehaviorType();
    Vector vecMin = Vector( -16.0f, -16.0f, 0.0f );
    Vector vecMax = Vector( 16.0f,  16.0f,  72.0f );
    UTIL_SetSize( pev, vecMin, vecMax );
    pev->solid    = SOLID_SLIDEBOX; 
    pev->movetype = MOVETYPE_STEP;
    pev->health   = gSkillData.hgruntHealth * m_healthMultiplier;
    m_bloodColor  = BLOOD_COLOR_RED; // 247 = 0xF7
    pev->effects  = 0;
    m_flFieldOfView = 0.1f;
    m_MonsterState  = MONSTERSTATE_NONE;
    m_afCapability  = bits_CAP_MOVE_SHOOT | bits_CAP_TAKE_COVER | bits_CAP_HEAR | bits_CAP_RANGE_ATTACK1; 
    m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
    m_flNextPainTime     = gpGlobals->time;
    m_iSentence          = -1;
    m_fEnemyEluded       = FALSE;
    m_fFirstEncounter    = TRUE;
    m_HackedGunPos       = Vector( 0.0f, 0.0f, 55.0f );
    SetupWeapons();

    // =========================================================================
    // ?? 16 容量計畫表機率輪盤全線格式化歸位 (do-while 內存清空鏡像)
    // =========================================================================
    for ( int i = 0; i < 16; i++ )
    {
        scheduleTable[i].activeFlag    = 0.0f;
        scheduleTable[i].percentChance = 1.0f;
    }

    // 依據當前特警行為代號，重寫計畫表參數性格
    SetupBehaviors( m_ctBehavior );
    CTalkMonster::g_talkWaitTime = 0.0f; // 語音信道開局暢通

    // 呼叫虛擬表 0x120 偏移方法 (拉起動畫序列配准)
    // 對應二進位 _vptr + 0x120
    ResetSequenceInfo();

    // =========================================================================
    // ?? 狙擊手與火箭筒兵特權：4096碼超視距廣角感知視線、與紅外線雷射構造
    // =========================================================================
    if ( m_ctBehavior == 4 || m_ctBehavior == 5 ) // 4: Sniper, 5: LAW
    {
        if ( m_customRange == 0 )
        {
            m_flDistTooFar = 2048.0f; // 鎖定遠程火力範圍
        }
        m_flDistLook    = 4096.0f; // ?? 4096碼 超視距老鷹眼神
        m_flFieldOfView = -0.7f;   // ?? 270度 戰術超廣角防繞後防禦
        
        SetupLaserGlow(); // 動態點亮特警紅外線雷射軌跡
    }

    // 霸體過濾
    if ( m_invulnerable != 0 )
    {
        pev->takedamage = DAMAGE_NO; // 0.0f
    }

    // 模型 Bodygroup 頭飾自適應加載
    if ( m_head > 0 )
    {
        CBaseAnimating::SetBodygroup( 1, m_head );
    }
    SetUse( &CCounterTerrorist::FollowerUse );
}

void CCounterTerrorist::ActivateTableEntry( int entry )
{
    switch ( entry )
    {
        case 18:
        case 50:
        case 64:
        case 66:
            if ( m_cantMove != 0 )
            {
                return;
            }
            break;

        default:
            break;
    }

    if ( (unsigned int)entry > 73 )
    {
        return;
    }

    scheduleTable[entry].activeFlag = 1.0f;
}

BOOL CCounterTerrorist::CanFollow( void )
{
    if ( !IsAlive() )
    {
        return FALSE;
    }

    if ( m_hTargetEnt != nullptr )
    {
        CBaseEntity *pTarget = m_hTargetEnt;
        if ( pTarget != nullptr )
        {
            if ( pTarget->IsPlayer() )
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

int CCounterTerrorist::CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel )
{
    if ( m_ctBehavior == 4 )
    {
        if ( m_pBeam != nullptr )
        {
            UTIL_Remove( m_pBeam );
            m_pBeam = nullptr;
        }

        if ( m_pLaserGlow != nullptr )
        {
            UTIL_Remove( m_pLaserGlow );
            m_pLaserGlow = nullptr;
        }
    }

    return CBaseMonster::CanPlaySequence( fDisregardMonsterState, interruptLevel );
}

void CCounterTerrorist::CheckAmmo( void )
{
    if ( m_cAmmoLoaded > 0 )
    {
        return;
    }

    m_afConditions |= COND_NO_AMMO_LOADED;
}

BOOL CCounterTerrorist::CheckMeleeAttack1( float flDot, float flDist )
{
    if ( m_hEnemy != nullptr )
    {
        CBaseEntity *pEnemy = m_hEnemy;
        if ( pEnemy != nullptr )
        {
            if ( pEnemy->MyMonsterPointer() == nullptr )
            {
                return FALSE;
            }
        }
    }

    if ( flDist <= 64.0f && flDot >= 0.7f )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CCounterTerrorist::CheckRangeAttack1( float flDot, float flDist )
{
    if ( ( m_afConditions & COND_SEE_ENEMY ) == 0 || flDist > 2048.0f || flDot < 0.5f )
    {
        return FALSE;
    }

    if ( !CSquadMonster::NoFriendlyFire() )
    {
        return FALSE;
    }

    Vector vecSrc = GetGunPosition();
    
    CBaseEntity *pEnemy = m_hEnemy;
    if ( pEnemy == nullptr )
    {
        return FALSE;
    }

    Vector vecEnd = pEnemy->BodyTarget( vecSrc );
    
    TraceResult tr;
    UTIL_TraceLine( vecSrc, vecEnd, ignore_monsters, ignore_glass, edict(), &tr );

    if ( tr.flFraction == 1.0f )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CCounterTerrorist::CheckRangeAttack2( float flDot, float flDist )
{
    if ( ( pev->weapons & ( m_iWeaponBit_HEGrenade | m_iWeaponBit_Flashbang ) ) == 0 )
    {
        return FALSE;
    }

    if ( m_flGroundSpeed != 0.0f )
    {
        m_fThrowGrenade = FALSE;
        return FALSE;
    }

    if ( gpGlobals->time < m_flNextGrenadeCheck )
    {
        return m_fThrowGrenade;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    if ( pEnemy == nullptr )
    {
        m_fThrowGrenade = FALSE;
        return FALSE;
    }

    if ( !( pEnemy->pev->flags & FL_ONGROUND ) )
    {
        if ( pEnemy->pev->waterlevel == 0 && pev->absmax.z < m_vecEnemyLKP.z )
        {
            m_fThrowGrenade = FALSE;
            return FALSE;
        }
    }

    if ( ( pev->weapons & m_iWeaponBit_Flashbang ) == 0 )
    {
        Vector vecEnemyOrigin = pEnemy->pev->origin;
        if ( ( m_afConditions & COND_SEE_ENEMY ) != 0 )
        {
            Vector vecDiff = vecEnemyOrigin - pev->origin;
            if ( vecDiff.Length() < 256.0f )
            {
                m_fThrowGrenade = FALSE;
                m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
                return FALSE;
            }
        }
    }

    if ( m_hSquadLeader != nullptr )
    {
        if ( SquadMemberInRange( m_vecEnemyLKP, 256.0f ) )
        {
            m_fThrowGrenade = FALSE;
            m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
            return FALSE;
        }
    }

    Vector vecSrc = GetGunPosition();
    Vector vecToss;

    if ( ( pev->weapons & m_iWeaponBit_Flashbang ) == 0 )
    {
        float flGrenadeSpeed = gSkillData.hgruntGrenadeSpeed;
        if ( VecCheckThrow( pev, vecSrc, m_vecEnemyLKP, flGrenadeSpeed, 0.5f, &vecToss ) )
        {
            m_fThrowGrenade = TRUE;
            m_vecTossVelocity = vecToss;
            m_flNextGrenadeCheck = gpGlobals->time + 0.3f;
            return TRUE;
        }
    }
    else
    {
        if ( VecCheckToss( pev, vecSrc, m_vecEnemyLKP, 0.5f, &vecToss ) )
        {
            m_fThrowGrenade = TRUE;
            m_vecTossVelocity = vecToss;
            m_flNextGrenadeCheck = gpGlobals->time + 0.3f;
            return TRUE;
        }
    }

    m_fThrowGrenade = FALSE;
    m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
    return FALSE;
}

int CCounterTerrorist::Classify( void )
{
    return m_classtype;
}

void CCounterTerrorist::ClearScheduleTableFlags( void )
{
    for ( int i = 0; i < 74; i++ )
    {
        scheduleTable[i].activeFlag = 0.0f;
    }
}

void CCounterTerrorist::DeathSound( void )
{
    if ( !FOkToSpeak() )
    {
        EMIT_SOUND_DYN( edict(), CHAN_VOICE, "common/null.wav", 0.5f, ATTN_NORM, 0, m_voicePitch );
    }

    Speak( "CT_DEATH", 0.8f, 0.8f, 0, m_voicePitch );

    SetUse( nullptr );
}

BOOL CCounterTerrorist::FCanCheckAttacks( void )
{
    if ( ( m_afConditions & COND_ENEMY_OCCLUDED ) != 0 )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CCounterTerrorist::FOkToSpeak( void )
{
    if ( gpGlobals->time <= CTalkMonster::g_talkWaitTime )
    {
        return FALSE;
    }

    edict_t *pPlayerEdict = pfnPEntityOfEntIndex( 1 );
    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;
        if ( gpGlobals->time > 2.0f && gpGlobals->time < pPlayer->m_flNextRadioTime )
        {
            return FALSE;
        }
    }

    if ( ( pev->spawnflags & SF_MONSTER_GAG ) && m_MonsterState != MONSTERSTATE_COMBAT )
    {
        return FALSE;
    }

    return TRUE;
}

void CCounterTerrorist::FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    if ( m_MonsterState == MONSTERSTATE_SCRIPT )
    {
        return;
    }

    edict_t *pPlayerEdict = pfnPEntityOfEntIndex( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        return;
    }

    CBaseEntity *pPlayer = (CBaseEntity *)pPlayerEdict->pvPrivateData;

    if ( m_useTarget != 0 )
    {
        const char *pTargetName = STRING( m_useTarget );
        if ( pTargetName != nullptr && strcmp( pTargetName, "none" ) != 0 )
        {
            FireTargets( pTargetName, pActivator, this, useType, value );
            return;
        }
    }

    if ( pev->spawnflags & SF_MONSTER_DONT_FOLLOW )
    {
        return;
    }

    if ( !IsAlive() )
    {
        StopFollowing( TRUE );
        Speak( "CT_WAIT", 0.8f, 0.8f, 0, m_voicePitch );
        return;
    }

    if ( m_hTargetEnt != nullptr )
    {
        CBaseEntity *pCurrentTarget = m_hTargetEnt;
        if ( pCurrentTarget != nullptr && pCurrentTarget->IsPlayer() )
        {
            StopFollowing( TRUE );
            Speak( "CT_WAIT", 0.8f, 0.8f, 0, m_voicePitch );
            return;
        }
    }

    if ( m_afMemory & bits_MEMORY_PROVOKED )
    {
        ALERT( at_console, "I'm not following you, you evil person!\n" );
        return;
    }

    if ( m_hEnemy != nullptr )
    {
        m_IdealMonsterState = MONSTERSTATE_ALERT;
    }

    m_hTargetEnt = pPlayer;
    m_afConditions &= ~COND_PROVOKED;
    
    ClearSchedule();
    Speak( "CT_OK", 0.8f, 0.8f, 0, m_voicePitch );
}

int CCounterTerrorist::GetBehaviorType( void )
{
    const char *pClassName = STRING( pev->classname );
    if ( pClassName == nullptr )
    {
        return 8;
    }

    if ( strstr( pClassName, "pistol" ) != nullptr ) return 0;
    if ( strstr( pClassName, "shotgun" ) != nullptr ) return 1;
    if ( strstr( pClassName, "assaultrifle" ) != nullptr ) return 2;
    if ( strstr( pClassName, "machinegun" ) != nullptr ) return 3;
    if ( strstr( pClassName, "sniperrifle" ) != nullptr ) return 4;
    if ( strstr( pClassName, "law" ) != nullptr ) return 5;
    if ( strstr( pClassName, "smg" ) != nullptr ) return 6;
    if ( strstr( pClassName, "grenader" ) != nullptr ) return 7;
    if ( strstr( pClassName, "mp5" ) != nullptr ) return 8;

    if ( m_behaviorType != 1 )
    {
        return 8;
    }

    return 4;
}

int CCounterTerrorist::GetFlyingSchedule( void )
{
    if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
    {
        if ( pev->flags & FL_ONGROUND )
        {
            pev->takedamage = DAMAGE_AIM;
            pev->movetype = MOVETYPE_WALK;
            return 56;
        }

        pev->takedamage = DAMAGE_NO;
        
        if ( m_MonsterState == MONSTERSTATE_COMBAT )
        {
            return 55;
        }
        else
        {
            return 54;
        }
    }

    return -1;
}

int CCounterTerrorist::GetForcedTargetSchedule( void )
{
    if ( m_forcedTarget == 0 )
    {
        return -1;
    }

    CheckAmmo();

    if ( m_afConditions & COND_NO_AMMO_LOADED )
    {
        return 30;
    }

    int iRandom = RANDOM_LONG( 0, 100 );
    if ( iRandom <= 30 )
    {
        return 57;
    }

    return 42;
}

Vector CCounterTerrorist::GetGunPosition( void )
{
    float flHeightOffset;

    if ( !m_bStanding )
    {
        flHeightOffset = 36.0f;
    }
    else
    {
        flHeightOffset = 60.0f;
    }

    return Vector( pev->origin.x, pev->origin.y, pev->origin.z + flHeightOffset );
}

int CCounterTerrorist::GetHearingSchedule( void )
{
    if ( ( m_afConditions & COND_HEAR_SOUND ) != 0 )
    {
        CSound *pSound = PBestSound();
        if ( pSound != nullptr )
        {
            if ( pSound->m_iType & BITS_SOUND_DANGER )
            {
                return 19;
            }

            if ( !( m_afConditions & COND_SEE_ENEMY ) && ( pSound->m_iType & ( BITS_SOUND_COMBAT | BITS_SOUND_WORLD ) ) )
            {
                MakeIdealYaw( pSound->m_vecOrigin );
            }
        }
    }

    return -1;
}

Schedule_t * CCounterTerrorist::GetSchedule( void )
{
    m_iSentence = -1;

    if ( gpGlobals->time < m_flStopTalkTime )
    {
        return CSquadMonster::GetScheduleOfType( 1 );
    }

    if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
    {
        if ( !( pev->flags & FL_ONGROUND ) )
        {
            pev->takedamage = DAMAGE_NO;
            return CSquadMonster::GetScheduleOfType( m_MonsterState == MONSTERSTATE_COMBAT ? 55 : 54 );
        }
        else
        {
            pev->takedamage = DAMAGE_AIM;
            pev->movetype = MOVETYPE_WALK;
            return CSquadMonster::GetScheduleOfType( 56 );
        }
    }

    if ( m_ctBehavior == 4 )
    {
        if ( m_pBeam != nullptr )
        {
            UTIL_Remove( m_pBeam );
            m_pBeam = nullptr;
        }
        if ( m_pLaserGlow != nullptr )
        {
            UTIL_Remove( m_pLaserGlow );
            m_pLaserGlow = nullptr;
        }
    }

    float flDistanceToEnemy = 0.0f;
    if ( m_hEnemy != nullptr )
    {
        Vector vecEnemyOrigin = m_hEnemy->pev->origin;
        Vector vecDiff = pev->origin - vecEnemyOrigin;
        flDistanceToEnemy = vecDiff.Length();
        m_IdealMonsterState = MONSTERSTATE_COMBAT;
    }

    ClearScheduleTableFlags();

    int iForcedSchedule = GetForcedTargetSchedule();
    if ( iForcedSchedule != -1 && iForcedSchedule < 74 )
    {
        if ( m_cantMove == 0 || ( iForcedSchedule != 18 && iForcedSchedule != 50 && iForcedSchedule != 64 && iForcedSchedule != 66 ) )
        {
            scheduleTable[iForcedSchedule].activeFlag = 1.0f;
        }
    }

    int iAttackSchedule = CheckAttacks();
    if ( iAttackSchedule != -1 && iAttackSchedule < 74 )
    {
        if ( m_cantMove == 0 || ( iAttackSchedule != 18 && iAttackSchedule != 50 && iAttackSchedule != 64 && iAttackSchedule != 66 ) )
        {
            scheduleTable[iAttackSchedule].activeFlag = 1.0f;
        }
    }

    if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
    {
        int iFlySched = ( pev->flags & FL_ONGROUND ) ? 56 : ( m_MonsterState == MONSTERSTATE_COMBAT ? 55 : 54 );
        scheduleTable[iFlySched].activeFlag = 1.0f;
    }

    int iHearingSchedule = GetHearingSchedule();
    if ( iHearingSchedule != -1 && iHearingSchedule < 74 )
    {
        if ( m_cantMove == 0 || ( iHearingSchedule != 18 && iHearingSchedule != 50 && iHearingSchedule != 64 && iHearingSchedule != 66 ) )
        {
            scheduleTable[iHearingSchedule].activeFlag = 1.0f;
        }
    }

    CNode *pCoverNode = GetCoverNode();
    if ( pCoverNode != nullptr )
    {
        float flDiffY = pev->origin.y - pCoverNode->m_vecOrigin.y;
        float flDiffX = pev->origin.x - pCoverNode->m_vecOrigin.x;
        float flDistToNode = sqrtf( flDiffY * flDiffY + flDiffX * flDiffX );

        if ( ( flDistToNode * 0.75f < flDistanceToEnemy ) && m_cantMove == 0 )
        {
            scheduleTable[64].activeFlag = 1.0f;
        }
    }

    int iFinalSchedule = GetScheduleFromTable();
    
    if ( FOkToSpeak() )
    {
        switch ( iFinalSchedule )
        {
            case 18:
                if ( RANDOM_LONG( 0, 1 ) != 0 )
                {
                    Speak( "CT_COVER", 0.8f, 0.8f, 0, m_voicePitch );
                    CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
                    m_iSentence = -1;
                }
                break;

            case 19:
                Speak( "CT_GREN", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
                break;

            case 24:
                if ( RANDOM_LONG( 0, 99 ) > 50 )
                {
                    Speak( "CT_ATTACK", 0.8f, 0.8f, 0, m_voicePitch );
                    m_iSentence = -1;
                    CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
                }
                break;

            case 25:
                Speak( "CT_THROW", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
                break;

            case 28:
                if ( RANDOM_LONG( 0, 99 ) > 50 )
                {
                    Speak( "CT_TAUNT", 0.8f, 0.8f, 0, m_voicePitch );
                    CTalkMonster::g_talkWaitTime = gpGlobals->time + 7.0f;
                    m_iSentence = -1;
                }
                break;

            case 50:
                if ( RANDOM_LONG( 0, 1 ) != 0 )
                {
                    Speak( "CT_MOVING", 0.8f, 0.8f, 0, m_voicePitch );
                    CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
                    m_iSentence = -1;
                }
                break;

            case 66:
                if ( RANDOM_LONG( 0, 1 ) != 0 )
                {
                    Speak( "CT_CHARGE", 0.8f, 0.8f, 0, m_voicePitch );
                    CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
                    m_iSentence = -1;
                }
                break;

            default:
                break;
        }

        if ( m_hEnemy != nullptr )
        {
            CBaseEntity *pEnemy = m_hEnemy;
            if ( pEnemy->IsPlayer() && RANDOM_LONG( 0, 99 ) > 75 )
            {
                Speak( "CT_MAD", 0.8f, 0.8f, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 6.0f;
                m_iSentence = -1;
            }
        }
    }

    if ( iFinalSchedule != -1 )
    {
        Schedule_t *pSched = GetScheduleOfType( iFinalSchedule );
        ALERT( at_aiconsole, "%s Playing Schedule: %s\n", STRING( pev->classname ), pSched->pName );
        return pSched;
    }

    Schedule_t *pFollowSched = HandleFollowing();
    if ( pFollowSched != nullptr )
    {
        return pFollowSched;
    }

    if ( m_pCine != nullptr && m_MonsterState == MONSTERSTATE_IDLE )
    {
        m_IdealMonsterState = MONSTERSTATE_SCRIPT;
        return CSquadMonster::GetScheduleOfType( 40 );
    }

    if ( m_hEnemy == nullptr && m_wanderTime < gpGlobals->time && m_canWander != 0 && m_cantMove == 0 )
    {
        float flRadius = RANDOM_FLOAT( 64.0f, 256.0f );
        m_vecMoveGoal.x += RANDOM_LONG( (int)-flRadius, (int)flRadius );
        m_vecMoveGoal.y += RANDOM_LONG( (int)-flRadius, (int)flRadius );
    }

    return CSquadMonster::GetSchedule();
}

int CCounterTerrorist::GetScheduleFromTable( void )
{
    float flTotalChance = 0.0f;

    for ( int i = 0; i < 73; i++ )
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            flTotalChance += scheduleTable[i].percentChance;
        }
    }

    if ( flTotalChance <= 0.0f )
    {
        goto ERROR_FALLBACK;
    }

    for ( int i = 0; i < 73; i++ )
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            scheduleTable[i].normalizedChance = scheduleTable[i].percentChance / flTotalChance;
        }
    }

    float flRoll = RANDOM_FLOAT( 0.0f, 1.0f );
    float flCumulativeChance = 0.0f;

    for ( int i = 0; i < 73; i++ )
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            flCumulativeChance += scheduleTable[i].normalizedChance;
            if ( flRoll < flCumulativeChance )
            {
                return i;
            }
        }
    }

ERROR_FALLBACK:
    if ( m_MonsterState == MONSTERSTATE_IDLE )
    {
        return -1;
    }

    if ( pev->targetname == 0 )
    {
        ALERT( at_aiconsole, "%s -- (no targetname) at (%d, %d, %d), Failed to find valid schedule in the table!\n", STRING( pev->classname ), (int)pev->origin.x, (int)pev->origin.y, (int)pev->origin.z );
        return -1;
    }

    ALERT( at_aiconsole, "%s -- %s at (%d, %d, %d), Failed to find valid schedule in the table!\n", STRING( pev->classname ), STRING( pev->targetname ), (int)pev->origin.x, (int)pev->origin.y, (int)pev->origin.z );
    return -1;
}

Schedule_t * CCounterTerrorist::GetScheduleOfType( int Type )
{
    if ( Type == 22 )
    {
        taskFailCount = 0.0f;
        return CSquadMonster::GetScheduleOfType( Type );
    }

    switch ( Type )
    {
        case 1:
            if ( gpGlobals->time < m_flStopTalkTime )
            {
                return slCTIdleEyecontact;
            }
            {
                Schedule_t *pIdleSched = HandleIdleHeadWatch();
                if ( pIdleSched != nullptr )
                {
                    return pIdleSched;
                }
            }
            return CSquadMonster::GetScheduleOfType( 1 );

        case 10:
            return slCTCombatFace;

        case 14:
            if ( m_hSquadLeader == nullptr || (CBaseEntity *)m_hSquadLeader == this )
            {
                return slCTVictoryDance;
            }
            return slCTFail;

        case 15:
            return slCTFaceTarget;

        case 16:
            return slCTFollow;

        case 18:
        case 67:
        case 68:
            return slCTTakeCover;

        case 19:
            return slCTTakeCoverFromBestSound;

        case 24:
            taskFailCount = 0.0f;
            if ( m_ctBehavior == 0 )
            {
                return slCTRangeAttack1C;
            }
            else
            {
                if ( RANDOM_LONG( 0, 99 ) < 50 )
                {
                    if ( m_ctBehavior == 3 )
                    {
                        m_bStanding = 1;
                        return slCTRangeAttackMG2;
                    }
                    return slCTRangeAttack1C;
                }
                else if ( m_ctBehavior != 3 )
                {
                    if ( RANDOM_LONG( 0, 9 ) < 5 )
                    {
                        m_bStanding = RANDOM_LONG( 0, 1 );
                    }
                    if ( m_bStanding == 0 )
                    {
                        return slCTRangeAttack1A;
                    }
                    return slCTRangeAttack1B;
                }
                m_bStanding = 1;
                return slCTRangeAttackMG2;
            }
            break;

        case 25:
            return slCTRangeAttack2;

        case 41:
            if ( m_hEnemy != nullptr )
            {
                return slCTCombatFail;
            }
            return slCTFail;

        case 46:
            return slCTWander;

        case 49:
            if ( m_hEnemy != nullptr )
            {
                CBaseEntity *pEnemy = m_hEnemy;
                if ( pEnemy->IsPlayer() && m_fFirstEncounter != 0 )
                {
                    m_fFirstEncounter = 0;
                    return slCTSignalSuppress;
                }
            }
            return slCTSuppress;

        case 50:
            if ( m_ctBehavior == 4 )
            {
                if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
                if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
            }
            return slCTEstablishLineOfFire;

        case 51:
            return slCTHideReload;

        case 52:
            return slCTSweep;

        case 53:
            return slCTFoundEnemy;

        case 54:
            if ( pev->velocity.z > -128.0f )
            {
                pev->velocity.z -= 32.0f;
            }
            return slCTRepel;

        case 55:
            if ( pev->velocity.z > -128.0f )
            {
                pev->velocity.z -= 32.0f;
            }
            return slCTRepelAttack;

        case 56:
            return slCTRepelLand;

        case 57:
            return slCTWaitInCover;

        case 58:
        case 59:
            if ( Type == 59 )
            {
                m_bStanding = 1;
                taskFailCount += 1.0f;
            }
            if ( m_ctBehavior == 4 )
            {
                if ( ( m_afConditions & COND_SEE_ENEMY ) && scheduleTable[62].percentChance > 0.0f )
                {
                    return slCTSniperAttack;
                }
            }
            else
            {
                if ( ( m_afConditions & COND_HEAR_SOUND ) && scheduleTable[22].percentChance > 0.0f ) return slCTEstablishLineOfFire;
                if ( ( m_afConditions & COND_ENEMY_OCCLUDED ) && scheduleTable[25].percentChance > 0.0f ) return slCTRangeAttack2;
                if ( ( m_afConditions & COND_SEE_ENEMY ) && scheduleTable[24].percentChance > 0.0f ) return slCTRangeAttack1C;
            }
            return slCTFail;

        case 60:
            return slCTInSmoke;

        case 61:
            return slCTFlashed;

        case 62:
            taskFailCount = 0.0f;
            m_bStanding = 1;
            return slCTSniperAttack;

        case 63:
            if ( m_hEnemy != nullptr )
            {
                if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
                if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
            }
            return slCTSniperTakeShot;
    }

    return CSquadMonster::GetScheduleOfType( Type );
}

int CCounterTerrorist::GetWeaponEffectSchedule( void )
{
    if ( m_iWeaponEffectType == 7 )
    {
        if ( m_bWeaponEffectActive == 0 )
        {
            return -1;
        }

        m_iWeaponEffectType = 0;
        m_bWeaponEffectActive = 0;

        if ( FOkToSpeak() )
        {
            Speak( "CT_INSMOKE", 0.8f, 0.8f, 0, m_voicePitch );
            CTalkMonster::g_talkWaitTime = gpGlobals->time + 6.0f;
            m_iSentence = -1;
        }

        int iRoll = RANDOM_LONG( 0, 99 );
        if ( iRoll < 60 )
        {
            return 60;
        }
        if ( iRoll < 90 )
        {
            return 17;
        }
    }

    if ( m_iWeaponEffectType == 4 && m_bWeaponEffectActive != 0 )
    {
        m_iWeaponEffectType = 0;
        m_bWeaponEffectActive = 0;

        if ( !FOkToSpeak() )
        {
            return 61;
        }

        Speak( "CT_FLASHED", 0.8f, 0.8f, 0, m_voicePitch );
        m_iSentence = -1;
        CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
        return 61;
    }

    return -1;
}

void CCounterTerrorist::GibMonster( void )
{
    int iBodyGroup = GetBodygroup( 2 );
    if ( iBodyGroup != 2 )
    {
        Vector vecGunPos, vecGunAngles;
        GetAttachment( 0, vecGunPos, vecGunAngles );

        if ( ( pev->weapons & m_iWeaponBit_Grenader ) != 0 )
        {
            CBaseEntity *pAmmoBox = DropItem( "ammo_ARgrenades", vecGunPos, vecGunAngles );
            if ( pAmmoBox != nullptr )
            {
                entvars_t *pAmmoPev = pAmmoBox->pev;
                pAmmoPev->velocity.z = RANDOM_FLOAT( 200.0f, 300.0f );
                pAmmoPev->velocity.y = RANDOM_FLOAT( -100.0f, 100.0f );
                pAmmoPev->velocity.x = RANDOM_FLOAT( -100.0f, 100.0f );

                pAmmoPev->avelocity.x = 0.0f;
                pAmmoPev->avelocity.z = 0.0f;
                pAmmoPev->avelocity.y = RANDOM_FLOAT( 200.0f, 400.0f );
            }
        }
    }

    CBaseMonster::GibMonster();
}

void CCounterTerrorist::HandleAnimEvent( MonsterEvent_t *pEvent )
{
    switch ( pEvent->event )
    {
        case 2:
        {
            if ( m_ctBehavior < 9 )
            {
                unsigned int uBit = 1 << ( m_ctBehavior & 0x1F );
                const char *pReloadSound;

                if ( ( uBit & 0x114 ) == 0 )
                {
                    if ( ( uBit & 0x41 ) == 0 )
                    {
                        if ( ( uBit & 2 ) == 0 )
                        {
                            goto UPDATE_CLIP;
                        }
                        pReloadSound = "weapons/reload_shotgun.wav";
                    }
                    else
                    {
                        pReloadSound = "weapons/pistol_reload.wav";
                    }
                }
                else
                {
                    pReloadSound = "weapons/reload_mp5.wav";
                }

                EMIT_SOUND_DYN( edict(), CHAN_ITEM, pReloadSound, 0.75f, ATTN_NORM, 0, 100 );
            }

        UPDATE_CLIP:
            m_afConditions &= ~COND_NO_AMMO_LOADED;
            m_cAmmoLoaded = m_cClipSize;
            break;
        }

        case 3:
        {
            CBaseEntity *pHurt = Kick();
            if ( pHurt != nullptr )
            {
                EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/knife_slash1.wav", 1.0f, ATTN_NORM, 0, 100 );
                UTIL_MakeVectors( pev->angles );

                pHurt->pev->punchangle.x = 5.0f;
                pHurt->pev->velocity = gpGlobals->v_up * 50.0f + gpGlobals->v_forward * 100.0f + pHurt->pev->velocity;

                pHurt->TakeDamage( pev, pev, gSkillData.hgruntDmgKick, DMG_CLUB );
            }
            break;
        }

        case 4:
        case 5:
        case 6:
        {
            justShotFlag = 1;
            switch ( m_ctBehavior )
            {
                case 0:
                case 7:
                    ShootPistol();
                    break;
                case 1:
                    ShootShotgun();
                    break;
                case 3:
                    ShootMachineGun();
                    break;
                case 4:
                    ShootSniperRifle();
                    break;
                case 5:
                    ShootLAW();
                    break;
                case 6:
                    ShootSMG();
                    break;
                case 8:
                    ShootMP5();
                    break;
                default:
                    ShootAssaultRifle();
                    break;
            }
            CSoundEnt::InsertSound( bits_SOUND_COMBAT, pev->origin, 384, 0.3f );
            break;
        }

        case 7:
        {
            UTIL_MakeVectors( pev->angles );
            Vector vecSrc = GetGunPosition();
            CGrenade::ShootTimed( pev, vecSrc, m_vecTossVelocity, 3.5f );
            
            m_fThrowGrenade = FALSE;
            m_flNextGrenadeCheck = gpGlobals->time + 6.0f;
            break;
        }

        case 8:
        {
            EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/glauncher.wav", 0.8f, ATTN_NORM, 0, 100 );
            Vector vecSrc = GetGunPosition();
            CGrenade::ShootContact( pev, vecSrc, m_vecTossVelocity );

            m_fThrowGrenade = FALSE;
            if ( g_iSkillLevel == 3 )
            {
                m_flNextGrenadeCheck = gpGlobals->time + RANDOM_FLOAT( 2.0f, 5.0f );
            }
            else
            {
                m_iWeaponEffectType = gpGlobals->time + 6.0f;
            }
            break;
        }

        case 9:
        {
            UTIL_MakeVectors( pev->angles );
            Vector vecSrc = pev->origin + gpGlobals->v_forward * 17.0f - gpGlobals->v_right * 27.0f + gpGlobals->v_up * 6.0f;
            CGrenade::ShootTimed( pev, vecSrc, m_vecTossVelocity, 3.0f );
            break;
        }

        case 10:
        {
            if ( FOkToSpeak() )
            {
                Speak( "CT_ENEMY", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            }
            break;
        }

        case 11:
        {
            Vector vecBoxPos, vecBoxAngles;
            GetAttachment( 0, vecBoxPos, vecBoxAngles );
            SetBodygroup( 2, 8 );

            if ( m_dropItem != 0 && RANDOM_FLOAT( 0.0f, 1.0f ) < m_dropChance )
            {
                DropItem( STRING( m_dropItem ), vecBoxPos, vecBoxAngles );
            }
            break;
        }

        case 13:
            break;

        case 14:
        {
            if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
            if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
            break;
        }

        default:
            CBaseMonster::HandleAnimEvent( pEvent );
            break;
    }
}

Schedule_t * CCounterTerrorist::HandleFollowing( void )
{
    if ( m_hEnemy == nullptr )
    {
        if ( m_hTargetEnt != nullptr )
        {
            CBaseEntity *pTarget = m_hTargetEnt;
            if ( pTarget->IsPlayer() )
            {
                if ( pTarget->IsAlive() )
                {
                    return GetScheduleOfType( 15 );
                }

                if ( m_movementGoal == MOVEGOAL_TARGETENT )
                {
                    RouteClear();
                }

                m_hTargetEnt = nullptr;

                if ( m_hEnemy != nullptr )
                {
                    m_IdealMonsterState = MONSTERSTATE_COMBAT;
                }
            }
        }
    }

    return nullptr;
}

Schedule_t * CCounterTerrorist::HandleIdleHeadWatch( void )
{
    if ( m_flStareTime <= gpGlobals->time && ( m_afConditions & COND_SEE_PLAYER ) != 0 )
    {
        if ( RANDOM_LONG( 0, 99 ) > 80 )
        {
            edict_t *pPlayerEdict = pfnPEntityOfEntIndex( 1 );
            if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
            {
                CBaseEntity *pPlayer = (CBaseEntity *)pPlayerEdict->pvPrivateData;
                
                Vector vecDiff = pPlayer->pev->origin - pev->origin;
                float flDistance = vecDiff.Length();

                if ( flDistance < 256.0f )
                {
                    m_hTalkTarget = pPlayer;
                    m_flStopTalkTime = gpGlobals->time + RANDOM_FLOAT( 4.0f, 6.0f );
                    m_flStareTime = gpGlobals->time + 12.0f;
                    
                    return slCTIdleEyecontact;
                }
            }
        }
    }

    return nullptr;
}

void CCounterTerrorist::HandleSpeaking( int schedule )
{
    if ( !FOkToSpeak() )
    {
        return;
    }

    switch ( schedule )
    {
        case 18:
            if ( RANDOM_LONG( 0, 1 ) != 0 )
            {
                Speak( "CT_COVER", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
            }
            break;

        case 19:
            Speak( "CT_GREN", 0.8f, 0.8f, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            break;

        case 24:
            if ( RANDOM_LONG( 0, 99 ) >= 51 )
            {
                Speak( "CT_ATTACK", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            }
            break;

        case 25:
            Speak( "CT_THROW", 0.8f, 0.8f, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            break;

        case 28:
            if ( RANDOM_LONG( 0, 99 ) > 50 )
            {
                Speak( "CT_TAUNT", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 7.0f;
            }
            break;

        case 50:
            if ( RANDOM_LONG( 0, 1 ) != 0 )
            {
                Speak( "CT_MOVING", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
            }
            break;

        case 66:
            if ( RANDOM_LONG( 0, 1 ) != 0 )
            {
                Speak( "CT_CHARGE", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
            }
            break;

        default:
            break;
    }

    if ( m_hEnemy != nullptr )
    {
        CBaseEntity *pEnemy = m_hEnemy;
        if ( pEnemy->IsPlayer() && RANDOM_LONG( 0, 99 ) > 75 )
        {
            Speak( "CT_MAD", 0.8f, 0.8f, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + 6.0f;
        }
    }
}

void CCounterTerrorist::IdleHeadTurn( Vector *vecFriend )
{
    if ( !( m_afCapability & bits_CAP_TURN_HEAD ) )
    {
        return;
    }

    if ( vecFriend == nullptr )
    {
        return;
    }

    Vector vecDir;
    vecDir.x = vecFriend->x - pev->origin.x;
    vecDir.y = vecFriend->y - pev->origin.y;
    vecDir.z = vecFriend->z - pev->origin.z;

    float flYaw = CBaseMonster::VecToYaw( vecDir );
    float flRelativeYaw = flYaw - pev->angles.y;

    if ( flRelativeYaw > 180.0f )
    {
        flRelativeYaw -= 360.0f;
    }
    
    if ( flRelativeYaw < -180.0f )
    {
        flRelativeYaw += 360.0f;
    }

    CBaseAnimating::SetBoneController( 0, flRelativeYaw );
}

void CCounterTerrorist::IdleSound( void )
{
    if ( RANDOM_LONG( 0, 99 ) > 75 )
    {
        return;
    }

    if ( !FOkToSpeak() )
    {
        return;
    }

    if ( g_fCTQuestion == 0 )
    {
        if ( RANDOM_LONG( 0, 1 ) == 0 )
        {
            return;
        }

        if ( g_fCTQuestion == 0 )
        {
            int iChoice = RANDOM_LONG( 0, 2 );
            if ( iChoice == 1 )
            {
                Speak( "CT_QUEST", 0.8f, 0.8f, 0, m_voicePitch );
                g_fCTQuestion = 2;
            }
            else if ( iChoice == 2 )
            {
                Speak( "CT_IDLE", 0.8f, 0.8f, 0, m_voicePitch );
            }
            else if ( iChoice == 0 )
            {
                Speak( "CT_CHECK", 0.8f, 0.8f, 0, m_voicePitch );
                g_fCTQuestion = 1;
            }
            goto REFRESH_TALK_CLOCK;
        }
    }

    if ( g_fCTQuestion == 1 )
    {
        Speak( "CT_CLEAR", 0.8f, 0.8f, 0, m_voicePitch );
    }
    else if ( g_fCTQuestion == 2 )
    {
        Speak( "CT_ANSWER", 0.8f, 0.8f, 0, m_voicePitch );
    }

    g_fCTQuestion = 0;

REFRESH_TALK_CLOCK:
    m_iSentence = -1;
    CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
}

void CCounterTerrorist::InitScheduleTable( void )
{
    for ( int i = 0; i < 73; i++ )
    {
        scheduleTable[i].activeFlag = 0.0f;
        scheduleTable[i].percentChance = 1.0f;
    }
}

int CCounterTerrorist::ISoundMask( void )
{
    return ( BITS_SOUND_COMBAT | BITS_SOUND_WORLD | BITS_SOUND_DANGER );
}

BOOL CCounterTerrorist::IsPlayerHidden( void )
{
    if ( m_MonsterState == MONSTERSTATE_COMBAT || m_MonsterState == MONSTERSTATE_SCRIPT )
    {
        return FALSE;
    }

    edict_t *pPlayerEdict = pfnPEntityOfEntIndex( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        return FALSE;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;
    if ( pPlayer->m_pStealthTrigger == nullptr )
    {
        return FALSE;
    }

    if ( CTriggerStealth::IsPlayerHidden( pPlayer->m_pStealthTrigger ) )
    {
        return TRUE;
    }

    pPlayer->m_pStealthTrigger = nullptr;
    return FALSE;
}

BOOL CCounterTerrorist::IsTalking( void )
{
    if ( gpGlobals->time < m_flStopTalkTime )
    {
        return TRUE;
    }

    return FALSE;
}

void CCounterTerrorist::JustSpoke( float nextSpeakTime )
{
    if ( nextSpeakTime == -1.0f )
    {
        CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
    }
    else
    {
        CTalkMonster::g_talkWaitTime = gpGlobals->time + nextSpeakTime;
    }

    m_iSentence = -1;
}

void CCounterTerrorist::KeyValue( KeyValueData *pkvd )
{
    if ( strcmp( pkvd->szKeyName, "behavior" ) == 0 )
    {
        m_behaviorType = strtol( pkvd->szValue, nullptr, 10 );
        pkvd->fHandled = 1;
    }
    else if ( strcmp( pkvd->szKeyName, "wander" ) == 0 )
    {
        m_canWander = ( strtol( pkvd->szValue, nullptr, 10 ) != 0 );
        pkvd->fHandled = 1;
    }
    else
    {
        CBaseMonster::KeyValue( pkvd );
    }
}

CBaseEntity * CCounterTerrorist::Kick( void )
{
    UTIL_MakeVectors( pev->angles );

    Vector vecStart;
    vecStart.x = pev->origin.x;
    vecStart.y = pev->origin.y;
    vecStart.z = pev->origin.z + ( pev->size.z * 0.75f );

    Vector vecEnd;

    if ( m_hEnemy == nullptr )
    {
        vecEnd.x = vecStart.x + gpGlobals->v_forward.x * 128.0f;
        vecEnd.y = vecStart.y + gpGlobals->v_forward.y * 128.0f;
        vecEnd.z = vecStart.z + gpGlobals->v_forward.z * 128.0f;
    }
    else
    {
        CBaseEntity *pEnemy = m_hEnemy;
        Vector vecEnemyTarget;
        vecEnemyTarget.x = pEnemy->pev->origin.x - vecStart.x;
        vecEnemyTarget.y = pEnemy->pev->origin.y - vecStart.y;
        vecEnemyTarget.z = ( pEnemy->pev->origin.z - pEnemy->pev->size.z * 0.25f ) - vecStart.z;

        float flLen = sqrtf( vecEnemyTarget.x * vecEnemyTarget.x + vecEnemyTarget.y * vecEnemyTarget.y + vecEnemyTarget.z * vecEnemyTarget.z );
        if ( flLen == 0.0f )
        {
            vecEnd.x = vecStart.x;
            vecEnd.y = vecStart.y;
            vecEnd.z = vecStart.z + 128.0f;
        }
        else
        {
            float flInvLen = 1.0f / flLen;
            vecEnd.x = vecStart.x + ( vecEnemyTarget.x * flInvLen * 128.0f );
            vecEnd.y = vecStart.y + ( vecEnemyTarget.y * flInvLen * 128.0f );
            vecEnd.z = vecStart.z + ( vecEnemyTarget.z * flInvLen * 128.0f );
        }
    }

    TraceResult tr;
    UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, head_hull, edict(), &tr );

    if ( tr.pHit != nullptr )
    {
        return CBaseEntity::Instance( tr.pHit );
    }

    return nullptr;
}

void CCounterTerrorist::LaserGlowOff( void )
{
    if ( m_pBeam != nullptr )
    {
        UTIL_Remove( m_pBeam );
        m_pBeam = nullptr;
    }

    if ( m_pLaserGlow != nullptr )
    {
        UTIL_Remove( m_pLaserGlow );
        m_pLaserGlow = nullptr;
    }
}

void CCounterTerrorist::LaserGlowOn( void )
{
    if ( m_pBeam != nullptr )
    {
        if ( m_hEnemy != nullptr )
        {
            CBaseEntity *pEnemy = m_hEnemy;
            int iEnemyIndex = ENTINDEX( pEnemy->edict() );
            int iMyIndex = ENTINDEX( edict() );

            m_pBeam->EntsInit( iMyIndex, iEnemyIndex );

            entvars_t *pBeamPev = m_pBeam->pev;
            pBeamPev->sequence = ( pBeamPev->sequence & 0x0FFF ) | 0x2000;

            m_laserBrightness = 64.0f;
            pBeamPev->renderamt = 64.0f;
        }
    }

    if ( m_pLaserGlow == nullptr )
    {
        return;
    }

    m_glowBrightness = 255.0f;
    m_pLaserGlow->pev->renderamt = 255.0f;
}

void CCounterTerrorist::LimitFollowers( CBaseEntity *pPlayer, int maxFollowers )
{
    return;
}

int CCounterTerrorist::LookupActivity( int activity )
{
    void *pModelPtr = GET_MODEL_PTR( edict() );

    if ( activity != ACT_IDLE )
    {
        return ::LookupActivity( pModelPtr, pev, activity );
    }

    const char *pSequenceName;

    if ( m_ctBehavior == 5 )
    {
        pSequenceName = "law_fire_idle";
    }
    else if ( m_ctBehavior == 0 || m_ctBehavior == 7 || m_ctBehavior > 7 )
    {
        if ( m_MonsterState != MONSTERSTATE_COMBAT )
        {
            return ::LookupActivity( pModelPtr, pev, ACT_IDLE );
        }

        if ( m_bStanding == 0 )
        {
            pSequenceName = "pistol_crouching_wait";
        }
        else
        {
            pSequenceName = "machete_combatidle";
        }
    }
    else
    {
        if ( m_MonsterState == MONSTERSTATE_COMBAT )
        {
            pSequenceName = "pistol_combatidle";
        }
        else
        {
            pSequenceName = "pistol_idle1";
        }
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::LookupActivityHeaviest( int activity )
{
    void *pModelPtr = GET_MODEL_PTR( edict() );

    if ( activity != ACT_IDLE )
    {
        return ::LookupActivity( pModelPtr, pev, activity );
    }

    const char *pSequenceName;

    if ( m_ctBehavior == 5 )
    {
        pSequenceName = "law_fire_idle";
    }
    else if ( m_ctBehavior == 0 || m_ctBehavior == 7 || m_ctBehavior > 7 )
    {
        if ( m_MonsterState != MONSTERSTATE_COMBAT )
        {
            return ::LookupActivityHeaviest( pModelPtr, pev, ACT_IDLE );
        }

        if ( m_bStanding == 0 )
        {
            pSequenceName = "pistol_crouching_wait";
        }
        else
        {
            pSequenceName = "machete_combatidle";
        }
    }
    else
    {
        pSequenceName = "pistol_idle1";
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::ObjectCaps( void )
{
    return ( FCAP_DONT_SAVE | FCAP_IMPULSE_USE );
}

void CCounterTerrorist::PainSound( void )
{
    if ( m_flNextPainTime < gpGlobals->time )
    {
        Speak( "CT_WOUND", 0.8f, 0.8f, 0, m_voicePitch );
        m_flNextPainTime = gpGlobals->time + 1.0f;
    }
}

void CCounterTerrorist::PlayScriptedSentence( char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener )
{
    if ( !IsAlive() || pev->deadflag == DEAD_DYING )
    {
        return;
    }

    if ( duration + 2.0f == -1.0f )
    {
        CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
    }
    else
    {
        CTalkMonster::g_talkWaitTime = gpGlobals->time + duration + 2.0f;
    }

    m_iSentence = -1;

    PlaySentence( pszSentence, duration, volume, attenuation );

    edict_t *pPlayerEdict = pfnPEntityOfEntIndex( 1 );
    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;
        pPlayer->m_flNextRadioTime = gpGlobals->time + duration;
    }

    m_hTalkTarget = pListener;

    if ( m_hTalkTarget != nullptr && ( m_afCapability & bits_CAP_TURN_HEAD ) )
    {
        if ( pListener != nullptr )
        {
            IdleHeadTurn( &(pListener->pev->origin) );
        }
    }

    m_flStopTalkTime = gpGlobals->time + duration;
}

void CCounterTerrorist::PrescheduleThink( void )
{
    if ( m_flStopTalkTime != 0.0f && m_flStopTalkTime < gpGlobals->time )
    {
        m_flStopTalkTime = 0.0f;
        CBaseAnimating::SetBoneController( 0, 0.0f );
        m_hTalkTarget = nullptr;
    }

    if ( m_hSquadLeader != nullptr && m_hEnemy != nullptr )
    {
        CSquadMonster *pSquadLeader = (CSquadMonster *)((CBaseEntity *)m_hSquadLeader);
        if ( pSquadLeader == nullptr )
        {
            pSquadLeader = this;
        }

        if ( m_afConditions & COND_SEE_ENEMY )
        {
            pSquadLeader->m_flLastEnemySightTime = gpGlobals->time;
            return;
        }

        if ( gpGlobals->time - pSquadLeader->m_flLastEnemySightTime > 5.0f )
        {
            pSquadLeader->m_fEnemyEluded = TRUE;
            return;
        }
    }
}

void CCounterTerrorist::ResetHeadWatch( void )
{
    CBaseAnimating::SetBoneController( 0, 0.0f );
    m_hTalkTarget = nullptr;
}

int CCounterTerrorist::Restore( CRestore *restore )
{
    if ( !CSquadMonster::Restore( restore ) )
    {
        return 0;
    }

    return restore->ReadFields( "CCounterTerrorist", this, m_SaveData, ARRAYSIZE( m_SaveData ) );
}

int CCounterTerrorist::Save( CSave *save )
{
    if ( !CSquadMonster::Save( save ) )
    {
        return 0;
    }

    return save->WriteFields( "CCounterTerrorist", this, m_SaveData, ARRAYSIZE( m_SaveData ) );
}

void CCounterTerrorist::RunTask( Task_t *pTask )
{
    switch ( pTask->iTask )
    {
        case 96:
        {
            if ( m_cantMove == 0 )
            {
                if ( m_hEnemy == nullptr )
                {
                    m_afConditions |= bits_COND_TASK_FAILED;
                    return;
                }

                Vector vecDiffGoal = m_vecMoveGoal - pev->origin;
                float flDistToGoal = sqrtf( vecDiffGoal.x * vecDiffGoal.x + vecDiffGoal.y * vecDiffGoal.y );

                if ( flDistToGoal >= m_fPreferedRange )
                {
                    CBaseEntity *pEnemy = m_hEnemy;
                    Vector vecDiffEnemy = m_vecMoveGoal - pEnemy->pev->origin;
                    float flDistGoalToEnemy = sqrtf( vecDiffEnemy.x * vecDiffEnemy.x + vecDiffEnemy.y * vecDiffEnemy.y + vecDiffEnemy.z * vecDiffEnemy.z );

                    if ( flDistGoalToEnemy <= m_fPreferedRange * 0.5f )
                    {
                        goto RES_ROUTE_CHECK;
                    }
                }

                CBaseEntity *pEnemyEnt = m_hEnemy;
                m_vecMoveGoal = pEnemyEnt->pev->origin;
                
                Vector vecNewDiff = m_vecMoveGoal - pev->origin;
                flDistToGoal = sqrtf( vecNewDiff.x * vecNewDiff.x + vecNewDiff.y * vecNewDiff.y );

                FRefreshRoute();

            RES_ROUTE_CHECK:
                if ( flDistToGoal < m_fPreferedRange )
                {
                    if ( !( m_afConditions & bits_COND_TASK_FAILED ) )
                    {
                        m_iTaskStatus = TASKSTATUS_COMPLETE;
                    }
                    RouteClear();
                }

                if ( m_movementActivity != ACT_RUN )
                {
                    m_movementActivity = ACT_RUN;
                }
                return;
            }
            break;
        }

        case 92:
        {
            Vector vecTargetToss;
            vecTargetToss.x = pev->origin.x + m_vecTossVelocity.x * 64.0f;
            vecTargetToss.y = pev->origin.y + m_vecTossVelocity.y * 64.0f;
            vecTargetToss.z = pev->origin.z + m_vecTossVelocity.z * 64.0f;

            MakeIdealYaw( vecTargetToss );
            ChangeYaw( (int)pev->yaw_speed );

            if ( !FacingIdeal() )
            {
                return;
            }
            
            m_iTaskStatus = TASKSTATUS_COMPLETE;
            return;
        }

        case 103:
        {
            if ( gpGlobals->time < m_flStopTalkTime )
            {
                if ( m_hTalkTarget != nullptr && ( m_afCapability & bits_CAP_TURN_HEAD ) )
                {
                    CBaseEntity *pTalkEnt = m_hTalkTarget;
                    IdleHeadTurn( &(pTalkEnt->pev->origin) );
                    return;
                }
            }
            break;
        }

        default:
            CBaseMonster::RunTask( pTask );
            return;
    }

    if ( m_afConditions & bits_COND_TASK_FAILED )
    {
        return;
    }

    m_iTaskStatus = TASKSTATUS_COMPLETE;
}

Schedule_t * CCounterTerrorist::ScheduleFromName( char *pName )
{
    Schedule_t *pSchedule = CSquadMonster::ScheduleInList( pName, m_scheduleList, 41 );
    if ( pSchedule != nullptr )
    {
        return pSchedule;
    }

    return CSquadMonster::ScheduleFromName( pName );
}

void CCounterTerrorist::SetActivity( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );
    int iSequence = -1;

    switch ( m_ctBehavior )
    {
        case 0:
        case 6:
            iSequence = SetActivityPistolBase( NewActivity );
            break;
        case 1:
            iSequence = SetActivityShotgunBase( NewActivity );
            break;
        case 2:
        case 3:
        case 8:
            iSequence = SetActivityAutomaticBase( NewActivity );
            break;
        case 4:
            iSequence = SetActivitySniperBase( NewActivity );
            break;
        case 5:
            iSequence = SetActivityLAWBase( NewActivity );
            break;
        case 7:
            iSequence = SetActivityPistolBase( NewActivity );
            if ( iSequence == -1 )
            {
                iSequence = SetActivityGrenadeBase( NewActivity );
            }
            break;
        default:
            break;
    }

    if ( iSequence == -1 )
    {
        switch ( NewActivity )
        {
            case ACT_IDLE:
                if ( m_idleAnim != 0 )
                {
                    iSequence = LookupSequence( STRING( m_idleAnim ) );
                    break;
                }
                if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    if ( m_bStanding == 0 ) iSequence = LookupSequence( "crouching_wait" );
                    else iSequence = LookupSequence( "combatidle" );
                }
                break;

            case ACT_WALK:
                if ( pev->health <= pev->max_health * 0.2f )
                {
                    iSequence = LookupActivity( ACT_WALK_HURT );
                }
                else
                {
                    if ( m_MonsterState == MONSTERSTATE_COMBAT ) iSequence = LookupSequence( "combat_walk_primary" );
                }
                break;

            case ACT_RUN:
                if ( pev->health <= pev->max_health * 0.2f )
                {
                    iSequence = LookupActivity( ACT_RUN_HURT );
                }
                else
                {
                    if ( m_MonsterState == MONSTERSTATE_COMBAT ) iSequence = LookupSequence( "combat_run_primary" );
                }
                break;

            case ACT_STAND:
                iSequence = LookupSequence( "crouching_2_stand" );
                break;

            case ACT_DIESIMPLE:
            case ACT_DIEBACKWARD:
            case ACT_DIEFORWARD:
            case ACT_DIEVIOLENT:
            case ACT_DIE_HEADSHOT:
            case ACT_DIE_CHESTSHOT:
            case ACT_DIE_GUTSHOT:
            case ACT_DIE_BACKSHOT:
                if ( m_deathAnim != 0 )
                {
                    iSequence = LookupSequence( STRING( m_deathAnim ) );
                }
                break;

            case ( ACT_LASTACTIVITY | ACT_IDLE ):
                iSequence = LookupSequence( "pistol_popout_right01" );
                break;
            case ( ACT_LASTACTIVITY | ACT_GUARD ):
                iSequence = LookupSequence( "pistol_popout_right02" );
                break;
            case ( ACT_LASTACTIVITY | ACT_WALK ):
                iSequence = LookupSequence( "choking" );
                break;
            case ( ACT_LASTACTIVITY | ACT_RUN ):
                iSequence = LookupSequence( "stunned01" );
                break;

            default:
                break;
        }
    }

    if ( iSequence == -1 )
    {
        iSequence = CBaseMonster::LookupActivity( NewActivity );
        m_Activity = NewActivity;
    }
    else
    {
        m_Activity = NewActivity;
    }

    if ( iSequence < 0 )
    {
        ALERT( at_console, "%s has no sequence for act:%d\n", STRING( pev->classname ), NewActivity );
        pev->sequence = 0;
        return;
    }

    if ( iSequence != pev->sequence || m_fSequenceLoops != 0 )
    {
        pev->frame = 0.0f;
    }

    pev->sequence = iSequence;
    ResetSequenceInfo();
    SetYawSpeed();
}

int CCounterTerrorist::SetActivityAutomaticBase( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );

    const char *pSequenceName;

    if ( NewActivity == ACT_RANGE_ATTACK1 )
    {
        if ( m_ctBehavior == 3 )
        {
            pSequenceName = "machinegun_fire";
        }
        else if ( m_bStanding == 0 )
        {
            pSequenceName = "crouching_mp5";
        }
        else
        {
            pSequenceName = "standing_mp5";
        }
    }
    else
    {
        if ( NewActivity != ACT_RELOAD )
        {
            return -1;
        }
        pSequenceName = "crouch_reload_mp5";
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::SetActivityGrenadeBase( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );

    if ( NewActivity != ACT_RANGE_ATTACK2 )
    {
        return -1;
    }

    const char *pSequenceName;

    if ( ( pev->weapons & m_iWeaponBit_Flashbang ) == 0 )
    {
        pSequenceName = "launchgrenade";
    }
    else
    {
        pSequenceName = "throwgrenade";
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::SetActivityLAWBase( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );

    const char *pSequenceName;

    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim == 0 )
            {
                pSequenceName = "law_fire_idle";
            }
            else
            {
                pSequenceName = STRING( m_idleAnim );
            }
            break;

        case ACT_WALK:
        case ACT_RUN:
            pSequenceName = "law_run";
            break;

        case ACT_TURN_LEFT:
            pSequenceName = "law_180L";
            break;

        case ACT_TURN_RIGHT:
            pSequenceName = "law_180R";
            break;

        case ACT_RANGE_ATTACK1:
            pSequenceName = "law_fire";
            break;

        case ACT_RELOAD:
            pSequenceName = "law_reload";
            break;

        default:
            return -1;
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::SetActivityPistolBase( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );
    const char *pSequenceName;

    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim != 0 )
            {
                pSequenceName = STRING( m_idleAnim );
            }
            else if ( m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pSequenceName = "pistol_idle1";
            }
            else if ( m_bStanding != 0 )
            {
                pSequenceName = "pistol_combatidlle";
            }
            else
            {
                pSequenceName = "pistol_crouching_wait";
            }
            break;

        case ACT_CROUCH:
            pSequenceName = "pistol_crouching_wait";
            break;

        case ACT_WALK:
            if ( pev->health <= pev->max_health * 0.2f || m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pSequenceName = "pistol_walk";
            }
            else
            {
                pSequenceName = "combat_walk_pistol";
            }
            break;

        case ACT_RUN:
            if ( pev->health <= pev->max_health * 0.2f || m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pSequenceName = "pistol_run";
            }
            else
            {
                pSequenceName = "combat_run_pistol";
            }
            break;

        case ACT_STRAFE_LEFT:
            pSequenceName = "pistol_strafefire_L";
            break;

        case ACT_STRAFE_RIGHT:
            pSequenceName = "pistol_strafefire_R";
            break;

        case ACT_TURN_LEFT:
            if ( m_MonsterState == MONSTERSTATE_COMBAT )
            {
                pSequenceName = "pistol_180L";
            }
            else
            {
                pSequenceName = "pistol_idle1";
            }
            break;

        case ACT_TURN_RIGHT:
            if ( m_MonsterState == MONSTERSTATE_COMBAT )
            {
                pSequenceName = "pistol_180R";
            }
            else
            {
                pSequenceName = "pistol_idle1";
            }
            break;

        case ACT_COWER:
            pSequenceName = "pistol_cower";
            break;

        case ACT_SMALL_FLINCH:
            pSequenceName = "pistol_smflinch";
            break;

        case ACT_RANGE_ATTACK1:
            if ( m_ctBehavior == 6 )
            {
                pSequenceName = ( m_bStanding == 0 ) ? "crouching_tmp" : "shootcycle_tmp";
            }
            else
            {
                pSequenceName = ( m_bStanding == 0 ) ? "pistol_crouching" : "pistol_standing";
            }
            break;

        case ACT_MELEE_ATTACK1:
            pSequenceName = "pistol_melee";
            break;

        case ACT_RELOAD:
            pSequenceName = "crouch_pistol_reload";
            break;

        case ACT_DIESIMPLE:
            pSequenceName = ( m_deathAnim == 0 ) ? "pistol_die-simple" : STRING( m_deathAnim );
            break;

        case ACT_DIEBACKWARD:
            pSequenceName = ( m_deathAnim == 0 ) ? "pistol_dieback1" : STRING( m_deathAnim );
            break;

        case ACT_DIEFORWARD:
        case ACT_DIE_BACKSHOT:
            pSequenceName = ( m_deathAnim == 0 ) ? "pistol_dieforward" : STRING( m_deathAnim );
            break;

        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
            pSequenceName = ( m_deathAnim == 0 ) ? "pistol_headshot" : STRING( m_deathAnim );
            break;

        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
            pSequenceName = ( m_deathAnim == 0 ) ? "pistol_diegutshot" : STRING( m_deathAnim );
            break;

        case ACT_FLINCH_LEFTARM:
            pSequenceName = "pistol_leftarmflinch";
            break;

        case ACT_FLINCH_RIGHTARM:
            pSequenceName = "pistol_rightarmflinch";
            break;

        case ACT_FLINCH_LEFTLEG:
            pSequenceName = "pistol_leftlegsmflinch";
            break;

        case ACT_FLINCH_RIGHTLEG:
            pSequenceName = "pistol_rightlegsmflinch";
            break;

        default:
            return -1;
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::SetActivityShotgunBase( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );

    const char *pSequenceName;

    if ( NewActivity == ACT_RANGE_ATTACK1 )
    {
        if ( m_bStanding == 0 )
        {
            pSequenceName = "crouching_shotgun";
        }
        else
        {
            pSequenceName = "standing_shotgun";
        }
    }
    else
    {
        if ( NewActivity != ACT_RELOAD )
        {
            return -1;
        }
        
        pSequenceName = "crouch_reload_shotgun";
    }

    return LookupSequence( pSequenceName );
}

int CCounterTerrorist::SetActivitySniperBase( Activity NewActivity )
{
    GET_MODEL_PTR( edict() );

    switch ( NewActivity )
    {
        case ACT_RANGE_ATTACK1:
        {
            const char *pSequenceName = ( m_bStanding == 0 ) ? "sniper_crouch-fire" : "sniper-fire";
            return LookupSequence( pSequenceName );
        }

        case ACT_ARM:
        {
            int iSeq = LookupSequence( "laser_on_stand" );
            LaserGlowOn();
            return iSeq;
        }

        case ACT_DISARM:
        {
            int iSeq = LookupSequence( "laser_off_stand" );
            LaserGlowOff();
            return iSeq;
        }

        case ACT_DIESIMPLE:
        case ACT_DIEBACKWARD:
        case ACT_DIEFORWARD:
        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
        case ACT_DIE_BACKSHOT:
        {
            LaserGlowOff();
            return -1;
        }

        default:
            break;
    }

    return -1;
}

void CCounterTerrorist::SetupBehaviors( int type )
{
    if ( type == 1 )
    {
        scheduleTable[18].percentChance = 0.0f;
        scheduleTable[17].percentChance = 0.9f;
        scheduleTable[67].percentChance = 0.0f;
        scheduleTable[68].percentChance = 0.0f;
        scheduleTable[51].percentChance = 1.0f;
        scheduleTable[50].percentChance = 0.0f;
        scheduleTable[66].percentChance = 0.4f;
        scheduleTable[24].percentChance = 0.9f;
        scheduleTable[25].percentChance = 0.0f;
        scheduleTable[22].percentChance = 0.3f;
        scheduleTable[62].percentChance = 0.0f;
        scheduleTable[63].percentChance = 0.0f;
        scheduleTable[64].percentChance = 0.2f;
        scheduleTable[28].percentChance = 0.1f;
        scheduleTable[46].percentChance = 0.4f;
        m_fPreferedRange = 128.0f;
        return;
    }

    if ( type == 2 )
    {
        scheduleTable[18].percentChance = 0.0f;
        scheduleTable[17].percentChance = 0.9f;
        scheduleTable[67].percentChance = 0.0f;
        scheduleTable[68].percentChance = 0.0f;
        scheduleTable[51].percentChance = 1.0f;
        scheduleTable[50].percentChance = 0.8f;
        scheduleTable[66].percentChance = 0.05f;
    }
    else
    {
        if ( type == 3 )
        {
            scheduleTable[18].percentChance = 0.0f;
            scheduleTable[17].percentChance = 0.9f;
            scheduleTable[67].percentChance = 0.0f;
            scheduleTable[68].percentChance = 0.0f;
            scheduleTable[51].percentChance = 0.0f;
            scheduleTable[50].percentChance = 0.5f;
            scheduleTable[66].percentChance = 0.0f;
            scheduleTable[24].percentChance = 0.95f;
            scheduleTable[25].percentChance = 0.0f;
            scheduleTable[22].percentChance = 0.0f;
            scheduleTable[62].percentChance = 0.0f;
            scheduleTable[63].percentChance = 0.0f;
            scheduleTable[64].percentChance = 0.1f;
            scheduleTable[28].percentChance = 0.1f;
            scheduleTable[46].percentChance = 0.1f;
            return;
        }

        if ( type == 6 )
        {
            scheduleTable[18].percentChance = 0.0f;
            scheduleTable[17].percentChance = 0.8f;
            scheduleTable[67].percentChance = 0.0f;
            scheduleTable[68].percentChance = 0.0f;
            scheduleTable[51].percentChance = 1.0f;
            scheduleTable[50].percentChance = 0.8f;
            scheduleTable[66].percentChance = 0.1f;
            scheduleTable[24].percentChance = 0.9f;
            scheduleTable[25].percentChance = 0.0f;
            scheduleTable[22].percentChance = 0.1f;
            scheduleTable[62].percentChance = 0.0f;
            scheduleTable[63].percentChance = 0.0f;
            scheduleTable[64].percentChance = 0.3f;
            scheduleTable[28].percentChance = 0.1f;
            scheduleTable[46].percentChance = 0.1f;
            m_fPreferedRange = 192.0f;
            return;
        }

        if ( type != 8 )
        {
            if ( type == 5 )
            {
                scheduleTable[18].percentChance = 0.0f;
                scheduleTable[17].percentChance = 0.9f;
                scheduleTable[67].percentChance = 0.0f;
                scheduleTable[68].percentChance = 0.0f;
                scheduleTable[51].percentChance = 1.0f;
                scheduleTable[50].percentChance = 0.9f;
                scheduleTable[66].percentChance = 0.0f;
                scheduleTable[24].percentChance = 1.0f;
                scheduleTable[25].percentChance = 0.0f;
                scheduleTable[22].percentChance = 0.0f;
                scheduleTable[62].percentChance = 0.0f;
                scheduleTable[63].percentChance = 0.0f;
                scheduleTable[64].percentChance = 0.0f;
                scheduleTable[28].percentChance = 0.1f;
                scheduleTable[46].percentChance = 0.0f;
                return;
            }

            if ( type == 0 )
            {
                scheduleTable[18].percentChance = 0.0f;
                scheduleTable[17].percentChance = 0.9f;
                scheduleTable[67].percentChance = 0.0f;
                scheduleTable[68].percentChance = 0.0f;
                scheduleTable[51].percentChance = 1.0f;
                scheduleTable[50].percentChance = 0.9f;
                scheduleTable[66].percentChance = 0.1f;
                scheduleTable[24].percentChance = 1.0f;
                scheduleTable[25].percentChance = 0.0f;
                scheduleTable[22].percentChance = 0.2f;
                scheduleTable[62].percentChance = 0.0f;
                scheduleTable[63].percentChance = 0.0f;
                scheduleTable[64].percentChance = 1.0f;
                scheduleTable[28].percentChance = 0.1f;
                scheduleTable[46].percentChance = 0.4f;
                m_fPreferedRange = 192.0f;
                return;
            }

            if ( type == 7 )
            {
                scheduleTable[18].percentChance = 0.1f;
                scheduleTable[17].percentChance = 0.9f;
                scheduleTable[67].percentChance = 0.0f;
                scheduleTable[68].percentChance = 0.0f;
                scheduleTable[51].percentChance = 1.0f;
                scheduleTable[50].percentChance = 0.0f;
                scheduleTable[66].percentChance = 0.0f;
                scheduleTable[24].percentChance = 0.1f;
                scheduleTable[25].percentChance = 1.0f;
                scheduleTable[22].percentChance = 0.0f;
                scheduleTable[62].percentChance = 0.0f;
                scheduleTable[63].percentChance = 0.0f;
                scheduleTable[64].percentChance = 0.0f;
                scheduleTable[28].percentChance = 0.2f;
                scheduleTable[46].percentChance = 0.5f;
                return;
            }

            if ( type != 4 )
            {
                return;
            }

            scheduleTable[18].percentChance = 0.0f;
            scheduleTable[17].percentChance = 1.0f;
            scheduleTable[67].percentChance = 0.0f;
            scheduleTable[68].percentChance = 0.0f;
            scheduleTable[51].percentChance = 0.0f;
            scheduleTable[50].percentChance = 0.5f;
            scheduleTable[66].percentChance = 0.0f;
            scheduleTable[24].percentChance = 0.0f;
            scheduleTable[25].percentChance = 0.0f;
            scheduleTable[22].percentChance = 0.2f;
            scheduleTable[62].percentChance = 1.0f;
            scheduleTable[63].percentChance = 0.5f;
            scheduleTable[64].percentChance = 0.0f;
            scheduleTable[28].percentChance = 0.0f;
            scheduleTable[46].percentChance = 0.0f;
            return;
        }
        scheduleTable[18].percentChance = 0.0f;
        scheduleTable[17].percentChance = 0.8f;
        scheduleTable[67].percentChance = 0.0f;
        scheduleTable[68].percentChance = 0.0f;
        scheduleTable[51].percentChance = 1.0f;
        scheduleTable[50].percentChance = 0.8f;
        scheduleTable[66].percentChance = 0.1f;
    }

    scheduleTable[24].percentChance = 0.9f;
    scheduleTable[25].percentChance = 0.0f;
    scheduleTable[22].percentChance = 0.3f;
    scheduleTable[62].percentChance = 0.0f;
    scheduleTable[63].percentChance = 0.0f;
    scheduleTable[64].percentChance = 0.4f;
    scheduleTable[28].percentChance = 0.1f;
    scheduleTable[46].percentChance = 0.6f;
    m_fPreferedRange = 256.0f;
}

void CCounterTerrorist::SetupLaserGlow( void )
{
    if ( m_pBeam == nullptr )
    {
        m_pBeam = CBeam::BeamCreate( "sprites/laserbeam.spr", 3 );
        if ( m_pBeam != nullptr )
        {
            m_pBeam->pev->spawnflags |= SF_BEAM_TEMPORARY;
            m_pBeam->pev->rendermode = kRenderTransAdd;
            m_pBeam->pev->rendercolor = Vector( 255.0f, 0.0f, 0.0f );
            m_pBeam->pev->renderamt = 0.0f;
            m_pBeam->pev->renderfx = kRenderFxNoDissipation;
            m_laserBrightness = 0.0f;
        }
    }

    if ( m_pLaserGlow == nullptr )
    {
        m_pLaserGlow = CSprite::SpriteCreate( "sprites/flare3.spr", pev->origin, FALSE );
        if ( m_pLaserGlow != nullptr )
        {
            entvars_t *pGlowPev = m_pLaserGlow->pev;
            pGlowPev->rendermode = kRenderTransAdd;
            pGlowPev->rendercolor = Vector( 255.0f, 0.0f, 0.0f );
            pGlowPev->renderamt = 0.0f;
            pGlowPev->renderfx = kRenderFxNoDissipation;

            if ( edict() != nullptr )
            {
                pGlowPev->skin = ENTINDEX( edict() );
                pGlowPev->body = 2;
                pGlowPev->aiment = edict();
                pGlowPev->movetype = MOVETYPE_FOLLOW;
            }
            pGlowPev->renderamt = 0.0f;
            m_glowBrightness = 0.0f;
        }
    }
}

void CCounterTerrorist::SetupWeapons( void )
{
    m_cClipSize = 36;
    m_cAmmoLoaded = 36;

    unsigned int uiWeapons = pev->weapons;

    if ( ( uiWeapons & 0x40 ) != 0 )
    {
        SetBodygroup( 2, 8 );
        m_cAmmoLoaded = 0;
        m_cClipSize = 0;
        return;
    }

    if ( m_ctBehavior == -1 )
    {
        if ( uiWeapons == 0 )
        {
            pev->weapons = 3;
            SetBodygroup( 2, 1 );
            return;
        }

        if ( ( uiWeapons & 1 ) != 0 )
        {
            SetBodygroup( 2, 0 );
            uiWeapons = pev->weapons;
        }
        if ( ( uiWeapons & 8 ) != 0 )
        {
            SetBodygroup( 2, 1 );
            m_cClipSize = 8;
            uiWeapons = pev->weapons;
        }
        if ( ( uiWeapons & 0x20 ) != 0 )
        {
            SetBodygroup( 2, 2 );
            uiWeapons = pev->weapons;
        }
        if ( ( uiWeapons & 0x10 ) != 0 )
        {
            SetBodygroup( 2, 3 );
        }
    }
    else
    {
        switch ( m_ctBehavior )
        {
            case 0:
                m_cClipSize = 16;
                SetBodygroup( 2, 5 );
                break;

            case 1:
                pev->weapons = 8;
                m_cClipSize = 8;
                SetBodygroup( 2, 1 );
                break;

            case 2:
                pev->weapons = 1;
                m_cClipSize = 30;
                SetBodygroup( 2, 4 );
                break;

            case 3:
                SetBodygroup( 2, 6 );
                break;

            case 4:
                pev->weapons = 16;
                SetBodygroup( 2, 3 );
                break;

            case 5:
                m_cClipSize = 1;
                SetBodygroup( 2, 2 );
                break;

            case 6:
                m_cClipSize = 45;
                pev->weapons = 1;
                SetBodygroup( 2, 7 );
                break;

            case 7:
                m_cClipSize = 16;
                pev->weapons = 2;
                SetBodygroup( 2, 5 );
                break;

            default:
                SetBodygroup( 2, 0 );
                break;
        }
    }

    m_cAmmoLoaded = m_cClipSize;
}

void CCounterTerrorist::SetYawSpeed( void )
{
    float flYawSpeed = 90.0f;
    Activity act = m_Activity;

    if ( ( act - ACT_IDLE ) < 51 )
    {
        switch ( act )
        {
            case ACT_WALK:
                flYawSpeed = 60.0f;
                break;
            case ACT_RUN:
                flYawSpeed = 150.0f;
                break;
            case ACT_IDLE:
            case ACT_COMBATIDLE:
                flYawSpeed = 90.0f;
                break;
            default:
                flYawSpeed = 90.0f;
                break;
        }
    }

    pev->yaw_speed = flYawSpeed;
}

void CCounterTerrorist::ShootAssaultRifle( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    pev->effects |= EF_MUZZLEFLASH;
    m_cAmmoLoaded--;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    Vector vecShellVelocity;
    Vector vecShellOrigin;

    if ( m_ctBehavior == 3 ) 
    {
        float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
        float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
        float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

        vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
        vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 8.0f;

        int iShellModel = m_iShellId; 
        EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

        const char *pAKSound = ( RANDOM_LONG( 0, 1 ) == 0 ) ? "weapons/ak47-2.wav" : "weapons/ak47-1.wav";
        EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pAKSound, 0.75f, ATTN_NORM, 0, 100 );
    }
    else
    {
        float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
        float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
        float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

        vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
        vecShellOrigin = vecShootOrigin - gpGlobals->v_forward * 24.0f;

        int iShellModel = m_iShellId;
        EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

        const char *pM4Sound = ( RANDOM_LONG( 0, 99 ) < 51 ) ? "weapons/m4a1_unsil-2.wav" : "weapons/m4a1_unsil-1.wav";
        EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pM4Sound, 0.75f, ATTN_NORM, 0, 100 );
    }
}

void CCounterTerrorist::ShootLAW( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    vecAngles.x = -vecAngles.x;

    UTIL_MakeVectors( vecAngles );

    float flRandRight1 = RANDOM_FLOAT( -0.5f, 0.5f );
    float flRandRight2 = RANDOM_FLOAT( -0.5f, 0.5f );
    float flRandUp1 = RANDOM_FLOAT( -0.5f, 0.5f );
    float flRandUp2 = RANDOM_FLOAT( -0.5f, 0.5f );

    float flSpreadRight = ( flRandRight1 + flRandRight2 ) * m_weaponAccuracy.x;
    float flSpreadUp = ( flRandUp1 + flRandUp2 ) * m_weaponAccuracy.y;

    Vector vecFinalDir;
    vecRefDir.x = gpGlobals->v_forward.x + ( gpGlobals->v_right.x * flSpreadRight ) + ( gpGlobals->v_up.x * flSpreadUp );
    vecRefDir.y = gpGlobals->v_forward.y + ( gpGlobals->v_right.y * flSpreadRight ) + ( gpGlobals->v_up.y * flSpreadUp );
    vecRefDir.z = gpGlobals->v_forward.z + ( gpGlobals->v_right.z * flSpreadRight ) + ( gpGlobals->v_up.z * flSpreadUp );

    Vector vecFinalAngles;
    UTIL_VecToAngles( vecFinalDir, vecFinalAngles );
    vecFinalAngles.x = -vecFinalAngles.x;

    CLawsRocket::CreateLawsRocket( vecShootOrigin, vecFinalAngles, this );

    unsigned short usLawsEvent = PRECACHE_EVENT( 1, "events/laws.sc" );
    PLAYBACK_EVENT_FULL( 0, edict(), usLawsEvent, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );

    vecAngles.x = -vecAngles.x;
    SetBlending( 0, vecAngles.x );

    m_cAmmoLoaded--;
}

void CCounterTerrorist::ShootMachineGun( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    pev->effects |= EF_MUZZLEFLASH;
    m_cAmmoLoaded--;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
    float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
    Vector vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 8.0f;

    int iShellModel = m_iBrassShell;
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

    const char *pM60Sound = ( RANDOM_LONG( 0, 1 ) == 0 ) ? "weapons/m60-2.wav" : "weapons/m4a1_unsil-1.wav"; // 註：反編譯重疊修復，原廠採用 M60 機槍雙重音效包
    if ( RANDOM_LONG( 0, 1 ) == 1 )
    {
        pM60Sound = "weapons/m60-1.wav";
    }

    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pM60Sound, 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootMP5( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    pev->effects |= EF_MUZZLEFLASH;
    m_cAmmoLoaded--;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
    float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
    Vector vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 8.0f;

    int iShellModel = m_iBrassShell;
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

    const char *pMP5Sound = ( RANDOM_LONG( 0, 1 ) == 0 ) ? "weapons/mp5-2.wav" : "weapons/mp5-1.wav";
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pMP5Sound, 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootPistol( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    m_cAmmoLoaded--;
    pev->effects |= EF_MUZZLEFLASH;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
    float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
    Vector vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 13.0f;

    int iShellModel = m_iBrassShell;
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/usp_unsil-1.wav", 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootShotgun( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.13053f, 0.13053f, 0.13053f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 8, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    pev->effects |= EF_MUZZLEFLASH;
    m_cAmmoLoaded--;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
    float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
    Vector vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 8.0f;

    int iShellModel = m_iShotgunShell;
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 2 );

    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/xm1014-1.wav", 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootSMG( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    pev->effects |= EF_MUZZLEFLASH;
    m_cAmmoLoaded--;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
    float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
    Vector vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 10.0f;

    int iShellModel = m_iBrassShell;
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/mac10-1.wav", 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootSniperRifle( void )
{
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin = GetGunPosition();
    Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flAccuracySq = ( m_weaponAccuracy.x * m_weaponAccuracy.x ) + ( m_weaponAccuracy.y * m_weaponAccuracy.y ) + ( m_weaponAccuracy.z * m_weaponAccuracy.z );
    float flLen = sqrtf( flAccuracySq );

    Vector vecSpread;
    if ( flLen <= 0.0f )
    {
        vecSpread = Vector( 0.04362f, 0.04362f, 0.04362f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    pev->effects |= EF_MUZZLEFLASH;

    Vector vecAngles;
    UTIL_VecToAngles( vecShootDir, vecAngles );
    SetBlending( 0, vecAngles.x );

    float flRandRight = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandUp = RANDOM_FLOAT( 75.0f, 200.0f );
    float flRandFwd = RANDOM_FLOAT( 40.0f, 90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + ( gpGlobals->v_up * flRandUp ) + ( gpGlobals->v_forward * flRandFwd );
    Vector vecShellOrigin = vecShootOrigin + gpGlobals->v_forward * 8.0f;

    int iShellModel = m_iBrassShell;
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, iShellModel, 1 );

    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/scout_fire-1.wav", 1.0f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::SpeakSentence( void )
{
    if ( m_iSentence != -1 )
    {
        if ( FOkToSpeak() )
        {
            Speak( pTerroristSentences[m_iSentence], 0.8f, 0.8f, 0, m_voicePitch );
            
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
        }
    }
}

void CCounterTerrorist::StartFollowing( CBaseEntity *pLeader )
{
    if ( m_hEnemy != nullptr )
    {
        m_IdealMonsterState = MONSTERSTATE_ALERT;
    }

    m_hTargetEnt = pLeader;
    m_afConditions &= ~bits_COND_TASK_FAILED;

    CBaseMonster::ClearSchedule();
}

void CCounterTerrorist::StartTask( Task_t *pTask )
{
    m_iTaskStatus = TASKSTATUS_RUNNING;

    switch ( pTask->iTask )
    {
        case 18:
        case 19:
            m_afMemory &= ~bits_MEMORY_KILLED_ENEMY;
            CBaseMonster::StartTask( pTask );
            return;

        case 28:
        case 30:
            if ( m_cantMove < 2 )
            {
                CBaseMonster::StartTask( pTask );
                if ( pev->movetype == MOVETYPE_FLY )
                {
                    m_IdealActivity = ACT_GLIDE;
                }
                return;
            }
            break;

        case 38:
            m_IdealActivity = ACT_RELOAD;
            return;

        case 92:
        case 103:
            return;

        case 93:
            SpeakSentence();
            break;

        case 94:
            if ( !NoFriendlyFire() )
            {
                m_afConditions |= bits_COND_WEAPON_BLOCKED;
            }
            if ( m_afConditions & bits_COND_TASK_FAILED )
            {
                return;
            }
            break;

        case 95:
            if ( m_pSpecialCoverNode == nullptr || !IsValidCover( m_pSpecialCoverNode ) )
            {
                m_afConditions |= bits_COND_TASK_FAILED;
                return;
            }
            if ( !MoveToLocation( ACT_RUN, 0.0f, m_pSpecialCoverNode->m_vecOrigin ) )
            {
                ALERT( at_console, "FAILED MOVING TO LOCATION.\n" );
                m_afConditions |= bits_COND_TASK_FAILED;
                return;
            }
            break;

        case 96:
            if ( m_cantMove == 0 && m_hEnemy != nullptr )
            {
                CBaseEntity *pEnemy = m_hEnemy;
                Vector vecDiff = pEnemy->pev->origin - pev->origin;
                float flDistance = sqrtf( vecDiff.x * vecDiff.x + vecDiff.y * vecDiff.y );

                if ( flDistance >= 64.0f )
                {
                    m_vecMoveGoal = pEnemy->pev->origin;
                    if ( MoveToEnemy( ACT_RUN, 0.0f ) )
                    {
                        return;
                    }
                    m_afConditions |= bits_COND_TASK_FAILED;
                }
            }
            break;

        case 97:
            if ( !MoveToLocation( ACT_WALK, 0.0f, m_wanderOrigin ) )
            {
                m_afConditions |= bits_COND_TASK_FAILED;
            }
            break;

        case 104:
            if ( m_hTalkTarget != nullptr && m_cantMove != 2 )
            {
                pev->yaw_speed = 60.0f;
                CBaseEntity *pTarget = m_hTalkTarget;
                
                Vector vecDir = pTarget->pev->origin - pev->origin;
                float flYaw = CBaseMonster::VecToYaw( vecDir );
                float flDiff = flYaw - pev->angles.y;

                if ( flDiff > 180.0f ) flDiff -= 360.0f;
                if ( flDiff < -180.0f ) flDiff += 360.0f;

                if ( flDiff < 0.0f )
                {
                    float flClamped = ( flDiff + 45.0f < 0.0f ) ? flDiff + 45.0f : 0.0f;
                    pev->ideal_yaw = pev->angles.y + flClamped;
                }
                else
                {
                    float flClamped = ( flDiff - 45.0f > 0.0f ) ? flDiff - 45.0f : 0.0f;
                    pev->ideal_yaw = pev->angles.y + flClamped;
                }
            }
            break;

        default:
            CBaseMonster::StartTask( pTask );
            return;
    }

    if ( !( m_afConditions & bits_COND_TASK_FAILED ) )
    {
        m_iTaskStatus = TASKSTATUS_COMPLETE;
    }
}

void CCounterTerrorist::StopFollowing( BOOL clearSchedule )
{
    if ( m_hTargetEnt != nullptr )
    {
        CBaseEntity *pTarget = m_hTargetEnt;
        if ( pTarget->IsPlayer() )
        {
            if ( m_movementGoal == MOVEGOAL_TARGETENT )
            {
                RouteClear();
            }

            m_hTargetEnt = nullptr;

            if ( clearSchedule )
            {
                CBaseMonster::ClearSchedule();
            }

            if ( m_hEnemy != nullptr )
            {
                m_IdealMonsterState = MONSTERSTATE_COMBAT;
                return;
            }
        }
    }
}

int CCounterTerrorist::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
    m_afMemory &= ~bits_MEMORY_KILLED_ENEMY;

    if ( pevAttacker != nullptr )
    {
        CBaseEntity *pAttacker = CBaseEntity::Instance( pevAttacker->pContainingEntity );
        if ( pAttacker != nullptr && pAttacker->IsPlayer() )
        {
            Speak( "CT_SHOT", 0.8f, 0.8f, 0, m_voicePitch );
            
            CTalkMonster::g_talkWaitTime = gpGlobals->time + 1.0f;
            m_iSentence = -1;
            m_flNextPainTime = gpGlobals->time + 2.0f;
        }
    }

    return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CCounterTerrorist::Talk( float flDuration )
{
    m_flStopTalkTime = gpGlobals->time + flDuration;
}

void CCounterTerrorist::Touch( CBaseEntity *pOther )
{
    m_wanderTime = 0.0f;
}

void CCounterTerrorist::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
    if ( ptr != nullptr && ptr->iHitgroup == 11 )
    {
        int iHelmetGroup = GetBodygroup( 1 );
        if ( iHelmetGroup == 0 && ( bitsDamageType & ( DMG_BULLET | DMG_CLUB ) ) )
        {
            flDamage -= 20.0f;
            if ( flDamage <= 0.0f )
            {
                UTIL_Ricochet( ptr->vecEndPos, 1.0f );
                flDamage = 0.01f;
            }
            ptr->iHitgroup = 1;
        }
    }

    CBaseMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

void CCounterTerrorist::UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    if ( m_useTarget == iStringNull )
    {
        return;
    }

    SUB_UseTargets( pActivator, useType, value );
}

bool CCounterTerrorist::ValidateActivation( int entry )
{
    if ( entry == 18 || entry == 50 || entry == 64 || entry == 66 )
    {
        return ( m_cantMove == 0 );
    }

    return true;
}

