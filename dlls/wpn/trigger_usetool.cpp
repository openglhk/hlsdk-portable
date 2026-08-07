enum ToolSet_e
{
    TOOL_BLOWTORCH  = 0x001, // 1   - 噴燈 (weapon_blowtorch)
    TOOL_BRIEFCASE  = 0x002, // 2   - 公事包 (weapon_briefcase)
    TOOL_CAMERA     = 0x004, // 4   - 相機 (weapon_camera)
    TOOL_FIBEROPTIC = 0x008, // 8   - 光纖鏡 (weapon_fiberopticcamera)
    TOOL_RADIO      = 0x010, // 16  - 無線電 (weapon_radio)
    TOOL_RCBOMB     = 0x020, // 32  - 遙控炸彈 (weapon_radiocontrolledbomb)
    TOOL_SYRINGE    = 0x040, // 64  - 注射器 (weapon_syringe)
    TOOL_BOMB_DEFUSE= 0x080, // 128 - 拆彈包 (bomb_defuse)
};

// ==========================================
// CTriggerUseTool 類別宣告
// ==========================================
class CTriggerUseTool : public CBaseTrigger
{
public:
    // 虛擬函數覆寫 (HLSDK 標準接口)
    void Spawn() override;
    void KeyValue(KeyValueData *pkvd) override;
    int  Save(CSave &save) override;
    int  Restore(CRestore &restore) override;
    
    // 自定義事件與核心邏輯
    void EXPORT TouchTriggerUseTool(CBaseEntity *pOther);
    void EXPORT FireUseTarget(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
    void EXPORT PreKillTarget();

    // 狀態與條件判定函數
    BOOL IsPlayerInTrigger();
    BOOL IsPlayerInPosition();
    BOOL IsPlayerInPositionUsingProperTool();
    int  IsPlayerUsingProperTool(); // 返回 ToolSet_e 的值
    BOOL IsBombDefuseTrigger();
    
    // HUD 訊息顯示
    void DisplayToolMessage();          // 外部難度篩選接口
    void ExecuteDisplayToolMessage();   // 內部核心 HUD 繪製

    // 特殊道具互動
    CBaseEntity *GetRCBombTarget();

public:
    // 地圖屬性欄位 (對應 KeyValue)
    string_t m_iToolName;         // "toolname" : 自定義要求武器名
    string_t m_iToolTarget;       // "tooltarget" : 要求玩家注視的目標實體名
    string_t m_iRCBombTarget;     // "rcbombtarget" : 遙控炸彈爆破目標名
    int      m_iToolSet;          // "toolset" : 劇情工具位元掩碼
    int      m_iDefuseTime;       // "bombdefusetime" : 拆彈所需時間

    // 動態狀態變數
    BOOL     m_bombPlanted;       // 標記遙控炸彈是否已成功安裝

    // 靜態存檔資料表
    static TYPEDESCRIPTION m_SaveData[];
};

void CTriggerUseTool::Spawn()
{
    // 檢查是否有設置旋轉角度，如果有，轉換為移動方向向量 (Movedir)
    if (pev->angles != g_vecZero)
    {
        SetMovedir(pev);
    }

    pev->solid    = SOLID_TRIGGER; // 1
    pev->movetype = MOVETYPE_NONE; // 0

    // 設定觸發器的 Model
    SET_MODEL(ENT(pev), STRING(pev->model));

    // 如果沒有開啟 showtriggers 變數，則隱藏觸發器實體
    if (CVAR_GET_FLOAT("showtriggers") == 0)
    {
        pev->effects |= EF_NODRAW; // 0x80
    }

    // 綁定成員函數指針
    SetTouch(&CTriggerUseTool::TouchTriggerUseTool);
    SetUse(&CTriggerUseTool::FireUseTarget);
}

void CTriggerUseTool::TouchTriggerUseTool(CBaseEntity *pOther)
{
    // 檢查碰觸觸發器的是否為玩家 (IsPlayer() 在虛擬函數表中通常位於 0x2a 附近)
    if (!pOther->IsPlayer())
        return;

    CBasePlayer *pPlayer = (CBasePlayer *)pOther;

    // 檢查 Master 觸發器狀態
    if (!UTIL_IsMasterTriggered(m_sMaster, pPlayer))
        return;

    // 如果玩家當前記錄的 Tool 觸發器不是此觸發器，且難易度為簡單 (Skill == 1)，顯示提示訊息
    if (pPlayer->m_pCurrentToolTrigger != this && g_iSkillLevel == 1)
    {
        DisplayToolMessage(m_sMaster); 
    }

    // 將此觸發器記錄到玩家身上
    pPlayer->m_pCurrentToolTrigger = this;

    // 檢查玩家的位置與是否裝備正確的工具
    BOOL bInPosition = IsPlayerInPosition();
    BOOL bUsingProperTool = IsPlayerUsingProperTool();

    // 如果玩家沒有斷線
    if (!pPlayer->IsDisconnected())
    {
        // 獲取玩家當前手持武器的名稱
        const char *pszWeaponName = STRING(pPlayer->GetActiveWeaponClassname());
        const char *pszToolName = STRING(m_iToolName);

        // --- 道具 1: 噴燈 (Blowtorch) ---
        if ((pszToolName && strcmp(pszToolName, "weapon_blowtorch") == 0) || (m_iToolSet & TOOL_BLOWTORCH))
        {
            pPlayer->m_iToolFlags |= TOOL_BLOWTORCH;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_BLOWTORCH;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_BLOWTORCH;
            }
        }

        // --- 道具 2: 拆彈包 (Bomb Defuse) ---
        if ((pszToolName && strcmp(pszToolName, "bomb_defuse") == 0) || (m_iToolSet & TOOL_BOMB_DEFUSE))
        {
            pPlayer->m_iToolFlags |= TOOL_BOMB_DEFUSE;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_BOMB_DEFUSE;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_BOMB_DEFUSE;
            }
        }

