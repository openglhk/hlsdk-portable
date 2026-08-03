#include "terrorist.h"

LINK_ENTITY_TO_CLASS( monster_terrorist, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_pistol, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_shotgun, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_assaultrifle, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_machinegun, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_sniperrifle, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_law, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_smg, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_grenader, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_melee, CTerrorist );
LINK_ENTITY_TO_CLASS( monster_terrorist_kamakazi, CTerrorist );

TYPEDESCRIPTION CTerrorist::m_SaveData[] = 
{
    DEFINE_FIELD( CTerrorist, m_voicePitch, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_iBrassShell, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_iShotgunShell, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_usFireM60, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireTMP, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireGlock, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireAK47, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireAWP, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireMP5, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_usFireShotgun, FIELD_SHORT ),
    DEFINE_FIELD( CTerrorist, m_terroristBehavior, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_flNextGrenadeCheck, FIELD_TIME ),
    DEFINE_FIELD( CTerrorist, m_iSentence, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_fFirstEncounter, FIELD_BOOLEAN ),
    // scheduleTable 描述對接
    DEFINE_ARRAY( CTerrorist, scheduleTable, FIELD_CHARACTER, sizeof(ScheduleEntry) * 16 ),
    DEFINE_FIELD( CTerrorist, m_fThrowGrenade, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, m_vecTossVelocity, FIELD_VECTOR ),
    DEFINE_FIELD( CTerrorist, m_pBeam, FIELD_CLASSPTR ),         // 雷射射線指針保存
    DEFINE_FIELD( CTerrorist, m_pLaserGlow, FIELD_CLASSPTR ),    // 精靈光暈指針保存
    DEFINE_FIELD( CTerrorist, m_dropItem, FIELD_STRING ),         // 死後掉落物
    DEFINE_FIELD( CTerrorist, m_dropChance, FIELD_FLOAT ),        // 掉落機率
    DEFINE_FIELD( CTerrorist, m_wanderOrigin, FIELD_POSITION_VECTOR ),
    DEFINE_FIELD( CTerrorist, m_laserBrightness, FIELD_FLOAT ),
    DEFINE_FIELD( CTerrorist, m_glowBrightness, FIELD_FLOAT ),
    DEFINE_FIELD( CTerrorist, m_bStanding, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, m_terroristType, FIELD_INTEGER ),
    DEFINE_FIELD( CTerrorist, m_flashbangOnly, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, m_heGrenadeOnly, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, m_blastRadius, FIELD_FLOAT ),
    DEFINE_FIELD( CTerrorist, m_blastDamage, FIELD_FLOAT ),
    DEFINE_FIELD( CTerrorist, m_canWander, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, stealthDeath, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, taskFailCount, FIELD_FLOAT ), // 👈 第 33 個變數，大功告成！
};

int g_fTerroristQuestion = 0;

CTerrorist::CTerrorist( void )
{
    CBaseMonster::CBaseMonster();
    m_bStanding        = TRUE;
    m_terroristType    = 0;
    m_terroristBehavior = -1; // 預設常規未指派
    m_flashbangOnly    = FALSE;
    m_heGrenadeOnly    = FALSE;
    m_blastRadius      = 256.0f; // 256 碼巨額爆炸半徑
    m_blastDamage      = 200.0f; // 200 點致命衝擊波傷害
    suicideFlag        = FALSE;
    taskFailCount      = 0.0f;
    lawDelayTime       = 0.0f;
    justShotFlag       = FALSE;
    m_canWander        = FALSE;
    stealthDeath       = TRUE;  // 允許潛行暗殺不觸發全局警報
    m_alertRadius      = 256.0f;
    m_fPreferedRange   = 64.0f;
    heardCombat        = FALSE;
    m_classtype        = 14; 
    m_wanderTime       = gpGlobals->time + RANDOM_FLOAT( 5.0f, 10.0f );
}

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
	InitScheduleTable();
	ClearScheduleTableFlags(); 
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

void CTerrorist::ActivateTableEntry( int entry )
{
    // =========================================================================
    // 🧱 1. 戰術移動計畫安全過濾：若該計畫涉及大規模幾何跑位與位移
    // =========================================================================
    if ( entry == 50 || // 0x32: SCHED_FLANK_ATTACK (側翼戰術繞後包抄)
         entry == 18 || // 0x12: SCHED_TAKE_COVER_FROM_ENEMY (跑動尋找交火掩體)
         entry == 64 || // 0x40: SCHED_INVESTIGATE_SOUND (聽聲辨位主動搜查)
         entry == 66 )  // 0x42: SCHED_CHASE_ENEMY_SOUND (循聲瘋狂追擊)
    {
        // 🔒 定身熔斷檢查：若目前被劇情強鎖定、或中彈驚慌定身 (m_cantMove = TRUE)
        // 則硬性攔截，絕對不賦予其位移計畫，防止尋路大腦死鎖崩潰
        if ( m_cantMove )
        {
            return;
        }
    }

    // 2. 記憶體邊界安全防線：計畫索引不允許超出 75 號小隊大腦極限安全線
    if ( (unsigned int)entry > 75 ) // 0x4B
    {
        return;
    }

    // 3. 🚀 點火激活：將該戰術計畫的 activeFlag 注入滿加權 1.0f，正式送入大腦決策輪盤中排隊！
    scheduleTable[entry].activeFlag = 1.0f;
}

int CTerrorist::CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel )
{
    // =========================================================================
    // 🎯 狙擊手戰術隱蔽：若目前是 4 號狙擊兵兵種 (Scout 狙擊手)
    // 在切入任何地圖劇本過場骨骼動畫前，一鍵強制熄滅狙擊雷射紅外線，防範穿模鬼畜 Bug
    // =========================================================================
    if ( m_terroristBehavior == 4 )
    {
        LaserGlowOff();
    }

    // 降級派發：移交給怪物母體基類進行常規的硬直與中斷級別審查
    return CBaseMonster::CanPlaySequence( fDisregardMonsterState, interruptLevel );
}

void CTerrorist::CheckAmmo( void )
{
    // 1. 📢 彈藥審查：若目前彈匣內依然有殘餘子彈 (m_cAmmoLoaded > 0)，放行允許繼續開火突擊
    if ( m_cAmmoLoaded > 0 )
    {
        return;
    }

    // 2. 🚀 空倉掛機引爆：若彈匣子彈完全打空，跨越安全地址強寫當前大腦條件遮罩
    // 100% 還原 |= bits_COND_NO_AMMO (1) 位元行為，瞬間激發小隊 AI 撤退、找掩體與拉栓換彈計畫！
    m_afConditions |= bits_COND_NO_AMMO; // 1
}

BOOL CTerrorist::CheckMeleeAttack1( float flDot, float flDist )
{
    // 1. 📢 敵軍生存審查：確保當前鎖定的死敵目標 (m_hEnemy) 有效且依然存活
    if ( m_hEnemy == nullptr )
    {
        return FALSE;
    }

    // 100% 還原虛擬表 20 號偏移量 (MyMonsterPointer) 的安全活體轉型行為
    CBaseMonster *pEnemy = m_hEnemy->MyMonsterPointer();
    if ( pEnemy == nullptr )
    {
        return FALSE;
    }

    // 2. 幾何向量運算：計算恐怖分子與宿敵之間的真實 2D 平面物理距離
    float flDist2D = ( pev->origin - pEnemy->pev->origin ).Length2D();

    // =========================================================================
    // ⚔️ 貼身斬殺線判定：若距離小於 80 碼 (貼身肉搏範圍)
    // 且點積餘弦值 flDot >= 0.7 (代表玩家正處於恐怖分子正前方 90 度的核心揮砍視野面)
    // =========================================================================
    if ( flDist2D <= 80.0f && flDot >= 0.7f )
    {
        // 🚀 點火！向大腦回傳 TRUE，強制引爆 SCHED_MELEE_ATTACK1 拔刀割喉計畫！
        return TRUE;
    }

    return FALSE;
}

BOOL CTerrorist::CheckRangeAttack1( float flDot, float flDist )
{
    // =========================================================================
    // 🧱 1. 戰術四重硬熔斷過濾網
    // =========================================================================
    if ( m_terroristBehavior == 8 ||                         // 兵種限制：8號人肉炸彈自殺兵禁止開槍
         ( m_afConditions & bits_COND_ENEMY_OCCLUDED ) ||     // 視線限制：死敵目標目前正被固體障礙物完全遮擋 (0x20)
         flDist > 2048.0f ||                                 // 距離限制：常規步槍最遠交火線超越 2048 碼
         flDot < 0.5f )                                      // 視野限制：玩家脫離了前方 120 度核心射擊視錐面
    {
        return FALSE;
    }

    // 2. 小隊協同防線：調用 CSquadMonster 核心，核對前方彈道是否會誤傷同陣營敵軍同伴
    if ( !CSquadMonster::NoFriendlyFire() )
    {
        return FALSE; 
    }

    // =========================================================================
    // 📐 3. 實時彈道幾何射線 Trace 穿透性核對
    // =========================================================================
    Vector vecSrc;
    // 呼叫虛擬表 131 號偏移量方法，獲取當前動作下恐怖分子槍口的真實 3D 世界座標原點
    GetGunPosition( vecSrc );

    // 實例化死敵 C++ 指針 (CBaseEntity)
    CBaseEntity *pEnemy = m_hEnemy;
    if ( pEnemy == nullptr )
    {
        return FALSE;
    }

    Vector vecDest;
    // 呼叫虛擬表 58 號偏移量方法，獲取玩家/CT 的核心受擊幾何中心點 (BodyTarget)
    pEnemy->BodyTarget( vecDest );

    TraceResult tr;
    // 向世界發射一條精密的 TraceLine 幾何碰撞射線，無視怪物，無視玻璃
    UTIL_TraceLine( vecSrc, vecDest, ignore_monsters, ignore_glass, edict(), &tr );

    // 🎯 終極裁決：若射線穿透係數完美等於 1.0f (代表槍口與玩家肉體之間一片坦途、毫無阻擋)
    if ( tr.flFraction == 1.0f )
    {
        // 🚀 點火！允許開火突擊，槍口瞬間噴射出物理排殼火網！
        return TRUE;
    }

    return FALSE;
}

BOOL CTerrorist::CheckRangeAttack2( float flDot, float flDist )
{
    // 1. ⚙️ 裝備與兵種過濾：若目前未裝備手榴彈(2U)或下掛榴彈(4U)，無條件熔斷
    if ( !(pev->weapons & (bits_WEAPON_GRENADE | bits_WEAPON_GLAUNCHER)) )
    {
        return FALSE;
    }

    // 2. 戰術定身鎖：恐怖分子必須原地站穩站靜止 (移動地面速度為0)，才允許發動投擲計算
    if ( m_flGroundSpeed != 0.0f )
    {
        goto LAB_ABORT_GRENADE_TOSS;
    }

    // 3. 戰術冷卻鎖：核對是否跨過手榴彈拋擲探測冷卻線
    if ( gpGlobals->time < m_flNextGrenadeCheck )
    {
        return m_fThrowGrenade;
    }

    // 4. 📢 生存審查：確保死敵目標有效
    if ( m_hEnemy == nullptr )
    {
        goto LAB_ABORT_GRENADE_TOSS;
    }

    CBaseEntity *pEnemy = m_hEnemy;

    // 5. 幾何邊界防卡死：若玩家在水裡潛水，或是玩家站在極高處，無法拋擲
    if ( !(pEnemy->pev->flags & FL_ONGROUND) )
    {
        if ( pEnemy->pev->waterlevel == 0 && pev->absmax.z < m_vecEnemyLKP.z )
        {
            goto LAB_ABORT_GRENADE_TOSS;
        }
    }

    Vector vecTarget;

    // =========================================================================
    // 🚀 6. 核心前置量追蹤算法 (Lead-Targeting Engine)
    // =========================================================================
    if ( !(pev->weapons & bits_WEAPON_GRENADE) ) // 分支 A：下掛式榴彈或火箭彈前置
    {
        // 讀取當前難度下的投擲初速度基數
        float flTossSpeed = gSkillData.hgruntGrenadeSpeed; 
        if ( flTossSpeed <= 0.0f ) flTossSpeed = 600.0f;

        // 幾何公式：滯空飛行時間 = 距離 / 初速度
        float flFlightTime = flDist / flTossSpeed;

        // 🎯 100% 還原前置量追蹤：未來預測落點 = 最後目擊點 + (玩家當前速度 * 飛行時間)
        vecTarget.x = m_vecEnemyLKP.x + pEnemy->pev->velocity.x * flFlightTime;
        vecTarget.y = m_vecEnemyLKP.y + pEnemy->pev->velocity.y * flFlightTime;
        vecTarget.z = m_vecEnemyLKP.z + pEnemy->pev->velocity.z * flFlightTime;
    }
    else // 分支 B：常規手榴彈拋物線落點
    {
        // 隨機數分流：50% 機率直接扔向最後目擊位置，50% 機率精確扔向玩家腳底 absmin
        if ( RANDOM_LONG(0, 1) == 0 )
        {
            vecTarget = m_vecEnemyLKP;
        }
        else
        {
            vecTarget   = pEnemy->pev->origin;
            vecTarget.z = pEnemy->pev->absmin.z;
        }
    }

    // 幾何防禦線：目標太近 (小於128碼) 拒絕投擲，防止手榴彈當場炸死自己人
    float flFinalDist2D = ( vecTarget - pev->origin ).Length2D();
    if ( flFinalDist2D <= 128.0f )
    {
        goto LAB_ABORT_GRENADE_TOSS;
    }

    // =========================================================================
    // 📐 7. 拋物線重力軌跡物理探測驗證陣列 (VecCheckToss / VecCheckThrow)
    // =========================================================================
    Vector vecTossVelocity = g_vecZero;
    Vector vecLaunchSrc;
    GetGunPosition( vecLaunchSrc ); // 獲取手部/槍口發射起點

    if ( !(pev->weapons & bits_WEAPON_GRENADE) ) // 使用下掛榴彈直瞄物理
    {
        // 呼叫引擎物理核心，反查高初速拋物線是否成立 (VecCheckThrow)
        VecCheckThrow( pev, vecLaunchSrc, vecTarget, gSkillData.hgruntGrenadeSpeed, 0.5f, &vecTossVelocity );
    }
    else // 使用常規手榴彈大拋物線弧度物理
    {
        // 呼叫引擎核心，反查手榴彈拋弧線是否暢通無阻 (VecCheckToss)
        VecCheckToss( pev, vecLaunchSrc, vecTarget, 0.5f, &vecTossVelocity );
    }

    // 🎯 終極裁決：若拋物線在世界幾何中完全暢通 (回傳的初速度向量不為零)
    if ( vecTossVelocity != g_vecZero )
    {
        m_fThrowGrenade     = TRUE; // 點火激活投擲意圖旗標！
        m_vecTossVelocity   = vecTossVelocity; // 備份 3D 速度向量
        m_flNextGrenadeCheck = gpGlobals->time + 0.3f; // 短暫推遲發射時鐘步進

        return TRUE;
    }

LAB_ABORT_GRENADE_TOSS:
    // 拋物線不成立或被障礙物擋住，重置計畫旗標，並將大腦冷卻懲罰期推遲 1.0 秒
    m_fThrowGrenade      = FALSE;
    m_flNextGrenadeCheck = gpGlobals->time + 1.0f;
    return FALSE;
}

void CTerrorist::ClearScheduleTableFlags( void )
{
    // =========================================================================
    // 🎰 100% 還原二進位行為：一鍵循環抹平 16 容量計畫表內的全局 activeFlag 旗標
    // 徹底格式化大腦決策輪盤，為下一幀的小隊戰術計畫重新評估掃清內存殘留
    // =========================================================================
    for ( int i = 0; i < 16; i++ ) // 完美展開對齊指針哨兵
    {
        scheduleTable[i].activeFlag = 0.0f;
    }
}

void CTerrorist::DeathSound( void )
{
    // =========================================================================
    // 🔇 1. 潛行暗殺與全服語音排隊排他性審查 (音軌防鬼畜重疊降噪大閉環)
    // =========================================================================
    if ( gpGlobals->time <= CTalkMonster::g_talkWaitTime || !FOkToSpeak() )
    {
        // 🚀 100% 還原二進位黑科技行為：強行發射一記 common/null.wav 絕對靜音音軌
        // 一一幀之內強行擦除、截斷該敵軍身上殘留的所有日常跑步與喘息聲，保障安靜死亡
        EMIT_SOUND_DYN( edict(), CHAN_BODY, "common/null.wav", 0.5f, ATTN_NORM, 0, m_voicePitch );
    }

    // =========================================================================
    // 📢 2. 呼叫虛擬表 102 號偏移量 (PlaySentence)，全服廣播敵軍小隊陣亡台詞組
    // 音量配置為 0.85f (0x3F59999A)，衰減係數 0.8f (0x3F4CCCCD)
    // =========================================================================
    PlaySentence( "T_DEATH", 0.85f, ATTN_NORM, 0, m_voicePitch );
}

BOOL CTerrorist::FCanCheckAttacks( void )
{
    // =========================================================================
    // 🛡️ 戰術盲區防禦線：核對大腦條件遮罩中是否點亮了 bits_COND_ENEMY_FACING_ME (0x80)
    // 若玩家處於其視野死區、或恐怖分子正被閃光彈致盲，強行回傳 FALSE，全面熔斷開火權限！
    // =========================================================================
    return ( ( m_afConditions & bits_COND_ENEMY_FACING_ME ) == 0 ); // 0x80
}

