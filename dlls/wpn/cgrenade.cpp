#include "cgrenade.h"

// 🚀 精確還原 CGrenade 在 Condition Zero 存檔系統中的 16 個欄位矩陣
TYPEDESCRIPTION CGrenade::m_SaveData[] =
{
    DEFINE_FIELD(CGrenade, m_fRegisteredSound, FIELD_BOOLEAN),
    DEFINE_FIELD(CGrenade, m_bIsC4, FIELD_BOOLEAN),
    DEFINE_FIELD(CGrenade, m_iBoxClass, FIELD_INTEGER),
    
    // 煙霧彈專屬狀態
    DEFINE_FIELD(CGrenade, m_SGSmoke, FIELD_INTEGER),
    DEFINE_FIELD(CGrenade, m_angle, FIELD_INTEGER),
    DEFINE_FIELD(CGrenade, m_usEvent, FIELD_SHORT),
    DEFINE_FIELD(CGrenade, m_bLightSmoke, FIELD_BOOLEAN),
    DEFINE_FIELD(CGrenade, m_bDetonated, FIELD_BOOLEAN),
    DEFINE_FIELD(CGrenade, m_throwerClasstype, FIELD_INTEGER),
    
    // C4 滴答聲與計時狀態
    DEFINE_FIELD(CGrenade, m_flNextFreq, FIELD_TIME),
    DEFINE_FIELD(CGrenade, m_flNextFreqInterval, FIELD_FLOAT),
    DEFINE_FIELD(CGrenade, m_flNextBeep, FIELD_TIME),
    DEFINE_FIELD(CGrenade, m_flNextBlink, FIELD_TIME),
    DEFINE_FIELD(CGrenade, m_flC4Blow, FIELD_TIME),
    
    // 拆彈進度狀態
    DEFINE_FIELD(CGrenade, m_bStartDefuse, FIELD_BOOLEAN),
    DEFINE_FIELD(CGrenade, m_flDefuseCountDown, FIELD_TIME),
};

// 實作 Half-Life 經典的 Save/Restore 鏈條綁定
//IMPLEMENT_SAVERESTORE(CGrenade, CBaseMonster);

int CGrenade::BloodColor(void)
{
    return DONT_BLEED; // -1 代表金屬/非生物實體，受擊時不會噴出任何血跡
}

void CGrenade::BounceSound(void)
{
    if (pev->dmg > 50.0f)
    {
        // 威力大於 50 的重型高爆破片手榴彈（HE Grenade），播放沉重的破片反彈音效
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/he_bounce-1.wav", 0.25f, ATTN_NORM, 0, PITCH_NORM);
        return;
    }

    // 威力小於等於 50 的投擲物（如閃光彈、煙霧彈），隨機播放輕型砸牆音效
    const char *sample = nullptr;
    switch (RANDOM_LONG(0, 2))
    {
        case 0: sample = "weapons/grenade_hit1.wav"; break;
        case 1: sample = "weapons/grenade_hit2.wav"; break;
        case 2: sample = "weapons/grenade_hit3.wav"; break;
    }

    if (sample)
    {
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, sample, 0.25f, ATTN_NORM, 0, PITCH_NORM);
    }
}

void CGrenade::C4Think(void)
{
    // 1. 安全邊界判定：若 C4 實體意外掉出地圖範圍，則立即進行記憶體垃圾回收
    if (!IsInWorld())
    {
        UTIL_Remove(this);
        return;
    }

    // 2. 設置 C4 的下一次核心 Think 更新步進率 (0.12秒/幀)
    pev->nextthink = gpGlobals->time + 0.12f;

    // 3. 🚨 反恐精英核心機制：C4 滴答聲頻率線性遞增算法 🚨
    if (m_flNextFreq <= gpGlobals->time)
    {
        m_flNextFreq = gpGlobals->time + m_flNextFreqInterval;
        m_flNextFreqInterval *= 0.9f; // 每響一次，滴答聲間隔縮短 10%，模擬即將爆炸的緊張感

        // 依據當前波形序列切換聲音文件與衰減半徑 (m_fAttenu)
        switch (m_iCurWave)
        {
            case 0: m_sBeepName = "weapons/c4_beep1.wav"; m_fAttenu = ATTN_STATIC; break;  // 1.5f
            case 1: m_sBeepName = "weapons/c4_beep2.wav"; m_fAttenu = ATTN_NORM; break;    // 1.0f
            case 2: m_sBeepName = "weapons/c4_beep3.wav"; m_fAttenu = 0.8f; break;
            case 3: m_sBeepName = "weapons/c4_beep4.wav"; m_fAttenu = ATTN_IDLE; break;    // 0.5f
            case 4: m_sBeepName = "weapons/c4_beep5.wav"; m_fAttenu = 0.2f; break;
        }

        m_iCurWave++;

        // 執行音效廣播並更新計時器
        if (gpGlobals->time >= m_flNextBeep)
        {
            m_flNextBeep = gpGlobals->time + 1.4f;
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, m_sBeepName, 1.0f, m_fAttenu, 0, PITCH_NORM);

            // 當播放音效時，同步在 C4 頂部（Z軸 +5碼）拉起一條 TE_GLOWSPRITE 閃爍紅光網路消息
            if (gpGlobals->time >= m_flNextBlink)
            {
                m_flNextBlink = gpGlobals->time + 2.0f;
                MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
                WRITE_BYTE(TE_GLOWSPRITE);
                WRITE_COORD(pev->origin.x);
                WRITE_COORD(pev->origin.y);
                WRITE_COORD(pev->origin.z + 5.0f);
                WRITE_SHORT(g_sModelIndexC4Glow); // 閃爍發光精靈圖索引
                WRITE_BYTE(1);   // Life
                WRITE_BYTE(3);   // Size
                WRITE_BYTE(255); // Brightness
                MESSAGE_END();
            }
        }
    }
    else
    {
        // 狀態機旁路跳轉分支
        if (gpGlobals->time >= m_flNextBeep)
        {
            m_flNextBeep = gpGlobals->time + 1.4f;
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, m_sBeepName, 1.0f, m_fAttenu, 0, PITCH_NORM);
        }
        if (gpGlobals->time >= m_flNextBlink)
        {
            m_flNextBlink = gpGlobals->time + 2.0f;
            MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
            WRITE_BYTE(TE_GLOWSPRITE);
            WRITE_COORD(pev->origin.x);
            WRITE_COORD(pev->origin.y);
            WRITE_COORD(pev->origin.z + 5.0f);
            WRITE_SHORT(g_sModelIndexC4Glow);
            WRITE_BYTE(1);
            WRITE_BYTE(3);
            WRITE_BYTE(255);
            MESSAGE_END();
        }
    }

    // 4. 🚨 判定是否到達 35/45秒 爆炸臨界點 🚨
    if (gpGlobals->time >= m_flC4Blow)
    {
        // 如果綁定了包點地圖實體（func_bomb_target），向其發送引爆腳本通知
        if (m_pentCurBombTarget && !FNullEnt(m_pentCurBombTarget))
        {
            CBaseEntity *pBombTarget = CBaseEntity::Instance(m_pentCurBombTarget);
            if (pBombTarget)
            {
                pBombTarget->Use(CBaseEntity::Instance(pev->owner), this, USE_TOGGLE, 0.0f);
            }
        }

        // 防水中引爆 Bug：如果 C4 被扔進水裡，則直接失效回收；正常地表則轉向毀滅性高爆函數 Detonate2
        if (pev->waterlevel == WATERLEVEL_NOTINWATER)
        {
            SetThink(&CGrenade::Detonate2);
        }
        else
        {
            UTIL_Remove(this);
            return;
        }
    }

    // 5. 🛠️ 拆彈控制流（Defuse Mechanics）與反作弊判定 🛠️
    if (!m_bStartDefuse)
    {
        return;
    }

    CBasePlayer *pDefuser = (CBasePlayer *)CBaseEntity::Instance(m_pBombDefuser);
    if (!pDefuser)
    {
        return;
    }

    // 拆彈倒計時結束 ➔ 判定拆彈成功
    if (m_flDefuseCountDown <= gpGlobals->time)
    {
        if (pDefuser->IsAlive())
        {
            // 向全伺服器廣播拆彈成功與 MVP 玩家網路事件消息
            MESSAGE_BEGIN(MSG_ALL, gmsgSendAudio);
            WRITE_BYTE(9); // 內聯常量對齊
            WRITE_BYTE(2);
            WRITE_SHORT(pDefuser->entindex());
            WRITE_SHORT(0);
            WRITE_LONG(703); // 0x2af 對應網路數據包字串 ID
            MESSAGE_END();

            // 寫入標準 Linux 伺服器賽事日誌日誌
            UTIL_LogPrintf("\"%s<%d><%s><CT>\" triggered \"Defused_The_Bomb\"\n", 
                           STRING(pDefuser->pev->netname), pDefuser->GetUserId(), pDefuser->GetPlayerAuthId());

            // 關閉滴答聲，播放拆彈成功的無窮無線廣播
            UTIL_EmitAmbientSound(ENT(pev), pev->origin, "weapons/c4_beep5.wav", 0.0f, 0.0f, SND_STOP, 0);
            EMIT_SOUND_DYN(ENT(pDefuser->pev), CHAN_WEAPON, "weapons/c4_disarmed.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);

            // 回收 C4 實體，解鎖玩家速度與拆彈狀態
            UTIL_Remove(this);
            m_bJustBlew = true;

            pDefuser->ResetMaxSpeed();
            pDefuser->m_bIsDefusing = false; // 由私有指針偏移量還原的狀態標記
            m_pBombDefuser = nullptr;
            m_bStartDefuse = false;
            return;
        }

        // 拆彈者中途死亡
        pDefuser->ResetMaxSpeed();
        pDefuser->m_bIsDefusing = false;
        m_bStartDefuse = false;
        m_pBombDefuser = nullptr;
        return;
    }

    // 6. 🚨 重要：反作弊與腳步防呆（空中拆彈攔截）🚨
    // 檢查玩家是否因外掛或被手榴彈衝擊波炸飛、離開了地面（FL_ONGROUND 消失）
    if (gpGlobals->time >= m_flNextDefuse)
    {
        if (pev->flags & FL_ONGROUND)
        {
            return; // 狀態正常，繼續拆彈
        }
    }
    else if (pev->flags & FL_ONGROUND)
    {
        return;
    }

    // 如果玩家雙腳離地，強制中斷拆彈，移除進度條，並在左下角印出經典提示
    ClientPrint(pDefuser->pev, ::clprintf_type::crt_print_center, "#C4_Defuse_Must_Be_On_Ground");

    pDefuser = (CBasePlayer *)CBaseEntity::Instance(m_pBombDefuser);
    pDefuser->ResetMaxSpeed();
    pDefuser->m_bIsDefusing = false;
    
    pDefuser->SetProgressBarTime(0); // 清空屏幕正中央的拆彈時間條
    m_pBombDefuser = nullptr;
    m_bStartDefuse = false;
    m_flDefuseCountDown = 0.0f;
}

