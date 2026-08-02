#ifndef GENERICITEM_H
#define GENERICITEM_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"

class CGenericItem : public CBaseMonster // 👈 根據二進位密碼精確更正為繼承自 CBaseMonster
{
public:
    CGenericItem();

    void Spawn( void ) override;
    void Precache( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;
    void Restart( void );
    void StartItem( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override; // 👈 完美宣告歸位！
    void AnimateThink( void );


private:
    BOOL           m_bCanSpawn;       // 標記目前道具是否成功在合法的規則下出生
    string_t       m_iSequence;       // 儲存地圖指定的預設 MDL 動畫名稱字串索引
    int            m_iLightValue;     // 儲存地圖指定的戰術螢光特效亮度值
    float          m_lastTime;        // 記錄上一次動畫步進的絕對時間戳
    BOOL           m_removeOnUse;     // 標記在被玩家使用後是否自動銷毀移除
    BOOL           m_bSetPVSDefault;  // PVS 網路動態可見性預設優化旗標
    BOOL           m_nopvs;           // 是否對此道具實施 PVS 可見性裁剪
    static TYPEDESCRIPTION m_SaveData[]; // 存檔數據表
};

#endif // GENERICITEM_H
