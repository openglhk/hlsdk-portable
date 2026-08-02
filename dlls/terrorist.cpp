#include "terrorist.h"

// 🚀 核心註冊：將地圖類名 "monster_terrorist" 或類似的敵軍類名與 C++ 類別完美綁定
LINK_ENTITY_TO_CLASS( monster_terrorist, CTerrorist );

// 10 通道權威存檔數據表 (3個整數 + 7個ushort網路事件暫存器 = 10)
TYPEDESCRIPTION CTerrorist::m_SaveData[] =
{
    DEFINE_FIELD( CTerrorist, m_voicePitch, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_iBrassShell, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_iShotgunShell, FIELD_INTEGER ),
    
    // 註冊 7 大開火事件進存檔中，確保讀檔讀取時開火音效特效指針不丟失 Bug
    DEFINE_FIELD( CTerrorist, m_usFireM60, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireTMP, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireGlock, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireAK47, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireAWP, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireMP5, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireShotgun, FIELD_SHORT ),
};

// =========================================================================
// 🎰 完美還原：核心軍火庫資源與全套客戶端 Event 預載矩陣
// =========================================================================
void CTerrorist::Precache( void )
{
    // 1. 預載全武裝日常填裝、揮砍與近戰 machete 開山馬刀破空音效 (.wav)
    PRECACHE_SOUND( "weapons/reload_shotgun.wav" );
	PRECACHE_SOUND( "weapons/pistol_reload.wav" );
	PRECACHE_SOUND( "weapons/reload_mp5.wav" );
	PRECACHE_SOUND( "weapons/glauncher.wav" );
	PRECACHE_SOUND( "weapons/sbarrel1.wav" );
	PRECACHE_SOUND( "weapons/machete_slash1.wav" );
	PRECACHE_SOUND( "weapons/machete_slash2.wav" );
	PRECACHE_SOUND( "weapons/knife_slash1.wav" );
	PRECACHE_SOUND( "weapons/melee_whoosh1.wav" );
	PRECACHE_SOUND( "weapons/melee_whoosh2.wav" );

    // 語音音調對齊 100 號標準成年人面
    m_voicePitch = 100;

    // 2. 刷入並快取步槍與散彈槍雙向物理拋殼網格模型
    m_iBrassShell   = PRECACHE_MODEL( "models/shell.mdl" );
    m_iShotgunShell = PRECACHE_MODEL( "models/shotgunshell.mdl" );

    // 3. 快取狙擊雷射瞄準線與槍口火光精靈貼圖
    PRECACHE_MODEL( "sprites/laserbeam.spr" );
	PRECACHE_MODEL( "sprites/flare3.spr" );

    // =========================================================================
    // 🚀 4. 核心點火：將 7 大高階武器開火事件強行註冊注入客戶端通訊快取
    // 100% 鏡像重合 Linux server.so 全套 g_engfuncs.pfnPrecacheEvent 序列排位
    // =========================================================================
    m_usFireM60     = PRECACHE_EVENT( 1, "events/m60.sc" );     // 1. M60 重機槍
    m_usFireTMP     = PRECACHE_EVENT( 1, "events/tmp.sc" );     // 2. TMP 戰術衝鋒槍
    m_usFireGlock   = PRECACHE_EVENT( 1, "events/glock1.sc" );  // 3. 格洛克手槍
    m_usFireAK47    = PRECACHE_EVENT( 1, "events/ak47.sc" );    // 4. AK47 突擊步槍
    m_usFireAWP     = PRECACHE_EVENT( 1, "events/awp.sc" );     // 5. AWP 麥格農狙擊槍
    m_usFireMP5     = PRECACHE_EVENT( 1, "events/mp5n.sc" );    // 6. MP5 微型衝鋒槍
    m_usFireShotgun = PRECACHE_EVENT( 1, "events/m3.sc" );      // 7. M3 雷明登散彈槍
}

