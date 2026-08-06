#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CBlowtorch : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;       // 固定返回 Slot 5 (特殊劇情/C4欄位)
    float GetMaxSpeed(void) override;   // 動態傳回速度：熔斷切割時 1.0f 物理死鎖，常規提燈 225.0f

    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    void Reload(void) override;         // 完全留空直接 return，劇情無限耐久工具
    void WeaponIdle(void) override;     // 每幀廣播 idle 網路事件維護長明火，熔斷結束後調用 ResetMaxSpeed 解鎖步伐

    void PrimaryAttack(void) override;   // 核心火控：對準 CTriggerUseTool 區域則將狀態推向 1，否則噴出藍白 HUD 提示

    // 2. 🚨 CZDS 客製化多維狀態機中央分流矩陣 🚨
    void SetWeaponState(unsigned short weaponState); // 狀態 1 實施 pfnSetClientMaxspeed 強殺 1.0f 步速
    void Restart(void);

    // 3. 🚀 DWARF原廠命名為Use的 64碼等離子高精準「隱形射線探針」 🚀
    Vector GetGunPosition(void);

public:
    // 4. 私有與成員變數欄位 (精確填補並完全閉環私有數據長度)
    unsigned short m_usIdleBlowtorch;    // 預載的 events/blowtorchidle.sc 網路長明火事件快取 ID
    unsigned short m_usHolsterBlowtorch; // 預載的 events/blowtorchholster.sc 網路殘火熄滅事件快取 ID

    // 儲存於 field_0x10e 記憶體偏移量的開火事件快取
    unsigned short m_usFireBlowtorch; 

    // 核心互斥狀態鎖
    int m_playFireSound; // 控制等離子高溫持續轟鳴音效在開火第一幀有且僅有播放一次的互斥標記
};

LINK_ENTITY_TO_CLASS(weapon_blowtorch, CBlowtorch);

void CBlowtorch::Precache(void)
{
    PRECACHE_MODEL("models/v_btorch.mdl");
    PRECACHE_MODEL("models/w_btorch.mdl");

    PRECACHE_MODEL("sprites/fire.spr");
    PRECACHE_MODEL("sprites/lgtning.spr");

    PRECACHE_SOUND("weapons/blowtorch-1.wav");

    m_usFireBlowtorch = PRECACHE_EVENT(1, "events/blowtorch.sc");       // 0x10e 偏移量對齊
    m_usIdleBlowtorch = PRECACHE_EVENT(1, "events/blowtorchidle.sc");
    m_usHolsterBlowtorch = PRECACHE_EVENT(1, "events/blowtorchholster.sc");
}

void CBlowtorch::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_blowtorch");
    Precache();
    m_iId = WEAPON_BLOWTORCH;
    SET_MODEL(ENT(pev), "models/w_btorch.mdl");
    m_iClip = WEAPON_NOCLIP;
    m_iDefaultAmmo = WEAPON_NOCLIP;
    m_bIsAccessory = true;
    this->m_playFireSound = 1;
    FallInit();
}

BOOL CBlowtorch::Deploy(void)
{
    return DefaultDeploy("models/v_btorch.mdl", "models/p_btorch.mdl", 2, "onehanded", UseDecrement() != FALSE);
}

int CBlowtorch::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = nullptr;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = WEAPON_NOCLIP;
    p->iSlot = 4;
    p->iPosition = 5;
    p->iFlags = 0;
    p->iId = WEAPON_BLOWTORCH;
    p->iWeight = 5;
    p->bIsAccessory = true;

    m_iId = WEAPON_BLOWTORCH;
    m_iPlayEmptySound = 1;

    return 1;
}

float CBlowtorch::GetMaxSpeed(void)
{
    unsigned short uState = m_WeaponState;

    if (uState == 1) // 🚨 核心機制：正在近距離對著目標熔斷、焊接或切割時 🚨
    {
        return 1.0f; // 將玩家步伐完全死鎖、釘在原地（僅允許 1.0f 的微調）
    }

    if (uState != 0 && uState > 4)
    {
        return 0.0f;
    }

    return 225.0f; // 常規提著割炬行進時的速度
}

void CBlowtorch::Holster(int skiplocal)
{
    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usHolsterBlowtorch, 0.0f, g_vecZero, g_vecZero, 
                        0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 
                        (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);
}

int CBlowtorch::iItemSlot(void)
{
    return 5;
}