void CGrenade::C4Touch(CBaseEntity *pOther)
{
    // C4 炸彈在掉落地面、卡在角落或與任何玩家/實體發生碰撞時，不做任何物理反彈、引爆或觸發回呼。
    return;
}

void CGrenade::DangerSoundThink(void)
{
    // 1. 安全邊界判定：若投擲物掉出地圖範圍之外，立即進行記憶體回收
    if (!IsInWorld())
    {
        UTIL_Remove(this);
        return;
    }

    // 2. 計算當前的物理速度純量
    float flSpeedSq = pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y + pev->velocity.z * pev->velocity.z;
    float flSpeed = sqrtf(flSpeedSq);

    // 3. 預測 0.5 秒後的飛行/滾動落點位置
    Vector vecPredictPos = pev->origin + pev->velocity * 0.5f;

    // 4. 在預測落點上注入一個 bits_SOUND_DANGER (0x20) 危險聲音標記
    // 聲音威脅半徑與當前投擲物的「移動速度」成正比，持續 0.2 秒
    CSoundEnt::InsertSound(bits_SOUND_DANGER, vecPredictPos, (int)flSpeed, 0.2f);

    // 5. 設定本思考函數的更新步進率為每 0.2 秒一次
    pev->nextthink = gpGlobals->time + 0.2f;

    // 6. 水中物理阻尼判定：如果投擲物進入水中，速度立即強制折半（乘上 0.5）
    if (pev->waterlevel != WATERLEVEL_NOTINWATER)
    {
        pev->velocity = pev->velocity * 0.5f;
    }
}

void CGrenade::Detonate(void)
{
    // 1. 向下打一條 40 單位的射線獲取地面/牆面法線與細節
    Vector vecSrc = pev->origin + Vector(0.0f, 0.0f, 8.0f);
    Vector vecEnd = vecSrc - Vector(0.0f, 0.0f, 40.0f);
    TraceResult tr;
    UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 2. 呼叫核心爆炸傷害與視覺效果函數，bitsDamageType 傳入 0x40 (DMG_BLAST，高爆衝擊波傷害)
    Explode(&tr, DMG_BLAST);

    // 3. 🚨 CZDS（刪除片段）獨家劇情戰術擴展 🚨
    // 動態在地圖上生成一個專屬的 "weapon_effectspoint"（武器 AI 感知標記點實體）
    // 注入爆炸物類型標記（4 = FLASH / GRENADE）以及投擲者陣營（m_throwerClasstype），
    // 用於即時通知單人關卡中的 NPC/Bot 判定該區域發生了劇烈爆炸或劇烈閃光，進而讓敵人 AI 產生耳鳴、硬直或向掩體逃跑。
    edict_t *pEffectEnt = CREATE_NAMED_ENTITY(MAKE_STRING("weapon_effectspoint"));
    if (pEffectEnt && !FNullEnt(pEffectEnt))
    {
        pEffectEnt->v.origin = pev->origin;
        pEffectEnt->v.angles = pev->angles;

        // 依據私有數據偏移量還原 CWeaponEffectsPoint 內部成員屬性
        int *pPrivate = (int *)GET_PRIVATE(pEffectEnt);
        pPrivate[0x68 / sizeof(int)] = 4;                          // Type 4 = FLASH / GRENADE
        pPrivate[0x6c / sizeof(int)] = this->m_throwerClasstype;   // 傳遞投擲者陣營 ID

        DispatchSpawn(pEffectEnt);
        pEffectEnt->v.owner = ENT(pev);
        return;
    }

    g_engfuncs.pfnAlertMessage(at_console, "NULL Ent in Smoke Grenade!\n");
}

void CGrenade::Detonate2(void)
{
    // 1. 向下打一條 40 單位的射線獲取地面/牆面法線與細節
    Vector vecSrc = pev->origin + Vector(0.0f, 0.0f, 8.0f);
    Vector vecEnd = vecSrc - Vector(0.0f, 0.0f, 40.0f);
    TraceResult tr;
    UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 2. 呼叫 C4 特有的核心爆炸傷害與視覺效果函數 Explode2
    // bitsDamageType 傳入 0x40 (DMG_BLAST，高爆衝擊波傷害)
    Explode2(&tr, DMG_BLAST);
}

void CGrenade::Detonate3(void)
{
    // 1. 向下打一條 40 單位的射線獲取地面/牆面法線與細節
    Vector vecSrc = pev->origin + Vector(0.0f, 0.0f, 8.0f);
    Vector vecEnd = vecSrc - Vector(0.0f, 0.0f, -40.0f);
    TraceResult tr;
    UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 2. 呼叫特有的爆炸核心函數 Explode3
    // bitsDamageType 傳入 0x1000000 (DMG_WALL_BLAST / DMG_NEVERGIB，特有場景或不碎屍高爆傷害)
    Explode3(&tr, DMG_NEVERGIB);
}

void CGrenade::DetonateUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    // 將 Think 指針無縫切換到通用引爆控制流 Detonate
    SetThink(&CGrenade::Detonate);

    // 立即（在下一個引擎刻度/幀）觸發引爆
    pev->nextthink = gpGlobals->time;
}

void CGrenade::Explode(TraceResult *pTrace, int bitsDamageType)
{
    // 1. 將實體進行隱形並關閉實體碰撞、關閉受傷判定，防止重複引爆
    pev->model = 0;
    pev->solid = SOLID_NOT;
    pev->takedamage = DAMAGE_NO;

    // 2. 貼地/貼牆位置修正：如果撞擊到了表面，根據法線方向動態微調它的著落點位置
    if (pTrace->flFraction != 1.0f)
    {
        float flOffset = pev->dmg - 24.0f;
        if (flOffset < 4.0f)
        {
            flOffset = 4.0f; // 確保有最小安全向外推開間距
        }
        pev->origin = pTrace->vecEndPos + pTrace->vecPlaneNormal * flOffset * 0.6f;
    }

    // 3. 取得著落點的環境內容材質
    int iContents = UTIL_PointContents(pev->origin);

    // 4. 在聲音系統中插入戰鬥/爆炸聲音標記，用於吸引周圍 NPC 與 AI Bot
    CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 1024, 3.0f);

    // 5. 提取投擲者（owner）的變數並解除所有權綁定
    entvars_t *pevOwner = nullptr;
    if (pev->owner && !FNullEnt(pev->owner))
    {
        pevOwner = VARS(pev->owner);
    }
    pev->owner = nullptr;

    // 6. 🚨 反恐精英核心致盲機制：調用 RadiusFlash 計算全域玩家白屏強烈度 🚨
    // 傳入的常數 0x40800000 即為浮點數 4.0f（控制致盲持續時間的基礎權重）
    RadiusFlash(pev->origin, pevOwner, bitsDamageType, 4.0f);

    // 7. 隨機在撞擊的牆面或地面上噴塗 SCORCH (焦黑) 彈孔貼圖痕跡 (0x0b 或 0x0c)
    if (RANDOM_FLOAT(0.0f, 1.0f) >= 0.5f)
    {
        UTIL_DecalTrace(pTrace, DECAL_SCORCH1);
    }
    else
    {
        UTIL_DecalTrace(pTrace, DECAL_SCORCH2);
    }

    // 8. 隨機播放拔插銷爆裂的初音音效
    const char *sample = (RANDOM_LONG(0, 1) == 0) ? "weapons/flashbang-2.wav" : "weapons/flashbang-1.wav";
    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, sample, 0.55f, ATTN_NORM, 0, PITCH_NORM);

    // 9. 速度歸零、附加 EF_NODRAW 不渲染標記，並在 0.3 秒後無縫轉向 Smoke 思考函數放煙
    pev->effects |= EF_NODRAW; // 0x80
    pev->velocity = g_vecZero;
    
    SetThink(&CGrenade::Smoke);
    pev->nextthink = gpGlobals->time + 0.3f;

    // 10. 如果不在水裡，隨機生成 1~3 組火花效果 (spark_shower) 增強視覺
    if (iContents != CONTENTS_WATER)
    {
        int sparkCount = RANDOM_LONG(0, 3);
        for (int i = 0; i < sparkCount; i++)
        {
            CBaseEntity::Create("spark_shower", pev->origin, pTrace->vecPlaneNormal, nullptr);
        }
    }
}

