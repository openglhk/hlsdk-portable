#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "squadmonster.h"

class CCounterTerrorist : public CSquadMonster
{
public:
    void Spawn( void ) override;
	void SpawnInit( void );
    void Precache( void ) override;
    void ActivateTableEntry( int entry ); 
    BOOL CanFollow( void ) override;
	int  CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel ) override;
	void CheckAmmo( void ) override;
	BOOL CheckMeleeAttack1( float flDot, float flDist ) override;
	BOOL CheckRangeAttack1( float flDot, float flDist ) override;
	BOOL CheckRangeAttack2( float flDot, float flDist ) override;
	int  Classify( void ) override;
	void ClearScheduleTableFlags( void );
	void DeathSound( void ) override;
	BOOL FCanCheckAttacks( void ) override;
	BOOL FOkToSpeak( void );
	void FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int  GetBehaviorType( void );
	int  GetFlyingSchedule( void ) override;
	int  GetForcedTargetSchedule( void );
	Vector GetGunPosition( void ) override;
	int  GetHearingSchedule( void ) override;
	Schedule_t * GetSchedule( void ) override;
	Schedule_t * GetScheduleOfType( int Type ) override;
	int GetScheduleFromTable( void );
	int GetWeaponEffectSchedule( void );
	void GibMonster( void ) override;
	void HandleAnimEvent( MonsterEvent_t *pEvent ) override;
	Schedule_t * HandleFollowing( void );
	Schedule_t * HandleIdleHeadWatch( void );
	Schedule_t * ScheduleFromName( char *pName ) override;
	void HandleSpeaking( int schedule );
	void IdleHeadTurn( Vector *vecFriend );
	void IdleSound( void ) override;
	void InitScheduleTable( void );
	int  ISoundMask( void ) override;
	BOOL IsPlayerHidden( void );
	BOOL IsTalking( void );
	void JustSpoke( float nextSpeakTime );
	void KeyValue( KeyValueData *pkvd ) override;
	CBaseEntity * Kick( void );
	void LaserGlowOff( void );
	void LaserGlowOn( void );
	void LimitFollowers( CBaseEntity *pPlayer, int maxFollowers );
	#define ACT_IDLE 1 
    int  LookupActivity( int activity ) override;
	int  LookupActivityHeaviest( int activity ) override;
	int  ObjectCaps( void ) override;
	void PainSound( void ) override;
	void PlayScriptedSentence( char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener ) override;
	void PrescheduleThink( void ) override;
	void ResetHeadWatch( void );
	int  Restore( CRestore *restore ) override;
	int  Save( CSave *save ) override;
	void StartTask( Task_t *pTask ) override;
	void RunTask( Task_t *pTask ) override;
	#define ACT_WALK_HURT 48 // 註冊殘血跛行走路枚舉
    #define ACT_RUN_HURT  49 // 註冊殘血跛行跑步枚舉
    void SetActivity( Activity NewActivity ) override;
	int  SetActivityAutomaticBase( Activity NewActivity );
	int  SetActivityGrenadeBase( Activity NewActivity );
	int  SetActivityLAWBase( Activity NewActivity );
	int  SetActivityPistolBase( Activity NewActivity );
	int  SetActivityShotgunBase( Activity NewActivity );
	#define ACT_ARM    29 // 註冊戰術雷射啟動活動枚舉
    #define ACT_DISARM 30 // 註冊戰術雷射關閉活動枚舉
    int  SetActivitySniperBase( Activity NewActivity );
	void SetupBehaviors( int type );
	void SetupLaserGlow( void );
	void SetupWeapons( void );
	void SetYawSpeed( void ) override;
	void ShootAssaultRifle( void );
	void ShootLAW( void );
	void ShootMachineGun( void );
	void ShootMP5( void );
	void ShootPistol( void );
	void ShootShotgun( void );
	void ShootSMG( void );
	void ShootSniperRifle( void );
	void SpeakSentence( void );
	void StartFollowing( CBaseEntity *pLeader );
	void StopFollowing( BOOL clearSchedule );
	int  TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType ) override;
	void Talk( float flDuration );
	void Touch( CBaseEntity *pOther ) override;
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) override; 
	void UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	bool ValidateActivation( int entry );
public:
	Vector m_wanderOrigin;
	int m_iShotgunShell;
	int m_iBrassShell;
	Vector m_weaponAccuracy; // 🔒 100% 精確對齊二進位內存排位之當前武器高動態彈道擴散暫存器
    int    m_iShellId;        // 🔒 100% 精確對齊二進位內存排位之黃銅步槍拋殼模型 .mdl 
	int    m_iAssaultRifleType; // 🔒 100% 精確對齊二進位內存之當前手持突擊步槍槍械型態 ID
    int    m_iShellModelIndex;  // 🔒 100% 精確對齊二進位內存之 3D 黃銅彈殼模型緩衝索引
	int m_idleAnim;
    int m_deathAnim;
	Schedule_t* m_scheduleList[41];
	float m_laserBrightness;
    float m_glowBrightness;
	float   m_flStareTime;
    EHANDLE m_hTalkTarget;
	int   m_cClipSize;
    int   justShotFlag;
    int   m_dropItem;
    float m_dropChance;
	int m_iWeaponEffectType;
    int m_bWeaponEffectActive;
	float m_fFirstEncounter;
    float taskFailCount;
	float m_flStopTalkTime;
    float m_wanderTime;
    int   m_canWander;
    int   m_iSentence;
	BOOL m_bStanding;
    int m_cantMove;
    int m_iId;
    ScheduleEntry scheduleTable[74];
    EHANDLE m_hTargetEnt;
private:
    static TYPEDESCRIPTION m_SaveData[];
};

class CCounterTerroristGIGN : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

class CCounterTerroristGSG9 : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

class CCounterTerroristSAS : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

class CCounterTerroristSpetsnaz : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

class CCounterTerroristSwat : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

class CCounterTerroristRepel : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    int  Save( CSave *save ) override;
    int  Restore( CRestore *restore ) override;
    void Spawn( void ) override;
    void RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    
public:
    int m_iSpriteTexture;
    int m_iSkin;
    int m_iHead;
    int m_classname;
    static TYPEDESCRIPTION m_SaveData[];
};

