#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ggrenade.h" // 繼承自我們之前完美重構的 CGrenade 基底類別

class CLaws;

class CLawsRocket : public CGrenade
{
public:
    // 1. 基底實體虛擬表接口覆蓋與 Spawn
    void Spawn(void) override;
    void Precache(void) override;

    // 2. 火箭彈專屬多段式動力學與失速自毀狀態機鏈條
    void IgniteThink(void); // 剛出筒 0.1 秒：切換 MOVETYPE_FLY 點火、拉起全網 TE_BEAMFOLLOW 藍白尾煙
    void FollowThink(void); // 飛行中每 0.1 秒：1秒黃金加速、2000碼極速鎖死、低於 1500碼失速空中自毀殉爆
    void RocketTouch(CBaseEntity *pOther); // 撞擊瞬间：中斷飛火音效，控制流直接強制指派給 CGrenade::Detonate3 炸毀場景

public:
    // 3. 靜態工廠生成函數（處理 GCC 暫存器傳參嚴重錯位與右肩扛筒位置 3D 坐標偏移）
    static CLawsRocket *CreateLawsRocket(Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CLaws *pLauncher);

public:
    // 4. 火箭拋體特化私有成員變數（精確對齊 0x3ac = 940 位元組的私有數據尾部錨點）
    int m_iTrail;            // 儲存 sprites/smoke.spr 煙霧尾跡的精靈圖索引快取 ID
    float m_flIgniteTime;    // 記錄火箭主發動機正式點火的絕對時間戳記
    CLaws *m_pLauncher;      // 指向發射本枚火箭彈的 M72 LAW 武器夾具宿主指針
};

LINK_ENTITY_TO_CLASS(laws_rocket, CLawsRocket); 

void CLawsRocket::Precache(void)
{
    PRECACHE_MODEL("models/lawrocket.mdl");
    m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
    PRECACHE_SOUND("weapons/rocket1.wav");
}

void CLawsRocket::Spawn(void)
{
    Precache();

    pev->movetype = MOVETYPE_BOUNCE; // 10
    pev->solid = SOLID_BBOX;          // 2
    
    SET_MODEL(ENT(pev), "models/lawrocket.mdl");
    UTIL_SetSize(pev, g_vecZero, g_vecZero);
    UTIL_SetOrigin(pev, pev->origin);
	pev->classname = MAKE_STRING("laws_rocket");

    // 1. 綁定火箭彈的狀態機控制流
    SetThink(&CLawsRocket::IgniteThink);
    SetTouch(&CGrenade::ExplodeTouch); // 直接調用先前重構的通用穿透法線接觸引爆

    // 2. 修正發射俯仰角 (Pitch) 並計算初速度
    // 在火箭離線瞬間暫時拉起 30 度用以計算方向，隨後無縫校正回朝向向量
    pev->angles.x -= 30.0f;
    UTIL_MakeVectors(pev->angles);
    pev->angles.x = -(pev->angles.x + 30.0f);

    // 火箭彈剛離手時的點火前初始初速度為 250.0f（隨後在 IgniteThink 中實施二次噴射暴增）
    pev->velocity = gpGlobals->v_forward * 250.0f;
    pev->gravity = 0.5f;
    pev->nextthink = gpGlobals->time;

    // 🚨 威力定義：M72 LAW 戰術火箭彈直接基礎爆炸傷害為 125 點 🚨
    pev->dmg = 125.0f;
}

CLawsRocket *CLawsRocket::CreateLawsRocket(Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CLaws *pLauncher)
{
    edict_t *pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("laws_rocket"));
    if (!pEntity)
    {
        return nullptr;
    }

    // 分配比 0x3a0 (928位元組) 更大的 0x3ac (940位元組) 私有記憶體長度
    CLawsRocket *pRocket = (CLawsRocket *)GET_PRIVATE(pEntity);
    if (!pRocket)
    {
        pRocket = (CLawsRocket *)pfnPvAllocEntPrivateData(pEntity, sizeof(CLawsRocket));
        pRocket->CBaseMonster::CBaseMonster();
        pRocket->pev = &pEntity->v;
    }

    UTIL_SetOrigin(pRocket->pev, vecOrigin);
    pRocket->pev->angles = vecAngles; // 由暫存器傳參錯位的 vecOrigin.y 還原

    // 呼叫虛擬函數表索引 [0] 處的基底 Spawn() 初始化模型與基礎速度 250.0f
    pRocket->Spawn();

    // 🚨 狀態機覆蓋：將觸碰回呼強行改綁為火箭彈專屬的 RocketTouch（而非 Spawn 裡的 ExplodeTouch）
    pRocket->SetTouch(&CLawsRocket::RocketTouch);

    // 儲存發射這枚火箭的 M72 LAW 武器夾具宿主指針（由 vecAngles.x 暫存器噪訊還原）
    pRocket->m_pLauncher = pLauncher;

    // 將火箭的物理 owner 欄位綁定為玩家的 edict
    // 0x208（520位元組）偏移量精確讀取了 pOwner->edict() 的內部快取指針（由 vecOrigin.z 錯位還原）
    if (pOwner)
    {
        pRocket->pev->owner = pOwner->edict();
    }

    return pRocket;
}

