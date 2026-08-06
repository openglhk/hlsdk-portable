#include "ammo.h"

LINK_ENTITY_TO_CLASS(ammo_338magnum, C338MagnumAmmo);

void C338MagnumAmmo::Spawn(void)
{
    Precache();

    if (m_numclips == 1)
    {
        SET_MODEL(ENT(pev), "models/w_338magnum.mdl");
    }
    else
    {
        SET_MODEL(ENT(pev), "models/w_338magnum_big.mdl");
    }

    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_BBOX;

    UTIL_SetSize(pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 16.0f));
    UTIL_SetOrigin(pev, pev->origin);

    SetTouch(&CBasePlayerAmmo::DefaultTouch);

    if (g_engfuncs.pfnCVarGetFloat("lightmultiplier") >= 0.1f)
    {
        pev->renderfx = kRenderFxGlowShell;
        pev->iuser4 = (int)g_engfuncs.pfnCVarGetFloat("lightmultiplier");
    }
}

void C338MagnumAmmo::Precache(void)
{
    if (m_numclips == 1)
    {
        PRECACHE_MODEL("models/w_338magnum.mdl");
    }
    else
    {
        PRECACHE_MODEL("models/w_338magnum_big.mdl");
    }

    PRECACHE_SOUND("items/9mmclip1.wav");
}

