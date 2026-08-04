#include "terrorist.h"

class CTerroristRussian : public CTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CTerroristRussian::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/terror.mdl" );
    }
    CTerrorist::Precache();
}

void CTerroristRussian::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/terror.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }
    m_terroristType = 4; 
    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "RU" );
    }
}

static void CreateRussianVariant(entvars_t *pev, const char *pszClassName)
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

    CTerroristRussian *pRussian = (CTerroristRussian *)pfnPvAllocEntPrivateData(pEdict, sizeof(CTerrorist));
    if (pRussian == nullptr)
    {
        return;
    }

    pRussian->CTerroristRussian::CTerroristRussian();

    pev->classname = ALLOC_STRING(pszClassName);
    pEdict->pvPrivateData = pRussian;
    pRussian->pev = pev;
}

extern "C" void monster_terrorist_russian(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian, monster_terrorist_russian );

extern "C" void monster_terrorist_russian_assaultrifle(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_assaultrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_assaultrifle, monster_terrorist_russian_assaultrifle );

extern "C" void monster_terrorist_russian_grenader(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_grenader");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_grenader, monster_terrorist_russian_grenader );

extern "C" void monster_terrorist_russian_kamakazi(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_kamakazi");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_kamakazi, monster_terrorist_russian_kamakazi );

extern "C" void monster_terrorist_russian_law(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_law");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_law, monster_terrorist_russian_law );

extern "C" void monster_terrorist_russian_machinegun(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_machinegun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_machinegun, monster_terrorist_russian_machinegun );

extern "C" void monster_terrorist_russian_melee(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_melee");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_melee, monster_terrorist_russian_melee );

extern "C" void monster_terrorist_russian_mp5(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_mp5");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_mp5, monster_terrorist_russian_mp5 );

extern "C" void monster_terrorist_russian_pistol(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_pistol");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_pistol, monster_terrorist_russian_pistol );

extern "C" void monster_terrorist_russian_shotgun(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_shotgun");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_shotgun, monster_terrorist_russian_shotgun );

extern "C" void monster_terrorist_russian_smg(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_smg");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_smg, monster_terrorist_russian_smg );

extern "C" void monster_terrorist_russian_sniperrifle(entvars_t *pev)
{
    CreateRussianVariant(pev, "monster_terrorist_russian_sniperrifle");
}
LINK_ENTITY_TO_FUNCPTR( monster_terrorist_russian_sniperrifle, monster_terrorist_russian_sniperrifle );