void CGrenade::Explode(Vector vecSrc, Vector vecAim)
{
    // 1. 從手榴彈當前中心點向下打一條 32 單位的物理射線獲取著落地點细節
    Vector vecEnd = pev->origin - Vector(0.0f, 0.0f, 32.0f);
    TraceResult tr;
    UTIL_TraceLine(pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 2. 重載調用上一步重構的 Explode(TraceResult*, int) 進行實際的致盲殺傷與粒子發射
    // bitsDamageType 傳入 0x40 (DMG_BLAST，高爆衝擊波傷害)
    Explode(&tr, DMG_BLAST);
}

void CGrenade::Explode2(TraceResult *pTrace, int bitsDamageType)
{
    // 1. 將 C4 實體隱形並關閉實體碰撞、關閉受傷判定，防止重複引爆
    pev->model = 0;
    pev->solid = SOLID_NOT;
    pev->takedamage = DAMAGE_NO;

    // 2. 劇烈震動周圍所有玩家的螢幕（C4 特有的終局震撼效果）
    // 參數：震動中心點、震動幅度 (25.0f)、震動頻率 (150.0f)、持續時間 (1.0f)、影響半徑 (3000.0f)
    UTIL_ScreenShake(&pTrace->vecEndPos, 25.0f, 150.0f, 1.0f, 3000.0f);
    m_bJustBlew = true;

    // 3. 位置修正：防止 C4 的爆炸中心點嵌進地圖牆壁或障礙物內部
    if (pTrace->flFraction != 1.0f)
    {
        float flOffset = pev->dmg - 24.0f;
        pev->origin = pTrace->vecEndPos + pTrace->vecPlaneNormal * flOffset * 0.6f;
    }

    int iContents = UTIL_PointContents(pev->origin);

    // 4. 🚀 核心網路消息：渲染 C4 專屬的「超巨型複合連鎖爆炸火球群」 🚀
    // 火球 1：主中心巨型黑煙火球 (g_sModelIndexFireball3)
    MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
    WRITE_BYTE(TE_EXPLOSION);
    WRITE_COORD(pev->origin.x);
    WRITE_COORD(pev->origin.y);
    WRITE_COORD(pev->origin.z - 10.0f);
    WRITE_SHORT(g_sModelIndexFireball3);
    WRITE_BYTE((int)((pev->dmg - 275.0f) * 0.6f)); // 依據 C4 威力動態縮放粒子尺寸
    WRITE_BYTE(150);                              // Framerate (0x96)
    MESSAGE_END();

    // 火球 2：隨機向外擴散的次級烈焰火球 (g_sModelIndexFireball2)
    MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
    WRITE_BYTE(TE_EXPLOSION);
    WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-512.0f, 512.0f));
    WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-512.0f, 512.0f));
    WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10.0f, 10.0f));
    WRITE_SHORT(g_sModelIndexFireball2);
    WRITE_BYTE((int)((pev->dmg - 275.0f) * 0.6f));
    WRITE_BYTE(150);
    MESSAGE_END();

    // 火球 3：外圍二次產生的巨型碎裂火球 (g_sModelIndexFireball3)
    MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
    WRITE_BYTE(TE_EXPLOSION);
    WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-512.0f, 512.0f));
    WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-512.0f, 512.0f));
    WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10.0f, 10.0f));
    WRITE_SHORT(g_sModelIndexFireball3);
    WRITE_BYTE((int)((pev->dmg - 275.0f) * 0.6f));
    WRITE_BYTE(150);
    MESSAGE_END();

    // 火球 4：底層擴散的小型衝擊波火球 (g_sModelIndexFireball)
    MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
    WRITE_BYTE(TE_EXPLOSION);
    WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-512.0f, 512.0f));
    WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-512.0f, 512.0f));
    WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10.0f, 10.0f));
    WRITE_SHORT(g_sModelIndexFireball);
    WRITE_BYTE((int)((int)((pev->dmg - 275.0f) * 0.6f)));
    WRITE_BYTE(17); // Framerate (0x11)
    MESSAGE_END();

    // 5. 播放轟天巨響的 C4 專屬爆炸音效，並向 AI 系統傳遞最高級別的戰鬥威脅聲
    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/c4_explode1.wav", 1.0f, ATTN_NONE, 0, PITCH_NORM);
    CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 1024, 3.0f);

    // 6. 解除所有權綁定（後面代碼在截斷處通常會調用 RadiusDamage 判定巨量範圍致死殺傷）
    entvars_t *pevOwner = nullptr;
    if (pev->owner && !FNullEnt(pev->owner))
    {
        pevOwner = VARS(pev->owner);
    }
    pev->owner = nullptr;
}

void CGrenade::Explode3(TraceResult *pTrace, int bitsDamageType)
{
    // 1. 將實體隱形、關閉碰撞盒與受傷判定，鎖定引爆狀態
    pev->model = 0;
    pev->solid = SOLID_NOT;
    pev->takedamage = DAMAGE_NO;

    // 2. 位置修正：防止引爆點中心卡進地圖牆壁或不規則刷子幾何體內部
    if (pTrace->flFraction != 1.0f)
    {
        float flOffset = pev->dmg - 24.0f;
        pev->origin = pTrace->vecEndPos + pTrace->vecPlaneNormal * flOffset * 0.6f;
    }

    // 3. 獲取引爆點材質，只要不在水裡 (-3)，就呼叫 g_usExplosion 全域網路事件渲染劇本場景爆炸效果
    int iContents = UTIL_PointContents(pev->origin);
    if (iContents != CONTENTS_WATER)
    {
        PLAYBACK_EVENT_FULL(FEV_RELIABLE, nullptr, g_usExplosion, 0.0f, pev->origin, g_vecZero, 0.0f, 0.0f, 0, 0, 0, 0);
    }

    // 4. 在聲音系統中向 AI 網絡注入 bits_SOUND_COMBAT (1) 戰鬥威脅標記
    CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 1024, 3.0f);

    // 5. 提取投擲者（owner）指針並解除實體所有權綁定
    entvars_t *pevOwner = nullptr;
    if (pev->owner && !FNullEnt(pev->owner))
    {
        pevOwner = VARS(pev->owner);
    }
    pev->owner = nullptr;

    // 6. 🚨 關卡劇情判定：對周圍實體與牆壁機關實施範圍殺傷 🚨
    RadiusDamage(pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType);

    // 7. 在爆開的牆面或幾何體上噴塗焦黑痕跡貼圖 (0x0b 或 0x0c)
    if (RANDOM_FLOAT(0.0f, 1.0f) >= 0.5f)
    {
        UTIL_DecalTrace(pTrace, DECAL_SCORCH1);
    }
    else
    {
        UTIL_DecalTrace(pTrace, DECAL_SCORCH2);
    }

    // 8. 隨機播放專屬的建築瓦礫、碎磚掉落與碎裂音效
    const char *sample = nullptr;
    switch (RANDOM_LONG(0, 2))
    {
        case 0: sample = "weapons/debris1.wav"; break;
        case 1: sample = "weapons/debris2.wav"; break;
        case 2: sample = "weapons/debris3.wav"; break;
    }

    if (sample)
    {
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, sample, 0.55f, ATTN_NORM, 0, PITCH_NORM);
    }

    // 9. 物理速度歸零，附加 EF_NODRAW (0x80) 標記，並在 0.55 秒後將思考指針導向放煙尾跡 Smoke3_C
    pev->effects |= EF_NODRAW;
    pev->velocity = g_vecZero;

    SetThink(&CGrenade::Smoke3_C);
    pev->nextthink = gpGlobals->time + 0.55f;
}

