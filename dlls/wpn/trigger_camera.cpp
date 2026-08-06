#define SF_CAMERA_PLAYER_TAKEOFF     0x0001 // 從玩家視角位置啟動相機
#define SF_CAMERA_PLAYER_TARGET      0x0002 // 將玩家自身設為追蹤目標
#define SF_CAMERA_FREEZE_PLAYER      0x0004 // 凍結玩家控制
#define SF_CAMERA_DISTORTION         0x0008 // 安全相機畫面扭曲效果 (全螢幕 Overlay 特效)
#define SF_CAMERA_SNAPTO             0x0010 // 直接瞬間轉向目標而不進行平滑插值
#define SF_CAMERA_SMOOTH             0x0020 // 移動時不進行慣性減速

class CTriggerCamera : public CBaseDelay
{
public:
    void Spawn(void) override;
    void KeyValue(KeyValueData *pkvd) override;
    void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) override;
    int ObjectCaps(void) override;
    
    int Save(CSave &save) override;
    int Restore(CRestore &restore) override;

    void EXPORT FollowTarget(void);
    void Move(void);
    BOOL MinimumTimeMet(void);

    static TYPEDESCRIPTION m_SaveData[];

public:
    EHANDLE m_hPlayer;       // 當前正在觀看此相機的玩家
    EHANDLE m_hTarget;       // 相機鏡頭追蹤的目標實體
    CBaseEntity *m_pentPath; // 相機目前正在前往的路徑節點 (path_corner)
    
    float m_flReturnTime;    // 相機自動關閉並切回玩家視角的剩餘時間
    float m_flMinimumTime;   // 相機啟動後的最小運作保護時間 (避免瞬間被連續觸發關閉)
    float m_moveDistance;    // 距離下一個路徑節點的剩餘距離
    float m_targetSpeed;     // 目標移動速度
    float m_initialSpeed;    // 由地圖編輯器設定的初始移動速度
    float m_acceleration;    // 相機移動加速度
    float m_deceleration;    // 相機移動減速度
    
    int m_state;             // 相機當前狀態 (0 = 關閉, 1 = 啟動中)
    int m_sPath;             // 路徑名稱字串索引 (ALLOC_STRING)
    
    // CZDS (Condition Zero Deleted Scenes) 專屬新增欄位
    int m_useOverlay;        // 是否啟用單人任務專屬的螢幕安全監控 HUD 疊加特效
    int m_immediate;         // 是否立即執行特定邏輯
};

#endif // TRIGGER_CAMERA_H

TYPEDESCRIPTION CTriggerCamera::m_SaveData[] =
{
    DEFINE_FIELD(CTriggerCamera, m_hPlayer, FIELD_EHANDLE),
    DEFINE_FIELD(CTriggerCamera, m_hTarget, FIELD_EHANDLE),
    DEFINE_FIELD(CTriggerCamera, m_pentPath, FIELD_CLASSPTR),
    DEFINE_FIELD(CTriggerCamera, m_flReturnTime, FIELD_TIME),
    DEFINE_FIELD(CTriggerCamera, m_flMinimumTime, FIELD_TIME),
    DEFINE_FIELD(CTriggerCamera, m_moveDistance, FIELD_FLOAT),
    DEFINE_FIELD(CTriggerCamera, m_targetSpeed, FIELD_FLOAT),
    DEFINE_FIELD(CTriggerCamera, m_initialSpeed, FIELD_FLOAT),
    DEFINE_FIELD(CTriggerCamera, m_acceleration, FIELD_FLOAT),
    DEFINE_FIELD(CTriggerCamera, m_deceleration, FIELD_FLOAT),
    DEFINE_FIELD(CTriggerCamera, m_state, FIELD_INTEGER),
    DEFINE_FIELD(CTriggerCamera, m_sPath, FIELD_STRING),
    
    // CZDS (Condition Zero Deleted Scenes) 專屬新增序列化欄位
    DEFINE_FIELD(CTriggerCamera, m_useOverlay, FIELD_INTEGER),
    DEFINE_FIELD(CTriggerCamera, m_immediate, FIELD_INTEGER),
    
    // 預留或補齊基底類別/對齊所需的欄位（以完美契合反編譯中的 0x10 陣列長度）
    DEFINE_FIELD(CTriggerCamera, m_flWait, FIELD_FLOAT),
    DEFINE_FIELD(CTriggerCamera, m_flStopTime, FIELD_TIME),
};

