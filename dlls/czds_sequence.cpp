#include "czds_sequence.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

std::map<std::string, sequenceEntry_s*> CZDS_SequenceManager::m_SequenceMap;

static std::string CleanString(std::string str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), str.end());
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        str = str.substr(1, str.size() - 2);
    }
    return str;
}

void CZDS_SequenceManager::LoadAllSequences() 
{
    for (auto& pair : m_SequenceMap) {
        sequenceEntry_s* entry = pair.second;
        if (entry) {
            sequenceCommandLine_s* cmd = entry->firstCommand;
            while (cmd) {
                sequenceCommandLine_s* next = cmd->nextCommandLine;
                if (cmd->fireTargetNames) free(cmd->fireTargetNames);
                if (cmd->killTargetNames) free(cmd->killTargetNames);
                if (cmd->soundFileName) free(cmd->soundFileName);
                if (cmd->speakerName) free(cmd->speakerName);
                if (cmd->listenerName) free(cmd->listenerName);
                if (cmd->clientMessage.pMessage) free(cmd->clientMessage.pMessage);
                delete cmd;
                cmd = next;
            }
            delete entry;
        }
    }
    m_SequenceMap.clear();

    if (gpGlobals->mapname == 0) return;
    std::string mapName = STRING(gpGlobals->mapname);

    std::string filepath = "czero/sequences/" + mapName + ".seq";
    std::ifstream file(filepath);
	
    if (!file.is_open()) {
        ALERT(at_console, "CZDS_Parser: 找不到該地圖的劇本檔案 (%s 或 %s.seq)！\n", mapName.c_str(), mapName.c_str());
        return;
    }

    std::string line;
    sequenceEntry_s* pCurrentEntry = nullptr;
    sequenceCommandLine_s* pLastCmd = nullptr;
    std::string currentSpeaker = "none";

    ALERT(at_console, "CZDS_Parser: 偵測到地圖切換，正在動態加載劇本: %s...\n", filepath.c_str());

    while (std::getline(file, line)) 
    {
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        line = CleanString(line);
        if (line.empty() || line == "{" || line == "}") continue;

        if (line.find("!sentences") == 0) {
            while (std::getline(file, line) && line.find("}") == std::string::npos);
            continue;
        }

        if (line.front() == '%') 
        {
            std::string entryName = CleanString(line.substr(1));
            pCurrentEntry = new sequenceEntry_s();
            pCurrentEntry->firstCommand = nullptr;
            m_SequenceMap[entryName] = pCurrentEntry;
            pLastCmd = nullptr;
            currentSpeaker = "none";
            continue;
        }

        if (pCurrentEntry != nullptr) 
        {
            if (line.front() == '@') {
                currentSpeaker = CleanString(line.substr(1));
                continue;
            }

            if (line.front() == '#') 
            {
                size_t eqPos = line.find('=');
                if (eqPos == std::string::npos) continue;

                std::string key = CleanString(line.substr(1, eqPos - 1));
                std::string val = CleanString(line.substr(eqPos + 1));

                sequenceCommandLine_s* pCmd = new sequenceCommandLine_s();
                memset(pCmd, 0, sizeof(sequenceCommandLine_s));
                pCmd->speakerName = strdup(currentSpeaker.c_str());
                pCmd->listenerName = strdup("player");

                if (key == "pause") {
                    pCmd->delay = std::stof(val);
                } 
                else if (key == "firetargets") {
                    pCmd->fireTargetNames = strdup(val.c_str());
                } 
                else if (key == "sound") {
                    std::string soundPath = "!" + val; 
                    pCmd->soundFileName = strdup(soundPath.c_str());
                }
                else if (key == "text") {
                    pCmd->clientMessage.pMessage = strdup(val.c_str());
                    pCmd->clientMessage.holdtime = -1.0f;
                }

                if (pCurrentEntry->firstCommand == nullptr) {
                    pCurrentEntry->firstCommand = pCmd;
                } else if (pLastCmd != nullptr) {
                    pLastCmd->nextCommandLine = pCmd;
                }
                pLastCmd = pCmd;
            }
        }
    }
    file.close();
    ALERT(at_console, "CZDS_Parser: 地圖 %s 的劇本加載成功！\n", mapName.c_str());
}

sequenceEntry_s* CZDS_SequenceManager::GetSequence(const char* name) 
{
    if (!name) return nullptr;
    if (m_SequenceMap.find(name) != m_SequenceMap.end()) {
        return m_SequenceMap[name];
    }
    return nullptr;
}

extern "C" void CZDS_LoadSequences_Hook() {
    CZDS_SequenceManager::LoadAllSequences();
}
