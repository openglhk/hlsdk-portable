#include "cterrorist.h"

LINK_ENTITY_TO_CLASS( monster_counter_terrorist_gign,     CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_counter_terrorist_gsg9,     CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_counter_terrorist_repel,    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_counter_terrorist_sas,      CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_counter_terrorist_spetsnaz, CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_counter_terrorist_swat,     CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct,                         CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_dead,                    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_repel,                   CCounterTerrorist );

// 2. GIGN (法國國家憲兵干預組) 全武器矩陣
LINK_ENTITY_TO_CLASS( monster_ct_gign,              CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_assaultrifle,  CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_grenader,      CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_law,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_machinegun,    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_mp5,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_pistol,        CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_shotgun,       CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_smg,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gign_sniperrifle,   CCounterTerrorist );

// 3. GSG9 (德國邊防第九反恐部隊) 全武器矩陣
LINK_ENTITY_TO_CLASS( monster_ct_gsg9,              CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_assaultrifle,  CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_grenader,      CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_law,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_machinegun,    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_mp5,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_pistol,        CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_shotgun,       CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_smg,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_sniperrifle,   CCounterTerrorist );

// 4. SWAT (美國特警) 全武器矩陣
LINK_ENTITY_TO_CLASS( monster_ct_swat,              CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_assaultrifle,  CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_grenader,      CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_law,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_machinegun,    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_mp5,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_pistol,        CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_shotgun,       CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_smg,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_swat_sniperrifle,   CCounterTerrorist );

// 5. SAS (英國空降特勤隊) 全武器矩陣
LINK_ENTITY_TO_CLASS( monster_ct_sas,              CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_assaultrifle,  CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_grenader,      CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_law,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_machinegun,    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_mp5,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_pistol,        CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_shotgun,       CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_smg,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_sas_sniperrifle,   CCounterTerrorist );

// 6. Spetsnaz (俄羅斯特種部隊) 全武器矩陣
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz,              CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_assaultrifle,  CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_grenader,      CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_law,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_machinegun,    CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_mp5,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_pistol,        CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_shotgun,       CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_smg,           CCounterTerrorist );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_sniperrifle,   CCounterTerrorist );

TYPEDESCRIPTION CCounterTerrorist::m_SaveData[] =
{
    // ?? 1~5: 兵種與行為核心變數
    DEFINE_FIELD( CCounterTerrorist, m_ctBehavior, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_behaviorType, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_canWander, FIELD_BOOLEAN ),
    DEFINE_FIELD( CCounterTerrorist, m_wanderTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_wanderOrigin, FIELD_POSITION_VECTOR ),

    // ?? 6~9: 地圖事件、多國語言與變體分流
    DEFINE_FIELD( CCounterTerrorist, m_useTarget, FIELD_STRING ),
    DEFINE_FIELD( CCounterTerrorist, m_ctType, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_language, FIELD_STRING ),
    DEFINE_FIELD( CCounterTerrorist, m_bFollowStuck, FIELD_BOOLEAN ),

    // ?? 10~15: 戰術無線電與語音排隊系統
    DEFINE_FIELD( CCounterTerrorist, m_iSentence, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_flStopTalkTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_flStareTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_flNextPainTime, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_flNextGrenadeCheck, FIELD_TIME ),
    DEFINE_FIELD( CCounterTerrorist, m_fThrowGrenade, FIELD_BOOLEAN ),

    // ?? 16~19: 關卡外觀、無敵標記與 3D 黃黃銅彈殼索引
    DEFINE_FIELD( CCounterTerrorist, m_head, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_invulnerable, FIELD_BOOLEAN ),
    DEFINE_FIELD( CCounterTerrorist, m_iBrassShell, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_iShotgunShell, FIELD_INTEGER ),

    // ?? 20~23: 戰術道具受干擾熔斷狀態
    DEFINE_FIELD( CCounterTerrorist, m_iWeaponEffectType, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_bWeaponEffectActive, FIELD_BOOLEAN ),
    DEFINE_FIELD( CCounterTerrorist, taskFailCount, FIELD_FLOAT ),
    DEFINE_FIELD( CCounterTerrorist, m_fFirstEncounter, FIELD_BOOLEAN ),

    // ?? 24~28: 武器彈藥基數與動態彈道精確度向量
    DEFINE_FIELD( CCounterTerrorist, m_cClipSize, FIELD_INTEGER ),
    DEFINE_FIELD( CCounterTerrorist, m_dropItem, FIELD_STRING ),
    DEFINE_FIELD( CCounterTerrorist, m_dropChance, FIELD_FLOAT ),
    DEFINE_FIELD( CCounterTerrorist, m_fPreferedRange, FIELD_FLOAT ),
    DEFINE_FIELD( CCounterTerrorist, m_weaponAccuracy, FIELD_VECTOR ), // ?? 剛好第 28 個欄位！
};

int  g_fCTQuestion = 0;

CCounterTerrorist::CCounterTerrorist()
{
    m_ctBehavior         = 0;
    m_behaviorType       = 0;
    m_flNextGrenadeCheck = 0.0f;
    m_flNextPainTime     = 0.0f;
    m_iSentence          = -1;
    m_fFirstEncounter    = 1;
    m_healthMultiplier   = 1.0f; 
    m_ctType             = 0;
    m_language           = 0;
    m_cantMove           = 0;
    m_pBeam              = nullptr;
    m_pLaserGlow         = nullptr;
    m_voicePitch         = 100;
    m_iBrassShell        = 0;
    m_iShotgunShell      = 0;
    m_usFireM60 = m_usFireUSP = m_usFireAK47 = m_usFireM4A1 = 0;
    m_usFireScout = m_usFireMAC10 = m_usFireShotgun = m_usFireMP5 = 0;
    m_fThrowGrenade      = FALSE;
    m_vecTossVelocity    = g_vecZero;
    m_classtype          = 16;         // 16 即 0x10 (CLASS_PLAYER_ALLY)
    m_forcedTarget       = 0;          // 0 代表無地圖強制劇情目標
    m_bStanding          = TRUE;
	m_flStopTalkTime     = 0.0f;
    m_wanderTime         = 0.0f;
    m_canWander          = TRUE; // 預設允許自由走動
    m_wanderOrigin       = g_vecZero;
    justShotFlag         = FALSE;
    m_fPreferedRange     = 512.0f; // 預設黃金交戰距離為 512 碼
    m_bFollowStuck 	     = FALSE;
    m_iWeaponEffectType   = 0;
    m_bWeaponEffectActive = FALSE;
	m_cClipSize   = 30; // 預設突擊步槍滿彈夾 30 發
    m_dropItem    = 0;
    m_dropChance  = 0.0f;
	m_flStareTime = 0.0f;
	m_laserBrightness     = 0.0f;
    m_glowBrightness      = 0.0f;
    m_flLastEnemySightTime = 0.0f;
    m_fEnemyEluded         = FALSE;
	m_weaponAccuracy = g_vecZero; // 預設一出生時首發精確度最高
    m_invulnerable       = FALSE; // 預設不是無敵狀態
    m_head               = 0;     // 預設採用 0 號標準迷彩皮膚
	m_useTarget = 0;
    memset( scheduleTable, 0, sizeof(scheduleTable) );
    if ( pev ) 
    {
        pev->nextthink = 0.0f;
    }
}

void CCounterTerrorist::Precache( void )
{
    PRECACHE_SOUND( "weapons/reload_shotgun.wav" );
    PRECACHE_SOUND( "weapons/pistol_reload.wav" );
    PRECACHE_SOUND( "weapons/reload_mp5.wav" );
    PRECACHE_SOUND( "weapons/glauncher.wav" );
    PRECACHE_SOUND( "weapons/sbarrel1.wav" );
    PRECACHE_SOUND( "weapons/knife_slash1.wav" );

    // ?? 強鎖特警美美式精英軍人語音標準音調偏置 (100)
    m_voicePitch = 100;

    // =========================================================================
    // ?? 2. 3D金屬/塑料動態拋殼粒子、與紅外線雷射精靈貼圖預載
    // =========================================================================
    m_iBrassShell   = PRECACHE_MODEL( "models/shell.mdl" );         // 黃銅小彈殼模型
    m_iShotgunShell = PRECACHE_MODEL( "models/shotgunshell.mdl" );  // 紅色塑料散彈大殼模型
    
    PRECACHE_MODEL( "sprites/laserbeam.spr" );                     // 狙擊雷射線射線
    PRECACHE_MODEL( "sprites/flare3.spr" );                        // 槍口光暈精靈

    // =========================================================================
    // ?? 3. 特警 8 大核心現代化輕重武器 Client-Side 網路 Delta 事件同步句柄註冊
    // =========================================================================
    m_usFireM60     = PRECACHE_EVENT( 1, "events/m60.sc" );
    m_usFireUSP     = PRECACHE_EVENT( 1, "events/usp.sc" );
    m_usFireAK47    = PRECACHE_EVENT( 1, "events/ak47.sc" );
    m_usFireM4A1    = PRECACHE_EVENT( 1, "events/m4a1.sc" );
    m_usFireScout   = PRECACHE_EVENT( 1, "events/scout.sc" );
    m_usFireMAC10   = PRECACHE_EVENT( 1, "events/mac10.sc" );
    m_usFireShotgun = PRECACHE_EVENT( 1, "events/xm1014.sc" );
    m_usFireMP5     = PRECACHE_EVENT( 1, "events/mp5n.sc" );
}

void CCounterTerrorist::SpawnInit( void )
{
    // =========================================================================
    // ?? 1. 優先級一：若關卡設計師指定了特殊實體模型 (如 sas.mdl / gign.mdl)
    // 則優先放行綁定，將 3D 三維骨骼網格加載進當前特警實體中
    // =========================================================================
    if ( pev->model != 0 )
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
        return;
    }

    // =========================================================================
    // ?? 優先級二：100% 還原二進位行為，基準降級相容，強鎖 HGrunt 陸軍模型鋼印
    // 確保骨骼動作解碼鏈與 Vanilla 引擎底層網格達成微米級精度對齊
    // =========================================================================
    SET_MODEL( edict(), "models/hgrunt.mdl" );
}

void CCounterTerrorist::Spawn( void )
{
    // 1. 優先拉起小隊怪物類別母體 (CSquadMonster) 的虛擬表基礎構造
    CSquadMonster::Spawn();
    
    // 呼叫虛擬表 0x224 偏移方法 (初始化小隊成員結構體網絡)
    // 對應二進位 _vptr + 0x224
    InitSquadData();

    // ?? 2. 戰術兵種型態反查與幾何凸殼尺寸配准
    m_ctBehavior = GetBehaviorType();

    Vector vecMin = Vector( -16.0f, -16.0f, 0.0f );
    Vector vecMax = Vector( 16.0f,  16.0f,  72.0f );
    UTIL_SetSize( pev, vecMin, vecMax );

    pev->solid    = SOLID_SLIDEBOX; // 3: 滑動尋路碰撞箱
    pev->movetype = MOVETYPE_STEP;  // 4: 尋路步進物理

    // ?? ?? 100% 還原二進位大廠機密：複用 HGrunt 人類陸軍常規血量，注入 247號 紅色血液鋼印
    pev->health   = gSkillData.hgruntHealth * m_healthMultiplier;
    m_bloodColor  = BLOOD_COLOR_RED; // 247 = 0xF7
    pev->effects  = 0;

    m_flFieldOfView = 0.1f; // 預設常規視角夾角
    m_MonsterState  = MONSTERSTATE_NONE;

    // 開闢特警特有戰術技能位元遮罩：支援掩體換彈、開火移動與高級聽覺
    m_afCapability  = bits_CAP_MOVE_SHOOT | bits_CAP_TAKE_COVER | bits_CAP_HEAR | bits_CAP_RANGE_ATTACK1; // 0x748
    
    m_flNextGrenadeCheck = gpGlobals->time + 1.0f; // 1秒後啟動反手投擲手榴彈排查
    m_flNextPainTime     = gpGlobals->time;
    m_iSentence          = -1;
    m_fEnemyEluded       = FALSE;
    m_fFirstEncounter    = TRUE;

    // ?? 格式化特警專屬武器槍口高度偏置 (相對於腳底板向上 55 碼)
    m_HackedGunPos       = Vector( 0.0f, 0.0f, 55.0f );

    // 加載特警軍火庫
    SetupWeapons();

    // =========================================================================
    // ?? 16 容量計畫表機率輪盤全線格式化歸位 (do-while 內存清空鏡像)
    // =========================================================================
    for ( int i = 0; i < 16; i++ )
    {
        scheduleTable[i].activeFlag    = 0.0f;
        scheduleTable[i].percentChance = 1.0f;
    }

    // 依據當前特警行為代號，重寫計畫表參數性格
    SetupBehaviors( m_ctBehavior );
    CTalkMonster::g_talkWaitTime = 0.0f; // 語音信道開局暢通

    // 呼叫虛擬表 0x120 偏移方法 (拉起動畫序列配准)
    // 對應二進位 _vptr + 0x120
    ResetSequenceInfo();

    // =========================================================================
    // ?? 狙擊手與火箭筒兵特權：4096碼超視距廣角感知視線、與紅外線雷射構造
    // =========================================================================
    if ( m_ctBehavior == 4 || m_ctBehavior == 5 ) // 4: Sniper, 5: LAW
    {
        if ( m_customRange == 0 )
        {
            m_flDistTooFar = 2048.0f; // 鎖定遠程火力範圍
        }
        m_flDistLook    = 4096.0f; // ?? 4096碼 超視距老鷹眼神
        m_flFieldOfView = -0.7f;   // ?? 270度 戰術超廣角防繞後防禦
        
        SetupLaserGlow(); // 動態點亮特警紅外線雷射軌跡
    }

    // 霸體過濾
    if ( m_invulnerable != 0 )
    {
        pev->takedamage = DAMAGE_NO; // 0.0f
    }

    // 模型 Bodygroup 頭飾自適應加載
    if ( m_head > 0 )
    {
        CBaseAnimating::SetBodygroup( 1, m_head );
    }

    // =========================================================================
    // ?? 戰術核心：強鎖 FollowerUse 接口指標
    // 允許玩家走過去按下 E 鍵引爆跟隨/撤離狀態機，成為最溫暖的 Companion 小隊！
    // =========================================================================
    SetUse( &CCounterTerrorist::FollowerUse );
}

void CCounterTerrorist::UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 1. 檢查地圖製作者是否為此特警配置了自定義觸發目標 (0 代表無觸發)
    if ( m_useTarget == 0 )
    {
        return; 
    }

    // 2. ?? 完美還原：向全伺服器地圖發送事件網絡廣播，遠程喚醒並觸發指定的關卡地圖實體
    FireTargets( STRING(m_useTarget), pActivator, this, useType, value );
}

void CCounterTerrorist::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
    if ( ptr == nullptr )
    {
        return;
    }

    // =========================================================================
    // ??? 核心黑科技：戰術防爆盾牌 / 鋼鐵頭盔跳彈與強制免傷物理矩陣
    // (CS:CZDS 規範中 HITGROUP_SHIELD = 11)
    // =========================================================================
    if ( ptr->iHitgroup == 11 ) 
    {
        // 讀取 1 號外觀部件組，核對特警目前是否配戴了標準戰術全防護防爆頭盔
        int iHeadGroup = GetBodygroup( 1 );
        
        // 鋼鐵阻擋判定：若是標準防護面具 (0) 且子彈威脅在頭盔防禦力之內
        // (bitsDamageType & 0xC6 通常對應子彈與打擊傷害 bits_DMG_BULLET | bits_DMG_CLUB)
        if ( iHeadGroup == 0 && (bitsDamageType & (bits_DMG_BULLET | bits_DMG_CLUB)) )
        {
            // 傷害驟減：子彈威力強行砍掉 20 點防禦力
            flDamage -= 20.0f;
            
            if ( flDamage <= 0.0f )
            {
                // 100% 還原：在特警中彈的鋼鐵面罩/盾牌表面，即時產生火花與金屬跳彈彈開特效
                UTIL_Ricochet( ptr->vecEndPos, 1.0f );
                
                // 強制將傷害壓制到 0.01 點微光刮痧 (鋼鐵免傷)
                flDamage = 0.01f;
            }
        }

        // 防禦溢出修正：將中彈部位重新歸入 1 號頭部槽位 (HITGROUP_HEAD = 1)，交由常規防彈衣護甲計算
        ptr->iHitgroup = 1; 
    }

    // 3. 最終放行：將修正後的減傷數值交由基底類別方法去渲染受擊火網
    CSquadMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

void CCounterTerrorist::Touch( CBaseEntity *pOther )
{
    // 1. 安全攔截：若踫撞物體無效，直接返回
    if ( pOther == nullptr )
    {
        return;
    }

    // 2. ?? 防卡死黑科技：一旦受到物理踫撞擠壓，立即將遊蕩漫步等待時鐘強行歸零
    // 這會逼迫大腦在下一幀的 GetSchedule() 中立即重新尋路走開，實現戰術讓路避讓
    m_wanderTime = 0.0f;
}

void CCounterTerrorist::Talk( float flDuration )
{
    // 完美還原：以當前遊戲時間為基準，向後注入指定的語音說話持續時長
    m_flStopTalkTime = gpGlobals->time + flDuration;
}

