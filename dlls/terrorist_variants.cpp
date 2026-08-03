#include "terrorist.h"

class CTerroristArctic : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristArctic::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/arctic.mdl" );
    }
    CTerrorist::Precache();
}

void CTerroristArctic::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/arctic.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }
    m_terroristType = 0; 
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "RU" );
    }
}

LINK_ENTITY_TO_CLASS( monster_terrorist_arctic, CTerroristArctic );

class CTerroristAsian : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristAsian::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/asian.mdl" );
    }
    CTerrorist::Precache();
}

void CTerroristAsian::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/asian.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }
    m_terroristType = 5; 
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "JA" );
    }
}

LINK_ENTITY_TO_CLASS( monster_terrorist_asian, CTerroristAsian );

class CTerroristDesert : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristDesert::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/leet.mdl" );
    }
    CTerrorist::Precache();
}

void CTerroristDesert::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/leet.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }
    m_terroristType = 1; 
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "AR" ); // 🚀 阿拉伯語大腦中樞激活點亮，大功告成！
    }
}

LINK_ENTITY_TO_CLASS( monster_terrorist_desert, CTerroristDesert );

class CTerroristJungle : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristJungle::Precache( void )
{
    // =========================================================================
    // 🎨 優先級 A：若地圖設計師在地圖中手寫指定了自定義服飾模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🌴 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典叢林游擊隊 (Guerilla) 模型
        // =========================================================================
        PRECACHE_MODEL( "models/guerilla.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CTerrorist::Precache()
    // =========================================================================
    CTerrorist::Precache();
}

void CTerroristJungle::SpawnInit( void )
{
    // =========================================================================
    // 🌴 1. 3D 叢林游擊隊物理網格與隨體碰撞模型固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/guerilla.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎兵種細分型態為叢林游擊隊陣營代號 (3)
    // =========================================================================
    m_terroristType = 3; 

    // =========================================================================
    // 🇪🇸 3. 戰術語系硬互鎖：動態記憶體分配 "SP" (西班牙語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道的西班牙語交火語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "SP" ); // 🚀 西班牙語大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_terrorist_jungle, CTerroristJungle );

class CTerroristRepel : public CTerrorist
{
public:
    void Precache( void ) override;
    void Spawn( void ) override; // 預留下一回合點火工廠
    
public:
    int m_iSpriteTexture; // 🔒 儲存 sprites/rope.spr 繩索幾何紋理索引的專屬成員變數
};

void CTerroristRepel::Precache( void )
{
    // =========================================================================
    // 🥷 1. 實體原型嵌套互鎖：一鍵拉起父類別 monster_terrorist 的全部軍火庫與感知大腦
    // =========================================================================
    UTIL_PrecacheOther( "monster_terrorist" );

    // =========================================================================
    // ⛓️ 2. 幾何資源預載：強鎖黑鷹直升機高空拋下繩索的動態 3D 紋理粒子 (rope.spr)
    // =========================================================================
    m_iSpriteTexture = PRECACHE_MODEL( "sprites/rope.spr" );
}

#define m_vecRepelEndPos m_vecTossVelocity // 🎰 完美解密內存指針偏置：原置複用 Vector 空間作為索降落腳點記憶體

void CTerroristRepel::Spawn( void )
{
    // 優先拉起基類基礎構造工廠
    CTerrorist::Spawn();

    // =========================================================================
    // 🚁 隱形劇本點火工廠初始化
    // 無碰撞體體積，並將 Use 訊號線死死指針綁定 RepelUse 物理核心
    // =========================================================================
    pev->solid = SOLID_NOT; // 0
    SetUse( &CTerroristRepel::RepelUse ); 
}

void CTerroristRepel::RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    TraceResult tr;
    Vector vecEnd = pev->origin;
    vecEnd.z -= 4096.0f; // 向正下方推移 4096 碼

    // 🎯 1. 發動 3D 射線下推碰撞探測，抓取正下方廢墟或地板的真實世界著陸點
    UTIL_TraceLine( pev->origin, vecEnd, dont_ignore_monsters, edict(), &tr );

    // =========================================================================
    // 🥷 2. 活體 NPC 實體工廠點火生成
    // 在艙門原位創造完全體死敵 AI，並強寫下墜速度與 51 號索降滑翔計畫
    // =========================================================================
    CBaseEntity *pEnt = CBaseEntity::Create( "monster_terrorist", pev->origin, pev->angles, nullptr );
    if ( pEnt == nullptr )
    {
        return;
    }

    // 跨類別安全多載，拉起死敵 AI 的大腦指標
    CTerrorist *pTerrorist = (CTerrorist *)pEnt->MyMonsterPointer();
    if ( pTerrorist == nullptr )
    {
        return;
    }

    pTerrorist->pev->movetype = MOVETYPE_FLY; // 5: 空中無重力下墜物理型態
    
    // 填裝 -128 到 -196 碼的隨機垂直向下重力補償速度
    pTerrorist->pev->velocity.z = RANDOM_FLOAT( -196.0f, -128.0f );
    pTerrorist->pev->velocity.x = 0.0f;
    pTerrorist->pev->velocity.y = 0.0f;

    // 🚀 強鎖計畫 51 (SCHED_TERRORIST_REPEL)，逼其體內 StartTask 強發 ACT_GLIDE 電影級單手控繩動作
    pTerrorist->ChangeSchedule( pTerrorist->GetScheduleOfType( 51 ) ); // 0x33 = 51

    // 📊 內存精確對齊：將剛才射線探測出的落腳點世界坐標，強寫儲存進新出生土匪體內，供其下滑判定
    pTerrorist->m_vecRepelEndPos = tr.vecEndPos;

    // =========================================================================
    // 🧶 3. 10碼加算透明高張力繩索幾何焊接 (CBeam)
    // 兩點一線：A端鎖死直升機艙門，B端鎖死土匪肉體，隨下墜動態拉伸
    // =========================================================================
    CBeam *pBeam = CBeam::BeamCreate( "sprites/rope.spr", 10 ); // 10 碼寬度

    if ( pBeam != nullptr )
    {
        Vector vecRopeTop = pev->origin;
        vecRopeTop.z += 112.0f; // 📐 偏置 112 碼，精確對齊黑鷹直升機主滑輪艙門高度

        // 發動動態兩點焊接
        pBeam->PointEntInit( vecRopeTop, pTerrorist->entindex() );
        
        // 配置加算自發光渲染參數模式
        pBeam->pev->rendermode = kRenderTransAdd; // 0x20 遮罩融合
        pBeam->pev->rendercolor = Vector( 255.0f, 255.0f, 255.0f ); // 純白高光鋼索

        // =========================================================================
        // ⏱️ 幾何時間軸自適應自毀演算：高度 / 速度 = 下滑精確秒數
        // 著陸的一瞬間，自動熔斷清空繩索實體，完美嚴防殘留 Bug！
        // =========================================================================
        float flRopeHeight = tr.flFraction * -4096.0f;
        float flRopeTime   = flRopeHeight / pTerrorist->pev->velocity.z;

        pBeam->SetThink( &CBaseEntity::SUB_Remove );
        pBeam->pev->nextthink = gpGlobals->time + flRopeTime + 0.5f; // 寬限 0.5 秒大圓滿緩衝
    }

    // 任務圓滿封箱，原本的隱形劇本點火存根徹底退出內存釋放空間
    UTIL_Remove( this );
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_terrorist_repel, CTerroristRepel );

class CTerroristRussian : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void ); // 預留下一回合點火中樞
};

