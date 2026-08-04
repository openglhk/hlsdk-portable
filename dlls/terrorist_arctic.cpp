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

static void CreateArcticVariant(entvars_t *pev, const char *pszClassName)
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

    CTerroristArctic *pArctic = (CTerroristArctic *)pfnPvAllocEntPrivateData(pEdict, sizeof(CTerrorist));
    if (pArctic == nullptr)
    {
        return;
    }

    pArctic->CTerroristArctic::CTerroristArctic();

    pev->classname = ALLOC_STRING(pszClassName);
    pEdict->pvPrivateData = pArctic;
    pArctic->pev = pev;
}

extern "C" void monster_terrorist_arctic(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic, monster_terrorist_arctic );

extern "C" void monster_terrorist_arctic_assaultrifle(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_assaultrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_assaultrifle, monster_terrorist_arctic_assaultrifle );

extern "C" void monster_terrorist_arctic_grenader(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_grenader");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_grenader, monster_terrorist_arctic_grenader );

extern "C" void monster_terrorist_arctic_kamakazi(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_kamakazi");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_kamakazi, monster_terrorist_arctic_kamakazi );

extern "C" void monster_terrorist_arctic_law(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_law");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_law, monster_terrorist_arctic_law );

extern "C" void monster_terrorist_arctic_machinegun(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_machinegun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_machinegun, monster_terrorist_arctic_machinegun );

extern "C" void monster_terrorist_arctic_melee(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_melee");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_melee, monster_terrorist_arctic_melee );

extern "C" void monster_terrorist_arctic_mp5(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_mp5");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_mp5, monster_terrorist_arctic_mp5 );

extern "C" void monster_terrorist_arctic_pistol(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_pistol");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_pistol, monster_terrorist_arctic_pistol );

extern "C" void monster_terrorist_arctic_shotgun(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_shotgun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_shotgun, monster_terrorist_arctic_shotgun );

extern "C" void monster_terrorist_arctic_smg(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_smg");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_smg, monster_terrorist_arctic_smg );

extern "C" void monster_terrorist_arctic_sniperrifle(entvars_t *pev)
{
    CreateArcticVariant(pev, "monster_terrorist_arctic_sniperrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_arctic_sniperrifle, monster_terrorist_arctic_sniperrifle );
