#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include <string>
#include <map>

// =========================================================================
// 1. 根據 Ghidra 偵錯符號精準還原的結構體與自訂類別宣告
// =========================================================================

struct clientMessage_s {
	float x;
	float y;
	float holdtime;
	float fadein;
	float fadeout;
	float fxtime;
	int effect;
	byte r1, g1, b1, a1;
	byte r2, g2, b2, a2;
	char* pMessage;
};

struct sequenceCommandLine_s {
	float                  delay;
	char*                  fireTargetNames;
	char*                  killTargetNames;
	clientMessage_s        clientMessage;
	int                    textChannel;
	char*                  soundFileName;
	char*                  speakerName;
	char*                  listenerName;
	int                    repeatCount;
	sequenceCommandLine_s* nextCommandLine;
};

struct sequenceEntry_s {
	sequenceCommandLine_s* firstCommand;
};

// 偽造的全域序列管理器，用來在本地讀取 sequences/*.seq，繞過不存在的引擎成員 pfnSequenceGet
class CZDS_SequenceManager
{
	private:
	static std::map<std::string, sequenceEntry_s*> m_SequenceMap;
	public:
	static void LoadAllSequences() {
		// TODO: 可以在此加入實作 std::ifstream 讀取文字檔邏輯
	}
	static sequenceEntry_s* GetSequence(const char* name) {
		if (!name) return nullptr;
		if (m_SequenceMap.find(name) != m_SequenceMap.end()) return m_SequenceMap[name];
		return nullptr;
	}
};
std::map<std::string, sequenceEntry_s*> CZDS_SequenceManager::m_SequenceMap;


class CTriggerSequence : public CBaseDelay
{
	public:
	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override;
	void Think( void ) override;
	void Precache( void ) override { return; }
	void Restart( void );
	void StopSequence( void );
	void GoToFirstCommand( void );
	void FastForward( void );
	
	void ExecuteSequenceCommand( sequenceCommandLine_s *command );
	void ExecuteFireTargets( char *fireTargetNames );
	void ExecuteKillTargets( char *killTargetNames );
	void ExecuteTextMessage( clientMessage_s *message, int textChannel );
	void ExecuteSoundOrSpeech( char *soundName, char *speakerName, char *listenerName, float duration );
	
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;
	static TYPEDESCRIPTION m_SaveData[];
	
	CTriggerSequence();
	
	private:
	sequenceEntry_s*       m_sequenceEntry;
	sequenceCommandLine_s* m_nextCommand;
	CBaseEntity*           m_activator;
	CBaseEntity*           m_caller;
	USE_TYPE               m_useType;
	int                    m_isBusy;
	
	char                   m_sequenceEntryName[256]; 
	float                  m_sequenceStartTime;
	float                  m_value;
	int                    m_isDisabled;
	int                    m_fastForward;
};

LINK_ENTITY_TO_CLASS( trigger_sequence, CTriggerSequence );

// =========================================================================
// 2. 成員方法實作（100% 對照原廠二進位邏輯還原）
// =========================================================================

TYPEDESCRIPTION CTriggerSequence::m_SaveData[] =
{
	DEFINE_ARRAY( CTriggerSequence, m_sequenceEntryName, FIELD_CHARACTER, 256 ),
	DEFINE_FIELD( CTriggerSequence, m_sequenceStartTime, FIELD_TIME ),
	DEFINE_FIELD( CTriggerSequence, m_value, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerSequence, m_isDisabled, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerSequence, m_fastForward, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerSequence, m_isBusy, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerSequence, m_useType, FIELD_INTEGER ),
};

CTriggerSequence::CTriggerSequence()
{
	m_sequenceEntry = nullptr;
	m_nextCommand = nullptr;
	m_activator = nullptr;
	m_caller = nullptr;
	m_useType = USE_OFF;
	m_isBusy = 0;
	deathfadedelay = 2.0f;
	fadespeed = 7;
	shotPlayerRecently = 0;
	shotPlayerEver = 0;
	shotConeTime = 0.0f;
	leapConeFromFull = 0;
	m_classtype = 0;
	memset(m_sequenceEntryName, 0, sizeof(m_sequenceEntryName));
	m_sequenceStartTime = 0.0f;
	m_value = 0.0f;
	m_isDisabled = 0;
	m_fastForward = 0;
	if ( pev ) pev->nextthink = 0.0f;
}