void CTriggerCamera::Spawn(void)
{
    pev->movetype = MOVETYPE_NONE;
    pev->solid = SOLID_NOT;
    pev->renderamt = 0.0f;
    pev->rendermode = kRenderTransTexture;
    m_initialSpeed = pev->speed;

    if (m_acceleration == 0.0f)
    {
        m_acceleration = 500.0f;
    }

    if (m_deceleration == 0.0f)
    {
        m_deceleration = 500.0f;
    }
}

void CTriggerCamera::FollowTarget(void)
{
    if (m_hPlayer == nullptr)
    {
        return;
    }

    if (m_hTarget == nullptr || m_flReturnTime < gpGlobals->time)
    {
        if (m_hPlayer->IsNetClient())
        {
            g_engfuncs.pfnSetView(m_hPlayer->edict(), m_hPlayer->edict());
            m_hPlayer->EnableControl(TRUE);
        }

        SUB_UseTargets(this, USE_TOGGLE, 0.0f);
        pev->avelocity = g_vecZero;

        CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(1));
        if (m_useOverlay && pPlayer != nullptr)
        {
            pPlayer->SetSecurityCamera(false, nullptr);
        }

        m_state = 0;
    }
    else
    {
        Vector vecDir = m_hTarget->pev->origin - pev->origin;
        Vector vecAngles;
        UTIL_VecToAngles(vecDir, vecAngles);
        vecAngles.x = -vecAngles.x;

        if (!(pev->spawnflags & SF_CAMERA_SNAPTO))
        {
            if (pev->angles.y > 360.0f)
            {
                pev->angles.y -= 360.0f;
            }
            if (pev->angles.y < 0.0f)
            {
                pev->angles.y += 360.0f;
            }

            float dx = vecAngles.x - pev->angles.x;
            float dy = vecAngles.y - pev->angles.y;

            if (dx < -180.0f) dx += 360.0f;
            if (dx > 180.0f)  dx -= 360.0f;
            if (dy < -180.0f) dy += 360.0f;
            if (dy > 180.0f)  dy -= 360.0f;

            pev->avelocity.x = dx * 40.0f * gpGlobals->frametime;
            pev->avelocity.y = dy * 40.0f * gpGlobals->frametime;
        }
        else
        {
            pev->angles = vecAngles;
            pev->avelocity = g_vecZero;
            pev->velocity = g_vecZero;
        }

        if (!(pev->spawnflags & SF_CAMERA_SMOOTH))
        {
            pev->velocity = pev->velocity * 0.8f;
            if (pev->velocity.Length() < 10.0f)
            {
                pev->velocity = g_vecZero;
            }
        }

        pev->nextthink = gpGlobals->time;
        Move();
    }
}

void CTriggerCamera::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "wait"))
    {
        m_flWait = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "useoverlay"))
    {
        m_useOverlay = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "moveto"))
    {
        m_sPath = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "acceleration"))
    {
        m_acceleration = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "deceleration"))
    {
        m_deceleration = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "immediate"))
    {
        m_immediate = (atoi(pkvd->szValue) != 0);
        pkvd->fHandled = TRUE;
    }
    else
    {
        CBaseDelay::KeyValue(pkvd);
    }
}

BOOL CTriggerCamera::MinimumTimeMet(void)
{
    return (m_flMinimumTime <= gpGlobals->time);
}