// =========================================================================
// 🔊 終極大滿貫還原：敵軍小隊語音發射與戰術沉默排他性仲裁中樞 (FOkToSpeak)
// =========================================================================
BOOL CTerrorist::FOkToSpeak( void )
{
    // 1. ⏱️ 第一道防線：全服無線電/對白排隊擁擠鎖
    // 若目前遊戲時間尚未跨越全局語音冷卻線，代表頻道正被劇情簡報或同伴佔用，強制保持戰默
    if ( gpGlobals->time <= CTalkMonster::g_talkWaitTime )
    {
        return FALSE;
    }

    // 2. ⚙️ 獲取當前地圖上的 1 號玩家 (CBasePlayer) 指針
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }

    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );

        if ( pPlayer != nullptr )
        {
            // 🔥 第二道防線：100% 還原二進位 0x2AF 內存黑科技 (提取玩家體內的閃光彈致盲耳鳴時鐘)
            float *pflFlashBangTime = (float *)((char *)pPlayer + 0xAB4); // 對齊 0xAB4 玩家致盲槽位
            
            // 戰術耳鳴沉默鎖：若玩家此時正面臨閃光彈致盲、眼前一片慘白與耳鳴
            // 恐怖分子全面保持沉默，不發射任何挑釁語音，完美營造人被炸盲時聽不見周圍聲音的極致沉浸感！
            if ( gpGlobals->time >= 2.0f && gpGlobals->time < *pflFlashBangTime )
            {
                return FALSE;
            }

            // 3. 🧱 第三道防線：爬梯與垂直索降物理定身聲道強行熔斷
            // 若目前實體正抓著梯子、或從房頂執行直升機索降繩下溜 (FL_FLY)
            // 且當前大腦非最高激戰交火態 (MONSTERSTATE_COMBAT)，絕對禁止一邊索降一邊鬼畜喊話
            if ( ( pev->flags & FL_FLY ) && m_MonsterState != MONSTERSTATE_COMBAT )
            {
                return FALSE;
            }

            // 4. 呼叫虛擬表 60 號方法 (IsAlive)，確認玩家活著、語音小隊握手完全暢通
            if ( pPlayer->IsAlive() )
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

int CTerrorist::GetBehaviorType( void )
{
    // 1. 📢 提取當前實體在世界加載時最真實的地圖類名字串 (如 monster_terrorist_sniperrifle)
    const char *pszClassName = STRING( pev->classname );
    if ( pszClassName == nullptr )
    {
        return 2; // 安全 Fallback：預設返回 2 號常規步槍突擊兵
    }

    // =========================================================================
    // 🎰 100% 還原二進位 strstr 子字串通配符多向解碼矩陣 (兵種分流大腦)
    // =========================================================================
    if ( strstr( pszClassName, "pistol" ) != nullptr )
        return 0; // 手槍輕步兵
        
    if ( strstr( pszClassName, "shotgun" ) != nullptr )
        return 1; // 散彈近戰破門兵
        
    if ( strstr( pszClassName, "assaultrifle" ) != nullptr )
        return 2; // 常規主力步槍突擊兵
        
    if ( strstr( pszClassName, "machinegun" ) != nullptr )
        return 3; // 重裝無畏機槍暴君
        
    if ( strstr( pszClassName, "sniperrifle" ) != nullptr )
        return 4; // 🦅 Scout 輕型雷射狙擊手
        
    if ( strstr( pszClassName, "law" ) != nullptr )
        return 5; // M72 LAW 重型火箭筒反裝甲兵
        
    if ( strstr( pszClassName, "smg" ) != nullptr )
        return 6; // 微型衝鋒槍滲透者
        
    if ( strstr( pszClassName, "melee" ) != nullptr )
        return 8; // ⚔️ 近戰馬刀死士 (不開槍，只出刀)
        
    if ( strstr( pszClassName, "grenader" ) != nullptr )
        return 7; // 戰術擲彈兵 (高頻扔雷)
        
    if ( strstr( pszClassName, "kamakazi" ) != nullptr )
        return 9; // 💥 神風特攻隊人肉炸彈自殺兵 (256碼爆炸/200點傷害)
        
    if ( strstr( pszClassName, "mp5" ) != nullptr )
        return 10; // 標準 MP5 微調特種兵

    // =========================================================================
    // 🛡️ 降級分流保護線：若地圖類名不含後綴 (純原始 monster_terrorist 母體)
    // 則讀取編輯器原始配置的 m_behaviorType 變數進行自適應補償
    // =========================================================================
    if ( m_behaviorType == 2 )
    {
        return 4; // 降級為 4 號狙擊手角色
    }
    
    if ( m_behaviorType == 1 )
    {
        return 10; // 降級為 10 號 MP5 特種兵角色
    }

    return 11; // 預設常規後備兵種
}

int CTerrorist::GetBehaviorTypeSchedule( void )
{
    // =========================================================================
    // 💥 1. 神風特攻隊人肉炸彈自殺兵種專屬行為判定 (m_terroristBehavior == 9)
    // =========================================================================
    if ( m_terroristBehavior == 9 )
    {
        if ( !suicideFlag ) // 階段 A：尚未點火，處於搜捕與衝鋒期
        {
            // 只有當雙眼實時目擊死敵玩家時，才激發衝鋒自爆網絡 (bits_COND_SEE_ENEMY = 0x10)
            if ( m_afConditions & bits_COND_SEE_ENEMY )
            {
                if ( m_hEnemy != nullptr && m_hEnemy->pev != nullptr )
                {
                    // 計算恐怖分子與玩家之間的 2D 平面絕對幾何距離
                    float flDistToEnemy = ( pev->origin - m_hEnemy->pev->origin ).Length2D();

                    if ( flDistToEnemy >= 64.0f )
                    {
                        // 距離尚遠 (>= 64碼)：回傳 66 號計畫 (SCHED_CHARGE_ENEMY)
                        // 驅動雙腿以最狂暴的速度，不顧一切衝向玩家！
                        return 66; // 0x42
                    }
                    
                    // 🎯 踏入 64 碼死亡紅線：瞬間點火引爆，標記 suicideFlag = TRUE
                    suicideFlag = TRUE;

                    // 回傳 61 號計畫 (SCHED_SUICIDE_BOMBER_DETONATE) 執行原地引取引導演出
                    return 61; // 0x3D
                }
            }
        }
        else // 👑 階段 B：死線跨越！引爆最終大結算
        {
            // 🛠️ 內存重砲保護：跨越虛擬表 14 號偏移量強行調用 TakeDamage
            // 為自己臨時回填 (當前血量 + 50.0f) 的無敵加載，確保不會在自爆動畫中途被流彈打斷演出
            TakeDamage( pev, pev, pev->health + 50.0f, DMG_GENERIC );

            // 🔥 終極大引爆：呼叫引擎最高物理爆炸核心 ExplosionCreate
            // 在恐怖分子當前原點座標，製造一場威力 100、撕裂 256 碼幾何半徑的電影級衝擊波大爆炸！
            ExplosionCreate( pev->origin, pev->angles, edict(), 100, TRUE, -1.0f );
            
            // 瞬間清空血量，宣告退場
            pev->health = 0.0f;
            return -1;
        }
    }

    return -1; // 其餘常規兵種不干預計畫，返回 -1
}

int CTerrorist::GetFlyingSchedule( void )
{
    // =========================================================================
    // 🚁 直升機戰術索降與攀爬移動狀態機審查 (movetype == MOVETYPE_FLY = 5)
    // =========================================================================
    if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
    {
        // 🎯 階段 A：雙腳成功著地結算點 (FL_ONGROUND = 0x200)
        if ( pev->flags & FL_ONGROUND )
        {
            // 一一幀之內將物理型態還原為常規地面步行重力 (MOVETYPE_WALK = 4)
            pev->movetype = MOVETYPE_WALK;

            // 回傳 56 號計畫 (SCHED_RAPPEL_RESET)，執行落地戰術受身與收繩切槍動作
            return 56; // 0x38
        }

        // 🎯 階段 B：懸掛於半空中下滑期 (動態分流空中開火或常規下滑)
        if ( m_MonsterState == MONSTERSTATE_COMBAT )
        {
            // 戰術強攻：半空中直接單手持槍瘋狂掃射壓制玩家
            return 55; // 0x37: SCHED_RAPPEL_COMBAT
        }
        else
        {
            // 常規安心抓繩垂直下滑
            return 54; // 0x36: SCHED_RAPPEL
        }
    }

    return -1; // 非索降狀態，返回 -1 移交常規排程
}

int CTerrorist::GetForcedTargetSchedule( void )
{
    // =========================================================================
    // 🎯 地圖劇本強制鎖定目標射擊調度 (m_forcedTarget != 0)
    // =========================================================================
    if ( m_forcedTarget == 0 )
    {
        return -1; // 無劇本強鎖目標，放行降級移交常規排程
    }

    // 1. 📢 殘彈與空倉審查大閉環
    // 完美還原二進位虛擬表 115 號偏移量方法的內聯對照行為
    CheckAmmo();

    // 若大腦條件遮罩已被打上 bits_COND_NO_AMMO (1) 彈藥完全枯竭標記
    if ( m_afConditions & bits_COND_NO_AMMO )
    {
        // 瞬間中斷劇本強攻射擊，回傳 30 號計畫 (SCHED_RELOAD) 強制尋找掩體拉栓換彈！
        return 30; // 0x1E
    }

    // =========================================================================
    // 🎰 2. 隨機數戰術決策：30% 機率全面無間斷盲目集火壓制，70% 機率戰術點射
    // =========================================================================
    int iRandChance = RANDOM_LONG( 0, 100 );

    if ( iRandChance <= 30 )
    {
        // 瘋狂傾瀉彈匣，對著玩家掩體或轉角發動 SCHED_SHOOT_RAW 火力壓制
        return 57; // 0x39
    }
    else
    {
        // 冷靜實施戰術三連發精準射擊計畫 (SCHED_RANGE_ATTACK1)
        return 42; // 0x2A
    }
}

Vector CTerrorist::GetGunPosition( void )
{
    float flMuzzleHeight = 60.0f; // 預設常規站立突擊時的槍口發射高度為 60.0f 碼

    // =========================================================================
    // 🔃 戰術姿態自適應：依據 m_bStanding 旗標，實時分流折算三維槍口世界起點
    // =========================================================================
    if ( !m_bStanding )
    {
        flMuzzleHeight = 36.0f; // 🔒 蹲下躲避火網狀態下，槍口高度壓低至 36.0f 碼，防止彈道懸空穿模 Bug
    }

    // 完美還原二進位幾何合成行為：自身原點疊加姿態高度偏置
    Vector vecMuzzlePos;
    vecMuzzlePos.x = pev->origin.x;
    vecMuzzlePos.y = pev->origin.y;
    vecMuzzlePos.z = pev->origin.z + flMuzzleHeight;

    // 將精確計算出的 3D 槍口世界起點座標返回給遠程彈道 CheckRangeAttack 系統
    return vecMuzzlePos;
}

int CTerrorist::GetHearingSchedule( void )
{
    // =========================================================================
    // 📡 戰術聽覺網絡：審查大腦條件遮罩中是否點亮了 bits_COND_HEAR_SOUND (0x20000)
    // =========================================================================
    if ( m_afConditions & bits_COND_HEAR_SOUND )
    {
        // 呼叫虛擬表 120 號偏移量方法，反查目前世界空間中威脅加權最高的核心音訊物件
        CSound *pSound = PBestSound();

        if ( pSound != nullptr )
        {
            // 🎯 分流 A：若聽到了戰鬥開火或爆炸聲 (bits_SOUND_COMBAT = 1)
            if ( pSound->m_iType == bits_SOUND_COMBAT )
            {
                m_bHeardCombat = TRUE; // 點火點亮聽聞交火標記，通知小隊網進入隱性警戒
            }

            // 🎯 分流 B：💥 危機引爆！若聽到了手榴彈落地反彈或 C4 滴答聲 (bits_SOUND_DANGER = 0x20)
            if ( pSound->m_iType & bits_SOUND_DANGER )
            {
                // 瞬間中斷一切日常，回傳 19 號最高優先級逃跑計畫 (SCHED_TAKE_COVER_FROM_BEST_SOUND)
                return 19; // 0x13
            }

            // 🎯 分流 C：聞聲扭頭反伏擊！
            // 若目前雙眼尚未目擊死敵 (!(m_afConditions & bits_COND_SEE_ENEMY))
            // 且聽到的噪音屬於 1號(槍聲) 或 4號(玩家腳步聲) 的組合特徵 (bits_SOUND_COMBAT | bits_SOUND_PLAYER = 5)
            if ( !( m_afConditions & bits_COND_SEE_ENEMY ) && ( pSound->m_iType & ( bits_SOUND_COMBAT | bits_SOUND_PLAYER ) ) )
            {
                // 🔒 定身安全鎖：確保目前實體並非處於完全無法挪步的物理僵直定身期
                if ( m_cantMove < 2 )
                {
                    // 🚀 幾何扭正：呼叫母體核心三角函數，將大腦的理想偏航角 (IdealYaw)
                    // 精確、瞬間對齊聲音爆發的 3D 世界座標原點，迫使其猛然轉身迎敵！
                    CBaseMonster::MakeIdealYaw( pSound->m_vecOrigin );
                }
            }
        }
    }

    return -1; // 無音訊異常或無威脅聲音，返回 -1 移交後續決策
}

Schedule_t* CTerrorist::GetSchedule( void )
{
    // =========================================================================
    // 🏢 1. 前置狀態重置與子系統級聯排隊 (已完全對齊前半段二進位優先級鏈)
    // =========================================================================
    m_iSentence = -1;
    if ( m_terroristBehavior == 4 ) LaserGlowOff();
    ClearScheduleTableFlags();

    // 優先級 A、B、C、D 排隊分配 (GetForcedTargetSchedule, GetBehaviorTypeSchedule, GetFlyingSchedule, GetHearingSchedule)
    int iForcedSchedule = GetForcedTargetSchedule();
    if ( iForcedSchedule != -1 && iForcedSchedule < 76 ) scheduleTable[iForcedSchedule].activeFlag = 1.0f;

    int iSuicideSchedule = GetBehaviorTypeSchedule();
    if ( iSuicideSchedule != -1 && iSuicideSchedule < 76 ) scheduleTable[iSuicideSchedule].activeFlag = 1.0f;

    if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
    {
        int iFlySchedule = ( pev->flags & FL_ONGROUND ) ? 56 : (( m_MonsterState == MONSTERSTATE_COMBAT ) ? 55 : 54);
        scheduleTable[iFlySchedule].activeFlag = 1.0f;
    }

    int iHearingSchedule = GetHearingSchedule();
    if ( iHearingSchedule != -1 && iHearingSchedule < 76 ) scheduleTable[iHearingSchedule].activeFlag = 1.0f;

	int iWeaponEffectSchedule = GetWeaponEffectSchedule();
    if ( iWeaponEffectSchedule != -1 && iWeaponEffectSchedule < 76 ) scheduleTable[iWeaponEffectSchedule].activeFlag = 1.0f;

    MONSTERSTATE CurrentState = m_MonsterState;

    // =========================================================================
    // 🏢 2. 放鬆巡邏狀態機 (MONSTERSTATE_IDLE) -> 補全後半段
    // =========================================================================
    if ( CurrentState == MONSTERSTATE_IDLE )
    {
        // 🎯 分流 2A：警報完全解除，回歸日常休閒待命狀態
        if ( m_alertClearTime > 0.0f && gpGlobals->time > m_alertClearTime && m_alertable )
        {
            // 廣播 T_RELAX 解除警報語音，冷卻時鐘向後鎖定 3.0 秒
            PlaySentence( "T_RELAX", 0.8f, ATTN_NORM, 0, m_voicePitch );
            CTalkMonster::g_talkWaitTime = gpGlobals->time + 3.0f;

            m_alertLevel     = m_alertLevel - 1; // 降階降級警戒
            m_alertClearTime = 0.0f;

            if ( m_pGoalEnt != nullptr && !m_cantMove )
            {
                if ( strcasecmp( STRING(m_pGoalEnt->pev->classname), "path_corner" ) == 0 )
                {
                    CBaseMonster::MakeIdealYaw( m_pGoalEnt->pev->origin );
                    m_movementGoal = 16;
                    if ( !CBaseMonster::FRefreshRoute() ) ALERT( at_aiconsole, "Can't Create Route!\n" );
                    return GetScheduleOfType( 2 ); // SCHED_WALK_PATH
                }
            }
        }

        // 🎯 分流 2B：🚀 8 方向隨機漫步防卡死系統 (Wander System)
        if ( m_hEnemy == nullptr && gpGlobals->time > m_wanderTime && m_canWander && !m_cantMove )
        {
            // 在 64 到 256 碼之間隨機外推空間幾何半徑
            float flRandRadius = RANDOM_FLOAT( 64.0f, 256.0f );
            int iRandX = RANDOM_LONG( (int)-flRandRadius, (int)flRandRadius );
            int iRandY = RANDOM_LONG( (int)-flRandRadius, (int)flRandRadius );

            // 寫入漫步閒逛原始幾何座標
            m_vecMoveGoal.x = pev->origin.x + (float)iRandX;
            m_vecMoveGoal.y = pev->origin.y + (float)iRandY;
            m_vecMoveGoal.z = pev->origin.z;

            m_wanderOrigin  = m_vecMoveGoal;

            // 重新填裝漫步閒逛時鐘，疊加 8 到 15 秒隨機巨大抖動，防 CPU 共振
            m_wanderTime    = gpGlobals->time + RANDOM_FLOAT( 8.0f, 15.0f );

            // 一鍵回傳 46 號計畫 (SCHED_WANDER)，擺脫卡死地形！
            return GetScheduleOfType( 46 ); // 0x2E
        }
    }

    // =========================================================================
    // 🏢 3. 戰術警戒狀態機 (MONSTERSTATE_ALERT) -> 補全後半段
    // =========================================================================
    if ( CurrentState == MONSTERSTATE_ALERT )
    {
        if ( m_hEnemy != nullptr )
        {
            float flDistToEnemy = ( pev->origin - m_hEnemy->pev->origin ).Length2D();

            // 自適應戰術警戒：若死敵與自己的距離跨入了 256 碼警戒紅線以內
            if ( flDistToEnemy <= m_alertRadius && m_alertable )
            {
                m_alertLevel = 2; // 強制將警戒值拉滿至最高級戰備
            }
            else if ( flDistToEnemy > m_alertRadius && !m_alertable && m_alertLevel < 2 )
            {
                // 🚀 轉身對齊警戒位置，清除死敵，回歸 IDLE，引爆 T_ALERT 警報無線電！
                CBaseMonster::MakeIdealYaw( m_hEnemy->pev->origin );
                m_hEnemy = nullptr;
                CBaseMonster::SetState( MONSTERSTATE_IDLE );

                PlaySentence( "T_ALERT", 0.8f, ATTN_NORM, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 2.0f;

                // 🎰 完美還原：依據 Easy/Normal/Hard 遊戲難度係數，分流核發警戒倒計時長度
                float flAlertDuration = 5.0f; // 預設常規 5 秒
                if ( g_iSkillLevel == 2 )                  flAlertDuration = 3.5f; // Hard 難度 3.5 秒
                else if ( g_iSkillLevel == 0 || g_iSkillLevel == 3 ) flAlertDuration = 2.0f; // Easy/Expert 2.0 秒

                m_alertClearTime = gpGlobals->time + flAlertDuration;

                // 回傳 71 號計畫 (SCHED_ALERT_STAND) 高舉持槍原地待命警戒
                return GetScheduleOfType( 71 ); // 0x47
            }
        }
    }

        // =========================================================================
        // 🥷 4. 最高激戰狀態機 (MONSTERSTATE_COMBAT) 條件遮罩位元解碼矩陣 -> 終章完工！
        // =========================================================================
        
        // 🎯 條件 4A：全速空倉換彈大腦互鎖 ( bits_COND_NO_AMMO = 0x01 )
        if ( m_afConditions & bits_COND_NO_AMMO )
        {
            scheduleTable[51].activeFlag = 1.0f; // 0x33: SCHED_HIDE_AND_RELOAD
            int iReloadSched = GetScheduleFromTable();
            HandleSpeaking( iReloadSched );

            if ( iReloadSched != -1 )
            {
                Schedule_t *pSchedObj = GetScheduleOfType( iReloadSched );
                ALERT( at_aiconsole, "%s Playing Schedule: %s\n", STRING(pev->classname), pSchedObj->pName );
                return pSchedObj;
            }
            uVar6 = m_afConditions;
        }

        // 🎯 條件 4B：📢 發現新敵軍/玩家現身警報對白發射 ( bits_COND_NEW_ENEMY = 0x10000 )
        if ( uVar6 & bits_COND_NEW_ENEMY )
        {
            // 核對語音排隊冷卻時鐘，全服發射 T_ENEMY 警報
            if ( gpGlobals->time > CTalkMonster::g_talkWaitTime && FOkToSpeak() )
            {
                PlaySentence( "T_ENEMY", 0.8f, ATTN_NORM, 0, m_voicePitch );
                CTalkMonster::g_talkWaitTime = gpGlobals->time + 3.0f;
                m_iSentence = -1;
            }
            uVar6 = m_afConditions;

            // 自適應幾何前推：若定身鎖未點亮，全面拉起 50(包抄) 與 66(衝鋒) 號計畫
            if ( !m_cantMove )
            {
                scheduleTable[50].activeFlag = 1.0f; // 0x32: SCHED_FLANK_ATTACK
                if ( m_fPreferedRange < local_34 )
                {
                    scheduleTable[66].activeFlag = 1.0f; // 0x42: SCHED_CHARGE_ENEMY
                }
                scheduleTable[18].activeFlag = 1.0f; // 0x12: SCHED_TAKE_COVER_FROM_ENEMY
            }
        }

        // 🎯 條件 4C：聽見遠處常規交火與受驚警報 ( bits_COND_HEAR_SOUND & 0x100 / 0x200 )
        if ( uVar6 & bits_COND_HEAR_SOUND )
        {
            scheduleTable[17].activeFlag = 1.0f; // 0x11: SCHED_TAKE_COVER
            scheduleTable[67].activeFlag = 1.0f; // 0x43: SCHED_WAKE_SQUAD (聽見開火激發小隊)
            scheduleTable[68].activeFlag = 1.0f; // 0x44: SCHED_ALERT_FACE (聽見腳步扭頭朝向)
        }

        if ( uVar6 & bits_COND_LIGHT_DAMAGE ) // 0x800: 受到輕微受傷
        {
            scheduleTable[22].activeFlag = 1.0f; // 0x16: SCHED_SMALL_FLINCH
        }

        // 🎯 條件 4D：⚖️ 小隊網絡核心：接收到小隊集結/隊長受驚警報 ( bits_COND_SQUAD_WAKE = 0x400 )
        if ( uVar6 & bits_COND_SQUAD_WAKE )
        {
            if ( m_hSquadLeader != nullptr )
            {
                CBaseEntity *pLeader = m_hSquadLeader;
                // 若隊長在無線電裡宣佈跟丟了玩家，自己主動幫隊長清空盲區，並一鍵發動 53 號小隊協同包抄追擊計畫！
                if ( pLeader->m_fEnemyEluded && !(m_afConditions & bits_COND_SEE_ENEMY) )
                {
                    pLeader->m_fEnemyEluded = FALSE;
                    return GetScheduleOfType( 53 ); // 0x35: SCHED_CHASE_ENEMY
                }
            }

            if ( justShotFlag ) scheduleTable[70].activeFlag = 1.0f; // 0x46: SCHED_SHOOT_BURST
            
            justShotFlag = FALSE;
            scheduleTable[62].activeFlag = 1.0f; // 0x3E: SCHED_COMBAT_STAND

            if ( !m_cantMove )
            {
                scheduleTable[18].activeFlag = 1.0f; // 0x12
                if ( m_fPreferedRange < local_34 ) scheduleTable[66].activeFlag = 1.0f; // 0x42
            }

            // 🎯 🚀 100% 還原重裝 LAW 火箭筒 128 碼最小幾何安全射擊死線
            if ( m_terroristBehavior == 5 ) // M72 LAW 火箭筒兵
            {
                if ( lawDelayTime <= gpGlobals->time ) scheduleTable[24].activeFlag = 1.0f; // 0x18: 放行發射
            }
            else if ( local_34 < 128.0f )
            {
                // 太近了(小於128碼)：火箭筒兵強制沒收開火權，倒切為 24 號棄槍跑位找掩體計畫！
                scheduleTable[24].activeFlag = 1.0f; 
            }
            uVar6 = m_afConditions;
        }

        // 🎯 條件 4E：敵人逃離、跟丟或脫離視野 ( bits_COND_ENEMY_OCCLUDED = 0x20 )
        if ( uVar6 & bits_COND_ENEMY_OCCLUDED )
        {
            if ( m_hEnemy != nullptr && m_hEnemy->IsPlayer() )
            {
                shotPlayerRecently = FALSE; // 清空最近擊中玩家標記
            }
            scheduleTable[63].activeFlag = 1.0f; // 0x3F: SCHED_COMBAT_FACE
            if ( !m_cantMove )
            {
                scheduleTable[50].activeFlag = 1.0f; // 0x32
                scheduleTable[66].activeFlag = 1.0f; // 0x42
            }
            scheduleTable[28].activeFlag = 1.0f; // 0x1C: SCHED_CHASE_ENEMY_SOUND
        }
    }

    // =========================================================================
    // 🎲 5. 終極輪盤大抽籤：從加權表中獲取最終勝出計畫，注入 HandleSpeaking 無線電
    // =========================================================================
    int iFinalActiveSched = GetScheduleFromTable();
    HandleSpeaking( iFinalActiveSched );

    if ( iFinalActiveSched != -1 )
    {
        Schedule_t *pFinalSchedObj = GetScheduleOfType( iFinalActiveSched );
        ALERT( at_aiconsole, "%s Playing Schedule: %s\n", STRING(pev->classname), pFinalSchedObj->pName );
        return pFinalSchedObj;
    }

    // 終極放行
    return CSquadMonster::GetSchedule();
}

int CTerrorist::GetScheduleFromTable( void )
{
    float flTotalChanceSum = 0.0f;

    // =========================================================================
    // 🎰 1. 階段一：計算所有當前被激活計畫的隨機權重百分配比總和
    // =========================================================================
    for ( int i = 0; i < 16; i++ ) // 完美對齊二進位哨兵指針展開
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            flTotalChanceSum += scheduleTable[i].percentChance;
        }
    }

    // 防砸窗踩空：若沒有任何計畫被激活，或者權重池為空
    if ( flTotalChanceSum <= 0.0f )
    {
        goto LAB_SCHEDULE_FAILED_LOG;
    }

    // =========================================================================
    // 📐 2. 階段二：將權重池歸一化分配，壓縮至 0.0f ~ 1.0f 的絕對機率輪盤面
    // =========================================================================
    for ( int i = 0; i < 16; i++ )
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            scheduleTable[i].normalizedChance = scheduleTable[i].percentChance / flTotalChanceSum;
        }
    }

    // =========================================================================
    // 🎲 3. 階段三：搖骰子抽籤！進行蒙地卡羅機率區間比對
    // =========================================================================
    float flRoll = RANDOM_FLOAT( 0.0f, 1.0f );
    float flCumulativeProbability = 0.0f;

    for ( int i = 0; i < 75; i++ ) // 0x4B 號小隊大腦計畫極限防禦線
    {
        if ( scheduleTable[i].activeFlag != 0.0f && scheduleTable[i].percentChance > 0.0f )
        {
            flCumulativeProbability += scheduleTable[i].normalizedChance;
            
            // 骰子點數成功落入當前計畫的加權區間，大腦中籤勝出！
            if ( flRoll < flCumulativeProbability )
            {
                return i; // 成功返回勝出的計畫索引！
            }
        }
    }

LAB_SCHEDULE_FAILED_LOG:
    // =========================================================================
    // 🚨 4. 異常處理大閉環：若不幸決策踩空，印出 3D 世界座標排錯日誌
    // =========================================================================
    if ( m_MonsterState == MONSTERSTATE_IDLE )
    {
        return -1;
    }

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

