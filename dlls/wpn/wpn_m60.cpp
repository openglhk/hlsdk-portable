#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CM60 : public CBasePlayerWeapon
{
public:
    // 1. 基底核心虛擬表接口覆蓋
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;       // 固定返回 Slot 1 (主武器欄位)
    float GetMaxSpeed(void) override;   // 限制玩家最高步速為 198.0f (全遊戲最沉重步伐懲罰)

    BOOL Deploy(void) override;
    void Reload(void) override;         // 5.766 秒全遊戲最慢五段頂蓋饋彈(Belt-fed)重裝填時脈
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;   // 140 碼完美架槍控速線與移動速度擴散加權分流
    BOOL UseDecrement(void) override;

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

    // 2. M60 特化核心開火流
    void M60Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
    // 3. 靜態反射存檔表格 (對齊 Save/Restore 中的 1 個核心變數長度)
    static TYPEDESCRIPTION m_SaveData[];

public:
    // 4. 私有與成員變數欄位 (精確填補並完全閉環 0x11c = 284 位元組的私有數據長度)
    int m_iShell;            // 預載的 models/rshell.mdl 大口徑步槍金屬彈殼索引快取
    unsigned short m_usFireM60; // 預載的 events/m60.sc 槍火與全網射擊事件快取 ID

    // 🚀 CZDS 獨家客製化序列化變數 🚀
    int iShellOn;            // 1位元組/整型對齊：控制第一人稱 V 模型上動態彈鏈的實體圖層渲染與長度遞減開關
};

TYPEDESCRIPTION CM60::m_SaveData[] =
{
    DEFINE_FIELD(CM60, iShellOn, FIELD_INTEGER), // 實時保存/讀回第一人稱 V 模型上動態彈鏈的視覺圖層狀態
};

LINK_ENTITY_TO_CLASS(weapon_m60, CM60); 

void CM60::Precache(void)
{
    PRECACHE_MODEL("models/v_m60.mdl");
    PRECACHE_MODEL("models/w_m60.mdl");

    PRECACHE_SOUND("weapons/m60-1.wav");
    PRECACHE_SOUND("weapons/m60-2.wav");
    PRECACHE_SOUND("weapons/m60_boxout.wav");
    PRECACHE_SOUND("weapons/m60_boxin.wav");
    PRECACHE_SOUND("weapons/m60_chain.wav");
    PRECACHE_SOUND("weapons/m60_coverup.wav");
    PRECACHE_SOUND("weapons/m60_coverdown.wav");

    // 載入重機槍專屬的大口徑步槍彈殼模型（rshell = Rifle Shell）
    m_iShell = PRECACHE_MODEL("models/rshell.mdl");

    // 快取全域網路火光與射擊事件事件
    m_usFireM60 = PRECACHE_EVENT(1, "events/m60.sc");
}

void CM60::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_m60");
    Precache();

    m_iId = WEAPON_M60; // 0x17 = 十進位 23
    SET_MODEL(ENT(pev), "models/w_m60.mdl");

    m_iShotsFired = 0;     // 連續開火計數器歸零
    m_iDefaultAmmo = 100;  // 預設給予 100 發巨量備彈（正好是一個滿彈箱的容彈量）
    m_flAccuracy = 0.2f;   // 基礎精確度擴散常數
    m_bIsAccessory = false; // 屬於主戰熱兵器，而非手榴彈/C4類的 Accessory 附屬挂件

    FallInit();
}

BOOL CM60::Deploy(void)
{
    m_iShotsFired = 0;       // 切槍時立即重置連續開火計數器，消除前次射擊的後座力累積
    m_flAccuracy = 0.2f;     // 恢復至基礎精確度擴散常數
    this->iShellOn = 1;      // 🚨 CZDS獨家：啟用第一人稱 V 模型上的視覺彈鏈/彈殼渲染標記 🚨

    // 呼叫基礎部屬函數，載入第一人稱 V 模型，播放 Animation 4（M60 經典的沉重拔槍提槍動作）
    return DefaultDeploy("models/v_m60.mdl", "models/p_m60.mdl", 4, "m60", UseDecrement() != FALSE);
}

