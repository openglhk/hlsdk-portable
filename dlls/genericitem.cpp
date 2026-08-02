#include "genericitem.h"

// 🚀 核心註冊：將 Hammer 地圖編輯器中的類名 "item_generic" 綁定到這個 C++ 類別上
LINK_ENTITY_TO_CLASS( item_generic, CGenericItem );

// 存檔數據表定義
TYPEDESCRIPTION CGenericItem::m_SaveData[] =
{
    DEFINE_FIELD( CGenericItem, m_bCanSpawn, FIELD_BOOLEAN ),
    DEFINE_FIELD( CGenericItem, m_iSequence, FIELD_STRING ),
    DEFINE_FIELD( CGenericItem, m_iLightValue, FIELD_INTEGER ),
};

void CGenericItem::Precache( void )
{
    // 調用引擎底層預加載地圖為該道具指定的模型資源
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING(pev->model) );
    }
}

void CGenericItem::KeyValue( KeyValueData *pkvd )
{
    // 解析地圖編輯器傳入的自定義屬性
    if ( strcasecmp( pkvd->szKeyName, "sequence" ) == 0 )
    {
        m_iSequence = ALLOC_STRING( pkvd->szValue );
        pkvd->fHandled = TRUE;
    }
    else if ( strcasecmp( pkvd->szKeyName, "lightvalue" ) == 0 )
    {
        m_iLightValue = atoi( pkvd->szValue );
        pkvd->fHandled = TRUE;
    }
    else
    {
        CBaseAnimating::KeyValue( pkvd );
    }
}

// 貼上我們上一個回合完美還原的 Spawn() 函數
void CGenericItem::Spawn( void )
{
    BOOL bValidRules = ( g_pGameRules != nullptr );
    m_bCanSpawn = bValidRules;

    if ( bValidRules )
    {
        pev->solid    = SOLID_NOT;
        pev->movetype = MOVETYPE_NONE;
        pev->effects  = 0;
        pev->frame    = 0.0f;
        pev->iuser1   = 0;

        pev->iuser3   = pev->spawnflags;
        pev->flags   |= FL_DONTTHINK; 

        Precache();
        SET_MODEL( edict(), STRING(pev->model) );

        if ( m_iSequence == 0 )
        {
            // 靜態道具
        }
        else
        {
            SetThink( &CGenericItem::StartItem );
            pev->nextthink = gpGlobals->time + 0.1f;
        }

        if ( pev->spawnflags & 1 ) // SF_ITEM_DROP_TO_FLOOR
        {
            if ( DROP_TO_FLOOR( edict() ) == 0 )
            {
                ALERT( at_error, "Item %s fell out of level at %f,%f,%f", 
                       STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z );
                UTIL_Remove( this );
                return;
            }
        }

        if ( pev->spawnflags & 2 ) // SF_ITEM_SOLID_FROM_ANIM
        {
            pev->solid = SOLID_SLIDEBOX;
            int iSeqIndex = LookupSequence( STRING(m_iSequence) );
            
            if ( iSeqIndex == -1 )
            {
                ALERT( at_console, "ERROR! FIX ME: item generic: %s, model: %s, does not have animation: %s\n",
                       STRING(pev->targetname), STRING(pev->model), STRING(m_iSequence) );
                iSeqIndex = 0;
            }

            ExtractBbox( iSeqIndex, pev->mins, pev->maxs );
            UTIL_SetSize( pev, pev->mins, pev->maxs );
            UTIL_SetOrigin( pev, pev->origin );
        }

        if ( m_iLightValue != 0 )
        {
            pev->renderfx = kRenderFxGlowShell;
            pev->iuser4   = m_iLightValue;
        }

        if ( pev->rendermode == kRenderNormal && pev->renderamt == 0.0f )
        {
            pev->renderamt  = 255.0f;
            pev->rendermode = kRenderTransTexture;
            return;
        }
    }
    else
    {
        SetThink( &CGenericItem::SUB_Remove );
        pev->nextthink = gpGlobals->time + 1.0f;
    }
}

int CGenericItem::Save( CSave &save )
{
    if ( !CBaseAnimating::Save( save ) ) return 0;
    return save.WriteFields( "CGenericItem", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}

int CGenericItem::Restore( CRestore &restore )
{
    if ( !CBaseAnimating::Restore( restore ) ) return 0;
    return restore.ReadFields( "CGenericItem", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData) );
}