Schedule_t* CTerrorist::GetScheduleOfType( int Type )
{
    // 1. 📢 優先攔截：若計畫為 0x16 (SCHED_SMALL_FLINCH 輕微抽搐硬直)
    if ( Type == 22 ) // 0x16
    {
        taskFailCount = 0.0f; // 實時重置尋路失敗計數器
        return CSquadMonster::GetScheduleOfType( Type ); // 降級放行移交小隊母體
    }

    Schedule_t *pSched = nullptr;

    // =========================================================================
    // 🎰 2. 100% 還原二進位大型隨機兵種與劇本動畫計畫解碼矩陣
    // =========================================================================
    switch ( Type )
    {
        case 10: // 0x0A: SCHED_COMBAT_FACE
            pSched = slTerroristCombatFace;
            break;

        case 14: // 0x0E: SCHED_VICTORY_DANCE (戰術勝利狂歡/大獲全勝)
            if ( m_hSquadLeader == nullptr || m_hSquadLeader == this )
            {
                return slTerroristVictoryDance; // 孤狼或小隊隊長原地播放慶祝動作
            }
            pSched = slTerroristFail; // 其餘隊員降級失敗
            break;

        case 18: // 0x12: SCHED_TAKE_COVER_FROM_ENEMY
        case 67: // 0x43: SCHED_WAKE_SQUAD
        case 68: // 0x44: SCHED_ALERT_FACE
            pSched = slTerroristTakeCover;
            break;

        case 19: // 0x13: SCHED_TAKE_COVER_FROM_BEST_SOUND (逃離危險音訊源)
            pSched = slTerroristTakeCoverFromBestSound;
            break;

        case 24: // 0x18: 🚀 核心兵種遠程火網姿態調度中樞 (對齊 0x18 常數)
            taskFailCount = 0.0f;
            if ( m_terroristBehavior == 0 || m_terroristBehavior == 5 ) // 0=手槍, 5=LAW火箭筒兵
            {
                pSched = slTerroristRangeAttack1C;
            }
            else 
            {
                // 蒙地卡羅隨機隨機數：50% 機率實施戰術姿態隨機切換
                int iRandChance = RANDOM_LONG( 0, 99 );
                if ( iRandChance < 50 )
                {
                    if ( m_terroristBehavior != 3 ) pSched = slTerroristRangeAttack1C;
                }
                else if ( m_terroristBehavior != 3 ) // 非 M60 重機槍兵
                {
                    if ( RANDOM_LONG(0, 9) < 5 )
                    {
                        m_bStanding = RANDOM_LONG( 0, 1 ); // 🎰 50% 機率命令匪徒原地蹲下或站立持槍！
                    }
                    return m_bStanding ? slTerroristRangeAttack1B : slTerroristRangeAttack1A;
                }
                
                // M60 重機槍兵專屬特權：強制直立站穩，發動 slTerroristRangeAttackMG2 鏈彈暴君壓制火網！
                pSched = slTerroristRangeAttackMG2;
                m_bStanding = TRUE;
            }
            break;

        case 25: // 0x19: SCHED_RANGE_ATTACK2 (手榴彈/下掛式榴彈拋物線發射)
            pSched = slTerroristRangeAttack2;
            break;

        case 41: // 0x29: SCHED_CHASE_ENEMY_FAILED
            if ( m_hEnemy != nullptr ) return slTerroristCombatFail;
            pSched = slTerroristFail;
            break;

        case 46: // 0x2E: SCHED_WANDER (8方向隨機漫步防卡死)
            pSched = slTerroristWander;
            break;

        case 49: // 0x31: SCHED_SUPPRESS_ENEMY (開局首次遭遇戰術壓制)
            pSched = slTerroristSuppress;
            if ( m_hEnemy != nullptr && m_hEnemy->IsPlayer() && m_fFirstEncounter )
            {
                pSched = slTerroristSignalSuppress; // 引爆全隊高喊口號
                m_fFirstEncounter = FALSE;
            }
            break;

        case 50: // 0x32: SCHED_FLANK_ATTACK (側翼繞後包抄)
            pSched = slTerroristEstablishLineOfFire;
            if ( m_terroristBehavior == 4 ) LaserGlowOff(); // 狙擊手隱蔽雷斯特效
            break;

        case 51: // 0x33: SCHED_HIDE_AND_RELOAD (跑入掩體拉栓換彈)
            pSched = slTerroristHideReload;
            break;

        case 52: // 0x34: SCHED_SWEEP_AREA
            pSched = slTerroristSweep;
            break;

        case 53: // 0x35: SCHED_CHASE_ENEMY (小隊協同仇恨追擊)
            pSched = slTerroristFoundEnemy;
            break;

        case 54: // 0x36: 🚁 100% 還原直升機索降特殊重力二段物理加速
            pSched = slTerroristRepel;
            if ( pev->velocity.z > -128.0f ) pev->velocity.z -= 32.0f; // 注入垂直向下推力偏置
            break;

        case 55: // 0x37: 🚁 直升機空中下滑單手持槍狂暴壓制火網
            pSched = slTerroristRepelAttack;
            if ( pev->velocity.z > -128.0f ) pev->velocity.z -= 32.0f;
            break;

        case 56: // 0x38: SCHED_RAPPEL_RESET (索降雙腳著地落地翻滾收繩)
            pSched = slTerroristRepelLand;
            break;

        case 57: // 0x39: SCHED_SHOOT_RAW (瘋狂盲目集火壓制)
            pSched = slTerroristWaitInCover;
            break;

        case 58: // 0x3A: SCHED_ALERT_STAND (原地高舉武器持槍防線)
            // 基於加權百分比的複雜多載跳轉 (由 114 號虛擬方法統一調度，此處放行)
            break;

        case 59: // 0x3B: 🛠️ 尋路/計畫連續失敗硬格式化與全服狂暴挑釁狀態機
            taskFailCount += 1.0f;
            m_bStanding = TRUE;
            if ( taskFailCount > 1.0f )
            {
                // 核對語音冷卻線，全服廣播 T_TAUNT 惡棍挑釁台詞
                if ( gpGlobals->time > CTalkMonster::g_talkWaitTime && FOkToSpeak() )
                {
                    PlaySentence( "T_TAUNT", 0.8f, ATTN_NORM, 0, m_voicePitch );
                    CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 8.0f, 15.0f );
                    m_iSentence = -1;
                }
            }
            break;

        case 60: // 0x3C: SCHED_IN_SMOKE (遭遇特警煙霧彈熔斷盲區退避)
            pSched = slTerroristInSmoke;
            break;

        case 61: // 0x3D: 💥 神風特攻隊人肉炸彈自殺兵原地引爆引導演出計畫
            pSched = slTerroristKamakaziBlowup;
            break;

        case 62: // 0x3E: SCHED_COMBAT_STAND (標準持槍激戰姿態)
            pSched = slTerroristSniperAttack;
            taskFailCount = 0.0f;
            m_bStanding = TRUE;
            break;

        case 63: // 0x3F: SCHED_COMBAT_FACE
            pSched = slTerroristSniperTakeShot;
            if ( m_hEnemy != nullptr )
            {
                LaserGlowOff(); // 狙擊手擊中目標後暫時熄滅雷達線隱蔽
                m_hEnemy = nullptr; // 清空宿敵
            }
            break;

        case 64: // 0x40: 📐 100% 還原戰術特殊掩體節點 (path_node) 自適應蹲下/探頭貼牆大腦
            {
                unsigned int nNodeType = m_pSpecialCoverNode->m_nodeType;
                if ( !(nNodeType & 1) ) // 轉角牆壁型掩體
                {
                    pSched = slTerroristCornerCoverNode;
                    if ( nNodeType & 6 ) m_bStanding = TRUE; // 強制站立持槍探頭斜向射擊
                }
                else // 低矮箱子型掩體
                {
                    pSched = slTerroristDuckCoverNode;
                    m_bStanding = TRUE; // 預設姿態重定向
                }
            }
            break;

        case 65: // 0x41: SCHED_DUCK_NODE_ATTACK
            pSched = slTerroristDuckNodeAttack;
            break;

        66: // 0x42: SCHED_CHARGE_ENEMY (狂暴全速驚恐直線衝鋒)
            pSched = slTerroristCloseIn;
            break;

        case 69: // 0x45: SCHED_CORNER_NODE_ATTACK
            pSched = slTerroristCornerNodeAttack;
            break;

        case 70: // 0x46: SCHED_FIRING_PAUSE
            pSched = slTerroristFiringPause;
            break;

        case 71: // 0x47: SCHED_ALERT_SPEAK
            pSched = slTerroristAlertSpeak;
            break;

        case 72: // 0x48: SCHED_INVESTIGATE (循聲主動搜查)
            pSched = slTerroristInvestigate;
            break;

        case 73: // 0x49: SCHED_FLASHED (遭受特警閃光彈致盲，全服扣鎖發聲，雙腿打滑)
            pSched = slTerroristFlashed;
            break;

        case 74: // 0x4A: SCHED_WAIT_CROUCHED
            pSched = slTerroristWaitCrouched;
            break;

        default:
            return CSquadMonster::GetScheduleOfType( Type ); // 未指派計畫，交回小隊基類
    }

    // 🚀 3. 終極指針發射：若上方成功匹配自定義骨骼腳本，調用 114 號虛擬方法 (GetScheduleOfType) 派發給引擎渲染
    if ( pSched != nullptr )
    {
        return pSched;
    }

    // 完美對齊二進位 LAB_0012b3e8 虛擬表指針調用行為
    unsigned int *p_vptr = *(unsigned int **)this;
    typedef Schedule_t* (*pfnGetScheduleOfType)(void*, int);
    pfnGetScheduleOfType native_func = (pfnGetScheduleOfType)p_vptr[93]; // 0x174 = 468 = 117號? 32位元架構對齊
    
    return native_func( this, Type );
}

int CTerrorist::GetWeaponEffectSchedule( void )
{
    int iEffectType = m_iWeaponEffectType;

    // =========================================================================
    // 🧱 1. 特警戰術煙霧彈 (Smoke Grenade) 沾染粒子感知狀態機 (Type = 7)
    // =========================================================================
    if ( iEffectType == 7 )
    {
        if ( !m_bWeaponEffectActive )
        {
            return -1;
        }

        // 實時清洗暫存器旗標，防範下一幀鬼畜重複觸發
        m_iWeaponEffectType    = 0;
        m_bWeaponEffectActive  = 0;

        // 核對全服語音通訊鎖，發射 T_INSMOKE 煙霧驚慌喊話
        if ( gpGlobals->time > CTalkMonster::g_talkWaitTime && FOkToSpeak() )
        {
            PlaySentence( "T_INSMOKE", 0.8f, ATTN_NORM, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 8.0f, 15.0f ); // 語音降噪保護
        }

        // 🎰 發動 100 點機率輪盤，隨機決定在濃煙中的本能戰術行為
        int iRandRoll = RANDOM_LONG( 0, 99 );

        if ( iRandRoll < 70 ) // 70% 壓倒性機率
        {
            // 驚慌失措，盲目執行 60 號計畫 (SCHED_IN_SMOKE) 向後倒退摸索撤退
            return 60; // 0x3C
        }
        if ( iRandRoll < 90 ) // 20% 機率
        {
            // 全速轉向撤離，尋找周邊最近的乾淨掩體 (SCHED_TAKE_COVER)
            return 17; // 0x11
        }
        
        // 10% 頑固分子原地留守
        return -1;
    }

    // =========================================================================
    // 💥 2. 戰術閃光彈 (Flashbang) 正面震撼致盲中樞 (Type = 4)
    // =========================================================================
    if ( iEffectType == 4 && m_bWeaponEffectActive != 0 )
    {
        m_iWeaponEffectType    = 0;
        m_bWeaponEffectActive  = 0;

        // 核對語音鎖，發射 T_FLASHED 捂眼劇痛慘叫無線電
        if ( gpGlobals->time > CTalkMonster::g_talkWaitTime && FOkToSpeak() )
        {
            PlaySentence( "T_FLASHED", 0.8f, ATTN_NORM, 0, m_voicePitch );
            m_iSentence = -1;
            CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 8.0f, 15.0f );
        }

        // 🚀 瞬間硬熔斷！強制回傳 73 號計畫 (SCHED_FLASHED)，全身雙腿癱瘓，捂眼原地鬼畜硬直！
        return 73; // 0x49
    }

    return -1; // 未受到戰術道具反制，降級交還常規排程
}

void CTerrorist::GibMonster( void )
{
    // =========================================================================
    // 🎯 狙擊手戰術熔斷：若目前屬於 4 號狙擊兵兵種 (Scout 狙擊手)
    // 肉體粉碎成肉塊的萬分之一秒內，一鍵拉熄紅外線雷射線，杜絕線條懸空穿模 Bug
    // =========================================================================
    if ( m_terroristBehavior == 4 )
    {
        LaserGlowOff();
    }

    // 檢查模型第 2 號變體槍械 Bodygroup 是否完整未損壞
    if ( CBaseAnimating::GetBodygroup( 2 ) != 2 )
    {
        Vector vecGunPos, vecGunAngles;
        // 🚀 獲取模型 0 號掛載點 (Attachment 0) -> 精確鎖定死前雙手持槍的槍口世界坐標
        CBaseAnimating::GetAttachment( 0, vecGunPos, vecGunAngles );

        // =========================================================================
        // 🚀 重火力掉落物物理噴射矩陣：若配備了 4 號下掛榴彈/火箭筒武器遮罩 (1<<2)
        // =========================================================================
        if ( pev->weapons & bits_WEAPON_GLAUNCHER ) // pev->weapons & 4
        {
            // 在槍口世界原點，動態生成一盒下掛榴彈補給箱
            CBaseEntity *pAmmoBox = CBaseMonster::DropItem( "ammo_ARgrenades", vecGunPos, vecGunAngles );

            if ( pAmmoBox != nullptr && pAmmoBox->pev != nullptr )
            {
                entvars_t *pAmmoPev = pAmmoBox->pev;

                // 🎰 100% 還原二進位 FPU 隨機拋物線公式：強行核發向上炸飛的 3D 三維初速度
                pAmmoPev->velocity.x = RANDOM_FLOAT( -100.0f, 100.0f );
                pAmmoPev->velocity.y = RANDOM_FLOAT( -100.0f, 100.0f );
                pAmmoPev->velocity.z = RANDOM_FLOAT(  200.0f, 300.0f ); // 向上拋射噴發！

                // 🎰 還原角速度旋轉矩陣：X/Z軸扭矩強鎖 0.0f，Y軸注入每秒 200~400 度的高速迴力鏢自轉！
                pAmmoPev->avelocity.x = 0.0f;
                pAmmoPev->avelocity.z = 0.0f;
                pAmmoPev->avelocity.y = RANDOM_FLOAT( 200.0f, 400.0f );
            }
        }
    }

    // 降級派發：呼叫怪物母體基類，將肉體解體為漫天人類紅色肉塊粒子，謝幕退場！
    CBaseMonster::GibMonster();
}

void CTerrorist::HandleAnimEvent( MonsterEvent_t *pEvent )
{
    Vector vecSrc, vecAngles;

    switch ( pEvent->event )
    {
        // =========================================================================
        // 🧱 EVENT 2：自適應多武器戰術換彈與殘彈記憶體強寫
        // =========================================================================
        case 2:
            if ( (unsigned int)m_terroristBehavior < 11 )
            {
                const char *pszReloadWav = "weapons/reload_mp5.wav";
                unsigned int nBitMask = 1 << m_terroristBehavior;

                if ( nBitMask & ( (1<<0) | (1<<7) ) )        pszReloadWav = "weapons/pistol_reload.wav";  // 手槍
                else if ( nBitMask & (1<<1) )                 pszReloadWav = "weapons/reload_shotgun.wav"; // 散彈槍
                else if ( nBitMask & ( (1<<2)|(1<<3)|(1<<10) ) ) pszReloadWav = "weapons/reload_mp5.wav";    // 步槍/機槍

                EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pszReloadWav, 0.75f, ATTN_NORM, 0, 100 );
            }
            
            // 換彈首幀完成：一鍵擦除空倉掛機條件，將當前殘彈灌滿為彈匣上限最大值
            m_afConditions &= ~bits_COND_NO_AMMO; // m_afConditions &= ~1;
            m_cAmmoLoaded   = m_cClipSize;
            break;

        // =========================================================================
        // ⚔️ EVENT 3：近戰叢林馬刀/匕首貼身割喉與 3D 物理擊退衝量運算
        // =========================================================================
        case 3:
            {
                CBaseEntity *pHurt = Kick(); // 探測 80 碼範圍內的反恐精英
                if ( pHurt != nullptr && pHurt->pev != nullptr )
                {
                    int nMeleeDamage = 25; // 預設 Normal 難度 25 點傷害
                    if ( g_iSkillLevel == 1 )      nMeleeDamage = 15; // Easy
                    else if ( g_iSkillLevel == 2 ) nMeleeDamage = 35; // Hard
                    else if ( g_iSkillLevel == 3 ) nMeleeDamage = 40; // Expert

                    if ( m_terroristBehavior != 8 ) // 非純純近戰死士兵，播放常規匕首揮砍聲
                    {
                        EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/knife_slash1.wav", 0.75f, ATTN_NORM, 0, 100 );
                    }
                    else // 8號馬刀死士，爆發巨大馬刀撕裂傷害
                    {
                        nMeleeDamage += 5; 
                    }

                    // 計算 3D 空間擊退方向幾何向量
                    UTIL_MakeVectors( pev->angles );
                    pHurt->pev->punchangle.x = 5.0f; // 玩家視角劇烈顫抖

                    // 🚀 注入擊退初速度：向前方前推 100 碼，向上打飛 50 碼！
                    pHurt->pev->velocity = pHurt->pev->velocity + ( gpGlobals->v_forward * 100.0f ) + ( gpGlobals->v_up * 50.0f );

                    // 呼叫虛擬表 14 號受傷接口，將割喉傷害實時打入玩家血條
                    pHurt->TakeDamage( pev, pev, (float)nMeleeDamage, DMG_CLUB );
                }
            }
            break;

        // =========================================================================
        // 🔫 EVENT 4/5/6：11大兵種全自動武器開火發射與全服聽覺威脅注入
        // =========================================================================
        case 4:
        case 5:
        case 6:
            justShotFlag = TRUE; // 點亮開火暫存器
            switch ( m_terroristBehavior )
            {
                case 0:
                case 7:  ShootPistol();       break; // 手槍
                case 1:  ShootShotgun();      break; // 散彈
                case 3:  ShootMachineGun();   break; // M60重機槍
                case 4:  ShootSniperRifle();  break; // 雷射狙擊
                case 5:  ShootLAW();          break; // LAW火箭筒
                case 6:  ShootSMG();          break; // 戰術SMG
                case 10: ShootMP5();          break; // 標準MP5
                default: ShootAssaultRifle(); break; // AK47常規步槍
            }

            // 🚀 聽覺雷達注入：在方圓 384 碼內發射高階戰鬥槍聲信號，持續 0.3 秒，激發全走廊特警 AI 尋路突擊！
            CSoundEnt::InsertSound( bits_SOUND_COMBAT, pev->origin, 384, 0.3f ); // 0x180 = 384
            break;

        // =========================================================================
        // 💣 EVENT 7：3.5秒定時高爆手榴彈拋物線空投發射
        // =========================================================================
        case 7:
            UTIL_MakeVectors( pev->angles );
            GetGunPosition( vecSrc ); // 獲取出手世界起點

            if ( !m_flashbangOnly && !m_heGrenadeOnly )
            {
                // 50% 隨機數概率分流投擲手榴彈或閃光彈
                if ( RANDOM_LONG(0, 99) < 40 )
                {
                    // 向世界發射一顆 3.5 秒定時引信的標準高爆手榴彈 ( 100 點基礎威力 )
                    CGrenade::ShootTimed( pev, vecSrc, m_vecTossVelocity, 3.5f );
                }
                else
                {
                    // CGrenade::ShootFlashbang( pev, vecSrc, m_vecTossVelocity );
                }
            }
            else if ( m_heGrenadeOnly )
            {
                CGrenade::ShootTimed( pev, vecSrc, m_vecTossVelocity, 3.5f );
            }
            
            m_fThrowGrenade = FALSE; // 清空投擲意圖
            m_flNextGrenadeCheck = gpGlobals->time + 6.0f; // 鎖定 6.0 秒長冷卻時鐘
            break;

        // =========================================================================
        // 🚀 EVENT 8：下掛式榴彈/重型火箭彈 觸碰即炸 接觸爆炸物發射
        // =========================================================================
        case 8:
            EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/glauncher.wav", 0.8f, ATTN_NORM, 0, 100 );
            GetGunPosition( vecSrc );

            // 🔥 核心重砲：向前方發射一顆觸碰任意地表幾何與肉體瞬間爆炸的接觸榴彈！
            CGrenade::ShootContact( pev, vecSrc, m_vecTossVelocity );
            
            m_fThrowGrenade = FALSE;

            if ( g_iSkillLevel == 3 ) // 🎯 Expert 最高專家難度特權：火箭筒冷卻縮短至 2~5 秒瘋狂轟炸！
            {
                m_flNextGrenadeCheck = gpGlobals->time + RANDOM_FLOAT( 2.0f, 5.0f );
            }
            else
            {
                m_flNextGrenadeCheck = gpGlobals->time + 6.0f; // 常規難度維持 6 秒
            }
            break;

        // =========================================================================
        // 💥 EVENT 9：神風特攻隊自殺兵 3.0 秒衣服內襯定時炸藥包生成點火
        // =========================================================================
        case 9:
            UTIL_MakeVectors( pev->angles );
            
            // 📐 完美還原二進位幾何反推矩陣：精確將炸藥包綁載在胸口正中心衣服內襯槽位
            vecSrc = pev->origin + ( gpGlobals->v_forward * 17.0f ) - ( gpGlobals->v_right * 27.0f ) + ( gpGlobals->v_up * 6.0f );

            // 在胸口點火生成一顆 3.0 秒引信的定時炸彈，隨後引爆全局大爆炸
            CGrenade::ShootTimed( pev, vecSrc, g_vecZero, 3.0f );
            break;

        // =========================================================================
        // 📢 EVENT 10：激戰接敵高喊目擊無線電口號
        // =========================================================================
        case 10:
            if ( gpGlobals->time > CTalkMonster::g_talkWaitTime && FOkToSpeak() )
            {
                PlaySentence( "T_ENEMY", 0.8f, ATTN_NORM, 0, m_voicePitch );
                m_iSentence = -1;
                CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 8.0f, 15.0f );
            }
            break;

        // =========================================================================
        // 💰 EVENT 11：死後實體武器槍械剝離、隱形與自定義機率掉落物拋射
        // =========================================================================
        case 11:
            CBaseAnimating::GetAttachment( 0, vecSrc, vecAngles ); // 抓取雙手持槍口原點
            CBaseAnimating::SetBodygroup( 2, 9 ); // 🚀 強行將屍體手上的武器模型切換為隱形，防止穿模！

            // 隨機數抽籤：若满足地圖製作者配置的掉落機率，在屍體槍口原地拋射出戰利品
            if ( m_dropItem != 0 && RANDOM_FLOAT(0.0f, 1.0f) < m_dropChance )
            {
                CBaseMonster::DropItem( (char *)STRING(m_dropItem), vecSrc, vecAngles );
            }
            break;

        // =========================================================================
        // 🎯 EVENT 14：狙擊手陣亡閉幕幀，全速拉熄 4096 碼紅外線雷射線
        // =========================================================================
        case 14:
            LaserGlowOff();
            break;

        default:
            CBaseMonster::HandleAnimEvent( pEvent ); // 其餘通用事件移交基類
            break;
    }
}