void CCounterTerrorist::ShootSniperRifle( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目開槍
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x20c 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // 4. 幾何散佈運算：若當前長槍精確度未配置，套用標準 CS 狙擊槍百步穿楊極限散佈基數
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.04362f, 0.04362f, 0.04362f ); // 狙擊步槍專屬超精密錐度
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (1 發，麥格農狙擊彈，2048 碼最大飛行穿透距離)
    FireBullets( 1, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 點亮槍口閃光特效 (?? 100% 還原二進位行為：此兵種豁免子彈消耗，由拉栓動畫維持射速間隔)
    pev->effects |= EF_MUZZLEFLASH; // 2

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 產生狙擊槍巨大的後座力抬頭震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    // =========================================================================
    // ?? 幾何衝量運算：計算 Steyr Scout 右側機匣排殼窗在 3D 空間中的動態拋殼力道
    // =========================================================================
    Vector vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

    // 狙擊槍拋殼窗相對偏右側前方位置 (+8.0f 補白)
    Vector vecShellOrigin = vecSrc + (gpGlobals->v_forward * 8.0f);

    // 噴發黃銅大步槍彈殼 (m_iBrassShell)
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

    // 7. 在全服最高分貝廣播反恐精英 Scout 經典狙擊發射音效 (Volume 拉滿 1.0f)
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "weapons/scout_fire-1.wav", 1.0f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootSMG( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目全自動掃射
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x20c 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // 4. 幾何散佈運算：若當前微衝精確度未配置，套用標準 CS 微衝點射散佈基數
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (1 發，9mm/45ACP 衝鋒槍彈，2048 碼最大飛行穿透距離)
    FireBullets( 1, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 點亮槍口閃光特效，並實時扣除 1 發子彈
    pev->effects |= EF_MUZZLEFLASH; // 2
    m_cAmmoLoaded--;

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 產生槍身物理後座力震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    // =========================================================================
    // ?? 幾何衝量運算：計算 MAC10/TMP 右側滑套排殼窗在 3D 空間中的動態噴射力道與座標
    // =========================================================================
    Vector vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

    // 微型衝鋒槍拋殼窗相對偏?湊前方位置 (+10.0f 精密偏置)
    Vector vecShellOrigin = vecSrc + (gpGlobals->v_forward * 10.0f);

    // 噴發手槍/微衝黃銅小彈殼 (m_iBrassShell)
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

    // 7. 廣播標準反恐精英 MAC10 發射音效
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "weapons/mac10-1.wav", 0.75f, ATTN_NORM, 0, 100 );
}

// 在你的 server.cpp 中，只需要保留這一個最完美的散彈槍開火實作：
void CCounterTerrorist::ShootShotgun( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目開槍
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x83 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // =========================================================================
    // ?? 拋殼特效 1：發射前瞬間抽殼，排殼窗向後偏置 (-24.0f 補白)
    // =========================================================================
    Vector vecShellVelocity1 = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));
    Vector vecShellOrigin1 = vecSrc - (gpGlobals->v_forward * 24.0f);
    
    // 噴發第一枚紅色散彈槍塑料殼 (m_iShotgunShell，排殼種類代號為 2)
    EjectBrass( vecShellOrigin1, vecShellVelocity1, pev->angles.y, m_iShotgunShell, 2 );

    // 4. 幾何散佈運算：若當前武器精確度未配置，套用標準 CS 散彈槍大面積散射錐度
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.13053f, 0.13053f, 0.13053f ); // XM1014 標準散射錐
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (一次轟出 8 顆合金彈丸，最大穿透距離 2048 碼)
    FireBullets( 8, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 點亮槍口閃光特效，並實時扣除 1 發子彈
    pev->effects |= EF_MUZZLEFLASH; // 2
    m_cAmmoLoaded--;

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 產生散彈槍巨大的槍身物理後座力震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    // =========================================================================
    // ?? 拋殼特效 2：子彈出膛後二次氣浪噴殼，排殼窗向前偏置 (+8.0f 補白)
    // =========================================================================
    Vector vecShellVelocity2 = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));
    Vector vecShellOrigin2 = vecSrc + (gpGlobals->v_forward * 8.0f);

    // 噴發第二枚紅色散彈槍塑料殼 (m_iShotgunShell，排殼種類代號為 2)
    EjectBrass( vecShellOrigin2, vecShellVelocity2, pev->angles.y, m_iShotgunShell, 2 );

    // 7. 廣播標準反恐精英 XM1014 自動散彈槍開火音效
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "weapons/xm1014-1.wav", 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootPistol( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目開槍
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x20c 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // 4. 幾何散佈運算：若當前手槍精確度未配置，套用標準 CS 手槍點射散佈基數
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (1 發，9mm 手槍彈，2048 碼最大飛行穿透距離)
    FireBullets( 1, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 點亮槍口閃光特效，並實時扣除 1 發子彈
    m_cAmmoLoaded--;
    pev->effects |= EF_MUZZLEFLASH; // 2

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 產生槍身物理後座力震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    // =========================================================================
    // ?? 幾何衝量運算：計算 USP 手槍右側滑套排殼窗在 3D 空間中的動態噴射力道與座標
    // =========================================================================
    Vector vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

    // USP 手槍拋殼窗相對偏右手平舉前方位置 (+13.0f 精密偏置)
    Vector vecShellOrigin = vecSrc + (gpGlobals->v_forward * 13.0f);

    // 噴發手槍黃銅小彈殼 (m_iBrassShell)
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

    // 7. 廣播標準反恐精英 USP 未加消音器發射音效
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "weapons/usp_unsil-1.wav", 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootMP5( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目全自動掃射
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x20c 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // 4. 幾何散佈運算：若當前長槍精確度未配置，套用標準 CS 衝鋒槍高精準點射散佈
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (1 發，9mm 衝鋒槍彈，2048 碼最大飛行穿透距離)
    FireBullets( 1, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 點亮槍口閃光特效，並實時扣除 1 發子彈
    pev->effects |= EF_MUZZLEFLASH; // 2
    m_cAmmoLoaded--;

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 產生槍身物理後座力抬頭震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    // =========================================================================
    // ?? 幾何衝量運算：計算 H&K MP5 右側排殼窗在 3D 空間中的動態噴射力道與座標
    // =========================================================================
    Vector vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

    // MP5 拋殼窗相對偏右側前方位置 (+8.0f 補白)
    Vector vecShellOrigin = vecSrc + (gpGlobals->v_forward * 8.0f);

    // 噴發手槍/衝鋒槍黃銅小彈殼 (m_iBrassShell)
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

    // 7. 隨機分發 50% 經典 MP5 衝鋒槍射擊音效
    const char *pszShootSound = RANDOM_LONG(0, 1) ? "weapons/mp5-1.wav" : "weapons/mp5-2.wav";
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, pszShootSound, 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootMachineGun( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目全自動掃射
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x20c 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // 4. 幾何散佈運算：若當前武器精確度未配置，套用標準 CS 輕機槍狂暴掃射散佈基數
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (1 發，重型機槍彈，2048 碼最大飛行穿透距離)
    FireBullets( 1, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 點亮槍口動態全自動強光特效 (?? 100% 還原二進位行為：此兵種不扣除子彈，維持無限火力壓制)
    pev->effects |= EF_MUZZLEFLASH; // 2

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 槍身射擊震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    // =========================================================================
    // ?? 幾何衝量運算：計算 M60 輕機槍右側受彈窗在 3D 空間中的動態噴射力道與座標
    // =========================================================================
    Vector vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                               (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                               (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

    // M60 彈鏈窗相對偏右前方位置 (+8.0f 補白)
    Vector vecShellOrigin = vecSrc + (gpGlobals->v_forward * 8.0f);

    // 噴發黃銅步槍大彈殼 (m_iBrassShell)
    EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

    // 7. 隨機分發 50% 經典 M60 重機槍瘋狂咆哮音效
    const char *pszShootSound = RANDOM_LONG(0, 1) ? "weapons/m60-1.wav" : "weapons/m60-2.wav";
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, pszShootSound, 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::ShootLAW( void )
{
    // 1. ?? 外部安全審查：確保發射瞬間敵人依然存在，防止對著空氣發射火箭彈
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取發射起點 (呼叫 GetGunPosition 虛擬函數)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 鎖定敵人，獲取精確方向向量
    ShootAtEnemy( vecSrc, &vecDirShooting );

    // 將方向轉化為絕對朝向角度
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    vecAngles.x = -vecAngles.x; // 修正 Z 軸仰角幾何

    UTIL_MakeVectors( vecAngles );

    // =========================================================================
    // ?? 火箭彈 3D 空間隨機戰術散佈 (Spread) 幾何矩陣運算
    // =========================================================================
    float flRandX = RANDOM_FLOAT( -0.5f, 0.5f ) + RANDOM_FLOAT( -0.5f, 0.5f );
    float flRandY = RANDOM_FLOAT( -0.5f, 0.5f ) + RANDOM_FLOAT( -0.5f, 0.5f );

    // 結合兵種精度屬性進行向量縮放
    float flSpreadX = flRandX * m_weaponAccuracy.x;
    float flSpreadY = flRandY * m_weaponAccuracy.y;

    // 完美融合：計算出最終出膛時帶有微小偏差的火箭彈射出向量
    Vector vecFinalDir = vecDirShooting + (gpGlobals->v_right * flSpreadX) + (gpGlobals->v_up * flSpreadY);

    // 4. ?? 核心實體創建：在世界空間中動態生成火箭彈，自帶物理碰撞與飛行動態
    CLawsRocket::CreateLawsRocket( vecSrc, vecFinalDir, this );

    // =========================================================================
    // ?? 網路優化：廣播 Delta 客戶端事件，在所有玩家端即時渲染火箭筒開火與白煙特效
    // =========================================================================
    unsigned short usFireLaws = PRECACHE_EVENT( 1, "events/laws.sc" );
    PLAYBACK_EVENT_FULL( 0, edict(), usFireLaws, 0.0f, (float *)&g_vecZero, (float *)&g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );

    // 5. 注入動畫混合器，產生巨大的重武器發射後座力抬頭震動
    SetBlending( 0, vecAngles.x );

    // 6. 扣除火箭彈，彈匣清空 (即刻驅動後續的換彈匣狀態機)
    m_cAmmoLoaded--;
}

// 在你的 server.cpp 中，只需要保留這一個最完美的突擊步槍開火實作：
void CCounterTerrorist::ShootAssaultRifle( void )
{
    // 1. ?? 外部安全審查攔截：確保扣動扳機的瞬間敵人依然存在，防止對著空氣盲目開火
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    // 2. 獲取真實槍口發射起點 (呼叫虛擬函數 0x83 偏移量即 GetGunPosition)
    Vector vecSrc = GetGunPosition();
    Vector vecDirShooting;

    // 3. 呼叫小隊怪物戰術瞄準，鎖定敵人精確方向
    ShootAtEnemy( vecSrc, &vecDirShooting );

    UTIL_MakeVectors( pev->angles );

    // 4. 幾何散佈運算：若當前長槍精確度未配置，套用標準 CS 步槍點射散佈基數
    Vector vecSpread;
    if ( m_weaponAccuracy.Length() <= 0.0f )
    {
        vecSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecSpread = m_weaponAccuracy;
    }

    // 5. 調用引擎核心發射子彈 (1 發，5.56mm 步槍彈，2048 碼最大飛行穿透距離)
    FireBullets( 1, vecSrc, vecDirShooting, vecSpread, 2048.0f, BULLET_MONSTER_MP5, 0 );

    // 6. 實時扣除子彈，並點亮槍口動態閃光特效
    pev->effects |= EF_MUZZLEFLASH; // 2
    m_cAmmoLoaded--;

    // 將射擊方向解碼為混色動畫混合偏角 (SetBlending 槍身射擊震動)
    Vector vecAngles;
    UTIL_VecToAngles( vecDirShooting, vecAngles );
    SetBlending( 0, vecAngles.x );

    Vector vecShellVelocity;
    Vector vecShellOrigin;
    const char *pszShootSound = nullptr;

    // =========================================================================
    // ?? 核心分流矩陣：依據陣營/特警外觀類型 (m_ctType)，動態決定武器外觀與物理排殼
    // =========================================================================
    if ( m_ctType == 1 ) // 1 代表配置 AK47 的特殊或反叛變體兵種
    {
        // 計算 AK47 拋殼窗在 3D 空間中的動態噴射力道
        vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                           (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                           (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

        // AK47 排殼窗相對偏前上方位置 (+8.0f 補白)
        vecShellOrigin = vecSrc + (gpGlobals->v_forward * 8.0f);

        // 噴發黃銅步槍彈殼 (m_iBrassShell)
        EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

        // 隨機分發 50% 經典 AK47 射擊音效
        pszShootSound = RANDOM_LONG(0, 1) ? "weapons/ak47-1.wav" : "weapons/ak47-2.wav";
    }
    else // 預設 0 代表配置 M4A1 的常規 SWAT/特警兵種
    {
        vecShellVelocity = (gpGlobals->v_right * RANDOM_FLOAT(40.0f, 90.0f)) + 
                           (gpGlobals->v_up * RANDOM_FLOAT(75.0f, 200.0f)) + 
                           (gpGlobals->v_forward * RANDOM_FLOAT(-40.0f, 40.0f));

        // M4A1 排殼窗相對偏後方 (-24.0f 補白)
        vecShellOrigin = vecSrc - (gpGlobals->v_forward * 24.0f);

        EjectBrass( vecShellOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, 1 );

        // 隨機分發 50% 經典 M4A1 未加消音器開火音效
        pszShootSound = (RANDOM_LONG(0, 99) < 51) ? "weapons/m4a1_unsil-1.wav" : "weapons/m4a1_unsil-2.wav";
    }

    // 7. 最終在槍口廣播步槍發射音效
    EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, pszShootSound, 0.75f, ATTN_NORM, 0, 100 );
}

void CCounterTerrorist::SetYawSpeed( void )
{
    float flYawSpeed = 90.0f; // 預設兜底轉身角速度為 90.0 度/秒

    // =========================================================================
    // ?? 依據當前骨骼動畫活動 (m_Activity) 動態校準精確的物理轉向偏航角速度
    // =========================================================================
    switch ( m_Activity )
    {
        case ACT_IDLE:
            flYawSpeed = 90.0f; // 原地待命時轉身平滑
            break;

        case ACT_WALK:
            flYawSpeed = 120.0f; // 戰術慢速摸索
            break;

        case ACT_RUN:
            flYawSpeed = 180.0f; // 奔跑尋路
            break;

        case ACT_TURN_LEFT:
        case ACT_TURN_RIGHT:
            flYawSpeed = 240.0f; // 原地急轉彎
            break;

        case ACT_RANGE_ATTACK1:
        case ACT_RANGE_ATTACK2:
            flYawSpeed = 200.0f; // 激戰開火中動態修正視線
            break;

        // 完美對齊：我們之前在 GetScheduleOfType 破譯的 0x42 (66) 突擊衝鋒計畫
        case 66: 
            flYawSpeed = 360.0f; // 戰術衝鋒突臉！轉向速度拉滿，進行極速視線鎖定
            break;

        default:
            flYawSpeed = 90.0f; // 其餘狀態回歸預設基數
            break;
    }

    // 完美寫入引擎核心變數
    pev->yaw_speed = flYawSpeed;
}

void CCounterTerrorist::SetupWeapons( void )
{
    // 1. 初始化彈匣容量與當前加載彈藥的預設基數 (0x24 = 36 發)
    m_cClipSize   = 36;
    m_cAmmoLoaded = 36;

    unsigned int uWeaponFlags = pev->weapons;

    // 2. 核心死屍/空手硬熔斷：若身上被打上空手或死屍標記 (0x40 = 64)
    if ( uWeaponFlags & 0x40 ) // bits_WEAPON_NONE
    {
        SetBodygroup( 2, 8 ); // 強行將手上武器組部件切換為 8 (空手/無槍)
        m_cAmmoLoaded = 0;
        m_cClipSize   = 0;
        return; // 提前中斷，防止死屍進行 AI 攻擊尋路
    }

    // =========================================================================
    // ?? 3. 階段 A：處理地圖未指派兵種代號 (-1) 時的舊版相容性解碼
    // =========================================================================
    if ( m_ctBehavior == -1 )
    {
        if ( uWeaponFlags == 0 )
        {
            pev->weapons = 3;
            SetBodygroup( 2, 1 );
            return;
        }
        if ( uWeaponFlags & 1 )
        {
            SetBodygroup( 2, 0 );
            uWeaponFlags = pev->weapons;
        }
        if ( uWeaponFlags & 8 ) // 散彈槍旗標
        {
            SetBodygroup( 2, 1 );
            m_cClipSize = 8;
            uWeaponFlags = pev->weapons;
        }
        if ( uWeaponFlags & 0x20 ) // 火箭筒旗標
        {
            SetBodygroup( 2, 2 );
            uWeaponFlags = pev->weapons;
        }
        if ( uWeaponFlags & 0x10 ) // 狙擊槍旗標
        {
            SetBodygroup( 2, 3 );
        }
    }
    // =========================================================================
    // ?? 階段 B：依照我們破譯的兵種武器代號 (0~8) 進行專業戰術矩陣填裝
    // =========================================================================
    else
    {
        switch ( m_ctBehavior )
        {
            case 0: // 手槍特警
                m_cClipSize = 16;
                SetBodygroup( 2, 5 ); // 切換為手槍模型組
                break;

            case 1: // XM1014 散彈槍兵
                pev->weapons = 8;
                m_cClipSize  = 8;
                SetBodygroup( 2, 1 ); // 切換為散彈槍模型組
                break;

            case 2: // M4A1 / AK47 突擊步槍兵
                pev->weapons = 1;
                m_cClipSize  = 30;
                SetBodygroup( 2, 4 ); // 切換為主力步槍模型組
                break;

            case 3: // M60 輕機槍兵
                SetBodygroup( 2, 6 ); // 滿彈 36 發，切換為輕機槍模型組
                break;

            case 4: // Scout / AWP 戰術狙擊手
                pev->weapons = 16; // 0x10
                SetBodygroup( 2, 3 ); // 滿彈 36 發，切換為狙擊長槍模型組
                break;

            case 5: // LAW 物理火箭筒重裝兵
                m_cClipSize = 1;
                SetBodygroup( 2, 2 ); // 單發填裝，切換為火箭筒模型組
                break;

            case 6: // TMP / MAC10 微型衝鋒槍兵
                m_cClipSize  = 45;
                pev->weapons = 1;
                SetBodygroup( 2, 7 ); // 切換為微衝模型組
                break;

            case 7: // 下掛突擊榴彈兵
                m_cClipSize  = 16;
                pev->weapons = 2;
                SetBodygroup( 2, 5 ); // 切換為掛載手槍/榴彈模型組
                break;

            default:
                SetBodygroup( 2, 0 ); // 兜底空閒外觀
                break;
        }
    }

    // 4. 最終同步：一出生強行將彈匣子彈填滿
    m_cAmmoLoaded = m_cClipSize;
}

void CCounterTerrorist::SetupLaserGlow( void )
{
    // =========================================================================
    // ?? 1. 動態創建狙擊手紅外線雷射線 (CBeam)
    // =========================================================================
    if ( m_pBeam == nullptr )
    {
        // 創建雷射線，加載光束紋理，硬編碼線寬為 3
        m_pBeam = CBeam::BeamCreate( "sprites/laserbeam.spr", 3 );
        
        if ( m_pBeam != nullptr )
        {
            // 打上動態端點動態更新旗標 (0x8000)
            m_pBeam->pev->spawnflags |= 0x8000;
            
            // 配置高階半透明光束光學特效 (kRenderTransAdd = 3, kRenderFxNoDissipation = 14)
            m_pBeam->pev->rendermode = kRenderTransAdd; // 3
            m_pBeam->pev->renderfx   = 14;              // 0x0E
            
            // 調校為 100% 戰術純紅光
            m_pBeam->pev->rendercolor.x = 255.0f;
            m_pBeam->pev->rendercolor.y = 0.0f;
            m_pBeam->pev->rendercolor.z = 0.0f;
            
            // 出生時初始亮度保持完全隱形
            m_pBeam->pev->renderamt = 0.0f;
            m_laserBrightness       = 0.0f;
        }
    }

    // =========================================================================
    // ?? 2. 動態創建槍口真實附著紅點精靈 (CSprite)
    // =========================================================================
    if ( m_pLaserGlow == nullptr )
    {
        // 在自身位置實例化高爆耀斑紅點精靈
        m_pLaserGlow = CSprite::SpriteCreate( "sprites/flare3.spr", pev->origin, FALSE );
        
        if ( m_pLaserGlow != nullptr )
        {
            // 配置與雷射對齊的光學特效
            m_pLaserGlow->pev->rendermode = kRenderTransAdd; // 3
            m_pLaserGlow->pev->renderfx   = 14;              // 0x0E
            m_pLaserGlow->pev->rendercolor.x = 255.0f;
            m_pLaserGlow->pev->rendercolor.y = 0.0f;
            m_pLaserGlow->pev->rendercolor.z = 0.0f;
            
            // ?? 核心黑科技：套用 MOVETYPE_FOLLOW 機制，將紅點精靈死死綁定在右手狙擊槍槍口 (Attachment 2)
            edict_t *pMyEdict = edict();
            if ( pMyEdict != nullptr )
            {
                m_pLaserGlow->pev->aiment   = pMyEdict;
                m_pLaserGlow->pev->movetype = MOVETYPE_FOLLOW; // 12 (0x0C)
                m_pLaserGlow->pev->skin     = ENTINDEX( pMyEdict );
                m_pLaserGlow->pev->body     = 2; // 精確指定依附在模型 2 號附著點 (槍口位置)
            }
            
            // 初始亮度保持隱形
            m_pLaserGlow->pev->renderamt = 0.0f;
            m_glowBrightness             = 0.0f;
        }
    }
}

void CCounterTerrorist::SetupBehaviors( int type )
{
    switch ( type )
    {
        case 0: // 手槍兵
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 0.9f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 1.0f; scheduleTable[0x32].percentChance = 0.9f;
            scheduleTable[0x42].percentChance = 0.1f; scheduleTable[0x18].percentChance = 1.0f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.2f;
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 1.0f; scheduleTable[0x1c].percentChance = 0.1f;
            scheduleTable[0x46].percentChance = 0.4f;
            m_fPreferedRange = 192.0f;
            break;

        case 1: // 散彈槍兵 (XM1014)
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 0.9f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 1.0f; scheduleTable[0x32].percentChance = 0.0f;
            scheduleTable[0x42].percentChance = 0.4f; scheduleTable[0x18].percentChance = 0.9f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.3f;
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 0.2f; scheduleTable[0x1c].percentChance = 0.1f;
            scheduleTable[0x46].percentChance = 0.4f;
            m_fPreferedRange = 128.0f; // 突臉黃金距離
            break;

        case 3: // M60 輕機槍兵
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 0.9f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 0.0f; scheduleTable[0x32].percentChance = 0.5f;
            scheduleTable[0x42].percentChance = 0.0f; scheduleTable[0x18].percentChance = 0.95f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.0f;
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 0.1f; scheduleTable[0x1c].percentChance = 0.1f;
            scheduleTable[0x46].percentChance = 0.1f;
            m_fPreferedRange = 256.0f;
            break;

        case 4: // 狙擊步槍手 (AWP / Scout)
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 1.0f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 0.0f; scheduleTable[0x32].percentChance = 0.5f;
            scheduleTable[0x42].percentChance = 0.0f; scheduleTable[0x18].percentChance = 0.0f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.2f;
            scheduleTable[0x3e].percentChance = 1.0f; scheduleTable[0x3f].percentChance = 0.5f; // 架槍雷射計畫開啟
            scheduleTable[0x40].percentChance = 0.0f; scheduleTable[0x1c].percentChance = 0.0f;
            scheduleTable[0x46].percentChance = 0.0f;
            m_fPreferedRange = 256.0f;
            break;

        case 5: // LAW 火箭筒重裝兵
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 0.9f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 1.0f; scheduleTable[0x32].percentChance = 0.9f;
            scheduleTable[0x42].percentChance = 0.0f; scheduleTable[0x18].percentChance = 1.0f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.0f;
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 0.0f; scheduleTable[0x1c].percentChance = 0.1f;
            scheduleTable[0x46].percentChance = 0.0f;
            m_fPreferedRange = 256.0f;
            break;

        case 6: // TMP / MAC10 微衝兵
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 0.8f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 1.0f; scheduleTable[0x32].percentChance = 0.8f;
            scheduleTable[0x42].percentChance = 0.1f; scheduleTable[0x18].percentChance = 0.9f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.1f;
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 0.3f; scheduleTable[0x1c].percentChance = 0.1f;
            scheduleTable[0x46].percentChance = 0.1f;
            m_fPreferedRange = 192.0f;
            break;

        case 7: // 槍掛突擊榴彈兵
            scheduleTable[0x12].percentChance = 0.1f; scheduleTable[0x11].percentChance = 0.9f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 1.0f; scheduleTable[0x32].percentChance = 0.0f;
            scheduleTable[0x42].percentChance = 0.0f; scheduleTable[0x18].percentChance = 0.1f;
            scheduleTable[0x19].percentChance = 1.0f; scheduleTable[0x16].percentChance = 0.0f; // 100% 瘋狂傾瀉榴彈
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 0.0f; scheduleTable[0x1c].percentChance = 0.2f;
            scheduleTable[0x46].percentChance = 0.5f;
            m_fPreferedRange = 256.0f;
            break;

        case 2: // 突擊步槍兵 (M4A1 / AK47)
        case 8: // MP5 衝鋒槍兵
        default: // 完美對齊反編譯中的嵌套兜底分支
            scheduleTable[0x12].percentChance = 0.0f; scheduleTable[0x11].percentChance = 0.8f;
            scheduleTable[0x43].percentChance = 0.0f; scheduleTable[0x44].percentChance = 0.0f;
            scheduleTable[0x33].percentChance = 1.0f; scheduleTable[0x32].percentChance = 0.8f;
            scheduleTable[0x42].percentChance = 0.1f; scheduleTable[0x18].percentChance = 0.9f;
            scheduleTable[0x19].percentChance = 0.0f; scheduleTable[0x16].percentChance = 0.3f;
            scheduleTable[0x3e].percentChance = 0.0f; scheduleTable[0x3f].percentChance = 0.0f;
            scheduleTable[0x40].percentChance = 0.4f; scheduleTable[0x1c].percentChance = 0.1f;
            scheduleTable[0x46].percentChance = 0.6f;
            m_fPreferedRange = 256.0f;
            break;
    }
}

int CCounterTerrorist::SetActivitySniperBase( Activity NewActivity )
{
    // 獲取 3D 模型指針（用於二進位底層對齊）
    void *pModel = GET_MODEL_PTR( edict() );

    const char *pszSeqName = nullptr;

    switch ( NewActivity )
    {
        // =========================================================================
        // ?? 1. 狙擊雙姿態開火重定向
        // =========================================================================
        case ACT_RANGE_ATTACK1:
            if ( !m_bStanding )
            {
                pszSeqName = "sniper_crouch-fire";
            }
            else
            {
                pszSeqName = "sniper-fire";
            }
            return LookupSequence( pszSeqName );

        // =========================================================================
        // ?? 2. 開啟紅外線雷射與架槍動畫
        // =========================================================================
        case ACT_ARM:
            LaserGlowOn(); // 呼叫先前還原的特效開啟中樞
            return LookupSequence( "laser_on_stand" );

        // =========================================================================
        // ?? 3. 關閉紅外線雷射與收槍動畫
        // =========================================================================
        case ACT_DISARM:
            LaserGlowOff(); // 呼叫先前還原的特效清理中樞 (等價於內聯的 UTIL_Remove 常規清理)
            return LookupSequence( "laser_off_stand" );

        // =========================================================================
        // ?? 4. 全套死亡事件：強行熔斷、一鍵清理所有殘留雷射特效，防止懸空雷射 Bug
        // =========================================================================
        case ACT_DIESIMPLE:
        case ACT_DIEBACKWARD:
        case ACT_DIEFORWARD:
        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
        case ACT_DIE_BACKSHOT:
            LaserGlowOff(); // 死亡時強制掐斷雷射光束
            return -1;      // 返回 -1 降級，交由主 SetActivity 方法去處理後續死亡模型骨骼序列

        default:
            return -1; // 其餘活動交還給通用狀態機
    }

    return -1;
}

int CCounterTerrorist::SetActivityShotgunBase( Activity NewActivity )
{
    // 獲取 3D 模型指針（用於二進位底層對齊）
    void *pModel = GET_MODEL_PTR( edict() );

    const char *pszSeqName = nullptr;

    // =========================================================================
    // ?? 散彈槍武器子系統：開火與雙姿態換彈動畫精確重定向
    // =========================================================================
    if ( NewActivity == ACT_RANGE_ATTACK1 ) // 開火事件
    {
        if ( !m_bStanding )
        {
            pszSeqName = "crouching_shotgun"; // 蹲姿散彈槍射擊
        }
        else
        {
            pszSeqName = "standing_shotgun";  // 站姿散彈槍射擊
        }
    }
    else if ( NewActivity == ACT_RELOAD ) // 換彈匣事件
    {
        if ( !m_bStanding )
        {
            pszSeqName = "crouch_reload_shotgun"; // 蹲姿持散彈槍填彈
        }
        else
        {
            // 完美還原 "crouch_reload_shotgun" + ACT_HOP (即 5) -> "reload_shotgun"
            pszSeqName = "reload_shotgun";        // 站姿持散彈槍填彈
        }
    }
    else
    {
        // 其餘活動返回 -1，降級交還給主 SetActivity 方法處理
        return -1; 
    }

    // 將選定的字串序列名稱轉化為底層 MDL 骨骼序列號
    if ( pszSeqName != nullptr )
    {
        return LookupSequence( pszSeqName );
    }

    return -1;
}

int CCounterTerrorist::SetActivityPistolBase( Activity NewActivity )
{
    // 獲取 3D 模型指針（用於二進位底層對齊）
    void *pModel = GET_MODEL_PTR( edict() );
    const char *pszSeqName = nullptr;

    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim != 0 )
            {
                return LookupSequence( STRING(m_idleAnim) );
            }
            if ( m_MonsterState == MONSTERSTATE_COMBAT && m_bStanding )
            {
                pszSeqName = "pistol_combatidle";
            }
            else
            {
                pszSeqName = "pistol_idle1";
            }
            break;

        case ACT_CROUCH:
            pszSeqName = "pistol_crouching_wait";
            break;

        case ACT_WALK:
            if ( pev->health <= pev->max_health * 0.2f || m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pszSeqName = "pistol_walk"; // 輕傷/非戰鬥手槍行走
            }
            else
            {
                pszSeqName = "combat_walk_pistol"; // 戰術舉槍橫移步
            }
            break;

        case ACT_RUN:
            if ( pev->health <= pev->max_health * 0.2f || m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pszSeqName = "pistol_run";
            }
            else
            {
                pszSeqName = "combat_run_pistol"; // 戰術持槍小跑
            }
            break;

        case ACT_STRAFE_LEFT:  pszSeqName = "pistol_strafefire_L"; break;
        case ACT_STRAFE_RIGHT: pszSeqName = "pistol_strafefire_R"; break;

        case ACT_TURN_LEFT:
            pszSeqName = ( m_MonsterState == MONSTERSTATE_COMBAT ) ? "pistol_180L" : "pistol_idle1";
            break;
        case ACT_TURN_RIGHT:
            pszSeqName = ( m_MonsterState == MONSTERSTATE_COMBAT ) ? "pistol_180R" : "pistol_idle1";
            break;

        case ACT_COWER:        pszSeqName = "pistol_cower"; break;
        case ACT_SMALL_FLINCH: pszSeqName = "pistol_smflinch"; break;

        case ACT_RANGE_ATTACK1: // 核心槍械射擊路由
            if ( m_ctBehavior == 6 ) // TMP / MAC10 微衝兵
            {
                pszSeqName = !m_bStanding ? "crouching_tmp" : "shootcycle_tmp";
            }
            else // 標準手槍兵
            {
                pszSeqName = !m_bStanding ? "pistol_crouching" : "pistol_standing";
            }
            break;

        case ACT_MELEE_ATTACK1:
            pszSeqName = "pistol_melee"; // 手槍近戰搏擊
            break;

        case ACT_RELOAD:
            // 完美還原 "crouch_pistol_reload" + ACT_HOP (5) -> "pistol_reload"
            pszSeqName = "pistol_reload"; 
            break;

        // 全套死亡受擊反應矩陣
        case ACT_DIESIMPLE:
            if ( m_deathAnim != 0 ) return LookupSequence( STRING(m_deathAnim) );
            pszSeqName = "pistol_die-simple";
            break;
        case ACT_DIEBACKWARD:
            if ( m_deathAnim != 0 ) return LookupSequence( STRING(m_deathAnim) );
            pszSeqName = "pistol_dieback1";
            break;
        case ACT_DIEFORWARD:
        case ACT_DIE_BACKSHOT:
            if ( m_deathAnim != 0 ) return LookupSequence( STRING(m_deathAnim) );
            pszSeqName = "pistol_dieforward";
            break;
        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
            if ( m_deathAnim != 0 ) return LookupSequence( STRING(m_deathAnim) );
            pszSeqName = "pistol_headshot";
            break;
        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
            if ( m_deathAnim != 0 ) return LookupSequence( STRING(m_deathAnim) );
            pszSeqName = "pistol_diegutshot";
            break;

        // 精確到肢體的受擊硬直
        case ACT_FLINCH_LEFTARM:  pszSeqName = "pistol_leftarmflinch"; break;
        case ACT_FLINCH_RIGHTARM: pszSeqName = "pistol_rightarmflinch"; break;
        case ACT_FLINCH_LEFTLEG:  pszSeqName = "pistol_leftlegsmflinch"; break;
        case ACT_FLINCH_RIGHTLEG: pszSeqName = "pistol_rightlegsmflinch"; break;

        default:
            return -1; // 降級派發
    }

    if ( pszSeqName != nullptr )
    {
        return LookupSequence( pszSeqName );
    }

    return -1;
}

int CCounterTerrorist::SetActivityLAWBase( Activity NewActivity )
{
    // 獲取 3D 模型指針（用於二進位底層對齊）
    void *pModel = GET_MODEL_PTR( edict() );

    const char *pszSeqName = nullptr;

    // =========================================================================
    // ?? 火箭筒 (LAW) 武器子系統動畫精確重定向矩陣
    // =========================================================================
    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim != 0 )
            {
                pszSeqName = STRING( m_idleAnim );
            }
            else
            {
                pszSeqName = "LAW_fire_idle";
            }
            break;

        case ACT_WALK:
        case ACT_RUN:
            pszSeqName = "law_run";
            break;

        case ACT_TURN_LEFT:
            pszSeqName = "law_180L";
            break;

        case ACT_TURN_RIGHT:
            pszSeqName = "law_180R";
            break;

        case ACT_RANGE_ATTACK1:
            pszSeqName = "LAW_fire";
            break;

        case ACT_RELOAD:
            pszSeqName = "law_reload";
            break;

        default:
            // 其餘未註冊活動不在此武器子系統處理，返回 -1 讓主方法降級向下派發
            return -1;
    }

    // 將選定的字串序列名稱轉化為底層 MDL 骨骼序列號
    if ( pszSeqName != nullptr )
    {
        return LookupSequence( pszSeqName );
    }

    return -1;
}

int CCounterTerrorist::SetActivityGrenadeBase( Activity NewActivity )
{
    // 獲取 3D 模型指針（用於二進位底層對齊）
    void *pModel = GET_MODEL_PTR( edict() );

    const char *pszSeqName = nullptr;

    // =========================================================================
    // ?? 投擲物/榴彈子系統：手榴彈與槍掛榴彈動畫精確重定向
    // =========================================================================
    if ( NewActivity == ACT_RANGE_ATTACK2 )
    {
        // 位元遮罩核對：如果身上沒有配置常規手榴彈 (bits_WEAPON_GRENADE = 2)
        if ( (pev->weapons & bits_WEAPON_GRENADE) == 0 )
        {
            pszSeqName = "launchgrenade"; // 執行突擊槍掛榴彈發射動畫
        }
        else
        {
            pszSeqName = "throwgrenade";  // 執行常規高爆雷拋體投擲動畫
        }

        // 將選定的字串序列名稱轉化為底層 MDL 骨骼序列號
        if ( pszSeqName != nullptr )
        {
            return LookupSequence( pszSeqName );
        }
    }

    // 其餘活動返回 -1 降級向下派發
    return -1;
}

int CCounterTerrorist::SetActivityAutomaticBase( Activity NewActivity )
{
    // 獲取 3D 模型指針（用於二進位底層對齊）
    void *pModel = GET_MODEL_PTR( edict() );

    const char *pszSeqName = nullptr;

    // =========================================================================
    // ?? 步槍/機槍/MP5 子系統：開火與換彈動畫精確重定向
    // =========================================================================
    if ( NewActivity == ACT_RANGE_ATTACK1 ) // 開火事件
    {
        if ( m_ctBehavior == 3 ) // M60 輕機槍兵
        {
            pszSeqName = "machinegun_fire";
        }
        else if ( !m_bStanding ) // 蹲姿突擊步槍 / MP5 兵
        {
            pszSeqName = "crouching_mp5";
        }
        else // 站姿突擊步槍 / MP5 兵
        {
            pszSeqName = "standing_mp5";
        }
    }
    else if ( NewActivity == ACT_RELOAD ) // 換彈匣事件
    {
        // 完美還原 "crouch_reload_mp5" + ACT_HOP (即 5) -> "reload_mp5"
        pszSeqName = "reload_mp5"; 
    }
    else
    {
        // 其餘活動不在此子系統處理，返回 -1 讓主方法降級向下派發
        return -1; 
    }

    // 將選定的字串序列名稱轉化為底層 MDL 骨骼序列號
    if ( pszSeqName != nullptr )
    {
        return LookupSequence( pszSeqName );
    }

    return -1;
}

void CCounterTerrorist::SetActivity( Activity NewActivity )
{
    int iSequence = -1;
    void *pModel = GET_MODEL_PTR( edict() );

    // =========================================================================
    // ?? 階段 1：根據武器兵種代號 (m_ctBehavior)，路由分發至專屬武器動畫基底
    // =========================================================================
    switch ( m_ctBehavior )
    {
        case 0:
        case 6: // 手槍 / 微衝
            iSequence = SetActivityPistolBase( NewActivity );
            break;
        case 1: // 散彈槍
            iSequence = SetActivityShotgunBase( NewActivity );
            break;
        case 2:
        case 3:
        case 8: // 突擊步槍 / 輕機槍 / MP5
            iSequence = SetActivityAutomaticBase( NewActivity );
            break;
        case 4: // 狙擊槍
            iSequence = SetActivitySniperBase( NewActivity );
            break;
        case 5: // LAW 火箭筒
            iSequence = SetActivityLAWBase( NewActivity );
            break;
        case 7: // 突擊榴彈兵
            iSequence = SetActivityPistolBase( NewActivity );
            if ( iSequence == -1 )
            {
                iSequence = SetActivityGrenadeBase( NewActivity );
            }
            break;
        default:
            break;
    }

    // =========================================================================
    // ?? 階段 2：若自定義武器基底未攔截，進入通用自定義戰術活動矩陣
    // =========================================================================
    if ( iSequence == -1 )
    {
        switch ( NewActivity )
        {
            case ACT_IDLE:
                if ( m_idleAnim != 0 )
                {
                    iSequence = LookupSequence( STRING(m_idleAnim) );
                }
                else if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    // 戰鬥待命：自動依據蹲/站狀態切換基準動畫
                    iSequence = LookupSequence( !m_bStanding ? "crouching_wait" : "combatidle" );
                }
                break;

            case ACT_WALK:
                // 殘血受傷機制：健康度低於 20% 時，強行轉為瘸腿負傷行走 (0x30 = ACT_WALK_HURT)
                if ( pev->health <= pev->max_health * 0.2f )
                {
                    iSequence = LookupActivity( 0x30 );
                }
                else if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    iSequence = LookupSequence( "combat_walk_primary" ); // 戰術平舉步槍前進
                }
                break;

            case ACT_RUN:
                // 殘血受傷機制：健康度低於 20% 時，強行轉為負傷奔跑 (0x31 = ACT_RUN_HURT)
                if ( pev->health <= pev->max_health * 0.2f )
                {
                    iSequence = LookupActivity( 0x31 );
                }
                else if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    iSequence = LookupSequence( "combat_run_primary" ); // 戰術衝鋒跑步
                }
                break;

            case ACT_STAND:
                iSequence = LookupSequence( "crouching_2_stand" ); // 起立過渡
                break;

            // 各類常規死亡判定
            case ACT_DIESIMPLE:
            case ACT_DIEBACKWARD:
            case ACT_DIEFORWARD:
            case ACT_DIEVIOLENT:
            case ACT_DIE_HEADSHOT:
            case ACT_DIE_CHESTSHOT:
            case ACT_DIE_GUTSHOT:
            case ACT_DIE_BACKSHOT:
                if ( m_deathAnim != 0 )
                {
                    iSequence = LookupSequence( STRING(m_deathAnim) );
                }
                break;

            // 特殊戰術受擊硬直事件 (對應原版 ACT_LASTACTIVITY | 遮罩)
            case (ACT_LASTACTIVITY | ACT_IDLE):
                iSequence = LookupSequence( "pistol_popout_right01" );
                break;
            case (ACT_LASTACTIVITY | ACT_GUARD):
                iSequence = LookupSequence( "pistol_popout_right02" );
                break;
            case (ACT_LASTACTIVITY | ACT_WALK):
                iSequence = LookupSequence( "choking" );   // 窒息/被勒脖子硬直
                break;
            case (ACT_LASTACTIVITY | ACT_RUN):
                iSequence = LookupSequence( "stunned01" ); // 電擊/眩暈硬直
                break;

            default:
                break;
        }
    }

    // =========================================================================
    // ?? 階段 3：若上述矩陣仍未找到動畫，強制向下派發給基底類別方法去提取
    // =========================================================================
    if ( iSequence == -1 )
    {
        iSequence = CSquadMonster::LookupActivity( NewActivity );
    }

    m_Activity = NewActivity;

    // 4. 動態防錯機制：若 MDL 模型中徹底缺少該動作，噴出控制台錯誤日誌並強制歸零
    if ( iSequence < 0 )
    {
        ALERT( at_console, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity );
        pev->sequence = 0;
        return;
    }

    // 5. 刷新動畫幀計時器
    if ( iSequence != pev->sequence || m_fSequenceLoops )
    {
        pev->frame = 0.0f;
    }

    pev->sequence = iSequence;
    ResetSequenceInfo(); // 重置動畫速率、長度和權重數據頭
    SetYawSpeed(); 
}

Schedule_t* CCounterTerrorist::ScheduleFromName( const char *pName )
{
    // 1. 優先在 SWAT 隊員專屬的 41 個自定義戰術計畫清單中進行字串匹配查找
    // sizeof(m_scheduleList) / sizeof(m_scheduleList[0]) 在編譯時會 100% 精確展開為 41 (0x29)
    Schedule_t *pSchedule = CSquadMonster::ScheduleInList( pName, m_scheduleList, sizeof(m_scheduleList) / sizeof(m_scheduleList[0]) );
    
    if ( pSchedule != nullptr )
    {
        return pSchedule; // 找到特警自定義計畫，立刻返回
    }

    // 2. 完美還原：若找不到，則回歸原版引擎基礎怪物庫中去兜底搜尋標準計畫名稱
    return CSquadMonster::ScheduleFromName( pName );
}

void CCounterTerrorist::RunTask( Task_t *pTask )
{
    switch ( pTask->iTask )
    {
        // =========================================================================
        // 任務 96 (0x60)：戰術步步逼近 / 動態追擊敵人 (Close In Task)
        // =========================================================================
        case 96:
        {
            if ( m_cantMove )
            {
                return;
            }

            if ( m_hEnemy == nullptr )
            {
                m_afConditions |= 0x40000000; // bits_COND_TASK_FAILED
                return;
            }

            // 計算自身與尋路目標點之間的平面 2D 距離
            float flDistToGoal = (pev->origin - m_vecMoveGoal).Length2D();

            if ( flDistToGoal >= m_fPreferedRange )
            {
                // 計算目標點與敵人精確位置的三維空間距離
                float flGoalToEnemy = (m_vecMoveGoal - m_hEnemy->pev->origin).Length();
                
                // 如果敵人跑出了黃金交戰距離的一半，刷新路由，進行智慧動態追蹤
                if ( flGoalToEnemy > (m_fPreferedRange * 0.5f) )
                {
                    m_vecMoveGoal = m_hEnemy->pev->origin;
                    FRefreshRoute(); // 重新整理動態尋路路徑
                    flDistToGoal = (pev->origin - m_vecMoveGoal).Length2D();
                }
            }

            // 順利攻入黃金交戰距離以內，宣告推進任務圓滿完成
            if ( flDistToGoal < m_fPreferedRange )
            {
                if ( (m_afConditions & 0x40000000) == 0 )
                {
                    m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
                }
                RouteClear(); // 原地停下腳步，準備就地射擊
            }

            // 推進期間，強行將移動動畫鎖定為奔跑姿態 ACT_RUN
            if ( m_movementActivity != ACT_RUN )
            {
                m_movementActivity = ACT_RUN;
            }
            break;
        }

        // =========================================================================
        // 任務 92 (0x5C)：扔雷 / 重武器發射方向前置動態對齊 (Face Toss Dir)
        // =========================================================================
        case 92:
        {
            // 利用拋射速度外推計算出未來的彈道世界座標點
            Vector vecTarget = pev->origin + (m_vecTossVelocity * 64.0f);
            
            // 計算理想面朝角度並呼叫 ChangeYaw 急速轉身
            MakeIdealYaw( vecTarget );
            ChangeYaw( (int)pev->yaw_speed );

            // 核心安全鎖：只有當身體100%完全轉向面對了目標點，才允許完成任務進入下一幀扔雷
            if ( !FacingIdeal() )
            {
                return; 
            }

            m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
            break;
        }

        // =========================================================================
        // 任務 103 (0x67)：語音播放期間的眼神實時頭部骨骼追蹤 (Wait Speak Task)
        // =========================================================================
        case 103:
        {
            if ( gpGlobals->time < m_flStopTalkTime )
            {
                if ( m_hTalkTarget != nullptr )
                {
                    // 只要具備轉頭能力，在每一幀持續調用 IdleHeadTurn 鎖定聽眾座標
                    if ( m_afCapability & bits_CAP_CAP_TURN_HEAD )
                    {
                        IdleHeadTurn( &(m_hTalkTarget->pev->origin) );
                        return; // 繼續維持說話與注視狀態
                    }
                }
            }

            // 說話完畢，且任務未打上失敗旗標，宣告語音等待任務結束
            if ( (m_afConditions & 0x40000000) == 0 )
            {
                m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
            }
            break;
        }

        default:
            // 其餘常規基本任務，直接派發給基底類別小隊怪物狀態機去每幀維護
            CSquadMonster::RunTask( pTask );
            break;
    }
}

int CCounterTerrorist::Save( CSave &save )
{
    // 1. 優先呼叫基底類別小隊怪物的 Save 方法，保存標準 AI 與尋路變數
    int status = CSquadMonster::Save( save );
    
    if ( status == 0 )
    {
        return 0; // 基底類別保存失敗，直接熔斷退出
    }

    // 2. 完美還原：將 28 個 CCounterTerrorist 特有的戰術自定義變數寫入存檔檔案
    // sizeof(m_SaveData) / sizeof(m_SaveData[0]) 在編譯時會 100% 精確展開為 28 (0x1c)
    return save.WriteFields( "CCounterTerrorist", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData[0]) );
}

int CCounterTerrorist::Restore( CRestore &restore )
{
    // 1. 優先呼叫基底類別小隊怪物的 Restore，恢復標準的 AI 尋路和狀態變數
    int status = CSquadMonster::Restore( restore );
    
    if ( status == 0 )
    {
        return 0; // 基底類別讀取失敗，直接熔斷退出
    }

    // 2. 完美還原：從存檔中讀取 28 個 CCounterTerrorist 特有的戰術自定義變數
    // sizeof(m_SaveData) / sizeof(m_SaveData[0]) 在編譯時會精確展開為 28 (0x1c)
    return restore.ReadFields( "CCounterTerrorist", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData[0]) );
}

