#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"

class CEnvLiveAmmoShooter : public CBaseDelay
{
public:
    void Spawn( void ) override;
	void Precache( void ) override;
    void KeyValue( KeyValueData *pkvd ) override;
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
    
    void ShootThink( void ); 
    CBaseEntity* CreateGib( void );

    int Save( CSave &save ) override;
    int Restore( CRestore &restore ) override;
    static TYPEDESCRIPTION m_SaveData[];

    CEnvLiveAmmoShooter();

private:
    int   m_iGibModelIndex;   
    float m_flGibVelocity;    
    float m_flVariance;       
    int   m_iGibs;            
    int   m_iGibCapacity;     
    
    float m_flGibLife;        
    int   m_bodyGroup;        
    int   m_quiet;
};