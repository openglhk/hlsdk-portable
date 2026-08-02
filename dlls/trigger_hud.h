#ifndef TRIGGER_HUD_H
#define TRIGGER_HUD_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"

// =========================================================================
// 🔓 終極正名宣告：戰術 HUD 畫面特效控制觸發器 (CTriggerHud)
// =========================================================================
class CTriggerHud : public CBaseTrigger // 👈 根據二進位密碼精確更正繼承自 CBaseTrigger
{
public:
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override; // 👈 本回破譯的核心屬性大腦
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void TriggerHudUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;

private:
    BOOL           m_display;            // 👈 完美正名歸位！HUD 畫面特效顯示/隱形開關控制旗標
    static TYPEDESCRIPTION m_SaveData[]; // 存檔數據表
};

#endif // TRIGGER_HUD_H