void CCounterTerrorist::ResetHeadWatch( void )
{
    // 1. 將 0 號頸部骨骼控制器強行重置為 0.0 度 (頭部迅速恢復面朝正前方)
    SetBoneController( 0, 0.0f );

    // 2. 安全註銷對話與注視目標，防止野指針記憶體崩潰
    m_hTalkTarget = nullptr;
}

void CCounterTerrorist::PrescheduleThink( void )
{
    // ?? 說話截止超時檢查：當語音播放完畢，一鍵調用函數讓頭部骨骼與注視焦點重置歸位
    if ( m_flStopTalkTime != 0.0f && m_flStopTalkTime < gpGlobals->time )
    {
        m_flStopTalkTime = 0.0f;
        ResetHeadWatch(); // ?? 直接這樣寫，代替原本那 2 行重置，結構極其漂亮！
    }

    // 2. ?? 小隊聯動：動態維護全隊共用的「敵人最後目擊時間戳」與「丟失警報」
    if ( m_hSquadLeader != nullptr && m_hEnemy != nullptr )
    {
        // 獲取小隊長指針 (若小隊長無效則以自己作為臨時基準)
        CCounterTerrorist *pSquadLeader = (CCounterTerrorist *)m_hSquadLeader.Get();
        if ( pSquadLeader == nullptr )
        {
            pSquadLeader = this;
        }

        // 判定 1：小隊中有成員親眼看到了敵人 (bits_COND_SEE_ENEMY = 0x10)
        if ( HasConditions( bits_COND_SEE_ENEMY ) )
        {
            // 動態重新整理全隊共用的最後目擊時間
            pSquadLeader->m_flLastEnemySightTime = gpGlobals->time;
            return;
        }

        // 判定 2：小隊全體成員已經連續 5 秒鐘沒看見敵人 (敵人利用地形盲區逃跑)
        if ( (gpGlobals->time - pSquadLeader->m_flLastEnemySightTime) > 5.0f )
        {
            // 在小隊長身上拉響戰術警報，觸發後續的全隊協同追擊計畫
            pSquadLeader->m_fEnemyEluded = TRUE;
            return;
        }
    }
}