void CTerroristRussian::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義服飾模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🇷🇺 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典東歐鳳凰戰警 (Phoenix) 模型
        // =========================================================================
        PRECACHE_MODEL( "models/terror.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CTerrorist::Precache()
    // 一鍵同步加載 3D 雷射線、拋殼粒子與 39 組劇情無線電台詞語音池！
    // =========================================================================
    CTerrorist::Precache();
}

void CTerroristRussian::SpawnInit( void )
{
    // =========================================================================
    // 🇷🇺 1. 3D 東歐鳳凰戰警物理網格與隨體碰撞模型固化 (SetModel)
    // =========================================================================
    if ( pev->model == 0 )
    {
        // 基準預設：將此衍生 NPC 物件與東歐鳳凰戰警三維模型進行空間固化綁定
        SET_MODEL( edict(), "models/terror.mdl" );
    }
    else
    {
        // 採用地圖製作者覆寫的自定義特定服飾模型
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    // =========================================================================
    // 🎰 2. 100% 還原二進位行為：重寫基礎兵種細分型態為東歐鳳凰戰警陣營代號 (4)
    // =========================================================================
    m_terroristType = 4; 

    // =========================================================================
    // 🇷🇺 3. 戰術語系硬互鎖：動態記憶體分配 "RU" (俄羅斯語) 語音包暫存器句柄
    // 迫使其大腦放聲或發動無線電時，100% 強制全服廣播地道的俄語交火語音！
    // =========================================================================
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "RU" ); // 🚀 俄語大腦中樞激活點亮，大功告成！
    }
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_terrorist_russian, CTerroristRussian );

class CTerroristUrban : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void ); // 預留下一回合點火中樞
};

void CTerroristUrban::Precache( void )
{
    // =========================================================================
    // 🎨 1. 優先級 A：若地圖設計師在地圖中手寫指定了自定義服飾模型，優先放行預載
    // =========================================================================
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        // =========================================================================
        // 🏢 優先級 B：基準預設，強鎖零點行動單人劇情版官方經典城市民兵 (Militia) 模型
        // =========================================================================
        PRECACHE_MODEL( "models/militia.mdl" );
    }

    // =========================================================================
    // 🏢 2. 繼承鏈回歸：引爆父類別 CTerrorist::Precache()
    // 一鍵同步加載 3D 雷射線、拋殼粒子與 39 組劇情無線電台詞語音池！
    // =========================================================================
    CTerrorist::Precache();
}

// 🎰 在遊戲引擎實體鏈接工廠中註冊該衍生實體，使其能在 Hammer 地圖中被快樂刷出
LINK_ENTITY_TO_CLASS( monster_terrorist_urban, CTerroristUrban );