BOOL C338MagnumAmmo::AddAmmo(CBaseEntity *pOther)
{
    int iResult = pOther->GiveAmmo(m_numclips * 10, "338Magnum", 30);

    if (iResult != -1)
    {
        EMIT_SOUND_DYN(edict(), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_357sig, C357SIGAmmo);

void C357SIGAmmo::Spawn(void)
{
    Precache();

    if (m_numclips == 1)
    {
        SET_MODEL(ENT(pev), "models/w_357sig.mdl");
    }
    else
    {
        SET_MODEL(ENT(pev), "models/w_357sig_big.mdl");
    }

    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_BBOX;

    UTIL_SetSize(pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 16.0f));
    UTIL_SetOrigin(pev, pev->origin);

    SetTouch(&CBasePlayerAmmo::DefaultTouch);

    if (g_engfuncs.pfnCVarGetFloat("lightmultiplier") >= 0.1f)
    {
        pev->renderfx = kRenderFxGlowShell;
        pev->iuser4 = (int)g_engfuncs.pfnCVarGetFloat("lightmultiplier");
    }
}

void C357SIGAmmo::Precache(void)
{
    if (m_numclips == 1)
    {
        PRECACHE_MODEL("models/w_357sig.mdl");
    }
    else
    {
        PRECACHE_MODEL("models/w_357sig_big.mdl");
    }

    PRECACHE_SOUND("items/9mmclip1.wav");
}

BOOL C357SIGAmmo::AddAmmo(CBaseEntity *pOther)
{
    int iResult = pOther->GiveAmmo(13, "357SIG", 52); // 0x0d = 13, 0x34 = 52 max ammo

    if (iResult != -1)
    {
        EMIT_SOUND_DYN(edict(), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_45acp,C45ACPAmmo);

void C45ACPAmmo::Spawn(void)
{
    Precache();

    if (m_numclips == 1)
    {
        SET_MODEL(ENT(pev), "models/w_45acp.mdl");
    }
    else
    {
        SET_MODEL(ENT(pev), "models/w_45acp_big.mdl");
    }

    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_BBOX;

    UTIL_SetSize(pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 16.0f));
    UTIL_SetOrigin(pev, pev->origin);

    SetTouch(&CBasePlayerAmmo::DefaultTouch);

    if (g_engfuncs.pfnCVarGetFloat("lightmultiplier") >= 0.1f)
    {
        pev->renderfx = kRenderFxGlowShell;
        pev->iuser4 = (int)g_engfuncs.pfnCVarGetFloat("lightmultiplier");
    }
}

void C45ACPAmmo::Precache(void)
{
    if (m_numclips == 1)
    {
        PRECACHE_MODEL("models/w_45acp.mdl");
    }
    else
    {
        PRECACHE_MODEL("models/w_45acp_big.mdl");
    }

    PRECACHE_SOUND("items/9mmclip1.wav");
}

BOOL C45ACPAmmo::AddAmmo(CBaseEntity *pOther)
{
    int iResult = pOther->GiveAmmo(m_numclips * 12, "45acp", 100); // 0x0c = 12, 100 max ammo

    if (iResult != -1)
    {
        EMIT_SOUND_DYN(edict(), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_50ae, C50AEAmmo );

void C50AEAmmo::Spawn(void)
{
    Precache();

    if (m_numclips == 1)
    {
        SET_MODEL(ENT(pev), "models/w_50ae.mdl");
    }
    else
    {
        SET_MODEL(ENT(pev), "models/w_50ae_big.mdl");
    }

    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_BBOX;

    UTIL_SetSize(pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 16.0f));
    UTIL_SetOrigin(pev, pev->origin);

    SetTouch(&CBasePlayerAmmo::DefaultTouch);

    if (CVAR_GET_FLOAT("lightmultiplier") >= 0.1f)
    {
        pev->renderfx = kRenderFxGlowShell;
        pev->iuser4 = (int)CVAR_GET_FLOAT("lightmultiplier");
    }
}

void C50AEAmmo::Precache( void )
{
    PRECACHE_MODEL( "models/w_fe_ammo.mdl" );
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C50AEAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // .50AE boxes give (m_numclips * 7) bullets up to a max capacity of 35 (0x23).
    if ( pOther->GiveAmmo( m_numclips * 7, "50AE", 35 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_556nato, C556NatoAmmo);

void C556NatoAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_556nato.mdl" );
        SET_MODEL( ENT(pev), "models/w_556nato.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_556nato_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_556nato_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS;
    pev->solid    = SOLID_TRIGGER;
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    // Checks if map-wide light multiplier adjustments are present
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        // 0x15 (21 decimal) is a specific CZDS RenderFX used for StudioRender scaling
        pev->renderfx = 21; 
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C556NatoAmmo::Precache( void )
{
    // Precache the appropriate world model based on the ammo box size/clip count
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_556nato.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_556nato_big.mdl" );
    }

    // Precache the standard universal item pickup sound
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C556NatoAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 5.56 NATO boxes give (m_numclips * 30) bullets up to a max capacity of 90 (0x5a).
    if ( pOther->GiveAmmo( m_numclips * 30, "556Nato", 90 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_556natobox, C556NatoBoxAmmo);

void C556NatoBoxAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_556natobox.mdl" );
        SET_MODEL( ENT(pev), "models/w_556natobox.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_556natobox_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_556natobox_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C556NatoBoxAmmo::Precache( void )
{
    // Precache the appropriate box model asset based on the clip count configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_556natobox.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_556natobox_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C556NatoBoxAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 5.56 HMG boxes give (m_numclips * 60) bullets up to a max capacity of 200.
    if ( pOther->GiveAmmo( m_numclips * 60, "556NatoBox", 200 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_57mm, C57MMAmmo);
	
void C57MMAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_57mm.mdl" );
        SET_MODEL( ENT(pev), "models/w_57mm.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_57mm_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_57mm_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C57MMAmmo::Precache( void )
{
    // Precache the appropriate 5.7mm model asset based on the clip count configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_57mm.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_57mm_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C57MMAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 5.7mm pickups give a flat 50 rounds up to a max capacity of 100.
    if ( pOther->GiveAmmo( 50, "5.7mm", 100 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_66mm, C66MMAmmo);
	
void C66MMAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_66mm.mdl" );
        SET_MODEL( ENT(pev), "models/w_66mm.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_66mm_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_66mm_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C66MMAmmo::Precache( void )
{
    // Precache the appropriate 66mm rocket model based on the clip count configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_66mm.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_66mm_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C66MMAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 66mm rocket pickups give a flat 1 round up to a max carrying capacity of 1.
    if ( pOther->GiveAmmo( 1, "66MM", 1 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_762nato, C762NatoAmmo);

void C762NatoAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_762nato.mdl" );
        SET_MODEL( ENT(pev), "models/w_762nato.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_762nato_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_762nato_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C762NatoAmmo::Precache( void )
{
    // Precache the appropriate 7.62mm model asset based on the clip count configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_762nato.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_762nato_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C762NatoAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 7.62 NATO boxes give (m_numclips * 30) bullets up to a max capacity of 90 (0x5a).
    if ( pOther->GiveAmmo( m_numclips * 30, "762Nato", 90 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_762natobox, C762NatoBoxAmmo);
	
void C762NatoBoxAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_762natobox.mdl" );
        SET_MODEL( ENT(pev), "models/w_762natobox.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_762natobox_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_762natobox_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C762NatoBoxAmmo::Precache( void )
{
    // Precache the appropriate heavy belt box model asset based on clip configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_762natobox.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_762natobox_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C762NatoBoxAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 7.62 HMG boxes give (m_numclips * 60) bullets up to a max capacity of 200.
    if ( pOther->GiveAmmo( m_numclips * 60, "762NatoBox", 200 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_9mm, C9MMAmmo);

void C9MMAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_9mmclip.mdl" );
        SET_MODEL( ENT(pev), "models/w_9mmclip.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_9mmclip_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_9mmclip_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void C9MMAmmo::Precache( void )
{
    // Precache the appropriate 9mm model asset based on the clip count configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_9mmclip.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_9mmclip_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL C9MMAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // 9mm boxes give (m_numclips * 30) bullets up to a max capacity of 120 (0x78).
    if ( pOther->GiveAmmo( m_numclips * 30, "9mm", 120 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_buckshot, 

void CBuckShotAmmo::Spawn( void )
{
    // Precache assets based on clip quantity 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_shotbox.mdl" );
        SET_MODEL( ENT(pev), "models/w_shotbox.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_shotbox_big.mdl" );
        SET_MODEL( ENT(pev), "models/w_shotbox_big.mdl" );
    }
    
    PRECACHE_SOUND( "items/9mmclip1.wav" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void CBuckShotAmmo::Precache( void )
{
    // Precache the appropriate shotgun shell box model asset based on clip configuration
    if ( m_numclips == 1 )
    {
        PRECACHE_MODEL( "models/w_shotbox.mdl" );
    }
    else
    {
        PRECACHE_MODEL( "models/w_shotbox_big.mdl" );
    }

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL CBuckShotAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Call GiveAmmo(int iCount, char *szName, int iMax) on the player entity.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // Shotgun shell boxes give (m_numclips * 8) rounds up to a max capacity of 32 (0x20).
    if ( pOther->GiveAmmo( m_numclips * 8, "buckshot", 32 ) != -1 )
    {
        // Play the pickup sound directly from the ammo entity's origin
        EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
        return TRUE;
    }

    return FALSE;
}

LINK_ENTITY_TO_CLASS(ammo_generic, CGenericAmmo);
	
void CGenericAmmo::Spawn( void )
{
    // Precache assets 
    // (If the virtual function pointer points to Precache, it executes inline)
    if ( m_pfnPrecache == &CGenericAmmo::Precache )
    {
        PRECACHE_MODEL( "models/w_weaponbox.mdl" );
        PRECACHE_SOUND( "items/9mmclip1.wav" );
    }
    else
    {
        Precache();
    }
    
    SET_MODEL( ENT(pev), "models/w_weaponbox.mdl" );

    // Physics and Collision boundaries
    pev->movetype = MOVETYPE_TOSS; // 6
    pev->solid    = SOLID_TRIGGER; // 1
    
    // Set standard item box size dimensions
    UTIL_SetSize( pev, Vector(-16, -16, 0), Vector(16, 16, 16) );
    UTIL_SetOrigin( pev, pev->origin );

    // Set the collision touch interaction callback
    SetTouch( &CBasePlayerAmmo::DefaultTouch );

    // Condition Zero: Deleted Scenes custom studio rendering light logic
    float flLightMultiplier = CVAR_GET_FLOAT( "lightmultiplier" );
    if ( flLightMultiplier >= 0.1f )
    {
        pev->renderfx = 21; // 0x15
        pev->iuser4   = (int)flLightMultiplier;
    }
}

void CGenericAmmo::Precache( void )
{
    // Precache the generic world weapon box model
    PRECACHE_MODEL( "models/w_weaponbox.mdl" );

    // Precache the universal item pickup audio effect
    PRECACHE_SOUND( "items/9mmclip1.wav" );
}

BOOL CGenericAmmo::AddAmmo( CBaseEntity *pOther )
{
    // Max out every single standard weapon ammunition pool in the game.
    // 0x1b is the vtable index for CBaseEntity::GiveAmmo in the GoldSrc engine.
    // Structure: GiveAmmo( AmmoCount, AmmoName, MaxCapacity )

    pOther->GiveAmmo( 90,  "762Nato",    90 );   // AK47, Scout
    pOther->GiveAmmo( 90,  "556Nato",    90 );   // M4A1, FAMAS, SG552
    pOther->GiveAmmo( 30,  "338Magnum",  30 );   // AWP (.338 Lapua Magnum)
    pOther->GiveAmmo( 35,  "50AE",       35 );   // Desert Eagle
    pOther->GiveAmmo( 120, "9mm",        120 );  // Glock18, MP5, TMP
    pOther->GiveAmmo( 100, "5.7mm",      100 );  // P90, Five-seveN
    pOther->GiveAmmo( 200, "762NatoBox", 200 );  // Heavy M60 Machine Gun
    pOther->GiveAmmo( 200, "556NatoBox", 200 );  // M249 SAW
    pOther->GiveAmmo( 32,  "buckshot",   32 );   // Shotguns (M3, XM1014)
    pOther->GiveAmmo( 100, "45acp",      100 );  // USP, UMP45, MAC-10
    pOther->GiveAmmo( 52,  "357SIG",     52 );   // P228 (0x34 = 52 rounds)

    // Play the pickup audio feedback confirmation
    EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
    return TRUE;
}

LINK_ENTITY_TO_CLASS(laws_rocket, CLawsAmmo);

void CLawsAmmo::Spawn(void)
{
    Precache();
    SET_MODEL(ENT(pev), "models/w_lawsammo.mdl");

    pev->movetype = MOVETYPE_FLY; // 6：單人模式彈藥箱不產生重力下墜
    pev->solid = SOLID_TRIGGER;    // 1：設定為接觸觸發器，玩家踩過去即可撿起

    UTIL_SetSize(pev, Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, 16.0f));
    UTIL_SetOrigin(pev, pev->origin);
	pev->classname = MAKE_STRING("laws_rocket");

    // 綁定 Half-Life SDK 經典的通用彈藥碰觸回呼函數
    SetTouch(&CBasePlayerAmmo::DefaultTouch);
    if (CVAR_GET_FLOAT("lightmultiplier") >= 0.1f)
    {
        pev->renderfx = kRenderFxGlowShell; // 0x15：在陰暗關卡中為火箭彈藥箱加上一層微弱的邊緣發光特效
        pev->iuser4 = (int)CVAR_GET_FLOAT("lightmultiplier"); // 將環境亮度權重暫存入 iuser4 供底層著色器動態縮放發光強度
    }
}

void CLawsAmmo::Precache(void)
{
    PRECACHE_MODEL("models/w_lawsammo.mdl");
    PRECACHE_SOUND("items/9mmclip1.wav");
}

BOOL CLawsAmmo::AddAmmo(CBaseEntity *pOther)
{
    // 💡 虛擬函數表索引 [0x1b] 精確對齊了玩家的 CBasePlayer::GiveAmmo 接口
    // 給予玩家編號為 5 的 66mm 火箭彈藥，數量為 1 發（&DAT_0017ca84）
    int iCount = pOther->GiveAmmo(1, "66mm", 5);

    // 如果 GiveAmmo 返回 -1，代表玩家身上的火箭彈藥庫已經滿載（iMaxAmmo1 = 1），此時不予拾取
    if (iCount != -1)
    {
        EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0f, ATTN_NORM, 0, PITCH_NORM);
        return TRUE;
    }

    return FALSE;
}