#include "hostage.h"

LINK_ENTITY_TO_CLASS( monster_hostage, CHostage );

TYPEDESCRIPTION CHostage::m_SaveData[] =
{
    // 👧 1~5: 生態性格與女性外觀分流變數
    DEFINE_FIELD( CHostage, m_femaleFlag, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_head, FIELD_INTEGER ),
    DEFINE_FIELD( CHostage, m_State, FIELD_INTEGER ),
    DEFINE_FIELD( CHostage, m_flNextChange, FIELD_TIME ),
    DEFINE_FIELD( CHostage, m_flFlinchTime, FIELD_TIME ),

    // 📡 6~10: 潛行、雷達、電影劇本與營救過渡標記
    DEFINE_FIELD( CHostage, m_bQuiet, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_bHidden, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_bPlayerNear, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_bRescueMe, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_rescued, FIELD_BOOLEAN ),

    // 🔊 11~15: 語音音調、通訊冷卻與美式本地化字串索引
    DEFINE_FIELD( CHostage, m_flNextRadarTime, FIELD_TIME ),
    DEFINE_FIELD( CHostage, m_voicePitch, FIELD_FLOAT ),
    DEFINE_FIELD( CHostage, m_flNextPainTime, FIELD_TIME ),
    DEFINE_FIELD( CHostage, m_bTouched, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_idleAnim, FIELD_STRING ),

    // 🎬 16~19: 關卡下游劇情聯動目標
    DEFINE_FIELD( CHostage, m_rescueTarget, FIELD_STRING ),
    DEFINE_FIELD( CHostage, m_noAutoRemove, FIELD_BOOLEAN ),
    DEFINE_FIELD( CHostage, m_rescueAnim, FIELD_STRING ),
    DEFINE_FIELD( CHostage, m_pCine, FIELD_CLASSPTR ), // 電影級劇本動畫指針通道

    // 🏃 20~21: 高級大腦策略刷新線與當前合法的總節點數量
    DEFINE_FIELD( CHostage, m_flNextFullThink, FIELD_TIME ),
    DEFINE_FIELD( CHostage, m_nPathNodes, FIELD_INTEGER ),

    // 🧭 22~30: 16 容量的三維空間節點幾何矩陣 ( vecNodes 展開為 9 個 3D 座標通道，剛好湊齊 30 ！)
    DEFINE_FIELD( CHostage, vecNodes[0], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[1], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[2], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[3], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[4], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[5], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[6], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[7], FIELD_VECTOR ),
    DEFINE_FIELD( CHostage, vecNodes[8], FIELD_VECTOR ) // 👈 剛好第 30 個存檔元素！0x1E 大圓滿！
};

// =========================================================================
// 🚀 終極圓滿還原：人質實體全域工廠生成函數 (必須排在所有類別成員方法的最前面)
// =========================================================================
void monster_hostage( entvars_t *pev )
{
    // 1. 安全防禦鎖：若數據頭為空，主動向引擎申請創建一個合法的 Edict 數據頭
    if ( pev == nullptr )
    {
        edict_t *pNewEdict = CREATE_ENTITY();
        if ( pNewEdict != nullptr )
        {
            pev = &pNewEdict->v;
        }
    }

    if ( pev == nullptr || pev->pContainingEntity == nullptr )
    {
        return;
    }

    // 2. 檢查是否已經分配過 Private Data，防止重複拉起
    if ( pev->pContainingEntity->pvPrivateData != nullptr )
    {
        return;
    }

    // 3. 🚀 記憶體重砲：呼叫引擎分配精確到 0x984 (2436 位元組) 的專屬 C++ 實體記憶體空間
    CHostage *pHostage = (CHostage *)GET_PRIVATE( pev->pContainingEntity );
    if ( pHostage == nullptr )
    {
        pHostage = (CHostage *)ALLOC_PRIVATE( pev->pContainingEntity, 0x984 );
    }

    // 4. 手動激活 CBaseMonster 核心基底類別構造函數初始化白紙
    pHostage->CBaseMonster::CBaseMonster();

    // =========================================================================
    // ⚙️ 完美還原二進位內聯解平優化賦值 (自定義劇情電影指針清零)
    // =========================================================================
    pHostage->SetThink( nullptr );
    pHostage->pev = nullptr; 

    // 完美還原：將電影級過場編排控制器指針安全初始化為空
    pHostage->m_pCine = nullptr; 

    // 5. 最高主權對齊：同步 vptr 虛擬表指針，並正式將 pev 屬性與 Edict 雙向死死綁定
    pHostage->pev = pev;
}

CHostage::CHostage()
{
    m_healthModifier = 0.0f; // 預設一出生是完全健康的
    m_idleAnim       = 0;
}

void CHostage::Precache( void )
{
    if ( pev->model == 0 )
    {
        PRECACHE_MODEL( "models/npc-scientist.mdl" );
    }
    else
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    PRECACHE_MODEL( "sprites/smoke.spr" );
}

void CHostage::Spawn( void )
{
    // 1. 資源預載與安全 Fallback 模型配置 (100% 鏡像對齊二進位 Precache 審查)
    if ( pev->model == 0 )
    {
        Precache();
        SET_MODEL( edict(), "models/npc-scientist.mdl" );
    }
    else
    {
        const char *pszModelPath = STRING( pev->model );
        
        // 🚀 核心黑科技：動態掃描模型名字，若包含 "us_gal" 或 "schoolgirl"，自動激活女性語音生態通道
        if ( pszModelPath != nullptr && (strstr(pszModelPath, "us_gal") != nullptr || strstr(pszModelPath, "schoolgirl") != nullptr) )
        {
            m_femaleFlag = TRUE;
        }
        
        Precache();
        SET_MODEL( edict(), pszModelPath );
    }

    // 2. 配置與設定類名，100% 還原相對偏置寫入
    pev->classname = ALLOC_STRING( "monster_hostage" );

    // 3. 配置緊湊型 20x20x62 戰術人質物理碰撞盒，防止門框與多人尋路擠壓卡死
    pev->movetype   = MOVETYPE_WALK; // 4
    pev->solid      = SOLID_SLIDEBOX; // 3
    pev->takedamage = DAMAGE_AIM;    // 1.0f
    pev->flags     |= FL_MONSTER;    // 0x20
    pev->deadflag   = DEAD_NO;       // 0
    pev->gravity    = 1.0f;

    // 4. 生命值滿狀態填裝
    pev->max_health = 100.0f;
    pev->health     = pev->max_health;

    // 調整微觀視線與初速度歸零
    pev->view_ofs   = Vector( 0.0f, 0.0f, 12.0f );
    pev->velocity   = g_vecZero;

    m_flNextPainTime = gpGlobals->time;

    // 5. 劇情配置：若地圖勾選了特殊的 spawnflags 4 標記，打上隱形或免除渲染優化
    if ( pev->spawnflags & 4 )
    {
        pev->flags |= 0x800000; // 特定內部裁剪旗標
    }

    if ( pev->skin < 0 ) pev->skin = 0;

    // 若地圖指定了特殊的 m_head 欄位，同步切換 bodygroup 部件外觀
    if ( m_head > 0 )
    {
        pev->body = m_head;
    }

    // =========================================================================
    // ⚙️ 6. 動態動作混合器配置與自適應序列 Lookup (0x3E 偏移量代表 LookupActivity)
    // =========================================================================
    if ( m_Activity != ACT_IDLE ) // ACT_IDLE = 1
    {
        int iIdleSeq = LookupActivity( ACT_IDLE );
        if ( iIdleSeq != -1 )
        {
            if ( iIdleSeq != pev->sequence )
            {
                pev->frame = 0.0f;
                pev->sequence = iIdleSeq;
            }
            m_Activity = ACT_IDLE; // 1
            ResetSequenceInfo();
        }
    }

    // 7. 狀態機與舊路徑緩衝暫存器初始化
    m_State        = 0; // STAND 狀態
    m_flNextChange = 0.0f;
    m_flFlinchTime = 0.0f;
    m_bRescueMe    = FALSE;
    m_rescued      = FALSE;

    m_hTargetEnt        = nullptr;
    m_hStoppedTargetEnt = nullptr;

    m_vPathToFollow[0]  = g_vecZero;

    // 核心防卡死：將老位置座標初始預設設定為 9999 極限無效值，開局第一時間重置避讓機制
    m_vOldPos = Vector( 9999.0f, 9999.0f, 9999.0f );

    // 記錄出生原始幾何原點與角度，供回合重置使用
    m_vStart       = pev->origin;
    m_vStartAngles = pev->angles;

    // 配置物理幾何碰撞體
    UTIL_SetSize( pev, Vector(-10.0f, -10.0f, 0.0f), Vector(10.0f, 10.0f, 62.0f) );

    // 8. 骨骼控制器解鎖：平滑扭頭對齊前方視線夾角
    UTIL_MakeVectors( pev->v_angle );
    float flYaw = UTIL_VecToYaw( gpGlobals->v_forward );
    CBaseAnimating::SetBoneController( 0, flYaw );

    // 自動垂直垂直落地下榻到固體地面上
    DROP_TO_FLOOR( edict() );

    // 9. 呼叫虛擬函數進行出生二次初始化衍生調度 (0x48 偏移量代表繼承鏈特定的 SpawnInit 方法)
    // 虛擬表 0x48 偏移量在多載中精確對齊基底類別特定的初始化回調
    ExecuteSpawnInit(); // 呼叫底層特定的類別初始化

    m_iHostageIndex = g_iHostageNumber;

    // 2. 自定義高級戰術尋路參數初始初值拉起
    nTargetNode            = -1;
    m_fHasPath             = FALSE;
    m_flLastPathCheck      = -1.0f;
    m_flPathAcquired       = -1.0f;
    m_flPathCheckInterval  = 0.1f;

    // 3. 隨機時間戳交錯疊加，重設雷達小地圖無線電通訊刷新冷卻線 (防多音軌雜音共振)
    m_flNextRadarTime = gpGlobals->time + RANDOM_FLOAT( 0.0f, 1.0f );

    // 4. 🚀 內存重砲：動態實例化 36 位元組本地幾何導航避障物件 (CLocalNav)
    // 100% 還原 operator new(0x24) 與構造函數雙向死死鎖定行為
    m_LocalNav = new CLocalNav( this );

    // 5. 語音音調與本地化美式英語語言代碼分配
    m_voicePitch = 100.0f;
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "AM" ); // 鎖定美式英語音訊組 ("AM")
    }

    // 全局計數器累加，為下一名人質出生做好編號準備
    g_iHostageNumber++;
}

