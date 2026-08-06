#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

class CM249 : public CBasePlayerWeapon
{
public:
    // 1. 基底核心虛擬表接口覆蓋
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;       // 固定返回 Slot 1 (主武器欄位)
    float GetMaxSpeed(void) override;   // 限制玩家最高步速為 198.0f (與 M60 對齊的重灌兵器懲罰)

    BOOL Deploy(void) override;
    void Reload(void) override;         // 4.666 秒頂蓋饋彈(Belt-fed)手動裝填時脈
    void WeaponIdle(void) override;

    void PrimaryAttack(void) override;   // 140 碼完美架槍控速線與運動姿態加權分流
    BOOL UseDecrement(void) override;

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

    // 2. M249 特化核心開火流
    void M249Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim);

public:
    // 3. 靜態反射存檔表格
    static TYPEDESCRIPTION m_SaveData[];

public:
    // 4. 私有與成員變數欄位
    int m_iShell;            // 預載的 models/rshell.mdl 大口徑步槍金屬彈殼索引快取
    unsigned short m_usFireM249; // 預載的 events/m249.sc 槍火與全網射擊事件快取 ID

    // 🚀 CZDS 獨家客製化序列化變數 🚀
    int iShellOn;            // 控制第一人稱 V 模型上動態彈鏈的實體圖層渲染與長度遞減開關
};

TYPEDESCRIPTION CM249::m_SaveData[] =
{
    DEFINE_FIELD(CM249, iShellOn, FIELD_INTEGER), // 記錄第一人稱 V 模型上動態彈鏈的視覺縮短圖層開關
};

LINK_ENTITY_TO_CLASS(weapon_m249, CM249); 

void CM249::Precache(void)
{
    PRECACHE_MODEL("models/v_m249.mdl");
    PRECACHE_MODEL("models/w_m249.mdl");

    PRECACHE_SOUND("weapons/m249-1.wav");
    PRECACHE_SOUND("weapons/m249-2.wav");
    PRECACHE_SOUND("weapons/m249_boxout.wav");
    PRECACHE_SOUND("weapons/m249_boxin.wav");
    PRECACHE_SOUND("weapons/m249_chain.wav");
    PRECACHE_SOUND("weapons/m249_coverup.wav");
    PRECACHE_SOUND("weapons/m249_coverdown.wav");

    // 與 M60 相同，拋殼使用大口徑步槍彈殼模型（rshell = Rifle Shell）
    m_iShell = PRECACHE_MODEL("models/rshell.mdl");

    // 快取 M249 專屬的全域網路火光與射擊事件
    m_usFireM249 = PRECACHE_EVENT(1, "events/m249.sc");
}

void CM249::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_m249");
    Precache();

    m_iId = WEAPON_M249; // 0xe = 十進位 14
    SET_MODEL(ENT(pev), "models/w_m249.mdl");

    m_iShotsFired = 0;     // 連續開火計數器歸零
    m_iDefaultAmmo = 100;  // 拾取時預設給予 100 發滿彈箱子彈
    m_flAccuracy = 0.2f;   // 初始基礎擴散精確度
    m_bIsAccessory = false; // 主戰熱兵器標記

    FallInit();
}

BOOL CM249::Deploy(void)
{
    m_iShotsFired = 0;       // 切槍時立即重置連續開火計數器，消除前次射擊的後座力累積
    m_flAccuracy = 0.2f;     // 恢復至基礎精確度擴散常數
    this->iShellOn = 1;      // 啟用第一人稱 V 模型上的視覺彈鏈/彈殼渲染標記

    // 呼叫基礎部屬函數，載入第一人稱 V 模型，播放 Animation 4（M249 經典的拉動拉機柄提槍動作）
    return DefaultDeploy("models/v_m249.mdl", "models/p_m249.mdl", 4, "m249", UseDecrement() != FALSE);
}

int CM249::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "556NatoBox"; // 🚨 經典多人對戰與單人任務共用的大容量 5.56mm 彈箱 🚨
    p->iMaxAmmo1 = 200;         // 背包備彈上限 200 發
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 100;          // 機槍滿載彈箱容彈量高達 100 發
    p->iSlot = 0;               // Slot 0 精確對齊主武器欄位 (Slot 1)
    p->iPosition = 4;           // HUD 武器選擇選單中的橫向優先級排位
    p->iFlags = 0;
    p->iId = WEAPON_M249;        // 0xe = 14 (正統 M249 核心 ID)
    p->iWeight = 25;            // 0x19 = 25：撿槍優先級權重
    p->bIsAccessory = false;

    m_iId = WEAPON_M249;

    return 1;
}

float CM249::GetMaxSpeed(void)
{
    // 扛著這挺重機槍時，最高移動速度被壓低至 198.0f，這與 M60 完全平起平坐
    return 198.0f;
}

int CM249::iItemSlot(void)
{
    // M249 班用機槍在物品欄中精確佔用第 1 格主武器槽位 (Slot 1)
    return 1;
}