        // --- 道具 3: 公事包 (Briefcase) ---
        if ((pszToolName && strcmp(pszToolName, "weapon_briefcase") == 0) || (m_iToolSet & TOOL_BRIEFCASE))
        {
            pPlayer->m_iToolFlags |= TOOL_BRIEFCASE;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_BRIEFCASE;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_BRIEFCASE;
            }
        }

        // --- 道具 4: 照相機 (Camera) ---
        if ((pszToolName && strcmp(pszToolName, "weapon_camera") == 0) || (m_iToolSet & TOOL_CAMERA))
        {
            pPlayer->m_iToolFlags |= TOOL_CAMERA;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_CAMERA;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_CAMERA;
            }
        }

        // --- 道具 5: 光纖鏡 (Fiberoptic Camera) ---
        if ((pszToolName && strcmp(pszToolName, "weapon_fiberopticcamera") == 0) || (m_iToolSet & TOOL_FIBEROPTIC))
        {
            pPlayer->m_iToolFlags |= TOOL_FIBEROPTIC;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_FIBEROPTIC;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_FIBEROPTIC;
            }
        }

        // --- 道具 6: 無線電 (Radio) ---
        if ((pszToolName && strcmp(pszToolName, "weapon_radio") == 0) || (m_iToolSet & TOOL_RADIO))
        {
            pPlayer->m_iToolFlags |= TOOL_RADIO;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_RADIO;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_RADIO;
            }
        }

        // --- 道具 7: 遙控炸彈 (Radio Controlled Bomb) [續前] ---
        if ((pszToolName && strcmp(pszToolName, "weapon_radiocontrolledbomb") == 0) || (m_iToolSet & TOOL_RCBOMB))
        {
            pPlayer->m_iToolFlags |= TOOL_RCBOMB;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_RCBOMB;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_RCBOMB;
            }
            
            // 重新獲取工具名稱指針，用於下一個道具的比對
            pszToolName = STRING(m_iToolName);
        }

        // --- 道具 8: 注射器 (Syringe) ---
        // 內聯長度 0xf 疊代對應 "weapon_syringe" (14字元 + \0 = 15字元)
        if ((pszToolName && strcmp(pszToolName, "weapon_syringe") == 0) || (m_iToolSet & TOOL_SYRINGE))
        {
            // 注意：代碼中的位元遮罩從前面的 0x40 突變成了 0x80 (即 128)
            pPlayer->m_iToolFlags |= TOOL_SYRINGE;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_SYRINGE;
            }
            if (bUsingProperTool)
            {
                pPlayer->m_iToolUsingFlags |= TOOL_SYRINGE;
            }
        }
    }
	return;
}