void CCounterTerrorist::PlayScriptedSentence( char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener )
{
    // 1. 活體安全審查：只有在活著且未處於死亡判定狀態下，才允許播放劇情腳本語音
    if ( !IsAlive() || pev->deadflag == DEAD_DYING ) // 1
    {
        // 排除死屍說話 Bug
        return; 
    }

    // 2. 鎖定全局語音通道，並加上 2.0 秒的台詞自然停頓安全緩衝補白
    if ( duration + 2.0f == -1.0f )
    {
        float flRandWait = RANDOM_FLOAT( 1.5f, 2.0f );
        CTalkMonster::g_talkWaitTime = gpGlobals->time + flRandWait;
    }
    else
    {
        CTalkMonster::g_talkWaitTime = gpGlobals->time + duration + 2.0f;
    }

    // 重設台詞狀態
    m_iSentence = -1;

    // 3. 呼叫虛擬函數執行底層劇情 Sentence 播放 (0x188 偏移量通常代表原版基底類別的 PlayScriptedSentence)
    // 註：這在 Xash3D/HLSDK 中通常是呼叫 CTalkMonster::PlayScriptedSentence
    CSquadMonster::PlayScriptedSentence( pszSentence, duration, volume, attenuation, bConcurrent, pListener );

    // 4. ?? 劇情最高控制權：強行鎖定 1 號玩家的語音/無線電發射通道，防止玩家台詞打架
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }

    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        // 完美對齊 FOkToSpeak 中的 0xabc 玩家 Private Data 偏移量
        float *pflPlayerSpeechLock = (float *)((char *)pPlayerEdict->pvPrivateData + 0xabc);
        *pflPlayerSpeechLock = gpGlobals->time + duration; // 玩家在特警說話期間禁止使用無線電
    }

    // 5. ?? 眼神交流：將頭部與對話焦點鎖定在聽眾 (pListener) 身上
    m_hTalkTarget = pListener;
    if ( m_hTalkTarget != nullptr )
    {
        // 如果此時具備轉頭能力 (bits_CAP_TURN_HEAD = 0x400)，立刻調用 IdleHeadTurn 進行骨骼偏角扭轉
        if ( m_afCapability & bits_CAP_CAP_TURN_HEAD )
        {
            // 將頭骨面向聽眾世界座標點
            IdleHeadTurn( &(m_hTalkTarget->pev->origin) ); 
        }
    }

    // 6. 設定強行說話截止時間戳
    m_pfnUse = 0x0;
    Talk( duration );
}

void CCounterTerrorist::PainSound( void )
{
    // 1. 受傷叫聲冷卻檢查：防止短時間內被連續中彈時發出鬼畜重複慘叫
    if ( gpGlobals->time > m_flNextPainTime )
    {
        // 2. 呼叫虛擬函數播放 /sound/sentences.txt 中預設的 CT 受傷痛呼語音組
        PlaySentence( "CT_WOUND", 0.8f, 0.8f, 0, m_voicePitch );

        // 3. 鎖定受傷語音通道 1 秒鐘，確保音效戰場降噪
        m_flNextPainTime = gpGlobals->time + 1.0f;
    }
}

int CCounterTerrorist::ObjectCaps( void )
{
    // 完美還原 10 (2 | 8) 能力旗標：允許且強制該隊員隨玩家跨越 Changelevel 關卡切換
    return FCAP_ACROSS_TRANSITION | FCAP_FORCE_TRANSITION;
}

int CCounterTerrorist::LookupActivityHeaviest( int activity )
{
    // 1. 獲取當前實體加載的 3D 模型指針 (MDL 數據頭)
    void *pModel = GET_MODEL_PTR( edict() );

    // 2. 核心攔截：只有當引擎請求 ACT_IDLE (1) 時，才介入權重最高動畫重定向
    if ( activity != ACT_IDLE ) // 1
    {
        // 其餘動作直接派發給全域最高權重活動查找器
        return ::LookupActivity( pModel, pev, activity );
    }

    const char *pszSeqName = nullptr;

    // =========================================================================
    // ?? 根據兵種武器行為代號 (m_ctBehavior) 動態定向最高權重基準動畫
    // =========================================================================
    if ( m_ctBehavior == 5 ) // LAW 火箭筒兵
    {
        pszSeqName = "LAW_fire_idle";
    }
    else if ( m_ctBehavior == 0 || m_ctBehavior == 7 ) // 手槍兵 / 盾牌兵 / 榴彈兵
    {
        // 完美對齊反編譯細節：這裡不進行 COMBAT 判定，直接無條件鎖定最穩定的基準手槍空閒動作
        pszSeqName = "pistol_idle1"; 
    }
    else // 2=突擊步槍, 3=輕機槍, 4=狙擊手, 6=微衝, 8=MP5 兵
    {
        // 若不處於戰鬥狀態，呼叫全域重載方法獲取常規最高權重 ACT_IDLE
        if ( m_MonsterState != MONSTERSTATE_COMBAT )
        {
            return ::LookupActivityHeaviest( pModel, pev, ACT_IDLE );
        }

        // 處於激戰中，精確切換高權重的交火持槍警戒動畫
        if ( !m_bStanding )
        {
            pszSeqName = "crouching_wait"; // 完美還原 "pistol_crouching_wait" + 7
        }
        else
        {
            pszSeqName = "combatidle";     // 完美還原 "machete_combatidle" + 8
        }
    }

    // 3. 將選定的字串序列名稱，轉化為底層整數序列號
    if ( pszSeqName != nullptr )
    {
        return LookupSequence( pszSeqName );
    }

    return CSquadMonster::LookupActivityHeaviest( activity );
}

int CCounterTerrorist::LookupActivity( int activity )
{
    // 1. 獲取當前實體加載的 3D 模型指針 (MDL 數據頭)
    void *pModel = GET_MODEL_PTR( edict() );

    // 2. 核心攔截：只有當引擎請求 ACT_IDLE (1) 空閒待命動畫時，才介入自定義 CS 武器動畫重定向
    if ( activity != ACT_IDLE ) // 1
    {
        // 其餘常規動作 (如受傷、跑步) 直接派發給全域活動查找器
        return ::LookupActivity( pModel, pev, activity );
    }

    const char *pszSeqName = nullptr;

    // =========================================================================
    // ?? 根據當前配置的武器兵種代號 (m_ctBehavior) 動態定向骨骼動畫
    // =========================================================================
    if ( m_ctBehavior == 5 ) // LAW 火箭筒兵
    {
        pszSeqName = "LAW_fire_idle";
    }
    else if ( m_ctBehavior == 0 || m_ctBehavior == 7 ) // 手槍兵 / 盾牌兵 / 榴彈兵
    {
        if ( m_MonsterState == MONSTERSTATE_COMBAT )
        {
            pszSeqName = "pistol_combatidle";
        }
        else
        {
            pszSeqName = "pistol_idle1";
        }
    }
    else // 2=突擊步槍, 3=輕機槍, 4=狙擊手, 6=微衝, 8=MP5 兵
    {
        // 若不處於戰鬥狀態，回歸標準的 1 號空閒動畫
        if ( m_MonsterState != MONSTERSTATE_COMBAT )
        {
            return ::LookupActivity( pModel, pev, ACT_IDLE );
        }

        // 處於激戰中，依據蹲姿站姿精確切換交火待命動畫
        if ( !m_bStanding )
        {
            pszSeqName = "crouching_wait"; // 完美還原 "pistol_crouching_wait" + 7
        }
        else
        {
            pszSeqName = "combatidle";     // 完美還原 "machete_combatidle" + 8
        }
    }

    // 3. 將選定的 CS 特有字串序列名稱，轉化為引擎渲染所需的底層整數序列號
    if ( pszSeqName != nullptr )
    {
        return LookupSequence( pszSeqName );
    }

    return CSquadMonster::LookupActivity( activity );
}

void CCounterTerrorist::LimitFollowers( CBaseEntity *pPlayer, int maxFollowers )
{
    // 完美對齊：此處在 Linux server.so 二進位中被作者寫死為 Stub 空函數，不執行任何人數限制
    return;
}