void CLawsRocket::FollowThink(void)
{
    // 1. 每 0.1 秒刷新一次前視向量，並根據當前速度朝向即時修正火箭模型的實體旋轉角（Angles）
    UTIL_MakeAimVectors(pev->angles);
    Vector vecForward = gpGlobals->v_forward;

    Vector vecNewAngles;
    UTIL_VecToAngles(vecForward, vecNewAngles);
    pev->angles = vecNewAngles;

    // 2. 獲取當前火箭彈拋體的速度純量
    float flSpeed = pev->velocity.Length();

    // 🚨 火箭主引擎高速動能階段 🚨
    // 判定點火後的「一秒鐘黃金加速推進窗（time - m_flIgniteTime < 1.0）」
    if (gpGlobals->time - m_flIgniteTime < 1.0f)
    {
        // 實施線性瘋狂超載加速：在新向量方向賦予「當前速度 * 0.8 + 400.0f」的二次噴射推力
        float flNewSpeed = flSpeed * 0.8f + 400.0f;
        pev->velocity = vecForward * flNewSpeed + pev->velocity * 0.2f;

        // 水中推進限速與冒泡機制（全身沒入水中 waterlevel == 3）
        if (pev->waterlevel == WATERLEVEL_HEAD)
        {
            // 水中產生極高阻尼，速度一旦大於 300 則強制削平、鎖死在 300.0f
            if (pev->velocity.Length() > 300.0f)
            {
                pev->velocity = pev->velocity.Normalize() * 300.0f;
            }

            // 在水下移動軌跡上，利用前後外推向後拉出一條 TE_BUBBLETRAIL 軌跡泡泡鏈
            Vector vecFrom = pev->origin;
            Vector vecTo = pev->origin - pev->velocity * 0.1f;
            UTIL_BubbleTrail(vecFrom, vecTo, 32);
        }
        else
        {
            // 常規空氣中推進極限：將 M72 LAW 火箭彈的最大飛行極速強行封頂、鎖死在 2000.0f 碼
            if (pev->velocity.Length() > 2000.0f)
            {
                pev->velocity = pev->velocity.Normalize() * 2000.0f;
            }
        }
    }
    // 🚨 火箭燃料耗盡、滑行與定時自毀階段（發射超過 1 秒） 🚨
    else
    {
        // 如果身上還附加有引擎噴火特效（EF_LIGHT 或特定光暈位元 0x40），關閉特效，並全面關閉武器飛火音效
        if (pev->effects & EF_BRIGHTLIGHT) // 0x40
        {
            pev->effects = 0;
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/rocket1.wav", 0.0f, 0.0f, SND_STOP, PITCH_NORM);
        }

        // 燃料燃盡後失去了噴火推力，速度開始衰減（朝前視方向給予 0.798 的慣性保持與 20% 的原動能留存）
        pev->velocity = pev->velocity * 0.2f + vecForward * flSpeed * 0.798f;

        // 如果在空氣中滑行（waterlevel == 0）
        if (pev->waterlevel == WATERLEVEL_NOTINWATER)
        {
            // 燃料耗盡後，只要火箭彈因空氣阻力導致速度「跌落、低於 1500.0f」的臨界安全速度時
            // 為了防範火箭在外殼失去動能後軟綿綿掉在地上穿模，代碼會直接強制觸發定時自毀（Detonate）空中殉爆！
            if (pev->velocity.Length() < 1500.0f)
            {
                CGrenade::Detonate();
                return;
            }
        }
    }

    // 更新計時器步進率，下一次 Think 將在 0.1 秒後繼續執行
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CLawsRocket::IgniteThink(void)
{
    // 1. 將移動模式無縫切換為不具有空氣重力下墜的直線滑行模式 (MOVETYPE_FLY = 5)
    pev->movetype = MOVETYPE_FLY;

    // 2. 附加全域動態發光特效，在火箭外殼與尾部亮起強烈光源 (EF_BRIGHTLIGHT = 0x40)
    pev->effects |= EF_BRIGHTLIGHT;

    // 3. 播放火箭引擎持續燃燒飛火音效
    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/rocket1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);

    // 4. 🚀 引擎核心網路消息：向全地圖廣播拉出沖天白煙尾跡鏈 🚀
    // 十六進位 0x17 對應 SVC_TEMPENTITY，寫入 0x16 則代表 TE_BEAMFOLLOW (建立與實體綁定的尾跡線)
    MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY, nullptr);
    WRITE_BYTE(TE_BEAMFOLLOW);
    WRITE_SHORT(entindex());          // 將尾跡死死依附在本火箭的實體索引上
    WRITE_SHORT(m_iTrail);            // 快取的 sprites/smoke.spr 白色煙霧精靈圖
    WRITE_BYTE(40);                   // Life (0x28)：尾跡留存時間窗
    WRITE_BYTE(5);                    // Width：白煙粒子的寬度尺寸
    WRITE_BYTE(224);                  // Red (0xe0)
    WRITE_BYTE(224);                  // Green (0xe0)
    WRITE_BYTE(255);                  // Blue (0xff)：調配出高亮度的北約燃料燃燒微藍白白煙色彩
    WRITE_BYTE(255);                  // Brightness (Alpha 混色不透明度)
    MESSAGE_END();

    // 5. 記錄當前的點火絕對時間戳記 m_flIgniteTime
    m_flIgniteTime = gpGlobals->time;

    // 6. 狀態機無縫交棒！將 Think 控制流正式移交給我們上一步重構的動力學FollowThink
    SetThink(&CLawsRocket::FollowThink);
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CLawsRocket::RocketTouch(CBaseEntity *pOther)
{
    // 1. 瞬間切斷並完全關閉發射出去後的火箭主發動機燃燒飞火音效（SND_STOP = 0x20）
    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/rocket1.wav", 0.0f, 0.0f, SND_STOP, PITCH_NORM);

    // 2. 🚨 核心劫持：直接切換 Think 指針到通用劇情爆破與環境破壞殺傷函數 Detonate3
    SetThink(&CGrenade::Detonate3);

    // 3. 瞬間（在下一個引擎刻度/幀）觸發引爆
    pev->nextthink = gpGlobals->time;
}
