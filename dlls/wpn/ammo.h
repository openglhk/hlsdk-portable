#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"

class C338MagnumAmmo : public CBasePlayerAmmo
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    BOOL AddAmmo(CBaseEntity *pOther) override;
};

class C357SIGAmmo : public CBasePlayerAmmo
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    BOOL AddAmmo(CBaseEntity *pOther) override;
};

class C45ACPAmmo : public CBasePlayerAmmo
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    BOOL AddAmmo(CBaseEntity *pOther) override;
};

class C50AEAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C556NatoAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C556NatoBoxAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C57MMAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C66MMAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C762NatoAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C762NatoBoxAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class C9MMAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class CBuckShotAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class CGenericAmmo : public CBasePlayerAmmo
{
public:
    void Spawn( void );
    void Precache( void );
    BOOL AddAmmo( CBaseEntity *pOther );
};

class CLawsAmmo : public CBasePlayerAmmo
{
public:
    void Spawn(void) override;
    void Precache(void) override;
    BOOL AddAmmo(CBaseEntity *pOther) override;
};