void CGrenade::ExplodeTouch(CBaseEntity *pOther)
{
    // 1. 紀錄直接撞擊的受害者（enemy），作為後續特定 AI 腳本的判定依據
    pev->enemy = pOther->edict();

    // 2. 獲取當前投擲物的物理速度純量
    float flSpeedSq = pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y + pev->velocity.z * pev->velocity.z;
    float flSpeed = sqrtf(flSpeedSq);

    Vector vecStart;
    
    // 3. 沿著飛行軌跡的方向向量「向後退 32 碼」作為追蹤線的起點，防止引爆射線陷入物體內部
    if (flSpeed == 0.0f)
    {
        vecStart = pev->origin - Vector(0.0f, 0.0f, 32.0f);
    }
    else
    {
        Vector vecDir = pev->velocity / flSpeed;
        vecStart = pev->origin - (vecDir * 32.0f);
    }

    // 4. 再次取得當前速度並計算朝飛行方向「向前延伸 64 碼」作為追蹤線的終點
    float flSpeedSq2 = pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y + pev->velocity.z * pev->velocity.z;
    float flSpeed2 = sqrtf(flSpeedSq2);

    Vector vecEnd;
    if (flSpeed2 == 0.0f)
    {
        vecEnd = vecStart + Vector(0.0f, 0.0f, 64.0f);
    }
    else
    {
        Vector vecDir2 = pev->velocity / flSpeed2;
        vecEnd = vecStart + (vecDir2 * 64.0f);
    }

    // 5. 實施貫穿引爆點的射線追蹤，精確抓取撞擊表面的法線（vecPlaneNormal）與 Fraction
    TraceResult tr;
    UTIL_TraceLine(vecStart, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 6. 重載調用劇情破壞核心 Explode3，傳入 0x40 (DMG_BLAST，高爆衝擊波傷害)
    Explode3(&tr, DMG_BLAST);
}

void CGrenade::Killed(entvars_t *pevAttacker, int iGib)
{
    // 1. 向下打一條 40 單位的射線獲取地面/牆面法線與細節
    Vector vecSrc = pev->origin + Vector(0.0f, 0.0f, 8.0f);
    Vector vecEnd = vecSrc - Vector(0.0f, 0.0f, 40.0f);
    TraceResult tr;
    UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 2. 當手榴彈被子彈、爆炸或其他傷害「強行摧毀 (Killed)」時，立即被動觸發殉爆
    // bitsDamageType 傳入 0x40 (DMG_BLAST，高爆衝擊波傷害)
    Explode(&tr, DMG_BLAST);

    // 3. 🚨 CZDS（刪除片段）獨家劇情戰術擴展 🚨
    // 即使是被打爆，依然要在地圖上生成專屬的 "weapon_effectspoint"（武器 AI 感知標記點實體）
    // 注入爆炸物類型標記（4 = FLASH / GRENADE）以及投擲者陣營（m_throwerClasstype），
    // 確保周圍的 NPC/Bot AI 依然能即時修正視覺感知線，判定此處發生了高爆威脅。
    edict_t *pEffectEnt = CREATE_NAMED_ENTITY(MAKE_STRING("weapon_effectspoint"));
    if (pEffectEnt && !FNullEnt(pEffectEnt))
    {
        pEffectEnt->v.origin = pev->origin;
        pEffectEnt->v.angles = pev->angles;

        // 依據私有數據偏移量還原 CWeaponEffectsPoint 內部成員屬性
        int *pPrivate = (int *)GET_PRIVATE(pEffectEnt);
        pPrivate[0x68 / sizeof(int)] = 4;                          // Type 4 = FLASH / GRENADE
        pPrivate[0x6c / sizeof(int)] = this->m_throwerClasstype;   // 傳遞投擲者陣營 ID

        DispatchSpawn(pEffectEnt);
        pEffectEnt->v.owner = ENT(pev);
        return;
    }

    g_engfuncs.pfnAlertMessage(at_console, "NULL Ent in Smoke Grenade!\n");
}

int CGrenade::ObjectCaps(void)
{
    if (m_bIsC4)
    {
        return FCAP_ACROSS_TRANSITION; // 0x10：允許 C4 在單人關卡切換地圖時被安全保留並轉移
    }

    return 0; // 普通手榴彈、閃光彈、煙霧彈在切換地圖時直接銷毀，不進行跨關卡轉移
}

void CGrenade::PreDetonate(void)
{
    // 1. 在爆炸前一刻，於當前原點注入一個半徑 400 單位的 bits_SOUND_DANGER (0x20) 危險聲音標記
    // 藉此強行驅散或驚嚇周圍的 Bot/NPC，使其做出閃避動作
    CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, 400, 0.3f);

    // 2. 將 Think 狀態機指向核心引爆函數 Detonate
    SetThink(&CGrenade::Detonate);

    // 3. 延遲 1.0 秒後正式引爆，為周圍 AI 敵人生存留出最後的反應時間窗
    pev->nextthink = gpGlobals->time + 1.0f;
}

int CGrenade::Save(CSave *save)
{
    if (!CBaseMonster::Save(save))
    {
        return 0;
    }

    return save->WriteFields("CGrenade", this, m_SaveData, 16); // 0x10 對應表格中擁有 16 個序列化變數欄位
}

int CGrenade::Restore(CRestore *restore)
{
    if (!CBaseMonster::Restore(restore))
    {
        return 0;
    }

    return restore->ReadFields("CGrenade", this, m_SaveData, 16);
}

CGrenade *CGrenade::ShootSmokeGrenade(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent)
{
    edict_t *pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));
    if (!pEntity)
    {
        return nullptr;
    }

    CGrenade *pGrenade = (CGrenade *)GET_PRIVATE(pEntity);
    if (!pGrenade)
    {
        pGrenade = (CGrenade *)pfnPvAllocEntPrivateData(pEntity, sizeof(CGrenade)); // 0x3a0 位元組
        pGrenade->CBaseMonster::CBaseMonster();
        pGrenade->pev = &pEntity->v;
    }

    pGrenade->pev->movetype = MOVETYPE_BOUNCE;
    pGrenade->pev->classname = MAKE_STRING("grenade");
    pGrenade->pev->solid = SOLID_BBOX;

    SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl");
    UTIL_SetSize(pGrenade->pev, g_vecZero, g_vecZero);

    pGrenade->pev->dmg = 30.0f;
    pGrenade->m_bIsC4 = false;
    pGrenade->m_fRegisteredSound = FALSE;

    UTIL_SetOrigin(pGrenade->pev, vecStart);
    pGrenade->pev->velocity = vecVelocity;
    pGrenade->pev->angles = pevOwner->angles;
    pGrenade->pev->owner = ENT(pevOwner);

    pGrenade->m_bLightSmoke = false;
    pGrenade->m_bDetonated = false;
    pGrenade->m_bJustBlew = true;
    pGrenade->m_SGSmoke = 0;
    pGrenade->m_usEvent = usEvent;

    SetTouch(&CGrenade::BounceTouch);
    pGrenade->pev->dmgtime = gpGlobals->time + time;

    SetThink(&CGrenade::SG_TumbleThink);
    pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

    if (time < 0.1f)
    {
        pGrenade->pev->nextthink = gpGlobals->time;
        pGrenade->pev->velocity = g_vecZero;
    }

    pGrenade->pev->sequence = RANDOM_LONG(3, 6);
    pGrenade->pev->framerate = 1.0f;
    pGrenade->pev->gravity = 0.5f;
    pGrenade->pev->friction = 0.8f;

    SET_MODEL(ENT(pGrenade->pev), "models/w_smokegrenade.mdl");
    pGrenade->pev->dmg = 35.0f;

    return pGrenade;
}

void CGrenade::SG_TumbleThink(void)
{
    if (!IsInWorld())
    {
        UTIL_Remove(this);
        return;
    }

    if (pev->flags & FL_ONGROUND)
    {
        pev->velocity = pev->velocity * 0.95f;
    }

    StudioFrameAdvance();
    pev->nextthink = gpGlobals->time + 0.1f;

    if (pev->dmgtime - 1.0f < gpGlobals->time)
    {
        float flTimeLeft = pev->dmgtime - gpGlobals->time;
        Vector vecPredictPos = pev->origin + pev->velocity * flTimeLeft;

        CSoundEnt::InsertSound(bits_SOUND_DANGER, vecPredictPos, 400, 0.1f);
    }

    if (pev->dmgtime <= gpGlobals->time && (pev->flags & FL_ONGROUND))
    {
        SetThink(&CGrenade::SG_Detonate);
    }

    if (pev->waterlevel != WATERLEVEL_NOTINWATER)
    {
        pev->velocity = pev->velocity * 0.5f;
        pev->framerate = 0.2f;
    }
}

void CGrenade::SG_Smoke(void)
{
    // 檢查煙霧彈當前的環境材質，-3 代表 CONTENTS_WATER (水中)
    if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
    {
        Vector vecMins = pev->origin - Vector(64.0f, 64.0f, 64.0f);
        Vector vecMaxs = pev->origin + Vector(64.0f, 64.0f, 64.0f);

        // 如果在水裡，不釋放煙霧粒子，而是呼叫引擎生成大量上升的水泡效果
        UTIL_Bubbles(vecMins, vecMaxs, 64);
    }
    else
    {
        UTIL_MakeVectors(pev->angles);

        // 隨機擴散半徑與亂數粒子生命週期
        float flRadius = RANDOM_FLOAT(3.0f, 8.0f);
        float flLife = RANDOM_FLOAT(1.5f, 3.5f);

        // 每次放煙時，以 30 度 (0x1e) 為單位遞增旋轉角度，確保煙霧均勻向四周擴散
        m_angle = (m_angle + 30) % 360;

        double dSin, dCos;
        sincos((double)(m_angle * M_PI / 180.0f), &dSin, &dCos);

        float flVelX = gpGlobals->v_forward.x * flRadius;
        float flVelY = gpGlobals->v_forward.y * flRadius;

        // 計算煙霧粒子在圓周運動下的多維度拋射分量
        float flPuffX = (float)(flVelX * dCos - dSin * flVelY);
        float flPuffY = (float)(flVelX * dSin + flVelY * dCos);

        // 調用我們在 Precache 事先預載的 events/createsmoke.sc 進行全域網路事件廣播
        PLAYBACK_EVENT_FULL(0, nullptr, m_usEvent, 0.0f, pev->origin, g_vecZero, flPuffX, flPuffY, (int)(flLife * 100.0f), 0, m_bLightSmoke, 6);
    }

    // 總共放煙 21 次 (0x15)，每次間隔 1.0 秒
    if (m_SGSmoke < 21)
    {
        m_SGSmoke++;
        pev->nextthink = gpGlobals->time + 1.0f;
        SetThink(&CGrenade::SG_Smoke);
        return;
    }

    // 21 秒時間到，將實體標記為不渲染隱形狀態 (EF_NODRAW = 0x80)，並回收記憶體
    pev->effects |= EF_NODRAW;
    UTIL_Remove(this);
}