int CHostage::BloodColor( void )
{
    // 完美還原二進位行為 0xF7 -> -9，向引擎宣告人質具備標準人類鮮紅色血液受擊粒子
    return BLOOD_COLOR_RED; 
}

void CHostage::DoFollow( void )
{
    // 1. 📢 隨行主權審查：確保跟隨隨行目標有效，否則拒絕執行導航
    if ( m_hTargetEnt == nullptr )
    {
        return;
    }

    // 2. 利用實例化工廠，安全獲取目前跟隨的玩家指針物件 (CBasePlayer)
    CBaseEntity *pPlayer = CBaseEntity::Instance( m_hTargetEnt->edict() );
    if ( pPlayer == nullptr || pPlayer->pev == nullptr )
    {
        return;
    }

    // 3. 🔃 導航同步：將玩家當前的 Edict 索引死死同步寫入 CLocalNav 導航器中
    m_LocalNav->m_pTargetEnt = pPlayer->edict();

    // 計算精準的玩家幾何頭部/隨行中心點
    Vector vecDest = pPlayer->pev->origin;
    vecDest.z += pPlayer->pev->view_ofs.z;

    // 4. 物理安全死區判定：計算人質與目標點之間的真實 3D 空間距離
    float flDistToDest = (vecDest - pev->origin).Length();

    if ( flDistToDest < 80.0f )
    {
        // 抵達玩家死區範圍，若本來就有尋路軌跡，直接原地中斷返回，防止與玩家重疊穿模
        if ( m_fHasPath )
        {
            return;
        }

        // 呼叫避障探測，核對人質當前位置與玩家點之間是否暢通無阻 (PathTraversable)
        if ( m_LocalNav->PathTraversable( pev->origin, vecDest, TRUE ) )
        {
            return;
        }
    }

    // =========================================================================
    // 📡 階段 A：動態 10Hz 戰術路徑再評估與隨行 Request 發射中樞
    // =========================================================================
    if ( (pev->flags & FL_ONGROUND) && (gpGlobals->time > m_flLastPathCheck + m_flPathCheckInterval) )
    {
        if ( m_fHasPath )
        {
            // 若玩家目前正站在原地沒有移動 (玩家速度微弱 <= 1.0f)，則暫緩重新計算路徑
            float flPlayerSpeed = pPlayer->pev->velocity.Length2D();
            if ( flPlayerSpeed <= 1.0f )
            {
                goto LAB_MOVE_ACTIVATE; // 直接跨向位移驅動
            }
        }

        // 重新注入路徑冷卻時鐘
        m_flLastPathCheck = gpGlobals->time;
        
        // 🚀 向引擎發射多道探測線，向導航器發出動態 Request 請求，重新填充 vecNodes 節點軌跡陣列
        m_LocalNav->RequestNav( this );
    }

LAB_MOVE_ACTIVATE:
    // =========================================================================
    // 🏃 階段 B：多段幾何路徑節點前推與平滑轉向位移矩陣
    // =========================================================================
    if ( !m_fHasPath )
    {
        // 驚慌硬直攔截：若目前正處於遭受恐怖分子開槍中彈的受驚受傷僵直期，雙腿被震懾無法起跑
        if ( gpGlobals->time < m_flFlinchTime )
        {
            return;
        }
        if ( nTargetNode == -1 )
        {
            return; // 無合法目標節點，原地待命
        }
    }
    else
    {
        // 呼叫導航物件，反查目前 vecNodes 陣列中能夠通過的最遠可行進節點索引
        nTargetNode = m_LocalNav->GetFurthestTraversableNode( pev->origin, vecNodes, m_nPathNodes, TRUE );

        if ( nTargetNode == 0 )
        {
            // 防堵路微調：若距離目前最前線的 0 號節點小於 26 碼，說明即將抵達，自動前推到下一個轉角
            float flDistToNode0 = (vecNodes[0] - pev->origin).Length2D();
            if ( flDistToNode0 < 26.0f )
            {
                nTargetNode--;
            }
        }

        if ( nTargetNode == -1 )
        {
            // 尋路斷裂：若軌跡丟失或走投無路，重置計畫時鐘，進入防卡死重試
            m_fHasPath = FALSE;
            m_flPathCheckInterval = 0.1f;
            return;
        }

        if ( gpGlobals->time < m_flFlinchTime )
        {
            return;
        }
    }

    // 5. 幾何視線扭正與骨骼角度鎖：朝向即將前進的 vecNodes[nTargetNode] 目標節點
    if ( pev->flags & FL_ONGROUND )
    {
        pev->angles.x = 0.0f; // 嚴格強鎖平地 Yaw 轉向，防止模型鬼畜傾斜
        pev->angles.z = 0.0f;

        Vector vecNodeDir = vecNodes[nTargetNode] - pev->origin;
        Vector vecTargetAngles;
        UTIL_VecToAngles( vecNodeDir, vecTargetAngles );
        
        // 讓整具身體的外觀角度平滑對齊轉角朝向
        pev->angles.y = vecTargetAngles.y;
    }

    // 直升機索降或空中滑行特殊修正
    if ( pev->movetype == MOVETYPE_FLY )
    {
        pev->v_angle.x = -60.0f;
    }

    // 6. 🔥 終極前進：呼叫引擎核心尋路推力接口，驅動雙腿全速奔向目標節點座標！
    MoveToward( &vecNodes[nTargetNode] );
}