void CTriggerUseTool::DisplayToolMessage()
{
    hudtextparms_t hudTextParms;

    // 初始化 HUD 文字參數
    hudTextParms.x           = -1.0f; // 螢幕水平置中
    hudTextParms.y           = 0.01f; // 靠近螢幕頂端
    hudTextParms.fadeinTime  = 0.05f;
    hudTextParms.fadeoutTime = 0.05f;
    hudTextParms.holdTime    = 1.0f;  // 顯示 1 秒
    hudTextParms.fxTime      = 0.0f;
    hudTextParms.effect      = 0;     // 無特殊效果
    
    // 顏色 1 (純藍色): RGBA(0, 0, 255, 255)
    hudTextParms.r1 = 0;
    hudTextParms.g1 = 0;
    hudTextParms.b1 = 255;
    hudTextParms.a1 = 255;

    // 顏色 2 (純白色): RGBA(255, 255, 255, 255)
    hudTextParms.r2 = 255;
    hudTextParms.g2 = 255;
    hudTextParms.b2 = 255;
    hudTextParms.a2 = 255;
    
    hudTextParms.channel = 0;

    // 獲取伺服器中的第一個玩家 (單人模式下通常 EntIndex = 1 便是主角)
    edict_t *pClientEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pClientEdict)
    {
        pClientEdict = g_engfuncs.pfnPEntityOfEntOffset(0);
    }

    if (!pClientEdict || !pClientEdict->pvPrivateData)
        return;

    // 雖然這裡獲取了 pPlayer，但後續的比對邏輯實際上是在檢查 Trigger 本身的屬性
    CBasePlayer *pPlayer = (CBasePlayer *)pClientEdict->pvPrivateData;

    const char *pszToolName = STRING(m_iToolName);

    // --- 檢查 1: 噴燈區域 ---
    if ((pszToolName && strcmp(pszToolName, "weapon_blowtorch") == 0) || (m_iToolSet & TOOL_BLOWTORCH))
    {
        UTIL_HudMessageAll(hudTextParms, "#BLOWTORCH_ZONE");
        pszToolName = STRING(m_iToolName); // 重新載入字串指針
    }

    // --- 檢查 2: 拆彈區域 ---
    if ((pszToolName && strcmp(pszToolName, "bomb_defuse") == 0) || (m_iToolSet & TOOL_BOMB_DEFUSE))
    {
        UTIL_HudMessageAll(hudTextParms, "#BOMBDEFUSE_ZONE");
        pszToolName = STRING(m_iToolName);
    }

    // --- 檢查 3: 公事包區域 ---
    if ((pszToolName && strcmp(pszToolName, "weapon_briefcase") == 0) || (m_iToolSet & TOOL_BRIEFCASE))
    {
        UTIL_HudMessageAll(hudTextParms, "#BRIEFCASE_ZONE");
        pszToolName = STRING(m_iToolName);
    }

    // --- 檢查 4: 相機區域 ---
    if ((pszToolName && strcmp(pszToolName, "weapon_camera") == 0) || (m_iToolSet & TOOL_CAMERA))
    {
        UTIL_HudMessageAll(hudTextParms, "#CAMERA_ZONE");
        pszToolName = STRING(m_iToolName);
    }

    // --- 檢查 5: 光纖鏡區域 ---
    if ((pszToolName && strcmp(pszToolName, "weapon_fiberopticcamera") == 0) || (m_iToolSet & TOOL_FIBEROPTIC))
    {
        UTIL_HudMessageAll(hudTextParms, "#FIBERCAMERA_ZONE");
        pszToolName = STRING(m_iToolName);
    }

    // --- 檢查 6: 無線電區域 ---
    if ((pszToolName && strcmp(pszToolName, "weapon_radio") == 0) || (m_iToolSet & TOOL_RADIO))
    {
        UTIL_HudMessageAll(hudTextParms, "#RADIO_ZONE");
        pszToolName = STRING(m_iToolName);
    }

    // --- 檢查 7: 遙控炸彈區域 ---
    if ((pszToolName && strcmp(pszToolName, "weapon_radiocontrolledbomb") == 0) || (m_iToolSet & TOOL_RCBOMB))
    {
        UTIL_HudMessageAll(hudTextParms, "#RCBOMB_ZONE");
    }
}

