#ifndef TERRORIST_H
#define TERRORIST_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "squadmonster.h" // 👈 載入標準小隊怪物頭文件

// =========================================================================
// 🔓 DWARF 終極正名宣告：戰術恐怖分子小隊母體 (monster_terrorist -> CTerrorist)
// =========================================================================
class CTerrorist : public CSquadMonster // 👈 精確更正繼承自 CSquadMonster
{
public:
    void Spawn( void ) override;
    void Precache( void ) override; 
    void KeyValue( KeyValueData *pkvd ) override;
    
    // ⚠️ 正式將本回解鎖的衍生初始化虛擬函數註冊進標頭檔
    void SpawnInit( void ); // 👈 完美宣告歸位！

    int  Classify( void ) override { return 1; } // CLASS_TERRORIST / CLASS_MILITARY
    int  BloodColor( void ) override { return BLOOD_COLOR_RED; } 

    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

private:
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
    ScheduleEntry  scheduleTable[16];    // 👈 完美宣告歸位！精確鎖定在 m_flNextGrenadeCheck 上方
    int  GetBehaviorType( void );
    void SetupWeapons( void );
    void SetupBehaviors( int behaviorType );
    void SetupLaserGlow( void );
    void UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    static TYPEDESCRIPTION m_SaveData[]; 
};

#endif // TERRORIST_H
