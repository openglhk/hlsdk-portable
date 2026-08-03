#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h" 

class CTriggerEndMission : public CBaseToggle
{
public:
    void Spawn( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    
    void EndMissionUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
    void EndMissionTouch( CBaseEntity *pOther );
    void ChangeLevelThink( void ); 

    CTriggerEndMission();

private:
    char m_szNextMap[64]; 
    int  m_mp3Name; // DWARF 符號證實的 mp3Name 成員變數 (在 HLSDK 中字串通常儲存為 string_t / int)

    // ?? 完美還原：原廠 DWARF 暴露的自訂變數成員
    float duration;   // 黑幕淡出秒數 (對應 0x40a00000 = 5.0f)
    float holdtime;   // 黑幕停留秒數 (對應 0x40400000 = 3.0f)
    float alphaamt;   // 黑幕最高 Alpha 透明度 (對應 0x437f0000 = 255.0f)
    float r, g, b;    // 黑幕顏色 RGB 欄位
    
    unsigned short m_usHolsterEvent; // 銲槍收回事件快取
};