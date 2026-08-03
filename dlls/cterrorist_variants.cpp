#include "cterrorist.h"

// =========================================================================
// 🔵 繼承自完全體特警 AI 的法國 GIGN 都市特種兵變體類別定義 (CCounterTerroristGIGN)
// =========================================================================
class CCounterTerroristGIGN : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void ); // 預留下一回合點火中樞
};

void CCounterTerroristGIGN::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義特警模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🔵 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典法國 GIGN 特警模型
        // =========================================================================
        PRECACHE_MODEL( "models/gign.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CCounterTerrorist::Precache()
    // 一鍵同步加載 3D 雷射線、拋殼粒子、與 8 大現代槍械 Client-Side 網路 Delta 事件同步句柄！
    // =========================================================================
    CCounterTerrorist::Precache();
}

void CCounterTerroristGIGN::SpawnInit( void )
{
    // =========================================================================
    // 🔵 1. 3D 法國 GIGN 物理網格與隨體碰撞模型固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        // 基準預設：將此衍生 NPC 物件與法國 GIGN 特警三維模型進行空間固化綁定
        SET_MODEL( edict(), "models/gign.mdl" );
    }
    else
    {
        // 採用地圖製作者覆寫的自定義特定服飾模型
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎特警細分型態為法國 GIGN 陣營代號 (1)
    // =========================================================================
    m_ctType = 1; 

    // =========================================================================
    // 🇫🇷 3. 戰術語系硬互鎖：動態記憶體分配 "FR" (法語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道的法語交火與跟隨語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "FR" ); // 🚀 法語大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_counterterrorist_gign, CCounterTerroristGIGN );

class CCounterTerroristGSG9 : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void ); // 預留下一回合點火中樞
};

void CCounterTerroristGSG9::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義特警模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🟢 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典德國 GSG9 特警模型
        // =========================================================================
        PRECACHE_MODEL( "models/gsg9.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CCounterTerrorist::Precache()
    // 一鍵同步加載 3D 雷射線、拋殼粒子、與 8 大現代槍械 Client-Side 網路 Delta 事件同步句柄！
    // =========================================================================
    CCounterTerrorist::Precache();
}

