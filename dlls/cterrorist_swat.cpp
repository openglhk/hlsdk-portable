#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "cterrorist.h"

class CCounterTerroristSwat : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristSwat::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/seal.mdl" );
    }
    CCounterTerrorist::Precache();
}

void CCounterTerroristSwat::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/seal.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    m_ctType = 0;

    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "AM" );
    }
}

LINK_ENTITY_TO_CLASS( monster_counterterrorist_swat, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_assaultrifle, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_grenader, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_law, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_machinegun, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_mp5, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_pistol, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_shotgun, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_smg, CCounterTerroristSwat );
LINK_ENTITY_TO_CLASS( monster_ct_swat_sniperrifle, CCounterTerroristSwat );
