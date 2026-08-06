#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CBriefcase : public CBasePlayerWeapon
{
public:
    // 1. 基底核心虛擬表接口覆蓋
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;       // 固定返回 Slot 5 (與 C4 / 割炬共用的劇情配件欄位)
    float GetMaxSpeed(void) override;   // 動態步速：傳輸情報時 1.0f 物理死鎖，常規提包行軍 225.0f

    BOOL Deploy(void) override;         // 核心防線：保存並還原 m_iIncognito 特工隱蔽偽裝值
    void Holster(int skiplocal = 0) override; // 強制推入狀態 1 並灌入 0.833 秒合上密碼鎖動畫硬直
    void Reload(void) override;         // 完全留空直接 return，無限耐久道具
    void WeaponIdle(void) override;     // 5.73 秒傳輸結束後向地圖發射 USE_SET 訊號，抹除 AccessoryBits 並呼叫 DestroyItem 自毀

    void PrimaryAttack(void) override;   // 核心火控：對準 CTriggerUseTool 區域則將狀態推向 3，否則拉起藍白 HUD 警告

    // 2. ?? CZDS 客製化多維狀態機中央分流矩陣 ??
    void SetWeaponState(unsigned short weaponState); // 狀態 3 實施 5.733 秒大硬直與 pfnSetClientMaxspeed 強殺 1.0f 步速

public:
    // 3. 私有與成員變數欄位 (精確填補並完全閉環 0x11c = 284 位元組的私有數據長度)
    // 儲存於 field_0x10e 記憶體偏移量的開火事件快取
    unsigned short m_usFireBriefcase; 
};

LINK_ENTITY_TO_CLASS(weapon_briefcase, CBriefcase);

void CBriefcase::Precache(void)
{
    PRECACHE_MODEL("models/v_briefcase.mdl");
    PRECACHE_MODEL("models/w_briefcase.mdl");

    PRECACHE_SOUND("weapons/briefcase_use.wav");
    PRECACHE_SOUND("weapons/briefcase_deploy.wav");

    m_usFireBriefcase = PRECACHE_EVENT(1, "events/briefcase.sc"); // 0x10e 偏移量對齊
}

void CBriefcase::Spawn(void)
{
    SET_MODEL(ENT(pev), "models/w_briefcase.mdl");
    pev->frame = 0.0f;
    pev->body = 3;
    pev->sequence = 0;
    pev->framerate = 0.0f;
    pev->classname = MAKE_STRING("weapon_briefcase");

    Precache();

    m_iId = WEAPON_BRIEFCASE; // 0x16 = 十進位 22
    m_iDefaultAmmo = WEAPON_NOCLIP;
    m_iClip = WEAPON_NOCLIP;
    m_bIsAccessory = true; // 核心定位：屬於任務專用特殊戰術配件

    FallInit();

    pev->nextthink = UTIL_WeaponTimeBase() + 0.1f;
}

BOOL CBriefcase::Deploy(void)
{
    // ?? 絕密潛入核心防線：保存玩家當前的「偽裝/隱蔽潛行」狀態位元 ??
    int iOriginalIncog = m_pPlayer->m_iIncognito;

    // 載入公事包 V 模型，播放 Animation 1（單手提包、解開鎖扣拔槍動畫），套用單手持包骨骼標籤 "onehanded"
    BOOL bResult = DefaultDeploy("models/v_briefcase.mdl", "models/p_briefcase.mdl", 1, "onehanded", UseDecrement() != FALSE);

    // 強制將偽裝值無縫還原，防止因為換槍動畫的網絡流漏洞導致玩家在潛入任務中瞬間破功暴露
    m_pPlayer->m_iIncognito = iOriginalIncog;

    return bResult;
}

int CBriefcase::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = nullptr;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = WEAPON_NOCLIP;
    p->iSlot = 4;                 // Slot 4 精確對齊第 5 物品欄槽位 (與 C4 炸彈共用)
    p->iPosition = 8;             // HUD 菜單選擇橫向排序權重
    p->iId = WEAPON_BRIEFCASE;   // 0x16 = 22
    p->iWeight = 5;
    p->iFlags = 0;
    p->bIsAccessory = true;

    m_iId = WEAPON_BRIEFCASE;
    m_iPlayEmptySound = 1;

    return 1;
}

float CBriefcase::GetMaxSpeed(void)
{
    unsigned short uState = m_WeaponState;

    // ?? 核心機制：當前情報傳輸 / 樣本採集狀態機位元鎖定 ??
    if (uState > 2)
    {
        // 狀態 3：代表正在與劇情任務終端機進行高頻情報上傳、數據對接中
        if (uState == 3)
        {
            return 1.0f; // 將特工玩家步伐瞬間死鎖、釘在原地（僅允許 1.0f 的微調觀賞動畫）
        }
        return 0.0f;
    }

    return 225.0f; // 常規提著公事包移動行軍時的速度
}

void CBriefcase::Holster(int skiplocal)
{
    // 呼叫虛擬函數表索引 0x65 (SetWeaponState) 將狀態機推向 狀態 1 (啟動收包/蓋上公事包動作)
    // 播放 Animation 1 (收槍動畫)，並灌入 0.833 秒的收包硬直時間窗
    SetWeaponState(1);
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.833f;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.833f;
}

int CBriefcase::iItemSlot(void)
{
    return 5;
}

