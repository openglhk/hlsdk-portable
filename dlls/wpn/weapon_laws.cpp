#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

// M72 LAW 火箭筒專屬中央火控狀態機枚舉
enum LAWS_STATE
{
    LAWS_STATE_READY = 0,    // 筒身完全拉開、已就緒閒置狀態
    LAWS_STATE_DEPLOYING,    // 正在拉長筒身或反向折疊筒身（硬直 3.266 秒）
    LAWS_STATE_FIRED,        // 剛剛扣動扳機、火箭彈射出瞬间（硬直 1.1 秒）
    LAWS_STATE_EMPTY_RETRACT // 彈藥已完全耗盡，正在播放 1.666 秒空管折疊並準備銷毀
};

class CLaws : public CBasePlayerWeapon
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    int GetItemInfo(ItemInfo *p) override;
    int iItemSlot(void) override;    // 固定返回 Slot 5（劫持 C4 槽位背在後背）
    float GetMaxSpeed(void) override; // 強制壓低至 199.0f（全遊戲最沉重步伐懲罰）

    BOOL Deploy(void) override;
    void Holster(int skiplocal = 0) override;
    void WeaponIdle(void) override;   // 核心自毀流：打完一發看過動畫後自動DestroyItem

    void PrimaryAttack(void) override;   // 核心火控：鎖定右肩偏移點物理孵化 laws_rocket
    void SecondaryAttack(void) override; // 完全留空直接 return

    // 🚀 CZDS 獨家客製化：多維狀態機中央分流矩陣（內嵌 Linux GCC 唯讀常數跳轉表）
    void SetWeaponState(unsigned short weaponState);

public:
    // 火箭筒專屬全域網路尾煙與發射事件（對齊 field_0x10e 記憶體偏移量）
    unsigned short m_usFireLaws; 
};

LINK_ENTITY_TO_CLASS(weapon_laws, CLaws); 

void CLaws::Precache(void)
{
    PRECACHE_MODEL("models/w_law.mdl");
    PRECACHE_MODEL("models/v_law.mdl");

    PRECACHE_SOUND("items/9mmclip1.wav");
    PRECACHE_SOUND("weapons/rocketfire1.wav");
    PRECACHE_SOUND("weapons/glauncher.wav");

    UTIL_PrecacheOther("laws_rocket");

    m_usFireLaws = PRECACHE_EVENT(1, "events/laws.sc");
}

void CLaws::Spawn(void)
{
    SET_MODEL(ENT(pev), "models/w_law.mdl");
    pev->frame = 0.0f;
    pev->body = 3;
    pev->sequence = 0;
    pev->framerate = 0.0f;

    Precache();

    m_iId = WEAPON_LAWS; // 0x15 = 十進位 21
    m_iDefaultAmmo = 1;
    m_iClip = WEAPON_NOCLIP; // -1 代表無彈匣，採用投擲物式的直接消耗流
    m_bIsAccessory = true;

    FallInit();

    pev->nextthink = UTIL_WeaponTimeBase() + 0.1f;
}

BOOL CLaws::Deploy(void)
{
    // 1. 呼叫基礎切槍函數，載入第一人稱 V 模型，播放 Animation 1（通常為拉開筒身的準備動作）
    BOOL bResult = DefaultDeploy("models/v_law.mdl", "models/p_law.mdl", 1, "laws", UseDecrement() != FALSE);

    // 2. 🚨 核心機制：劫持切槍流，強行注入 3.26 秒的「拉開火箭筒筒身」硬直狀態機 🚨
    // 0x65 虛擬表偏移量精確對齊了 SetWeaponState 接口
    if (GetWeaponState() == 0) 
    {
        // 播放拉開火箭筒、推入卡榫與解鎖瞄具的次級展開骨骼動畫
        SendWeaponAnim(1, UseDecrement() != FALSE);
        
        // 將火箭筒自身內部火控狀態標記為 1（通常代表已解鎖、展開就緒）
        m_WeaponState = 1;

        // 強行灌入高達 3.266 秒的超長冷卻硬直！
        // 在這三秒內，玩家無法開火（m_flNextAttack）、無法觸發閒置，必須完整看完把 LAW 筒身拉長的戰術動畫。
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.266f;
        m_pPlayer->m_flNextAttack = 3.266f; 
    }
    else
    {
        SetWeaponState(1);
    }

    return bResult;
}

