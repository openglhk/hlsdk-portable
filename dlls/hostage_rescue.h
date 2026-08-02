#ifndef HOSTAGE_RESCUE_H
#define HOSTAGE_RESCUE_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"

// =========================================================================
// 🔓 究極無死角大圓滿宣告：人質營救與戰術雷達小地圖同步中樞 (CHostageRescue)
// =========================================================================
class CHostageRescue : public CPointEntity 
{
public:
    CHostageRescue(); 

    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override; 
    
    // 🔃 狂暴每幀極速雷達探測 Think
    void RescueThink( void ); 
    
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

private:
    // =========================================================================
    // 📊 100% 與二進位完全重合、佔用 116 位元組的終極私有成員變數 (4位元組再對齊)
    // =========================================================================
    float          m_flNextSearchTime;   // 2.0f 探測冷卻鎖時鐘
    int            m_iRescueType;        // 7 = RESCUE_HOSTAGE 營救類型
    int            m_iRescuedCount;      // 已成功營救人質計數器
    BOOL           m_bQuiet;             // 潛行關卡安靜營救模式
    BOOL           m_bHidden;            // 隱藏雷達與戰術 HUD 位置提示
    
    // ⚠️ 終極解鎖：RescueThink 曝光的玩家近身隨行標記
    BOOL           m_bPlayerNear;        // 👈 補上這行：標記目前玩家是否已進入 256 碼雷達探測圈

    static TYPEDESCRIPTION m_SaveData[]; 
};

#endif // HOSTAGE_RESCUE_H
