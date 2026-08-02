#ifndef TRIGGER_OBJECTIVE_H
#define TRIGGER_OBJECTIVE_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"

// =========================================================================
// 🔓 終極大圓滿宣告：戰術任務目標觸發器 (CTriggerObjective)
// =========================================================================
class CTriggerObjective : public CBaseTrigger // 👈 根據二進位密碼精確更正繼承自 CBaseTrigger
{
public:
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void ObjectiveUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ); // 👈 完美宣告歸位！
	int  ObjectCaps( void ) override; // 👈 完美宣告覆寫歸位！
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

    // 高級 HUD 多任務動態渲染排版函數
    BOOL DisplayObjective( int lineCount ); 

private:
    // =========================================================================
    // 📊 100% 與 Linux server.so 二進位完全對齊、無盲區的 0x21C 私有成員變數
    // =========================================================================
    int            m_iObjectiveID;       // 任務自定義 ID 編號
    float          m_flDisplayDuration;  // 任務 HUD 顯示持續時長 (預設 30.0)
    int            objectiveState;       // 任務狀態：0=未激活, 1=進行中, 2=已完成
    string_t       objectiveText;        // 任務描述本地化字串池索引
    BOOL           m_bTransition;        // 標記在跨越 Changelevel 切換地圖時是否保留任務狀態

    static TYPEDESCRIPTION m_SaveData[]; // 4 欄位存檔數據表
};

#endif // TRIGGER_OBJECTIVE_H
