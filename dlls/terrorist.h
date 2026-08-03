#ifndef TERRORIST_H
#define TERRORIST_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "squadmonster.h"
#include "effects.h"

class CTerrorist : public CSquadMonster // 👈 精確更正繼承自 CSquadMonster
{
public:
    void Spawn( void ) override;
    void Precache( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    void SpawnInit( void );
	void ActivateTableEntry( int entry );
	void ClearScheduleTableFlags( void ); 
	int  CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel ) override;
	BOOL CheckMeleeAttack1( float flDot, float flDist ) override;
	BOOL CheckRangeAttack1( float flDot, float flDist ) override;
	BOOL CheckRangeAttack2( float flDot, float flDist ) override;
	Schedule_t* GetSchedule( void ) override;
	Schedule_t* GetScheduleOfType( int Type ) override;
	void GibMonster( void ) override;
	void HandleAnimEvent( MonsterEvent_t *pEvent ) override;
	void IdleSound( void ) override;
	void JustSpoke( float nextSpeakTime );
	int  IRelationship( CBaseEntity *pTarget ) override;
	int  ISoundMask( void ) override;
	BOOL FCanCheckAttacks( void ) override;
	Vector GetGunPosition( void ) override;
	void DeathSound( void ) override;
    int  Classify( void ) override { return m_classtype; } 
    int  BloodColor( void ) override { return BLOOD_COLOR_RED; } 

    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

private:
	CBaseEntity* Kick( void );
    void ShootPistol( void );
    void ShootShotgun( void );
    void ShootAssaultRifle( void );
    void ShootMachineGun( void );
    void ShootSniperRifle( void );
    void ShootLAW( void );
    void ShootSMG( void );
    void ShootMP5( void );
    int            m_voicePitch;         
    int            m_iBrassShell;        
    int            m_iShotgunShell;      
    unsigned short m_usFireM60;          
    unsigned short m_usFireTMP;          
    unsigned short m_usFireGlock;        
    unsigned short m_usFireAK47;         
    unsigned short m_usFireAWP;          
    unsigned short m_usFireMP5;          
    unsigned short m_usFireShotgun;
    int            m_terroristBehavior;  // 戰術兵種型態代號 (0=突擊, 4/5=狙擊手等)
    float          m_flNextGrenadeCheck; // 下一次發射戰術手榴彈的冷卻線
    int            m_iSentence;          // 敵軍小隊喊話/無線電台詞句索引
    BOOL           m_fFirstEncounter;    // 標記是否為開局首次目擊玩家 (用於引爆受驚大喊劇本)
    ScheduleEntry  scheduleTable[16];
    BOOL           m_fThrowGrenade;      // 標記目前大腦是否判定拋物線暢通、決定投擲
    Vector         m_vecTossVelocity;
	CBeam*         m_pBeam;              // 指向動態生成的 3D 雷射射線實體物件指針
    CSprite*       m_pLaserGlow;         // 指向槍口紅外線點精靈貼圖特效物件指針
    string_t       m_dropItem;           // 擊殺死後機率掉落的自定義實體武器名 (如 weapon_ak47)
    float          m_dropChance;         // 掉落機率折算加權面 (0.0f ~ 1.0f)
    Vector         m_wanderOrigin;       // 8方向隨機漫步逃逸原始幾何座標
    int  GetBehaviorType( void );
    void SetupWeapons( void );
    void SetupBehaviors( int behaviorType );
	int  GetBehaviorTypeSchedule( void );
	int  GetFlyingSchedule( void );
	int  GetForcedTargetSchedule( void );
	int  GetHearingSchedule( void );
	int  GetWeaponEffectSchedule( void );
	int  GetScheduleFromTable( void );
	void HandleSpeaking( int schedule );
	void InitScheduleTable( void );
	BOOL IsPlayerHidden( void );
    void SetupLaserGlow( void );
	void LaserGlowOff( void );
	void CheckAmmo( void );
	BOOL FOkToSpeak( void );
    void UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    BOOL           m_bStanding;          // 是否處於常規站立待命姿態
    int            m_terroristType;      // 敵軍基礎兵種細分型態
    BOOL           m_flashbangOnly;      // 戰術硬限制：是否只允許攜帶投擲閃光彈
    BOOL           m_heGrenadeOnly;      // 戰術硬限制：是否只允許攜帶投擲高爆手榴彈
    float          m_blastRadius;        // 人肉炸彈兵種專屬：自爆衝擊波幾何半徑 (256.0)
    float          m_blastDamage;        // 人肉炸彈兵種專屬：自爆最大致命傷害 (200.0)
    BOOL           m_canWander;          // 是否允許發動開局無目的日常閒逛尋路
    BOOL           stealthDeath;         // 潛行背刺暗殺關卡專用：安靜死亡不驚動同伴標記
    float          taskFailCount;        // 高級尋路戰術任務連續失敗硬停機計數器
    BOOL           suicideFlag;          // 是否已決定引爆人肉炸彈衝向玩家旗標
    float          lawDelayTime;         // M72 LAW 重型火箭筒開火戰術冷卻延時線
    BOOL           justShotFlag;         // 是否剛剛開槍打完一個連發週期的開火暫存器
    float          m_alertRadius;        // 小隊開槍/受驚無線電全局連動戰術警戒半徑 (256.0)
    float          m_fPreferedRange;     // 核心槍械火網覆蓋黃金親密距離 (64.0)
    BOOL           heardCombat;          // 是否聽到了側面走廊傳來的戰術交火戰鬥聲響
    float          m_wanderTime;         // 5~10秒交叉錯落的下一次隨機漫步閒逛時鐘線
    static TYPEDESCRIPTION m_SaveData[]; 
};

#endif // TERRORIST_H
