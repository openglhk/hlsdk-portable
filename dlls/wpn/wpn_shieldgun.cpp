#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "shield_state.h" // WSTATE_SHIELD_DRAWN = (1 << 5)

class CShieldGun : public CBasePlayerWeapon
{
public:
    // 1. 基底核心虛擬表接口覆蓋
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;       // 固定返回 Slot 1 (主武器長槍欄位)
    float GetMaxSpeed(void) override;   // 動態傳回 m_fMaxSpeed (180.0f 或 150.0f 烏龜步速)

    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    void Reload(void) override;         // 舉盾全防禦狀態下禁止換彈，露頭姿態 2.7 秒單手裝填
    void WeaponIdle(void) override;     // 舉盾姿態下強制鎖定播放 Animation 6 持續格擋

    void PrimaryAttack(void) override;   // 舉盾時強制封鎖開火管道，露頭時觸發 0.725f~0.92f 劣化擴散點射
    void SecondaryAttack(void) override; // 核心大腦：右鍵開關全屏無敵防禦判定、即時削砍行軍速度與網路骨骼多載
    BOOL UseDecrement(void) override;

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;

    // 2. 編譯器內聯核心開火流輔助函數
    void ShieldGunFire(float flSpread, float flCycleTime, BOOL fUseSemi);

public:
    // 3. 靜態雙變數反射存檔表格
    static TYPEDESCRIPTION m_SaveData[];

public:
    // 4. 私有與成員變數欄位 (精確閉環並對齊私有結構體長度)
    int m_iShell;                  // 預載的 models/pshell.mdl 小口徑五七手槍金屬彈殼索引快取
    unsigned short m_usFireShieldGun; // 預載的 events/shieldgun.sc 全網持盾擊發事件快取 ID

    // ?? 戰術盾牌狀態機中央控制矩陣 ??
    BOOL m_bShieldDrawn;           // 實時判定盾牌在三維空間中是否完全拉起格擋 (對齊反射存檔變數 1)
    float m_fMaxSpeed;             // 實時保存隨右鍵舉/收盾而動態削砍、拉伸後的行軍步速 (對齊反射存檔變數 2)
};

TYPEDESCRIPTION CShieldGun::m_SaveData[] =
{
    DEFINE_FIELD(CShieldGun, m_bShieldDrawn, FIELD_BOOLEAN), // 1. 記錄玩家存檔一瞬間，盾牌究竟是完全拉起防守，還是偏側收攏
    DEFINE_FIELD(CShieldGun, m_fMaxSpeed, FIELD_FLOAT),       // 2. 記錄存檔一瞬間，玩家因持盾狀態所被動態削砍、縮放後的持槍行軍步速
};

LINK_ENTITY_TO_CLASS(weapon_shieldgun, CShieldGun);
//IMPLEMENT_SAVERESTORE(CShieldGun, CBasePlayerWeapon);

void CShieldGun::Precache(void)
{
    PRECACHE_MODEL("models/v_shield_r.mdl");
    PRECACHE_MODEL("models/w_shield.mdl");

    PRECACHE_SOUND("weapons/fiveseven-1.wav");
    PRECACHE_SOUND("weapons/fiveseven_clipout.wav");
    PRECACHE_SOUND("weapons/fiveseven_clipin.wav");
    PRECACHE_SOUND("weapons/fiveseven_sliderelease.wav");
    PRECACHE_SOUND("weapons/fiveseven_slidepull.wav");

    // 載入小口徑手槍專屬的拋殼模型（pshell = Pistol Shell）
    m_iShell = PRECACHE_MODEL("models/pshell.mdl");

    // 快取戰術盾牌手槍專屬的全域網路火光與射擊事件
    m_usFireShieldGun = PRECACHE_EVENT(1, "events/shieldgun.sc");
}

void CShieldGun::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_shieldgun");
    Precache();

    m_iId = WEAPON_SHIELDGUN; // 0x1a = 十進位 26
    SET_MODEL(ENT(pev), "models/w_shield.mdl");

    // ?? 戰術盾牌狀態機初始化位元遮罩 ??
    // 0xffffffdf 代表清除特定狀態位元（即 ~WSTATE_SHIELD_DRAWN / 1 << 5）
    m_iWeaponState &= ~WSTATE_SHIELD_DRAWN; 
    m_bShieldDrawn = FALSE; // 初始未完全展開立起盾牌

    m_iDefaultAmmo = 20;   // 0x14：預設提供 FN Five-Seven 滿載彈匣 20 發子彈
    m_fMaxSpeed = 180.0f;  // 持盾時的基礎持槍移動速度上限
    m_bIsAccessory = false;

    FallInit();
}