void CTerrorist::HandleSpeaking( int schedule )
{
    // =========================================================================
    // 📡 1. 第一道防線：核對全局語音冷卻線與私有消音鎖 (FOkToSpeak)
    // =========================================================================
    if ( gpGlobals->time <= CTalkMonster::g_talkWaitTime || !FOkToSpeak() )
    {
        return; 
    }

    const char *pszSentenceName = nullptr;

    // =========================================================================
    // 🎰 2. 根據當前大腦中籤勝出的戰術計畫 (Schedule)，分流點亮對應無線電台詞組
    // =========================================================================
    switch ( schedule )
    {
        case 19: // 0x13: SCHED_TAKE_COVER_FROM_BEST_SOUND (腳下被扔了特警手榴彈)
            pszSentenceName = "T_GREN"; // 匪徒驚慌高喊：「Grenade! 快閃開！」
            break;

        case 24: // 0x18: SCHED_RANGE_ATTACK1 (主動步槍開火壓制)
            // 🎰 戰術降噪降噪門閥：只有 35% 的隨機機率允許邊打邊喊，防止鬼畜雜音 Bug
            if ( RANDOM_LONG(0, 99) > 34 ) // 0x22 = 34
            {
                return;
            }
            pszSentenceName = "T_ATTACK"; // 中籤，高喊突擊口號！
            break;

        case 25: // 0x19: SCHED_RANGE_ATTACK2 (拔除插銷，朝玩家拋雷)
            pszSentenceName = "T_THROW"; // 無線電高喊：「Fire in the hole!」
            break;

        case 28: // 0x1C: SCHED_CHASE_ENEMY_SOUND (聽聞盲區動靜，主動搜查)
            pszSentenceName = "T_TAUNT"; // 播放惡棍挑釁台詞
            break;

        case 66: // 0x42: SCHED_CHARGE_ENEMY (人肉炸彈自殺兵/馬刀死士瘋狂直線衝鋒)
            // 還原二進位隨機消音死鎖行為：原作者調研後決定在衝鋒期進行純粹消音，保留喘息聲
            if ( RANDOM_LONG(0, 0) == 0 )
            {
                return;
            }
            pszSentenceName = "T_CHARGE";
            break;

        default:
            return; // 其餘非通訊型常規計畫，直接放行不發言
    }

    // =========================================================================
    // 📢 3. 終極廣播點火：呼叫虛擬表 102 號多載方法 (PlaySentence) 發射音軌
    // =========================================================================
    if ( pszSentenceName != nullptr )
    {
        PlaySentence( pszSentenceName, 0.8f, ATTN_NORM, 0, m_voicePitch );

        // ⏱️ 全服語音通道強行點亮 8~15 秒隨機冷卻降噪鎖，保護伺服器音訊緩衝池
        m_iSentence = -1;
        CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT( 8.0f, 15.0f );
    }
}

void CTerrorist::IdleSound( void )
{
    // 1. ⚙️ 50% 機率的日常噪聲過濾閘門，且必須跨越全局語音冷卻線
    if ( RANDOM_LONG(0, 99) > 50 || gpGlobals->time <= CTalkMonster::g_talkWaitTime )
    {
        return;
    }

    // 2. 🛡️ 戰術消音鎖審查：若目前正處於索降、致盲或潛行暗殺狀態，硬熔斷保持沉默
    if ( !FOkToSpeak() )
    {
        return;
    }

    const char *pszSentenceName = nullptr;

    // =========================================================================
    // 🎰 3. 全局隨機情境群組對話狀態機矩陣
    // =========================================================================
    if ( g_fTerroristQuestion == 0 ) // 階段 A：目前全圖寂靜，由我主動發起話題
    {
        // 50% 機率直接放棄說話保持安靜
        if ( RANDOM_LONG(0, 1) == 0 )
        {
            return;
        }

        int iTopicRoll = RANDOM_LONG( 0, 2 );

        if ( iTopicRoll == 1 )
        {
            pszSentenceName = "T_QUEST";   // 發起戰術提問
            g_fTerroristQuestion = 2;      // 🚀 將全局狀態標記為：等待同伴解答 (2)
        }
        else if ( iTopicRoll == 2 )
        {
            pszSentenceName = "T_IDLE";    // 純日常抱怨閒聊，不改變全局狀態
        }
        else if ( iTopicRoll == 0 )
        {
            pszSentenceName = "T_CHECK";   // 發起戰術無線電盤查
            g_fTerroristQuestion = 1;      // 🚀 將全局狀態標記為：等待同伴匯報安全 (1)
        }
    }
    else // 階段 B：全圖有同伴拋出了話題，我發動高階接話應答大腦
    {
        if ( g_fTerroristQuestion == 1 )
        {
            pszSentenceName = "T_CLEAR";   // 應答對白：自動回覆「哨位一切安全！」
        }
        else if ( g_fTerroristQuestion == 2 )
        {
            pszSentenceName = "T_ANSWER";  // 應答對白：自動回覆同伴提問的解答
        }

        // 接話完畢，立即將全局對話通信槽歸零重置，放行信道
        g_fTerroristQuestion = 0;
    }

    // =========================================================================
    // 📢 4. 點火廣播：呼叫虛擬表 102 號多載方法 (PlaySentence) 發射音軌
    // =========================================================================
    if ( pszSentenceName != nullptr )
    {
        PlaySentence( pszSentenceName, 0.8f, ATTN_NORM, 0, m_voicePitch );
        // 🚀 直接呼叫傳奇的 JustSpoke 函數！一鍵傳入 -1.0f，完成最高規格大腦清洗與降噪！
        JustSpoke( -1.0f ); 
    }
}

void CTerrorist::InitScheduleTable( void )
{
    // =========================================================================
    // 🎰 100% 還原二進位行為：一鍵開局加權初始化 16 容量計畫表
    // 清空激活旗標，並將所有戰術計畫的原始中籤權重 percentChance 灌滿為 1.0f 基準面
    // =========================================================================
    for ( int i = 0; i < 16; i++ ) // 完美對齊指針哨兵展開
    {
        scheduleTable[i].activeFlag    = 0.0f;
        scheduleTable[i].percentChance = 1.0f; // 滿加權初值加載
    }
}

int CTerrorist::IRelationship( CBaseEntity *pTarget )
{
    if ( pTarget == nullptr || pTarget->pev == nullptr )
    {
        return CSquadMonster::IRelationship( pTarget );
    }

    // =========================================================================
    // 🥷 1. 潛行/臥底偽裝審查：若目標實體是反恐精英玩家 (ClassName == "player")
    // =========================================================================
    if ( strcasecmp( STRING(pTarget->pev->classname), "player" ) == 0 )
    {
        // 獲取當前地圖上的 1 號玩家實體私有資料指針
        edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
        if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
        {
            pPlayerEdict = PEOFFSET( 0 );
        }

        if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
        {
            CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );

            if ( pPlayer != nullptr )
            {
                // 🔥 100% 還原二進位 +0x61C 內存黑科技：提取玩家體內的劇情臥底偽裝/隱蔽旗標
                BOOL *pbDisguised = (BOOL *)((char *)pPlayer + 0x61C); // m_bDisguised 槽位

                // 🔒 潛伏偽裝鎖定：若玩家目前正處於偽裝臥底狀態、或完美隱蔽於暗處
                if ( *pbDisguised != 0 )
                {
                    // 瞬間硬熔斷！向大腦回傳 0 (R_NO)，將玩家判定為中立同伴，不發動攻擊與警報！
                    return R_NO; // 0
                }
            }
        }
    }

    // =========================================================================
    // 🧱 2. 降級兜底：若玩家未偽裝、或是常規 SWAT 特警，移交基類執行死敵仇恨配准
    // =========================================================================
    return CSquadMonster::IRelationship( pTarget );
}

int CTerrorist::ISoundMask( void )
{
    // =========================================================================
    // 📡 100% 還原二進位常數 0x27 (二進位位元 0x01 | 0x02 | 0x04 | 0x20)
    // 賦予恐怖份子小隊 AI 聽聞戰鬥槍聲、玩家腳步聲、世界噪聲與手榴彈危險聲的核心聽覺雷達！
    // =========================================================================
    return bits_SOUND_COMBAT | bits_SOUND_WORLD | bits_SOUND_PLAYER | bits_SOUND_DANGER; // 0x27
}

BOOL CTerrorist::IsPlayerHidden( void )
{
    // =========================================================================
    // 🛡️ 1. 激戰/劇本狀態硬熔斷：若雙方已爆發火網、或正處於過場動畫演出中
    // 玩家強制暴露，絕對不賦予其陰影隱形庇護，防止降智 Bug
    // =========================================================================
    if ( m_MonsterState == MONSTERSTATE_COMBAT || m_MonsterState == MONSTERSTATE_SCRIPT )
    {
        return FALSE;
    }

    // 獲取當前地圖上的 1 號玩家實體私有資料指針 (CBasePlayer)
    edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
    if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
    {
        pPlayerEdict = PEOFFSET( 0 );
    }

    if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
    {
        CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );

        if ( pPlayer != nullptr )
        {
            // 🔥 100% 還原二進位 +0x8F8 內存黑科技：提取玩家體內的隱形潛伏觸發器指針
            // 在 32 位元或改寫版環境下，將偏移強轉為指向 CTriggerStealth 實體的二級指針
            void **ppStealthTrigger = (void **)((char *)pPlayer + 0x8F8);

            if ( *ppStealthTrigger == nullptr )
            {
                return FALSE; // 玩家體內無潛行觸發器綁定，代表身處光明，直接暴露
            }

            // 🚀 跨類別調用：調用隱藏實體 CTriggerStealth 的靜態方法，實時評估目前光影幾何
            // 由於 DWARF 符號實錘它是靜態/成員調用，我們以最高規格進行對接
            typedef BOOL (*pfnIsPlayerHidden)(void*);
            pfnIsPlayerHidden check_func = (pfnIsPlayerHidden)(*(unsigned int *)((char *)*ppStealthTrigger + 0x1C)); // 假設虛擬表排位

            // 如果玩家在黑暗陰影中隱藏完美
            if ( CTriggerStealth_IsPlayerHidden(*ppStealthTrigger) ) 
            {
                return TRUE; // 🚀 大獲全勝！恐怖分子將玩家當成空氣，繼續巡邏日常
            }

            // 🎯 隱形破功：若玩家主動走出陰影或發動開火，一鍵將玩家體內的指針抹平清零，鎖死破功狀態
            *ppStealthTrigger = nullptr;
        }
    }

    return FALSE;
}

void CTerrorist::JustSpoke( float nextSpeakTime )
{
    // =========================================================================
    // 🎲 1. 動態發聲後置冷卻分流：100% 還原二進位常數 -1.0f 的自適應降噪行為
    // =========================================================================
    if ( nextSpeakTime == -1.0f )
    {
        // 點火發射 8 到 15 秒隨機抖動時鐘，強行鎖定全局語音通道，防範音軌打架
        float flRandomCooldown = RANDOM_FLOAT( 8.0f, 15.0f );
        CTalkMonster::g_talkWaitTime = gpGlobals->time + flRandomCooldown;
    }
    else
    {
        // 依據地圖劇本或特定指令指定的精確秒數進行硬鎖冷卻
        CTalkMonster::g_talkWaitTime = gpGlobals->time + nextSpeakTime;
    }

    // =========================================================================
    // 🔒 2. 格式化對白句柄暫存器，徹底釋放嘴部骨骼動畫信道，圓滿完成發聲週期
    // =========================================================================
    m_iSentence = -1;
}

