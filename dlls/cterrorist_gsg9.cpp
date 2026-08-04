#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "cterrorist.h"

class CCounterTerroristGSG9 : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristGSG9::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/gsg9.mdl" );
    }
    CCounterTerrorist::Precache();
}

void CCounterTerroristGSG9::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/gsg9.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    m_ctType = 2;

    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "GE" );
    }
}

LINK_ENTITY_TO_CLASS( monster_counterterrorist_gsg9, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_assaultrifle, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_grenader, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_law, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_machinegun, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_mp5, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_pistol, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_shotgun, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_smg, CCounterTerroristGSG9 );
LINK_ENTITY_TO_CLASS( monster_ct_gsg9_sniperrifle, CCounterTerroristGSG9 );