void CGrenade::SlideTouch(CBaseEntity *pOther)
{
    // 防止手榴彈滑動時與投擲者本人（owner）發生碰撞
    if (pev->owner == pOther->pev->pContainingEntity)
    {
        return;
    }

    if ((pev->flags & FL_ONGROUND) == 0)
    {
        // 如果手榴彈還在空中（撞牆或擦過邊緣），播放專屬的反彈撞擊聲 (BounceSound)
        BounceSound();
        
        if (pev->dmg <= 50.0f)
        {
            // 威力小於等於 50 的投擲物（如閃光彈、煙霧彈），隨機播放輕型砸牆音效
            const char *sample = nullptr;
            switch (RANDOM_LONG(0, 2))
            {
                case 0: sample = "weapons/grenade_hit1.wav"; break;
                case 1: sample = "weapons/grenade_hit2.wav"; break;
                case 2: sample = "weapons/grenade_hit3.wav"; break;
            }
            if (sample)
            {
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, sample, 0.25f, ATTN_NORM, 0, PITCH_NORM);
            }
        }
        else
        {
            // 威力大於 50 的重型高爆破片手榴彈（HE Grenade），播放沉重的破片反彈音效
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/he_bounce-1.wav", 0.25f, ATTN_NORM, 0, PITCH_NORM);
        }
    }
    else
    {
        // 如果已經完全落地的滑行（Slide）狀態，給予 5% 的常規滾動摩擦力減速
        pev->velocity = pev->velocity * 0.95f;
    }
}

void CGrenade::BounceTouch(CBaseEntity *pOther)
{
    // 防止投擲物在剛扔出時立刻與投擲者本人（owner）發生碰撞
    if (pOther->pev->pContainingEntity == pev->owner)
    {
        return;
    }

    // 檢查碰撞物體是否為可破壞玻璃或木箱 (func_breakable)
    if (FClassnameIs(pOther->pev, "func_breakable") && pOther->pev->rendermode != kRenderNormal)
    {
        // 如果是特定透明度或半透明的可破壞玻璃，穿透並賦予反向反彈加成
        pev->velocity = pev->velocity * -1.5f;
        return;
    }

    // 當投擲物即將停止滾動（速度低於 60）且尚未註冊落地威脅時
    if (m_fRegisteredSound == 0)
    {
        float flSpeedSq = (pev->velocity.z * 0.7f) * (pev->velocity.z * 0.7f) + (pev->velocity.y * pev->velocity.y) + (pev->velocity.x * pev->velocity.x);
        float flSpeed = sqrtf(flSpeedSq);

        if (flSpeed <= 60.0f)
        {
            // 動態在聲音系統中插入一個危險威脅標記 bits_SOUND_DANGER，讓 NPC/Bot 判定該區域有即將爆炸的物體
            CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, (int)(pev->dmg / 0.4f), 0.3f);
            m_fRegisteredSound = 1;
        }
    }

    if ((pev->flags & FL_ONGROUND) == 0)
    {
        // 如果手榴彈還在空中（撞到牆壁），呼叫虛擬函數播放反彈撞擊聲 [0x8b] (BounceSound)
        BounceSound();
    }
    else
    {
        // 如果已經著地滾動，給予常規地面摩擦力衰減（速度乘上 0.8）
        pev->velocity = pev->velocity * 0.8f;
        pev->sequence = RANDOM_LONG(1, 1); // 切換至著地滾動的骨骼動畫序列
    }

    // 根據手榴彈當前的剩餘速度，動態縮放骨骼動畫的播放速率 (framerate)
    float flCurrentSpeed = pev->velocity.Length();
    pev->framerate = flCurrentSpeed / 200.0f;

    if (pev->framerate > 1.0f)
    {
        pev->framerate = 1.0f;
    }
    else if (pev->framerate < 0.5f)
    {
        pev->framerate = 0.0f; // 速度太慢時完全停止動畫播放
    }
}

void CGrenade::SG_Explode(TraceResult *pTrace, int bitsDamageType)
{
    // 將煙霧彈實體隱形並關閉實體碰撞、關閉受傷判定，防止重複引爆
    pev->model = 0;
    pev->solid = SOLID_NOT;
    pev->takedamage = DAMAGE_NO;

    // 如果煙霧彈撞擊到了表面 (Fraction != 1.0)，根據法線方向動態微調它的著落點位置
    if (pTrace->flFraction != 1.0f)
    {
        float flOffset = pev->dmg - 24.0f;
        pev->origin = pTrace->vecEndPos + pTrace->vecPlaneNormal * flOffset * 0.6f;
    }

    // 取得著落點的環境內容材質
    int iContents = UTIL_PointContents(pev->origin);

    // 在聲音系統中插入一個核心 bits_SOUND_COMBAT (戰鬥/爆炸聲音標記)，吸引周圍 NPC 與 AI Bot
    CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, 1024, 3.0f);

    // 解除與投擲者的所有權屬性綁定
    pev->owner = nullptr;

    // 隨機在撞擊的牆面或地面上噴塗 SCORCH (焦黑/塵土) 彈孔貼圖痕跡 (0x0b 或 0x0c)
    if (RANDOM_FLOAT(0.0f, 1.0f) >= 0.5f)
    {
        UTIL_DecalTrace(pTrace, DECAL_SCORCH1);
    }
    else
    {
        UTIL_DecalTrace(pTrace, DECAL_SCORCH2);
    }

    // 隨機播放 flashbang 音效 (雖然是煙霧彈，但引爆瞬間共用了拔插銷爆裂小音效)
    const char *sample = (RANDOM_LONG(0, 1) == 0) ? "weapons/flashbang-2.wav" : "weapons/flashbang-1.wav";
    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, sample, 0.55f, ATTN_NORM, 0, PITCH_NORM);

    // 進入核心放煙狀態！將 Think 指針無縫切換到剛才重構的動態圓周放煙函數 SG_Smoke
    SetThink(&CGrenade::SG_Smoke);
    pev->nextthink = gpGlobals->time + 0.1f;

    // 如果不在水裡 (iContents != CONTENTS_WATER)，隨機生成 1~3 組火花效果 (spark_shower) 模擬拉銷爆裂
    if (iContents != CONTENTS_WATER)
    {
        int sparkCount = RANDOM_LONG(0, 3);
        for (int i = 0; i < sparkCount; i++)
        {
            CBaseEntity::Create("spark_shower", pev->origin, pTrace->vecPlaneNormal, nullptr);
        }
    }
}