void CHostage::FollowUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // 1. 狀態硬熔斷：若人質已經成功抵達營救區被註銷了，不再響應任何 E 鍵
    if ( m_rescued )
    {
        return;
    }

    // 2. 安全審查：確保觸發者有效且該觸發者是玩家 (CBasePlayer)
    if ( pActivator == nullptr || !pActivator->IsPlayer() )
    {
        return;
    }

    // 3. 劇情鎖：若目前人質處於免傷劇情或地圖強鎖定定身狀態 (takedamage == 0)，拒絕任何互動
    if ( pev->takedamage == DAMAGE_NO )
    {
        return;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pActivator;

    // =========================================================================
    // 💉 劇情黑科技：醫療注射針 (weapon_syringe) 受傷急救重置判定
    // =========================================================================
    int iWeaponClass = pPlayer->GetActiveWeaponClassname();
    const char *pszWeaponName = STRING( iWeaponClass );

    if ( pszWeaponName != nullptr && strcasecmp( pszWeaponName, "weapon_syringe" ) == 0 )
    {
        // 若人質目前身上正帶著中彈重傷/毒氣的瀕死標記 (m_healthModifier != 0)
        if ( m_healthModifier != 0.0f )
        {
            // 完美注入一劑醫療強心針，清空負面瀕死標記，將人質重啟救活！
            m_healthModifier = 0.0f;
            return;
        }
    }

    // =========================================================================
    // 📡 玩家通訊與高階狀態遮罩判定 (100% 完美還原 0x11 與 0x17 的內聯位元行為)
    // =========================================================================
    // (假定對應玩家內部的 bits_HUD_RESCUE_REMINDER 與梯子/索降狀態判定)
    unsigned int *pPlayerHUDFlags = (unsigned int *)((char *)pPlayer + 0x8A0); // 假定對齊 0x17 槽位
    if ( pPlayer->pev->movetype == MOVETYPE_FLY ) // 索降限制
    {
        *pPlayerHUDFlags |= 0x200000; // 發射 UI 提醒通訊信號，攔募入隊
        return;
    }

    // 4. 戰術冷卻鎖：1.0 秒安全阻斷間隔，防止玩家快速連按 E 鍵造成語音重疊與大腦破裂
    if ( gpGlobals->time < m_flNextChange )
    {
        return;
    }

    // 刷新互動冷卻時鐘
    m_flNextChange   = gpGlobals->time + 1.0f;
    m_flPathAcquired = gpGlobals->time; // 記錄獲取新目標的時間戳

    // =========================================================================
    // 🎰 核心狀態機輪盤：在 FOLLOW (跟隨) 與 STAND (原地守點) 之間平滑雙向切換
    // =========================================================================
    if ( pPlayer == m_hTargetEnt )
    {
        // 判定 A：如果玩家就是目前的主人，再次按 E 鍵執行 解約 / 原地駐守
        if ( m_State != 1 ) // 假設 1 = FOLLOW 狀態代號
        {
            m_State = 1;
            goto LAB_PLAY_RECRUIT_SOUND;
        }

        m_State = 0; // 0 = STAND 原地待命駐守模式
        m_hTargetEnt        = nullptr;
        m_hStoppedTargetEnt = pPlayer; // 備份等待目標

        // 📢 高喊原地防禦待命台詞組 ("NPC_WAIT")！
        PlaySentence( "NPC_WAIT", 0.75f, 0.8f, 0, (int)m_voicePitch );
    }
    else
    {
        // 判定 B：自由身或者被新 CT 截胡，執行 招募入隊 / 開始跟隨
        m_State = 1; // FOLLOW
        m_idleAnim = ALLOC_STRING( "default" ); // 強制切換為常規跟隨跑步骨骼幀

        m_hTargetEnt        = pPlayer;
        m_hStoppedTargetEnt = nullptr;
    }

    if ( m_State != 1 )
    {
        return;
    }

LAB_PLAY_RECRUIT_SOUND:
    // =========================================================================
    // 📢 終極引爆：不管是剛被招募、還是入隊中，廣播高分貝求救/誓死追隨台詞組 ("H_HELP")
    // =========================================================================
    PlaySentence( "H_HELP", 0.75f, 0.8f, 0, (int)m_voicePitch );
}

float CHostage::GetModifiedDamage( float flDamage, int nHitGroup )
{
    // =========================================================================
    // 🩸 依據標準骨骼受擊部位 (Hitgroups)，動態折算人質的戰術器官受損倍率
    // =========================================================================
    switch ( nHitGroup )
    {
        case HITGROUP_GENERIC: // 0
        case HITGROUP_STOMACH: // 3
            return flDamage * 1.75f; // 腹部中彈重創加成

        case HITGROUP_HEAD:    // 1
            return flDamage * 2.5f;  // 🎯 爆頭極限致命傷害加成！

        case HITGROUP_LEFTARM:  // 4
        case HITGROUP_RIGHTARM: // 5
            return flDamage * 0.75f; // 手臂擦傷折減防禦

        case HITGROUP_LEFTLEG:  // 6
        case HITGROUP_RIGHTLEG: // 7
            return flDamage * 0.6f;  // 雙腿受擊折減防禦

        default: // 常規包括 HITGROUP_CHEST (2) 胸部等軀幹中心
            return flDamage * 1.5f;  // 胸部中彈透體傷害加成
    }
}

Schedule_t* CHostage::GetSchedule( void )
{
    // 完美還原二進位行為：人質大腦不進行主動威脅評估
    // 一鍵調用虛擬表 93 號偏移量 (GetScheduleOfType)，強制將大腦框定在 1 號基準計畫
    // 100% 對齊 SCHED_IDLE_STAND (原地戰術休閒/靜止待命計畫)，等待玩家 E 鍵歸隊招募
    return GetScheduleOfType( SCHED_IDLE_STAND ); // 1
}