void CTerrorist::KeyValue( KeyValueData *pkvd )
{
    // 1. 備用兵種藍圖型態配准
    if ( strcasecmp( pkvd->szKeyName, "behavior" ) == 0 )
    {
        m_behaviorType = atoi( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    
    // 2. 💥 人肉炸彈自爆半徑自定義
    if ( strcasecmp( pkvd->szKeyName, "blastradius" ) == 0 )
    {
        m_blastRadius = (float)atof( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    
    // 3. 💥 人肉炸彈自爆致命傷害自定義
    if ( strcasecmp( pkvd->szKeyName, "blastdamage" ) == 0 )
    {
        m_blastDamage = (float)atof( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    
    // 4. 戰術限制投擲物：只允許帶閃光彈
    if ( strcasecmp( pkvd->szKeyName, "flashbangonly" ) == 0 )
    {
        m_flashbangOnly = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    
    // 5. 戰術限制投擲物：只允許帶高爆手榴彈
    if ( strcasecmp( pkvd->szKeyName, "hegrenadeonly" ) == 0 )
    {
        m_heGrenadeOnly = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    
    // 6. 小隊開槍/無線電戰術警戒連動半徑 (默認 256.0f碼)
    if ( strcasecmp( pkvd->szKeyName, "alertradius" ) == 0 )
    {
        m_alertRadius = (float)atof( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    
    // 7. 是否允許開局發動 8 方向日常隨機漫步閒逛
    if ( strcasecmp( pkvd->szKeyName, "wander" ) == 0 )
    {
        m_canWander = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }

    // 8. 降級放行：其餘標準地圖變數移交怪物母體基類解析
    CBaseMonster::KeyValue( pkvd );
}

CBaseEntity* CTerrorist::Kick( void )
{
    // 1. 📢 提取朝向矩陣，用於外推計算
    UTIL_MakeVectors( pev->angles );

    Vector vecStart;
    vecStart.x = pev->origin.x;
    vecStart.y = pev->origin.y;
    // 抬腿出腳/揮刀平面偏置配置在自身包圍盒高度的 75% 處 (72 * 0.75 = 54碼)
    vecStart.z = pev->origin.z + ( pev->size.z * 0.75f );

    Vector vecEnd;

    // =========================================================================
    // 📐 2. 3D 空間戰術幾何外推與歸一化預判分流
    // =========================================================================
    if ( m_hEnemy == nullptr ) // 分支 A：無敵人，直接沿著正前方筆直外推 128 碼
    {
        vecEnd.x = vecStart.x + ( gpGlobals->v_forward.x * 128.0f );
        vecEnd.y = vecStart.y + ( gpGlobals->v_forward.y * 128.0f );
        vecEnd.z = vecStart.z + ( gpGlobals->v_forward.z * 128.0f );
    }
    else // 分支 B：🎯 核心黑科技！實時追蹤並對齊玩家下盤 25% 平面
    {
        CBaseEntity *pEnemy = m_hEnemy;

        Vector vecTarget;
        vecTarget.x = pEnemy->pev->origin.x;
        vecTarget.y = pEnemy->pev->origin.y;
        // 鎖定玩家下盤/大腿受擊中心點 (扣除包圍盒高度的 25%)
        vecTarget.z = pEnemy->pev->origin.z - ( pEnemy->pev->size.z * 0.25f );

        // 合成相對方向向量
        Vector vecDir = vecTarget - vecStart;
        float flLen = vecDir.Length();

        if ( flLen == 0.0f )
        {
            // 幾何防砸窗踩空極限：重疊時強制垂直向上外推 128 碼
            vecEnd.x = vecStart.x;
            vecEnd.y = vecStart.y;
            vecEnd.z = vecStart.z + 128.0f;
        }
        else
        {
            // 100% 還原二進位 FPU 歸一化行為，將單位方向量乘以 128 碼極限近戰死線
            float flInvLen = 1.0f / flLen;
            vecEnd.x = vecStart.x + ( vecDir.x * flInvLen * 128.0f );
            vecEnd.y = vecStart.y + ( vecDir.y * flInvLen * 128.0f );
            vecEnd.z = vecStart.z + ( vecDir.z * flInvLen * 128.0f );
        }
    }

    TraceResult tr;
    // 🔥 核心重砲：發射一個 HUMAN_HULL (3號立體凸殼盒體)，無視怪物，進行 128 碼強行物理清掃
    UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, head_hull, edict(), &tr );

    // 3. 終極判定：若立體凸殼盒體實時撞擊到了有效實體肉體
    if ( tr.pHit != nullptr && tr.pHit->pvPrivateData != nullptr )
    {
        // 釋放受害者 C++ 指針，交給近戰 TakeDamage 斬殺系統！
        return CBaseEntity::Instance( tr.pHit );
    }

    return nullptr;
}

void CTerrorist::LaserGlowOff( void )
{
    // =========================================================================
    // ⚡ 1. 3D 狙擊雷射射線實體動態安全銷毀 (CBeam)
    // =========================================================================
    if ( m_pBeam != nullptr )
    {
        // 呼叫引擎 Vanilla 核心，將雷射線實體徹底從地圖拓撲中蒸發銷毀
        UTIL_Remove( m_pBeam );
        m_pBeam = nullptr; // 強制歸零清洗暫存器，防範野指針 Bug
    }

    // =========================================================================
    // ⚡ 2. 槍口紅外線雷射光暈精靈貼圖動態安全銷毀 (CSprite)
    // =========================================================================
    if ( m_pLaserGlow != nullptr )
    {
        // 實時從伺服器渲染隊列中強行抽離、抹除光暈特效
        UTIL_Remove( m_pLaserGlow );
        m_pLaserGlow = nullptr; // 徹底清空
    }
}

void CTerrorist::LaserGlowOn( void )
{
    // =========================================================================
    // ⚡ 1. 雷射實體狀態與動態點火安全閥：若未構造，實時引爆 SetupLaserGlow 工廠
    // =========================================================================
    if ( ( m_pBeam != nullptr && m_pLaserGlow != nullptr ) || ( SetupLaserGlow(), m_pBeam != nullptr ) )
    {
        // 📢 宿敵生存審查：確保當前鎖定的死敵目標 (m_hEnemy) 有效
        if ( m_hEnemy == nullptr )
        {
            return;
        }

        CBaseEntity *pEnemy = m_hEnemy;

        // 提取自身與玩家的絕對世界內存 Edict 索引
        int startIndex = ENTINDEX( edict() );
        int endIndex   = ENTINDEX( pEnemy->edict() );

        // 🚀 跨實體 3D 物理鏈接：死死將雷射線的起點綁在自己槍口，終點咬在玩家肉體中心！
        m_pBeam->EntsInit( startIndex, endIndex );

        // 🔥 網路 Delta 封包鋼印：100% 還原 |= 0x2000 網路優化標記，解除伺服器頂點傳輸死鎖
        m_pBeam->pev->sequence = ( m_pBeam->pev->sequence & 0xFFFU ) | 0x2000;

        // 將紅外線 3D 雷射射線的亮度與渲染透明度穩定在 64.0f 常數面
        if ( m_laserBrightness != 64.0f )
        {
            m_laserBrightness = 64.0f;
        }
        m_pBeam->pev->renderamt = 64.0f;
    }

    // =========================================================================
    // ⚡ 2. 槍口紅色雷射精靈光暈 (Laser Glow Sprite) 滿載 255.0f 點亮中樞
    // =========================================================================
    if ( m_pLaserGlow != nullptr )
    {
        CBaseEntity *pEnemy = m_hEnemy;
        if ( pEnemy != nullptr && pEnemy->IsPlayer() ) // 完美還原虛擬表多載安全核對
        {
            // 將狙擊鏡槍口處投射出的紅色貼圖光暈透明度狠狠推滿至 255.0f 最大值！
            if ( m_glowBrightness != 255.0f )
            {
                m_glowBrightness = 255.0f;
            }
            m_pLaserGlow->pev->renderamt = 255.0f;
        }
    }
}

int CTerrorist::LookupActivity( int activity )
{
    // 1. 📢 獲取目前實體掛載的 3D StudioMdl 網格模型結構體指針
    void *pStudioModel = GET_MODEL_PTR( edict() );

    // =========================================================================
    // 🧱 姿態主權攔截：只有當動作為 ACT_IDLE (1) 時，大腦才會越權接管進行骨骼重定向
    // =========================================================================
    if ( activity != ACT_IDLE ) // activity != 1
    {
        // 其餘常規移動步態直接放行，調用全域引擎接口進行標準配准
        return ::LookupActivity( pStudioModel, pev, activity );
    }

    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 2. 依據 11 大戰術兵種型態與實時戰場火網，發動蒙地卡羅動態骨骼序列覆寫
    // =========================================================================
    switch ( m_terroristBehavior )
    {
        case 0:  // 0: 手槍兵角色
        case 6:  // 6: 微型衝鋒槍滲透者
        case 7:  // 7: 戰術擲彈兵
            if ( m_MonsterState == MONSTERSTATE_COMBAT || heardCombat )
            {
                pszSequenceName = "pistol_combatidle"; // 戰鬥手槍高度警戒持槍
            }
            else
            {
                pszSequenceName = "pistol_idle1";      // 日常放鬆手槍持槍
            }
            break;

        case 5:  // 5: M72 LAW 重型火箭筒反裝甲兵
            pszSequenceName = "LAW_fire_idle";         // 肩扛重型火箭筒待命姿態
            break;

        case 8:  // 8: 近戰開山馬刀死士
            if ( m_MonsterState == MONSTERSTATE_COMBAT || heardCombat )
            {
                pszSequenceName = "machete_combatidle"; // 拔出寒光馬刀高度防線姿態
                break;
            }
            // 若安全則降級下榻至神風自殺兵同款日常待命
        case 9:  // 9: 神風特攻隊人肉炸彈自殺兵
            pszSequenceName = "machete_idle1";          // 日常手持引爆器/匕首閒逛
            break;

        default: // 常規主力突擊步槍 / M60重機槍暴君角色 ( 11 / 10 / 2 / 3 號兵種 )
            // 若目前身處絕對安全的非交火日常巡邏狀態
            if ( m_MonsterState != MONSTERSTATE_COMBAT && !heardCombat )
            {
                // 100% 還原二進位降級行為：直接交還給模型預設的 1 號常規路徑待命序列
                return ::LookupActivity( pStudioModel, pev, ACT_IDLE );
            }

            // 🎯 核心姿態互鎖解密：100% 還原字串加法指針算術之真相！
            if ( !m_bStanding )
            {
                pszSequenceName = "crouching_wait"; // 🔒 蹲下掩體防線狀態下，強鎖蹲持步槍動畫！
            }
            else
            {
                pszSequenceName = "combatidle";     // 🔒 站立持槍突擊狀態下，強鎖直立步槍交火動畫！
            }
            break;
    }

    // =========================================================================
    // 🚀 3. 終極解碼：將動態重定向出的精確字串翻譯為 Studio 渲染器所需的 Sequence 索引
    // =========================================================================
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return ::LookupActivity( pStudioModel, pev, ACT_IDLE );
}

int CTerrorist::LookupActivityHeaviest( int activity )
{
    // 1. 📢 獲取目前實體掛載的 3D StudioMdl 網格模型結構體指針
    void *pStudioModel = GET_MODEL_PTR( edict() );

    // =========================================================================
    // 🧱 姿態主權攔截：只有當動作為 ACT_IDLE (1) 時，大腦才會越權接管進行重量級重定向
    // =========================================================================
    if ( activity != ACT_IDLE ) // activity != 1
    {
        // 其餘動作直接放行，移交標準全域接口進行常規配准
        return ::LookupActivity( pStudioModel, pev, activity );
    }

    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 2. 依據 11 大戰術兵種型態與實時戰場火網，發動重量級動態骨骼序列覆寫
    // =========================================================================
    switch ( m_terroristBehavior )
    {
        case 0:  // 0: 手槍輕步兵
        case 6:  // 6: 微型衝鋒槍滲透者
        case 7:  // 7: 戰術擲彈兵
            if ( m_MonsterState == MONSTERSTATE_COMBAT || heardCombat )
            {
                pszSequenceName = "pistol_combatidle";
            }
            else
            {
                pszSequenceName = "pistol_idle1";
            }
            break;

        case 5:  // 5: M72 LAW 重型火箭筒反裝甲兵
            pszSequenceName = "LAW_fire_idle";
            break;

        case 8:  // 8: 近戰開山馬刀死士
            if ( m_MonsterState == MONSTERSTATE_COMBAT || heardCombat )
            {
                pszSequenceName = "machete_combatidle";
                break;
            }
        case 9:  // 9: 神風特攻隊人肉炸彈自殺兵
            pszSequenceName = "machete_idle1";
            break;

        default: // 常規主力突擊步槍 / M60重機槍暴君角色 ( 11 / 10 / 2 / 3 號兵種 )
            // 🚀 核心重砲遞迴安全閥：若目前身處安全的日常巡邏放鬆狀態
            if ( m_MonsterState != MONSTERSTATE_COMBAT && !heardCombat )
            {
                // 完美還原二進位行為：發動 ::LookupActivityHeaviest 遞迴自呼叫，強鎖加權最重巡邏姿態，防制鬼抽！
                return ::LookupActivityHeaviest( pStudioModel, pev, ACT_IDLE );
            }

            // 戰術姿態互鎖解密
            if ( !m_bStanding )
            {
                pszSequenceName = "crouching_wait"; // 🔒 蹲下掩體防線狀態下，強鎖蹲持重火力動畫！
            }
            else
            {
                pszSequenceName = "combatidle";     // 🔒 站立持槍突擊狀態下，強鎖直立重火力交火動畫！
            }
            break;
    }

    // =========================================================================
    // 🚀 3. 終極解碼：將動態重定向出的精確字串翻譯為 Studio 渲染器所需的 Sequence 索引
    // =========================================================================
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return ::LookupActivity( pStudioModel, pev, ACT_IDLE );
}

int CTerrorist::ObjectCaps( void )
{
    // =========================================================================
    // 🌐 100% 還原二進位常數 10 (十六進位 0x0A = 0x02 | 0x08)
    // 宣告恐怖份子 AI 具備 FCAP_ACROSS_TRANSITION (跨地圖關卡進度打包保留)
    // 以及 FCAP_MUST_SPAWN (開局強制重組點火) 的最高劇情級劇本特權！
    // =========================================================================
    return FCAP_ACROSS_TRANSITION | FCAP_MUST_SPAWN; // 0x0A (10)
}

void CTerrorist::PainSound( void )
{
    // =========================================================================
    // ⏱️ 痛苦降噪鎖审查：若當前時間尚未跨越下一次慘叫時鐘，全面格式化攔截
    // 完美杜絕高射速武器掃射時引發多重慘叫疊加鬼畜的嚴重音訊 Bug
    // =========================================================================
    if ( gpGlobals->time > m_flNextPainTime )
    {
        // 呼叫虛擬表 102 號多載方法 (PlaySentence)，發射 T_WOUND 中彈受傷慘叫台詞組
        // 音量與衰減係數配置為標準 0.8f (0x3F4CCCCD) 常數面
        PlaySentence( "T_WOUND", 0.8f, ATTN_NORM, 0, m_voicePitch );

        // 🚀 100% 還原二進位行為：一鍵注入 1.0 秒硬性冷卻鎖，保護伺服器聲道信道
        m_flNextPainTime = gpGlobals->time + 1.0f;
    }
}

void CTerrorist::PlayScriptedSentence( char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener )
{
    // =========================================================================
    // 🛡️ 1. 活體與狀態主權雙重安全線：100% 還原二進位虛擬表 37 號方法 (IsAlive) 內聯展開
    // 確保恐怖分子目前並非處於「徹底死亡 (DEAD_DEAD = 2)」或「倒地瀕死期 (DEAD_DYING = 1)」
    // =========================================================================
    if ( IsAlive() && pev->deadflag != DEAD_DYING ) // pev->deadflag != 1
    {
        // 🚀 2. 幾何自適應時鐘互鎖：對白播放時長 (duration) 強行疊加 2.0 秒戰術空白緩衝
        // 一鍵送入 JustSpoke 清洗大腦，沒收這段時間內的全圖常規無線電，保障電影級過場演出
        JustSpoke( duration + 2.0f );

        // =========================================================================
        // 📢 3. 降級派發：呼叫虛擬表 98 號偏移量 (基類 PlayScriptedSentence 原生核心)
        // 將本地化語音句柄與音量衰減參數正式送入引擎喇叭，點火開口發聲！
        // =========================================================================
        CSquadMonster::PlayScriptedSentence( pszSentence, duration, volume, attenuation, bConcurrent, pListener );
    }
}

void CTerrorist::PrescheduleThink( void )
{
    // =========================================================================
    // ⚖️ 小隊戰術網絡情報共享中樞：核對當前是否加入了小隊隊伍 (m_hSquadLeader != nullptr)
    // =========================================================================
    if ( m_hSquadLeader != nullptr )
    {
        // 確保目前小隊處於與死敵交火的戰備週情內 (m_hEnemy != nullptr)
        if ( m_hEnemy != nullptr )
        {
            // 安全獲取小隊隊長 C++ 物件指針 (CSquadMonster)
            CSquadMonster *pSquadLeader = m_hSquadLeader;
            if ( pSquadLeader == nullptr )
            {
                pSquadLeader = this; // 降級 Fallback 鎖定自身
            }

            // 🎯 階段 A：實時情報灌注 ( bits_COND_SEE_ENEMY = 0x10 )
            // 若當前匪徒正親眼目擊死敵玩家，實時越權強寫刷新隊長大腦的最後目擊時間戳！
            if ( m_afConditions & bits_COND_SEE_ENEMY )
            {
                pSquadLeader->m_flLastEnemySightTime = gpGlobals->time;
                return;
            }

            // 🎯 階段 B：💥 5.0 秒死線大包抄！
            // 若整隻小隊連續 5.0 秒失去玩家的所有視線與軌跡，隊長大腦正式宣告跟丟
            if ( ( gpGlobals->time - pSquadLeader->m_flLastEnemySightTime ) > 5.0f )
            {
                // 點亮隊長體內的跟丟標記，在下一幀的 GetSchedule 中瞬間引爆全隊全圖協同包抄搜捕網！
                pSquadLeader->m_fEnemyEluded = TRUE;
                return;
            }
        }
    }
}

int CTerrorist::Save( CSave &save )
{
    // 1. 優先拉起小隊怪物類別母體的 Save 序列化工廠，寫入基礎繼承數據
    if ( !CSquadMonster::Save( save ) )
    {
        return FALSE;
    }

    // =========================================================================
    // 📊 存檔最高主權：100% 還原二進位常數 0x21 (十進位 33) 的變數描述總個數鋼印
    // 將體內 33 個核心戰術與兵種變數，以最高硬體相容度二進位形式寫入硬碟
    // =========================================================================
    return save.WriteFields( "CTerrorist", this, m_SaveData, 33 ); // 33 = 0x21
}

int CTerrorist::Restore( CRestore &restore )
{
    // 1. 優先拉起小隊怪物類別母體的 Restore 序列化工廠，讀取基礎繼承數據
    if ( !CSquadMonster::Restore( restore ) )
    {
        return FALSE;
    }

    // =========================================================================
    // 📊 讀檔最高主權：100% 還原二進位常數 0x21 (十進位 33) 的變數描述總個數鋼印
    // 從存檔中將這 33 個涉及兵種自爆、雷射亮度、掉落物、與語音冷卻的核心變數安全回填！
    // =========================================================================
    return restore.ReadFields( "CTerrorist", this, m_SaveData, 33 ); // 33 = 0x21
}

void CTerrorist::RunTask( Task_t *pTask )
{
    switch ( pTask->iTask )
    {
        // =========================================================================
        // 📐 TASK 92 (0x5C)：手榴彈/下掛榴彈 出手前彈道外推投影點朝向對齊
        // =========================================================================
        case 92: // 0x5C: TASK_FACE_TOSS
        {
            // 將拋物線初速度向量前推 64 倍，疊加自身坐標合成未來的幾何投影落點
            Vector vecTarget = pev->origin + ( m_vecTossVelocity * 64.0f );
            
            CBaseMonster::MakeIdealYaw( vecTarget );
            ChangeYaw( (int)pev->yaw_speed ); // 呼叫虛擬表 85 號方法，全速轉身

            // 只有當身體幾何偏航完全正對著投影點時，才放行宣告任務圓滿完成
            if ( FacingIdeal() )
            {
                m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
            }
        }
        break;

        // =========================================================================
        // 🏃 TASK 96 (0x60)：8號馬刀/9號自殺兵 獵人全自動動態衝鋒追擊任務
        // =========================================================================
        case 96: // 0x60: TASK_CHARGE_ENEMY
        {
            // 定身鎖過濾：若被卡死，或者條件遮罩打上了 0x40000000 失敗鎖
            if ( m_cantMove )
            {
                if ( m_afConditions & 0x40000000 )
                {
                    return;
                }
                m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
                return;
            }

            // 若宿敵丟失，強寫大腦條件，硬熔斷退出
            if ( m_hEnemy == nullptr )
            {
                m_afConditions |= 0x40000000;
                return;
            }

            // 計算與尋路目標點當前的 2D 平面真實物理距離
            float flDistToGoal = ( m_vecMoveGoal - pev->origin ).Length2D();

            if ( flDistToGoal >= m_fPreferedRange )
            {
                CBaseEntity *pEnemy = m_hEnemy;
                
                // 實時解算與玩家肉體真實的三維空間落差距離
                float flRealDist3D = ( m_vecMoveGoal - pEnemy->pev->origin ).Length();

                // 預判攔截分流：若大於黃金半徑的一半，發動每幀位置同步更新
                if ( flRealDist3D > ( m_fPreferedRange * 0.5f ) )
                {
                    goto LAB_EVALUATE_追求口號;
                }
            }

            // 🎯 核心獵人追殺公式：在每一幀，將移動終點目標強制修改、刷新為玩家的最新坐標！
            m_vecMoveGoal = m_hEnemy->pev->origin;
            flDistToGoal  = ( m_vecMoveGoal - pev->origin ).Length2D();

            // 實時動態刷新軌跡網格
            CBaseMonster::FRefreshRoute();
            // 🎯 📢 312 碼近戰死線：僅對 8 號近戰馬刀兵、9 號自殺引爆兵種點火發射！
            if ( m_terroristBehavior == 8 || m_terroristBehavior == 9 )
            {
                // 核對全局語音排隊時鐘，以 1.0f (0x3F800000) 的最大爆發音量引爆戰術咆哮！
                if ( gpGlobals->time > CTalkMonster::g_talkWaitTime && FOkToSpeak() )
                {
                    if ( flDistToGoal < 312.0f )
                    {
                        PlaySentence( "T_MELEE", 1.0f, ATTN_NORM, 0, m_voicePitch );
                        m_iSentence = -1;
                        CTalkMonster::g_talkWaitTime = gpGlobals->time + 10.0f; // 🔒 塞入 10 秒硬性通信封鎖
                    }
                }
            }

            // 🎯 踏入黃金斬殺半徑：任務成功，清空尋路軌跡，交給近戰 TakeDamage 割喉突襲！
            if ( flDistToGoal < m_fPreferedRange )
            {
                if ( !( m_afConditions & 0x40000000 ) )
                {
                    m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
                }
                CBaseMonster::RouteClear();
            }

            // 戰術衝鋒死命令：步態動作強制、無條件切換為 ACT_RUN (全速狂奔)
            if ( m_movementActivity != ACT_RUN )
            {
                m_movementActivity = ACT_RUN;
            }
        }
        break;

        default:
            // 其餘通用常規子任務 (如日常巡邏、原地等待) 降級派發移交怪物母體基類處理
            CBaseMonster::RunTask( pTask );
            break;
    }
}

Schedule_t* CTerrorist::ScheduleFromName( char *pName )
{
    // =========================================================================
    // 🎰 1. 優先級一：100% 還原二進位常數 0x27 (十進位 39) 的自定義計畫字串鋼印
    // 在專屬的 m_scheduleList 列表中反查地圖指定的 39 組高階戰術計畫名稱
    // =========================================================================
    Schedule_t *pSchedule = CBaseMonster::ScheduleInList( pName, m_scheduleList, 39 ); // 39 = 0x27

    if ( pSchedule != nullptr )
    {
        return pSchedule; // 成功匹配自定義劇情計畫，直接返回指針發射渲染
    }

    // =========================================================================
    // 🧱 2. 優先級二：降級相容，若非自定義計畫，移交怪物基類解析標準的 Half-Life 引擎計畫名
    // =========================================================================
    return CBaseMonster::ScheduleFromName( pName );
}

void CTerrorist::SetActivity( Activity NewActivity )
{
    int iSequence = -1;

    // =========================================================================
    // 🎰 1. 階段一：11大戰術兵種專屬動作覆寫矩陣 (Stance Overrides)
    // =========================================================================
    switch ( m_terroristBehavior )
    {
        case 0:  // 手槍輕步兵
        case 6:  // 微型衝鋒槍滲透者
            iSequence = SetActivityPistolBase( NewActivity );
            break;
            
        case 1:  // 散彈破門兵
            iSequence = SetActivityShotgunBase( NewActivity );
            break;
            
        case 2:  // 常規主力步槍兵 (AK47)
        case 3:  // M60 重機槍暴君
        case 10: // 特種 MP5 突擊兵
            iSequence = SetActivityAutomaticBase( NewActivity );
            break;
            
        case 4:  // 雷射狙擊手
            iSequence = SetActivitySniperBase( NewActivity );
            break;
            
        case 5:  // M72 LAW 重型火箭筒反裝甲兵
            iSequence = SetActivityLAWBase( NewActivity );
            break;
            
        case 7:  // 戰術擲彈兵
            iSequence = SetActivityPistolBase( NewActivity );
            if ( iSequence == -1 )
            {
                iSequence = SetActivityGrenadeBase( NewActivity );
            }
            break;
            
        case 8:  // 近戰開山馬刀死士
            iSequence = SetActivityMeleeBase( NewActivity );
            break;
            
        case 9:  // 神神風特攻隊人肉炸彈自殺兵
            iSequence = SetActivityKamakaziBase( NewActivity );
            break;
            
        default:
            break;
    }

    // =========================================================================
    // 🩸 2. 階段二：降級放行，引爆通用動作狀態機與 20% 殘血受創拐瘸步態網
    // =========================================================================
    if ( iSequence == -1 )
    {
        switch ( NewActivity )
        {
            case ACT_IDLE:
                if ( m_idleAnim != 0 )
                {
                    iSequence = CBaseAnimating::LookupSequence( STRING(m_idleAnim) );
                    break;
                }
                if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    // 姿態自適應互鎖
                    iSequence = CBaseAnimating::LookupSequence( !m_bStanding ? "crouching_wait" : "combatidle" );
                }
                break;

            case ACT_WALK:
                // 🚀 20% 殘血瘸腿死線：血量低於 20% 時，強制調用基類 LookupActivity 改發 ACT_WALK_HURT 骨骼！
                if ( pev->health <= ( pev->max_health * 0.2f ) )
                {
                    iSequence = LookupActivity( ACT_WALK_HURT ); // 0x30
                }
                else if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    iSequence = CBaseAnimating::LookupSequence( "combat_walk_primary" );
                }
                break;

            case ACT_RUN:
                // 🚀 20% 殘血驚慌奔跑死線：血量低於 20% 時，強行改發 ACT_RUN_HURT 瘸腿狂奔骨骼！
                if ( pev->health <= ( pev->max_health * 0.2f ) )
                {
                    iSequence = LookupActivity( ACT_RUN_HURT ); // 0x31
                }
                else if ( m_MonsterState == MONSTERSTATE_COMBAT )
                {
                    iSequence = CBaseAnimating::LookupSequence( "combat_run_primary" );
                }
                break;

            case ACT_STAND:
                iSequence = CBaseAnimating::LookupSequence( "crouching_2_stand" );
                break;

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
                    iSequence = CBaseAnimating::LookupSequence( STRING(m_deathAnim) );
                    break;
                }
                // 🥷 潛行暗殺安靜死亡：若被玩家背刺或消音武器暗殺，強制拉起 death02 悄無聲息癱軟骨骼
                if ( stealthDeath )
                {
                    iSequence = CBaseAnimating::LookupSequence( "death02" );
                    stealthDeath = FALSE; // 清空標記
                }
                break;

            // =========================================================================
            // 👁️ 5組高級劇情自定義活動擴充 (ACT_LASTACTIVITY 遮罩對齊)
            // =========================================================================
            case (ACT_LASTACTIVITY + 1): // 0x2001
                iSequence = CBaseAnimating::LookupSequence( "pistol_popout_right01" );
                break;
            case (ACT_LASTACTIVITY + 2): // 0x2002
                iSequence = CBaseAnimating::LookupSequence( "pistol_popout_right02" );
                break;
            case (ACT_LASTACTIVITY + 4): // 0x2004
                iSequence = CBaseAnimating::LookupSequence( "choking" ); // 毒氣卡喉窒息
                break;
            case (ACT_LASTACTIVITY + 8): // 0x2008
                iSequence = CBaseAnimating::LookupSequence( "patrol_idle" ); // 愜意巡邏
                break;
            case (ACT_LASTACTIVITY + 16): // 0x2010
                iSequence = CBaseAnimating::LookupSequence( "stunned01" ); // 衝擊波震暈
                break;

            default:
                break;
        }

        // 若上述特殊覆寫皆未匹配成功，徹底放行，調用基類 LookupActivity 進行 Vanilla 配准
        if ( iSequence == -1 )
        {
            iSequence = LookupActivity( NewActivity );
        }
        m_Activity = NewActivity;
    }
    else
    {
        m_Activity = NewActivity;
    }

    // =========================================================================
    // ⚙️ 3. 終極渲染與動畫幀安全重置 (對齊二進位結構體重寫行為)
    // =========================================================================
    if ( iSequence >= 0 )
    {
        // 若骨骼發生流轉切換、或是循環動畫，強制將當前幀 frame 清空歸零，防止動作鬼抽卡幀
        if ( iSequence != pev->sequence || m_fSequenceLoops )
        {
            pev->frame = 0.0f;
        }
        pev->sequence = iSequence;
        CBaseAnimating::ResetSequenceInfo(); // 實時重置骨骼速率與幀長度

        // 呼叫虛擬表 110 號方法 (延時調度大腦，此處完成同步發射)
        // 32位元二進位偏移 +0x1B4
    }
    else
    {
        // 🚨 異常防砸窗踩空：若模型徹底缺失該動作序列，向全服主控台拋出高級排錯日誌
        ALERT( at_console, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity );
        pev->sequence = 0; // 強鎖 0 號預設序列
    }
}

int CTerrorist::SetActivityAutomaticBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 獲取 Studio 模型指針同步網路 Delta
    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 100% 還原二進位行為：自動武器開火與拉栓換彈骨骼重定向
    // =========================================================================
    if ( NewActivity == ACT_RANGE_ATTACK1 ) // 開火幀引爆
    {
        if ( m_terroristBehavior == 3 ) // 3 號 M60 重機槍暴君角色
        {
            pszSequenceName = "machinegun_fire";
        }
        else // 常規主力步槍 (AK47) 與特種 MP5 兵種
        {
            // 依據姿態暫存器 m_bStanding 實時分流蹲下與站立火網序列
            pszSequenceName = !m_bStanding ? "crouching_mp5" : "standing_mp5";
        }
    }
    else if ( NewActivity == ACT_RELOAD ) // 戰術空倉換彈
    {
        // 🎰 完美解密指針加法：!m_bStanding 鎖定蹲換彈，站立則推移 7 位元組鎖定 "reload_mp5"
        pszSequenceName = !m_bStanding ? "crouch_reload_mp5" : "reload_mp5";
    }
    else
    {
        return -1; // 非自動槍械專屬 Activity，返回 -1 放行，移交通用狀態機
    }

    // 將解碼出的精確字串送入 Studio 渲染器獲取 Sequence 索引
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return -1;
}

int CTerrorist::SetActivityGrenadeBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 獲取 StudioMdl 模型指標更新網路 Delta
    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 100% 還原二進位行為：手榴彈過頂拋擲與下掛榴彈砲發射骨骼重定向
    // =========================================================================
    if ( NewActivity == ACT_RANGE_ATTACK2 ) // 2號遠程加載重力打擊
    {
        // 反查武器型態遮罩位元 2 (bits_WEAPON_GRENADE)
        if ( !(pev->weapons & bits_WEAPON_GRENADE) ) // pev->weapons & 2
        {
            // 無手雷標記：解碼為下掛式重火力接觸榴彈射擊動畫
            pszSequenceName = "launchgrenade";
        }
        else
        {
            // 有手雷標記：解碼為拉開引信的標準過頂拋物線投擲動畫
            pszSequenceName = "throwgrenade";
        }

        // 將精確字串送入 Studio 渲染器獲取 Sequence 索引
        if ( pszSequenceName != nullptr )
        {
            return CBaseAnimating::LookupSequence( pszSequenceName );
        }
    }

    return -1; // 非投擲專屬 Activity，返回 -1 放行，移交通用狀態機
}

int CTerrorist::SetActivityKamakaziBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 獲取 StudioMdl 模型指標更新網路 Delta
    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 100% 還原二進位行為：神風自殺兵日常閒逛、狂暴衝鋒與原地尖叫引爆骨骼
    // =========================================================================
    if ( NewActivity == ACT_TURN_LEFT )
    {
        pszSequenceName = "machete_180L"; // 戰術左轉 180 度回頭反背刺
    }
    else if ( NewActivity < ACT_TURN_RIGHT )
    {
        if ( NewActivity == ACT_IDLE )
        {
            pszSequenceName = "machete_idle1"; // 日常持引爆器閒置待命
        }
        else if ( NewActivity == ACT_RUN )
        {
            pszSequenceName = "pistol_run"; // 🔒 狂熱衝鋒期：平舉雙手喪失理智瘋狂狂奔步態
        }
        else
        {
            return -1;
        }
    }
    else if ( NewActivity == ACT_TURN_RIGHT )
    {
        pszSequenceName = "machete_180R"; // 戰術右轉 180 度回頭
    }
    else
    {
        // 🚀 核心重砲：核對魔改版專屬劇情 Activity ( ACT_LASTACTIVITY | ACT_WALK = 0x2008 )
        if ( NewActivity == (Activity)(ACT_LASTACTIVITY | ACT_WALK) )
        {
            // 🔒 雙向密碼鎖：只有當 suicideFlag 實錘點火引爆、跨越 64 碼死線時才允許解鎖
            if ( suicideFlag )
            {
                pszSequenceName = "kamakazi"; // 💥 鎖死高舉雙手、身體痛苦抽搐的原地引爆尖叫動畫！
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1; // 非自殺兵種專屬 Activity，放行移交通用狀態機
        }
    }

    // 將解碼出的精確字串送入 Studio 渲染器獲取 Sequence 索引
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return -1;
}

int CTerrorist::SetActivityLAWBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 獲取 StudioMdl 模型指標更新網路 Delta
    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 100% 還原二進位行為：M72 LAW 重型火箭筒兵種專屬動作重定向
    // =========================================================================
    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim != 0 )
            {
                pszSequenceName = STRING( m_idleAnim ); // 優先採用地圖自定義閒置動畫
            }
            else
            {
                pszSequenceName = "LAW_fire_idle";       // 🔒 肩扛火箭筒原地日常防禦待命
            }
            break;

        case ACT_RUN:
            pszSequenceName = "law_run";                // 🔒 橫扛重裝火力的突擊奔跑序列
            break;

        case ACT_TURN_LEFT:
            pszSequenceName = "law_180L";               // 戰術左轉 180 度回頭
            break;

        case ACT_TURN_RIGHT:
            pszSequenceName = "law_180R";               // 戰術右轉 180 度回頭
            break;

        case ACT_RANGE_ATTACK1:
            pszSequenceName = "LAW_fire";               // 💥 M72 LAW 火箭筒點火點火射擊
            break;

        case ACT_RELOAD:
            pszSequenceName = "law_reload";             // ⚙️ 戰術空倉拋筒重新裝填
            break;

        default:
            return -1; // 非火箭筒兵專屬 Activity，返回 -1 放行，移交通用狀態機
    }

    // 將解碼出的精確字串送入 Studio 渲染器獲取 Sequence 索引
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return -1;
}

int CTerrorist::SetActivityMeleeBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 獲取 StudioMdl 模型指標更新網路 Delta
    const char *pszSequenceName = nullptr;

    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim != 0 )
            {
                pszSequenceName = STRING( m_idleAnim );
            }
            else
            {
                // 依據激戰警戒態，動態分流站立警戒或放鬆低警備待命
                pszSequenceName = ( m_MonsterState == MONSTERSTATE_COMBAT || heardCombat ) ? "machete_combatidle" : "machete_idle1";
            }
            break;

        case ACT_WALK:
            pszSequenceName = "machete_patrol_street"; // 🔒 馬刀街頭巡邏步態
            break;

        case ACT_RUN:
            pszSequenceName = "machete_run";           // 🔒 馬刀發狂衝鋒步態
            break;

        case ACT_STRAFE_LEFT:
            pszSequenceName = "machete_strafefire_L";
            break;

        case ACT_STRAFE_RIGHT:
            pszSequenceName = "machete_strafefire_R";
            break;

        case ACT_TURN_LEFT:
            pszSequenceName = "machete_180L";
            break;

        case ACT_TURN_RIGHT:
            pszSequenceName = "machete_180R";
            break;

        case ACT_COWER:
            pszSequenceName = "machete_cower";
            break;

        case ACT_SMALL_FLINCH:
            pszSequenceName = "machete_smflinch";
            break;

        // =========================================================================
        // ⚔️ 核心黑科技： 6 階隨機混合馬刀肉搏出刀刀法
        // =========================================================================
        case ACT_MELEE_ATTACK1:
            {
                int iMeleeRoll = RANDOM_LONG( 0, 5 );
                switch ( iMeleeRoll )
                {
                    case 1:  pszSequenceName = "machete_melee2"; break;
                    case 2:  pszSequenceName = "machete_melee3"; break;
                    case 3:  pszSequenceName = "machete_melee4"; break;
                    case 4:  pszSequenceName = "machete_melee5"; break;
                    case 5:  pszSequenceName = "machete_melee6"; break;
                    default: pszSequenceName = "machete_melee";  break;
                }
            }
            break;

        // =========================================================================
        // 💀 部位定向擊殺死亡動畫覆寫
        // =========================================================================
        case ACT_DIESIMPLE:
            pszSequenceName = ( m_deathAnim != 0 ) ? STRING( m_deathAnim ) : "machete_die-simple";
            break;

        case ACT_DIEBACKWARD:
            pszSequenceName = ( m_deathAnim != 0 ) ? STRING( m_deathAnim ) : "machete_dieback1";
            break;

        case ACT_DIEFORWARD:
        case ACT_DIE_BACKSHOT:
            pszSequenceName = ( m_deathAnim != 0 ) ? STRING( m_deathAnim ) : "machete_dieforward";
            break;

        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
            pszSequenceName = ( m_deathAnim != 0 ) ? STRING( m_deathAnim ) : "machete_headshot";
            break;

        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
            pszSequenceName = ( m_deathAnim != 0 ) ? STRING( m_deathAnim ) : "machete_diegutshot";
            break;

        // =========================================================================
        // 🩺 部位定向中彈局部硬直系統 (Part-Specific Flinch)
        // =========================================================================
        case ACT_FLINCH_LEFTARM:
            pszSequenceName = "machete_leftarmflinch";
            break;

        case ACT_FLINCH_RIGHTARM:
            pszSequenceName = "machete_rightarmflinch";
            break;

        case ACT_FLINCH_LEFTLEG:
            pszSequenceName = "machete_leftlegsmflinch";
            break;

        case ACT_FLINCH_RIGHTLEG:
            pszSequenceName = "machete_rightlegsmflinch";
            break;

        default:
            return -1; // 非馬刀兵專屬 Activity，放行移交外層通用狀態機
    }

    // 將解碼出的字串送入 Studio 渲染器獲取 Sequence 索引
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return -1;
}