void CBlowtorch::PrimaryAttack(void)
{
    unsigned short uState = GetWeaponState();

    if (uState != 0 && uState != 1)
    {
        return;
    }

    CTriggerUseTool *pTrigger = m_pPlayer->m_toolTrigger;

    if (pTrigger != nullptr)
    {
        if (pTrigger->IsPlayerInPositionUsingProperTool())
        {
            SetWeaponState(1);
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

    UTIL_HudMessageAll(&hudTextParms, "#BLOWTORCH_MESSAGE");
}

void CBlowtorch::Reload(void)
{
    // 割炬為單人劇情專用無限耐久工具，不具備任何裝填/更換燃料包動作。
    return;
}

void CBlowtorch::Restart(void)
{
    this->m_playFireSound = 1;
}

void CBlowtorch::SetWeaponState(unsigned short weaponState)
{
    float flAttackDuration = 0.0f;
    float flIdleDuration = 0.0f;

    switch (weaponState)
    {
        case 0: // 狀態 0：停止熔斷，重置回閒置長明火狀態
            this->m_playFireSound = 1;
            // 瞬間切斷並完全關閉等離子噴火的持續怪叫聲（SND_STOP = 0x20）
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/blowtorch-1.wav", 0.0f, 0.0f, SND_STOP, PITCH_NORM);
            flAttackDuration = 0.0f;
            flIdleDuration = 20.0f;
            break;

        case 1: // 狀態 1：🚨 核心高溫等離子熔斷推進狀態 🚨
            m_pPlayer->m_iWeaponFlash = 128; // 0x80：激發強烈的焊接火花物理閃光體積

            // 調用底層工具物理探針（Ghidra 錯位分析為 Use），即時計算焊槍與鐵鎖之間的射線法線與坐標向量
            Vector vecDirection;
            GetGunPosition(vecDirection); // 將計算完的粒子噴射中心點與朝向暫存入向量

            // 調用預先快取的 events/blowtorch.sc 進行全伺服器大範圍等離子火焰與焊接電弧噴射粒子廣播
            PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireBlowtorch, 0.0f, g_vecZero, g_vecZero, 
                                vecDirection.x, vecDirection.y, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 
                                (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

            // 實時修改引擎物理層（PM_Move），強行將玩家的動態持槍步速上限強殺、壓死在極限低速 1.0f 碼
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/blowtorch-1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);
            ENGINE_SET_CLIENT_MAXSPEED(m_pPlayer->edict(), 1.0f);

            flAttackDuration = 0.066f;
            flIdleDuration = 0.466f;
            break;

        case 2: // 狀態 2：特定切換或啟動動畫時脈
            flAttackDuration = 1.0f;
            flIdleDuration = 1.0f;
            break;

        case 3: // 狀態 3：超長熔斷破壞後的過渡動畫時脈
            flAttackDuration = 4.7f;
            flIdleDuration = 4.7f;
            break;

        case 4: // 狀態 4：收尾或特殊排氣硬直時脈
            flAttackDuration = 1.066f;
            flIdleDuration = 1.066f;
            break;

        default:
            flAttackDuration = 0.0f;
            flIdleDuration = 0.0f;
            break;
    }

    // 呼叫虛擬函數表接口 SendWeaponAnim 播放與狀態碼完全對齊的第一人稱 V 模型割炬動畫
    SendWeaponAnim(weaponState, UseDecrement() != FALSE);
    
    // 更新工具內部的中央火控狀態機
    m_WeaponState = weaponState;

    // 將矩陣算出的戰術動畫與行動阻斷時長，無縫灌入玩家 NextAttack 與武器 Idle 的行動鎖定窗
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flIdleDuration;
    m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + flAttackDuration;
}

Vector CBlowtorch::GetGunPosition(void)
{
    m_iShotsFired++;
    m_bDelayFire = true;
    m_flLastFire = gpGlobals->time;

    Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    Vector vecSrc;
    m_pPlayer->GetGunPosition(vecSrc);

    if (this->m_playFireSound)
    {
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/blowtorch-1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);
        this->m_playFireSound = 0;
    }

    m_bDelayFire = false;

    // 🚀 核心探針：利用 FireBullets3 朝前發射一條 64 碼、不具備殺傷力的特殊判定射線 🚀
    // 參數 vecDirShooting.x = 64.0f 即切割探針的極限距離
    Vector vecReturn;
    m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, 0.0f, 64.0f, 0, BULLET_PLAYER_CUSTOM_DAMAGE, m_pPlayer->random_seed, 0.98f);

    return vecSrc;
}

void CBlowtorch::WeaponIdle(void)
{
    // 1. 每幀（或每次Think）高頻廣播 m_usIdleBlowtorch (events/blowtorchidle.sc) 
    // 用以在客戶端動態維持前端亮起的微弱長明火苗與環境音效
    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usIdleBlowtorch, 0.0f, g_vecZero, g_vecZero, 
                        0.0f, 0.0f, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 
                        (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

    // 2. 時脈安全邊界攔截
    if (UTIL_WeaponTimeBase() < m_flTimeWeaponIdle)
    {
        return;
    }

    // 3. 狀態回收與步速解除狀態機
    switch (GetWeaponState())
    {
        case 0:
        case 2:
        case 3:
            // 閒置或過渡動畫時間到，強制將狀態歸零（0：回到基礎常明火狀態），重設 20 秒閒置窗
            SetWeaponState(0);
            return;

        case 1: // 🚨 核心機制：剛結束熔斷切割動作，進入閒置回收 🚨
            SetWeaponState(0);
            // 💡 物理移動解鎖：熔斷結束，立即解鎖 1.0f 的死鎖懲罰，將玩家的最大移速重置恢復正常！
            m_pPlayer->ResetMaxSpeed();
            return;

        case 4:
            // 狀態 4：過渡完畢，呼叫虛擬函數表 [0x61] RetractWeapon 接口完全收回或隱藏工具
            RetractWeapon();
            return;
    }
}
