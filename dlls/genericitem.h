#ifndef GENERICITEM_H
#define GENERICITEM_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"

// =========================================================================
// 🔓 新實體類別：通用互動道具/任務物件 (item_generic -> CGenericItem)
// =========================================================================
class CGenericItem : public CBaseAnimating
{
public:
    void Spawn( void ) override;
    void Precache( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;

    // 💾 存讀檔序列化系統 (完全對齊先前破譯的 Save/Restore 規範)
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

    // ⚙️ 內部 Think 延時核心方法
    void StartItem( void );

private:
    // =========================================================================
    // 📊 記憶體對齊與存檔所必需的核心自定義變數
    // =========================================================================
    BOOL           m_bCanSpawn;     // 標記目前道具是否成功在合法的規則下出生
    string_t       m_iSequence;     // 儲存地圖指定的預設 MDL 動畫名稱字串索引
    int            m_iLightValue;   // 儲存地圖指定的戰術螢光特效亮度值

    static TYPEDESCRIPTION m_SaveData[]; // 存檔對射數據表
};

#endif // GENERICITEM_H
