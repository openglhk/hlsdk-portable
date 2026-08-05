#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class CLawsAmmo : public CBasePlayerAmmo
{
public:
    // 1. 基底實體虛擬表接口覆蓋
    void Spawn(void) override;
    void Precache(void) override;

    // 2. 玩家踩過時的拾取與上限分流判定
    BOOL AddAmmo(CBaseEntity *pOther) override;
};

void CLawsAmmo::Precache(void)
{
    PRECACHE_MODEL("models/w_lawsammo.mdl");
    PRECACHE_SOUND("items/9mmclip1.wav");
}

void CLawsAmmo::Spawn(void)
{
    Precache();
    SET_MODEL(ENT(pev), "models/w_lawsammo.mdl");

    pev->movetype = MOVETYPE_FLY; // 6：單人模式彈藥箱不產生重力下墜
    pev->solid = SOLID_TRIGGER;    // 1：設定為接觸觸發器，玩家踩過去即可撿起

    UTIL_SetSize(pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 16.0f));
    UTIL_SetOrigin(pev, pev->origin);

    // 綁定 Half-Life SDK 經典的通用彈藥碰觸回呼函數
    SetTouch(&CBasePlayerAmmo::DefaultTouch);

    // 🚨 CZDS（刪除片段）單人任務獨家引擎渲染優化 🚨
    // 檢查地圖亮度倍率控制變數 "lightmultiplier"
    if (CVAR_GET_FLOAT("lightmultiplier") >= 0.1f)
    {
        pev->renderfx = kRenderFxGlowShell; // 0x15：在陰暗關卡中為火箭彈藥箱加上一層微弱的邊緣發光特效
        pev->iuser4 = (int)CVAR_GET_FLOAT("lightmultiplier"); // 將環境亮度權重暫存入 iuser4 供底層著色器動態縮放發光強度
    }
}

BOOL CLawsAmmo::AddAmmo(CBaseEntity *pOther)
{
    // 💡 虛擬函數表索引 [0x1b] 精確對齊了玩家的 CBasePlayer::GiveAmmo 接口
    // 給予玩家編號為 5 的 66mm 火箭彈藥，數量為 1 發（&DAT_0017ca84）
    int iCount = pOther->GiveAmmo(1, "66mm", 5);

    // 如果 GiveAmmo 返回 -1，代表玩家身上的火箭彈藥庫已經滿載（iMaxAmmo1 = 1），此時不予拾取
    if (iCount != -1)
    {
        // 拾取成功，播放原版 Half-Life 經典的彈藥進包清脆音效
        EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);
        return TRUE;
    }

    return FALSE;
}
