#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "cterrorist.h"

class CCounterTerroristSpetsnaz : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristSpetsnaz::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/spetsnaz.mdl" );
    }
    CCounterTerrorist::Precache();
}

void CCounterTerroristSpetsnaz::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/spetsnaz.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    m_ctType = 3;

    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "RU" );
    }
}

LINK_ENTITY_TO_CLASS( monster_counterterrorist_spetsnaz, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_assaultrifle, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_grenader, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_law, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_machinegun, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_mp5, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_pistol, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_shotgun, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_smg, CCounterTerroristSpetsnaz );
LINK_ENTITY_TO_CLASS( monster_ct_spetsnaz_sniperrifle, CCounterTerroristSpetsnaz );