int CM60::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "762NatoBox"; // 🚨 越戰蘭博重機槍專屬大口徑 7.62x51mm 北約標準彈箱 🚨
    p->iMaxAmmo1 = 200;         // 備彈上限 200 發
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 100;          // 滿載彈箱容彈量高達 100 發
    p->iSlot = 0;               // Slot 0 精確對齊主武器欄位 (Slot 1)
    p->iPosition = 21;          // 0x15：HUD 武器選擇選單中的橫向優先級排位
    p->iFlags = 0;
    p->iId = WEAPON_M60;        // 0x17 = 23 (劫持多人版 M249 的核心 ID)
    p->iWeight = 25;            // 0x19 = 25：撿槍優先級權重（極高優先權）
    p->bIsAccessory = false;

    m_iId = WEAPON_M60;

    return 1;
}

float CM60::GetMaxSpeed(void)
{
    // 扛著這挺重機槍時，最高移動速度被壓低至 198.0f，比 M72 LAW 火箭筒還要慢 1 碼
    return 198.0f;
}

int CM60::iItemSlot(void)
{
    // M60 輕機槍在物品欄中精確佔用第 1 格主武器槽位 (Slot 1)
    return 1;
}

void CM60::M60Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    // 1. 🚨 反恐精英核心秘密：M60 專屬的三次方後座力精確度惡化矩陣 🚨
    // 擴散公式：(ShotsFired^3) / 175 + 0.4f
    float flCurrentAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175.0f) + 0.4f;
    
    // 將最大擴散值強制焊死在 0.9f，防止子彈朝向背後發射 [1.32]
    if (flCurrentAccuracy > 0.9f)
    {
        flCurrentAccuracy = 0.9f;
    }
    m_flAccuracy = flCurrentAccuracy;

    // 2. 空倉防呆判定
    if (m_iClip < 1)
    {
        if (m_fFireOnEmpty)
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
            return;
        }
    }
    else
    {
        m_iClip--;
        
        // 觸發玩家槍火槍口閃爍（EF_MUZZLEFLASH = 2）與動畫更新
        m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        // 3. 多維後座力視角加算（當前視角 + 螢幕震動衝擊角）
        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors(vecAngles);

        m_pPlayer->m_iWeaponVolume = 2250; // 0x8ca
        m_pPlayer->m_iWeaponFlash = 512;   // 0x200

        // 4. 輔助瞄準與 3D 彈道軌跡演算
        Vector vecAutoaim;
        m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES); // 0.17364818

        Vector vecSrc;
        m_pPlayer->GetGunPosition(vecSrc);

        // 調用 GoldSrc 最頂級的 FireBullets3 進行實時大口徑穿牆射線追蹤（傳入隨機擴散與當前精確度常數）
        m_pPlayer->FireBullets3(vecSrc, vecAutoaim, flSpread, flCycleTime, 2, BULLET_PLAYER_762MM, 
                                 m_pPlayer->random_seed, m_flAccuracy);

        // 5. 調用預先快取的 events/m60.sc 進行全域開火、拋殼與巨型槍火特效廣播
        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireM60, 0.0f, g_vecZero, g_vecZero, 
                            flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 
                            (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

        // 刷新武器次擊與主擊的冷卻時脈
        m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flCycleTime; // 此處的引數 flSpread 實質在暫存器中被優化複寫為了射擊間隔 flCycleTime

        // 如果打光最後一發且身上沒有任何 7.6251mm 備彈，向玩家防護服灌入語音警告
        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.6f;

        // 6. 🚨 核心動態後座力回饋矩陣 (KickBack 參數群) 🚨
        // 分流 A：玩家正處於空中飛行或跳躍狀態（FL_ONGROUND 消失）➔ 後座力狂暴化暴增
        if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
        {
            KickBack(1.8f, 0.65f, 0.45f, 0.125f, 5.0f, 3.5f, 8);
        }
        else
        {
            float flPlayerSpeed = m_pPlayer->pev->velocity.Length2D();
            
            // 分流 B：玩家正處於地表跑步或移動狀態 ➔ 後座力大幅度劣化
            if (flPlayerSpeed > 0.0f)
            {
                KickBack(1.1f, 0.5f, 0.3f, 0.06f, 4.0f, 3.0f, 8);
                return;
            }

            // 分流 C：玩家保持靜止站立狀態 ➔ 標準後座力
            if ((m_pPlayer->pev->flags & FL_DUCKING) == 0)
            {
                KickBack(0.8f, 0.35f, 0.3f, 0.03f, 3.75f, 3.0f, 9);
            }
            // 分流 D：玩家處於完全蹲下（Crouch / FL_DUCKING = 0x4000）架槍狀態 ➔ 獲得最高精確度與最低後座力補償
            else
            {
                KickBack(0.75f, 0.325f, 0.25f, 0.025f, 3.5f, 2.5f, 9);
            }
        }
    }
}

