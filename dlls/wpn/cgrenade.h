#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"

// CZDS 遙控炸藥/C4 矩陣控制列枚舉
enum SATCHELCODE
{
    SATCHEL_DETONATE = 1,
    SATCHEL_RELEASE
};

class CGrenade : public CBaseMonster
{
public:
    // 1. 基底核心虛擬表回呼與實體 Spawn
    void Spawn(void) override;
    int ObjectCaps(void) override; // [0x10] 判定：控制 C4 實體是否允許跨地圖關卡轉移

    int Save(CSave *save) override;
    int Restore(CRestore *restore) override;
    int BloodColor(void) override; // 固定返回 DONT_BLEED (-1) 防止非生物噴血

    // 2. 牆面、斜坡與物體幾何碰撞核心物理反應
    void BounceTouch(CBaseEntity *pOther);
    void SlideTouch(CBaseEntity *pOther);
    virtual void BounceSound(void); // 依據 pev->dmg 區分重型 HE 或輕型閃光/煙霧撞牆音效

    // 3. 多載爆炸傷害與視覺效果渲染主核心
    void Explode(Vector vecSrc, Vector vecAim);
    void Explode(TraceResult *pTrace, int bitsDamageType);   // 閃光彈全域致盲核心 (RadiusFlash)
    void Explode2(TraceResult *pTrace, int bitsDamageType);  // C4 轟天大爆炸核心 (多維亂數火球矩陣)
    void Explode3(TraceResult *pTrace, int bitsDamageType);  // 關卡劇情場景破壞殺傷核心 (RadiusDamage)
    void ExplodeTouch(CBaseEntity *pOther);                  // 接觸即爆拋體（如槍掛 M203 榴彈）探針射線
    
    // 4. 定時與腳本劫持引爆前置過渡
    void Detonate(void);     // 通用計時引爆（閃光/高爆）
    void Detonate2(void);    // C4 計時引爆
    void Detonate3(void);    // 特殊劇本/防碎屍引爆
    void PreDetonate(void);  // 預引爆緩衝（強行釘下 DANGER 聲音驅散 Bot 逃跑演出）
    void DetonateUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

    // 5. 反恐精英 C4 包點埋設、玩家按 E 拆彈雙迴路狀態機
    void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
    void C4Think(void);  // 每 0.12 秒心跳：10%等比嗶嗶聲退火、空中拆彈攔截與 MVP 結算
    void C4Touch(CBaseEntity *pOther); // 全空 return 防止 C4 埋設後被身體撞走或卡死地圖

    // 6. 戰術煙霧彈核心狀態機
    void SG_Detonate(void);
    void SG_Explode(TraceResult *pTrace, int bitsDamageType);
    void SG_Smoke(void);       // 12方向圓周正餘弦發射矩陣，持續 21 秒
    void SG_TumbleThink(void); // 煙霧彈著地減速與水下防呆

    // 7. 通用計時空中翻滾與放煙尾流控制
    void TumbleThink(void);      // 通用計時器：依據 pev->dmg <= 40.0f 分流引爆閃光或高爆
    void DangerSoundThink(void); // 0.5 秒未來時空飛行軌跡預測，即時嚇跑沿途敵人 AI
    void Smoke(void);            // 手榴彈完結篇：TE_SMOKE 規模 25 塵土廣播 / 水下防呆
    void Smoke2(void);           // C4 完結篇：TE_SMOKE 規模 150 巨型蘑菇塵暴廣播
    void Smoke3_A(void);         // 劇情破壞粉塵 A 組不規則外推粒子
    void Smoke3_B(void);         // 劇情破壞粉塵 B 組交錯粒子，0.15 秒後交棒給 A 組
    void Smoke3_C(void);         // 劇情破壞粉塵起手式（經編譯器死碼剪裁，僅保留水下冒泡）

public:
    // 8. 靜態工廠生成函數鏈（Static Factory Methods）
    static CGrenade *ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, float damage, int throwerClasstype);
    static CGrenade *ShootTimed2(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, int iTeam, unsigned short usEvent);
    static CGrenade *ShootSmokeGrenade(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent);
    static CGrenade *ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity); // M203 榴彈
    static CGrenade *ShootSatchelCharge(entvars_t *pevOwner, Vector vecStart, Vector vecAngles);   // C4 實體化
    
    // 全域群組引爆管理列（用於關卡腳本一瞬間並行引爆、或者沒收玩家遙控權）
    static void UseSatchelCharges(entvars_t *pevOwner, SATCHELCODE code);

public:
    // 9. 反射存檔核心表格
    static TYPEDESCRIPTION m_SaveData[16]; // 0x10 對齊：硬碟讀寫 16 個關鍵私有成員

public:
    // 10. 私有與成員變數欄位（完全對齊 0x3a0 = 928 位元組私有數據長度）
    BOOL m_fRegisteredSound;
    BOOL m_bIsC4;
    int m_iBoxClass;

    // 煙霧彈擴展
    int m_SGSmoke;
    int m_angle;
    unsigned short m_usEvent;
    BOOL m_bLightSmoke;
    BOOL m_bDetonated;
    int m_throwerClasstype; // 陣營分類（用於注入 weapon_effectspoint）

    // C4 狀態機專屬
    float m_flNextFreq;          // 嗶嗶聲計時
    float m_flNextFreqInterval;  // *0.9f 響起間隔等比級數遞減
    float m_flNextBeep;
    float m_flNextBlink;         // 紅光閃爍計時
    float m_flC4Blow;            // 35/45秒爆炸終點
    int m_iCurWave;              // 當前音效波形
    float m_fAttenu;             // 聲音衰減半徑
    const char *m_sBeepName;     // 嗶嗶聲音頻路徑
    edict_t *m_pentCurBombTarget; // 綁定的 func_bomb_target 包點實體

    // 拆彈控制專屬
    BOOL m_bStartDefuse;
    CBaseEntity *m_pBombDefuser; // 當前拆彈 CT 玩家指針
    float m_flDefuseCountDown;
    float m_fNextDefuse;         // 0.5 秒雙腳離地反作弊檢驗窗
    
    // 拋體遺留
    BOOL m_bJustBlew;
};
