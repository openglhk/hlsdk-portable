#include "terrorist.h"

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
        m_language = ALLOC_STRING( "AR" );
    }
}

static void CreateDesertVariant(entvars_t *pev, const char *pszClassName)
{
    if (pev == nullptr)
    {
        edict_t *pNewEdict = CREATE_ENTITY();
        if (pNewEdict != nullptr)
        {
            pev = &pNewEdict->v;
        }
        else
        {
            return;
        }
    }

    edict_t *pEdict = pev->pContainingEntity;
    if (pEdict != nullptr && pEdict->pvPrivateData != nullptr)
    {
        return;
    }

    CTerroristDesert *pDesert = (CTerroristDesert *)pfnPvAllocEntPrivateData(pEdict, sizeof(CTerrorist));
    if (pDesert == nullptr)
    {
        return;
    }

    pDesert->CTerroristDesert::CTerroristDesert();

    pev->classname = ALLOC_STRING(pszClassName);
    pEdict->pvPrivateData = pDesert;
    pDesert->pev = pev;
}

extern "C" void monster_terrorist_desert(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert, monster_terrorist_desert );

extern "C" void monster_terrorist_desert_assaultrifle(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_assaultrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_assaultrifle, monster_terrorist_desert_assaultrifle );

extern "C" void monster_terrorist_desert_grenader(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_grenader");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_grenader, monster_terrorist_desert_grenader );

extern "C" void monster_terrorist_desert_kamakazi(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_kamakazi");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_kamakazi, monster_terrorist_desert_kamakazi );

extern "C" void monster_terrorist_desert_law(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_law");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_law, monster_terrorist_desert_law );

extern "C" void monster_terrorist_desert_machinegun(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_machinegun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_machinegun, monster_terrorist_desert_machinegun );

extern "C" void monster_terrorist_desert_melee(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_melee");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_melee, monster_terrorist_desert_melee );

extern "C" void monster_terrorist_desert_mp5(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_mp5");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_mp5, monster_terrorist_desert_mp5 );

extern "C" void monster_terrorist_desert_pistol(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_pistol");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_pistol, monster_terrorist_desert_pistol );

extern "C" void monster_terrorist_desert_shotgun(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_shotgun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_shotgun, monster_terrorist_desert_shotgun );

extern "C" void monster_terrorist_desert_smg(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_smg");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_smg, monster_terrorist_desert_smg );

extern "C" void monster_terrorist_desert_sniperrifle(entvars_t *pev)
{
    CreateDesertVariant(pev, "monster_terrorist_desert_sniperrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_desert_sniperrifle, monster_terrorist_desert_sniperrifle );