BOOL CShieldGun::Deploy(void)
{
    m_bShieldDrawn = FALSE; // 切槍時立即將盾牌設為初始收攏/手槍外露狀態

    // ?? 核心切槍狀態位清除：0xffffffdf (~WSTATE_SHIELD_DRAWN)
    // 確保拔出武器時，完全重置全網持盾全屏防禦狀態
    m_iWeaponState &= ~WSTATE_SHIELD_DRAWN; 

    // 持盾單手射擊精度?罰：手槍基礎擴散值劣化至高達 0.92f
    m_flAccuracy = 0.92f;

    // 呼叫基礎部屬函數，載入第一人稱 V 模型，播放 Animation 5（盾牌加掛五七手槍的標準拔槍武裝動作）
    return DefaultDeploy("models/v_shield_r.mdl", "models/p_shield_r.mdl", 5, "shieldgun", UseDecrement() != FALSE);
}

int CShieldGun::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "57mm";        // ?? 採用 FN Five-Seven 專屬的 5.7x28mm 高速子彈 ??
    p->iMaxAmmo1 = 100;          // 備彈上限 100 發
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = 20;            // 0x14：手槍滿載彈匣容彈量 20 發
    p->iSlot = 0;                // Slot 0 精確對齊主武器欄位 (Slot 1)
    p->iPosition = 20;           // 0x14：HUD 武器選擇選單中的橫向優先級排位
    p->iFlags = 0;
    p->iId = WEAPON_SHIELDGUN;   // 0x1a = 26
    p->iWeight = 50;             // 0x32 = 50：全遊戲最高級別的撿槍優先級權重！
    p->bIsAccessory = false;

    m_iId = WEAPON_SHIELDGUN;

    return 1;
}

float CShieldGun::GetMaxSpeed(void)
{
    // 實時傳回當前盾牌狀態下的移動速度上限
    // 初始值為 180.0f（Spawn中釘下），會隨右鍵「拉起完全防禦」或「收攏探頭」在動態火控中實時縮放
    return m_fMaxSpeed;
}

void CShieldGun::Holster(int skiplocal)
{
    // 1. ?? 核心機制：收槍時強制喚醒、還原玩家畫面上的常規全十字準星 ??
    // 傳入 true 代表關閉盾牌窄視角遮罩，將準星重置為正常射擊外觀
    m_pPlayer->UpdateShieldCrosshair(true);

    // 2. 核心狀態位清除：0xffffffdf (~WSTATE_SHIELD_DRAWN)
    // 確保盾牌被收入背後時，完全抹除全網持盾全屏防禦狀態
    m_iWeaponState &= ~WSTATE_SHIELD_DRAWN;
}

int CShieldGun::iItemSlot(void)
{
    // 戰術防彈盾牌在物品欄中精確佔用第 1 格主武器槽位 (Slot 1)
    return 1;
}