void CTriggerSequence::KeyValue( KeyValueData *pkvd )
{
	if ( strcasecmp(pkvd->szKeyName, "sequence_id") == 0 ) {
		size_t len = strlen(pkvd->szValue);
		if ( len > 255 ) {
			ALERT(at_error, "Sequence entry label '%s' too long\n", pkvd->szValue);
		}
		strncpy(m_sequenceEntryName, pkvd->szValue, sizeof(m_sequenceEntryName) - 1);
		pkvd->fHandled = TRUE;
		return;
	}
	CBaseDelay::KeyValue( pkvd );
}

void CTriggerSequence::Spawn( void )
{
	this->m_sequenceEntry = CZDS_SequenceManager::GetSequence(this->m_sequenceEntryName);
}

void CTriggerSequence::Restart( void )
{
	this->m_nextCommand = nullptr;
	this->m_sequenceEntry = CZDS_SequenceManager::GetSequence(this->m_sequenceEntryName);
	this->m_fastForward = this->m_isBusy;
}

void CTriggerSequence::GoToFirstCommand( void )
{
	this->m_sequenceStartTime = gpGlobals->time;
	if (this->m_sequenceEntry) this->m_nextCommand = this->m_sequenceEntry->firstCommand;
}

void CTriggerSequence::StopSequence( void )
{
	this->m_nextCommand = nullptr;
	this->m_sequenceStartTime = 0.0f;
	this->m_activator = nullptr;
	this->m_caller = nullptr;
	this->m_useType = USE_OFF;
	this->m_value = 0.0f;
	this->m_isBusy = 0;
	if ( pev ) pev->nextthink = 0.0f;
}

void CTriggerSequence::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (m_sequenceEntry == nullptr) {
		StopSequence();
		UTIL_LogPrintf("CTriggerSequence: unknown sequence \"%s\"\n", m_sequenceEntryName);
		return;
	}
	if (m_isDisabled == 0) {
		if ((pev->spawnflags & 1) != 0) m_isDisabled = 1;
		m_sequenceStartTime = 0.0f;
		m_nextCommand = nullptr;
		m_activator = nullptr;
		m_caller = nullptr;
		m_useType = USE_OFF;
		m_value = 0.0f;
		m_isBusy = 0;
		if (pev) pev->nextthink = 0.0f;
		
		if (m_sequenceEntry->firstCommand != nullptr) {
			m_value = value;
			m_activator = pActivator;
			m_caller = pCaller;
			m_useType = useType;
			m_sequenceStartTime = gpGlobals->time;
			m_nextCommand = m_sequenceEntry->firstCommand;
			m_isBusy = 1;
			Think(); 
			return;
		}
		UTIL_LogPrintf("CTriggerSequence: sequence \"%s\" was empty!\n", m_sequenceEntryName);
	}
}

void CTriggerSequence::FastForward( void )
{
	float timeNow = gpGlobals->time;
	float timeSim = this->m_sequenceStartTime;
	if (!m_sequenceEntry || !m_sequenceEntry->firstCommand) return;
	sequenceCommandLine_s* pCmd = m_sequenceEntry->firstCommand;
	this->m_nextCommand = pCmd;
	if (timeSim + 0.0001f < timeNow) {
		while (true) {
			timeSim += pCmd->delay;
			pCmd = pCmd->nextCommandLine;
			this->m_nextCommand = pCmd;
			if (timeNow <= timeSim + 0.0001f) break;
			if (pCmd == nullptr) return;
		}
	}
}

void CTriggerSequence::ExecuteFireTargets( char *fireTargetNames )
{
	if ( fireTargetNames != nullptr ) FireTargets( fireTargetNames, m_activator, this, m_useType, m_value );
}

void CTriggerSequence::ExecuteKillTargets( char *killTargetNames )
{
	if ( killTargetNames == nullptr || *killTargetNames == '\0' ) return;
	edict_t *pKillEdict = g_engfuncs.pfnFindEntityByString( nullptr, "targetname", killTargetNames );
	while ( pKillEdict != nullptr && !FNullEnt( pKillEdict ) ) {
		if ( pKillEdict->pvPrivateData != nullptr ) {
			CBaseEntity *pKillEnt = (CBaseEntity *)pKillEdict->pvPrivateData;
			pKillEnt->UpdateOnRemove();
			UTIL_Remove( pKillEnt );
		}
		pKillEdict = g_engfuncs.pfnFindEntityByString( pKillEdict, "targetname", killTargetNames );
	}
}