void CGrenade::SG_Detonate(void)
{
    // 1. 向下打一條 40 單位的射線獲取地面/牆面法線與細節
    Vector vecSrc = pev->origin + Vector(0.0f, 0.0f, 8.0f);
    Vector vecEnd = vecSrc - Vector(0.0f, 0.0f, 40.0f);
    TraceResult tr;
    UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, ENT(pev), &tr);

    // 2. 播放專屬的煙霧彈爆開聲音
    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/sg_explode.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);

    // 3. 核心優化算法：遍歷地圖上所有其他煙霧彈實體
    // 檢查是否有其他煙霧彈已經在附近 250 碼內且正處於生效狀態。如果是，則本顆煙霧彈改為「輕量化渲染（m_bLightSmoke = true）」，防止粒子重疊引發客戶端掉幀 FPS 暴跌。
    edict_t *pentFind = g_engfuncs.pfnFindEntityByString(nullptr, "classname", "grenade");
    while (pentFind && !FNullEnt(pentFind))
    {
        CGrenade *pOther = (CGrenade *)GET_PRIVATE(pentFind);
        if (pOther && pOther != this)
        {
            float flDist = (pOther->pev->origin - pev->origin).Length();
            if (flDist > 0.0f && flDist <= 250.0f && pOther->pev->dmgtime < gpGlobals->time)
            {
                m_bLightSmoke = true;
            }
        }
        pentFind = g_engfuncs.pfnFindEntityByString(pentFind, "classname", "grenade");
    }

    m_bDetonated = true;

    // 4. 調用全域網路事件廣播初次爆開的特效與煙霧渲染強度
    PLAYBACK_EVENT_FULL(0, nullptr, m_usEvent, 0.0f, pev->origin, g_vecZero, 0.0f, 0.0f, 0, 1, m_bLightSmoke, 0);

    // 5. 賦予手榴彈外殼隨機的物理彈跳速度與無規律翻滾角速度（模擬拔梢後的氣體反衝）
    pev->velocity.x = RANDOM_FLOAT(-175.0f, 175.0f);
    pev->velocity.y = RANDOM_FLOAT(-175.0f, 175.0f);
    pev->velocity.z = RANDOM_FLOAT(250.0f, 350.0f);

    pev->avelocity.x = RANDOM_FLOAT(150.0f, 450.0f);
    pev->avelocity.y = RANDOM_FLOAT(150.0f, 450.0f);
    pev->avelocity.z = RANDOM_FLOAT(150.0f, 450.0f);

    // 6. 將 Think 控制流直接移交給前幾步完成的圓周動態放煙主循環
    SetThink(&CGrenade::SG_Smoke);
    pev->nextthink = gpGlobals->time + 0.1f;

    // 7. 🚨 CZDS（刪除片段）獨家劇情戰術擴展 🚨
    // 動態在地圖上生成一個專屬的 "weapon_effectspoint"（武器AI感知標記點實體）
    // 注入煙霧彈類型標記（7 = SMOKE）以及投擲者陣營（m_throwerClasstype），
    // 用於實時遮蔽單人關卡中 NPC/Bot 的視覺感知線（Line of Sight），讓敵人 AI 丟失目標、停止射擊並觸發搜查腳本。
    edict_t *pEffectEnt = CREATE_NAMED_ENTITY(MAKE_STRING("weapon_effectspoint"));
    if (pEffectEnt && !FNullEnt(pEffectEnt))
    {
        pEffectEnt->v.origin = pev->origin;
        pEffectEnt->v.angles = pev->angles;

        // 依據私有數據偏移量還原 CWeaponEffectsPoint 內部成員屬性
        int *pPrivate = (int *)GET_PRIVATE(pEffectEnt);
        pPrivate[26] = 7;                          // Type 7 = SMOKE
        pPrivate[27] = this->m_throwerClasstype;   // 傳遞投擲者陣營 ID

        DispatchSpawn(pEffectEnt);
        pEffectEnt->v.owner = ENT(pev);
        return;
    }

    g_engfuncs.pfnAlertMessage(at_console, "NULL Ent in Smoke Grenade!\n");
}

CGrenade *CGrenade::ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
    edict_t *pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));
    if (!pEntity)
    {
        return nullptr;
    }

    // 分配與 CZDS 相同的 0x3a0 記憶體空間並呼叫建構子
    CGrenade *pGrenade = (CGrenade *)GET_PRIVATE(pEntity);
    if (!pGrenade)
    {
        pGrenade = (CGrenade *)pfnPvAllocEntPrivateData(pEntity, sizeof(CGrenade));
        pGrenade->CBaseMonster::CBaseMonster();
        pGrenade->pev = &pEntity->v;
    }

    pGrenade->pev->movetype = MOVETYPE_BOUNCE;  // 10
    pGrenade->pev->classname = MAKE_STRING("grenade");
    pGrenade->pev->solid = SOLID_BBOX;          // 2

    // 歷史化石：HLSDK 遺留的 M203 榴彈模型與初始包圍盒
    SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl");
    UTIL_SetSize(pGrenade->pev, g_vecZero, g_vecZero);

    pGrenade->pev->dmg = 30.0f;
    pGrenade->m_bIsC4 = false;
    pGrenade->m_fRegisteredSound = FALSE;

LAB_000c936f:
    pGrenade->pev->gravity = 0.5f;
    UTIL_SetOrigin(pGrenade->pev, vecStart);
    pGrenade->pev->velocity = vecVelocity;

    // 將飛行的速度向量轉化為朝向角度，使榴彈模型始終朝向飛行前方
    Vector vecAngles;
    UTIL_VecToAngles(pGrenade->pev->velocity, vecAngles);
    pGrenade->pev->angles = vecAngles;
    pGrenade->pev->owner = pevOwner->pContainingEntity;

    // 注入動態危險路徑預測（AI 避彈邏輯），即刻開始計算
    SetThink(&CGrenade::DangerSoundThink);
    pGrenade->pev->nextthink = gpGlobals->time;

    // 給予拋體沿著 X 軸（俯仰角）每秒 -100 到 -500 度的隨機旋轉特效
    pGrenade->pev->avelocity.x = RANDOM_FLOAT(-100.0f, -500.0f);

    // 🚨 接觸引爆核心：將 Touch 觸發回呼綁定至我們之前重構的 ExplodeTouch 🚨
    SetTouch(&CGrenade::ExplodeTouch);

    // 從 Condition Zero 核心技能平衡資料庫中直接讀取 M203 榴彈的傷害數值
    pGrenade->pev->dmg = gSkillData.plrDmgM203Grenade;
    pGrenade->m_bJustBlew = true;

    return pGrenade;
}

CGrenade *CGrenade::ShootSatchelCharge(entvars_t *pevOwner, Vector vecStart, Vector vecAngles)
{
    edict_t *pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));
    if (!pEntity)
    {
        return nullptr;
    }

    // 分配與 CZDS 相同的 0x3a0 記憶體空間並呼叫建構子
    CGrenade *pGrenade = (CGrenade *)GET_PRIVATE(pEntity);
    if (!pGrenade)
    {
        pGrenade = (CGrenade *)pfnPvAllocEntPrivateData(pEntity, sizeof(CGrenade));
        pGrenade->CBaseMonster::CBaseMonster();
        pGrenade->pev = &pEntity->v;
    }

    pGrenade->pev->movetype = MOVETYPE_FLY;     // 6：C4包安裝在牆面或地面時固定不產生重力下墜
    pGrenade->pev->classname = MAKE_STRING("grenade");
    pGrenade->pev->solid = SOLID_BBOX;          // 2

    // 載入包裝好的世界包點模型並重設 C4 專屬的對稱包圍盒大小
    SET_MODEL(ENT(pGrenade->pev), "models/w_c4.mdl");
    UTIL_SetSize(pGrenade->pev, g_vecZero, Vector(8.0f, 8.0f, 8.0f));

    // C4 基底設定
    pGrenade->pev->dmg = 100.0f; // 基礎常數，後續會被 c4_blow 控制變數覆蓋
    UTIL_SetOrigin(pGrenade->pev, vecStart);
    pGrenade->pev->velocity = g_vecZero; // 埋設後初始動能歸零
    pGrenade->pev->angles = vecAngles;
    pGrenade->pev->owner = pevOwner->pContainingEntity;

    // 🚨 核心狀態機初始化：將狀態鏈無縫綁定至我們之前攻克的 C4 邏輯 🚨
    SetThink(&CGrenade::C4Think);
    SetTouch(&CGrenade::C4Touch);

    pGrenade->pev->spawnflags |= SF_NORESPAWN; // 1：防刷新標記
    pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

    // 🚀 反恐精英 C4 專屬狀態機內嵌變數精確還原 🚀
    pGrenade->m_bIsC4 = true;
    pGrenade->m_iCurWave = 0;
    pGrenade->m_fAttenu = 0.0f;
    pGrenade->m_sBeepName = nullptr;
    
    pGrenade->m_flNextFreq = gpGlobals->time;
    pGrenade->m_flNextBeep = gpGlobals->time + 0.5f;
    pGrenade->m_flNextBlink = gpGlobals->time + 2.0f;
    
    pGrenade->m_flNextDefuse = 0.0f;
    pGrenade->m_bStartDefuse = false;
    pGrenade->m_bJustBlew = false;

    pGrenade->pev->friction = 0.9f;

    return pGrenade;
}

CGrenade *CGrenade::ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, float damage, int throwerClasstype)
{
    edict_t *pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));
    if (!pEntity)
    {
        return nullptr;
    }

    // 獲取私有數據指針，分配與 CZDS 相同的 0x3a0 (928位元組) 記憶體空間
    CGrenade *pGrenade = (CGrenade *)GET_PRIVATE(pEntity);
    if (!pGrenade)
    {
        pGrenade = (CGrenade *)pfnPvAllocEntPrivateData(pEntity, sizeof(CGrenade));
        pGrenade->CBaseMonster::CBaseMonster();
        pGrenade->pev = &pEntity->v;
    }

    pGrenade->pev->movetype = MOVETYPE_BOUNCE;  // 10
    pGrenade->pev->classname = MAKE_STRING("grenade");
    pGrenade->pev->solid = SOLID_BBOX;          // 2

    // 歷史化石：HLSDK 遺留的基礎模型載入與包圍盒大小重置
    SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl");
    UTIL_SetSize(pGrenade->pev, g_vecZero, g_vecZero);

    pGrenade->pev->dmg = 30.0f;
    pGrenade->m_bIsC4 = false;
    pGrenade->m_fRegisteredSound = FALSE;