void CBriefcase::PrimaryAttack(void)
{
    unsigned short uState = GetWeaponState();

    if (uState != 0)
    {
        return;
    }

    CTriggerUseTool *pTrigger = m_pPlayer->m_toolTrigger;

    if (pTrigger != nullptr)
    {
        if (pTrigger->IsPlayerInPositionUsingProperTool())
        {
            // ?? 解鎖終端對接通道：將狀態機直接推向 狀態 3（啟動 1.0f 速度死鎖與高頻數據上傳）??
            SetWeaponState(3);
            return;
        }
    }

    hudtextparms_t hudTextParms;
    hudTextParms.effect = 0;
    hudTextParms.fadeinTime = 0.25f;
    hudTextParms.fadeoutTime = 0.25f;
    hudTextParms.holdTime = 2.0f;
    hudTextParms.fxTime = 0.0f;
    hudTextParms.x = -1.0f;
    hudTextParms.y = 0.01f;
    hudTextParms.r1 = 0;
    hudTextParms.g1 = 0;
    hudTextParms.b1 = 255;
    hudTextParms.a1 = 255;
    hudTextParms.r2 = 255;
    hudTextParms.g2 = 255;
    hudTextParms.b2 = 255;
    hudTextParms.a2 = 255;
    hudTextParms.channel = 0;

    UTIL_HudMessageAll(&hudTextParms, "#BRIEFCASE_MESSAGE");
}

void CBriefcase::Reload(void)
{
    // 情報公事包為單人劇情任務無限耐久容器，不具備任何裝填/換彈動作。
    return;
}

void CBriefcase::SetWeaponState(unsigned short weaponState)
{
    float flAttackDuration = 0.0f;
    float flIdleDuration = 0.0f;

    switch (weaponState)
    {
        case 0: // 狀態 0：常規手提閒置
            flAttackDuration = 0.0f;
            flIdleDuration = 20.0f; // 0x41a00000 = 20.0f
            break;

        case 1: // 狀態 1：收包 / 部屬過渡
            flAttackDuration = 0.833f; // 0x3f553f7d = 0.833f
            flIdleDuration = 0.833f;
            break;

        case 2: // 狀態 2：特殊起手動畫
            flAttackDuration = 0.533f; // 0x3f0872b0 = 0.533f
            flIdleDuration = 0.533f;
            break;

        case 3: // 狀態 3：?? 核心絕密情報高頻數據上傳 / 拷貝狀態 ??
            m_pPlayer->m_iWeaponFlash = 128; // 0x80

            // 實時調用 GetGunPosition 座標探針計算公事包與任務終端機之間的朝向向量
            Vector vecDirection;
            GetGunPosition(); 

            // 調用預先快取的 events/briefcase.sc 全網同步情報下載和加密面板閃爍事件
            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireBriefcase, 0.0f, g_vecZero, g_vecZero, 
                                0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 
                                (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

            // 實時修改引擎物理層（PM_Move），強行將特工玩家的最高步速強殺、壓死在極限低速 1.0f 碼
            ENGINE_SET_CLIENT_MAXSPEED(m_pPlayer->edict(), 1.0f);

            flAttackDuration = 5.733f; // 0x40b774bc = 5.733f
            flIdleDuration = 5.733f;
            break;

        default:
            flAttackDuration = 0.0f;
            flIdleDuration = 0.0f;
            break;
    }

    // 呼叫虛擬函數表接口 SendWeaponAnim 播放與狀態碼完全對齊的第一人稱 V 模型提包動畫
    SendWeaponAnim(weaponState, UseDecrement() != FALSE);
    
    // 更新工具內部的中央火控狀態機
    m_WeaponState = weaponState;

    // 將矩陣算出的戰術動畫與行動阻斷時長，無縫灌入玩家 NextAttack 與武器 Idle 的行動鎖定窗
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flIdleDuration;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + flAttackDuration;
}

void CBriefcase::WeaponIdle(void)
{
    unsigned short uState = GetWeaponState();

    // 1. 狀態 0 與 狀態 1：常規手提與切換過渡，時間到後重置狀態機 0 並刷新閒置時脈
    if (uState == 1 || uState == 0)
    {
        if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
        {
            SetWeaponState(0);
            return;
        }
    }
    // 2. 狀態 2：特殊過渡動畫結束，呼叫虛擬函數表 [0x61] RetractWeapon 隱藏工具
    else if (uState == 2)
    {
        if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
        {
            RetractWeapon();
            return;
        }
    }
    // 3. 狀態 3：?? 5.73秒情報傳輸完美結束！解鎖地圖劇本與物品物理銷毀 ??
    else if (uState == 3)
    {
        if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
        {
            CBasePlayer *pPlayer = m_pPlayer;
            CTriggerUseTool *pTrigger = pPlayer->m_toolTrigger;

            // A. 向關卡地圖的核心控制器發射 USE_SET 啟動訊號，瞬間引爆後續劇情（例如炸毀機房、開啟防爆門、完成通關任務）
            if (pTrigger != nullptr)
            {
                pTrigger->FireUseTarget(pPlayer, nullptr, USE_SET, 0.0f);
            }

            // B. 呼交 [0x61] 接口完成工具的物理收網與隱藏
            RetractWeapon();

            // C. ?? 全網記憶體回收 ??
            // 0xffbfffff 位元遮罩精確清除玩家身上的 m_iAccessoryBits 情報公事包持有圖層標記
            pPlayer->m_iAccessoryBits &= ~(1 << 22); // 清除第 22 號配件位元

            // D. 劇情道具「一發自毀」：情報已安全送達，將公事包武器夾具從玩家物品欄中徹底銷毀、永久抹除
            DestroyItem();

            // E. 物理移動解放：拷貝完成，瞬間解除 1.0f 罰站死鎖，將特工玩家的最大行軍步速完全恢復正常！
            pPlayer->ResetMaxSpeed();
            return;
        }
    }
}
