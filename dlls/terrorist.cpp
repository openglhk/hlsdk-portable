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

// 10 通道權威存檔數據表 (3個整數 + 7個ushort網路事件暫存器 = 10)
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
	DEFINE_FIELD( CTerrorist, m_fThrowGrenade, FIELD_BOOLEAN ),
    DEFINE_FIELD( CTerrorist, m_vecTossVelocity, FIELD_VECTOR ),
};

int g_fTerroristQuestion = 0;

CTerrorist::CTerrorist( void )
{
    // 1. 優先手動拉起母體小隊怪物類別的構造工廠，清洗繼承內存
    CBaseMonster::CBaseMonster();

    // 2. 初始化核心運動狀態機與兵種特權角色
    m_bStanding        = TRUE;
    m_terroristType    = 0;
    m_terroristBehavior = -1; // 預設常規未指派

    // 特種限制投擲兵種控制
    m_flashbangOnly    = FALSE;
    m_heGrenadeOnly    = FALSE;

    // 3. 🚀 人肉炸彈自爆兵種核心幾何常數填裝
    m_blastRadius      = 256.0f; // 256 碼巨額爆炸半徑
    m_blastDamage      = 200.0f; // 200 點致命衝擊波傷害
    suicideFlag        = FALSE;
    taskFailCount      = 0.0f;

    // 4. 戰術重型 LAW 火箭筒射擊延時與開火旗標清零
    lawDelayTime       = 0.0f;
    justShotFlag       = FALSE;

    // 5. 潛行暗殺安靜旗標、256碼小隊警戒紅線、以及 64碼黃金交火距離配置
    m_canWander        = FALSE;
    stealthDeath       = TRUE;  // 允許潛行暗殺不觸發全局警報
    m_alertRadius      = 256.0f;
    m_fPreferedRange   = 64.0f;
    heardCombat        = FALSE;

    // =========================================================================
    // ⚔️ 6. 陣營主權對齊：100% 鏡像重合二進位常數 0x0E (十進位 14)
    // 正式劃入魔改版專屬的 CLASS_MILITARY 敵軍軍事惡棍陣營！
    // =========================================================================
    m_classtype        = 14; 

    // =========================================================================
    // 🎰 7. 隨機漫步時鐘交叉錯落疊加 (5.0f ~ 10.0f 秒)
    // 確保多名敵軍出生時閒逛尋路計算 perfect 錯開，平滑保護伺服器 FPS 幀率
    // =========================================================================
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