void CCounterTerrorist::LaserGlowOn( void )
{
    // =========================================================================
    // ?? 1. 狙擊雷射線 (CBeam) 實體動態對齊與渲染
    // =========================================================================
    if ( m_pBeam != nullptr )
    {
        // 必須在目前有敵人的情況下，才開啟雷射瞄準
        if ( m_hEnemy != nullptr )
        {
            // 獲取自身與敵人的引擎內部 Edict 實體索引
            int startIndex = ENTINDEX( edict() );
            int endIndex   = ENTINDEX( m_hEnemy->edict() );

            // 將雷射線的起點和終點精確綁定在自身與敵人的骨骼中心，由引擎自動即時追蹤
            m_pBeam->EntsInit( startIndex, endIndex );

            // 設定雷射線的骨骼與特殊渲染序列旗標 (0x2000 對應特定的半透明光束遮罩)
            m_pBeam->pev->sequence = (m_pBeam->pev->sequence & 0xFFFU) | 0x2000;

            // 鎖定雷射線的亮度值為 64.0f (適中的戰術紅外線微光)
            if ( m_laserBrightness != 64.0f )
            {
                m_laserBrightness = 64.0f;
            }
            m_pBeam->pev->renderamt = 64.0f;
        }
        else
        {
            return; // 無敵人則提前攔截
        }
    }

    // =========================================================================
    // ?? 2. 槍口/目標點紅點 (CSprite) 特效亮度控制
    // =========================================================================
    if ( m_pLaserGlow == nullptr )
    {
        return;
    }

    // 鎖定目標點紅點的亮度為最大值 255.0f (高亮警告)
    if ( m_glowBrightness != 255.0f )
    {
        m_glowBrightness = 255.0f;
    }
    m_pLaserGlow->pev->renderamt = 255.0f;
}

void CCounterTerrorist::LaserGlowOff( void )
{
    // 1. 檢查並安全移除狙擊手的雷射紅外線瞄準線實體
    if ( m_pBeam != nullptr )
    {
        UTIL_Remove( m_pBeam );
        m_pBeam = nullptr; // 強制歸零，防止野指針記憶體崩潰
    }

    // 2. 檢查並安全移除槍口/目標點上的紅點 Sprite 精靈特效
    if ( m_pLaserGlow != nullptr )
    {
        UTIL_Remove( m_pLaserGlow );
        m_pLaserGlow = nullptr;
    }
}

CBaseEntity* CCounterTerrorist::Kick( void )
{
    // 1. 刷新當前面朝方向向量矩陣
    UTIL_MakeVectors( pev->angles );

    // 2. 計算飛踢探測的起點：自身原點 + 自身高度的 75% 處 (約胸口高度)
    Vector vecStart = pev->origin;
    vecStart.z += pev->size.z * 0.75f;

    Vector vecEnd;

    // 3. 幾何終點運算：向前方或敵人方向精確推進 128 單元距離
    if ( m_hEnemy == nullptr )
    {
        // 若無敵人，終點直接設定在面朝正前方 128 單元處
        vecEnd = vecStart + (gpGlobals->v_forward * 128.0f);
    }
    else
    {
        // 若有敵人，計算指向敵人的戰術方向向量 (目標鎖定在敵人高度的 25% 處)
        Vector vecTarget = m_hEnemy->pev->origin;
        vecTarget.z -= m_hEnemy->pev->size.z * 0.25f; // 調校目標點高度

        Vector vecDir = vecTarget - vecStart;
        float flLen = vecDir.Length();

        if ( flLen == 0.0f )
        {
            // 異常死鎖兜底：直接向上推進 128 單位
            vecEnd = vecStart;
            vecEnd.z += 128.0f;
        }
        else
        {
            // 向量歸一化並強制鎖定 128 碼的近戰極限長度
            vecEnd = vecStart + (vecDir * (1.0f / flLen) * 128.0f);
        }
    }

    TraceResult tr;

    // 4. 調用引擎核心凸殼掃描：使用 3 號精細正方體盒 (head_hull) 進行空間軌跡撞擊
    UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, head_hull, edict(), &tr );

    // 5. 核對撞擊結果：如果撞到了合法的遊戲實體，返回該實體指針
    if ( tr.pHit != nullptr )
    {
        return CBaseEntity::Instance( tr.pHit );
    }

    return nullptr;
}

void CCounterTerrorist::KeyValue( KeyValueData *pkvd )
{
    // 1. 檢查地圖屬性鍵名是否為 "behavior" (不區分大小寫比對)
    if ( strcasecmp( pkvd->szKeyName, "behavior" ) == 0 )
    {
        // 將地圖傳入的字串值轉化為十進位整數，賦值給兵種行為代號
        m_behaviorType = atoi( pkvd->szValue );
        
        // 標記該屬性已被此實體成功攔截處理
        pkvd->fHandled = TRUE;
        return;
    }
    // 2. 檢查地圖屬性鍵名是否為 "wander"
    else if ( strcasecmp( pkvd->szKeyName, "wander" ) == 0 )
    {
        // 讀取布林值：若地圖傳入非 0 數值則開啟自主巡邏遊蕩，否則關閉
        m_canWander = ( atoi( pkvd->szValue ) != 0 );
        
        pkvd->fHandled = TRUE;
        return;
    }

    // 3. 其餘通用屬性（如 health, targetname 等）交由基底類別方法自動解析
    CSquadMonster::KeyValue( pkvd );
}

void CCounterTerrorist::JustSpoke( float nextSpeakTime )
{
    // 1. 如果傳入特定的 -1.0f 信號，自動動態計算 1.5 ~ 2.0 秒的戰術隨機冷卻
    if ( nextSpeakTime == -1.0f )
    {
        float flRandomWait = RANDOM_FLOAT( 1.5f, 2.0f );
        CTalkMonster::g_talkWaitTime = gpGlobals->time + flRandomWait;
    }
    else
    {
        // 2. 常規冷卻：直接套用指定的冷卻時間戳，鎖定全局語音通道
        CTalkMonster::g_talkWaitTime = gpGlobals->time + nextSpeakTime;
    }

    // 3. 釋放目前說話狀態，清空台詞池緩衝區
    m_iSentence = -1;
}

BOOL CCounterTerrorist::IsTalking( void )
{
    // 完美還原：若當前遊戲時間小於說話截止時間，代表 NPC 正忙於語音通訊
    if ( gpGlobals->time < m_flStopTalkTime )
    {
        return TRUE;
    }
    
    return FALSE;
}

BOOL CCounterTerrorist::IsPlayerHidden( void )
{
    // 1. 戰鬥思維熔斷：如果目前處於 MONSTERSTATE_COMBAT (3) 或更高戰備狀態，玩家無法在特警面前隱蔽
    if ( m_MonsterState == MONSTERSTATE_COMBAT || m_MonsterState > MONSTERSTATE_COMBAT )
    {
        return FALSE;
    }

    // 2. 獲取單人劇情遊戲中的 1 號玩家實體
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }

    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        return FALSE;
    }

    // 3. 核心解密：讀取玩家 Private Data 偏移 0x8f8 位置的動態潛行觸發器指針
    // (char*) 用於進行精確到每個位元組的記憶體指針偏移運算
    CTriggerStealth **ppStealthTrigger = (CTriggerStealth **)((char *)pPlayerEdict->pvPrivateData + 0x8f8);

    // 如果玩家此時沒有站在任何地圖陰影/潛行觸發器區域內
    if ( *ppStealthTrigger == nullptr )
    {
        return FALSE;
    }

    // 4. 調用潛行系統核心，評估玩家在該區域的隱蔽系數是否達標
    if ( CTriggerStealth::IsPlayerHidden( *ppStealthTrigger ) )
    {
        return TRUE; // 玩家隱蔽成功！特警將無視玩家
    }

    // 5. 破功懲罰：如果潛行失敗（例如在陰影中開槍或走動），強行將玩家身上的潛行指針清空歸零
    *ppStealthTrigger = nullptr;
    
    return FALSE;
}

int CCounterTerrorist::ISoundMask( void )
{
    // 完美還原 0x23 聽覺遮罩：只聽取 槍聲、環境物理碰撞聲 以及 手榴彈爆炸危險聲
    return bits_SOUND_COMBAT | bits_SOUND_WORLD | bits_SOUND_DANGER; 
}

void CCounterTerrorist::InitScheduleTable( void )
{
    // 完美還原：遍歷所有 74 個戰術計畫通道
    for ( int i = 0; i < 74; i++ )
    {
        scheduleTable[i].activeFlag    = 0.0f; // 一出生全部預設未啟用
        scheduleTable[i].percentChance = 1.0f; // 將所有計畫的基礎隨機中籤權重重設為 1.0 基數
    }
}

void CCounterTerrorist::IdleSound( void )
{
    // 1. 機率過濾：大於 75 則保持沉默 (約 24% 機率不說話)
    if ( RANDOM_LONG( 0, 99 ) > 75 )
    {
        return;
    }

    // 2. 語音鎖審查：若環境不允許開口，保持安靜
    if ( !FOkToSpeak() )
    {
        return;
    }

    if ( g_fCTQuestion == 0 )
    {
        // 50% 機率不主動發起話題
        if ( RANDOM_LONG( 0, 1 ) == 0 )
        {
            return;
        }

        // 發起新一輪閒置對話話題
        int iRandTopic = RANDOM_LONG( 0, 2 );
        if ( iRandTopic == 0 )
        {
            PlaySentence( "CT_CHECK", 0.8f, 0.8f, 0, m_voicePitch );
            g_fCTQuestion = 1; // 標記目前有人發起了【環境檢查】提問
        }
        else if ( iRandTopic == 1 )
        {
            PlaySentence( "CT_QUEST", 0.8f, 0.8f, 0, m_voicePitch );
            g_fCTQuestion = 2; // 標記目前有人發起了【疑惑/尋求指示】提問
        }
        else if ( iRandTopic == 2 )
        {
            PlaySentence( "CT_IDLE", 0.8f, 0.8f, 0, m_voicePitch ); // 單純碎碎念，不改變話題狀態
        }
    }
    else
    {
        // 應答分支：目前場面上有人拋出了話題，AI 自動進行回話對接
        if ( g_fCTQuestion == 1 )
        {
            PlaySentence( "CT_CLEAR", 0.8f, 0.8f, 0, m_voicePitch ); // 迴應：一切安全/收到
        }
        else if ( g_fCTQuestion == 2 )
        {
            PlaySentence( "CT_ANSWER", 0.8f, 0.8f, 0, m_voicePitch ); // 迴應：解答或聽從指令
        }

        g_fCTQuestion = 0; // 對話大閉環完成，重置全域狀態為空閒
    }

    // 3. 設定對話結束後的 5 秒語音硬性冷卻
    m_iSentence = -1;
    CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
}

void CCounterTerrorist::IdleHeadTurn( Vector *vecFriend )
{
    // 1. 外部安全審查（對應你貼的第二段代碼）
    if ( (m_afCapability & bits_CAP_TURN_HEAD) == 0 )
    {
        return;
    }

    if ( vecFriend == nullptr )
    {
        return;
    }

    // 2. 內部幾何數學運算（對應你貼的第一段代碼，已被編譯器自動內聯）
    Vector vecDir = *vecFriend - pev->origin;
    float flTargetYaw = UTIL_VecToYaw( vecDir );
    float flDeltaYaw = flTargetYaw - pev->angles.y;

    // 角度歸一化
    if ( flDeltaYaw > 180.0f )  flDeltaYaw -= 360.0f;
    if ( flDeltaYaw < -180.0f ) flDeltaYaw += 360.0f;

    // 注入骨骼控制器
    SetBoneController( 0, flDeltaYaw );
}

