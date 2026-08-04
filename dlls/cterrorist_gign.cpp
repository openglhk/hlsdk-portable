#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "cterrorist.h"

class CCounterTerroristGIGN : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristGIGN::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/gign.mdl" );
    }
    CCounterTerrorist::Precache();
}

void CCounterTerroristGIGN::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/gign.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    m_ctType = 1;

    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "FR" );
    }
}

LINK_ENTITY_TO_CLASS( monster_counterterrorist_gign, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_assaultrifle, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_grenader, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_law, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_machinegun, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_mp5, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_pistol, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_shotgun, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_smg, CCounterTerroristGIGN );
LINK_ENTITY_TO_CLASS( monster_ct_gign_sniperrifle, CCounterTerroristGIGN );
