#pragma once
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "squadmonster.h" 
#include "schedule.h"

// =========================================================================
// 🚀 外部全局計畫物件與台詞池宣告歸位
// =========================================================================
extern Schedule_t slCTIdleEyecontact;
extern Schedule_t slCTCombatFace;
extern Schedule_t slCTVictoryDance;
extern Schedule_t slCTFaceTarget;
extern Schedule_t slCTFollow;
extern Schedule_t slCTTakeCover;
extern Schedule_t slCTTakeCoverFromBestSound;
extern Schedule_t slCTRangeAttack1A;
extern Schedule_t slCTRangeAttack1B;
extern Schedule_t slCTRangeAttack1C;
extern Schedule_t slCTRangeAttackMG2;
extern Schedule_t slCTRangeAttack2;
extern Schedule_t slCTCombatFail;
extern Schedule_t slCTFail;
extern Schedule_t slCTWander;
extern Schedule_t slCTSuppress;
extern Schedule_t slCTSignalSuppress;
extern Schedule_t slCTEstablishLineOfFire;
extern Schedule_t slCTHideReload;
extern Schedule_t slCTSweep;
extern Schedule_t slCTFoundEnemy;
extern Schedule_t slCTRepel;
extern Schedule_t slCTRepelAttack;
extern Schedule_t slCTRepelLand;
extern Schedule_t slCTWaitInCover;
extern Schedule_t slCTInSmoke;
extern Schedule_t slCTFlashed;
extern Schedule_t slCTSniperAttack;
extern Schedule_t slCTSniperTakeShot;
extern Schedule_t slCTCornerCoverNode;
extern Schedule_t slCTDuckCoverNode;
extern Schedule_t slCTDuckNodeAttack;
extern Schedule_t slCTCloseIn;
extern Schedule_t slCTCornerNodeAttack;
extern Schedule_t slCTFiringPause;
extern Schedule_t slCTStopFollowing;
extern Schedule_t slCTWaitCrouched;
extern const char* pTerroristSentences[]; 

// 🎯 核心記憶體對齊結構體
struct ScheduleEntry {
    float activeFlag;      
    float percentChance;
    float normalizedChance;
};

// 💡 提示：g_fCTQuestion 變數已被移至 ct_swat.cpp 頂部，徹底根絕重複定義編譯 Bug

// 前置宣告潛行觸發器類別，防止編譯器報錯
class CTriggerStealth {
public:
    static BOOL IsPlayerHidden( CTriggerStealth *pTrigger );
};

// =========================================================================
// 💂‍♂️ 主體類別宣告：反恐精英小隊特警 (CMonsterCtSwat)
// =========================================================================
class CMonsterCtSwat : public CSquadMonster
{
public:
    CMonsterCtSwat();

    // 基礎虛擬方法覆寫 (Engine / Base Framework 回調)
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;
    int  ObjectCaps( void ) override;
    void Touch( CBaseEntity *pOther ) override; 
    void UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    void Precache( void ) override; 
    int  Classify( void ) override { return m_classtype; }
    int  ISoundMask( void ) override;
    void CheckAmmo( void ) override;
    void DeathSound( void ) override;
    void PainSound( void ) override;
    int  TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType ) override;
    void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) override;
    void PlayScriptedSentence( char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener ) override;
    void Talk( float flDuration ) override; 
    void IdleSound( void ) override;
    void GibMonster( void ) override;
    Vector GetGunPosition( void ) override;
    void IdleHeadTurn( Vector *vecFriend ) override;
    void ResetHeadWatch( void );

    // 戰術與攻擊評估方法覆寫
    BOOL CheckMeleeAttack1( float flDot, float flDist ) override;
    BOOL CheckRangeAttack1( float flDot, float flDist ) override;
    BOOL CheckRangeAttack2( float flDot, float flDist ) override;
    int  CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel ) override;
    void FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
    void StartFollowing( CBaseEntity *pLeader ) override;
    void StopFollowing( BOOL clearSchedule ) override; 
    void HandleAnimEvent( MonsterEvent_t *pEvent ) override;
    int  LookupActivity( int activity ) override;
    int  LookupActivityHeaviest( int activity ) override;
    void SetActivity( Activity NewActivity ) override;
    void SetYawSpeed( void ) override;
    
    // 自定義戰術配置工廠接口
    void SpawnInit( void ); 
    void ActivateTableEntry( int entry );
    void ClearScheduleTableFlags( void );
    void InitScheduleTable( void );
    void SetupBehaviors( int type );
    void SetupWeapons( void );
    bool ValidateActivation( int entry );
    BOOL FOkToSpeak( void );
    BOOL FCanCheckAttacks( void ) override;
    BOOL IsPlayerHidden( void );
    BOOL CanFollow( void ); 
    BOOL IsTalking( void ) override;
    void PrescheduleThink( void ) override;
    Schedule_t* GetSchedule( void ) override;
    Schedule_t* GetScheduleOfType( int Type ) override;
    Schedule_t* ScheduleFromName( const char *pName ) override;
    void StartTask( Task_t *pTask ) override;
    void RunTask( Task_t *pTask ) override;
    