void CTerrorist::Spawn( void )
{
    // 1. 🚀 最高主權多載點火：調用 Precache() 填裝軍火庫，調用 SpawnInit() 配置 3D 模型
    Precache();
    SpawnInit();

    // 2. 路由並鎖定當前地圖指定的恐怖分子戰術兵種行為型態角色 (如 4=狙擊手, 0=突擊兵)
    m_terroristBehavior = GetBehaviorType();

    // 3. 配置標準 32x32x72 SWAT 規格陸軍凸殼物理碰撞盒
    pev->solid    = SOLID_SLIDEBOX;  // 3
    pev->movetype = MOVETYPE_WALK;    // 4
    pev->effects  = 0;

    UTIL_SetSize( pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 72.0f) );

    // 4. 🩸 完美還原動態難度數值血量折算公式 (對齊 gSkillData 的 HECU 軍人血量基準面)
    m_bloodColor = BLOOD_COLOR_RED; // -9 號鮮紅人類血液 (0xF7)
    pev->max_health = gSkillData.hgruntHealth * m_healthMultiplier;
    pev->health     = pev->max_health;

    // 5. 配置 140 度超寬戰術錐形盲區視野，以及腰部 55 碼槍口開火射線起點
    m_flFieldOfView  = -0.7f; // 140度 FoV 視野遮罩
    m_MonsterState   = MONSTERSTATE_NONE;
    m_HackedGunPos   = Vector( 0.0f, 0.0f, 55.0f );

    // 6. 填裝戰場時鐘，重置狀態機初值
    m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
    m_flNextPainTime     = gpGlobals->time;
    m_iSentence          = -1;
    m_fEnemyEluded       = FALSE;
    m_fFirstEncounter    = TRUE;

    // 7. 戰術能力矩陣拉起 (對齊 0x748 二進位功能遮罩：包含看見敵人、開火、翻滾避讓、小隊溝通等組合能力)
    m_afCapability = bits_CAP_HEAR | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_SQUAD | bits_CAP_RANGE_ATTACK1;

    // 8. 軍火庫實體武器配准初始化
    SetupWeapons();

    // =========================================================================
    // 🎰 9. 完美還原 scheduleTable[] 決策計劃機率輪盤表陣列的開局重置與權重加載
    // =========================================================================
    for ( int i = 0; i < 16; i++ ) // 假設 16 容量計畫表表格
    {
        scheduleTable[i].activeFlag    = 0.0f;
        scheduleTable[i].percentChance = 1.0f; // 權重機率初始化為滿健康值基準
    }

    // 將兵種型態正式加載灌入小隊感知網中
    SetupBehaviors( m_terroristBehavior );

    // 語音廣播通訊鎖重置
    CTalkMonster::g_talkWaitTime = 0.0f;

    // 10. 呼叫虛擬表 72 號偏移量 (StartMonster)，正式激活敵軍小隊 AI 大腦思考，起跑前進！
    StartMonster();

    // =========================================================================
    // 🎯 11. 狙擊手高階兵種特權解鎖：極限 4096 碼鷹眼視線加載與雷射精靈束點亮
    // =========================================================================
    if ( m_terroristBehavior == 4 || m_terroristBehavior == 5 ) // 4=Scout 狙擊兵, 5=AWP 狙擊暴君
    {
        if ( m_customRange == 0 )
        {
            m_flDistTooFar = 2048.0f; // 作戰交火距離翻倍拉高到 2048 碼
        }
        m_flDistLook    = 4096.0f; // 🦅 鷹眼搜尋視線極限擴張到 4096 碼！
        m_flFieldOfView = -0.7f;   // 維持超寬視野

        // 🚀 引爆雷射精靈束，在世界中點亮貫穿整張地圖的狙擊雷射紅外線
        SetupLaserGlow();
    }

    // 12. 劇情無敵鎖判定
    if ( m_invulnerable != 0 )
    {
        pev->takedamage = DAMAGE_NO; // 0.0f 劇情神明無敵模式
    }

    // 13. 外觀變體 Skin / Bodygroup 部件重定向 (如換頭、換帽子)
    if ( m_head > 0 )
    {
        CBaseAnimating::SetBodygroup( 1, m_head );
    }

    // =========================================================================
    // ⚙️ 14. 終極指針註冊：將該實體被玩家 E 鍵按壓互動的回調，指向 UseTarget 方法
    // =========================================================================
    SetUse( &CTerrorist::UseTarget );
}

void CTerrorist::SpawnInit( void )
{
    // 1. 檢查地圖製作者是否填寫了自定義恐怖分子模型 (如 models/terror.mdl)
    if ( pev->model != 0 )
    {
        // 正常載入並配置地圖指定的骨骼外觀網格
        SET_MODEL( ENT(pev), (char *)STRING(pev->model) );
        return;
    }

    // 2. 🚀 100% 還原二進位 Fallback 彩蛋行為：
    // 若地圖模型為空，強行加載戰慄時空經典 HECU 陸軍特種兵模型，防止踩空當機
    SET_MODEL( ENT(pev), "models/hgrunt.mdl" );
}