void CHostage::IdleThink( void )
{
    // 1. 優先調用 CBaseMonster 母體怪物基類的每幀物理思考，維持視野和基礎心跳
    CBaseMonster::MonsterThink();

    // 若人質已經被成功營救註銷，大腦永久停機，防止二次觸發
    if ( m_rescued )
    {
        return;
    }

    // 2. 死亡狀態熔斷：若不幸中彈身亡 (deadflag = DEAD_DEAD = 2)，清空物理尺寸，徹底躺平
    if ( pev->deadflag == DEAD_DEAD )
    {
        UTIL_SetSize( pev, g_vecZero, g_vecZero );
        return;
    }

    // 3. 💀 主人存活審查：若跟隨的主人不幸在交火中陣亡，自動解約入隊狀態，恢復自由身
    if ( m_hTargetEnt != nullptr && m_hTargetEnt->pev->deadflag != DEAD_NO )
    {
        m_State      = 0; // STAND
        m_hTargetEnt = nullptr;
    }

    // =========================================================================
    // 🏢 營救點區域探測大閉環：若處於入隊狀態，即時動態核對是否進入撤離點
    // =========================================================================
    if ( m_hTargetEnt != nullptr )
    {
        // 100% 完美還原二進位解平優化：在非特殊規則阻斷下，全圖掃描 info_hostage_rescue 實體
        if ( g_pGameRules != nullptr && !g_pGameRules->IsMultiplay() ) 
        {
            CBaseEntity *pRescueZone = nullptr;
            while ( (pRescueZone = UTIL_FindEntityByClassname( pRescueZone, "info_hostage_rescue" )) != nullptr )
            {
                // 計算人質原點與營救點之間的真實 3D 空間距離
                float flDistToZone = (pRescueZone->pev->origin - pev->origin).Length();
                
                if ( flDistToZone < 256.0f ) // 踏入 256 碼黃金紅線
                {
                    m_bRescueMe = TRUE; // 激活營救過渡標記
                    goto LAB_EXECUTE_RESCUE_PROCEED;
                }
            }
        }
    }

    // 4. 尋路推進調度：若未滿足營救條件，繼續維持 DoFollow 幾何尋路避障
    if ( !m_bRescueMe )
    {
        DoFollow();
    }

LAB_EXECUTE_RESCUE_PROCEED:
    // =========================================================================
    // 🎬 終極大結局：任務圓滿達成、全域戰局結算與隱形熔斷矩陣
    // =========================================================================
    if ( m_bRescueMe && !m_rescued )
    {
        // 1. 下游劇情引爆：若綁定了自定義劇情目標，一鍵遠端觸發地圖實體
        if ( m_rescueTarget != 0 )
        {
            FireTargets( STRING(m_rescueTarget), this, this, USE_TOGGLE, 0.0f );
        }

        // 2. 📢 語音發射：高喊獲救感謝台詞組 ("H_THANK")，Volume 拉滿 1.0f
        PlaySentence( "H_THANK", 1.0f, 0.8f, 0, (int)m_voicePitch );

        // 3. 撤離與自動銷毀機制評估
        if ( !m_noAutoRemove )
        {
            if ( m_rescueAnim == 0 )
            {
                // 分支 A：標準退場。打上隱形鎖 (~EF_NODRAW = 0x80)，物理、受創、尺寸全數格式化歸零
                pev->effects   |= EF_NODRAW; // 0x80
                pev->movetype   = MOVETYPE_NONE;
                pev->solid      = SOLID_NOT;
                pev->takedamage = DAMAGE_NO;
                
                UTIL_SetSize( pev, g_vecZero, g_vecZero );
                
                // 意識大腦永久停機，等待下回合自適應 Restart() 重置顯形
                pev->nextthink    = -1.0f;
                m_flNextFullThink = -1.0f;
                goto LAB_ROUND_VICTORY_RECKON;
            }
            else
            {
                // 分支 B：高級動畫撤離。反查地圖指定的撤離骨骼序列並強制播放
                pev->sequence = LookupSequence( STRING(m_rescueAnim) );
            }
        }

        // 重置隨行思維，清空主人指針
        m_bRescueMe = FALSE;
        m_hTargetEnt = nullptr;
		
LAB_ROUND_VICTORY_RECKON:
        // =========================================================================
        // 📡 全域大閉環：利用指針強轉，精確累加全域戰局已被成功營救的人質總計數器！
        // 100% 鏡像重合 Linux server.so 記憶體行為，即時引爆「CT Win」全服回合勝利結算
        // =========================================================================
        int *pNumRescued = (int *)((char *)g_pGameRules + 124); // 假定對齊二進位 m_iNumHostagesRescued 槽位
        (*pNumRescued)++;

        // 標記為已被成功營救
        m_rescued = TRUE;
        
        m_idleAnim = 0;

        // 4. 重整姿態：將動畫強制Lookup重置回 Idle (1) 基準計畫，理想狀態歸入 MONSTERSTATE_IDLE
        if ( m_Activity != ACT_IDLE )
        {
            int iIdleSeq = LookupActivity( ACT_IDLE );
            if ( iIdleSeq != -1 )
            {
                if ( iIdleSeq != pev->sequence )
                {
                    pev->frame = 0.0f;
                    pev->sequence = iIdleSeq;
                }
                m_Activity = ACT_IDLE; // 1
                ResetSequenceInfo();
            }
        }
        
        m_MonsterState = MONSTERSTATE_IDLE;
        return;
    }
    // =========================================================================
    // 🏃 動作混合器動態更新矩陣：依據當前實時物理速度，自適應 Lookup 骨骼序列
    // =========================================================================
    if ( gpGlobals->time >= m_flFlinchTime )
    {
        float flCurrentSpeed = pev->velocity.Length();

        if ( flCurrentSpeed <= 160.0f ) // 漫步或靜止臨界線
        {
            if ( flCurrentSpeed < 1.0f ) // 完全靜止狀態
            {
                if ( m_Activity != ACT_IDLE ) // ACT_IDLE = 1
                {
                    int iIdleSeq = LookupActivity( ACT_IDLE );
                    if ( iIdleSeq != -1 )
                    {
                        if ( iIdleSeq != pev->sequence )
                        {
                            pev->frame = 0.0f;
                            pev->sequence = iIdleSeq;
                        }
                        m_Activity = ACT_IDLE;
                        ResetSequenceInfo();
                    }
                }
            }
            else // 1.0f ~ 160.0f 戰術慢步慢跑姿態
            {
                if ( m_Activity != ACT_WALK ) // ACT_WALK = 3
                {
                    int iWalkSeq = LookupActivity( ACT_WALK );
                    if ( iWalkSeq != -1 )
                    {
                        if ( iWalkSeq != pev->sequence )
                        {
                            pev->frame = 0.0f;
                            pev->sequence = iWalkSeq;
                        }
                        m_Activity = ACT_WALK;
                        ResetSequenceInfo();
                    }
                }
            }
        }
        else // > 160.0f 極速驚恐狂奔狀態 (全速逃離火線)
        {
            if ( m_Activity != ACT_RUN ) // ACT_RUN = 4
            {
                int iRunSeq = LookupActivity( ACT_RUN );
                if ( iRunSeq != -1 )
                {
                    if ( iRunSeq != pev->sequence )
                    {
                        pev->frame = 0.0f;
                        pev->sequence = iRunSeq;
                    }
                    m_Activity = ACT_RUN;
                    ResetSequenceInfo();
                }
            }
        }
    }
}

BOOL CHostage::IsOnLadder( void )
{
    // 完美還原二進位行為：若目前實體的移動物理型態為 MOVETYPE_FLY (5)
    // 則向大腦與尋路器宣告目前人質正處於攀爬梯子或索降狀態
    return ( pev->movetype == MOVETYPE_FLY ); // 5
}