void CCounterTerrorist::HandleSpeaking( int schedule )
{
    // 1. 全局語音與衝突檢查：如果不允許說話，直接返回
    if ( !FOkToSpeak() )
    {
        return;
    }

    // 2. 根據當前勝出的計畫 ID，動態廣播對應的戰術無線電/台詞
    switch ( schedule )
    {
        case 0x12: // 18: 尋求掩護/跑位
            if ( RANDOM_LONG( 0, 1 ) )
            {
                PlaySentence( "CT_COVER", 0.8f, 0.8f, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
                m_iSentence = -1;
            }
            break;

        case 0x13: // 19: 閃避致命爆炸物 (聽到手榴彈滾動)
            PlaySentence( "CT_GREN", 0.8f, 0.8f, 0, m_voicePitch );
            m_iSentence = -1;
            // 隨機設定 1.5 到 2.0 秒的語音冷卻
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            break;

        case 0x18: // 24: 步槍/衝鋒槍常規點射與掃射開火
            if ( RANDOM_LONG( 0, 99 ) >= 51 ) // 反編譯對應: iVar6 < 0x33 (51) 則跳出
            {
                PlaySentence( "CT_ATTACK", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            }
            break;

        case 0x19: // 25: 投擲手榴彈/發射重武器 (火箭筒/榴彈)
            PlaySentence( "CT_THROW", 0.8f, 0.8f, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            break;

        case 0x1c: // 28: 戰術挑釁與對峙
            if ( RANDOM_LONG( 0, 99 ) > 50 ) // 0x32 = 50
            {
                PlaySentence( "CT_TAUNT", 0.8f, 0.8f, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 7.0f;
                m_iSentence = -1;
            }
            break;

        case 0x32: // 50: 建立交火線/戰術跑位
            if ( RANDOM_LONG( 0, 1 ) )
            {
                PlaySentence( "CT_MOVING", 0.8f, 0.8f, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
                m_iSentence = -1;
            }
            break;

        case 0x42: // 66: 突擊衝鋒
            if ( RANDOM_LONG( 0, 1 ) )
            {
                PlaySentence( "CT_CHARGE", 0.8f, 0.8f, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 5.0f;
                m_iSentence = -1;
            }
            break;

        default:
            break;
    }

    // 3. ?? 隊友遭遇叛變/瘋狂反擊玩家彩蛋判定
    if ( m_hEnemy != nullptr )
    {
        // 檢查當前死敵是否為玩家 (IsPlayer() = 虛函數 0x2a 偏移)
        if ( m_hEnemy->IsPlayer() )
        {
            // 有 24% 的低機率觸發大罵台詞 (0x4b = 75，大於 75 即 76~99)
            if ( RANDOM_LONG( 0, 99 ) > 75 )
            {
                PlaySentence( "CT_MAD", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 6.0f;
            }
        }
    }
}

Schedule_t* CCounterTerrorist::HandleIdleHeadWatch( void )
{
    // 1. 眼神冷卻計時器檢查 ＆ 視野內是否正好看見玩家 (bits_COND_SEE_CLIENT = 0x200000)
    if ( gpGlobals->time >= m_flStareTime && HasConditions( bits_COND_SEE_CLIENT ) )
    {
        // 2. 隨機機率過濾：僅有 20% 的低機率會觸發轉頭注視玩家
        if ( RANDOM_LONG( 0, 99 ) > 80 )
        {
            // 3. 獲取單人遊戲中的 1 號玩家實體
            edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
            if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
            {
                pPlayerEdict = PEOFFSET( 0 );
            }

            if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
            {
                CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );
                
                // 4. 計算與玩家之間的 2D 幾何距離
                float flDistToPlayer = (pev->origin - pPlayer->pev->origin).Length2D();
                
                // 5. 距離限制：只有當玩家走近至 256 單元以內時，才會觸發注視
                if ( flDistToPlayer < 256.0f )
                {
                    m_hTalkTarget = pPlayer; // 將頭部/對話焦點鎖定在玩家身上
                    
                    // 設定眼神注視的持續硬直時間 (4 到 6 秒隨機)
                    m_flStopTalkTime = gpGlobals->time + RANDOM_FLOAT( 4.0f, 6.0f );
                    
                    // ?? 設定高達 12 秒的眼神注視冷卻，防止 AI 連續死盯玩家
                    m_flStareTime = gpGlobals->time + 12.0f;
                    
                    // 返回空閒眼神對視計畫，鎖定骨骼動畫朝向
                    return &slCTIdleEyecontact;
                }
            }
        }
    }

    // 未觸發注視，返回 nullptr 讓 GetScheduleOfType 繼續分發常規空閒行為
    return nullptr;
}

Schedule_t* CCounterTerrorist::HandleFollowing( void )
{
    // 1. 安全攔截：只有在目前「沒有敵人」的情況下，才評估隨行尋路
    if ( m_hEnemy == nullptr )
    {
        // 2. 檢查目前是否有跟隨目標，且該目標是玩家
        if ( m_hTargetEnt != nullptr && m_hTargetEnt->IsPlayer() )
        {
            // 3. 活體檢查：如果目標玩家還活著
            if ( m_hTargetEnt->IsAlive() )
            {
                // 返回計畫 15 (0x0F)，精確對應 slCTFaceTarget 計畫 (保持隨行/面朝玩家)
                return GetScheduleOfType( 15 ); 
            }
            
            // 4. 悲劇分支：玩家陣亡！內聯熔斷展開，強行解除跟隨狀態
            if ( m_movementGoal == 1 ) // MOVEGOAL_TARGETENT = 1 (尋路目標為實體)
            {
                RouteClear(); // 清空當前尋路路徑，原地停下腳步
            }

            m_hTargetEnt = nullptr; // 徹底註銷跟隨目標

            // 如果附近有潛在敵情，大腦理想狀態立刻切換回戰鬥思維
            if ( m_hEnemy != nullptr )
            {
                m_IdealMonsterState = MONSTERSTATE_COMBAT;
            }
        }
    }

    // 無任何跟隨事件，返回 nullptr 讓 GetSchedule 繼續往下走遊蕩巡邏
    return nullptr;
}

void CCounterTerrorist::HandleAnimEvent( MonsterEvent_t *pEvent )
{
    switch ( pEvent->event )
    {
        case 2: // 換彈夾完成事件 (Reload Complete)
        {
            if ( m_ctBehavior < 9 )
            {
                const char *pszReloadSound = "weapons/reload_mp5.wav";
                
                // 動態判斷兵種播放配套換彈音效
                if ( m_ctBehavior == 1 ) // 散彈槍
                    pszReloadSound = "weapons/reload_shotgun.wav";
                else if ( m_ctBehavior == 0 || m_ctBehavior == 7 ) // 手槍/榴彈兵
                    pszReloadSound = "weapons/pistol_reload.wav";

                EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, pszReloadSound, 0.75f, ATTN_NORM, 0, 100 );
            }
            
            // 補滿彈匣並重置 AI 狀態
            m_cAmmoLoaded = m_cClipSize;
            m_afConditions &= ~bits_COND_NO_AMMO_LOADED;
            break;
        }

        case 3: // 近戰戰術飛踢事件 (Melee Kick)
        {
            CBaseEntity *pHurt = Kick(); // 呼叫自定義近戰檢測
            if ( pHurt != nullptr )
            {
                EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "weapons/knife_slash1.wav", 1.0f, ATTN_NORM, 0, 100 );
                
                UTIL_MakeVectors( pev->angles );
                
                // 戰術物理掀飛：推動目標向後上方飛出
                pHurt->pev->punchangle.x = 5.0f;
                pHurt->pev->velocity = pHurt->pev->velocity + (gpGlobals->v_forward * 100.0f) + (gpGlobals->v_up * 50.0f);
                
                // 套用數值庫飛踢傷害 (0x80 代表打擊/鈍器傷害類型 bits_DMG_CLUB)
                pHurt->TakeDamage( pev, pev, gSkillData.hgruntDmgKick, bits_DMG_CLUB );
            }
            break;
        }

        case 4: // 常規開火事件
        case 5: // 點射開火事件
        case 6: // 掃射開火事件
        {
            justShotFlag = TRUE;
            
            // 根據行為兵種代號，分發開火函數
            switch ( m_ctBehavior )
            {
                case 0:
                case 7: ShootPistol();       break;
                case 1: ShootShotgun();      break;
                case 3: ShootMachineGun();    break;
                case 4: ShootSniperRifle();   break;
                case 5: ShootLAW();           break;
                case 6: ShootSMG();           break;
                case 8: ShootMP5();           break;
                default: ShootAssaultRifle(); break; // 2 代表突擊步槍
            }
            
            // 在空間中大範圍廣播槍聲實體，吸引敵方 AI
            CSoundEnt::InsertSound( bits_SOUND_COMBAT, pev->origin, 384, 0.3f );
            break;
        }

        case 7: // 投擲定時手手榴彈事件 (HE Grenade Throw)
        {
            UTIL_MakeVectors( pev->angles );
            
            Vector vecGunPos, vecGunAngles;
            GetAttachment( 0, vecGunPos, vecGunAngles ); // 獲取右手槍口/手掌附著點
            
            // 實例化定時手榴彈，速度繼承自彈道估算變數 m_vecTossVelocity
            CGrenade::ShootTimed( pev, vecGunPos, m_vecTossVelocity, 3.5f ); // 3.5秒引信
            
            m_fThrowGrenade = FALSE;
            m_flNextGrenadeCheck = gpGlobals->time + 6.0f; // 進入6秒扔雷冷卻
            break;
        }

        case 8: // 發射觸碰即炸突擊榴彈事件 (AR Grenade Launcher)
        {
            EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.8f, ATTN_NORM, 0, 100 );
            
            Vector vecGunPos, vecGunAngles;
            GetAttachment( 0, vecGunPos, vecGunAngles );
            
            // 實例化觸碰即炸的榴彈
            CGrenade::ShootContact( pev, vecGunPos, m_vecTossVelocity );
            
            m_fThrowGrenade = FALSE;
            
            // 困難難度下 AI 瘋狂連發榴彈 (2~5秒隨機冷卻)，其餘難度固定 6 秒
            if ( g_iSkillLevel == 3 )
                m_flNextGrenadeCheck = gpGlobals->time + RANDOM_FLOAT( 2.0f, 5.0f );
            else
                m_flNextGrenadeCheck = gpGlobals->time + 6.0f;
            break;
        }

        case 9: // 特殊索降平拋雷事件 (Repel Grenade Drop)
        {
            UTIL_MakeVectors( pev->angles );
            
            // 幾何矩陣運算：計算索降時扔雷的空間安全起點，防止炸到繩索
            Vector vecOrigin = pev->origin + (gpGlobals->v_forward * 17.0f) - (gpGlobals->v_right * 27.0f) + (gpGlobals->v_up * 6.0f);
            Vector vecVel = (gpGlobals->v_forward * 35.0f);
            
            CGrenade::ShootTimed( pev, vecOrigin, vecVel, 3.0f );
            break;
        }

        case 10: // 動態口令事件：高喊發現敵人
        {
            if ( FOkToSpeak() )
            {
                PlaySentence( "CT_ENEMY", 0.8f, 0.8f, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
            }
            break;
        }

        case 11: // 武器脫手掉落事件 (Weapon Drop on Death)
        {
            Vector vecGunPos, vecGunAngles;
            GetAttachment( 0, vecGunPos, vecGunAngles );
            
            // 隱藏死亡怪物模型手上的主武器 (2號部件組切換為8空手狀態)
            SetBodygroup( 2, 8 );
            
            // 依據地圖設定與概率，在右手位置爆出實體武器拾取箱供玩家撿取
            if ( m_dropItem != 0 && RANDOM_FLOAT(0.0f, 1.0f) < m_dropChance )
            {
                CBaseMonster::DropItem( STRING(m_dropItem), vecGunPos, vecGunAngles );
            }
            break;
        }

        case 14: // 狙擊手雷射和紅點點強行清理事件
        {
            if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
            if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
            break;
        }

        case 13: // 空白預留事件
            break;

        default:
            // 其餘未攔截事件，交還給基底類別處理
            CSquadMonster::HandleAnimEvent( pEvent );
            break;
    }
}

void CCounterTerrorist::GibMonster( void )
{
    // 1. 讀取模型部件組 2，檢查 SWAT 目前手上是否有拿著武器 (2 通常代表空手/無槍)
    int iWeaponBodyGroup = GetBodygroup( 2 );
    
    if ( iWeaponBodyGroup != 2 )
    {
        Vector vecGunPos, vecGunAngles;
        
        // 2. 獲取右手骨骼附著點 0 的精確位置與角度，作為遺物噴發點
        GetAttachment( 0, vecGunPos, vecGunAngles );

        // 3. 兵種特殊遺物判定：如果是榴彈步槍兵 (bits_WEAPON_GRENADER = 4)
        if ( pev->weapons & bits_WEAPON_GRENADER )
        {
            // 在右手位置掉落一個 AR 突擊榴彈彈藥箱
            CBaseEntity *pDroppedItem = CBaseMonster::DropItem( "ammo_ARgrenades", vecGunPos, vecGunAngles );
            
            if ( pDroppedItem != nullptr )
            {
                // 4. 注入爆炸氣浪物理衝量，讓彈藥箱被隨機掀飛
                pDroppedItem->pev->velocity.x = RANDOM_FLOAT( -100.0f, 100.0f );
                pDroppedItem->pev->velocity.y = RANDOM_FLOAT( -100.0f, 100.0f );
                pDroppedItem->pev->velocity.z = RANDOM_FLOAT(  200.0f, 300.0f ); // 強力向上噴射

                // 5. 注入旋轉角速度，讓彈藥箱在空中沿著 Y 軸打轉
                pDroppedItem->pev->avelocity.x = 0.0f;
                pDroppedItem->pev->avelocity.z = 0.0f;
                pDroppedItem->pev->avelocity.y = RANDOM_FLOAT( 200.0f, 400.0f );
            }
        }
    }

    // 6. 最終呼叫基底類別方法，執行標準的碎屍特效與肉塊生成
    CBaseMonster::GibMonster();
}

int CCounterTerrorist::GetWeaponEffectSchedule( void )
{
    // 1. 獲取當前受到的戰術道具效果類型
    int iEffectType = m_iWeaponEffectType;

    // =========================================================================
    // 分支 A：煙霧彈（Smoke Grenade）危害評估 (iVar2 == 7)
    // =========================================================================
    if ( iEffectType == 7 )
    {
        if ( !m_bWeaponEffectActive )
        {
            return -1;
        }

        // 清除狀態鎖，防止每幀重複觸發語音
        m_iWeaponEffectType   = 0;
        m_bWeaponEffectActive = FALSE;

        // 播放處於煙霧中的咳嗽或無線電回報台詞
        if ( FOkToSpeak() )
        {
            PlaySentence( "CT_INSMOKE", 0.8f, 0.8f, 0, m_voicePitch );
            CTalkMonster::g_talkWaitTime = gpGlobals->time + 6.0f;
            m_iSentence = -1;
        }

        // 隨機進行煙霧彈戰術抉擇
        int iRand = RANDOM_LONG( 0, 99 );
        if ( iRand < 60 )
        {
            return 60; // 0x3C 精確對應 SCHED_IN_SMOKE (原地迷茫/咳嗽硬直)
        }
        if ( iRand < 90 )
        {
            return 17; // 0x11 精確對應 SCHED_INVESTIGATE_SOUND (警惕撤離煙霧區)
        }

        return -1; // 10% 的鐵血悍將機率：無視煙霧彈
    }

    // =========================================================================
    // 分支 B：閃光彈（Flashbang）致盲評估 (iVar2 == 4)
    // =========================================================================
    if ( iEffectType == 4 && m_bWeaponEffectActive )
    {
        // 立刻重置狀態鎖
        m_iWeaponEffectType   = 0;
        m_bWeaponEffectActive = FALSE;

        // 播放被閃光彈致盲的慘叫台詞
        if ( FOkToSpeak() )
        {
            PlaySentence( "CT_FLASHED", 0.8f, 0.8f, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 1.5f, 2.0f );
        }

        // 閃光彈致盲具備絕對強制性，無條件返回 61
        return 61; // 0x3D 精確對應 SCHED_FLASHED (捂眼盲目硬直)
    }

    // 無任何道具異常狀態
    return -1;
}

int CCounterTerrorist::GetScheduleFromTable( void )
{
    float flTotalChanceSum = 0.0f;

    // 1. 第一步：計算所有當前被激活計畫的權重總和 (sum)
    for ( int i = 0; i < 74; i++ )
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            flTotalChanceSum += scheduleTable[i].percentChance;
        }
    }

    // 2. 第二步：將機率進行動態歸一化分配 (Normalization)
    for ( int i = 0; i < 74; i++ )
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            // 完美還原：當前計畫機率 / 總機率
            scheduleTable[i].normalizedChance = scheduleTable[i].percentChance / flTotalChanceSum;
        }
    }

    // 3. 第三步：投擲 0.0 到 1.0 之間的隨機數，執行輪盤抽籤 (Random Roll)
    float flRoll = RANDOM_FLOAT( 0.0f, 1.0f );
    float flAccumulatedChance = 0.0f;

    for ( int i = 0; i < 73; i++ ) // 反編譯中限制最大為 0x49 (73)
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            flAccumulatedChance += scheduleTable[i].normalizedChance;
            
            // 一旦累加機率蓋過了隨機數，代表該戰術中籤，立刻返回計畫 ID
            if ( flRoll < flAccumulatedChance )
            {
                return i; 
            }
        }
    }

    // 4. 兜底防錯區域：如果徹底死鎖沒有中籤任何計畫
    if ( m_MonsterState == MONSTERSTATE_IDLE )
    {
        return -1;
    }

    // 列印極其高階的關卡地圖除錯日誌 (AIconsole Alert)
    if ( pev->targetname == 0 )
    {
        ALERT( at_aiconsole, "%s -- (no targetname) at (%d, %d, %d), Failed to find valid schedule in the table!\n",
               STRING(pev->classname), (int)pev->origin.x, (int)pev->origin.y, (int)pev->origin.z );
    }
    else
    {
        ALERT( at_aiconsole, "%s -- %s at (%d, %d, %d), Failed to find valid schedule in the table!\n",
               STRING(pev->classname), STRING(pev->targetname), (int)pev->origin.x, (int)pev->origin.y, (int)pev->origin.z );
    }

    return -1;
}

Schedule_t* CCounterTerrorist::GetScheduleOfType( int Type )
{
    // 1. 特殊計畫攔截 (0x16 任務失敗計數清零)
    if ( Type == 0x16 )
    {
        taskFailCount = 0.0f;
        return CSquadMonster::GetScheduleOfType( Type );
    }

    // 2. 核心計畫工廠分支矩陣
    switch ( Type )
    {
        case 1: // 空閒/常規等待
            if ( gpGlobals->time < m_flStopTalkTime ) return &slCTIdleEyecontact;
            {
                Schedule_t *pIdleSched = HandleIdleHeadWatch();
                if ( pIdleSched != nullptr ) return pIdleSched;
            }
            return CSquadMonster::GetScheduleOfType( 1 );

        case 10: return &slCTCombatFace; // 戰鬥面朝?人

        case 0x0E: // 14: 勝利歡呼動畫
            if ( m_hSquadLeader == nullptr || m_hSquadLeader.Get() == this ) return &slCTVictoryDance;
            return &slCTFail;

        case 0x0F: return &slCTFaceTarget; // 15: 面朝目標
        case 0x10: return &slCTFollow;     // 16: 跟隨計畫

        case 0x12: // 18: 掩護計畫
        case 0x43: // 67
        case 0x44: // 68
            return &slCTTakeCover;

        case 0x13: return &slCTTakeCoverFromBestSound; // 19: 規避爆炸物

        case 0x18: // 24: 核心槍械射擊計畫 (動態姿態調度控制)
            taskFailCount = 0.0f;
            if ( m_ctBehavior == 0 ) return &slCTRangeAttack1C; // 普通兵
            else 
            {
                if ( RANDOM_LONG( 0, 99 ) < 50 )
                {
                    if ( m_ctBehavior == 3 ) { m_bStanding = TRUE; return &slCTRangeAttackMG2; } // 機槍兵
                    return &slCTRangeAttack1C;
                }
                else
                {
                    if ( m_ctBehavior != 3 )
                    {
                        if ( RANDOM_LONG( 0, 9 ) < 5 ) m_bStanding = RANDOM_LONG( 0, 1 );
                        return (!m_bStanding) ? &slCTRangeAttack1A : &slCTRangeAttack1B; 
                    }
                    m_bStanding = TRUE;
                    return &slCTRangeAttackMG2;
                }
            }
            break;

        case 0x19: return &slCTRangeAttack2; // 25: 扔雷/重武器發射

        case 0x29: // 41: 戰鬥失敗
            if ( m_hEnemy != nullptr ) return &slCTCombatFail;
            return &slCTFail;

        case 0x2E: return &slCTWander; // 46: 自主巡邏漫步

        case 0x31: // 49: 火力壓制
            if ( m_hEnemy != nullptr && m_hEnemy->IsPlayer() && m_fFirstEncounter )
            {
                m_fFirstEncounter = FALSE;
                return &slCTSignalSuppress; 
            }
            return &slCTSuppress;

        case 0x32: // 50: 建立交火線 (狙擊手雷射清理)
			if ( m_ctBehavior == 4 )
			{
				LaserGlowOff(); // ?? 直接這樣寫，代替原本那 4 行 UTIL_Remove！
			}
            return &slCTEstablishLineOfFire;

        case 0x33: return &slCTHideReload;     // 51: 尋找掩體換彈匣
        case 0x34: return &slCTSweep;          // 52: 陣列扇形區域掃描
        case 0x35: return &slCTFoundEnemy;     // 53: 發現並鎖定敵人

        case 0x36: // 54: 常規索降下滑 (物理速度手動修正)
            if ( pev->velocity.z > -128.0f ) pev->velocity.z -= 32.0f;
            return &slCTRepel;

        case 0x37: // 55: 索降射擊 (物理速度手動修正)
            if ( pev->velocity.z > -128.0f ) pev->velocity.z -= 32.0f;
            return &slCTRepelAttack;

        case 0x38: return &slCTRepelLand;     // 56: 索降著陸
        case 0x39: return &slCTWaitInCover;    // 57: 在掩體中強制等待
        case 0x3C: return &slCTInSmoke;        // 60: 處於煙霧彈中
        case 0x3D: return &slCTFlashed;        // 61: 被閃光彈閃瞎

        case 0x3E: // 62: 狙擊手定點潛伏狙擊
            taskFailCount = 0.0f;
            m_bStanding = TRUE;
            return &slCTSniperAttack;

        case 0x3F: // 63: 狙擊手瞬間射擊 (開火時瞬間關閉雷射線與清除敵人)
            if ( m_hEnemy != nullptr )
            {
                if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
                if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
                m_hEnemy = nullptr; // 完美對齊: EHANDLE::operator=(pEVar1, 0)
            }
            return &slCTSniperTakeShot;

        // ?? 核心優化：整合 case 0x3a / 0x3b 的條件判定路由跳轉
        case 0x3A: 
        case 0x3B: 
            if ( Type == 0x3B )
            {
                m_bStanding = TRUE;
                taskFailCount += 1.0f;
            }
            // 依據條件碼動態重新路由
            if ( m_ctBehavior == 4 )
            {
                if ( HasConditions(0x400) && scheduleTable[0x3e].percentChance > 0.0f ) 
                    return CSquadMonster::GetScheduleOfType( 0x3E );
            }
            else
            {
                if ( HasConditions(0x800) && scheduleTable[0x16].percentChance > 0.0f ) 
                    return CSquadMonster::GetScheduleOfType( 0x16 );
                if ( HasConditions(0x1000) && scheduleTable[0x19].percentChance > 0.0f ) 
                    return CSquadMonster::GetScheduleOfType( 0x19 );
                if ( HasConditions(0x400) && scheduleTable[0x18].percentChance > 0.0f ) 
                    return CSquadMonster::GetScheduleOfType( 0x18 );
            }
            if ( scheduleTable[0x32].percentChance > 0.0f ) return CSquadMonster::GetScheduleOfType( 0x32 );
            if ( scheduleTable[0x42].percentChance > 0.0f ) return CSquadMonster::GetScheduleOfType( 0x42 );
            
            return CSquadMonster::GetScheduleOfType( Type );

        // ?? 核心優化：地圖特殊掩體節點智慧姿態控制機制
        case 0x40: // 64: 進入掩體節點
            if ( m_pSpecialCoverNode != nullptr )
            {
                // 如果是矮牆掩體 (m_nodeType & 1)
                if ( m_pSpecialCoverNode->m_nodeType & 1 ) 
                {
                    m_bStanding = FALSE; // 強制蹲下躲避！
                    return &slCTDuckCoverNode;
                }
                else // 高牆轉角掩體
                {
                    if ( m_pSpecialCoverNode->m_nodeType & 6 ) m_bStanding = TRUE; // 側身探頭保持站姿
                    return &slCTCornerCoverNode;
                }
            }
            break;

        case 0x41: return &slCTDuckNodeAttack;   // 65: 蹲姿掩體探頭反擊
        case 0x42: return &slCTCloseIn;           // 66: 步步逼近壓制
        case 0x45: return &slCTCornerNodeAttack; // 69: 轉角牆面側身反擊
        case 0x46: return &slCTFiringPause;      // 70: 射擊間歇過渡
        case 0x48: return &slCTWaitCrouched;     // 72: 蹲下原地防禦待命

        case 0x47: // 71: 隨行被卡住
            m_bFollowStuck = TRUE; // 補全: m_bFollowStuck 欄位
            return &slCTStopFollowing;
    }

    return CSquadMonster::GetScheduleOfType( Type );
}