void CM60::PrimaryAttack(void)
{
    // 1. 實時抓取玩家 pev 屬性，判定當前物理移動速度
    float flBaseSpread = 0.0f;

    // 分流 A：玩家正處於空中飛行或跳躍狀態（FL_ONGROUND = 0x200 消失）
    if ((m_pPlayer->pev->flags & FL_ONGROUND) == 0)
    {
        flBaseSpread = 0.5f;
    }
    else
    {
        // 計算二維橫向移動速度 (Length2D)
        float flPlayerSpeed = m_pPlayer->pev->velocity.Length2D();

        // 分流 B：玩家處於靜止或慢速精確架槍射擊狀態（速度低於 140 碼）
        if (flPlayerSpeed <= 140.0f)
        {
            // 解鎖最高精確度開火通道！
            // 傳入的隨機擴散隨機分量極低，僅為「當前三次方後座力擴散 m_flAccuracy * 0.03」
            // 射擊間隔固定為 0.1 秒（每分鐘 600 發的北約重型壓制射速）
            M60Fire(m_flAccuracy * 0.03f, 0.1f, FALSE);
            return;
        }

        // 分流 C：玩家正處於全速奔跑或無架槍移動狀態（速度高於 140 碼）
        flBaseSpread = 0.095f;
    }

    // 將運動姿態算出的基礎擴散（flBaseSpread）結合當前連續開火惡化常數，加算代入開火主核心
    float flFinalSpread = flBaseSpread * m_flAccuracy + 0.045f;
    M60Fire(flFinalSpread, 0.1f, FALSE);
}

void CM60::Reload(void)
{
    // 1. 檢查玩家背包內是否還有剩餘的 762NatoBox 重機槍備用彈藥
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        // 2. 🚨 核心機制：拉起全遊戲最長的頂蓋饋彈Belt-fed重裝填時脈 🚨
        // 參數：最大裝填量 100 發、動畫序列索引 3（M60五段繁瑣換彈動畫）、超長硬直時間 5.766 秒
        BOOL bResult = DefaultReload(100, 3, 5.766f);
        
        if (bResult)
        {
            // 3. 播放玩家在第三人稱視角下的通用換彈實體動作
            m_pPlayer->SetAnimation(PLAYER_RELOAD);

            // 4. 重置核心火控計數器與三次方精確度擴散矩陣
            m_flAccuracy = 0.2f;   // 恢復至最初精確度
            m_bDelayFire = false;  // 解除延遲開火鎖定
            m_iShotsFired = 0;     // 連續開火計數器徹底歸零
            return;
        }
    }
}

int CM60::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    // 💡 終極鐵證還原：精確讀回 M60 重機槍在反射表格中唯一註冊的 1 個核心狀態欄位
    return restore->ReadFields("CM60", this, m_SaveData, 1);
}

int CM60::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    // 與 Restore 完全對稱：精確向進度檔寫入 M60 重機槍唯一註冊的 1 個核心狀態欄位 (iShellOn)
    return save->WriteFields("CM60", this, m_SaveData, 1);
}

BOOL CM60::UseDecrement(void)
{
    // 扣動扳機開火時，確認消耗彈箱內部的實體子彈數量
    return TRUE;
}

void CM60::WeaponIdle(void)
{
    // 1. 呼叫虛擬函數表索引 0x5a [90] 處的基底 ResetEmptySound 接口重置空倉聲
    ResetEmptySound();

    // 2. 輔助瞄準探針維持：每幀為重機槍刷新輔助瞄準的前視向量矩陣
    Vector vecAutoaim;
    m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES); // 0.17364818

    // 3. 時脈安全邊界攔截：若尚未到達武器閒置的觸發時間點，則直接返回
    if (UTIL_WeaponTimeBase() < m_flTimeWeaponIdle)
    {
        return;
    }

    // 4. 重置閒置時間窗：設定高達 20.0 秒的超長安全閒置時脈
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;

    // 5. 調用虛擬函數表索引 0x5b [91] SendWeaponAnim 播放 Animation 0（M60 標準扛肩呼吸閒置動畫）
    SendWeaponAnim(0, UseDecrement() != FALSE);
}