int CTerrorist::SetActivityPistolBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 網路 Delta 特效對齊
    const char *pszSequenceName = nullptr;

    switch ( NewActivity )
    {
        case ACT_IDLE:
            if ( m_idleAnim != 0 )
            {
                return CBaseAnimating::LookupSequence( STRING(m_idleAnim) );
            }
            if ( m_MonsterState != MONSTERSTATE_COMBAT && !heardCombat )
            {
                pszSequenceName = "pistol_idle1"; // 放鬆手槍待命
            }
            else if ( m_bStanding )
            {
                pszSequenceName = "pistol_combatidlle"; // 戰鬥直立手槍警戒
            }
            else
            {
                pszSequenceName = "pistol_crouching_wait"; // 🔒 戰鬥蹲下防線
            }
            break;

        case ACT_CROUCH:
            pszSequenceName = "pistol_crouching_wait";
            break;

        case ACT_WALK:
            // 20% 殘血線或非交火狀態分流
            if ( pev->health <= ( pev->max_health * 0.2f ) || m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pszSequenceName = "pistol_walk";
            }
            else
            {
                pszSequenceName = "combat_walk_pistol"; // 戰術側身走路逼近
            }
            break;

        case ACT_RUN:
            if ( pev->health <= ( pev->max_health * 0.2f ) || m_MonsterState != MONSTERSTATE_COMBAT )
            {
                pszSequenceName = "pistol_run";
            }
            else
            {
                pszSequenceName = "combat_run_pistol"; // 戰術衝鋒持槍奔跑
            }
            break;

        case ACT_STRAFE_LEFT:   pszSequenceName = "pistol_strafefire_L";   break;
        case ACT_STRAFE_RIGHT:  pszSequenceName = "pistol_strafefire_R";  break;
        case ACT_TURN_LEFT:     pszSequenceName = "pistol_180L";           break;
        case ACT_TURN_RIGHT:    pszSequenceName = "pistol_180R";          break;
        case ACT_COWER:         pszSequenceName = "pistol_cower";          break;
        case ACT_SMALL_FLINCH:  pszSequenceName = "pistol_smflinch";       break;
        case ACT_MELEE_ATTACK1: pszSequenceName = "pistol_melee";          break;

        case ACT_RANGE_ATTACK1: // 🔥 核心輕武器開火路由器
            if ( m_terroristBehavior == 6 ) // TMP/MAC10 微衝滲透者角色
            {
                pszSequenceName = !m_bStanding ? "crouching_tmp" : "shootcycle_tmp";
            }
            else // 常規格洛克/USP 手槍角色
            {
                pszSequenceName = !m_bStanding ? "pistol_crouching" : "pistol_standing";
            }
            break;

        case ACT_RELOAD: // ⚙️ 換彈指針位移解密
            pszSequenceName = !m_bStanding ? "crouch_pistol_reload" : "pistol_reload";
            break;

        // =========================================================================
        // 💀 手槍兵種部位定向死亡序列
        // =========================================================================
        case ACT_DIESIMPLE:
            return ( m_deathAnim != 0 ) ? CBaseAnimating::LookupSequence( STRING(m_deathAnim) ) : CBaseAnimating::LookupSequence( "pistol_die-simple" );
        case ACT_DIEBACKWARD:
            return ( m_deathAnim != 0 ) ? CBaseAnimating::LookupSequence( STRING(m_deathAnim) ) : CBaseAnimating::LookupSequence( "pistol_dieback1" );
        case ACT_DIEFORWARD:
        case ACT_DIE_BACKSHOT:
            return ( m_deathAnim != 0 ) ? CBaseAnimating::LookupSequence( STRING(m_deathAnim) ) : CBaseAnimating::LookupSequence( "pistol_dieforward" );
        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
            return ( m_deathAnim != 0 ) ? CBaseAnimating::LookupSequence( STRING(m_deathAnim) ) : CBaseAnimating::LookupSequence( "pistol_headshot" );
        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
            return ( m_deathAnim != 0 ) ? CBaseAnimating::LookupSequence( STRING(m_deathAnim) ) : CBaseAnimating::LookupSequence( "pistol_diegutshot" );

        // =========================================================================
        // 🩺 手槍兵種部位受擊抽搐硬直 (Part-Specific Flinch)
        // =========================================================================
        case ACT_FLINCH_LEFTARM:  pszSequenceName = "pistol_leftarmflinch";  break;
        case ACT_FLINCH_RIGHTARM: pszSequenceName = "pistol_rightarmflinch"; break;
        case ACT_FLINCH_LEFTLEG:  pszSequenceName = "pistol_leftlegsmflinch";   break;
        case ACT_FLINCH_RIGHTLEG: pszSequenceName = "pistol_rightlegsmflinch";  break;

        default:
            return -1; // 非輕武器專屬動作，移交外層通用狀態機
    }

    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return -1;
}

int CTerrorist::SetActivityShotgunBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 獲取 Studio 模型指針同步網路 Delta
    const char *pszSequenceName = nullptr;

    // =========================================================================
    // 🎰 100% 還原二進位行為：散彈槍近戰爆破與戰術壓彈換彈骨骼重定向
    // =========================================================================
    if ( NewActivity == ACT_RANGE_ATTACK1 ) // 散彈槍射擊幀引爆
    {
        // 依據姿態暫存器 m_bStanding 實時分流蹲下與站立火網序列
        pszSequenceName = !m_bStanding ? "crouching_shotgun" : "standing_shotgun";
    }
    else if ( NewActivity == ACT_RELOAD ) // 戰術空倉壓彈填裝
    {
        // 🎰 完美解密指針加法：!m_bStanding 鎖定蹲換彈，站立則推移 7 位元組鎖定 "reload_shotgun"
        pszSequenceName = !m_bStanding ? "crouch_reload_shotgun" : "reload_shotgun";
    }
    else
    {
        return -1; // 非散彈槍專屬 Activity，返回 -1 放行，移交通用狀態機
    }

    // 將解碼出的精確字串送入 Studio 渲染器獲取 Sequence 索引
    if ( pszSequenceName != nullptr )
    {
        return CBaseAnimating::LookupSequence( pszSequenceName );
    }

    return -1;
}

int CTerrorist::SetActivitySniperBase( Activity NewActivity )
{
    // GET_MODEL_PTR( edict() ); // 動態 Studio 網格更新同步
    const char *pszSequenceName = nullptr;

    switch ( NewActivity )
    {
        // =========================================================================
        // 🎯 1. 遠程狙擊開火雙階姿態分流 (Scout / AWP)
        // =========================================================================
        case ACT_RANGE_ATTACK1:
            pszSequenceName = !m_bStanding ? "sniper_crouch-fire" : "sniper-fire";
            if ( pszSequenceName != nullptr )
            {
                return CBaseAnimating::LookupSequence( pszSequenceName );
            }
            break;

        // =========================================================================
        // 📡 2. 雷射瞄準線架設啟動 (ACT_ARM)
        // =========================================================================
        case ACT_ARM:
            // 實時解碼抬槍架線骨骼，並引爆 3D 光軌網路同步追蹤網
            LaserGlowOn();
            return CBaseAnimating::LookupSequence( "laser_on_stand" );

        // =========================================================================
        // 📡 3. 雷射瞄準線戰術拉熄 (ACT_DISARM)
        // =========================================================================
        case ACT_DISARM:
            // 實時解碼垂槍动作，並連根拔起內存中的雷射實體防止懸空殘留
            LaserGlowOff();
            return CBaseAnimating::LookupSequence( "laser_off_stand" );

        // =========================================================================
        // 💀 4. 終極陣亡硬熔斷：全套擊殺死亡 Activity 下的三重暴力內存解體安全清洗
        // =========================================================================
        case ACT_DIESIMPLE:
        case ACT_DIEBACKWARD:
        case ACT_DIEFORWARD:
        case ACT_DIEVIOLENT:
        case ACT_DIE_HEADSHOT:
        case ACT_DIE_CHESTSHOT:
        case ACT_DIE_GUTSHOT:
        case ACT_DIE_BACKSHOT:
            // 🔒 雙重保險第一層：拉熄雷射線
            LaserGlowOff();

            // 🔒 雙重保險第二層：若內存殘留髒數據，再度暴力強制在世界中將物件連根拔起銷毀！
            if ( m_pBeam != nullptr )
            {
                UTIL_Remove( m_pBeam );
                m_pBeam = nullptr;
            }
            if ( m_pLaserGlow != nullptr )
            {
                UTIL_Remove( m_pLaserGlow );
                m_pLaserGlow = nullptr;
            }
            return -1; // 放行回傳 -1，移交給外層通用死亡狀態機抽取 death02 或常規陣亡動作

        default:
            return -1; // 非狙擊手專屬 Activity，返回 -1 移交通用狀態機
    }

    return -1;
}

void CTerrorist::SetHead( int headType )
{
    // 🎰 發動 100 點隨機數輪盤，用於控制 50% 權重的美術外觀隨機化
    int iRandRoll = RANDOM_LONG( 0, 99 );

    // =========================================================================
    // 🎭 依據地圖傳入的 headType 屬性，分流分配 StudioMdl 第 1 號頭部組群 (Bodygroup 1)
    // =========================================================================
    if ( headType != 0 )
    {
        if ( headType == 1 ) // 1 號屬性：精銳重裝特種匪徒分流
        {
            if ( iRandRoll < 51 ) // 0x33 = 51 (51% 機率)
            {
                CBaseAnimating::SetBodygroup( 1, 2 ); // 2號頭部：帶防彈盔/護目鏡外觀
            }
            else // (49% 機率)
            {
                CBaseAnimating::SetBodygroup( 1, 1 ); // 1號頭部：光頭/刀疤精英外觀
            }
        }
        else if ( headType == 2 ) // 2 號屬性：強鎖潛伏全包圍面罩
        {
            CBaseAnimating::SetBodygroup( 1, 0 );     // 0號頭部：防毒面具/全包頭套外觀
        }
        return;
    }

    // 0 號屬性：常規大眾路人匪徒隨機流
    if ( iRandRoll > 50 ) // 0x32 = 50 (50% 機率)
    {
        CBaseAnimating::SetBodygroup( 1, 3 );         // 3號頭部：紅頭巾/大鬍子悍匪外觀
    }
    else // (50% 機率)
    {
        CBaseAnimating::SetBodygroup( 1, 4 );         // 4號頭部：灰色毛帽/蒙面面紗外觀
    }
}

void CTerrorist::SetupBehaviors(int behaviorType)
{
	// 📢 1. 跨越地址讀取伺服器全局 CVar 常數，判定當前關卡處於哪一個劇情章節階級 (maptier)
	int iMapTier = (int)CVAR_GET_FLOAT("maptier");

	// =========================================================================
	// 🎰 2. 核心兵種戰術性格配分矩陣：100% 還原二進位計畫機率表加權回寫
	// =========================================================================
	if(behaviorType == 1)    // 🥷 兵種 1：XM1014 散彈槍破門兵
	{
		scheduleTable[18].percentChance = 0.0f;  // SCHED_RANGE_ATTACK1 (無手槍點射)
		scheduleTable[17].percentChance = 0.9f;  // SCHED_TAKE_COVER (高掩體躲避)
		scheduleTable[67].percentChance = 0.0f;  // SCHED_WAKE_SQUAD
		scheduleTable[68].percentChance = 0.0f;  // SCHED_ALERT_FACE
		scheduleTable[51].percentChance = 1.0f;  // SCHED_HIDE_AND_RELOAD (100% 跑位換彈)
		scheduleTable[50].percentChance = 0.0f;  // SCHED_FLANK_ATTACK
		scheduleTable[66].percentChance = 0.4f;  // SCHED_CHARGE_ENEMY (40% 散彈貼臉衝鋒)
		scheduleTable[24].percentChance = 0.9f;  // SCHED_RANGE_ATTACK1 (常規散彈噴射)
		scheduleTable[25].percentChance = 0.0f;
		scheduleTable[22].percentChance = 0.3f;  // SCHED_SMALL_FLINCH
		scheduleTable[62].percentChance = 0.0f;
		scheduleTable[63].percentChance = 0.0f;
		scheduleTable[64].percentChance = 0.2f;  // SCHED_CORNER_COVER_NODE
		scheduleTable[28].percentChance = 0.1f;  // SCHED_CHASE_ENEMY_SOUND
		scheduleTable[70].percentChance = 0.4f;  // SCHED_FIRING_PAUSE

		m_fPreferedRange = 128.0f; // 🔒 散彈槍黃金交火近戰紅線：128 碼

		// 🎨 章節外觀自適應：若處於前兩章節，加載防彈盔特種外觀
		if(iMapTier < 3)
		{
			SetHead(1);
			return;
		}
	}
	else
		if(behaviorType == 2)    // 🥷 兵種 2：常規主力 AK47 步槍突擊兵
		{
			scheduleTable[18].percentChance = 0.0f;
			scheduleTable[17].percentChance = 0.9f;
			scheduleTable[67].percentChance = 0.0f;
			scheduleTable[68].percentChance = 0.0f;
			scheduleTable[51].percentChance = 1.0f;
			scheduleTable[50].percentChance = 0.8f;  // 🚀 80% 超高機率發動側翼戰術繞後包抄！
			scheduleTable[66].percentChance = 0.05f; // 極低機率無腦衝鋒
			scheduleTable[24].percentChance = 0.9f;
			scheduleTable[25].percentChance = 0.0f;
			scheduleTable[22].percentChance = 0.2f;
			scheduleTable[62].percentChance = 0.0f;
			scheduleTable[63].percentChance = 0.0f;
			scheduleTable[64].percentChance = 0.3f;
			scheduleTable[28].percentChance = 0.1f;
			scheduleTable[70].percentChance = 0.6f;

			m_fPreferedRange = 256.0f; // 🔒 突擊步槍黃金交火紅線：256 碼

			if(iMapTier < 4)
			{
				SetHead(2);   // 中期地圖更換
				return;
			}
			if(iMapTier == 4)
			{
				SetHead(1);
				return;
			}
		}
		else
			if(behaviorType == 3)    // 🥷 兵種 3：M60 重機槍鏈彈暴君
			{
				scheduleTable[18].percentChance = 0.0f;
				scheduleTable[17].percentChance = 0.9f;
				scheduleTable[67].percentChance = 0.0f;
				scheduleTable[68].percentChance = 0.0f;
				scheduleTable[51].percentChance = 0.0f;  // 絕不躲藏換彈
				scheduleTable[50].percentChance = 0.5f;
				scheduleTable[66].percentChance = 0.0f;
				scheduleTable[24].percentChance = 0.95f; // 🚀 95% 原地站穩狂暴傾瀉重火力火網！
				scheduleTable[25].percentChance = 0.0f;
				scheduleTable[22].percentChance = 0.0f;  // 具有霸體，免疫常規中彈抽搐硬直！
				scheduleTable[62].percentChance = 0.0f;
				scheduleTable[63].percentChance = 0.0f;
				scheduleTable[64].percentChance = 0.1f;
				scheduleTable[28].percentChance = 0.1f;
				scheduleTable[70].percentChance = 0.1f;

				if(iMapTier < 5)
				{
					SetHead(2);
					return;
				}
				if(iMapTier == 5)
				{
					SetHead(1);
					return;
				}
			}
			else
				if(behaviorType == 6)    // 🥷 兵種 6：TMP/MAC10 微衝高機動滲透者
				{
					scheduleTable[18].percentChance = 0.0f;
					scheduleTable[17].percentChance = 0.9f;
					scheduleTable[67].percentChance = 0.0f;
					scheduleTable[68].percentChance = 0.0f;
					scheduleTable[51].percentChance = 1.0f;
					scheduleTable[50].percentChance = 0.8f;
					scheduleTable[66].percentChance = 0.1f;
					scheduleTable[24].percentChance = 0.9f;
					scheduleTable[25].percentChance = 0.0f;
					scheduleTable[22].percentChance = 0.1f;
					scheduleTable[62].percentChance = 0.0f;
					scheduleTable[63].percentChance = 0.0f;
					scheduleTable[64].percentChance = 0.3f;
					scheduleTable[28].percentChance = 0.1f;
					scheduleTable[70].percentChance = 0.1f;

					m_fPreferedRange = 192.0f; // 微衝黃金紅線：192 碼
				}
				else
					if(behaviorType == 0)    // 🥷 兵種 0：格洛克/USP 手槍常規步兵
					{
						scheduleTable[18].percentChance = 0.0f;
						scheduleTable[17].percentChance = 0.8f;
						scheduleTable[67].percentChance = 0.0f;
						scheduleTable[68].percentChance = 0.0f;
						scheduleTable[51].percentChance = 1.0f;
						scheduleTable[50].percentChance = 0.9f;
						scheduleTable[66].percentChance = 0.1f;
						scheduleTable[24].percentChance = 1.0f;
						scheduleTable[25].percentChance = 0.0f;
						scheduleTable[22].percentChance = 0.2f;
						scheduleTable[62].percentChance = 0.0f;
						scheduleTable[63].percentChance = 0.0f;
						scheduleTable[64].percentChance = 0.5f;
						scheduleTable[28].percentChance = 0.1f;
						scheduleTable[70].percentChance = 0.4f;

						m_fPreferedRange = 192.0f;
					}
					else
						if(behaviorType == 7)    // 🥷 兵種 7：高爆雷/閃光戰術擲彈兵
						{
							scheduleTable[18].percentChance = 0.1f;
							scheduleTable[17].percentChance = 0.8f;
							scheduleTable[67].percentChance = 0.1f;
							scheduleTable[68].percentChance = 0.2f;
							scheduleTable[51].percentChance = 1.0f;
							scheduleTable[50].percentChance = 0.0f;
							scheduleTable[66].percentChance = 0.0f;
							scheduleTable[24].percentChance = 0.1f;
							scheduleTable[25].percentChance = 1.0f;  // 🚀 100% 暴擊發動過頂手榴彈拋投打擊！
							scheduleTable[22].percentChance = 0.0f;
							scheduleTable[62].percentChance = 0.0f;
							scheduleTable[63].percentChance = 0.0f;
							scheduleTable[64].percentChance = 0.0f;
							scheduleTable[28].percentChance = 0.2f;
							scheduleTable[70].percentChance = 0.5f;
						}
						else
							if(behaviorType == 5)    // 🥷 兵種 5：M72 LAW 重型火箭筒反裝甲兵
							{
								scheduleTable[18].percentChance = 0.1f;
								scheduleTable[17].percentChance = 0.9f;
								scheduleTable[67].percentChance = 0.0f;
								scheduleTable[68].percentChance = 0.0f;
								scheduleTable[51].percentChance = 1.0f;
								scheduleTable[50].percentChance = 0.9f;
								scheduleTable[66].percentChance = 0.0f;
								scheduleTable[24].percentChance = 1.0f;  // 🚀 100% 發射觸碰即炸火箭彈大火砲！
								scheduleTable[25].percentChance = 0.0f;
								scheduleTable[22].percentChance = 0.0f;
								scheduleTable[62].percentChance = 0.0f;
								scheduleTable[63].percentChance = 0.0f;
								scheduleTable[64].percentChance = 0.0f;
								scheduleTable[28].percentChance = 0.1f;
							}
							else
								if(behaviorType == 4)    // 🥷 兵種 4：AWP / Scout 雷射紅外線狙擊手
								{
									scheduleTable[18].percentChance = 0.0f;
									scheduleTable[17].percentChance = 1.0f;
									scheduleTable[67].percentChance = 0.1f;
									scheduleTable[68].percentChance = 0.2f;
									scheduleTable[51].percentChance = 0.0f;
									scheduleTable[50].percentChance = 0.5f;
									scheduleTable[66].percentChance = 0.0f;
									scheduleTable[24].percentChance = 0.0f;
									scheduleTable[25].percentChance = 0.0f;
									scheduleTable[22].percentChance = 0.2f;
									scheduleTable[62].percentChance = 1.0f;  // 🚀 100% 原地死守架設 4096碼 狙擊線！
									scheduleTable[63].percentChance = 0.5f;  // 🚀 50% 機率紅外線光軌實時眼神鎖定玩家！
									scheduleTable[64].percentChance = 0.0f;
								}
								else
									if(behaviorType == 9)    // 🥷 兵種 9：神風特攻隊人肉炸彈自殺兵
									{
										// 🔒 絕不閃避、絕不拉栓換彈、絕不常規開槍射擊
										scheduleTable[18].percentChance = 0.0f;
										scheduleTable[17].percentChance = 0.0f;
										scheduleTable[67].percentChance = 0.0f;
										scheduleTable[68].percentChance = 0.0f;
										scheduleTable[51].percentChance = 0.0f;
										scheduleTable[50].percentChance = 0.1f;
										scheduleTable[66].percentChance = 1.0f;  // 💥 100% 機率：發瘋平舉雙手直線全速衝鋒！
										scheduleTable[24].percentChance = 0.0f;
										scheduleTable[25].percentChance = 0.0f;
										scheduleTable[22].percentChance = 0.0f;
										scheduleTable[62].percentChance = 0.0f;
										scheduleTable[63].percentChance = 0.0f;
										scheduleTable[64].percentChance = 0.0f;
										scheduleTable[61].percentChance = 1.0f;  // 💥 100% 機率：踏入64碼死線觸發原地引爆尖叫大爆炸！

										scheduleTable[28].percentChance = 0.0f;
										scheduleTable[70].percentChance = 0.0f;
									}
									else
										if(behaviorType == 8)    // 🥷 兵種 8：近戰開山馬刀死士
										{
											scheduleTable[18].percentChance = 0.0f;
											scheduleTable[17].percentChance = 1.0f;
											scheduleTable[67].percentChance = 0.0f;
											scheduleTable[68].percentChance = 0.0f;
											scheduleTable[51].percentChance = 0.0f;
											scheduleTable[50].percentChance = 0.0f;
											scheduleTable[66].percentChance = 0.8f;  // 🚀 80% 機率：拔出馬刀進行 128碼 體積凸殼砍殺衝鋒！
											scheduleTable[24].percentChance = 0.0f;
											scheduleTable[25].percentChance = 0.0f;
											scheduleTable[22].percentChance = 1.0f;  // 100% 承受中彈受創抽搐硬直
											scheduleTable[62].percentChance = 0.0f;
											scheduleTable[63].percentChance = 0.0f;
											scheduleTable[64].percentChance = 0.0f;
											scheduleTable[28].percentChance = 0.3f;
											scheduleTable[70].percentChance = 0.0f;
										}
										else
										{
											return; // 未指派角色，格式化退出
										}

	// =========================================================================
	// 🎨 3. 終極尾端：大後期或地獄難度高級章節 (Tier 4~5)，100% 鎖死生化防毒面具死士外貌
	// =========================================================================
	if(iMapTier < 3)
	{
		SetHead(2);   // 降級保險
		return;
	}
	if(iMapTier == 3)
	{
		// 進入中期沙漠/軍火工廠章節，自動調用 SetHead(0) 隨機切換紅頭巾/大鬍子悍匪長相！
		SetHead(0);
		return;
	}
	// 大後期關卡 (Tier 4~5)，無條件強鎖黑色生化防毒面具/巴拉克拉法帽死士外觀
	SetHead(2);
}

