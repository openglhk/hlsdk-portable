include "terrorist.h"

class CTerroristRepel : public CTerrorist
{
public:
    void Precache( void ) override;
    void Spawn( void ) override;
    
public:
    int m_iSpriteTexture;
};

void CTerroristRepel::Precache( void )
{
    UTIL_PrecacheOther( "monster_terrorist" );
    m_iSpriteTexture = PRECACHE_MODEL( "sprites/rope.spr" );
}

#define m_vecRepelEndPos m_vecTossVelocity

void CTerroristRepel::Spawn( void )
{
    CTerrorist::Spawn();
    pev->solid = SOLID_NOT; // 0
    SetUse( &CTerroristRepel::RepelUse ); 
}

void CTerroristRepel::RepelUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
    TraceResult tr;
    Vector vecEnd = pev->origin;
    vecEnd.z -= 4096.0f;
    UTIL_TraceLine( pev->origin, vecEnd, dont_ignore_monsters, edict(), &tr );
    CBaseEntity *pEnt = CBaseEntity::Create( "monster_terrorist", pev->origin, pev->angles, nullptr );
    if ( pEnt == nullptr )
    {
        return;
    }
    CTerrorist *pTerrorist = (CTerrorist *)pEnt->MyMonsterPointer();
    if ( pTerrorist == nullptr )
    {
        return;
    }

    pTerrorist->pev->movetype = MOVETYPE_FLY;
    pTerrorist->pev->velocity.z = RANDOM_FLOAT( -196.0f, -128.0f );
    pTerrorist->pev->velocity.x = 0.0f;
    pTerrorist->pev->velocity.y = 0.0f;
    pTerrorist->ChangeSchedule( pTerrorist->GetScheduleOfType( 51 ) );
    pTerrorist->m_vecRepelEndPos = tr.vecEndPos;
    CBeam *pBeam = CBeam::BeamCreate( "sprites/rope.spr", 10 );
    if ( pBeam != nullptr )
    {
        Vector vecRopeTop = pev->origin;
        vecRopeTop.z += 112.0f;
        pBeam->PointEntInit( vecRopeTop, pTerrorist->entindex() );
        pBeam->pev->rendermode = kRenderTransAdd;
        pBeam->pev->rendercolor = Vector( 255.0f, 255.0f, 255.0f ); 
        float flRopeHeight = tr.flFraction * -4096.0f;
        float flRopeTime   = flRopeHeight / pTerrorist->pev->velocity.z;
        pBeam->SetThink( &CBaseEntity::SUB_Remove );
        pBeam->pev->nextthink = gpGlobals->time + flRopeTime + 0.5f;
    }
    UTIL_Remove( this );
}

LINK_ENTITY_TO_CLASS( monster_terrorist_repel, CTerroristRepel );