Schedule_t *CCounterTerrorist::GetSchedule(void)
{
	// 重設台詞句型
	m_iSentence = -1;

	// 獲取 1 號玩家指針 (單人劇情主機玩家)
	edict_t *pPlayerEdict = INDEX_TO_ENT(1);
	if(pPlayerEdict == nullptr)
	{
		pPlayerEdict = PEOFFSET(0);
	}

	// 1. 全局語音強鎖斷：如果還在強制說話冷卻期，繼續執行等待/說話計畫
	if(gpGlobals->time < m_flStopTalkTime)
	{
		return GetScheduleOfType(1);
	}

	// 2. 索降/飛行狀態機攔截
	if(pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE)
	{
		int iRepelSched = 54;
		if((pev->flags & FL_ONGROUND) == 0)
		{
			pev->takedamage = DAMAGE_NO;
			iRepelSched = (m_MonsterState == MONSTERSTATE_COMBAT) ? 55 : 54;
		}
		else
		{
			pev->takedamage = DAMAGE_AIM;
			pev->movetype   = MOVETYPE_WALK;
			iRepelSched = 56;
		}
		return GetScheduleOfType(iRepelSched);
	}

	// 3. 狙擊手專屬邏輯：刷新時清除舊的雷射和紅點特效
	if(m_ctBehavior == 4)
	{
		LaserGlowOff();
	}

	// 4. 敵對目標感知更新與距離運算
	float flDistToEnemy = 0.0f;
	if(m_hEnemy != nullptr)
	{
		flDistToEnemy = (pev->origin - m_hEnemy->pev->origin).Length2D();
		m_IdealMonsterState = MONSTERSTATE_COMBAT; // 強制進入戰鬥思維
	}

	// 5. 清空狀態機權重池
	ClearScheduleTableFlags();

	// 6. 多重戰術計畫權重池堆疊
	int iHearingSched = GetHearingSchedule();
	if(iHearingSched >= 0 && iHearingSched < 74)
	{
		if(!(m_cantMove && (iHearingSched == 0x12 || iHearingSched == 0x32 || iHearingSched == 0x40 || iHearingSched == 0x42)))
			scheduleTable[iHearingSched].activeFlag = 1.0f;
	}

	// 7. 進入戰鬥思維決策鏈
	if(m_MonsterState == MONSTERSTATE_COMBAT)
	{
		// 戰鬥判定 A：敵人死亡 (0x100000 = bits_COND_ENEMY_DEAD)
		if(HasConditions(0x100000))
		{
			if(RANDOM_LONG(0, 99) > 66 && FOkToSpeak())
			{
				PlaySentence("CT_KILL", 0.8f, 0.8f, 0, m_voicePitch);
				CTalkMonster::g_talkWaitTime = gpGlobals->time + 3.0f;
				m_iSentence = -1;
			}
			if(m_cantMove)
			{
				scheduleTable[0x39].activeFlag = 1.0f;
			}
		}

		// 戰鬥最高優先權：沒子彈了，強行進行換彈評估與快速執行！
		if(HasConditions(bits_COND_NO_AMMO_LOADED))
		{
			scheduleTable[0x33].activeFlag = 1.0f;
			int iReloadSched = GetScheduleFromTable();
			if(iReloadSched != -1)
			{
				HandleSpeaking(iReloadSched);
				ALERT(at_aiconsole, "%s Playing Schedule: %d\n", STRING(pev->classname), iReloadSched);
				return GetScheduleOfType(iReloadSched);
			}
		}

		// 戰鬥判定 B：發現新敵人 (0x10000 = bits_COND_NEW_ENEMY)
		if(HasConditions(0x10000))
		{
			if(FOkToSpeak())
			{
				PlaySentence("CT_ENEMY", 0.8f, 0.8f, 0, m_voicePitch);
				m_iSentence = -1;
				CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT(1.5f, 2.0f);
			}
			if(!m_cantMove)
			{
				scheduleTable[0x32].activeFlag = 1.0f;
				if(m_fPreferedRange < flDistToEnemy)
				{
					scheduleTable[0x42].activeFlag = 1.0f;
				}
				scheduleTable[0x12].activeFlag = 1.0f;
			}
		}

		// 戰鬥判定 C：受傷與重傷反應 (0x100, 0x200, 0x800)
		if(HasConditions(0x100)) {
			scheduleTable[0x11].activeFlag = 1.0f;
			scheduleTable[0x43].activeFlag = 1.0f;
		}
		if(HasConditions(0x200)) {
			scheduleTable[0x11].activeFlag = 1.0f;
			scheduleTable[0x44].activeFlag = 1.0f;
		}
		if(HasConditions(0x800)) {
			scheduleTable[0x16].activeFlag = 1.0f;
		}

		// 戰鬥判定 D：小隊長戰術聯動與包抄追擊 (0x400 = bits_COND_SEE_FEAR)
		if(HasConditions(0x400) && m_hSquadLeader != nullptr)
		{
			CCounterTerrorist* pLeader = (CCounterTerrorist*)m_hSquadLeader.Get();
			if(pLeader == nullptr)
				pLeader = this;

			if(pLeader->m_fEnemyEluded && !HasConditions(0x80000))
			{
				pLeader->m_fEnemyEluded = FALSE;
				return GetScheduleOfType(0x35);
			}

			if(flDistToEnemy < 128.0f || m_ctBehavior != 7)
			{
				scheduleTable[0x18].activeFlag = 1.0f;
			}
			if(justShotFlag)
			{
				scheduleTable[0x46].activeFlag = 1.0f;
			}

			justShotFlag = FALSE;
			scheduleTable[0x3e].activeFlag = 1.0f;

			if(!m_cantMove && flDistToEnemy > m_fPreferedRange)
			{
				scheduleTable[0x42].activeFlag = 1.0f;
			}
			scheduleTable[0x12].activeFlag = 1.0f;
		}

		if(HasConditions(0x1000)) {
			scheduleTable[0x19].activeFlag = 1.0f;
		}
		if(HasConditions(0x20))        // bits_COND_ENEMY_OCCLUDED
		{
			scheduleTable[0x3f].activeFlag = 1.0f;
			if(!m_cantMove)
			{
				scheduleTable[0x32].activeFlag = 1.0f;
				scheduleTable[0x42].activeFlag = 1.0f;
			}
			scheduleTable[0x1c].activeFlag = 1.0f;
		}

		// 常規戰鬥距離與特殊掩體節點動態加權 (對應反編譯中後半段補全)
		if(!m_cantMove && (!HasConditions(0x800) || scheduleTable[0x16].percentChance == 0.0f))
		{
			if(m_fPreferedRange < flDistToEnemy)
			{
				scheduleTable[0x42].activeFlag = 1.0f;
			}
			scheduleTable[0x32].activeFlag = 1.0f;
		}

		// ?? 戰術掩體尋路加權：動態核對周圍有沒有合適的特殊掩體 (Fgd/Node 系統)
		CNode *pCoverNode = CSquadMonster::HasValidSpecialCoverNode();
		m_pSpecialCoverNode = pCoverNode;
		if(pCoverNode != nullptr)
		{
			float flDistToCover = (pev->origin - pCoverNode->m_vecOrigin).Length2D();
			// 如果掩體距離足夠近 (75%比敵人近)，打上 0x40 掩護計畫標籤
			if(flDistToCover * 0.75f < flDistToEnemy && !m_cantMove)
			{
				scheduleTable[0x40].activeFlag = 1.0f;
			}
		}
	}
	else // 8. 非戰鬥狀態 (MONSTERSTATE_IDLE / ALERT) 決策分支
	{
		Schedule_t *pFollowSched = HandleFollowing();
		if(pFollowSched != nullptr)
		{
			return pFollowSched;
		}

		if(m_pCine != nullptr && m_MonsterState == MONSTERSTATE_IDLE)
		{
			m_IdealMonsterState = MONSTERSTATE_SCRIPT;
			return GetScheduleOfType(0x28);
		}

		// 自主戰術遊蕩巡邏 (Wander)
		if(m_hEnemy == nullptr && gpGlobals->time > m_wanderTime && m_canWander && !m_cantMove)
		{
			float flRandDist = RANDOM_FLOAT(64.0f, 256.0f);
			m_vecMoveGoal.x += RANDOM_LONG(-flRandDist, flRandDist);
			m_vecMoveGoal.y += RANDOM_LONG(-flRandDist, flRandDist);
			m_vecMoveGoal.z = pev->origin.z;

			m_wanderOrigin = m_vecMoveGoal;
			m_wanderTime = gpGlobals->time + RANDOM_FLOAT(8.0f, 15.0f);

			return GetScheduleOfType(0x2e);
		}
	}

	// =========================================================================
	// ?? 核心計畫語音調度大模組：依據最終挑選的計畫 ID，動態廣播戰術無線電
	// =========================================================================
    int iFinalSched = GetScheduleFromTable();
    if ( iFinalSched != -1 )
    {
        HandleSpeaking( iFinalSched );
        Schedule_t *pSchedObj = GetScheduleOfType( iFinalSched );
        if ( pSchedObj != nullptr )
        {
            ALERT( at_aiconsole, "%s Playing Schedule: %s\n", STRING(pev->classname), pSchedObj->pName );
            return pSchedObj;
        }
    }
	// 9. 終極兜底返回基底類別
	return CSquadMonster::GetSchedule();
}

int CCounterTerrorist::GetHearingSchedule( void )
{
    // 1. 檢查目前是否觸發了「聽到聲音」的 AI 條件標籤
    if ( HasConditions( bits_COND_HEAR_SOUND ) )
    {
        // 2. 呼叫虛擬函數獲取目前環境中最值得注意的聲音物件 (反編譯中 0x1e0 偏移)
        CSound *pSound = PBestSound();
        
        if ( pSound != nullptr )
        {
            // 3. 危險物檢查：如果聽到的是爆炸或手榴彈等危險聲音 (bits_SOUND_DANGER = 0x20)
            if ( pSound->m_iType & bits_SOUND_DANGER )
            {
                return 19; // 0x13 精確對應 SCHED_TAKE_COVER_FROM_BEST_SOUND (緊急尋找掩體躲避)
            }

            // 4. 敵對動態搜尋：如果目前看不到敵人，但聽到了槍聲或腳步聲 (bits_SOUND_COMBAT | bits_SOUND_PLAYER = 5)
            if ( !HasConditions( bits_COND_SEE_ENEMY ) && (pSound->m_iType & (bits_SOUND_COMBAT | bits_SOUND_PLAYER)) )
            {
                // 計算理想面朝角度 (Ideal Yaw)，讓 SWAT 隊員動態轉身面對聲音來源的方向
                MakeIdealYaw( pSound->m_vecOrigin );
            }
        }
    }

    // 5. 無特殊聽覺反應，交還給主狀態機
    return -1;
}

Vector CCounterTerrorist::GetGunPosition( void )
{
    // 1. 初始化基礎位置為實體原點
    Vector vecGunPos = pev->origin;

    // 2. 依據站立或蹲下狀態，動態疊加 Z 軸高度偏移
    if ( m_bStanding )
    {
        vecGunPos.z += 60.0f; // 站立時槍口高度為 60 單位
    }
    else
    {
        vecGunPos.z += 36.0f; // 蹲下時槍口高度為 36 單位
    }

    // 3. 返回最終計算的三維幾何點
    return vecGunPos;
}

int CCounterTerrorist::GetForcedTargetSchedule( void )
{
    // 1. 檢查目前是否有地圖指派的「強制劇情攻擊目標」
    if ( m_forcedTarget == 0 )
    {
        return -1; // 沒有的話，直接退出，由常規 AI 接管
    }

    // 2. 戰術彈藥檢查
    CheckAmmo(); // 呼叫虛擬函數更新子彈狀態 (反編譯中被優化展開的部分)

    // 3. 換彈優先權：如果發現彈匣已空 (HasConditions(bits_COND_NO_AMMO_LOADED))
    if ( HasConditions( bits_COND_NO_AMMO_LOADED ) )
    {
        return 30; // 0x1e 精確對應 SCHED_RELOAD (強制進入戰術換彈匣狀態)
    }

    // 4. 彈藥充足，執行強制目標攻擊隨機戰術分配
    int iRand = RANDOM_LONG( 0, 100 );
    
    if ( iRand <= 30 )
    {
        return 57; // 0x39 對應 SCHED_FORCED_ATTACK / 特殊戰術壓制行為
    }
    
    return 42; // 0x2a 精確對應 SCHED_RANGE_ATTACK1 (對著強制目標全自動開火)
}

int CCounterTerrorist::GetFlyingSchedule( void )
{
    // 1. 檢查是否處於索降下滑狀態 (MOVETYPE_FLY = 5) 
    // 並且排除躺倒/爬行等特殊受傷狀態 (MONSTERSTATE_PRONE)
    if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
    {
        // 2. 落地檢查：如果雙腳已經接觸地面 (FL_ONGROUND = 0x200)
        if ( pev->flags & FL_ONGROUND )
        {
            pev->takedamage = DAMAGE_AIM; // 1.0f, 解除索降無敵，恢復正常受傷判定
            pev->movetype   = MOVETYPE_WALK; // 4, 切換回常規地面步行物理
            
            return 56; // 0x38 對應 SCHED_REPEL_LAND (落地解脫繩索動畫)
        }

        // 3. 空中滑行檢查：若還在半空中，此時強制開啟【過場動畫無敵保護】
        pev->takedamage = DAMAGE_NO; // 0.0f, 防止空中索降時被打死導致斷劇情

        // 4. 空中戰術動態行為
        if ( m_MonsterState == MONSTERSTATE_COMBAT )
        {
            return 55; // 0x37 對應 SCHED_REPEL_SHOOT (空中遭遇敵人，允許邊下滑邊開火)
        }
        
        return 54; // 0x36 對應 SCHED_REPEL (常規安全下滑)
    }

    // 5. 如果不是索降兵，返回 -1 讓常規 GetSchedule() 去處理
    return -1; 
}

int CCounterTerrorist::GetBehaviorType( void )
{
    // 1. 獲取當前實體的地圖類別名稱字串
    const char* pszClassname = STRING( pev->classname );
    if ( pszClassname == nullptr )
    {
        return 8; // 安全兜底
    }

    // 2. 依照反編譯順序進行字串特徵掃描
    if ( strstr( pszClassname, "pistol" ) != nullptr )       return 0;
    if ( strstr( pszClassname, "shotgun" ) != nullptr )      return 1;
    if ( strstr( pszClassname, "assaultrifle" ) != nullptr ) return 2;
    if ( strstr( pszClassname, "machinegun" ) != nullptr )   return 3;
    if ( strstr( pszClassname, "sniperrifle" ) != nullptr )  return 4;
    if ( strstr( pszClassname, "law" ) != nullptr )          return 5;
    if ( strstr( pszClassname, "smg" ) != nullptr )          return 6;
    if ( strstr( pszClassname, "grenader" ) != nullptr )     return 7;
    if ( strstr( pszClassname, "mp5" ) != nullptr )          return 8;

    // 3. 兜底邏輯：處理純 monster_ct_swat 或自訂類別名的狀況
    // 依據基底成員 m_behaviorType 決定預設兵種行為
    if ( m_behaviorType == 1 )
    {
        return 4; // 預設歸類為 狙擊手行為
    }
    
    return 8; // 預設歸類為 MP5 行為
}

void CCounterTerrorist::FollowerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 1. 劇情最高優先級熔斷：若目前正處於不可打斷的劇情動畫劇本中，直接攔截返回
    if ( m_MonsterState == MONSTERSTATE_SCRIPT )
    {
        return;
    }

    // 2. 獲取單人劇情關卡中的 1 號玩家實體物件
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        return;
    }
    CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );

    // =========================================================================
    // ?? 階段 A：地圖特置事件觸發路由 (優先於隨行系統)
    // =========================================================================
    if ( m_useTarget != 0 )
    {
        const char *pszTargetName = STRING( m_useTarget );
        
        // 完美還原 do-while 內聯循環：排除地圖製作者故意填寫 "none" 的特殊情況
        if ( pszTargetName != nullptr && strcasecmp( pszTargetName, "none" ) != 0 )
        {
            // 觸發遠端目標地圖實體，不加入隨行，直接提前中斷退出
            FireTargets( pszTargetName, pActivator, this, useType, value );
            return;
        }
    }

    // 3. 不可招募審查：若地圖勾選了 SF_MONSTER_GAG (0x100) 旗標，拒絕任何 E 鍵歸隊
    if ( pev->spawnflags & 0x100 )
    {
        return;
    }

    // =========================================================================
    // ??? 階段 B：動態隨行與狀態切換核心矩陣
    // =========================================================================
    
    // 判定 1：若特警目前【已經】處於跟隨玩家的隨行模式中 (執行解除跟隨/原地駐守)
    if ( m_hTargetEnt != nullptr && m_hTargetEnt->IsPlayer() )
    {
        // 一鍵調用 StopFollowing(TRUE)，命令他停下腳步並清除計畫
        StopFollowing( TRUE );

        // 播放反恐精英經典的原地駐守與警戒台詞組
        PlaySentence( "CT_WAIT", 0.8f, 0.8f, 0, m_voicePitch );
        m_iSentence = -1;
        CTalkMonster::g_talkWaitTime = gpGlobals->time + 1.0f; // 鎖定語音通道
    }
    // 判定 2：若特警目前處於自由身 (執行招募入隊/歸隊跟隨)
    else
    {
        // ?? 忠誠度硬熔斷：若玩家之前惡意攻擊過特警 (bits_MEMORY_PROVOKED = 1)，拒絕跟隨
        if ( m_afMemory & 1 ) // bits_MEMORY_PROVOKED
        {
            ALERT( at_console, "I'm not following you, you evil person!\n" );
            return;
        }

        // 戰術大腦降級與防卡死重設：若身邊有敵人，優先切換至 ALERT 警惕模式靠攏玩家
        if ( m_hEnemy != nullptr )
        {
            m_IdealMonsterState = MONSTERSTATE_ALERT;
        }

        // 正式鎖定 1 號玩家為隨行主權目標
        m_hTargetEnt = pPlayer;

        // 無條件抹除身上之前積累的所有尋路被卡死負面旗標
        m_afConditions &= ~bits_COND_TASK_FAILED; // & 0xefffffff

        // 掐斷日常碎碎念動畫，逼迫大腦在下一訊號幀立即刷新 slCTFollow 計畫
        ClearSchedule();

        // ?? 高喊答應與誓死護航入隊台詞組！
        PlaySentence( "CT_OK", 0.8f, 0.8f, 0, m_voicePitch );
        m_iSentence = -1;
        CTalkMonster::g_talkWaitTime = gpGlobals->time + 1.0f;
    }
}

BOOL CCounterTerrorist::FOkToSpeak( void )
{
    // 1. 全局語音冷卻檢查：如果當前時間小於全局對話等待時間，則不能說話
    if ( gpGlobals->time <= CTalkMonster::g_talkWaitTime )
    {
        return FALSE;
    }

    // 2. 玩家語音衝突檢查：獲取 1 號玩家實體（單人遊戲中的玩家）
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 ); // 兜底檢查 0 號偏移
    }

    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        // 取得玩家 C++ 類別指針（假定偏移 0xabc 對應 m_flSpeechWaitTime）
        float flPlayerSpeechWaitTime = *(float *)((char *)pPlayerEdict->pvPrivateData + 0xabc);
        
        // 如果遊戲時間大於 2 秒，且玩家自己正在說話/發送無線電，NPC 選擇保持安靜
        if ( gpGlobals->time > 2.0f && gpGlobals->time < flPlayerSpeechWaitTime )
        {
            return FALSE;
        }
    }

    // 3. 地圖 Gag 標記與戰鬥狀態檢查
    // SF_MONSTER_GAG = 2。如果勾選了閉嘴，且不處於戰鬥狀態，則禁止說話
    if ( (pev->spawnflags & SF_MONSTER_GAG) && m_MonsterState != MONSTERSTATE_COMBAT )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CCounterTerrorist::FCanCheckAttacks( void )
{
    // 完美還原：當 0x80 (bits_COND_...) 條件不成立時，才允許檢查攻擊
    if ( HasConditions( 0x80 ) )
    {
        return FALSE;
    }
    
    return TRUE;
}