int CLaws::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "66mm";
    p->iMaxAmmo1 = 1;
    p->pszAmmo2 = nullptr;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = WEAPON_NOCLIP; // 0 代表無彈匣，一發消耗制
    p->iSlot = 4;
    p->iPosition = 7;
    p->iId = WEAPON_LAWS; // 0x15 = 21
    p->iWeight = 2;
    p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE; // 0x18
    p->bIsAccessory = true;

    m_iId = WEAPON_LAWS;
    m_iPlayEmptySound = 1; // 記錄允許播放空倉脆響

    return 1;
}

float CLaws::GetMaxSpeed(void)
{
    return 199.0f;
}

void CLaws::Holster(int skiplocal)
{
    // 🚨 核心機制：劫持收槍流 🚨
    // 0x65 虛擬表偏移量精確對齊了 SetWeaponState 接口
    // 如果火箭筒處於完全展開狀態，收槍時會強行觸發反向狀態硬直，不允許瞬間切換武器
    if (GetWeaponState() == 0) 
    {
        // 播放將發射筒重新縮回、折疊並鎖定瞄具的反向折疊骨骼動畫 (Animation 1)
        SendWeaponAnim(1, UseDecrement() != FALSE);
        
        m_WeaponState = 1;

        // 強行在收槍時再度灌入 3.266 秒的超長動畫硬直窗口，防止玩家透過頻繁切槍逃避展開硬直
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.266f;
        m_pPlayer->m_flNextAttack = 3.266f;
        return;
    }

    SetWeaponState(1);
}

int CLaws::iItemSlot(void)
{
    // M72 LAW 火箭筒在物品欄中精確佔用第 5 格特殊武器槽位 (Slot 5)
    return 5;
}

void CLaws::PrimaryAttack(void)
{
    // 1. 安全校驗：檢查火箭筒是否處於展開就緒狀態 (m_WeaponState == 0 代表就緒)
    // 0x64 (100) 虛擬表偏移量精確對齊了 GetWeaponState 接口
    if (GetWeaponState() != 0)
    {
        return;
    }

    // 2. 劫持狀態機：將武器狀態切換至 2（已發射/空管），播放發射骨骼動畫 (Animation 2)
    // 並灌入 1.1 秒的冷卻硬直，防止開火瞬間立刻進行其他武器動作
    SendWeaponAnim(2, UseDecrement() != FALSE);
    m_WeaponState = 2;
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1f;
    m_pPlayer->m_flNextAttack = 1.1f;

    // 3. 注入開火聲響與槍火閃爍體積，同步播放玩家第一人稱開火骨骼動畫
    m_pPlayer->m_iWeaponVolume = 2250; // 0x8ca
    m_pPlayer->m_iWeaponFlash = 512;   // 0x200
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    // 4. 計算火箭彈的初始發射位置與向量矩陣（從玩家右肩扛筒位置射出）
    UTIL_MakeVectors(m_pPlayer->pev->v_angle);
    
    // 呼叫玩家虛擬表指針 [0x83] 取得精確的視線發射源起點點位置 (GetGunPosition)
    Vector vecGetGun;
    m_pPlayer->GetGunPosition(vecGetGun);

    // 結合前視、右偏與上舉向量，將發射點精確偏移至玩家右肩的火箭筒發射口
    Vector vecSrc = vecGetGun + gpGlobals->v_forward * 16.0f + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -8.0f;
    Vector vecAngles = m_pPlayer->pev->v_angle;

    // 5. 🚀 物理生成：在戰場上孵化一枚高爆火箭彈拋體實體 🚀
    CLawsRocket *pRocket = CLawsRocket::CreateLawsRocket(vecSrc, vecAngles, m_pPlayer);

    // 6. 動能補償演算（初速度與玩家自身移動慣性動能疊加）
    // 計算玩家當前移動方向在火箭飛行方向上的速度分量，並無縫加算至火箭彈的 velocity 三維速度向量中
    UTIL_MakeVectors(m_pPlayer->pev->v_angle);
    float flPlayerForwardSpeed = DotProduct(m_pPlayer->pev->velocity, gpGlobals->v_forward);
    pRocket->pev->velocity = pRocket->pev->velocity + gpGlobals->v_forward * flPlayerForwardSpeed;

    // 7. 調用 Precache 事先快取的 events/laws.sc 進行全域開火與發射尾煙網路事件廣播
    PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireLaws, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0);

    // 8. 摧毀機制：火箭筒一發即空，強行扣除所有 66mm 彈藥，並清空玩家身上的反裝甲裝備配件狀態位
    int iAmmoIndex = m_pPlayer->GetAmmoIndex("66mm");
    m_pPlayer->m_rgAmmo[iAmmoIndex] = 0;
    
    // 0xffdfffff 清除玩家身上的重型反裝甲武器 Accessory 狀態位 (ACCESSORY_LAWS)
    m_pPlayer->m_iAccessoryBits &= ~ACCESSORY_LAWS; 
}