LAB_000c9813:
    UTIL_SetOrigin(pGrenade->pev, vecStart);
    pGrenade->pev->velocity = vecVelocity;
    pGrenade->pev->angles = pevOwner->angles;
    pGrenade->pev->owner = ENT(pevOwner);

    // 設定定時爆炸的核心參數
    pGrenade->m_bJustBlew = true;
    pGrenade->m_throwerClasstype = throwerClasstype; // 記錄投擲者陣營，傳遞給 AI 感知點
    
    SetTouch(&CGrenade::BounceTouch);
    pGrenade->pev->dmgtime = gpGlobals->time + time;

    // 將普通定時物（閃光彈）的思考指針導向通用翻滾 TumbleThink
    SetThink(&CGrenade::TumbleThink);
    pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

    // 瞬間引爆防呆判定：若引爆時間極短，則立即原地引爆且速度歸零
    if (time < 0.1f)
    {
        pGrenade->pev->nextthink = gpGlobals->time;
        pGrenade->pev->velocity = g_vecZero;
    }

    // 動態隨機切換手榴彈翻滾的骨骼動畫序列
    pGrenade->pev->sequence = RANDOM_LONG(3, 6);
    pGrenade->pev->framerate = 1.0f;
    
    // 設定拋體的重力比重與空中/地面滑行摩擦力常數
    pGrenade->pev->gravity = 0.5f;
    pGrenade->pev->friction = 0.8f;

    // 關鍵魔改：粗暴將模型覆蓋成閃光彈世界模型，並注入由 CFlashbang 傳遞進來的實質威力/範圍數值
    SET_MODEL(ENT(pGrenade->pev), "models/w_flashbang.mdl");
    pGrenade->pev->dmg = damage; // 由引數傳入的 vecVelocity.x (即實際傷害/白屏強度常數)

    return pGrenade;
}

CGrenade *CGrenade::ShootTimed2(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time, int iTeam, unsigned short usEvent)
{
    edict_t *pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("grenade"));
    if (!pEntity)
    {
        return nullptr;
    }

    // 分配與 CZDS 相同的 0x3a0 記憶體空間並呼叫建構子
    CGrenade *pGrenade = (CGrenade *)GET_PRIVATE(pEntity);
    if (!pGrenade)
    {
        pGrenade = (CGrenade *)pfnPvAllocEntPrivateData(pEntity, sizeof(CGrenade));
        pGrenade->CBaseMonster::CBaseMonster();
        pGrenade->pev = &pEntity->v;
    }

    pGrenade->pev->movetype = MOVETYPE_BOUNCE;  // 10
    pGrenade->pev->classname = MAKE_STRING("grenade");
    pGrenade->pev->solid = SOLID_BBOX;          // 2

    // 歷史化石：HLSDK 遺留的模型加載
    SET_MODEL(ENT(pGrenade->pev), "models/grenade.mdl");
    UTIL_SetSize(pGrenade->pev, g_vecZero, g_vecZero);

    pGrenade->pev->dmg = 30.0f;
    pGrenade->m_bIsC4 = false;
    pGrenade->m_fRegisteredSound = FALSE;

LAB_000c9596:
    UTIL_SetOrigin(pGrenade->pev, vecStart);
    pGrenade->pev->velocity = vecVelocity;
    pGrenade->pev->angles = pevOwner->angles;
    pGrenade->pev->owner = ENT(pevOwner);

    // 儲存事件 ID 與陣營
    pGrenade->m_usEvent = usEvent; // 由暫存器優化錯位的 vecVelocity.y._0_2_ 還原
    pGrenade->m_iTeam = iTeam;     // 由暫存器優化錯位的 vecVelocity.x 還原

    SetTouch(&CGrenade::BounceTouch);
    pGrenade->pev->dmgtime = gpGlobals->time + time;

    // 將破片手榴彈的計時器思考指針同樣導向通用翻滾 TumbleThink
    SetThink(&CGrenade::TumbleThink);
    pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

    pGrenade->pev->sequence = RANDOM_LONG(3, 6);
    pGrenade->pev->framerate = 1.0f;
    pGrenade->m_bJustBlew = true;

    // 🚨 破片手榴彈（HE）專屬物理常數調校 🚨
    // 相比閃光彈/煙霧彈的 0.5 重力與 0.8 摩擦，HE 機榴彈擁有更高重力與更低滑行摩擦
    pGrenade->pev->gravity = 0.55f;
    pGrenade->pev->friction = 0.7f;

    // 正式加載世界模型並設定 175 的基礎爆炸威力/半徑
    SET_MODEL(ENT(pGrenade->pev), "models/w_hegrenade.mdl");
    pGrenade->pev->dmg = 175.0f;

    return pGrenade;
}

void CGrenade::Smoke(void)
{
    // 1. 取得爆炸中心點的環境材質，-3 代表 CONTENTS_WATER (水中)
    int iContents = UTIL_PointContents(pev->origin);

    if (iContents != CONTENTS_WATER)
    {
        // 2. 如果不在水裡，向附近所有的玩家廣播一條 TE_SMOKE (0x05) 臨時實體網路消息
        // 這會在閃光彈/手榴彈爆炸的中心點瞬間拉起一團由 g_sModelIndexSmoke 索引的標準爆炸灰色煙塵粒子
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
        WRITE_BYTE(TE_SMOKE);
        WRITE_COORD(pev->origin.x);
        WRITE_COORD(pev->origin.y);
        WRITE_COORD(pev->origin.z);
        WRITE_SHORT(g_sModelIndexSmoke);
        WRITE_BYTE(25);                  // Scale (0x19)：控制爆炸煙塵粒子的膨脹尺寸
        WRITE_BYTE(6);                   // Framerate：動畫撥放速率
        MESSAGE_END();

        // 3. 特效發射完畢，立即回收手榴彈實體記憶體
        UTIL_Remove(this);
        return;
    }

    // 4. 水中引爆機制：如果在水裡，則不發射空氣煙塵，改為呼叫引擎生成水下冒泡效果
    Vector vecMins = pev->origin - Vector(64.0f, 64.0f, 64.0f);
    Vector vecMaxs = pev->origin + Vector(64.0f, 64.0f, 64.0f);
    
    UTIL_Bubbles(vecMins, vecMaxs, 64);

    // 5. 特效發射完畢，回收記憶體
    UTIL_Remove(this);
}

void CGrenade::Smoke2(void)
{
    // 1. 取得爆炸中心點的環境材質，-3 代表 CONTENTS_WATER (水中)
    int iContents = UTIL_PointContents(pev->origin);

    if (iContents != CONTENTS_WATER)
    {
        // 2. 廣播 TE_SMOKE (0x05) 臨時實體網路消息，為 C4 渲染大爆炸後的尾隨粉塵
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
        WRITE_BYTE(TE_SMOKE);
        WRITE_COORD(pev->origin.x);
        WRITE_COORD(pev->origin.y);
        WRITE_COORD(pev->origin.z);
        WRITE_SHORT(g_sModelIndexSmoke);
        WRITE_BYTE(150);                 // Scale (0x96)：C4 專屬的超巨型爆炸煙塵尺寸（普通手榴彈僅為 25）
        WRITE_BYTE(8);                   // Framerate：更快的動畫播放速率
        MESSAGE_END();

        // 3. 特效發射完畢，立即回收 C4 實體記憶體
        UTIL_Remove(this);
        return;
    }

    // 4. 水中引爆機制：如果在水裡，則呼叫引擎生成水下冒泡效果
    Vector vecMins = pev->origin - Vector(64.0f, 64.0f, 64.0f);
    Vector vecMaxs = pev->origin + Vector(64.0f, 64.0f, 64.0f);
    
    UTIL_Bubbles(vecMins, vecMaxs, 64);

    // 5. 特效發射完畢，回收記憶體
    UTIL_Remove(this);
}

void CGrenade::Smoke3_A(void)
{
    // 1. 取得爆炸中心點的環境材質，-3 代表 CONTENTS_WATER (水中)
    int iContents = UTIL_PointContents(pev->origin);

    if (iContents != CONTENTS_WATER)
    {
        // 2. 廣播 TE_SMOKE (0x05) 臨時實體網路消息，為場景劇情爆破生成 A 組粉塵
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
        WRITE_BYTE(TE_SMOKE);

        // 在原始引爆點周圍進行 3D 隨機偏移（X/Y軸 +-128碼，Z軸 +-10碼），模擬大範圍碎屑不對稱擴散
        WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-128.0f, 128.0f));
        WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-128.0f, 128.0f));
        WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10.0f, 10.0f));
        
        WRITE_SHORT(g_sModelIndexSmoke);

        // 隨機動態縮放粉塵尺寸：基礎大小 15.0f 加上 0.0f 到 10.0f 的隨機抖動
        WRITE_BYTE((int)(15.0f + RANDOM_FLOAT(0.0f, 10.0f)));
        
        WRITE_BYTE(12); // Framerate (0x0c)：更平滑的動畫播放速率
        MESSAGE_END();

        // 特效發射完畢，回收實體記憶體
        UTIL_Remove(this);
        return;
    }

    // 3. 水中引爆防呆：如果在水裡，則呼叫引擎生成水下冒泡效果
    Vector vecMins = pev->origin - Vector(64.0f, 64.0f, 64.0f);
    Vector vecMaxs = pev->origin + Vector(64.0f, 64.0f, 64.0f);
    
    UTIL_Bubbles(vecMins, vecMaxs, 64);

    // 特效發射完畢，回收實體記憶體
    UTIL_Remove(this);
}