void CM249::M249Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
    m_bDelayFire = true;
    m_iShotsFired++;

    // 1. 🚨 反恐精英核心秘密：M249 與 M60 共用的三次方後座力精確度惡化矩陣 🚨
    // 擴散公式：(ShotsFired^3) / 175 + 0.4f
    float flCurrentAccuracy = (float)((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175.0f) + 0.4f;
    
    // 將最大擴散值強制焊死在 0.9f，防止彈道越界
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
        
        // 觸發玩家槍火閃爍特效（EF_MUZZLEFLASH = 2）與動畫更新
        m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        // 3. 多維後座力視角加算（當前視角 + 螢幕震動衝擊角）
        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors(vecAngles);

        // 數值特徵分流：M249 的開火音量與 M60 的 2250 不同，設定為 1500 (0x5dc)
        m_pPlayer->m_iWeaponVolume = 1500; // 0x5dc
        m_pPlayer->m_iWeaponFlash = 512;   // 0x200

        // 4. 輔助瞄準與 3D 彈道軌跡演算
        Vector vecAutoaim;
        m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES); // 0.17364818

        Vector vecSrc;
        m_pPlayer->GetGunPosition(vecSrc);

        // 調用 GoldSrc 的 FireBullets3 實施大口徑穿牆射線追蹤（5.56mm 彈藥類型，BULLET_PLAYER_556MM = 3）
        m_pPlayer->FireBullets3(vecSrc, vecAutoaim, flSpread, flCycleTime, 2, BULLET_PLAYER_556MM, 
                                 m_pPlayer->random_seed, m_flAccuracy);

        // 5. 調用預先快取的 events/m249.sc 進行全域開火、拋殼與槍火特效廣播
        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireM249, 0.0f, g_vecZero, g_vecZero, 
                            flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 100.0f), 
                            (int)(m_pPlayer->pev->punchangle.y * 100.0f), 0, 0);

        // 刷新武器次擊與主擊的冷卻時脈
        m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flCycleTime; // 此處的引數 flSpread 實質在暫存器中被優化複寫為了射擊間隔 flCycleTime

        // 如果打光子彈且背包空倉，向玩家防護服灌入語音警告
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
            
            // 分流 B：玩家正處於地表跑步或移動狀態 ➔ 後座力大幅度擴散
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

void CM249::PrimaryAttack(void)
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
            // 傳入的隨機擴散隨機分量極低，僅為「當前三次方後座力擴散 m_flAccuracy * 0.03f」
            // 射擊間隔固定為 0.1 秒（每分鐘 600 發的重型連發壓制射速）
            M249Fire(m_flAccuracy * 0.03f, 0.1f, FALSE);
            return;
        }

        // 分流 C：玩家正處於全速奔跑或無架槍移動狀態（速度高於 140 碼）
        flBaseSpread = 0.095f;
    }

    // 將運動姿態算出的基礎擴散（flBaseSpread）結合當前連續開火惡化常數，加算代入 M249 開火主核心
    float flFinalSpread = flBaseSpread * m_flAccuracy + 0.045f;
    M249Fire(flFinalSpread, 0.1f, FALSE);
}

void CM249::Reload(void)
{
    // 1. 檢查玩家背包內是否還有剩餘的 556NatoBox 班用機槍備用彈藥
    if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        // 2. 🚨 核心機制：拉起 M249 經典的五段式頂蓋饋彈裝填時脈 🚨
        // 參數：最大裝填量 100 發、動畫序列索引 3（M249 經典換彈動畫）、硬直時間 4.666 秒
        BOOL bResult = DefaultReload(100, 3, 4.666f);
        
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

int CM249::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    // 💡 讀載對稱還原：讀取 M249 班用機槍在反射表格中唯一註冊的 1 個核心狀態欄位 (iShellOn)
    return restore->ReadFields(restore, "CM249", this, m_SaveData, 1);
}

int CM249::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    // 與 Restore 完全對稱：精確向進度檔寫入 M249 班用機槍唯一註冊的 1 個核心狀態欄位 (iShellOn)
    return save->WriteFields(save, "CM249", this, m_SaveData, 1);
}

BOOL CM249::UseDecrement(void)
{
    // 扣動扳機開火時，確認消耗彈箱內部的實體子彈數量
    return TRUE;
}

void CM249::WeaponIdle(void)
{
    // 1. 呼叫虛擬函數表索引 0x5a 處的基底 ResetEmptySound 接口重置空倉音效
    ResetEmptySound();

    // 2. 輔助瞄準探針維持：每幀為班用機槍刷新輔助瞄準的前視向量矩陣
    Vector vecAutoaim;
    m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES); // 0.17364818

    // 3. 時脈安全邊界攔截：若尚未到達武器閒置的觸發時間點，則直接返回
    if (UTIL_WeaponTimeBase() < m_flTimeWeaponIdle)
    {
        return;
    }

    // 4. 重置閒置時間窗：設定高達 20.0 秒的超長安全閒置時脈
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;

    // 5. 調用虛擬函數表索引 0x5b SendWeaponAnim 播放 Animation 0（M249 標準扛肩呼吸閒置動畫）
    SendWeaponAnim(0, UseDecrement() != FALSE);
}

