#ifndef TRIGGER_CHANGEKEYVALUE_H
#define TRIGGER_CHANGEKEYVALUE_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"

// =========================================================================
// 🔓 終極大圓滿宣告：動態多重屬性批次改寫觸發器 (CTriggerChangeKeyValue)
// =========================================================================
class CTriggerChangeKeyValue : public CBaseDelay
{
public:
    CTriggerChangeKeyValue(); // 構造函數

    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override; // 👈 本回解鎖的批次解析大腦
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	int  ObjectCaps( void ) override;
	
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

private:
    // =========================================================================
    // 📊 100% 與 Linux server.so 毫米不差、完全重合的 236 位元組自定義變數
    // =========================================================================
    string_t       m_changeTargetName;   // 欲進行批次強行修改的目標地圖實體群組名稱 (對齊 changetarget)
    int            m_cTargets;           // 目前已成功捕獲並加載的自定義改寫屬性總對數 (上限 16)
    
    // ⚠️ 終極解鎖：16 容量動態改寫屬性與數值字串池指針矩陣 (16 * 4 * 2 = 128 位元組)
    string_t       m_iKey[16];           // 儲存 16 組欲修改的屬性關鍵字 (KeyNames)
    string_t       m_iValue[16];         // 儲存 16 組欲覆蓋寫入的新數值 (Values)

    static TYPEDESCRIPTION m_SaveData[]; // 存檔數據表
};

#endif // TRIGGER_CHANGEKEYVALUE_H