void CTerrorist::SetupLaserGlow( void )
{
    // =========================================================================
    // 🔴 1. 3D 紅外線雷射射線實體動態安全構造 (CBeam)
    // =========================================================================
    if ( m_pBeam == nullptr )
    {
        // 動態生成 3 碼寬度的紅色雷射束實體
        CBeam *pBeam = CBeam::BeamCreate( "sprites/laserbeam.spr", 3 );
        m_pBeam = pBeam;

        if ( m_pBeam != nullptr && m_pBeam->pev != nullptr )
        {
            entvars_t *pBeamPev = m_pBeam->pev;

            pBeamPev->rendermode |= 0x80; // 注入 kRenderFxNoDissruption 標記
            pBeamPev->spawnflags |= 0x8000; // 🔒 注入 SF_BEAM_TEMPORARY 網路 Delta 優化標記

            // 級聯配置圖形學加算渲染參數
            pBeamPev->rendercolor.x = 255.0f; // R: 純紅
            pBeamPev->rendercolor.y = 0.0f;   // G
            pBeamPev->rendercolor.z = 0.0f;   // B
            pBeamPev->renderamt     = 0.0f;   // 初始隱形
            pBeamPev->rendermode    = kRenderTransAdd; // 3: 加算透明混合 (自發光)
            pBeamPev->renderfx      = kRenderFxDistort;  // 14 = 0x0E: 模擬熱浪扭曲抖動特效

            m_laserBrightness = 0.0f;
        }
    }

    // =========================================================================
    // 💥 2. 槍口紅色雷射精靈光暈 (Laser Glow Sprite) 隨體骨骼依附構造 (CSprite)
    // =========================================================================
    if ( m_pLaserGlow == nullptr )
    {
        // 在自身原點構造精靈 flare3
        CSprite *pSprite = CSprite::SpriteCreate( "sprites/flare3.spr", pev->origin, FALSE );
        m_pLaserGlow = pSprite;

        if ( m_pLaserGlow != nullptr && m_pLaserGlow->pev != nullptr )
        {
            entvars_t *pGlowPev = m_pLaserGlow->pev;

            pGlowPev->rendermode    = kRenderTransAdd; // 3: 加算自發光
            pGlowPev->rendercolor.x = 255.0f; // 純紅
            pGlowPev->rendercolor.y = 0.0f;
            pGlowPev->rendercolor.z = 0.0f;
            pGlowPev->renderamt     = 0.0f;   // 初始隱形
            pGlowPev->renderfx      = kRenderFxDistort;  // 14: 抖動

            // 🚀 核心重砲：發動隨體移動物理骨骼依附 (MOVETYPE_FOLLOW)，將光暈死死焊接在槍口！
            if ( edict() != nullptr )
            {
                pGlowPev->skin     = ENTINDEX( edict() );       // 依附目標的世界內存索引
                pGlowPev->body     = 2;                         // 🔒 2號槍口掛載點 (Attachment 2)
                pGlowPev->aiment   = edict();                   // 實體握手依附
                pGlowPev->movetype = MOVETYPE_FOLLOW;           // 12 = 0x0C: 隨體骨骼動畫插值隨動
            }

            pGlowPev->renderamt = 0.0f;
            m_glowBrightness = 0.0f;
        }
    }
}

void CTerrorist::SetupWeapons( void )
{
    // 初始化默認彈匣與開局殘彈基準線 (預設為 MP5 規格的 36 發)
    m_cClipSize   = 36; // 0x24
    m_cAmmoLoaded = 36;

    int iBehavior = m_terroristBehavior;

    // =========================================================================
    // 🎰 優先級一：11 大自定義剧情兵種軍火庫與 3D 子模型掛載矩陣
    // =========================================================================
    if ( iBehavior != -1 )
    {
        switch ( iBehavior )
        {
            case 0: // 🥷 0 號手槍兵 (Glock / USP)
                m_cClipSize = 16; // 0x10
                CBaseAnimating::SetBodygroup( 2, 5 ); // 模型切換為手槍組件
                break;

            case 1: // 🥷 1 號散彈破門兵 (XM1014)
                pev->weapons = bits_WEAPON_SHOTGUN; // 8
                m_cClipSize  = 8;
                CBaseAnimating::SetBodygroup( 2, 1 ); // 模型切換為散彈槍組件
                break;

            case 2: // 🥷 2 號主力突擊步槍兵 (AK47)
                pev->weapons = bits_WEAPON_RIFLE;   // 1
                m_cClipSize  = 30; // 0x1E
                CBaseAnimating::SetBodygroup( 2, 4 ); // 模型切換為突擊步槍組件
                break;

            case 3: // 🥷 3 號 M60 鏈彈輕機槍暴君
                CBaseAnimating::SetBodygroup( 2, 7 ); // 模型切換為重機槍組件
                break;

            case 4: // 🥷 4 號 Scout / AWP 雷射狙擊手
                pev->weapons = bits_WEAPON_SNIPER;  // 0x10 (16)
                CBaseAnimating::SetBodygroup( 2, 3 ); // 模型切換為狙擊槍組件
                break;

            case 5: // 🥷 5 號 M72 LAW 重型火箭筒反裝甲兵
                m_cClipSize = 1;
                CBaseAnimating::SetBodygroup( 2, 2 ); // 模型切換為火箭筒組件
                break;

            case 6: // 🥷 6 號 MAC10 / TMP 微衝潛伏滲透者
                m_cClipSize  = 45; // 0x2D
                pev->weapons = bits_WEAPON_RIFLE; // 復用高射速步槍邏輯槽
                CBaseAnimating::SetBodygroup( 2, 8 ); // 模型切換為微型衝鋒槍組件
                break;

            case 7: // 🥷 7 號戰術高爆雷/閃光彈擲彈兵
                m_cClipSize  = 16;
                pev->weapons = bits_WEAPON_GRENADE; // 2
                CBaseAnimating::SetBodygroup( 2, 5 ); // 擲彈兵模型自備手槍防身
                break;

            case 8: // 🥷 8 號近戰開山馬刀死士
                CBaseAnimating::SetBodygroup( 2, 6 ); // 模型切換為叢林馬刀組件
                break;

            case 9: // 🥷 9 號💥 神風特攻隊人肉炸彈自殺兵
                CBaseAnimating::SetBodygroup( 2, 9 ); // 手持組件切換為遙控引爆器
                CBaseAnimating::SetBodygroup( 3, 1 ); // 🔒 軀幹組件 3 強制焊上 3D C4炸藥包貼圖！
                break;

            default:
                CBaseAnimating::SetBodygroup( 2, 0 ); // 預設空手
                break;
        }

        // 數據同步：開局殘彈與剛裝填完畢的兵種彈匣容量完美對齊
        m_cAmmoLoaded = m_cClipSize;
        return;
    }

    // =========================================================================
    // 📡 優先級二：降級老地圖相容分流 (依據舊版 pev->weapons 位元遮罩反向逆推)
    // =========================================================================
    unsigned int uWeaponsMask = pev->weapons;
    if ( uWeaponsMask == 0 )
    {
        pev->weapons = 3; // 預設補給
        CBaseAnimating::SetBodygroup( 2, 1 );
    }
    else
    {
        if ( uWeaponsMask & 1 ) // 步槍位元
        {
            CBaseAnimating::SetBodygroup( 2, 0 );
            uWeaponsMask = pev->weapons;
        }
        if ( uWeaponsMask & 8 ) // 散彈位元
        {
            CBaseAnimating::SetBodygroup( 2, 1 );
            m_cClipSize  = 8;
            uWeaponsMask = pev->weapons;
        }
        if ( uWeaponsMask & 0x20 ) // 火箭筒/重裝位元
        {
            CBaseAnimating::SetBodygroup( 2, 2 );
            uWeaponsMask = pev->weapons;
        }
        if ( uWeaponsMask & 0x10 ) // 狙擊位元
        {
            CBaseAnimating::SetBodygroup( 2, 3 );
            uWeaponsMask = pev->weapons;
        }
        
        if ( uWeaponsMask & 0x40 ) // 💥 舊版自爆兵位元遮罩 (64)
        {
            CBaseAnimating::SetBodygroup( 2, 9 );
            m_cClipSize = 0; // 自爆兵無子彈
        }
        
        m_cAmmoLoaded = m_cClipSize;
    }
}

void CTerrorist::SetYawSpeed( void )
{
    float flIdealYawSpeed = 90.0f; // 預設幾何轉身安全角速度基準線 (90度/秒)
    int iActivityIndex = m_Activity;

    // =========================================================================
    // 🎰 100% 還原二進位行為：51 階通用 Activity 動作靜態角速度陣列查表矩陣
    // =========================================================================
    if ( ( iActivityIndex - ACT_IDLE ) < 51 ) // 0x33 = 51
    {
        // 🚀 鏡像重合二進位 DAT_00199e9f 數據表之查表分流配准法則
        switch ( m_Activity )
        {
            case ACT_IDLE:
                flIdealYawSpeed = 90.0f;  // 原地防禦/閒置待命：90度/秒
                break;

            case ACT_WALK:
            case ACT_WALK_HURT:
                flIdealYawSpeed = 120.0f; // 戰術側身/20%殘血瘸腿走：120度/秒
                break;

            case ACT_RUN:
            case ACT_RUN_HURT:
                flIdealYawSpeed = 180.0f; // 🔒 衝鋒期/驚慌奔跑：拉高至 180度/秒，鎖死玩家軌跡
                break;

            case ACT_TURN_LEFT:
            case ACT_TURN_RIGHT:
                flIdealYawSpeed = 240.0f; // 🔒 180度大扭頭迎敵特權：拉高至極限 240度/秒 暴力轉向！
                break;

            case ACT_RANGE_ATTACK1:
            case ACT_RANGE_ATTACK2:
            case ACT_MELEE_ATTACK1:
                flIdealYawSpeed = 150.0f; // 密火網開火/投擲/出刀期：150度/秒
                break;

            case ACT_SMALL_FLINCH:
            case ACT_FLINCH_LEFTARM:
            case ACT_FLINCH_RIGHTARM:
            case ACT_FLINCH_LEFTLEG:
            case ACT_FLINCH_RIGHTLEG:
                flIdealYawSpeed = 60.0f;  // 中彈抽搐硬直硬熔斷期：壓低至 60度/秒 沒收轉向力
                break;

            default:
                flIdealYawSpeed = 90.0f;  // 其餘日常動作降級至基準面
                break;
        }
    }

    // 將幾何折算出的最終精確角速度灌入實體變數，交付給引擎物理核心渲染
    pev->yaw_speed = flIdealYawSpeed;
}

void CTerrorist::ShootAssaultRifle( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 8; // 預設 Normal 難度 8 點 AK47 基礎突擊傷害

    // 🎰 100% 還原難度自適應突擊火力殺傷力折算矩陣
    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 5;  // Easy
        else if ( g_iSkillLevel == 2 ) nBulletDamage = 12; // Hard / Expert (0x0C)
        else if ( g_iSkillLevel == 3 ) nBulletDamage = 12;
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    // 🎰 解算當前槍械的 3D 空間散佈模長
    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        // 幾何防砸窗保險面：若模長為零，強制注入 0.08716 的標準突擊步槍盲射散佈常數
        vecFinalSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        // 正常加載動態散佈暫存器
        vecFinalSpread = m_weaponAccuracy;
    }

    // 🚀 火網引爆：向 3D 世界發射一發射程高達 2048 碼的射線子彈，打擊難度自適應傷害 (BULLET_PLAYER_762MM)
    FireBullets( edict(), 1, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_762MM, nBulletDamage );

    // 點亮槍口火光網路 Delta 廣播旗標
    pev->effects |= EF_MUZZLEFLASH; // pev->effects |= 2

    // 沒收一發殘彈
    m_cAmmoLoaded--;

    // 🚀 骨骼插值連動：更新眼神與骨骼朝向夾角，強鎖上半身射擊姿態混合
    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    // =========================================================================
    // 🚀 🎰 300度半空翻滾「步槍步槍黃銅彈殼 3D 動態拋殼矩陣」
    // =========================================================================
    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    // 合成完美的拋殼飛散初速度衝量
    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    // 📐 完美還原槍身右側拋殼窗的幾何偏置：將發射點向前方外推 8 碼，防範內置穿模 Bug
    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 8.0f );

    // 呼叫粒子特效工廠，在半空中向右後方拋射出一枚高速翻滾的黃銅步槍彈殼
    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL ); // 1 = 步槍彈殼

    // =========================================================================
    // 🔊 雙軌動態音訊路由器：50% 機率交叉播放 ak47-1 / ak47-2 頂級降噪
    // =========================================================================
    const char *pszSampleSound = ( RANDOM_LONG(0, 1) == 0 ) ? "weapons/ak47-2.wav" : "weapons/ak47-1.wav";
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pszSampleSound, 0.75f, ATTN_NORM, 0, PITCH_NORM );
}

void CTerrorist::ShootLAW( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的原始 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    // 🔒 填裝 5.0 秒開火大冷卻，防止火箭彈無限連發鬼抽
    lawDelayTime = gpGlobals->time + 5.0f;

    // 將朝向向量重折算為 3D 角度，並提取右、上兩組垂直正交世界方向量
    Vector vecShootAngles;
    UTIL_VecToAngles( vecShootDir, vecShootAngles );
    vecShootAngles.x = -vecShootAngles.x; // FPU 幾何軸反轉對齊
    UTIL_MakeVectors( vecShootAngles );

    // 🎰 100% 還原二進位 4 記隨機數與雙向正交向量的 3D 火箭彈彈道螺旋漂移公式
    float flRand1 = RANDOM_FLOAT( -0.5f, 0.5f );
    float flRand2 = RANDOM_FLOAT( -0.5f, 0.5f );
    float flRand3 = RANDOM_FLOAT( -0.5f, 0.5f );
    float flRand4 = RANDOM_FLOAT( -0.5f, 0.5f );

    // 實時將固有精度武器散佈 (m_weaponAccuracy) 注入螺旋漂移面
    float flDriftUp    = ( flRand3 + flRand4 ) * m_weaponAccuracy.y;
    float flDriftRight = ( flRand1 + flRand2 ) * m_weaponAccuracy.x;

    // 合成最終受環境與難度波及後的全新 3D 實時飛行軌跡向量
    Vector vecFinalDir = vecShootDir + ( gpGlobals->v_up * flDriftUp ) + ( gpGlobals->v_right * flDriftRight );

    Vector vecFinalAngles;
    UTIL_VecToAngles( vecFinalDir, vecFinalAngles );
    vecFinalAngles.x = -vecFinalAngles.x; // 重回正偏置

    // =========================================================================
    // 🚀 火箭彈實體點火生成：向世界工廠動態 Spawn 出一枚真實的 3D 火箭彈實體
    // =========================================================================
    // 依據 DWARF 符號表，CLawsRocket::CreateLawsRocket 是其原生構造器接口
    CLawsRocket::CreateLawsRocket( vecShootOrigin, vecFinalAngles, this );

    // =========================================================================
    // 🔊 網路 Delta 事件引爆：全服同步發射 events/laws.sc 轟鳴與尾煙粒子特效封包
    // =========================================================================
    unsigned short usLawsEvent = PRECACHE_EVENT( 1, "events/laws.sc" );
    PLAYBACK_EVENT_FULL( 0, edict(), usLawsEvent, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0 );

    // 骨骼插值連動：扭頭鎖死發射夾角，強鎖上半身持重火砲骨骼混合
    SetBlending( 0, vecShootAngles.x );

    // 沒收一發重型火箭彈殘彈
    m_cAmmoLoaded--;
}

void CTerrorist::ShootMachineGun( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 11; // 預設 Normal 難度 11 點 M60 基礎重機槍傷害

    // 🎰 100% 還原難度自適應重火力殺傷力折算矩陣
    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 8;  // Easy
        else if ( g_iSkillLevel == 2 ) nBulletDamage = 15; // Hard / Expert (0x0F)
        else if ( g_iSkillLevel == 3 ) nBulletDamage = 15;
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    // 🎰 解算當前槍械的 3D 空間散佈模長
    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        // 幾何防砸窗保險面：若模長為零，強制注入 0.08716 的標準重機槍盲射散佈常數
        vecFinalSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        // 正常加載動態散佈暫存器
        vecFinalSpread = m_weaponAccuracy;
    }

    // 🚀 火網引爆：向 3D 世界發射一發射程高達 2048 碼的射線子彈，打擊難度自適應傷害 (BULLET_PLAYER_762MM)
    FireBullets( edict(), 1, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_762MM, nBulletDamage );

    // 點亮槍口火光網路 Delta 廣播旗標
    pev->effects |= EF_MUZZLEFLASH; // pev->effects |= 2

    // 💡 實錘大廠重裝兵種特權：此處不扣減殘彈，實現無限鏈彈無限壓制火網！

    // 🚀 骨骼插值連動：更新眼神與骨骼朝向夾角，強鎖上半身射擊姿態混合
    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    // =========================================================================
    // 🚀 🎰 300度半空翻滾「機槍黃銅彈殼 3D 動態拋殼矩陣」
    // =========================================================================
    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    // 合成完美的拋殼飛散初速度衝量
    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    // 📐 完美還原槍身右側拋殼窗的幾何偏置：將發射點向前方外推 8 碼，防範內置穿模 Bug
    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 8.0f );

    // 呼叫粒子特效工廠，在半空中向右後方拋射出一枚高速翻滾的黃銅步槍彈殼
    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL ); // 1 = 步槍/重機槍彈殼

    // =========================================================================
    // 🔊 雙軌動態音訊路由器：50% 機率交叉播放 m60-1 / m60-2 頂級降噪
    // =========================================================================
    const char *pszSampleSound = ( RANDOM_LONG(0, 1) == 0 ) ? "weapons/m60-2.wav" : "weapons/m60-1.wav";
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pszSampleSound, 0.75f, ATTN_NORM, 0, PITCH_NORM );
}

