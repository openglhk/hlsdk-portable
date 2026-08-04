#include "terrorist.h"

class CTerroristUrban : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristUrban::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/militia.mdl" );
    }
    CTerrorist::Precache();
}

void CTerroristUrban::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/militia.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }
    m_terroristType = 2; 
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "AM" );
    }
}

static void CreateUrbanVariant(entvars_t *pev, const char *pszClassName)
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

    CTerroristUrban *pUrban = (CTerroristUrban *)pfnPvAllocEntPrivateData(pEdict, sizeof(CTerrorist));
    if (pUrban == nullptr)
    {
        return;
    }

    pUrban->CTerroristUrban::CTerroristUrban();

    pev->classname = ALLOC_STRING(pszClassName);
    pEdict->pvPrivateData = pUrban;
    pUrban->pev = pev;
}

extern "C" void monster_terrorist_urban(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban, monster_terrorist_urban );

extern "C" void monster_terrorist_urban_assaultrifle(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_assaultrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_assaultrifle, monster_terrorist_urban_assaultrifle );

extern "C" void monster_terrorist_urban_grenader(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_grenader");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_grenader, monster_terrorist_urban_grenader );

extern "C" void monster_terrorist_urban_kamakazi(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_kamakazi");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_kamakazi, monster_terrorist_urban_kamakazi );

extern "C" void monster_terrorist_urban_law(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_law");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_law, monster_terrorist_urban_law );

extern "C" void monster_terrorist_urban_machinegun(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_machinegun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_machinegun, monster_terrorist_urban_machinegun );

extern "C" void monster_terrorist_urban_melee(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_melee");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_melee, monster_terrorist_urban_melee );

extern "C" void monster_terrorist_urban_mp5(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_mp5");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_mp5, monster_terrorist_urban_mp5 );

extern "C" void monster_terrorist_urban_pistol(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_pistol");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_pistol, monster_terrorist_urban_pistol );

extern "C" void monster_terrorist_urban_shotgun(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_shotgun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_shotgun, monster_terrorist_urban_shotgun );

extern "C" void monster_terrorist_urban_smg(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_smg");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_smg, monster_terrorist_urban_smg );

extern "C" void monster_terrorist_urban_sniperrifle(entvars_t *pev)
{
    CreateUrbanVariant(pev, "monster_terrorist_urban_sniperrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_urban_sniperrifle, monster_terrorist_urban_sniperrifle );
