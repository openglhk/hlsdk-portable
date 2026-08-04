#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "cterrorist.h"

class CCounterTerroristSAS : public CCounterTerrorist
{
public:
    void Precache( void ) override;
    void SpawnInit( void );
};

void CCounterTerroristSAS::Precache( void )
{
    if ( pev->model != 0 )
    {
        PRECACHE_MODEL( (char *)STRING( pev->model ) );
    }
    else
    {
        PRECACHE_MODEL( "models/sas.mdl" );
    }
    CCounterTerrorist::Precache();
}

void CCounterTerroristSAS::SpawnInit( void )
{
    if ( pev->model == 0 )
    {
        SET_MODEL( edict(), "models/sas.mdl" );
    }
    else
    {
        SET_MODEL( edict(), (char *)STRING( pev->model ) );
    }

    m_ctType = 4;

    if ( m_language == 0 )
    {
        m_language = ALLOC_STRING( "BR" );
    }
}

LINK_ENTITY_TO_CLASS( monster_counterterrorist_sas, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_assaultrifle, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_grenader, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_law, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_machinegun, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_mp5, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_pistol, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_shotgun, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_smg, CCounterTerroristSAS );
LINK_ENTITY_TO_CLASS( monster_ct_sas_sniperrifle, CCounterTerroristSAS );