void CLaws::SecondaryAttack(void)
{
    // M72 LAW 火箭筒沒有任何右鍵次要攻擊（如機械瞄具放大、近戰托擊或切換射擊模式）。
    return;
}

void CLaws::SetWeaponState(unsigned short weaponState)
{
    float flIdleDuration = 0.0f;
    float flAttackDuration = 0.0f;

    // 💡 核心優化矩陣還原：讀取 Linux GCC 內聯的靜態跳轉矩陣常數
    // 狀態 0：常規就緒閒置狀態窗
    // 狀態 1：展開 / 折疊筒身動作（硬直時間 3.266 秒）
    // 狀態 2：發射火箭彈空管動作（硬直時間 1.1 秒）
    switch (weaponState)
    {
        case 0:
            flIdleDuration = 0.0f;
            flAttackDuration = 0.0f;
            break;
        case 1:
            flIdleDuration = 3.266f;
            flAttackDuration = 3.266f;
            break;
        case 2:
            flIdleDuration = 1.1f;
            flAttackDuration = 1.1f;
            break;
        default:
            flIdleDuration = 0.0f;
            flAttackDuration = 0.0f;
            break;
    }

    // 呼叫虛擬函數表索引 SendWeaponAnim 播放對應狀態的 V 模型骨骼動畫
    SendWeaponAnim(weaponState, UseDecrement() != FALSE);
    
    // 更新火箭筒內部的核心火控狀態機欄位
    m_WeaponState = weaponState;

    // 將矩陣算出的戰術動畫時長，無縫灌入武器與玩家的 NextAttack 行動鎖定窗
    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flIdleDuration;
    m_pPlayer->m_flNextAttack = flAttackDuration;
}

void CLaws::WeaponIdle(void)
{
    if (m_flTimeWeaponIdle > gpGlobals->time)
    {
        return;
    }

    // 1. 取得火箭筒當前的狀態代碼
    unsigned short uState = GetWeaponState();

    // 2. 狀態分流機制：依據當前狀態機位置進行平滑閒置或強制銷毀
    if (uState == 1 || uState == 0)
    {
        // 狀態 0 或 1 ➔ 代表剛剛切槍展開完畢，將狀態收攏歸零（0：完全就緒閒置狀態）
        // 播放常規扛在肩上的第一人稱閒置動畫（Animation 0），並設定高達 20 秒的超長閒置窗
        SetWeaponState(0);
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
    }
    else if (uState == 2)
    {
        // 狀態 2 ➔ 代表剛剛左鍵開火擊發完畢，將狀態推進至 3（已打空，準備銷毀空管）
        // 播放收回、折疊或拋棄火箭筒的過渡動畫（Animation 3），並灌入 1.66 秒的動畫等待硬直
        SetWeaponState(3);
        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.666f;
    }
    else if (uState == 3)
    {
        // 狀態 3 ➔ 1.66 秒的空管過渡動畫看完了，火箭筒正式功成身退
        // 呼叫虛擬函數表 [0x61] RetractWeapon 接口收回武器，隨即執行 DestroyItem() 徹底銷毀發射筒實體
        RetractWeapon();
        DestroyItem();
    }
}
