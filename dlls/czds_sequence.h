#pragma once
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include <string>
#include <map>

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

class CZDS_SequenceManager
{
private:
    static std::map<std::string, sequenceEntry_s*> m_SequenceMap;

public:
    static void LoadAllSequences();
    static sequenceEntry_s* GetSequence(const char* name);
};
