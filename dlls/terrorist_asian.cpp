#include "terrorist.h"

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

static void CreateAsianVariant(entvars_t *pev, const char *pszClassName)
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

    CTerroristAsian *pAsian = (CTerroristAsian *)pfnPvAllocEntPrivateData(pEdict, sizeof(CTerrorist));
    if (pAsian == nullptr)
    {
        return;
    }

    pAsian->CTerroristAsian::CTerroristAsian();

    pev->classname = ALLOC_STRING(pszClassName);
    pEdict->pvPrivateData = pAsian;
    pAsian->pev = pev;
}

extern "C" void monster_terrorist_asian(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian, monster_terrorist_asian );

extern "C" void monster_terrorist_asian_assaultrifle(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_assaultrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_assaultrifle, monster_terrorist_asian_assaultrifle );

extern "C" void monster_terrorist_asian_grenader(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_grenader");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_grenader, monster_terrorist_asian_grenader );

extern "C" void monster_terrorist_asian_kamakazi(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_kamakazi");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_kamakazi, monster_terrorist_asian_kamakazi );

extern "C" void monster_terrorist_asian_law(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_law");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_law, monster_terrorist_asian_law );

extern "C" void monster_terrorist_asian_machinegun(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_machinegun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_machinegun, monster_terrorist_asian_machinegun );

extern "C" void monster_terrorist_asian_melee(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_melee");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_melee, monster_terrorist_asian_melee );

extern "C" void monster_terrorist_asian_mp5(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_mp5");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_mp5, monster_terrorist_asian_mp5 );

extern "C" void monster_terrorist_asian_pistol(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_pistol");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_pistol, monster_terrorist_asian_pistol );

extern "C" void monster_terrorist_asian_shotgun(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_shotgun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_shotgun, monster_terrorist_asian_shotgun );

extern "C" void monster_terrorist_asian_smg(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_smg");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_smg, monster_terrorist_asian_smg );

extern "C" void monster_terrorist_asian_sniperrifle(entvars_t *pev)
{
    CreateAsianVariant(pev, "monster_terrorist_asian_sniperrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_asian_sniperrifle, monster_terrorist_asian_sniperrifle );