void CGrenade::Smoke3_B(void)
{
    // 1. 取得爆炸中心點的環境材質，-3 代表 CONTENTS_WATER (水中)
    int iContents = UTIL_PointContents(pev->origin);

    if (iContents == CONTENTS_WATER)
    {
        // 水中引爆防呆：如果在水裡，則呼叫引擎生成水下冒泡效果
        Vector vecMins = pev->origin - Vector(64.0f, 64.0f, 64.0f);
        Vector vecMaxs = pev->origin + Vector(64.0f, 64.0f, 64.0f);
        UTIL_Bubbles(vecMins, vecMaxs, 64);
    }
    else
    {
        // 2. 廣播 TE_SMOKE (0x05) 臨時實體網路消息，為場景劇情爆破生成 B 組粉塵
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
        WRITE_BYTE(TE_SMOKE);

        // 在原始引爆點周圍進行 3D 隨機偏移（X/Y軸 +-128碼，Z軸 +-10碼），營造粉塵大面積擴散感
        WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-128.0f, 128.0f));
        WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-128.0f, 128.0f));
        WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10.0f, 10.0f));
        
        WRITE_SHORT(g_sModelIndexSmoke);

        // 隨機動態縮放粉塵尺寸：基礎大小 15.0f 加上 0.0f 到 10.0f 的隨機抖動
        WRITE_BYTE((int)(15.0f + RANDOM_FLOAT(0.0f, 10.0f)));
        
        WRITE_BYTE(10); // Framerate：與 A 組 (12) 不同的動畫播放速率，製造視覺交錯感
        MESSAGE_END();
    }

    // 3. 🚨 核心連鎖設計：不要回收實體！將狀態機交棒給下一階段 🚨
    // 設定在 0.15 秒後，將思考指針導向 Smoke3_A，進一步延伸爆破粉塵的層次感
    pev->nextthink = gpGlobals->time + 0.15f;
    SetThink(&CGrenade::Smoke3_A);
}

void CGrenade::Spawn(void)
{
    pev->movetype = MOVETYPE_BOUNCE;  // 10
    pev->classname = MAKE_STRING("grenade");
    pev->solid = SOLID_BBOX;          // 2

    // 歷史化石痕跡：HLSDK 原生 9mmAR 下掛榴彈的模型載入
    SET_MODEL(ENT(pev), "models/grenade.mdl");
    UTIL_SetSize(pev, g_vecZero, g_vecZero);

    pev->dmg = 30.0f;
    m_bIsC4 = false;
    m_fRegisteredSound = FALSE;
}

void CGrenade::TumbleThink(void)
{
    if (!IsInWorld())
    {
        UTIL_Remove(this);
        return;
    }

    StudioFrameAdvance();
    pev->nextthink = gpGlobals->time + 0.1f;

    if (pev->dmgtime - 1.0f < gpGlobals->time)
    {
        float flTimeLeft = pev->dmgtime - gpGlobals->time;
        Vector vecPredictPos = pev->origin + pev->velocity * flTimeLeft;

        CSoundEnt::InsertSound(bits_SOUND_DANGER, vecPredictPos, 400, 0.1f);
        
        if (pev->dmgtime <= gpGlobals->time)
        {
            goto LAB_000c7691;
        }
    }
    else if (pev->dmgtime <= gpGlobals->time)
    {
LAB_000c7691:
        if (pev->dmg <= 40.0f)
        {
            // 威力小於等於 40 的投擲物（如閃光彈 4.0f），導向常規引爆（全域致盲與屏幕全白）
            SetThink(&CGrenade::Detonate);
        }
        else
        {
            // 威力大於 40 的重型高爆破片手榴彈（HE 175.0f），導向帶有不碎屍修正的場景破壞引爆
            SetThink(&CGrenade::Detonate3);
        }
    }

    if (pev->waterlevel != WATERLEVEL_NOTINWATER)
    {
        pev->velocity = pev->velocity * 0.5f;
        pev->framerate = 0.2f;
    }
}

void CGrenade::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    // 1. 安全過濾：只有 C4 炸彈實體才響應玩家按 E 鍵的 Use 行為（普通手榴彈在此處隱式 return）
    if (!m_bIsC4)
    {
        return;
    }

    if (!pActivator || !pActivator->IsPlayer())
    {
        return;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pActivator;

    // 2. 🚨 反恐精英核心防禦：只有反恐精英（CT）陣營允許對 C4 執行拆彈行為 🚨
    if (pPlayer->m_iTeam == CT)
    {
        // 3. 起手式判定：如果 C4 當前還沒有被任何人開始拆除
        if (!m_bStartDefuse)
        {
            // 將拆彈玩家的移動速度強制降為 1.0f（使其原地罰站無法移動）
            g_engfuncs.pfnSetClientMaxspeed(pPlayer->edict(), 1.0f);

            pPlayer->m_bIsDefusing = true;
            m_pBombDefuser = pPlayer;
            m_bStartDefuse = true;

            int iProgressBarTime = 10; // 預設沒有鉗子需要 10 秒

            // 4. 分流 A：擁有拆彈鉗（Defuse Kit）的 CT
            if (pPlayer->m_bHasDefuser == 1)
            {
                UTIL_LogPrintf("\"%s<%d><%s><CT>\" triggered \"Begin_Bomb_Defuse_With_Kit\"\n",
                               STRING(pPlayer->pev->netname), pPlayer->GetUserId(), pPlayer->GetPlayerAuthId());

                ClientPrint(pPlayer->pev, crt_print_center, "#Defusing_Bomb_With_Defuse_Kit");
                
                EMIT_SOUND_DYN(pPlayer->edict(), CHAN_ITEM, "weapons/c4_disarm.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);

                m_flDefuseCountDown = gpGlobals->time + 5.0f; // 拆彈倒計時 5 秒
                m_fNextDefuse = gpGlobals->time + 0.5f;       // 地面狀態安全檢驗間隔
                iProgressBarTime = 5;
            }
            // 5. 分流 B：赤手空拳沒有鉗子的 CT
            else
            {
                UTIL_LogPrintf("\"%s<%d><%s><CT>\" triggered \"Begin_Bomb_Defuse_Without_Kit\"\n",
                               STRING(pPlayer->pev->netname), pPlayer->GetUserId(), pPlayer->GetPlayerAuthId());

                ClientPrint(pPlayer->pev, crt_print_center, "#Defusing_Bomb_Without_Defuse_Kit");

                EMIT_SOUND_DYN(pPlayer->edict(), CHAN_ITEM, "weapons/c4_disarm.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);

                m_flDefuseCountDown = gpGlobals->time + 10.0f; // 拆彈倒計時 10 秒
                m_fNextDefuse = gpGlobals->time + 0.5f;
                iProgressBarTime = 10;
            }

            // 6. 全網消息同步：在 CT 玩家螢幕中央拉起對應秒數的進度條（Progress Bar）
            pPlayer->SetProgressBarTime(iProgressBarTime);
            return;
        }

        // 7. 持續按住階段：若已經在拆除中，每幀刷新地面防外掛計時檢驗窗
        m_fNextDefuse = gpGlobals->time + 0.5f;
    }
}

void CGrenade::UseSatchelCharges(entvars_t *pevOwner, SATCHELCODE code)
{
    if (pevOwner == nullptr)
    {
        return;
    }

    CBaseEntity *pOwner = CBaseEntity::Instance(pevOwner);
    if (!pOwner)
    {
        return;
    }

    // 1. 從投擲者身上提取其專屬的引實體序列號/索引 ID
    // 0x208（520位元組）偏移量精確鎖定了玩家的 CBasePlayer::entindex() 或特定 ID 欄位
    int iOwnerID = *(int *)((int)pOwner + 0x208);

    // 2. 遍歷地圖上所有的 "grenade" 實體
    edict_t *pentFind = g_engfuncs.pfnFindEntityByString(nullptr, "classname", "grenade");
    while (pentFind && !FNullEnt(pentFind))
    {
        CGrenade *pGrenade = (CGrenade *)GET_PRIVATE(pentFind);
        if (pGrenade && pGrenade->pev)
        {
            // 3. 核心安全校驗：
            // 欄位 *(byte*)(iVar4 + 0x1a0) & 1 對應了 pGrenade->m_bIsC4 是否為 true 的位元標記
            // 欄位 *(int*)(iVar4 + 0x198) 則為該 C4 內嵌記下的擁有者 ID
            if ((pGrenade->m_bIsC4 == true) && (*(int *)((int)pGrenade->pev + 0x198) == iOwnerID))
            {
                // 分流 A：如果是常規的引爆指令 SATCHEL_DETONATE
                if (code == SATCHEL_DETONATE)
                {
                    // 呼叫虛擬函數表 [200 / sizeof(void*)]（即索引 50 處的 Use 接口）
                    // 相當於執行了 pGrenade->Use(pOwner, pOwner, USE_TOGGLE, 0.0f);
                    // 藉此劫持控制流，直接將 C4 狀態機強行扭轉至 Detonate2 或 Detonate 瞬間炸開
                    pGrenade->Use(pOwner, pOwner, USE_TOGGLE, 0.0f);
                }
                // 分流 B：如果是拆除或地圖腳本強制解除擁有權（如解除遙控權）
                else
                {
                    *(int *)((int)pGrenade->pev + 0x198) = 0; // 將擁有者 ID 清空，使其脫離玩家的引爆佇列
                }
            }
        }
        
        pentFind = g_engfuncs.pfnFindEntityByString(pentFind, "classname", "grenade");
    }
}