void CShieldGun::PrimaryAttack(void)
{
    // 1. 水中防呆：若玩家全身浸入深水中 (waterlevel == 3)，禁止開火，播放空倉敲擊聲
    if (m_pPlayer->pev->waterlevel == WATERLEVEL_HEAD)
    {
        PlayEmptySound();
        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
        return;
    }

    // 2. ?? 持盾全屏防禦狀態攔截 ??
    // 0x20 位元對應 WSTATE_SHIELD_DRAWN。如果盾牌此時已經完全立起拉開 (m_bShieldDrawn == true)
    // 控制流會直接被卡死 return，無法扣動扳機！想要開火必須先按右鍵收回防禦
    if ((m_iWeaponState & WSTATE_SHIELD_DRAWN) && m_bShieldDrawn)
    {
        return;
    }

    // 3. 連續開火與單手持槍動態擴散退火矩陣 (Five-Seven 手槍特化版)
    m_iShotsFired++;
    if (m_iShotsFired < 2) // 用於判斷單發/連發精確度邊界
    {
        if (m_flLastFire == 0.0f)
        {
            m_flLastFire = gpGlobals->time;
        }
        else
        {
            // 彈道退火衰減公式：基於兩次射擊之間的時間差，動態惡化單手平舉手槍時的震盪擴散值
            float flNewAccuracy = m_flAccuracy - (0.275f - (gpGlobals->time - m_flLastFire)) * 0.25f;
            
            // 盾牌手槍的精確度被上下限強行鎖死在 0.725f 到 0.92f 之間 (相比一般手槍極度劣化)
            if (flNewAccuracy <= 0.92f)
            {
                m_flAccuracy = (flNewAccuracy < 0.725f) ? 0.725f : flNewAccuracy;
            }
            else
            {
                m_flAccuracy = 0.92f;
            }
            m_flLastFire = gpGlobals->time;
        }

        // 4. 彈匣消耗與網絡骨骼動畫拓展同步
        if (m_iClip < 1)
        {
            if (m_fFireOnEmpty)
            {
                PlayEmptySound();
                m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
            }
            return;
        }

        m_iClip--;
        m_pPlayer->pev->effects |= EF_MUZZLEFLASH; // 槍口火光閃爍

        // 根據目前盾牌狀態，動態切換玩家在第三人稱（P模型）下的骨骼動畫擴充標籤 (m_szAnimExtention)
        // 收盾持槍狀態使用 "shieldgun"，完全防禦狀態（雖被攔截，此處為防呆）使用 "shield"
        if (!(m_iWeaponState & WSTATE_SHIELD_DRAWN) || !m_bShieldDrawn)
        {
            strcpy(m_pPlayer->m_szAnimExtention, "shieldgun");
        }
        else
        {
            strcpy(m_pPlayer->m_szAnimExtention, "shield");
        }
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        // 5. 多維後座力視角計算與射線探針發射
        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors(vecAngles);

        m_pPlayer->m_iWeaponVolume = 500;
        m_pPlayer->m_iWeaponFlash = 128; // 0x80

        Vector vecAutoaim;
        m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES);

        Vector vecSrc;
        m_pPlayer->GetGunPosition(vecSrc);

        // 結合運動姿態與此處退火算出的單手持盾精確度常數 (m_flAccuracy)
        // 計算隨機分量並調用 FireBullets3 進行 5.7x28mm 手槍彈藥（BULLET_PLAYER_57MM = 15）實時射線追蹤
        float flSpread = (1.0f - m_flAccuracy) * 0.255f;
        m_pPlayer->FireBullets3(vecSrc, vecAutoaim, flSpread, 0.15f, 2, BULLET_PLAYER_57MM, m_pPlayer->random_seed, m_flAccuracy);

        // 6. 調用預先快取的 events/shieldgun.sc 進行全域開火與盾牌受擊判定網絡事件廣播
        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireShieldGun, 0.0f, g_vecZero, g_vecZero, 
                            flSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 10000000.0f), 
                            (int)(m_pPlayer->pev->punchangle.y * 10000000.0f), (m_iClip == 0), 0);

        // 固定五七手槍 0.15 秒的擊發射速冷卻視窗
        m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
        
        // 槍口純視覺硬直回彈上跳效果
        m_pPlayer->pev->punchangle.x -= 2.0f;
    }
}