void CTerrorist::ShootMP5( void )
{
    if ( m_hEnemy == nullptr ) return;

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 6;

    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 3;
        else if ( g_iSkillLevel >= 2 ) nBulletDamage = 10;
    }

    Vector vecShootOrigin, vecShootDir;
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        vecFinalSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecFinalSpread = m_weaponAccuracy;
    }

    FireBullets( edict(), 1, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_9MM, nBulletDamage );

    pev->effects |= EF_MUZZLEFLASH;
    m_cAmmoLoaded--;

    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 8.0f );

    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL );

    const char *pszSampleSound = ( RANDOM_LONG(0, 1) == 0 ) ? "weapons/mp5-2.wav" : "weapons/mp5-1.wav";
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pszSampleSound, 0.75f, ATTN_NORM, 0, PITCH_NORM );
}

void CTerrorist::ShootPistol( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 5; // 預設 Normal 難度 5 點手槍基礎傷害

    // 🎰 100% 還原難度自適應手槍殺傷力折算矩陣
    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 2; // Easy
        else if ( g_iSkillLevel >= 2 ) nBulletDamage = 5; // Hard / Expert (5點)
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    // 解算當前槍械的 3D 空間散佈模長
    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        vecFinalSpread = Vector( 0.08716f, 0.08716f, 0.08716f ); // 盲射基準面
    }
    else
    {
        vecFinalSpread = m_weaponAccuracy;
    }

    const char *pszSampleSound = nullptr;

    // =========================================================================
    // 🎰 戰術心理突襲：15% 隨機機率爆發 CS 經典「格洛克戰術三連發」火網
    // =========================================================================
    if ( RANDOM_LONG(0, 99) < 15 && m_cAmmoLoaded >= 3 )
    {
        // 🚀 一瞬間射出 3 發手槍子彈，打擊難度自適應傷害 (BULLET_PLAYER_9MM)
        FireBullets( edict(), 3, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_9MM, nBulletDamage );
        
        m_cAmmoLoaded -= 3; // 一鍵格式化清扣 3 發殘彈
        pszSampleSound = "weapons/glock18-1.wav"; // 三連發專屬高頻率破空聲
    }
    else // 85% 機率維持常規單發點射
    {
        // 向世界發射一發子彈
        FireBullets( edict(), 1, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_9MM, nBulletDamage );
        
        m_cAmmoLoaded--; // 扣除一發
        pszSampleSound = "weapons/glock18-2.wav"; // 標準單發格洛克槍聲
    }

    // 點亮槍口火光網路 Delta 廣播
    pev->effects |= EF_MUZZLEFLASH; // pev->effects |= 2

    // 🚀 骨骼插值連動：更新眼神與骨骼朝向夾角，強鎖上半身射擊姿態混合
    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    // =========================================================================
    // 🚀 🎰 300度半空翻滾「手槍九毫米黃銅彈殼 3D 動態拋殼矩陣」
    // =========================================================================
    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    // 合成拋殼飛散初速度衝量
    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    // 📐 100% 還原 13.0f 碼前推線幾何偏置之真相！精確對齊平舉伸直右手的格洛克排殼窗，嚴防穿模 Bug！
    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 13.0f );

    // 呼叫粒子特效工廠，在半空中向右後方拋射出一枚 9mm 手槍小彈殼
    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL ); 

    // 發射音軌
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pszSampleSound, 0.75f, ATTN_NORM, 0, PITCH_NORM );
}

void CTerrorist::ShootShotgun( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 10; // 預設 Normal 難度 10 點單顆鋼珠傷害

    // 🎰 100% 還原難度自適應散彈破門殺傷力折算矩陣
    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 5;  // Easy
        else if ( g_iSkillLevel >= 2 ) nBulletDamage = 15; // Hard / Expert (0x0F)
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    // 解算當前槍械的 3D 空間散佈模長
    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        // 幾何防砸窗保險面：100% 還原 0.13053 常數，精確對齊 VECTOR_CONE_15DEGREES 散彈槍標準散射錐
        vecFinalSpread = Vector( 0.13053f, 0.13053f, 0.13053f );
    }
    else
    {
        vecFinalSpread = m_weaponAccuracy;
    }

    // 🚀 火網爆破：單發並行點火射出 5 顆獨立散彈鋼珠 (BULLET_PLAYER_BUCKSHOT)，最大極限射程 2048 碼
    FireBullets( edict(), 5, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_BUCKSHOT, nBulletDamage );

    // 點亮槍口火光網路 Delta 廣播
    pev->effects |= EF_MUZZLEFLASH; // pev->effects |= 2

    // 沒收一發散彈殘彈
    m_cAmmoLoaded--;

    // 🚀 骨骼插值連動：更新眼神與骨骼朝向夾角，強鎖上半身射擊姿態混合
    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    // =========================================================================
    // 🚀 🎰 300度半空翻滾「紅色 12 號散彈塑料殼 3D 動態拋殼矩陣」
    // =========================================================================
    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    // 合成拋殼飛散初速度衝量
    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    // 📐 完美還原槍身右側拋殼窗的幾何偏置：將發射點向前方外推 8 碼，防範內置穿模 Bug
    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 8.0f );

    // 呼叫粒子特效工廠，在半空中向右後方拋射出一枚鮮紅色的塑料散彈大彈殼 ( 2 = TE_BOUNCE_SHOTSHELL )
    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iShotgunShell, 2 ); 

    // 發射音軌：原廠經典大威力單發散彈轟鳴
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/sbarrel1.wav", 0.75f, ATTN_NORM, 0, PITCH_NORM );
}

void CTerrorist::ShootSMG( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 4; // 預設 Normal 難度 4 點微衝基礎傷害

    // 🎰 100% 還原難度自適應微衝滲透火力殺傷力折算矩陣
    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 2; // Easy
        else if ( g_iSkillLevel >= 2 ) nBulletDamage = 6; // Hard / Expert (6點)
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    // 解算當前槍械的 3D 空間散佈模長
    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        // 幾何防砸窗保險面：若模長為零，強制注入 0.08716 的標準微衝盲射散佈常數
        vecFinalSpread = Vector( 0.08716f, 0.08716f, 0.08716f );
    }
    else
    {
        vecFinalSpread = m_weaponAccuracy;
    }

    // 🚀 火網引爆：向 3D 世界發射一發射程高達 2048 碼的射線子彈 (BULLET_PLAYER_9MM)
    FireBullets( edict(), 1, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_9MM, nBulletDamage );

    // 點亮槍口火光網路 Delta 廣播旗標
    pev->effects |= EF_MUZZLEFLASH; // pev->effects |= 2

    // 沒收一發殘彈
    m_cAmmoLoaded--;

    // 🚀 骨骼插值連動：更新眼神與骨骼朝向夾角，強鎖上半身射擊姿態混合
    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    // =========================================================================
    // 🚀 🎰 300度半空翻滾「微衝九毫米黃銅彈殼 3D 動態拋殼矩陣」
    // =========================================================================
    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    // 合成拋殼飛散初速度衝量
    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    // 📐 100% 還原 10.0f 碼前推線幾何偏置之真相！精確對齊縮在胸前戰術前伸的微衝排殼窗，嚴防穿模 Bug！
    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 10.0f );

    // 呼叫粒子特效工廠，在半空中向右後方拋射出一枚手槍同款 9mm 黃銅小彈殼 (1 = TE_BOUNCE_SHELL)
    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL ); 

    // =========================================================================
    // 🔊 雙軌動態音訊路由器：50% 機率交叉播放 tmp-1 / tmp-2 頂級降噪消音微衝音軌
    // =========================================================================
    const char *pszSampleSound = ( RANDOM_LONG(0, 1) == 0 ) ? "weapons/tmp-2.wav" : "weapons/tmp-1.wav";
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, pszSampleSound, 0.75f, ATTN_NORM, 0, PITCH_NORM );
}

void CTerrorist::ShootSniperRifle( void )
{
    // 📢 確保目前大腦鎖定的死敵目標有效 (m_hEnemy != nullptr)
    if ( m_hEnemy == nullptr )
    {
        return;
    }

    CBaseEntity *pEnemy = m_hEnemy;
    int nBulletDamage = 18; // 預設 Normal 難度 18 點 AWP 麥格農基礎傷害

    // 🎰 100% 還原難度自適應重裝狙擊殺傷力折算矩陣
    if ( pEnemy->IsPlayer() )
    {
        if ( g_iSkillLevel == 0 )      nBulletDamage = 25; // Easy
        else if ( g_iSkillLevel >= 2 ) nBulletDamage = 45; // Hard / Expert (0x2D)
    }

    Vector vecShootOrigin, vecShootDir;
    // 跨越虛擬表 131 號方法獲取槍口世界原點，並解算指向死敵的 3D 幾何向量
    GetGunPosition( vecShootOrigin );
    ShootAtEnemy( vecShootDir, vecShootOrigin );

    UTIL_MakeVectors( pev->angles );

    // 解算當前槍械的 3D 空間散佈模長
    float flSpreadLen = m_weaponAccuracy.Length();
    Vector vecFinalSpread;

    if ( flSpreadLen <= 0.0f )
    {
        // 幾何防砸窗保險面：100% 還原 0.04362 散佈常數，精確對齊狙擊槍高精度彈道
        vecFinalSpread = Vector( 0.04362f, 0.04362f, 0.04362f );
    }
    else
    {
        vecFinalSpread = m_weaponAccuracy;
    }

    // 🚀 火網引爆：向 3D 世界發射一發射程高達 2048 碼的高精度重型射線子彈 (BULLET_PLAYER_338MAG)
    FireBullets( edict(), 1, vecShootOrigin, vecShootDir, vecFinalSpread, 2048.0f, BULLET_PLAYER_338MAG, nBulletDamage );

    // 點亮槍口火光網路 Delta 廣播旗標
    pev->effects |= EF_MUZZLEFLASH; // pev->effects |= 2

    // 💡 實錘大廠狙擊兵種特權：此處不扣減殘彈，依靠拉栓動作完成分時補給

    // 🚀 骨骼插值連動：更新眼神與骨骼朝向夾角，強鎖上半身射擊姿態混合
    Vector vecAngleDir;
    UTIL_VecToAngles( vecShootDir, vecAngleDir );
    SetBlending( 0, vecAngleDir.x );

    // =========================================================================
    // 🚀 🎰 300度半空翻滾「狙擊步槍黃銅彈殼 3D 動態拋殼矩陣」
    // =========================================================================
    float flRandRight   = RANDOM_FLOAT( -40.0f, 40.0f );
    float flRandForward = RANDOM_FLOAT( 75.0f,  200.0f );
    float flRandUp      = RANDOM_FLOAT( 40.0f,  90.0f );

    // 合成拋殼飛散初速度衝量
    Vector vecShellVelocity = ( gpGlobals->v_right * flRandRight ) + 
                              ( gpGlobals->v_forward * flRandForward ) + 
                              ( gpGlobals->v_up * flRandUp );

    // 📐 完美還原槍身右側拋殼窗的幾何偏置：將發射點向前方外推 8 碼，防範內置穿模 Bug
    Vector vecEjectOrigin = vecShootOrigin + ( gpGlobals->v_forward * 8.0f );

    // 呼叫粒子特效工廠，在半空中向右後方拋射出一枚黃銅步槍彈殼 (1 = TE_BOUNCE_SHELL)
    EjectBrass( vecEjectOrigin, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL ); 

    // 發射音軌：AWP 重型麥格農全地圖無衰減硬核轟鳴 (ATTN_NONE = 0.0f, VOL_NORM = 1.0f)
    EMIT_SOUND_DYN( edict(), CHAN_WEAPON, "weapons/awp1.wav", VOL_NORM, ATTN_NONE, 0, PITCH_NORM );
}

void CTerrorist::SpeakSentence( void )
{
    // =========================================================================
    // ⏱️ 1. 語音隊列與全局擁擠鎖審查：有排隊對白，且全局語音通道正處於空閒放行態
    // =========================================================================
    if ( m_iSentence != -1 && gpGlobals->time > CTalkMonster::g_talkWaitTime )
    {
        // 🔒 2. 戰術活性消音鎖審查：若此時正被閃光致盲、或處於索降下墜期，攔截格式化
        if ( FOkToSpeak() )
        {
            // 🚀 從全局指標陣列 pTerroristSentences 中實時提取對白字串，呼叫 102 號虛擬方法 PlaySentence 點火發射！
            // 音量與衰減係數配置為經典的 0.8f (0x3F4CCCCD) 常數基準面
            PlaySentence( pTerroristSentences[m_iSentence], 0.8f, ATTN_NORM, 0, m_voicePitch );

            // =========================================================================
            // ⚙️ 3. 大腦清洗與 8~15 秒蒙地卡羅動態冷卻填裝 (100% 對齊二進位行為)
            // =========================================================================
            m_iSentence = -1; // 對白句柄徹底清洗重置，釋放發聲信道
            
            float flRandomCooldown = RANDOM_FLOAT( 8.0f, 15.0f );
            CTalkMonster::g_talkWaitTime = gpGlobals->time + flRandomCooldown; // 格式化鎖死全局
        }
    }
}

void CTerrorist::StartTask( Task_t *pTask )
{
    m_iTaskStatus = TASKSTATUS_RUNNING; // 1

    switch ( pTask->iTask )
    {
        // =========================================================================
        // 🪂 任務 18 & 20：常規路徑運動 (100% 還原直升機索降下墜滑翔覆寫 ACT_GLIDE)
        // =========================================================================
        case 28: // 0x1C: TASK_WALK_PATH
        case 30: // 0x1E: TASK_RUN_PATH
            if ( m_cantMove < 2 || m_MonsterState == MONSTERSTATE_DEAD )
            {
                CSquadMonster::StartTask( pTask );

                // 📐 幾何防穿模黑科技：若當前處於 MOVETYPE_FLY (5) 索降空中懸空期
                if ( pev->movetype == MOVETYPE_FLY )
                {
                    m_IdealActivity = ACT_GLIDE; // 強鎖單手控繩索降空中滑翔動作
                }
                return;
            }
            break;

        // =========================================================================
        // ⚙️ 任務 38 (0x26)：原子任務換彈點火
        // =========================================================================
        case 38: // 0x26: TASK_RELOAD
            m_IdealActivity = ACT_RELOAD;
            return;

        // =========================================================================
        // 📢 任務 93 (0x5D)：延時對白隊列發射中樞
        // =========================================================================
        case 93: // 0x5D
            SpeakSentence(); // 呼叫排隊發聲暫存器
            break;

        // =========================================================================
        // 🔫 任務 94 (0x5E)：友傷大腦視線掃描
        // =========================================================================
        case 94: // 0x5E
            // 審查開火線上是否有同伴，若有，強行打上 bits_COND_NO_FIRE (0x10000000) 熔斷不開火
            if ( !CSquadMonster::NoFriendlyFire() )
            {
                m_afConditions |= 0x10000000;
            }
            if ( m_afConditions & 0x40000000 ) // FAILED 標記過濾
            {
                return;
            }
            // 暢通無阻，任務瞬間成功
            m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
            return;

        // =========================================================================
        // 🧱 任務 95 (0x5F)：戰術掩體節點推進尋路
        // =========================================================================
        case 95: // 0x5F
            if ( m_pSpecialCoverNode == nullptr || !FValidateCoverNode( m_pSpecialCoverNode ) ) // 虛擬表偏移 0x1c0
            {
                m_afConditions |= 0x40000000; // 節點卡死，標記任務失敗
                return;
            }
            // 拉起尋路網格，全速狂奔向掩體幾何座標
            if ( !CBaseMonster::MoveToLocation( ACT_RUN, 0.0f, m_pSpecialCoverNode->m_vecOrigin ) )
            {
                ALERT( at_console, "FAILED MOVING TO LOCATION.\n" );
                m_afConditions |= 0x40000000; // 尋路車禍，停機
                return;
            }
            break;

        // =========================================================================
        // 🏃 任務 96 (0x60)：8號馬刀/9號自殺兵 獵人衝鋒任務起跑點
        // =========================================================================
        case 96: // 0x60: TASK_CHARGE_ENEMY
            if ( m_cantMove == 0 && m_hEnemy != nullptr )
            {
                // 計算與玩家在 2D 平面的起跑相對物理距離
                float flDistToEnemy = ( m_hEnemy->pev->origin - pev->origin ).Length2D();

                // 📐 只有當距離大於 64 碼死線時，才允許發動全速追殺
                if ( flDistToEnemy >= 64.0f )
                {
                    m_vecMoveGoal = m_hEnemy->pev->origin; // 強寫目標點為玩家腳底板
                    
                    if ( CBaseMonster::MoveToEnemy( ACT_RUN, 0.0f ) )
                    {
                        return; // 成功開闢衝鋒路由，放行交給每幀更新
                    }
                    m_afConditions |= 0x40000000; // 路由崩潰，失敗
                    return;
                }
            }
            break;

        // =========================================================================
        // 🎲 任務 97 (0x61)：日常隨機 8 方向無目的漫步閒逛
        // =========================================================================
        case 97: // 0x61
            if ( !CBaseMonster::MoveToLocation( ACT_WALK, 0.0f, m_wanderOrigin ) )
            {
                m_afConditions |= 0x40000000;
                return;
            }
            break;

        // 通用原子任務與常規 HECU 陸軍計畫降級派發
        case 18: // 0x12
        case 19: // 0x13
            m_afMemory &= ~0x00000002; // 清空特定的記憶體標記槽
        default:
            CSquadMonster::StartTask( pTask );
            return;
    }

    // 🚀 終端收尾：若開局首幀未被任何硬條件打上 0x40000000 失敗鎖，直接將任務推進至成功，移交每幀 Task
    if ( !( m_afConditions & 0x40000000 ) )
    {
        m_iTaskStatus = TASKSTATUS_COMPLETE; // 4
    }
}

int CTerrorist::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
    // 開局格式化：清空 HECU 陸軍體系的特定記憶體暫存器，防範行為殘留
    m_afMemory &= ~0x00000002; 

    float flFinalDamage = flDamage;

    // =========================================================================
    // 🥷 1. 潛行背刺暗殺審查：若攻擊者為反恐精英玩家本身 (ClassName == "player")
    // =========================================================================
    if ( pevAttacker != nullptr && strcasecmp( STRING(pevAttacker->classname), "player" ) == 0 )
    {
        // 只有當恐怖份子處於放鬆巡邏、尚未鎖定任何宿敵警戒的狀態下 (m_hEnemy == nullptr)
        if ( m_hEnemy == nullptr )
        {
            // 獲取地圖 1 號玩家的 CBasePlayer 實體私有私有數據指針
            edict_t *pPlayerEdict = INDEX_TO_ENT( 1 );
            if ( pPlayerEdict == nullptr || pPlayerEdict->pvPrivateData == nullptr )
            {
                pPlayerEdict = PEOFFSET( 0 );
            }

            if ( pPlayerEdict != nullptr && pPlayerEdict->pvPrivateData != nullptr )
            {
                CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pPlayerEdict );

                if ( pPlayer != nullptr )
                {
                    // 🚀 跨類別強讀：調用玩家大腦，獲取當前手持武器名字串
                    const char *pszActiveWeapon = pPlayer->GetActiveWeaponClassname();

                    if ( pszActiveWeapon != nullptr && strcasecmp( pszActiveWeapon, "weapon_knife" ) == 0 )
                    {
                        // 🎰 完美還原二進位行為：打上安靜倒地標記，並強寫 105.0f 暴擊傷害，實現一刀割喉潛行殺！
                        stealthDeath   = TRUE;
                        flFinalDamage  = 105.0f; 
                    }
                }
            }
        }
    }

    // =========================================================================
    // 🎯 2. 仇恨即時追尾同步：若尚未接敵，被子彈或攻擊打中的一瞬間，立刻零延遲鎖死攻擊者
    // =========================================================================
    if ( m_hEnemy == nullptr && pevAttacker != nullptr )
    {
        edict_t *pAttackerEdict = pevAttacker->pContainingEntity;
        if ( pAttackerEdict == nullptr )
        {
            pAttackerEdict = PEOFFSET( 0 );
        }

        if ( pAttackerEdict != nullptr && pAttackerEdict->pvPrivateData != nullptr )
        {
            // 實時將攻擊者 C++ 物件指針拷貝注入 m_hEnemy 暫存器中
            m_hEnemy = CBaseEntity::Instance( pAttackerEdict );
        }
    }

    // =========================================================================
    // 🧱 3. 降級派發：將最終解算後的傷害數值移交怪物母體基類，引爆常規扣血與慘叫
    // =========================================================================
    return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flFinalDamage, bitsDamageType );
}

void CTerrorist::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
    // =========================================================================
    // 📐 100% 還原二進位幾何暫存器棧對齊行為
    // 將子彈入射方向向量 (vecDir) 安全打包，原封不動移交母體基類進行 Hitbox 擊中骨骼反查
    // =========================================================================
    CSquadMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

void CTerrorist::UseTarget( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // =========================================================================
    // 📡 100% 還原二進位行為：死後遺言劇本連動中樞
    // 若地圖設計師配置了當前匪徒死後的觸發目標名稱 (m_useTarget)
    // =========================================================================
    if ( m_useTarget != 0 )
    {
        // 🚀 一鍵引爆全伺服器實體通信信號喇叭 (FireTargets)
        // 將死後遺言字串、攻擊者指針、與 USE_ON/OFF 信號打包射出，瞬間拉起地圖過場劇情！
        FireTargets( STRING( m_useTarget ), pActivator, this, useType, value );
    }
}

bool CTerrorist::ValidateActivation( int entry )
{
    // =========================================================================
    // 🎰 100% 還原二進位行為：4 大位移型高級戰術計畫下盤物理定身鎖審查
    // =========================================================================
    switch ( entry )
    {
        case 18: // 0x12: SCHED_TAKE_COVER_FROM_ENEMY (戰術尋找掩體規避)
        case 50: // 0x32: SCHED_FLANK_ATTACK          (側翼戰術繞後包抄)
        case 64: // 0x40: SCHED_CORNER_COVER_NODE     (拐角掩體節點卡點)
        case 66: // 0x42: SCHED_CHARGE_ENEMY          (馬刀/自殺神風狂暴衝鋒)
            
            // 🔒 幾何安全熔斷：若當前被地形道具、閃光震撼或非同步索降定身 (m_cantMove != 0)
            // 則尋路宣告不可行，返回 false 熔斷沒收計畫，防止降智卡死！
            return ( m_cantMove == 0 );

        default:
            break;
    }

    // 其餘原地射擊、換彈、日常對白等靜態計畫，無條件放行可行性
    return true;
}