void CTriggerCamera::Move(void)
{
    if (m_pentPath == nullptr)
    {
        return;
    }

    m_moveDistance -= pev->speed * gpGlobals->frametime;

    if (m_moveDistance <= 0.0f)
    {
        if (m_pentPath->pev->message != 0)
        {
            FireTargets(STRING(m_pentPath->pev->message), this, this, USE_TOGGLE, 0.0f);
            if (m_pentPath->pev->spawnflags & SF_CORNER_FIREONCE)
            {
                m_pentPath->pev->message = 0;
            }
        }

        m_pentPath = m_pentPath->GetNextCorner();

        if (m_pentPath == nullptr)
        {
            pev->velocity = g_vecZero;
        }
        else
        {
            if (m_pentPath->pev->speed != 0.0f)
            {
                m_targetSpeed = m_pentPath->pev->speed;
            }

            Vector vecDelta = m_pentPath->pev->origin - pev->origin;
            m_moveDistance = vecDelta.Length();

            if (m_moveDistance == 0.0f)
            {
                pev->movedir = Vector(0.0f, 0.0f, 1.0f);
            }
            else
            {
                pev->movedir = vecDelta.Normalize();
            }

            m_flStopTime = gpGlobals->time + m_pentPath->GetDelay();
        }
    }

    if (gpGlobals->time < m_flStopTime)
    {
        pev->speed = UTIL_Approach(0.0f, pev->speed, m_deceleration * gpGlobals->frametime);
    }
    else
    {
        pev->speed = UTIL_Approach(m_targetSpeed, pev->speed, m_acceleration * gpGlobals->frametime);
    }

    float fraction = gpGlobals->frametime * 2.0f;
    pev->velocity = (pev->velocity * (1.0f - fraction)) + (pev->movedir * pev->speed * fraction);
}

int CTriggerCamera::ObjectCaps(void)
{
    return 0;
}

int CTriggerCamera::Restore(CRestore &restore)
{
    if (!CBaseDelay::Restore(restore))
    {
        return 0;
    }

    return restore.ReadFields("CTriggerCamera", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData[0]));
}

int CTriggerCamera::Save(CSave &save)
{
    if (!CBaseDelay::Save(save))
    {
        return 0;
    }

    return save.WriteFields("CTriggerCamera", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData));
}

void CTriggerCamera::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    if (!ShouldToggle(useType, m_state))
    {
        return;
    }

    CBasePlayer *pLocalPlayer = (CBasePlayer *)CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(1));

    if (m_state != 0)
    {
        m_state = 0;
        m_flReturnTime = gpGlobals->time;
        return;
    }

    m_state = 1;

    if (pActivator == nullptr || !pActivator->IsPlayer())
    {
        pActivator = pLocalPlayer;
    }

    m_hPlayer = pActivator;
    m_flReturnTime = gpGlobals->time + m_flWait;
    m_flMinimumTime = gpGlobals->time + 0.3f;
    pev->speed = m_initialSpeed;
    m_targetSpeed = m_initialSpeed;

    if (!(pev->spawnflags & SF_CAMERA_PLAYER_TARGET))
    {
        m_hTarget = GetNextTarget();
    }
    else
    {
        m_hTarget = m_hPlayer;
    }

    if (m_hTarget == nullptr)
    {
        return;
    }

    if (pev->spawnflags & SF_CAMERA_FREEZE_PLAYER)
    {
        ((CBasePlayer *)pActivator)->EnableControl(FALSE);
    }

    if (m_sPath == 0)
    {
        m_pentPath = nullptr;
        m_flStopTime = gpGlobals->time;
    }
    else
    {
        m_pentPath = CBaseEntity::Instance(g_engfuncs.pfnFindEntityByString(nullptr, "targetname", STRING(m_sPath)));
        m_flStopTime = gpGlobals->time;

        if (m_pentPath != nullptr)
        {
            if (m_pentPath->pev->speed != 0.0f)
            {
                m_targetSpeed = m_pentPath->pev->speed;
            }
            m_flStopTime += m_pentPath->GetDelay();
        }
    }

    if (!(pev->spawnflags & SF_CAMERA_PLAYER_TAKEOFF))
    {
        pev->velocity = g_vecZero;
    }
    else
    {
        UTIL_SetOrigin(pev, pActivator->pev->origin + pActivator->pev->view_ofs);
        pev->angles.x = -pActivator->pev->angles.x;
        pev->angles.y = pActivator->pev->angles.y;
        pev->angles.z = 0.0f;
        pev->velocity = pActivator->pev->velocity;
    }

    if (m_useOverlay)
    {
        pLocalPlayer->SetSecurityCamera(true, (pev->spawnflags & SF_CAMERA_DISTORTION) ? this : nullptr);
    }

    g_engfuncs.pfnSetView(pActivator->edict(), edict());
    g_engfuncs.pfnSetModel(edict(), STRING(pActivator->pev->model));

    SetThink(&CTriggerCamera::FollowTarget);
    pev->nextthink = gpGlobals->time;
    m_moveDistance = 0.0f;
    Move();
}