// 這是外部呼叫的接口函數（通常在 Touch 或其他事件中被直接調用）
void CTriggerUseTool::DisplayToolMessage()
{
    // 如果不是簡單難度 (Easy)，直接返回，不顯示任何任務道具提示
    if (g_iSkillLevel != 1)
    {
        return;
    }

    // 難易度檢查通過，調用實際處理 HUD 顯示的核心實作
    // 在 C++ 中，它可能被寫成：this->DisplayToolMessage(this); 
    // 或者這是一個重載版本，例如：DisplayToolMessageCore();
    ExecuteDisplayToolMessage(); 
}

void CTriggerUseTool::FireUseTarget(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    // 1. 檢查 Master 觸發器狀態是否啟用
    if (!UTIL_IsMasterTriggered(m_sMaster, pActivator))
    {
        return;
    }

    // 2. 如果有設定 target，觸發地圖上的目標實體
    if (pev->target != 0)
    {
        CBaseDelay::SUB_UseTargets(pActivator, useType, value);
    }

    // 3. 檢查 Spawnflags 是否包含 SF_TRIGGER_ONLY_ONCE (1)
    // 如果設置了「只能觸發一次」，則需要清理玩家狀態並移除自身
    if (pev->spawnflags & SF_TRIGGER_ONLY_ONCE)
    {
        // 獲取伺服器中的第一個玩家 (單人劇情主角)
        edict_t *pClientEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
        if (!pClientEdict)
        {
            pClientEdict = g_engfuncs.pfnPEntityOfEntOffset(0);
        }

        if (pClientEdict && pClientEdict->pvPrivateData)
        {
            CBasePlayer *pPlayer = (CBasePlayer *)pClientEdict->pvPrivateData;
            
            // 關鍵還原：0x8fc 對應玩家類別中記錄當前觸發器的成員變數
            // 當此觸發器被刪除時，必須將玩家身上的關聯指針清空
            pPlayer->m_pCurrentToolTrigger = NULL;
        }

        // 從遊戲世界中永久移除這個觸發器實體
        SUB_Remove();
    }
}

CBaseEntity* CTriggerUseTool::GetRCBombTarget()
{
    // 檢查是否有設定遙控炸彈的目標名稱 (Targetname)
    if (m_iRCBombTarget != 0)
    {
        // 透過目標名稱在遊戲世界中尋找該實體
        CBaseEntity *pTargetEnt = UTIL_FindEntityByTargetname(NULL, STRING(m_iRCBombTarget));
        
        // 標記炸彈已經成功安裝
        m_bombPlanted = TRUE;
        
        return pTargetEnt;
    }
    
    // 如果沒有設定目標，返回空指針
    return NULL;
}

BOOL CTriggerUseTool::IsBombDefuseTrigger()
{
    // 獲取工具名稱字串
    const char *pszToolName = STRING(m_iToolName);
    
    // 如果字串有效，且內容等於 "bomb_defuse"，則返回 TRUE
    if (pszToolName && strcmp(pszToolName, "bomb_defuse") == 0)
    {
        return TRUE;
    }
    
    // 否則返回 FALSE
    return FALSE;
}

BOOL CTriggerUseTool::IsPlayerInPosition()
{
    // 獲取第一個玩家的 edict (單人劇情主角)
    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict)
    {
        return FALSE;
    }

    // --- 階段 1: 軸對齊包圍盒 (AABB) 相交檢查 ---
    // 檢查玩家是否在觸發器邊界內 (相當於原版的偏好巨集或自訂區間判定)
    if (pPlayerEdict->v.absmin.x > pev->absmax.x ||
        pPlayerEdict->v.absmin.y > pev->absmax.y ||
        pPlayerEdict->v.absmin.z > pev->absmax.z ||
        pev->absmin.x > pPlayerEdict->v.absmax.x ||
        pev->absmin.y > pPlayerEdict->v.absmax.y ||
        pev->absmin.z > pPlayerEdict->v.absmax.z)
    {
        return FALSE; // 只要有一軸沒重疊，代表玩家不在區域內
    }

    // 獲取玩家的 C++ 實體指針
    CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;
    if (!pPlayer)
    {
        return FALSE;
    }

    // --- 階段 2: 玩家視野/視角朝向檢查 ---
    // 如果地圖沒有指定朝向目標，玩家只要站在區域內就合格了
    if (m_iToolTarget != 0)
    {
        // 在地圖中尋找要求的目標實體（例如某個閥門、鎖頭或通風管口）
        CBaseEntity *pTargetEnt = UTIL_FindEntityByTargetname(NULL, STRING(m_iToolTarget));
        
        if (pTargetEnt != NULL)
        {
            // 關鍵偏移還原：虛擬函數表 +0x134 處（十進制增量，通常對應 FInViewCone 或 FVisible 的變體）
            // 傳入的浮點常數 0x41f00000 經 IEEE 754 轉換後為 30.0f
            // 這裡的邏輯是檢查：目標實體 pTargetEnt 是否在玩家的 30 度視角扇形區域（View Cone）內
            if (!pPlayer->FInViewCone(pTargetEnt, 30.0f))
            {
                return FALSE; // 玩家背對著目標或沒看著它
            }
        }
    }

    // 通過所有檢查，玩家站位與視角完全正確
    return TRUE;
}