private:
    // ⚔️ 輕重武器骨骼動畫子系統映射接口
    int   SetActivityPistolBase( Activity NewActivity );
    int   SetActivityShotgunBase( Activity NewActivity );
    int   SetActivityAutomaticBase( Activity NewActivity );
    int   SetActivitySniperBase( Activity NewActivity );
    int   SetActivityLAWBase( Activity NewActivity );
    int   SetActivityGrenadeBase( Activity NewActivity );
    float GetActivityYawSpeed( Activity NewActivity );

    // 📡 計畫決策樹與戰術分流子模組
    int   GetBehaviorType( void );
    int   GetForcedTargetSchedule( void );
    int   GetFlyingSchedule( void );
    int   GetHearingSchedule( void );
    int   GetScheduleFromTable( void );

    // 🔴 狙擊手紅外線雷射特效裝配接口
    void  LaserGlowOn( void );
    void  LaserGlowOff( void );
    void  SetupLaserGlow( void ); 

    // 戰術降噪與無線電排隊發射器
    void  LimitFollowers( CBaseEntity *pPlayer, int maxFollowers );
    void  HandleSpeaking( int schedule );
    void  JustSpoke( float nextSpeakTime );
    void  SpeakSentence( void );
    int   GetWeaponEffectSchedule( void );
    Schedule_t* HandleFollowing( void );

    // 近戰飛踢與全武武裝射擊火網子矩陣
    CBaseEntity* Kick( void );
    void  ShootPistol( void );
    void  ShootShotgun( void );
    void  ShootAssaultRifle( void );
    void  ShootMachineGun( void );
    void  ShootSniperRifle( void );
    void  ShootLAW( void );
    void  ShootSMG( void );
    void  ShootMP5( void );

    // =========================================================================
    // 🔓 記憶體對齊封裝：必須與 Linux server.so 二進位空間毫釐不差！
    // =========================================================================
    int            m_ctBehavior;       
    int            m_behaviorType;     
    
    // ⚠️ 記憶體核心對齊陣列：精確排在 m_flNextGrenadeCheck 之前！
    ScheduleEntry  scheduleTable[74];  

    float          m_flNextGrenadeCheck; 
    float          m_flNextPainTime;
    int            m_iSentence;
    int            m_fFirstEncounter;
    float          m_healthMultiplier; 

    int            m_ctType;         
    string_t       m_language;       
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

    // 投擲物拋射評估暫存器
    BOOL           m_fThrowGrenade;    
    Vector         m_vecTossVelocity;  
    
    // 陣營與地圖強制目標
    int            m_classtype;        
    string_t       m_forcedTarget;     
    
    // 動態姿態與日常巡邏冷卻
    BOOL           m_bStanding;
    float          m_flStopTalkTime;   
    float          m_wanderTime;       
    BOOL           m_canWander;        
    Vector         m_wanderOrigin;     
    
    // 戰術推進與小隊索敵警報標記
    BOOL           justShotFlag;       
    float          m_fPreferedRange;   
    float          taskFailCount;      
    
    // 👀 眼神對視、小隊共用目擊時鐘與跟隨狀態
    BOOL           m_bFollowStuck;     
    int            m_iWeaponEffectType;   
    BOOL           m_bWeaponEffectActive; 

    // =========================================================================
    // 🔓 🚀 終極修復：以下為填補被截斷缺失的最後核心拼圖，達成大圓滿閉環！
    // =========================================================================
    int            m_cClipSize;            // 長短槍標準彈匣最大容量基數
    string_t       m_dropItem;             // 死亡時脫手掉落的裝備武器名稱字串索引
    float          m_dropChance;           // 裝備掉落的浮點數機率 (0.0~1.0)
    float          m_flLastEnemySightTime; // 全小隊共用的敵人最後目擊時間戳
    BOOL           m_fEnemyEluded;         // 標記目前敵人是否已從整個小隊視野中逃脫丟失
    float          m_flStareTime;          // 眼神注視冷卻硬性截止時間戳
    float          m_laserBrightness;      // 狙擊手紅外線雷射光束的動態渲染亮度
    float          m_glowBrightness;       // 槍口真實附著紅點精靈的動態渲染亮度
    string_t       m_useTarget;            // 儲存與地圖編輯器對接的遠程事件喚醒目標名稱

    // 28 欄位存檔序列化數據表宣告
    static TYPEDESCRIPTION m_SaveData[];   
    
    // 41 容量自定義計畫清單陣列
    static Schedule_t     *m_scheduleList[41]; 
    
    // 長槍連續開火動態散佈圓錐向量
    Vector         m_weaponAccuracy;       
};