void CCounterTerroristGSG9::SpawnInit( void )
{
    // =========================================================================
    // 🟢 1. 3D 德國 GSG9 物理網格與隨體碰撞模型固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        // 基準預設：將此衍生 NPC 物件與德國 GSG9 特警三維模型進行空間固化綁定
        SET_MODEL( edict(), "models/gsg9.mdl" );
    }
    else
    {
        // 採用地圖製作者覆寫的自定義特定服飾模型
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎特警細分型態為德國 GSG9 陣營代號 (2)
    // =========================================================================
    m_ctType = 2; 

    // =========================================================================
    // 🇩🇪 3. 戰術語系硬互鎖：動態記憶體分配 "GE" (德語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道的德語交火與跟隨語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "GE" ); // 🚀 德語大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_counterterrorist_gsg9, CCounterTerroristGSG9 );

class CCounterTerroristSAS : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristSAS::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義特警模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🇬🇧 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典英國 SAS 特警模型
        // =========================================================================
        PRECACHE_MODEL( "models/sas.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CCounterTerrorist::Precache()
    // =========================================================================
    CCounterTerrorist::Precache();
}

void CCounterTerroristSAS::SpawnInit( void )
{
    // =========================================================================
    // 🇬🇧 1. 3D 英國 SAS 物理網格與防毒面具模型空間固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/sas.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎特警細分型態為英國 SAS 陣營代號 (4)
    // =========================================================================
    m_ctType = 4; 

    // =========================================================================
    // 🇬🇧 3. 戰術語系硬互鎖：動態記憶體分配 "BR" (英式英語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道深沉的倫敦腔交火與跟隨語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "BR" ); // 🚀 英倫防毒面具大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_counterterrorist_sas, CCounterTerroristSAS );

class CCounterTerroristSpetsnaz : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristSpetsnaz::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義特警模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🇷🇺 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典俄羅斯 Spetsnaz 模型
        // =========================================================================
        PRECACHE_MODEL( "models/spetsnaz.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CCounterTerrorist::Precache()
    // =========================================================================
    CCounterTerrorist::Precache();
}

void CCounterTerroristSpetsnaz::SpawnInit( void )
{
    // =========================================================================
    // 🇷🇺 1. 3D 俄羅斯 Spetsnaz 物理網格與迷彩重裝頭盔模型空間固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/spetsnaz.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎特警細分型態為俄羅斯 Spetsnaz 陣營代號 (3)
    // =========================================================================
    m_ctType = 3; 

    // =========================================================================
    // 🇷🇺 3. 戰術語系硬互鎖：動態記憶體分配 "RU" (俄羅斯語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道專業的俄語交火與跟隨語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "RU" ); // 🚀 俄語阿爾法特警大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_counterterrorist_spetsnaz, CCounterTerroristSpetsnaz );

class CCounterTerroristSwat : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristSwat::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義特警模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🇺🇸 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典美軍海豹部隊 (SEAL) 模型
        // =========================================================================
        PRECACHE_MODEL( "models/seal.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CCounterTerrorist::Precache()
    // =========================================================================
    CCounterTerrorist::Precache();
}

void CCounterTerroristSwat::SpawnInit( void )
{
    // =========================================================================
    // 🇺🇸 1. 3D 美國海豹部隊物理網格與戰術頭盔模型空間固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/seal.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎特警細分型態為美國海豹部隊陣營代號 (0)
    // =========================================================================
    m_ctType = 0; 

    // =========================================================================
    // 🇺🇸 3. 戰術語系硬互鎖：動態記憶體分配 "AM" (美式英語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道專業的美語交火與跟隨語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "AM" ); // 🚀 美式英語海豹特警大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_counterterrorist_swat, CCounterTerroristSwat );


class CCounterTerroristRepel : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ); // 預留下一回合裂變工廠
    
public:
    int m_iSpriteTexture; // 🔒 儲存 sprites/rope.spr 繩索幾何紋理索引的專屬成員變數
};

void CCounterTerroristRepel::Precache( void )
{
    // =========================================================================
    // 🥷 1. 核心動態類別重定向：開局首幀若無特定指派，強制將類別名強寫綁定為 SAS 微衝兵
    // =========================================================================
    if ( pev->classname == 0 )
    {
        pev->classname = ALLOC_STRING( "monster_ct_sas_mp5" ); // 🚀 皇牌 SAS 突擊隊大腦綁定！
    }

    // 發動實體原型嵌套互鎖，一鍵載入該特警兵種的所有軍火庫與對白音訊
    UTIL_PrecacheOther( (char *)STRING( pev->classname ) );

    // =========================================================================
    // 🔊 2. 電影級聽覺細節：加載滑索摩擦連環迴圈音效與著陸衝擊音軌
    // =========================================================================
    PRECACHE_SOUND( "common/body_zipdrop.wav" );
    PRECACHE_SOUND( "common/body_ziploop.wav" );

    // =========================================================================
    // 🧶 3. 幾何資源預載：強鎖黑鷹直升機高空拋下繩索的動態 3D 紋理粒子 (rope.spr)
    // =========================================================================
    m_iSpriteTexture = PRECACHE_MODEL( "sprites/rope.spr" );
}

void CCounterTerroristRepel::Spawn( void )
{
    // =========================================================================
    // 📐 1. 100% 還原二進位行為：編編譯器內聯 Precache 安全防砸窗排查
    // =========================================================================
    // 復原 GCC 內聯展開邏輯，確保與原生二進位虛擬表 1 號方法完美對齊
    Precache(); 

    // =========================================================================
    // 🚁 2. 隱形劇本點火工廠狀態配置
    // 無碰撞體體積，並將 Use 訊號線死死指針綁定 RepelUse 物理核心
    // =========================================================================
    pev->solid = SOLID_NOT; // 0: 隱形非實體化存根
    
    // 建立實體 I/O 通信握手線，等待直升機腳本發射 Use 信號激活索降
    SetUse( &CCounterTerroristRepel::RepelUse ); 
}

void CCounterTerroristRepel::KeyValue( KeyValueData *pkvd )
{
    // =========================================================================
    // 🎨 1. 鍵值比對：repelskin (100% 還原二進位長度 10 內聯字串匹配)
    // 控制索降特警出生時播放的迷彩服飾皮膚編號
    // =========================================================================
    if ( strcasecmp( pkvd->szKeyName, "repelskin" ) == 0 )
    {
        m_iSkin = strtol( pkvd->szValue, nullptr, 10 );
        pkvd->fHandled = TRUE;
    }
    // =========================================================================
    // 👮 2. 鍵值比對：repelhead (100% 還原二進位長度 10 內聯字串匹配)
    // 控制索降特警模型的防彈頭盔、護目鏡、面罩等 Bodygroup 配件變體
    // =========================================================================
    else if ( strcasecmp( pkvd->szKeyName, "repelhead" ) == 0 )
    {
        m_iHead = strtol( pkvd->szValue, nullptr, 10 );
        pkvd->fHandled = TRUE;
    }
    // =========================================================================
    // 🚀 3. 鍵值比對：monstertype (100% 還原二進位長度 12 內聯字串匹配)
    // 驚天重定向核心：動態改寫類別名，決定著陸一瞬間裂變孵化出哪一型完全體王牌特警 AI！
    // =========================================================================
    else if ( strcasecmp( pkvd->szKeyName, "monstertype" ) == 0 )
    {
        m_classname = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
    }
    // =========================================================================
    // 🏢 4. 降級放行：交由怪物母體基類解析日常幾何座標
    // =========================================================================
    else
    {
        CBaseMonster::KeyValue( pkvd );
    }
}

void CCounterTerroristRepel::RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    // =========================================================================
    // 📐 1. 4096碼 垂直向下 3D 物理射線探測，預解算著陸點幾何與降落高度比例
    // =========================================================================
    Vector vecTraceEnd = pev->origin;
    vecTraceEnd.z -= 4096.0f; // 向地底正下方外推 4096 碼死線

    TraceResult tr;
    UTIL_TraceLine( pev->origin, vecTraceEnd, dont_ignore_monsters, edict(), &tr );

    // =========================================================================
    // 👮 2. 活體特警實體工廠大裂變：動態孵化出地圖指定或重定向的特警兵種實體
    // =========================================================================
    edict_t *pNewEdict = CREATE_NAMED_ENTITY( m_classname ); // 預設解密為 monster_ct_sas_mp5
    if ( pNewEdict == nullptr || pNewEdict->pvPrivateData == nullptr )
    {
        ALERT( at_console, "NULL Ent in Create!\n" );
        return;
    }

    // 跨類別多載，解鎖 Companion 特警大腦指針
    CCounterTerrorist *pCT = (CCounterTerrorist *)CBaseEntity::Instance( pNewEdict );
    if ( pCT == nullptr )
    {
        return;
    }

    // 清空實體宿主隸屬鏈，防範權限污染
    pCT->pev->owner = nullptr;

    // 幾何座標克隆：將新生成特警的 3D 世界位置與面部角度與當前直升機艙門完美重合
    pCT->pev->origin = pev->origin;
    pCT->pev->angles = pev->angles;

    // 🎨 美術外觀與頭戴配件繼承：一鍵將隱形物件讀取出的皮膚與頭部變體數據強寫拷貝給新特警
    pCT->m_head      = m_iHead;
    pCT->pev->skin   = m_iSkin;

    // 點火調用 DispatchSpawn 激活新特警的內存神經元，拉起其體內的基類 Spawn
    DispatchSpawn( pCT->edict() );

    // =========================================================================
    // 🧗 3. 重裝特權注入：賦予其 200點 索降神仙血量、空中滑翔物理、與重力加速度速度補償
    // =========================================================================
    pCT->pev->health   = 200.0f;       // 🔒 200點 索降高爆生存防線，嚴防空中被秒殺
    pCT->pev->movetype = MOVETYPE_FLY; // 5: 空中無重力下墜物理型態
    pCT->pev->velocity.x = 0.0f;
    pCT->pev->velocity.y = 0.0f;
    
    // 填裝 -128 到 -196 碼的隨機垂直向下重力下榻初速度
    pCT->pev->velocity.z = RANDOM_FLOAT( -196.0f, -128.0f );

    // =========================================================================
    // 🏃 4. 🎰 皇牌黑科技：4 大輕重武器兵種步態動畫自適應對齊中樞
    // 依據當前特警手持的是步槍、狙擊槍還是火箭筒，自動配准最流暢的單手控繩下墜動作
    // =========================================================================
    int iSequence = -1;

    switch ( pCT->m_ctBehavior )
    {
        case 4: // 🎯 狙擊手特警
            iSequence = pCT->SetActivitySniperBase( ACT_GLIDE );
            break;
        case 5: // 🚀 火箭筒重裝特警
            iSequence = pCT->SetActivityLAWBase( ACT_GLIDE );
            break;
        case 7: // 💣 戰術擲彈特警
            iSequence = pCT->SetActivityGrenadeBase( ACT_GLIDE );
            break;
        default: // 0, 1, 2, 3, 6, 8: 常規 M4A1 突擊步槍 / MP5 / 手槍特警兵種
            // 降級調用 Vanilla 多載方法反查
            iSequence = pCT->LookupActivity( 51 ); // 0x33 = 51 = ACT_GLIDE
            break;
    }

    if ( iSequence == -1 )
    {
        // 級聯保險安全熔斷
        iSequence = pCT->LookupActivity( 51 );
    }

    pCT->m_Activity = ACT_GLIDE;

    if ( iSequence < 0 )
    {
        // 拋出官方標準警告日誌，將骨骼硬鎖在 0 號基準面防止畫面崩塌
        ALERT( at_console, "%s has no sequence for act:%d\n", STRING( pCT->pev->classname ), 51 );
        pCT->pev->sequence = 0;
    }
    else
    {
        if ( iSequence != pCT->pev->sequence || pCT->m_fSequenceLoops != 0 )
        {
            pCT->pev->frame = 0.0f; // 重置動畫時鐘起點
        }
        pCT->pev->sequence = iSequence;
        
        // 拉起動畫矩陣序列配准刷新
        CBaseAnimating::ResetSequenceInfo( (CBaseAnimating *)pCT );

        // =========================================================================
        // 📐 5. 下肢角速度轉身速度自適應內聯展開表格查表
        // 確保下滑時以完美的 3D 幾何夾角面朝開火死線
        // =========================================================================
        float flIdealYawSpeed = 90.0f;
        int iActivityIndex = pCT->m_Activity;

        if ( ( iActivityIndex - ACT_IDLE ) < 51 ) // 0x33 = 51
        {
            // 鏡像重合二進位 DAT_0018357f 數據表查表法
            switch ( pCT->m_Activity )
            {
                case ACT_IDLE: flIdealYawSpeed = 90.0f;  break;
                case ACT_WALK: flIdealYawSpeed = 120.0f; break;
                case ACT_RUN:  flIdealYawSpeed = 180.0f; break;
                default:       flIdealYawSpeed = 90.0f;  break;
            }
        }
        pCT->pev->yaw_speed = flIdealYawSpeed;
    }

    // 📊 內存精確對齊：將探測到的未來著陸點世界座標，拷貝注入新出生的特警體內記憶體中
    pCT->m_vecLastPosition = tr.vecEndPos;

    // =========================================================================
    // 🧶 6. 10碼加算透明高張力繩索幾何焊接 (CBeam)
    // 兩點一線：A端鎖死直升機艙門高度，B端鎖死新特警肉體，隨下墜幾何動態伸展
    // =========================================================================
    CBeam *pBeam = CBeam::BeamCreate( "sprites/rope.spr", 10 ); // 10 碼寬度粗鋼索

    if ( pBeam != nullptr )
    {
        Vector vecRopeTop = pev->origin;
        vecRopeTop.z += 112.0f; // 📐 偏置 112 碼，精確對齊黑鷹直升機外置拋繩架

        // 發動兩點動態焊接，繩索下端黏死在正在急速墜落的特警 Edict 索引上
        pBeam->PointEntInit( vecRopeTop, pCT->entindex() );

        entvars_t *pBeamPev = pBeam->pev;
        pBeamPev->rendermode = ( pBeamPev->rendermode & 0x0F ) | kRenderTransAdd; // 0x20 = 加算透明自發光
        pBeamPev->rendercolor = Vector( 255.0f, 255.0f, 255.0f );                 // 純白高光

        // =========================================================================
        // ⏱️ 幾何幾何時間軸自適應自毀演算：高度 / 速度 = 下滑精確秒數
        // 著陸的一瞬間，自動熔斷清空繩索實體，完美嚴防殘留 Bug！
        // =========================================================================
        float flRopeHeight = tr.flFraction * -4096.0f;
        float flRopeTime   = flRopeHeight / pCT->pev->velocity.z; // 高度除以 Z 軸高速

        pBeam->SetThink( &CBaseEntity::SUB_Remove );
        pBeamPev->nextthink = gpGlobals->time + flRopeTime + 0.5f; // 寬限 0.5 秒大圓滿緩衝
    }

    // 💀 7. 功德圓滿：原本的隱形劇本點火存根徹底退出內存釋放空間，將戰場主導權完全移交新英雄！
    UTIL_Remove( this );
}

TYPEDESCRIPTION CCounterTerroristRepel::m_SaveData[] =
{
    // DEFINE_FIELD 巨集負責將變數、記憶體步長、與序列化標記完美對齊綁定
    DEFINE_FIELD( CCounterTerroristRepel, m_iSpriteTexture, FIELD_INTEGER ), // 1. rope.spr 繩索幾何紋理句柄
    DEFINE_FIELD( CCounterTerroristRepel, m_iSkin,          FIELD_INTEGER ), // 2. 特警自定義皮膚制服編號
    DEFINE_FIELD( CCounterTerroristRepel, m_iHead,          FIELD_INTEGER ), // 3. 特警自定義頭部頭盔編號
};

int CCounterTerroristRepel::Save( CSave *save )
{
    // 1. 優先拉起怪物母體基類的 Save 存檔矩陣，備份生命值與尋路節點
    if ( !CBaseMonster::Save( save ) )
    {
        return FALSE; // 基類保存崩潰，拒絕寫入欄位
    }

    // =========================================================================
    // 🚀 2. 核心點火：發動 3 階自定義滑索暫存器精確硬寫，硬鎖 3 組黃金變數
    // 確保高空快速存檔讀檔時，3D鋼索、服飾皮膚與頭盔配件 100% 電影級毫釐不差還原！
    // =========================================================================
    return save->WriteFields( "CCounterTerroristRepel", this, m_SaveData, ARRAYSIZE(m_SaveData) ); // 3
}

int CCounterTerroristRepel::Restore( CRestore *restore )
{
    // 1. 優先拉起怪物母體基類的 Restore 還原矩陣，恢復生命值與尋路節點
    if ( !CBaseMonster::Restore( restore ) )
    {
        return FALSE; // 基類還原崩潰，拒絕讀取欄位
    }

    // =========================================================================
    // 🚀 2. 核心點火：發動 3 階自定義滑索暫存器逆向重載，硬鎖 3 組黃金變數的反序列化
    // 確保高空讀取存檔時，3D鋼索、服飾皮膚與頭盔配件 100% 電影級毫釐不差完美復甦！
    // =========================================================================
    return restore->ReadFields( "CCounterTerroristRepel", this, m_SaveData, ARRAYSIZE(m_SaveData) ); // 3
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_counterterrorist_repel, CCounterTerroristRepel );