void CTriggerSequence::ExecuteTextMessage( clientMessage_s *message, int textChannel )
{
	if (message != nullptr && message->pMessage != nullptr) {
		hudtextparms_t hudParms;
		hudParms.effect = message->effect;
		hudParms.holdTime = message->holdtime;
		hudParms.x = message->x;
		hudParms.y = message->y;
		hudParms.r1 = message->r1; hudParms.g1 = message->g1; hudParms.b1 = message->b1; hudParms.a1 = message->a1;
		hudParms.r2 = message->r2; hudParms.g2 = message->g2; hudParms.b2 = message->b2; hudParms.a2 = message->a2;
		hudParms.fadeinTime = message->fadein;
		hudParms.fadeoutTime = message->fadeout;
		hudParms.fxTime = message->fxtime;
		hudParms.channel = textChannel;
		if (hudParms.holdTime == -1.0f) {
			hudParms.holdTime = (float)strlen(message->pMessage) * 0.05f + 1.0f;
		}
		UTIL_HudMessageAll(&hudParms, message->pMessage);
	}
}

void CTriggerSequence::ExecuteSoundOrSpeech(char *soundName, char *speakerName, char *listenerName, float duration)
{
	if (soundName == nullptr) return;
	if (speakerName != nullptr && strcasecmp(speakerName, "none") != 0) {
		CBaseEntity *pSpeaker = UTIL_FindEntityByTargetname(nullptr, speakerName);
		if (pSpeaker != nullptr) {
			CBaseMonster *pMonster = pSpeaker->MyMonsterPointer();
			if (pMonster != nullptr && pev != nullptr) {
				CBaseEntity *pListener = UTIL_FindEntityGeneric(listenerName, &pev->origin, 4096.0f);
				// ?? 提示：如果 SDK 的 CBaseMonster 中沒有 PlayScriptSpeech，可改用 EMIT_SOUND_DYN 替代：
				// EMIT_SOUND_DYN(pSpeaker->edict(), CHAN_VOICE, soundName, 1.0, ATTN_NORM, 0, PITCH_NORM);
				pMonster->PlayScriptSpeech(soundName, duration, 1.0f, 0.8f, 1, pListener);
				return;
			}
		}
		return;
	}
	if (pev != nullptr) SENTENCEG_PlayRndSz(pev->pContainingEntity, soundName, 1.0f, 0.0f, 0, 100);
}
void CTriggerSequence::ExecuteSequenceCommand( sequenceCommandLine_s *command )
{
	if ( command == nullptr ) return;
	ExecuteFireTargets( command->fireTargetNames );
	ExecuteKillTargets( command->killTargetNames );
	if ( command->clientMessage.pMessage != nullptr ) ExecuteTextMessage( &(command->clientMessage), command->textChannel );
	if ( command->soundFileName != nullptr ) ExecuteSoundOrSpeech( command->soundFileName, command->speakerName, command->listenerName, command->delay );
	// 進度步進
	if ( command->repeatCount == 0 ) {
		m_nextCommand = m_nextCommand->nextCommandLine;
		} else {
		GoToFirstCommand();
	}
}
void CTriggerSequence::Think( void )
{
	float timeNow = gpGlobals->time;
	if ( m_fastForward != 0 ) {
		FastForward();
		m_fastForward = 0;
	}
	if ( m_nextCommand == nullptr ) {
		StopSequence();
		return;
	}
	float timeNext = m_sequenceStartTime;
	do {
		sequenceCommandLine_s* pCmd = m_nextCommand;
		timeNext += pCmd->delay;
		ExecuteSequenceCommand( pCmd );
	} while ( m_nextCommand != nullptr && timeNext <= timeNow );
	if ( m_nextCommand == nullptr ) StopSequence();
	else if ( pev != nullptr ) pev->nextthink = timeNext;
}
int CTriggerSequence::Save( CSave &save ) {
	if ( !CBaseDelay::Save( save ) ) return 0;
	return save.WriteFields( "CTriggerSequence", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData[0]) );
}
int CTriggerSequence::Restore( CRestore &restore ) {
	int status = CBaseDelay::Restore( restore );
	if ( status == 0 ) return 0;
	return restore.ReadFields( "CTriggerSequence", this, m_SaveData, sizeof(m_SaveData) / sizeof(m_SaveData[0]) );
}