void CShieldGun::Reload(void)
{
    // 1. ?? 雙重安全防護鎖定 ??
    // 條件 A：若盾牌當前已完全拉起進入防禦狀態 (m_bShieldDrawn == true 且 WSTATE_SHIELD_DRAWN 成立)
    // 則不允許換彈！玩家必須保持露頭的常規持槍姿態才能進行彈匣更換。
    // 條件 B：玩家背包內必須還有剩餘的 57mm 手槍備用彈藥。
    if ((!(m_iWeaponState & WSTATE_SHIELD_DRAWN) || !m_bShieldDrawn) && 
        m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
    {
        // 2. 呼叫基礎裝填函數：最大裝填量 20 發（0x14）、動畫序列索引 4、換彈硬直冷卻時間 2.7 秒
        BOOL bResult = DefaultReload(20, 4, 2.7f);
        
        if (bResult)
        {
            // 3. 同步播放玩家在第三人稱視角下的通用換彈實體骨骼動作
            m_pPlayer->SetAnimation(PLAYER_RELOAD);

            // 4. 重置手槍射擊精度惡化矩陣：恢復至初始劣化上限 0.92f
            m_flAccuracy = 0.92f;
            return;
        }
    }
}

int CShieldGun::Restore(CRestore *restore)
{
    if (!CBasePlayerWeapon::Restore(restore))
    {
        return 0;
    }

    // ?? 讀載對稱還原：精確讀回防彈盾牌在反射表格中註冊的 2 個核心狀態欄位
    return restore->ReadFields("CShieldGun", this, m_SaveData, 2);
}

int CShieldGun::Save(CSave *save)
{
    if (!CBasePlayerWeapon::Save(save))
    {
        return 0;
    }

    // 與 Restore 完全對稱：精確向進度檔寫入戰術盾牌唯一註冊的 2 個核心變數 (m_bShieldDrawn, m_fMaxSpeed)
    return save->WriteFields("CShieldGun", this, m_SaveData, 2);
}

void CShieldGun::SecondaryAttack(void)
{
    unsigned int uState = m_iWeaponState;

    // ?? 狀態分流 A：當前處於常規持槍姿態，按下右鍵展開/拉起盾牌實施全防禦 ??
    // 0x20 位元對應 WSTATE_SHIELD_DRAWN 
    if ((uState & WSTATE_SHIELD_DRAWN) == 0 && m_bShieldDrawn == FALSE)
    {
        // 1. 全網寫入持盾位元標記
        m_iWeaponState = uState | WSTATE_SHIELD_DRAWN;

        // 2. 呼叫虛擬函數表索引 0x5b SendWeaponAnim 播放 Animation 7（拉起盾牌的過渡骨骼動畫）
        SendWeaponAnim(7, UseDecrement() != FALSE);

        // 3. 劫持第三人稱 P 模型骨骼，強行將擴展動畫標籤改寫為 "shield"，通知全網渲染正前方立盾外觀
        strcpy(m_pPlayer->m_szAnimExtention, "shield");
        m_bShieldDrawn = TRUE;

        // 4. ?? 物理運動學削砍：持盾全防禦姿態下，行軍最高步速被攔腰斬至極速 150.0f ??
        this->m_fMaxSpeed = 150.0f;
        m_pPlayer->m_bShieldDrawn = TRUE; // 同步改寫 CBasePlayer 內置的防護受擊判定標記
    }
    // ?? 狀態分流 B：當前正處於完全防禦姿態，按下右鍵放低/收回盾牌恢復常規射擊 ??
    else if (m_bShieldDrawn != FALSE)
    {
        // 1. 清除持盾位元標記：0xffffffdf (~WSTATE_SHIELD_DRAWN)
        m_iWeaponState = uState & ~WSTATE_SHIELD_DRAWN;

        // 2. 呼叫 SendWeaponAnim 播放 Animation 8（放下盾牌、露出右手五七手槍的過渡骨骼動畫）
        SendWeaponAnim(8, UseDecrement() != FALSE);

        // 3. 恢復第三人稱動畫標籤為 "shieldgun"，通知全網改回單手提槍外觀
        strcpy(m_pPlayer->m_szAnimExtention, "shieldgun");
        m_bShieldDrawn = FALSE;

        // 4. 物理速度釋放：將最高行軍步速拉回基礎手持盾牌速度 180.0f
        this->m_fMaxSpeed = 180.0f;
        m_pPlayer->m_bShieldDrawn = FALSE; // 清除玩家身上的受擊防護阻擋標記
    }
    else
    {
        return;
    }

LAB_0016be16:
    // 5. 畫面準星動態退火刷新：當 m_bShieldDrawn == 0（收盾）時傳入 true 還原常規準星
    // 當 m_bShieldDrawn == 1（舉盾防守）時傳入 false，強行在屏幕中央拉起特殊的盾牌窄視角防彈窗準星
    m_pPlayer->UpdateShieldCrosshair(m_bShieldDrawn == FALSE);

    // 6. 強制灌入 0.4 秒姿態切換冷卻硬直，防止玩家利用滑鼠右鍵巨集超高頻閃爍盾牌卡無敵判定
    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.4f;
    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.4f;
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6f;
}

void CShieldGun::ShieldGunFire(float flSpread, float flCycleTime, BOOL fUseSemi)
{
    unsigned int uState = m_iWeaponState;

    // 1. ?? 安全持盾開火過濾 ??
    // 0x20 位元對應 WSTATE_SHIELD_DRAWN。如果盾牌此時已經完全立起拉開 (m_bShieldDrawn == true)
    // 控制流會直接被卡死 return，手槍無法擊發！想要開火必須先按右鍵收回防禦
    if ((uState & WSTATE_SHIELD_DRAWN) && m_bShieldDrawn)
    {
        return;
    }

    m_iShotsFired++;
    if (m_iShotsFired < 2) // 用於判斷單發/連發精確度邊界
    {
        if (m_flLastFire == 0.0f)
        {
            m_flLastFire = gpGlobals->time;
        }
        else
        {
            // 彈道退火波形計算：基於前後開火的時間差，動態惡化單手平舉手槍時的震盪擴散值
            float flNewAccuracy = m_flAccuracy - (0.275f - (gpGlobals->time - m_flLastFire)) * 0.25f;
            
            // 盾牌手槍的精確度被上下限強行鎖死在 0.725f 到 0.92f 之間 (相比一般手槍極度劣化)
            if (flNewAccuracy <= 0.92f)
            {
                m_flAccuracy = (flNewAccuracy < 0.725f) ? 0.725f : flNewAccuracy;
            }
            else
            {
                m_flAccuracy = 0.92f;
            }
            m_flLastFire = gpGlobals->time;
        }

        // 2. 彈匣消耗與空倉攔截
        if (m_iClip < 1)
        {
            if (m_fFireOnEmpty)
            {
                PlayEmptySound();
                m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
            }
            return;
        }

        m_iClip--;
        m_pPlayer->pev->effects |= EF_MUZZLEFLASH; // 槍口火光閃爍

        // 根據目前盾牌狀態，動態切換玩家在第三人稱（P模型）下的骨骼動畫擴充標籤 (m_szAnimExtention)
        if (!(m_iWeaponState & WSTATE_SHIELD_DRAWN) || !m_bShieldDrawn)
        {
            strcpy(m_pPlayer->m_szAnimExtention, "shieldgun");
        }
        else
        {
            strcpy(m_pPlayer->m_szAnimExtention, "shield");
        }
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);

        // 3. 多維後座力視角計算與射線探針發射
        Vector vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
        UTIL_MakeVectors(vecAngles);

        m_pPlayer->m_iWeaponVolume = 500;
        m_pPlayer->m_iWeaponFlash = 128; // 0x80

        Vector vecAutoaim;
        m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES);

        Vector vecSrc;
        m_pPlayer->GetGunPosition(vecSrc);

        // 調用 FireBullets3 進行 5.7x28mm 手槍彈藥（BULLET_PLAYER_57MM = 15）實時射線追蹤
        float flFinalSpread = (1.0f - m_flAccuracy) * 0.255f;
        m_pPlayer->FireBullets3(vecSrc, vecAutoaim, flFinalSpread, flCycleTime, 2, BULLET_PLAYER_57MM, m_pPlayer->random_seed, m_flAccuracy);

        // 4. 調用預先快取的 events/shieldgun.sc 進行全域開火與盾牌受擊判定網絡事件廣播
        PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireShieldGun, 0.0f, g_vecZero, g_vecZero, 
                            flFinalSpread, vecAngles.x, (int)(m_pPlayer->pev->punchangle.x * 10000000.0f), 
                            (int)(m_pPlayer->pev->punchangle.y * 10000000.0f), (m_iClip == 0), 0);

        // 5. 刷新冷卻時脈
        // 暫存器優化噪訊修正：flSpread 引數在二進位中被複寫，實際傳遞的是射擊間隔 flCycleTime [1.32]
        m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

        if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1)
        {
            m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
        }

        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
        
        // 槍口純視覺硬直回彈上跳效果
        m_pPlayer->pev->punchangle.x -= 2.0f;
    }
}

