#include "terrorist.h"

class CTerroristJungle : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristJungle::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/guerilla.mdl" );
    }
    CTerrorist::Precache();
}

void CTerroristJungle::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/guerilla.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }
    m_terroristType = 3; 
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "SP" );
    }
}

static void CreateJungleVariant(entvars_t *pev, const char *pszClassName)
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

    CTerroristJungle *pJungle = (CTerroristJungle *)pfnPvAllocEntPrivateData(pEdict, sizeof(CTerrorist));
    if (pJungle == nullptr)
    {
        return;
    }

    pJungle->CTerroristJungle::CTerroristJungle();

    pev->classname = ALLOC_STRING(pszClassName);
    pEdict->pvPrivateData = pJungle;
    pJungle->pev = pev;
}

extern "C" void monster_terrorist_jungle(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle, monster_terrorist_jungle );

extern "C" void monster_terrorist_jungle_assaultrifle(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_assaultrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_assaultrifle, monster_terrorist_jungle_assaultrifle );

extern "C" void monster_terrorist_jungle_grenader(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_grenader");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_grenader, monster_terrorist_jungle_grenader );

extern "C" void monster_terrorist_jungle_kamakazi(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_kamakazi");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_kamakazi, monster_terrorist_jungle_kamakazi );

extern "C" void monster_terrorist_jungle_law(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_law");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_law, monster_terrorist_jungle_law );

extern "C" void monster_terrorist_jungle_machinegun(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_machinegun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_machinegun, monster_terrorist_jungle_machinegun );

extern "C" void monster_terrorist_jungle_melee(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_melee");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_melee, monster_terrorist_jungle_melee );

extern "C" void monster_terrorist_jungle_mp5(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_mp5");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_mp5, monster_terrorist_jungle_mp5 );

extern "C" void monster_terrorist_jungle_pistol(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_pistol");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_pistol, monster_terrorist_jungle_pistol );

extern "C" void monster_terrorist_jungle_shotgun(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_shotgun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_shotgun, monster_terrorist_jungle_shotgun );

extern "C" void monster_terrorist_jungle_smg(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_smg");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_smg, monster_terrorist_jungle_smg );

extern "C" void monster_terrorist_jungle_sniperrifle(entvars_t *pev)
{
    CreateJungleVariant(pev, "monster_terrorist_jungle_sniperrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_jungle_sniperrifle, monster_terrorist_jungle_sniperrifle );