void CCounterTerrorist::DeathSound( void )
{
    // 1. 檢查目前是否允許播放語音
    if ( !FOkToSpeak( this ) )
    {
        // 如果不允許說話，播放靜音音效，強行掐斷（Override）他嘴裡還在說的其他台詞
        EMIT_SOUND_DYN( ENT(pev), CHAN_BODY, "common/null.wav", 0.5f, ATTN_NORM, 0, m_voicePitch );
    }

    // 2. 播放 /sound/sentences.txt 中預設的 CT 死亡慘叫語音組
    // (0x3f4ccccd 經浮點數轉換後為 0.8f，對應音量與衰減系數)
    PlaySentence( "CT_DEATH", 0.8f, 0.8f, 0, m_voicePitch );

    // 3. 核心安全鎖：NPC 死亡後，立刻清除並註銷他的 +USE 按鍵回調函式
    // 防止玩家對著屍體按 E 還能觸發跟隨邏輯
    SetUse( nullptr );
}

void CCounterTerrorist::ClearScheduleTableFlags( void )
{
    // 完美還原 do-while 循環：清空 74 個狀態機的所有啟用標記
    for ( int i = 0; i < 74; i++ )
    {
        scheduleTable[i].activeFlag = 0.0f;
    }
}

void CCounterTerrorist::CheckAmmo( void )
{
    if ( m_cAmmoLoaded > 0 )
    {
        return;
    }

    // ?? 修正 1：利用標準 SDK 的公有方法 SetConditions 替代私有的 m_afConditions 操作，
    // 100% 繞過 Android NDK 的 Private 存取阻礙！
    SetConditions( bits_COND_NO_AMMO_LOADED ); 
}

void CCounterTerrorist::ActivateTableEntry( int entry )
{
    if ( entry == 0x12 || entry == 0x32 || entry == 0x40 || entry == 0x42 )
    {
        if ( this->m_cantMove != 0 ) return;
    }

    if ( entry > 0x49 ) // 73
    {
        return;
    }

    this->scheduleTable[entry].activeFlag = 1.0f;
}

int CCounterTerrorist::CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel )
{
    if ( this->m_ctBehavior == 4 )
    {
        if ( m_pBeam != nullptr ) { UTIL_Remove( m_pBeam ); m_pBeam = nullptr; }
        if ( m_pLaserGlow != nullptr ) { UTIL_Remove( m_pLaserGlow ); m_pLaserGlow = nullptr; }
    }
    return CBaseMonster::CanPlaySequence( fDisregardMonsterState, interruptLevel );
}

BOOL CCounterTerrorist::CanFollow( void )
{
    if ( !IsAlive() ) return FALSE;
    if ( m_hTargetEnt != nullptr && m_hTargetEnt->IsPlayer() ) return FALSE;
    return TRUE; 
}

BOOL CCounterTerrorist::CheckRangeAttack1( float flDot, float flDist )
{
    // bits_COND_ENEMY_OCCLUDED (0x20)
    if ( (HasConditions(bits_COND_ENEMY_OCCLUDED)) || (flDist > 2048.0f) || (flDot < 0.5f) )
    {
        return FALSE;
    }

    //if ( !CBaseMonster::NoFriendlyFire() )
    //{
    //    return FALSE; 
    //}

    if ( m_hEnemy == nullptr ) return FALSE;

    Vector vecSrc = GetGunPosition();
    Vector vecEnd = m_hEnemy->BodyTarget( vecSrc );

    TraceResult tr;
    UTIL_TraceLine( vecSrc, vecEnd, ignore_monsters, ignore_glass, edict(), &tr );

    if ( tr.flFraction == 1.0f )
    {
        return TRUE; 
    }

    return FALSE;
}

BOOL CCounterTerrorist::CheckMeleeAttack1( float flDot, float flDist )
{
    if ( m_hEnemy != nullptr )
    {
        CBaseMonster *pEnemyMonster = m_hEnemy->MyMonsterPointer();
        if ( pEnemyMonster == nullptr ) return FALSE;
    }
    if ( flDist <= 64.0f && flDot >= 0.7f )
    {
        return TRUE; 
    }
    return FALSE;
}

BOOL CCounterTerrorist::CheckRangeAttack2( float flDot, float flDist )
{
    // 1. 安全檢查：如果身上沒有配置對應的重武器/手榴彈，直接不允許攻擊
    // (反編譯對應: pev->weapons & 6U)
    if ( (pev->weapons & (bits_WEAPON_GRENADE | bits_WEAPON_LAW)) == 0 )
    {
        return FALSE;
    }

    // 2. 行動狀態檢查：必須停下腳步才能投擲/發射 (反編譯對應: m_flGroundSpeed == 0.0)
    if ( m_flGroundSpeed != 0.0f )
    {
        return FALSE;
    }

    // 3. 冷卻時間（Timer）檢查：未到計時器時間，直接返回上一次的投擲決定
    if ( gpGlobals->time < m_flNextGrenadeCheck )
    {
        return m_fThrowGrenade;
    }

    // 4. 目標合法性檢查
    if ( m_hEnemy == nullptr )
    {
        m_fThrowGrenade = FALSE;
        return FALSE;
    }

    // 取得敵人最後已知位置 (Enemy Last Known Position)
    Vector vecTarget = m_vecEnemyLKP;

    // 如果敵人沒有處於「隱蔽/不可見」狀態，更新其精確原點
    // (反編譯對應: (m_afConditions & 0x10) 即 bits_COND_ENEMY_OCCLUDED)
    if ( !HasConditions(bits_COND_ENEMY_OCCLUDED) )
    {
        vecTarget = m_hEnemy->pev->origin;
    }

    // 5. 隊友安全檢查 (Friendly Fire Prevention)
    // 檢查是否有同小隊成員距離目標落點 256 單元以內，避免炸死隊友
    if ( m_hSquadLeader != nullptr )
    {
        if ( CSquadMonster::SquadMemberInRange( vecTarget, 256.0f ) )
        {
            m_fThrowGrenade = FALSE;
            m_flNextGrenadeCheck = gpGlobals->time + 1.0f; // 1秒內不准再嘗試
            return FALSE;
        }
    }

    // 6. 距離檢查：太近不扔（炸到自己），反編譯中判定距離需大於 256
    float flTargetDist = (vecTarget - pev->origin).Length2D();
    if ( flTargetDist <= 256.0f )
    {
        m_fThrowGrenade = FALSE;
        m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
        return FALSE;
    }

    // 7. 彈道軌跡預測計算 (Trajectory Check)
    Vector vecTossVel;
    Vector vecSrc = GetGunPosition(); // 反編譯中的虛函數呼叫：this->_vptr.CBaseEntity[0x20c]

    if ( (pev->weapons & bits_WEAPON_LAW) == 0 )
    {
        // 模式 A：常規拋體投擲手榴彈
        // 利用引擎內建的 VecCheckToss 計算初速度
        VecCheckToss( pev, vecSrc, vecTarget, &vecTossVel, 0.5f );
    }
    else
    {
        // 模式 B：火箭筒 (LAW) 或 榴彈發射器
        // 使用定速平拋/強推軌跡 VecCheckThrow
        float flGrenadeSpeed = gSkillData.hgruntGrenadeSpeed; // 借用了 HGrunt 的數值平衡庫
        VecCheckThrow( pev, vecSrc, vecTarget, flGrenadeSpeed, 0.5f, &vecTossVel );
    }

    // 8. 評估彈道結果
    if ( vecTossVel == g_vecZero )
    {
        // 軌跡被地形阻擋，投擲失敗
        m_fThrowGrenade = FALSE;
        m_flNextGrenadeCheck = gpGlobals->time + 1.0f; // 1秒後重試
        return FALSE;
    }

    // 軌跡合法！儲存拋射初速度，並設定短暫延時立刻進入攻擊 Sequence
    m_fThrowGrenade = TRUE;
    m_vecTossVelocity = vecTossVel; // 儲存速度供後續 Throw/Fire 函數取用
    
    // 設定冷卻，0.3秒內維持這個狀態以完成動畫過渡
    m_flNextGrenadeCheck = gpGlobals->time + 0.3f; 
    
    return TRUE;
}

void CCounterTerrorist::SpeakSentence( void )
{
    // 1. 檢查台詞緩衝槽：若目前憋著一句尚未喊出的台詞 (-1 代表空槽)
    if ( m_iSentence != -1 )
    {
        // 2. 語音鎖審查：若目前戰場環境允許開口說話
        if ( FOkToSpeak() )
        {
            // 讀取全局恐怖分子/特警通用台詞陣列，播放對應的戰術語音 (0.8f 音量)
            PlaySentence( pTerroristSentences[m_iSentence], 0.8f, 0.8f, 0, m_voicePitch );

            // 3. 釋放緩衝槽
            m_iSentence = -1;

            // 4. 動態重設 1.5 ~ 2.0 秒的戰術隨機冷卻，防止下一句台詞重疊雜音
            float flRandomWait = RANDOM_FLOAT( 1.5f, 2.0f );
            CTalkMonster::g_talkWaitTime = gpGlobals->time + flRandomWait;
        }
    }
}

void CCounterTerrorist::StartFollowing( CBaseEntity *pLeader )
{
    if ( pLeader == nullptr )
    {
        return;
    }

    // 1. 戰術思維降級：若目前正在與敵人交火，將理想思考狀態強制轉為 ALERT (警戒模式)
    // 確保特警能聽從玩家撤退命令，優先轉身靠攏
    if ( m_hEnemy != nullptr )
    {
        m_IdealMonsterState = MONSTERSTATE_ALERT;
    }

    // 2. 將跟隨目標 (通常是玩家) 鎖定寫入隨行實體槽位
    m_hTargetEnt = pLeader;

    // 3. 核心防卡死：利用位元遮罩無條件清空身上所有已積累的任務失敗/尋路死鎖條件碼旗標
    m_afConditions &= ~bits_COND_TASK_FAILED; // 完美還原 & 0xefffffff 二進位行為

    // 4. 計畫強鎖斷：立刻掐斷當前所有日常閒置動作，逼迫大腦在下一幀重新執行跟隨尋路計畫
    ClearSchedule();
}

void CCounterTerrorist::StartTask( Task_t *pTask )
{
    // 預設將任務狀態標記為 1 (TASKSTATUS_RUNNING 啟動中)
    m_iTaskStatus = TASKSTATUS_RUNNING; 

    switch ( pTask->iTask )
    {
        // =========================================================================
        // 任務 18 / 19：掩體尋路前置記憶體清理 (bits_MEMORY_FLINCHED = 0x02)
        // =========================================================================
        case 0x12:
        case 0x13:
            m_afMemory &= ~0x02; // 無條件洗掉受擊中彈硬直記憶，防止奔跑途中產生抽搐 Bug
            CSquadMonster::StartTask( pTask );
            return;

        // =========================================================================
        // 任務 28 / 30：索降滑行特殊狀態啟動
        // =========================================================================
        case 0x1C:
        case 0x1E:
            if ( m_cantMove < 2 )
            {
                CSquadMonster::StartTask( pTask );
                if ( pev->movetype == MOVETYPE_FLY ) // 5 (掛在直升機繩索上)
                {
                    m_IdealActivity = ACT_GLIDE; // 強行進入沿繩索急速下滑滑行序列
                }
                return;
            }
            break;

        // =========================================================================
        // 任務 38 (0x26)：戰術換彈匣啟動
        // =========================================================================
        case 0x26:
            m_IdealActivity = ACT_RELOAD; // 啟動長短槍自定義換彈匣序列
            return;

        // =========================================================================
        // 任務 93 (0x5D)：語音緩衝槽即時發射
        // =========================================================================
        case 0x5D:
            SpeakSentence(); // 嘗試將憋在緩衝槽內的台詞第一時間喊出來
            break;

        // =========================================================================
        // 任務 94 (0x5E)：Friendly Fire 隊友阻擋開火熔斷審查
        // =========================================================================
        case 0x5E:
            // 檢查彈道上是否有友軍 (NoFriendlyFire = TRUE 代表安全，FALSE 代表阻擋)
            if ( !CSquadMonster::NoFriendlyFire() )
            {
                m_afConditions |= 0x10000000; // 阻擋開火！打上特定任務失敗/無法射擊旗標
            }
            if ( m_afConditions & 0x40000000 ) // bits_COND_TASK_FAILED 兜底
            {
                return;
            }
            break;

        // =========================================================================
        // 任務 95 (0x5F)：特殊地圖掩體節點智慧尋路衝鋒 (Move to Cover Node)
        // =========================================================================
        case 0x5F:
            if ( m_pSpecialCoverNode == nullptr || !m_pSpecialCoverNode->IsValidNode() ) 
            {
                m_afConditions |= 0x40000000; // 掩體失效，打上任務失敗旗標 bits_COND_TASK_FAILED
                return;
            }
            // 命令特警全速跑步衝向地圖掩體節點原點座標
            if ( !MoveToLocation( ACT_RUN, 0.0f, m_pSpecialCoverNode->m_vecOrigin ) )
            {
                ALERT( at_console, "FAILED MOVING TO LOCATION.\n" );
                m_afConditions |= 0x40000000; 
                return;
            }
            break;

        // =========================================================================
        // 任務 96 (0x60)：黃金戰術交火距離全速推進逼近 (Close In Task Init)
        // =========================================================================
        case 0x60:
            if ( m_cantMove == 0 && m_hEnemy != nullptr )
            {
                float flDistToEnemy = (pev->origin - m_hEnemy->pev->origin).Length2D();
                if ( flDistToEnemy >= 64.0f ) // 距離大於 64 碼時，發動步兵線合攏尋路
                {
                    m_vecMoveGoal = m_hEnemy->pev->origin;
                    if ( MoveToEnemy( ACT_RUN, 0.0f ) )
                    {
                        return; // 順利跨入奔跑追擊
                    }
                    m_afConditions |= 0x40000000; // 尋路失敗熔斷
                    return;
                }
            }
            break;

        // =========================================================================
        // 任務 97 (0x61)：回歸自主巡邏隨機遊蕩原點 (Move to Wander Origin)
        // =========================================================================
        case 0x61:
            if ( !MoveToLocation( ACT_WALK, 0.0f, m_wanderOrigin ) )
            {
                m_afConditions |= 0x40000000;
            }
            break;

        // =========================================================================
        // 任務 104 (0x68)：隨行對話原地轉身與 45 度大底座死區幾何鎖 (Face Talk Target)
        // =========================================================================
        case 0x68:
            if ( m_hTalkTarget != nullptr && m_cantMove != 2 )
            {
                pev->yaw_speed = 60.0f; // 原地調整轉向速度
                
                // 計算自身原點指向對話焦點玩家的方向向量
                Vector vecDir = m_hTalkTarget->pev->origin - pev->origin;
                float flTargetYaw = UTIL_VecToYaw( vecDir );
                float flDeltaYaw  = flTargetYaw - pev->angles.y;

                // 角度歸一化鎖定
                if ( flDeltaYaw > 180.0f )  flDeltaYaw -= 360.0f;
                if ( flDeltaYaw < -180.0f ) flDeltaYaw += 360.0f;

                // ?? 45 度死區夾角優化：若夾角在 45 度內，身體不轉，僅脖子扭過去；大於 45 度身體才跟進原地轉向
                if ( flDeltaYaw < 0.0f )
                {
                    float flYawOffset = 0.0f;
                    if ( flDeltaYaw + 45.0f < 0.0f ) flYawOffset = flDeltaYaw + 45.0f;
                    pev->ideal_yaw = pev->angles.y + flYawOffset;
                }
                else
                {
                    float flYawOffset = flDeltaYaw - 45.0f;
                    if ( flDeltaYaw - 45.0f <= 0.0f ) flYawOffset = 0.0f;
                    pev->ideal_yaw = pev->angles.y + flYawOffset;
                }
            }
            break;

        // 其餘常規次要任務，直接一鍵降級派發給基底類別小隊怪物去初始加載
        default:
            CSquadMonster::StartTask( pTask );
            return;
    }

    // 任務成功完成一階段初始化，將狀態重設為 4 (TASKSTATUS_COMPLETE 完成，交由 RunTask 接管)
    if ( (m_afConditions & 0x40000000) == 0 )
    {
        m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
    }
}

void CCounterTerrorist::StopFollowing( BOOL clearSchedule )
{
    // 1. 審查當前隨行目標，確保其有效且該目標是玩家
    if ( m_hTargetEnt != nullptr && m_hTargetEnt->IsPlayer() )
    {
        // 2. 移動目標審查：若當前的尋路目標是動態實體 (MOVEGOAL_TARGETENT = 1)
        if ( m_movementGoal == 1 ) 
        {
            RouteClear(); // 強行清空當前尋路路徑，命令特警立即停下腳步
        }

        // 3. 註銷歸零跟隨隨行目標
        m_hTargetEnt = nullptr;

        // 4. 計畫重置判定：若指定要求清理計畫，立刻打斷當前正在播放的隨行骨骼序列
        if ( clearSchedule )
        {
            ClearSchedule();
        }

        // 5. 戰意升級：若解除跟隨的瞬間現場突然冒出死敵，大腦理想思維瞬間拉回 COMBAT (戰鬥模式)
        if ( m_hEnemy != nullptr )
        {
            m_IdealMonsterState = MONSTERSTATE_COMBAT;
            return;
        }
    }
}

int CCounterTerrorist::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
    // 1. 受擊記憶清理：洗掉舊的硬直旗標 (bits_MEMORY_FLINCHED = 0x02)
    m_afMemory &= ~0x02;

    if ( pevAttacker != nullptr && pevAttacker->pContainingEntity != nullptr )
    {
        // 2. 利用引擎實例化獲取開槍攻擊者的 C++ 實體物件指針
        CBaseEntity *pAttacker = CBaseEntity::Instance( pevAttacker->pContainingEntity );
        
        // 3. 核心解密：利用虛擬接口 (0xA8 偏移) 判定開槍打中特警的攻擊者是不是玩家 (友軍傷害)
        if ( pAttacker != nullptr && pAttacker->IsPlayer() )
        {
            // 4. ?? 友軍傷害防禦鎖激活：高喊 "CT_SHOT" 無線電警告玩家注意彈道
            PlaySentence( "CT_SHOT", 0.8f, 0.8f, 0, m_voicePitch );
            
            // 鎖定語音通道 1 秒，清空緩衝槽
            CTalkMonster::g_talkWaitTime = gpGlobals->time + 1.0f;
            m_iSentence = -1;

            // 5. ??? 核心降噪：強行將痛苦叫聲冷卻時間延遲 2 秒，優先確保誤傷警告台詞廣播完畢
            m_flNextPainTime = gpGlobals->time + 2.0f;
        }
    }

    // 6. 最終放行：交還給基底類別核心去計算護甲折減、實時扣除生命值
    return CSquadMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

bool CCounterTerrorist::ValidateActivation( int entry )
{
    // =========================================================================
    // ?? 戰術移動可行性審查矩陣
    // 18=尋找掩體, 50=建立交火線, 64=走進掩體節點, 66=突擊逼近
    // =========================================================================
    if ( entry == 0x12 || entry == 0x32 || entry == 0x40 || entry == 0x42 )
    {
        // 核心安全鎖：若特警目前處於無法移動/定身狀態 (m_cantMove != 0)，無條件熔斷該計畫的激活權限
        if ( m_cantMove != 0 )
        {
            return false;
        }
    }

    // 非位移動作（原地開火、換彈、說話、硬直等），無條件放行允許激活
    return true;
}