BOOL CShieldGun::UseDecrement(void)
{
    // 扣動扳機擊發五七手槍時，確認消耗彈匣內部的實體子彈數量
    return TRUE;
}

void CShieldGun::WeaponIdle(void)
{
    // 1. 呼叫虛擬函數表索引 0x5a 處的基底 ResetEmptySound 接口重置空倉聲
    ResetEmptySound();

    // 2. 輔助瞄準探針維持：每幀為盾牌手槍刷新輔助瞄準的前視向量矩陣
    Vector vecAutoaim;
    m_pPlayer->GetAutoaimVector(vecAutoaim, AUTOAIM_5DEGREES); // 0.17364818

    // 3. 時脈安全邊界攔截：若尚未到達武器閒置的觸發時間點，則直接返回
    if (UTIL_WeaponTimeBase() < m_flTimeWeaponIdle)
    {
        return;
    }

    // 4. 重置閒置時間窗：設定高達 20.0 秒的超長安全閒置時脈
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;

    // 5. ?? 舉盾防禦姿態專屬的閒置動畫維護 ??
    // 0x20 位元對應 WSTATE_SHIELD_DRAWN。如果盾牌當前正完全立起在正前方（全防禦狀態）
    // 則必須將 SendWeaponAnim 播放的閒置動畫強行指定為 Animation 6（持盾完全遮擋姿態的專屬 Idle 骨骼動作）
    if (m_iWeaponState & WSTATE_SHIELD_DRAWN)
    {
        SendWeaponAnim(6, UseDecrement() != FALSE);
        return;
    }
}