BOOL CTriggerUseTool::IsPlayerInPositionUsingProperTool()
{
    // 1. 首先檢查玩家當前手持的武器/工具是否正確
    if (!IsPlayerUsingProperTool())
    {
        return FALSE;
    }

    // 2. 工具正確後，直接複用空間與視野朝向判定
    // 註：因為底層編譯器將 IsPlayerInPosition() 的幾何代碼內聯（Inline）到了這裡，
    // 在原始 C++ 程式碼中，作者可能寫了重複的代碼，或者這裡本質上就是：
    // return IsPlayerInPosition();

    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict)
    {
        return FALSE;
    }

    // --- 軸對齊包圍盒 (AABB) 相交檢查 ---
    if (pPlayerEdict->v.absmin.x > pev->absmax.x ||
        pPlayerEdict->v.absmin.y > pev->absmax.y ||
        pPlayerEdict->v.absmin.z > pev->absmax.z ||
        pev->absmin.x > pPlayerEdict->v.maxs.x || // 這裡對應原版的上下限重疊比對
        pev->absmin.y > pPlayerEdict->v.absmax.y ||
        pev->absmin.z > pPlayerEdict->v.absmax.z)
    {
        return FALSE;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;
    if (!pPlayer)
    {
        return FALSE;
    }

    // --- 視野朝向檢查 (30度夾角) ---
    if (m_iToolTarget != 0)
    {
        CBaseEntity *pTargetEnt = UTIL_FindEntityByTargetname(NULL, STRING(m_iToolTarget));
        if (pTargetEnt != NULL)
        {
            // 呼叫虛擬函數表的 FInViewCone，傳入 30.0f (0x41f00000)
            if (!pPlayer->FInViewCone(pTargetEnt, 30.0f))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOL CTriggerUseTool::IsPlayerInTrigger()
{
    // 獲取第一個玩家的 edict (單人劇情主角)
    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict)
    {
        return FALSE;
    }

    // 進行 AABB (Axis-Aligned Bounding Box) 碰撞邊界交叉檢查
    // 檢查玩家的包圍盒是否與觸發器的包圍盒相交
    if (pPlayerEdict->v.absmin.x <= pev->absmax.x &&
        pPlayerEdict->v.absmin.y <= pev->absmax.y &&
        pPlayerEdict->v.absmin.z <= pev->absmax.z &&
        pev->absmin.x <= pPlayerEdict->v.absmax.x &&
        pev->absmin.y <= pPlayerEdict->v.absmax.y &&
        pev->absmin.z <= pPlayerEdict->v.absmax.z)
    {
        return TRUE; // 玩家完全處於觸發器之內
    }

    return FALSE; // 玩家不在區域內
}

int CTriggerUseTool::IsPlayerUsingProperTool()
{
    // 獲取第一個玩家的 edict (單人劇情主角)
    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict)
    {
        pPlayerEdict = g_engfuncs.pfnPEntityOfEntOffset(0);
    }

    if (!pPlayerEdict || !pPlayerEdict->pvPrivateData)
    {
        return 0;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;

    // 獲取玩家當前手持激活武器的 Classname 字串
    const char *pszActiveWeapon = STRING(pPlayer->GetActiveWeaponClassname());
    if (!pszActiveWeapon)
    {
        return 0;
    }

    // --- 1. 檢查拆彈包 (Bomb Defuse) ---
    if (strcmp(pszActiveWeapon, "bomb_defuse") == 0)
    {
        if (m_iToolSet & TOOL_BOMB_DEFUSE) // 0x80
            return TOOL_BOMB_DEFUSE;
    }
    // --- 2. 檢查噴燈 (Blowtorch) ---
    else if (strcmp(pszActiveWeapon, "weapon_blowtorch") == 0)
    {
        if (m_iToolSet & TOOL_BLOWTORCH) // 0x01
            return TOOL_BLOWTORCH;
    }
    // --- 3. 檢查公事包 (Briefcase) ---
    else if (strcmp(pszActiveWeapon, "weapon_briefcase") == 0)
    {
        if (m_iToolSet & TOOL_BRIEFCASE) // 0x02
            return TOOL_BRIEFCASE;
    }
    // --- 4. 檢查相機 (Camera) ---
    else if (strcmp(pszActiveWeapon, "weapon_camera") == 0)
    {
        if (m_iToolSet & TOOL_CAMERA) // 0x04
            return TOOL_CAMERA;
    }
    // --- 5. 檢查光纖鏡 (Fiberoptic Camera) ---
    else if (strcmp(pszActiveWeapon, "weapon_fiberopticcamera") == 0)
    {
        if (m_iToolSet & TOOL_FIBEROPTIC) // 0x08
            return TOOL_FIBEROPTIC;
    }
    // --- 6. 檢查無線電 (Radio) ---
    else if (strcmp(pszActiveWeapon, "weapon_radio") == 0)
    {
        if (m_iToolSet & TOOL_RADIO) // 0x10
            return TOOL_RADIO;
    }
    // --- 7. 檢查遙控炸彈 (RC Bomb) ---
    else if (strcmp(pszActiveWeapon, "weapon_radiocontrolledbomb") == 0)
    {
        if (m_iToolSet & TOOL_RCBOMB) // 0x20
            return TOOL_RCBOMB;
    }
    // --- 8. 檢查注射器 (Syringe) ---
    else if (strcmp(pszActiveWeapon, "weapon_syringe") == 0)
    {
        if (m_iToolSet & TOOL_SYRINGE) // 0x40
            return TOOL_SYRINGE;
    }

    // --- 後備安全檢查：全字串直接比對 ---
    // 如果上面的硬編碼（Hardcoded）快捷分支都沒中，
    // 或者是地圖編輯器使用了完全自定義的武器道具名稱，
    // 則直接比對 m_iToolName 與當前手持武器是否 100% 相同。
    const char *pszRequiredTool = STRING(m_iToolName);
    if (pszRequiredTool && strcmp(pszRequiredTool, pszActiveWeapon) == 0)
    {
        // 這裡會返回觸發器自己設定的任何符合標記，代碼結尾的跳轉證明了這點
        return 1; 
    }

    return 0;
}

void CTriggerUseTool::KeyValue(KeyValueData *pkvd)
{
    // --- 1. 解析 "toolname" (自定義工具武器名稱) ---
    // 內聯長度 9 對應 "toolname" (8字元 + \0)
    if (FStrEq(pkvd->szKeyName, "toolname"))
    {
        m_iToolName = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    // --- 2. 解析 "tooltarget" (要求玩家視角看著的目標實體) ---
    // 內聯長度 11 對應 "tooltarget" (10字元 + \0)
    else if (FStrEq(pkvd->szKeyName, "tooltarget"))
    {
        m_iToolTarget = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    // --- 3. 解析 "rcbombtarget" (遙控炸彈安裝爆破的目標物件) ---
    // 內聯長度 13 對應 "rcbombtarget" (12字元 + \0)
    else if (FStrEq(pkvd->szKeyName, "rcbombtarget"))
    {
        m_iRCBombTarget = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    // --- 4. 解析 "toolset" (內建工具的位元旗標整型值) ---
    // 內聯長度 8 對應 "toolset" (7字元 + \0)
    else if (FStrEq(pkvd->szKeyName, "toolset"))
    {
        m_iToolSet = atoi(pkvd->szValue); // 底層編譯為 strtol(..., 10)
        pkvd->fHandled = TRUE;
    }
    // --- 5. 解析 "bombdefusetime" (該區域的拆彈所需時間) ---
    // 內聯長度 15 對應 "bombdefusetime" (14字元 + \0)
    else if (FStrEq(pkvd->szKeyName, "bombdefusetime"))
    {
        m_iDefuseTime = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    // --- 後備安全處理：交給基底類別解析 ---
    else
    {
        CBaseTrigger::KeyValue(pkvd);
    }
}

void CTriggerUseTool::PreKillTarget()
{
    // 獲取第一個玩家的 edict (單人劇情主角)
    edict_t *pPlayerEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
    if (!pPlayerEdict)
    {
        pPlayerEdict = g_engfuncs.pfnPEntityOfEntOffset(0);
    }

    if (!pPlayerEdict || !pPlayerEdict->pvPrivateData)
    {
        return;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEdict->pvPrivateData;

    // 核心安全檢查：檢查玩家當前鎖定的工具觸發器是不是「自己 (this)」
    // 0x8fc 偏移再次確認對應 pPlayer->m_pCurrentToolTrigger
    if (pPlayer->m_pCurrentToolTrigger == this)
    {
        // 如果是自己，在被刪除前必須安全地將玩家身上的關聯指針清空
        pPlayer->m_pCurrentToolTrigger = NULL;
    }
}

// 建立存檔資料描述表 (Save/Restore Data Description)
// 根據 ReadFields 傳入的常數 6，這裡剛好對應類別中的 6 個核心成員變數
TYPEDESCRIPTION CTriggerUseTool::m_SaveData[] = 
{
    DEFINE_FIELD(CTriggerUseTool, m_iToolName, FIELD_STRING),
    DEFINE_FIELD(CTriggerUseTool, m_iToolTarget, FIELD_STRING),
    DEFINE_FIELD(CTriggerUseTool, m_iRCBombTarget, FIELD_STRING),
    DEFINE_FIELD(CTriggerUseTool, m_iToolSet, FIELD_INTEGER),
    DEFINE_FIELD(CTriggerUseTool, m_iDefuseTime, FIELD_INTEGER),
    DEFINE_FIELD(CTriggerUseTool, m_bombPlanted, FIELD_BOOLEAN),
};

int CTriggerUseTool::Restore(CRestore &restore)
{
    // 1. 首先呼叫繼承鏈中的上層類別 CBaseToggle 進行還原
    if (!CBaseToggle::Restore(restore))
    {
        return 0;
    }

    // 2. 還原中介類別 CBaseTrigger 的專屬資料欄位 (包含 6 個元素)
    if (!restore.ReadFields("CBaseTrigger", this, CBaseTrigger::m_SaveData, CBaseTrigger_SAVE_COUNT))
    {
        return 0;
    }

    // 3. 還原目前類別 CTriggerUseTool 本身的 6 個資料欄位
    // 這裡的 sizeof(m_SaveData) / sizeof(m_SaveData[0]) 在編譯後就是常數 6
    return restore.ReadFields("CTriggerUseTool", this, m_SaveData, ARRAYSIZE(m_SaveData));
}

int CTriggerUseTool::Save(CSave &save)
{
    // 1. 首先呼叫上層父類別 CBaseToggle 執行基礎實體狀態的存檔
    if (!CBaseToggle::Save(save))
    {
        return 0;
    }

    // 2. 寫入中介父類別 CBaseTrigger 的 6 個內置存檔欄位
    if (!save.WriteFields("CBaseTrigger", this, CBaseTrigger::m_SaveData, CBaseTrigger_SAVE_COUNT))
    {
        return 0;
    }

    // 3. 寫入目前 CTriggerUseTool 類別本身的 6 個自定義成員變數欄位
    // ARRAYSIZE(m_SaveData) 在編譯期間被直接轉化為常數 6
    return save.WriteFields("CTriggerUseTool", this, m_SaveData, ARRAYSIZE(m_SaveData));
}

void CTriggerUseTool::TouchTriggerUseTool(CBaseEntity *pOther)
{
    // 檢查碰觸實體是否為玩家 (虛擬函數表 0x2a 處的 IsPlayer())
    if (!pOther->IsPlayer())
        return;

    CBasePlayer *pPlayer = (CBasePlayer *)pOther;

    // 檢查 Master 觸發器狀態
    if (!UTIL_IsMasterTriggered(m_sMaster, pPlayer))
        return;

    // 如果玩家當前記錄的工具觸發器不是自己，且難易度為簡單 (Easy = 1)，顯示 HUD 提示
    // 0x19._vptr 經推導就是 pPlayer->m_pCurrentToolTrigger 的底層錯位表現
    if (pPlayer->m_pCurrentToolTrigger != this && g_iSkillLevel == 1)
    {
        DisplayToolMessage(); // 呼叫外部難度檢查接口
    }

    // 鎖定當前觸發器到玩家對象身上
    pPlayer->m_pCurrentToolTrigger = this;

    // 計算當前空間與手上道具狀態
    BOOL bInPosition     = IsPlayerInPosition();
    int  iActiveToolFlag = IsPlayerUsingProperTool(); // 返回 ToolSet_e 的特定位元值

    // 如果玩家在線
    if (!pPlayer->IsDisconnected())
    {
        // 獲取玩家當前手持武器名與此觸發器要求的工具名
        const char *pszActiveWeapon = STRING(pPlayer->GetActiveWeaponClassname());
        const char *pszRequiredTool = STRING(m_iToolName);

        // --- 道具 1: 噴燈 (Blowtorch) ---
        if ((pszRequiredTool && strcmp(pszRequiredTool, "weapon_blowtorch") == 0) || (m_iToolSet & TOOL_BLOWTORCH))
        {
            pPlayer->m_iToolFlags |= TOOL_BLOWTORCH;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_BLOWTORCH;
            }
            pPlayer->m_iToolUsingFlags |= iActiveToolFlag; 
        }

        // --- 道具 2: 拆彈包 (Bomb Defuse) ---
        // 注意：拆彈包在玩家身上的標記值是 0x200 (512)
        if ((pszRequiredTool && strcmp(pszRequiredTool, "bomb_defuse") == 0) || (m_iToolSet & TOOL_BOMB_DEFUSE))
        {
            pPlayer->m_iToolFlags |= 0x200;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= 0x200;
            }
            pPlayer->m_iToolUsingFlags |= 0x200;
        }

        // --- 道具 3: 公事包 (Briefcase) ---
        if ((pszRequiredTool && strcmp(pszRequiredTool, "weapon_briefcase") == 0) || (m_iToolSet & TOOL_BRIEFCASE))
        {
            pPlayer->m_iToolFlags |= TOOL_BRIEFCASE;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_BRIEFCASE;
            }
            pPlayer->m_iToolUsingFlags |= iActiveToolFlag;
        }

        // --- 道具 4: 照相機 (Camera) ---
        if ((pszRequiredTool && strcmp(pszRequiredTool, "weapon_camera") == 0) || (m_iToolSet & TOOL_CAMERA))
        {
            pPlayer->m_iToolFlags |= TOOL_CAMERA;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_CAMERA;
            }
            pPlayer->m_iToolUsingFlags |= iActiveToolFlag;
        }

        // --- 道具 5: 光纖鏡 (Fiberoptic Camera) ---
        if ((pszRequiredTool && strcmp(pszRequiredTool, "weapon_fiberopticcamera") == 0) || (m_iToolSet & TOOL_FIBEROPTIC))
        {
            pPlayer->m_iToolFlags |= TOOL_FIBEROPTIC;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_FIBEROPTIC;
            }
            pPlayer->m_iToolUsingFlags |= iActiveToolFlag;
        }

        // --- 道具 6: 無線電 (Radio) ---
        if ((pszRequiredTool && strcmp(pszRequiredTool, "weapon_radio") == 0) || (m_iToolSet & TOOL_RADIO))
        {
            pPlayer->m_iToolFlags |= TOOL_RADIO;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_RADIO;
            }
            pPlayer->m_iToolUsingFlags |= iActiveToolFlag;
        }

        // --- 道具 7: 遙控炸彈 (Radio Controlled Bomb) ---
        if ((pszRequiredTool && strcmp(pszRequiredTool, "weapon_radiocontrolledbomb") == 0) || (m_iToolSet & TOOL_RCBOMB))
        {
            pPlayer->m_iToolFlags |= TOOL_RCBOMB;
            if (bInPosition)
            {
                pPlayer->m_iToolInPositionFlags |= TOOL_RCBOMB;
            }
            pPlayer->m_iToolUsingFlags |= iActiveToolFlag;
        }