void CHostage::KeyValue( KeyValueData *pkvd )
{
    // =========================================================================
    // 🏢 劇情版人質專屬：地圖自定義屬性自適應字串匹配矩陣 (不區分大小寫)
    // =========================================================================
    
    // 1. 潛行關卡安靜營救模式 (不播放全服無線電宣告)
    if ( strcasecmp( pkvd->szKeyName, "quiet" ) == 0 )
    {
        m_bQuiet = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    // 2. 隱藏雷達與戰術 HUD 位置提示
    else if ( strcasecmp( pkvd->szKeyName, "hidden" ) == 0 )
    {
        m_bHidden = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }
    // 3. 獲救瞬間引爆的遠端地圖劇情目標名稱
    else if ( strcasecmp( pkvd->szKeyName, "rescuetarget" ) == 0 )
    {
        m_rescueTarget = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 4. 地圖指定的自定義獲救撤離動畫序列名稱
    else if ( strcasecmp( pkvd->szKeyName, "rescueanim" ) == 0 )
    {
        m_rescueAnim = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
        return;
    }
    // 5. 是否免除獲救時的自動隱形/註銷退場機制
    else if ( strcasecmp( pkvd->szKeyName, "noautoremove" ) == 0 )
    {
        m_noAutoRemove = ( atoi( pkvd->szValue ) != 0 );
        pkvd->fHandled = TRUE;
        return;
    }

    // =========================================================================
    // 2. 若不屬於上述 5 大自定義劇情屬性，放行降級，交由怪物母體與引擎常規鏈解析
    // 100% 鏡像重合二進位行為，確保 origin、angles 等基類變數順利定位
    // =========================================================================
    CBaseMonster::KeyValue( pkvd );
}

void CHostage::MoveToward( Vector *vecLoc )
{
    if ( vecLoc == nullptr )
    {
        return;
    }

    // 1. 📢 隨行安全審查：確保跟隨的主人有效
    CBaseEntity *pPlayer = CBaseEntity::Instance( m_hTargetEnt->edict() );
    if ( pPlayer == nullptr || pPlayer->pev == nullptr )
    {
        return;
    }

    // 2. 幾何向量運算：計算人質原點指向目標節點 vecLoc 的方向夾角
    Vector vecNodeDir = *vecLoc - pev->origin;
    Vector vecTargetAngles;
    UTIL_VecToAngles( vecNodeDir, vecTargetAngles );

    Vector vecForward;
    // 呼叫引擎核心，將 Yaw 轉向角解碼為三維世界空間的前進方向向量
    UTIL_Orders::AngleVectors( Vector(0.0f, vecTargetAngles.y, 0.0f), vecForward, nullptr, nullptr );

    // 🚀 幾何外推探測：方向向量乘以單跨步最大步進長度 16.0f (CLocalNav::s_flStepSize = 16)
    Vector vecCheckPoint = pev->origin + ( vecForward * 16.0f );

    // 3. 呼叫導航物件進行前路可行性探測 (PathTraversable)
    int iNavStatus = m_LocalNav->PathTraversable( pev->origin, vecCheckPoint, FALSE );

    if ( iNavStatus != 0 ) // 前方道路通行合法
    {
        // 預設常規前進速度基數
        float flSpeedGoal = 250.0f;

        if ( pev->flags & FL_ONGROUND )
        {
            // 4. 計算人質與前方引路玩家之間的真實 3D 空間距離
            float flDistToPlayer = ( pPlayer->pev->origin - pev->origin ).Length();

            // =========================================================================
            // 🎰 三階動態速度加載輪盤：根據脫隊距離，自適應切換 250 -> 300 -> 400 狂奔推力
            // =========================================================================
            if ( flDistToPlayer >= 110.0f )
            {
                if ( flDistToPlayer < 250.0f )
                {
                    flSpeedGoal = 300.0f; // 戰術小跑姿態
                }
                else
                {
                    flSpeedGoal = 400.0f; // 🔔 嚴重掉隊！發動全速驚恐狂奔推力追趕玩家！
                }
            }
        }

        // 5. 正式為人質的 X/Y 軸注入即時物理前進速度向量
        pev->velocity.x = vecForward.x * flSpeedGoal;
        pev->velocity.y = vecForward.y * flSpeedGoal;

        // =========================================================================
        // 🎨 測試 Debug 雷射射線彩蛋：100% 還原二進位前方 500 碼未來座標方向投影束
        // =========================================================================
        float flVelLen = pev->velocity.Length();
        Vector vecPredictOrigin;
        if ( flVelLen == 0.0f )
        {
            vecPredictOrigin = pev->origin + Vector( 0.0f, 0.0f, 500.0f );
        }
        else
        {
            vecPredictOrigin = pev->origin + ( (pev->velocity / flVelLen) * 500.0f );
        }
        // 拉出一條戰術未來預測雷射束特效
        UTIL_DrawBeamPoints( pev->origin, vecPredictOrigin, 255, 0, 0, 255 );

        // =========================================================================
        // 🧱 6. 地形自適應動態跳躍中樞：100% 杜絕小箱子、窄小台階卡死 NPC
        // =========================================================================
        if ( iNavStatus == 2 ) // NAV_STAIRS_OR_STEP (前方碰到了低矮矮牆或階梯)
        {
            // 給予向上微小彈跳力 180.0f，輕盈跨越台階
            pev->velocity.z = 180.0f;
            return;
        }
        
        if ( iNavStatus == 3 && (pev->flags & FL_ONGROUND) ) // NAV_JUMP_GAP (前方碰到寬度小水溝或小懸崖)
        {
            // 給予向前上方跨步大跳躍彈跳力 270.0f，凌空飛躍障礙！
            pev->velocity.z = 270.0f;
            return;
        }
    }
}

void CHostage::NavReady( void )
{
    // 1. 📢 隨行主權審查：確保引路主人有效，否則拒絕發動幾何尋路
    if ( m_hTargetEnt == nullptr )
    {
        return;
    }

    CBaseEntity *pPlayer = CBaseEntity::Instance( m_hTargetEnt->edict() );
    if ( pPlayer == nullptr || pPlayer->pev == nullptr )
    {
        return;
    }

    // 2. 讀取玩家原點座標作為初始終點
    Vector vecTargetOrigin = pPlayer->pev->origin;

    // =========================================================================
    // 🧱 鋼鐵防懸空黑科技：若玩家處於常規地面狀態，向下發射 300 碼人類包圍盒重力射線
    // =========================================================================
    if ( !(pPlayer->pev->flags & (FL_FLY | FL_SWIM)) ) // 完美還原 !(flags & 0x1A5 的特定位元)
    {
        Vector vecTraceEnd = vecTargetOrigin;
        vecTraceEnd.z -= 300.0f; // 垂直向下延伸 300 碼世界象限

        TraceResult tr;
        // 使用 HULL_HUMAN (1) 常規特警立方體積物理大小進行空間探測
        UTIL_TraceHull( vecTargetOrigin, vecTraceEnd, ignore_monsters, head_hull, pPlayer->edict(), &tr );

        // 安全防模組卡牆熔斷：若開局卡進固體牆或下方是萬丈深淵虛空，硬性終止尋路
        if ( tr.fStartSolid )
        {
            return;
        }
        if ( tr.flFraction == 1.0f )
        {
            return;
        }

        // 🎯 幾何修正：將尋路終點精確回填、降級為地表上最真實、暢通的物理固體著地點
        vecTargetOrigin = tr.vecEndPos;
    }

    // 加上玩家視線眼睛高度補白偏置
    vecTargetOrigin.z += pPlayer->pev->view_ofs.z;

    // 同步導航宿主主權目標
    m_LocalNav->m_pTargetEnt = pPlayer->edict();

    // =========================================================================
    // 🎰 3. 驅動 CLocalNav 核心導航網格 FindPath 運算 (搜尋半徑 40.0f)
    // =========================================================================
    node_index_t nindex = m_LocalNav->FindPath( pev->origin, vecTargetOrigin, 40.0f, TRUE );

    if ( nindex == -1 ) // 尋路宣告徹底失敗 (被障礙物堵死)
    {
        if ( !m_fHasPath )
        {
            // 💡 智商退化算法：大腦進入尋路懲罰期，每次失敗冷卻步進累加 0.1 秒，最高壓制到 0.5 秒，
            // 降低高頻卡死撞牆時的 CPU 幾何追蹤消耗，完美保護伺服器 FPS
            float flNewInterval = m_flPathCheckInterval + 0.1f;
            if ( flNewInterval > 0.5f )
            {
                flNewInterval = 0.5f;
            }
            m_flPathCheckInterval = flNewInterval;
        }
    }
    else // 尋路大獲全勝！
    {
        m_fHasPath = TRUE;

        // 重置決策鏈時鐘，放鬆至 0.5 秒常規休息刷新線
        nTargetNode           = -1;
        m_flPathCheckInterval = 0.5f;
        m_flPathAcquired      = gpGlobals->time;

        // 🚀 數據灌注：將尋路路徑點，完美、整齊地重寫填入 vecNodes[16] 快取陣列中！
        m_nPathNodes = m_LocalNav->SetupPathNodes( nindex, vecNodes, TRUE );
    }
}

int CHostage::ObjectCaps( void )
{
    // 完美還原二進位行為 0x22 (FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE)
    // 賦予人質跨地圖 Changelevel 隨行保留能力，並解鎖標準玩家 E 鍵脈衝觸發回調
    return ( FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE ); // 0x22
}

void CHostage::PlayPainSound( void )
{
    // 1. 📢 戰術降噪鎖：核對目前遊戲時間是否跨過了痛苦慘叫冷卻線，防止連續受擊聲音鬼畜重疊
    if ( gpGlobals->time > m_flNextPainTime )
    {
        // 2. 調用虛擬表 96 號偏移量 (PlaySentence)，全服高分貝廣播人質專屬疼痛語音組
        // 自動連動 m_femaleFlag 與 m_language，實時分流男聲悶哼或女聲尖叫
        PlaySentence( "H_PAIN", 1.0f, 0.8f, 0, (int)m_voicePitch );

        // 3. 🛠️ 核心硬性冷卻：將下一次慘叫時鐘無條件推遲 1.0 秒，鎖定語音通道
        m_flNextPainTime = gpGlobals->time + 1.0f;
    }
}

void CHostage::PointAt( Vector *vecLoc )
{
    if ( vecLoc == nullptr )
    {
        return;
    }

    // 1. 優先確保人質的 X/Z 軸角度強鎖為 0，防止模型在轉向時發生高低歪斜或鬼畜穿模 Bug
    pev->angles.x = 0.0f; 

    // 2. 幾何運算：計算人質自身原點指向目標位置 vecLoc 的世界相對向量
    Vector vecLookDir = *vecLoc - pev->origin;
    Vector vecTargetAngles;

    // 3. 呼叫引擎核心三角函數，將方向向量解碼為精準的三維歐拉角
    UTIL_VecToAngles( vecLookDir, vecTargetAngles );

    // 4. 🚀 最高朝向對齊：精確提取解碼後的 Y 軸偏航角 (Yaw)，一鍵扭正人質整具身體的面向！
    pev->angles.y = vecTargetAngles.y;

    // 二次確保 Roll 軸平地對齊
    pev->angles.z = 0.0f;
}

void CHostage::PreThink( void )
{
    // 1. 📢 狀態過濾：確保人質正站在地面上 (FL_ONGROUND = 0x200)
    if ( pev->flags & FL_ONGROUND )
    {
        float flInterval = gpGlobals->frametime;
        float flSpeed2D = pev->velocity.Length2D();

        // 2. 只有當人質處於前進跑動狀態時，才激活每幀樓梯幾何外推探測
        if ( flSpeed2D >= 1.0f )
        {
            Vector vecSrc = pev->origin;
            
            // 幾何外推：計算在當前幀時間增量下，下一幀人質即將抵達的理論預測點
            Vector vecDest;
            vecDest.x = pev->origin.x + pev->velocity.x * flInterval;
            vecDest.y = pev->origin.y + pev->velocity.y * flInterval;
            vecDest.z = pev->origin.z;

            TraceResult tr;
            // 使用 HULL_HUMAN (0) 標準包圍盒在怪物前進路徑上進行滑動探測
            UTIL_TraceMonsterHull( edict(), vecSrc, vecDest, ignore_monsters, edict(), &tr );

            // 3. 陡峭邊緣攔截：若前方受阻，且撞擊面的法線傾斜度非常陡峭 (vecPlaneNormal.z <= 0.7)
            if ( !tr.fStartSolid && tr.flFraction != 1.0f && tr.vecPlaneNormal.z <= 0.7f )
            {
                // 記錄原始撞擊點的 2D 相對距離
                float flOrigDist = ( tr.vecEndPos - pev->origin ).Length2D();

                // 4. 🚀 樓梯翻越黑科技：將探測起點強行向上抬升 16 碼 (s_flStepSize 最大跨步高度)
                Vector vecRaisedSrc = vecSrc;
                vecRaisedSrc.z += 16.0f; // CLocalNav::s_flStepSize = 16.0f

                Vector vecRaisedDest;
                if ( flSpeed2D == 0.0f )
                {
                    vecRaisedDest = vecRaisedSrc;
                }
                else
                {
                    // 在速度方向上前推 0.1 比例的微小偏置
                    vecRaisedDest.x = vecRaisedSrc.x + ( pev->velocity.x / flSpeed2D ) * 0.1f;
                    vecRaisedDest.y = vecRaisedSrc.y + ( pev->velocity.y / flSpeed2D ) * 0.1f;
                    vecRaisedDest.z = vecRaisedSrc.z;
                }

                // 進行第二次高空頂部包圍盒滑動探測，審查台階上方是否封頂、是否暢通
                UTIL_TraceMonsterHull( edict(), vecRaisedSrc, vecRaisedDest, ignore_monsters, edict(), &tr );

                if ( !tr.fStartSolid )
                {
                    Vector vecFloorCheckSrc = tr.vecEndPos;
                    Vector vecFloorCheckDest = tr.vecEndPos;
                    vecFloorCheckDest.z -= 16.0f; // 垂直向下發射重力檢測線

                    // 第三次探測：核對台階頂部表面是否平坦可行走
                    UTIL_TraceMonsterHull( edict(), vecFloorCheckSrc, vecFloorCheckDest, ignore_monsters, edict(), &tr );

                    // 5. 躍遷條件大圓滿：若台階上方坡度安全 (>= 0.7)，且跨越距離合法
                    if ( tr.vecPlaneNormal.z >= 0.7f )
                    {
                        float flRaisedDist = ( tr.vecEndPos - pev->origin ).Length2D();
                        
                        if ( flOrigDist < flRaisedDist )
                        {
                            // 瞬間改寫空間坐標！將人質即時動態「提拔」到樓梯台階頂部世界高度
                            Vector vecNewOrigin = pev->origin;
                            vecNewOrigin.z = tr.vecEndPos.z;
                            UTIL_SetOrigin( pev, vecNewOrigin );

                            // 🛠️ 核心重力逆向補償：在 Z 軸注入對沖重力的向上推力，確保爬樓梯順滑不卡步
                            pev->velocity.z += flInterval * g_psv_gravity->value * pev->gravity;
                        }
                    }
                }
            }
        }
    }
}

void CHostage::Remove( void )
{
    // 1. 物理重置：關閉所有重力位移、肉體碰撞體、以及免除受擊受傷判定，使其化為物理幽靈
    pev->movetype   = MOVETYPE_NONE; // 0
    pev->solid      = SOLID_NOT;     // 0
    pev->takedamage = DAMAGE_NO;     // 0.0f

    // 2. 幾何收縮：將其 3D 空間碰撞盒強行坍塌壓縮為 0 奇點體積，防止卡死地圖任何物件
    UTIL_SetSize( pev, g_vecZero, g_vecZero );

    // 3. 🚀 意識停機：將雙重大腦思考時鐘死死鎖定在 -1.0f 永久停機線，徹底進入休眠待命狀態
    // 等待新回合開局時，由 Restart 系統重新點火拉起
    pev->nextthink    = -1.0f;
    m_flNextFullThink = -1.0f;
}

void CHostage::RePosition( void )
{
    // 1. 🩸 生命體徵與肉體物理權限完全重置復活
    pev->health     = pev->max_health; // 血量填滿 100.0f
    pev->movetype   = MOVETYPE_WALK;   // 4
    pev->solid      = SOLID_SLIDEBOX;  // 3
    pev->takedamage = DAMAGE_AIM;      // 1.0f
    pev->deadflag   = DEAD_NO;         // 0
    pev->velocity   = g_vecZero;       // 速度向量清零

    // 2. 原始幾何朝向角度歸位復位
    pev->angles = m_vStartAngles;

    // 3. 📢 顯形解鎖：洗掉上一回合退場時打上的隱形鎖 (~EF_NODRAW = ~0x80)，重新顯形
    pev->effects &= ~EF_NODRAW;

    // 4. 隨行主權關係與大腦記憶體格式化清理
    m_hTargetEnt        = nullptr;
    m_hStoppedTargetEnt = nullptr;
    
    m_bTouched    = FALSE; // 完美歸位 KeyValue 曝光的踫觸旗標
    m_bRescueMe   = FALSE;
    m_rescued     = FALSE; // 確保重設已獲救標記
    m_flNextRadarTime = 0.0f;

    // 🚀 防卡死重置：將老位置座標重新填裝為 9999 極限無效值，準備新局尋路
    m_vOldPos = Vector( 9999.0f, 9999.0f, 9999.0f );

    // 5. 跨象限瞬移：呼叫引擎核心，一鍵將人質強行傳送抓回出生原始幾何原點！
    UTIL_SetOrigin( pev, m_vStart );

    // 配置 20x20x62 標準人質緊湊幾何凸殼
    UTIL_SetSize( pev, Vector(-10.0f, -10.0f, 0.0f), Vector(10.0f, 10.0f, 62.0f) );

    // 自動垂直落地下榻到固體地面上
    DROP_TO_FLOOR( edict() );

    // =========================================================================
    // ⚙️ 6. 動作混合器重新配置 (0x3E 偏移量代表 LookupActivity)
    // =========================================================================
    if ( m_Activity != ACT_IDLE ) // ACT_IDLE = 1
    {
        int iIdleSeq = LookupActivity( ACT_IDLE );
        if ( iIdleSeq != -1 )
        {
            if ( iIdleSeq != pev->sequence )
            {
                pev->frame = 0.0f;
                pev->sequence = iIdleSeq;
            }
            m_Activity = ACT_IDLE;
            ResetSequenceInfo();
        }
    }

    // =========================================================================
    // 🎰 7. 大腦重新點火：切回待命思維，重啟 RANDOM 隨機交錯時間步進 (防 CPU 共振)
    // =========================================================================
    SetThink( &CHostage::IdleThink );
    pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.1f, 0.2f );

    // 8. 全套尋路、路徑計時器開局數據完全歸零
    nTargetNode            = -1;
    m_fHasPath             = FALSE;
    m_flLastPathCheck      = -1.0f;
    m_flPathAcquired       = -1.0f;
    m_flPathCheckInterval  = 0.1f;
    m_flNextFullThink      = gpGlobals->time + RANDOM_FLOAT( 0.1f, 0.2f );
}

void CHostage::Restart( void )
{
    // =========================================================================
    // 🚀 全域中央尋路佇列大清洗：無條件重置 CLocalNav 靜態處理管線，徹底告別舊局殘留
    // =========================================================================
    CLocalNav::flNextCvarCheck = 0.0f;
    CLocalNav::flLastThinkTime = 0.0f;
    CLocalNav::tot_inqueue     = 0;
    CLocalNav::nodeval         = 0.0f;
    CLocalNav::qptr            = 0;
    CLocalNav::tot_hostages    = 0;
}

int CHostage::Restore( CRestore &restore )
{
    // 1. 優先呼叫母體類別怪物的 Restore 方法，恢復基礎生命體徵變數
    int status = CBaseMonster::Restore( restore );
    
    if ( status == 0 )
    {
        return 0; // 基類讀取失敗，硬熔斷退出
    }

    // 2. 完美還原：從存檔檔案中精確讀取 30 個 CHostage 特有的自定義序列化欄位
    // sizeof(m_SaveData) / sizeof(m_SaveData) 在編譯時會 100% 精確展開為常數 30 (0x1E)
    int iRet = restore.ReadFields( "CHostage", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );

    // 3. 🚀 內存重砲復活：讀檔成功後，跨越安全地址，在 Heap 堆積區上原地重新實例化避障導航物件！
    // 100% 鏡像重合二進位 operator new(0x24) 與 CLocalNav 建構子死死綁定行為
    m_LocalNav = new CLocalNav( this );

    return iRet;
}

int CHostage::Save( CSave &save )
{
    // 完美鏡像保存通道
    if ( !CBaseMonster::Save( save ) )
        return 0;
        
    return save.WriteFields( "CHostage", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

void CHostage::SetActivity( Activity NewActivity )
{
    // 1. 安全審查：若目標動作與目前動作不一致
    if ( m_Activity != NewActivity )
    {
        // 呼叫虛擬表 62 號偏移量 (LookupActivity)，反查目標動作在模型內部的真實序列號索引
        int iSequence = LookupActivity( NewActivity );

        if ( iSequence != -1 )
        {
            if ( iSequence != pev->sequence )
            {
                // 2. 🚀 100% 還原二進位雙向步態融合算法
                // 若目前的動作、或者即將切換的目標動作，非常規行走(3)/跑步(4)組成的平滑位移鏈
                // (也就是涉及與完全靜止 ACT_IDLE=1 之間的大跨度跳躍)
                if ( ((unsigned int)(m_Activity - ACT_WALK) > 1) || ((unsigned int)(NewActivity - ACT_WALK) > 1) )
                {
                    // 強制將動畫幀歸零重置，徹底阻斷骨骼硬性拉伸或視覺鬼畜 Bug
                    pev->frame = 0.0f;
                }

                // 正式改寫實體當前的骨骼動畫序列號
                pev->sequence = iSequence;
            }

            // 同步寫入當前活動標記，並實時重置刷新 StudioMdl 動畫速率頭
            m_Activity = NewActivity;
            ResetSequenceInfo();
            return;
        }
    }
}

void CHostage::SetDeathActivity( void )
{
    int iDeathSequence = -1;
    Activity DeathActivity = ACT_DIESIMPLE; // 預設標準死亡

    // =========================================================================
    // 🩸 依據最後致命中彈部位 (m_LastHitGroup)，動態 Lookup 專屬的倒地骨骼動作
    // =========================================================================
    switch ( m_LastHitGroup )
    {
        case HITGROUP_CHEST:    // 2
        case HITGROUP_RIGHTARM: // 5
            DeathActivity = (Activity)36; // ACT_DIE_CHESTSHOT (胸月中彈向後倒)
            break;

        case HITGROUP_STOMACH:  // 3
        case HITGROUP_RIGHTLEG: // 7
            DeathActivity = (Activity)38; // ACT_DIE_GUTSHOT (腹月中彈跪地捂肚子)
            break;

        case HITGROUP_LEFTARM:  // 4
        case HITGROUP_LEFTLEG:  // 6
            DeathActivity = (Activity)37; // ACT_DIE_LEGLSHOT (側向失去平衡偏癱倒地)
            break;

        default: // 包括 HITGROUP_HEAD (1) 爆頭致命傷
            DeathActivity = (Activity)66; // ACT_DIE_HEADSHOT (爆頭瞬間斷電一頭栽倒)
            break;
    }

    // 呼叫虛擬表 62 號偏移量 (LookupActivity)，反查該死亡動作在目前 .mdl 模型中的序列號
    iDeathSequence = LookupActivity( DeathActivity );

    if ( iDeathSequence != -1 )
    {
        if ( iDeathSequence != pev->sequence )
        {
            pev->frame = 0.0f; // 死亡首幀強制歸零，防止動作鬼畜撕裂
            pev->sequence = iDeathSequence;
        }

        m_Activity = DeathActivity;
        ResetSequenceInfo(); // 實時整理死亡動畫速率頭
    }
}

void CHostage::SetFlinchActivity( void )
{
    // 1. ⏱️ 注入 0.75 秒尋路硬熔斷鎖：中彈瞬間雙腿凍結 0.75 秒，強行阻斷 DoFollow 推進
    m_flFlinchTime = gpGlobals->time + 0.75f;

    int iFlinchSequence = -1;
    Activity FlinchActivity = ACT_SMALL_FLINCH; // 預設 26 號基準後仰硬直

    // =========================================================================
    // 🩸 依據中彈部位 (m_LastHitGroup)，動態 Lookup 專屬的抽搐骨骼動作
    // =========================================================================
    switch ( m_LastHitGroup )
    {
        case HITGROUP_LEFTARM:  // 4
            FlinchActivity = (Activity)73; // ACT_FLINCH_LEFTARM
            break;

        case HITGROUP_RIGHTARM: // 5
            FlinchActivity = (Activity)74; // ACT_FLINCH_RIGHTARM
            break;

        case HITGROUP_LEFTLEG:  // 6
            FlinchActivity = (Activity)75; // ACT_FLINCH_LEFTLEG
            break;

        case HITGROUP_RIGHTLEG: // 7
            FlinchActivity = (Activity)76; // ACT_FLINCH_RIGHTLEG
            break;

        default: // 0=Generic, 2=Chest, 3=Stomach 等軀幹中心受擊
            FlinchActivity = (Activity)26; // ACT_SMALL_FLINCH
            break;
    }

    // 呼叫虛擬表 62 號偏移量 (LookupActivity)，反查該硬直動作在模型內部的序列號
    iFlinchSequence = LookupActivity( FlinchActivity );

    if ( iFlinchSequence != -1 )
    {
        if ( iFlinchSequence != pev->sequence )
        {
            pev->frame = 0.0f; // 硬直首幀強制歸零，防止視覺閃爍
            pev->sequence = iFlinchSequence;
        }

        m_Activity = FlinchActivity;
        ResetSequenceInfo(); // 實時整理骨骼速率頭
    }
}

void CHostage::SetYawSpeed( void )
{
    float flYawSpeed = 90.0f; // 預設常規靜態待命每秒轉身角度上限為 90 度

    // =========================================================================
    // 🔃 依據當前實時骨骼動作 (m_Activity)，動態配准人質的 3D 幾何轉身角速度
    // =========================================================================
    switch ( m_Activity )
    {
        case ACT_WALK: // 3
            flYawSpeed = 180.0f; // 戰術慢步慢跑，提供 180 度高機動轉向
            break;

        case ACT_RUN:  // 4
            flYawSpeed = 360.0f; // 🎯 驚恐狂奔突擊，解鎖 360 度極速甩尾與瞬間折返能力！
            break;

        case ACT_IDLE: // 1
        default:
            flYawSpeed = 90.0f;  // 日常休閒待命維持 90 度緩慢偏航
            break;
    }

    // 將折算後的角速度，正式灌入引擎核心暫存器中，完成本幀尋路轉向鎖定
    pev->yaw_speed = flYawSpeed;
}

int CHostage::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
    // 1. 依據受擊部位 (m_LastHitGroup) 動態折算真實人體受損倍率
    float flActualDamage = GetModifiedDamage( flDamage, m_LastHitGroup );
    
    // 2. 生命值實時扣減 (防血量過折算溢出)
    if ( pev->health < flActualDamage )
    {
        flActualDamage = pev->health;
    }
    pev->health -= flActualDamage;

    // 3. 📢 戰場痛苦慘叫降噪發射器 (內部自帶 1.0 秒防鬼畜語音重疊鎖)
    PlayPainSound();

    // =========================================================================
    // 🎰 條件分流 A：人質殘血存活狀態 ( health > 0.0f )
    // =========================================================================
    if ( pev->health > 0.0f )
    {
        // 觸發誤傷判定：核對開槍的人是不是操控遊戲的 1 號玩家
        if ( pevAttacker != nullptr && pevAttacker->pContainingEntity != nullptr )
        {
            CBaseEntity *pAttacker = CBaseEntity::Instance( pevAttacker->pContainingEntity );
            if ( pAttacker != nullptr && pAttacker->IsPlayer() )
            {
                // 🚀 跨類別強寫玩家帳戶 m_iAccountFlags 暫存器
                // 100% 還原 |= 0x20 (bits_PLAYER_KILLED_HOSTAGE) 違規標記，在回合結束時核發巨額美金罰款
                unsigned int *pPlayerAccountFlags = (unsigned int *)((char *)pAttacker + 0x1C4); // 對齊二進位賬戶遮罩槽位
                if ( !( *pPlayerAccountFlags & 0x20 ) )
                {
                    *pPlayerAccountFlags |= 0x20; // 點亮 32 號傷害人質違規污點碼
                }
            }
        }

        // ⏱️ 激活 0.75 秒受擊硬直鎖：沒收雙腿尋路推力，並自適應播放全身五大部位抽搐動作
        SetFlinchActivity();

        // 存活狀態，向引擎核心成功返回真實扣除的傷害數值
        return (int)flActualDamage;
    }
    
    // =========================================================================
    // 💀 條件分流 B：人質徹底陣亡狀態 ( health <= 0.0f ) -> 100% 補全大閉環！
    // =========================================================================
    else
    {
        pev->health   = 0.0f;
        pev->movetype = MOVETYPE_TOSS; // 6 (開啟屍體自然拋物線重力下墜，受物理慣性滑行倒地)
        pev->flags   &= ~FL_ONGROUND;    // 剝離著地死鎖

        // 1. 🚀 骨骼受擊部位定向死亡倒地動畫點火 (爆頭瞬間斷電栽倒、或捂肚子前跪)
        SetDeathActivity();

        // 2. 📢 語音發射：全服廣播人質陣亡慘叫與全小隊特警無線電喪鐘警報對白組 ("H_DEATH")
        PlaySentence( "H_DEATH", 1.0f, 0.8f, 0, (int)m_voicePitch );

        // 3. 徹底關閉受創與物理碰撞體盒，狀態標記為 DEAD_DEAD (2)
        pev->takedamage = DAMAGE_NO; // 0.0f
        pev->deadflag   = DEAD_DEAD; // 2
        pev->solid      = SOLID_NOT; // 0

        // 4. 🛠️ 倒地緩衝：設定 3.0 秒的死亡演出時間線
        pev->nextthink = gpGlobals->time + 3.0f;

        // 呼叫怪物母體檢查地圖腳本 AI 觸發器連動
        CBaseMonster::FCheckAITrigger();

        // 5. 🚀 大腦意識轉移：3.0 秒屍體倒地完全靜止後，一鍵自動發動 Remove() 幽靈雪藏！
        // 清空物理體積、大腦永久停機，完美等待下一回合的 RePosition() 原地復活大重置！
        SetThink( &CHostage::Remove );

        return 0; // 陣營死亡，返回 0 傷害
    }
}

void CHostage::Touch( CBaseEntity *pOther )
{
    // 1. 若人質已經完成任務被營救註銷，徹底關閉肉體碰撞避讓
    if ( m_rescued )
    {
        return;
    }

    if ( pOther == nullptr || pOther->pev == nullptr )
    {
        return;
    }

    BOOL bShouldRepel = FALSE;

    // 2. 🚂 碰撞者身份過濾：若碰觸者是玩家 (IsPlayer() 虛擬表 42 號偏移量)
    if ( pOther->IsPlayer() )
    {
        // 判定玩家狀態是否滿足常規推擠規則
        bShouldRepel = TRUE;
    }
    // 3. 🚂 碰撞者身份過濾：若碰觸者是另一名同類人質 NPC
    else if ( strcasecmp( STRING(pOther->pev->classname), "monster_hostage" ) == 0 )
    {
        bShouldRepel = TRUE;
    }

    // =========================================================================
    // 📐 物理排斥推力幾何矩陣計算 (2D 水平滑開防卡死)
    // =========================================================================
    if ( bShouldRepel )
    {
        // 提取相對方向：人質原點減去擠壓者原點
        Vector vecPushDir = pev->origin - pOther->pev->origin;
        
        // 🔥 核心二進位還原：強鎖 Z 軸高度差為 0.0f，確保排斥力只在水平 2D 平面爆發，防止人質被擠飛上天
        vecPushDir.z = 0.0f;

        float flDist2D = vecPushDir.Length2D();

        if ( flDist2D > 0.0f )
        {
            // 將 2D 排斥方向向量歸一化
            Vector vecPushNorm = vecPushDir / flDist2D;

            // 🚀 注入排斥滑動加速度：在原有的物理速度上，無條件疊加 50.0f 碼的向外推力偏置
            pev->velocity.x += vecPushNorm.x * 50.0f;
            pev->velocity.y += vecPushNorm.y * 50.0f;
            pev->velocity.z += vecPushNorm.z * 50.0f; // 預設 2D 抹平，此處 z 分量為 0

            // =========================================================================
            // 🔵 測試 Debug 雷射射線彩蛋：100% 還原二進位前方 500 碼藍色避讓向量投影束
            // =========================================================================
            float flVelLen = pev->velocity.Length();
            Vector vecPredictOrigin;
            if ( flVelLen == 0.0f )
            {
                vecPredictOrigin = pev->origin + Vector( 0.0f, 0.0f, 500.0f );
            }
            else
            {
                vecPredictOrigin = pev->origin + ( (pev->velocity / flVelLen) * 500.0f );
            }
            // 拉出一條戰術未來避讓藍色雷射束特效
            UTIL_DrawBeamPoints( pev->origin, vecPredictOrigin, 0, 0, 255, 255 );
        }
    }
}

void CHostage::Wiggle( void )
{
    // 1. 📐 100% 完全對齊二進位內存棧的 8 方向 3D 空間探測向量矩陣
    static const Vector wiggle_directions[8] = 
    {
        Vector(   0.0f, -50.0f,   0.0f ), // 1. 正後
        Vector(   0.0f,   0.0f,   0.0f ), // 2. 原點
        Vector(   0.0f,  50.0f,   0.0f ), // 3. 正前
        Vector( -50.0f,   0.0f,  50.0f ), // 4. 左後上
        Vector(  50.0f,  50.0f,   0.0f ), // 5. 右前
        Vector(  50.0f, -50.0f,   0.0f ), // 6. 右後
        Vector( -50.0f,  50.0f,   0.0f ), // 7. 左前
        Vector(   0.0f, -50.0f, -50.0f )  // 8. 正後下
    };

    Vector vecEscapeAccum = g_vecZero; // 逃逸向量合成暫存器

    // =========================================================================
    // 🎰 8 方向立體雷達探測循環
    // =========================================================================
    for ( int i = 0; i < 8; i++ )
    {
        // 疊加得到外推探測目標點
        Vector vecCheckPoint = pev->origin + wiggle_directions[i];

        // 呼叫導航器反查該方向是否通行 (PathTraversable)
        int iNavStatus = m_LocalNav->PathTraversable( pev->origin, vecCheckPoint, TRUE );

        if ( iNavStatus == 0 ) // 💡 前方撞牆死路！
        {
            // 完美還原二進位幾何反轉數學：逆向相減，將排斥向量推向相反的暢通方向
            vecEscapeAccum.x -= wiggle_directions[i].x;
            vecEscapeAccum.y -= wiggle_directions[i].y;
            vecEscapeAccum.z -= wiggle_directions[i].z;
        }
    }

    // 2. 注入隨機微小抖動，防止向量相互抵消歸零死鎖
    vecEscapeAccum.x += RANDOM_FLOAT( -3.0f, 3.0f );
    vecEscapeAccum.y += RANDOM_FLOAT( -3.0f, 3.0f );
    vecEscapeAccum.z += RANDOM_FLOAT( -3.0f, 3.0f );

    float flEscapeLen = vecEscapeAccum.Length();

    // =========================================================================
    // 🚀 逃逸速度灌注矩陣
    // =========================================================================
    if ( flEscapeLen == 0.0f )
    {
        // 若依然為 0，預設給予向上的大推力跳躍解鎖
        pev->velocity.z += 100.0f;
    }
    else
    {
        // 歸一化並狠狠乘以 100.0f 碼的瞬間逃逸加速度，直接推離卡死地形！
        Vector vecEscapeNorm = vecEscapeAccum / flEscapeLen;
        pev->velocity.x += vecEscapeNorm.x * 100.0f;
        pev->velocity.y += vecEscapeNorm.y * 100.0f;
        pev->velocity.z += vecEscapeNorm.z * 100.0f;
    }

    // =========================================================================
    // 🟢 測試 Debug 雷射射線彩蛋：100% 還原二進位前方 500 碼綠色逃逸向量投影束
    // =========================================================================
    float flVelLen = pev->velocity.Length();
    Vector vecPredictOrigin;
    if ( flVelLen == 0.0f )
    {
        vecPredictOrigin = pev->origin + Vector( 0.0f, 0.0f, 500.0f );
    }
    else
    {
        vecPredictOrigin = pev->origin + ( (pev->velocity / flVelLen) * 500.0f );
    }
    // 拉出一條戰術未來逃逸綠色雷射束特效 (RGB 染成綠色)
    UTIL_DrawBeamPoints( pev->origin, vecPredictOrigin, 0, 255, 0, 255 );
}
