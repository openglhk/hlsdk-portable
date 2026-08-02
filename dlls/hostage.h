#ifndef HOSTAGE_H
#define HOSTAGE_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"

class CHostage : public CBaseMonster
{
public:
    CHostage();
    void Spawn( void ) override;
    void Precache( void ) override; 
    void KeyValue( KeyValueData *pkvd ) override;
	void RePosition( void );
	void Restart( void );
	void PreThink( void ) override;
	void Touch( CBaseEntity *pOther ) override;
    int Classify( void ) override { return CLASS_PLAYER_ALLY; }
    void IdleThink( void );
    void FollowUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void ShootPistol( void );
    void DoFollow( void );
    void MoveToward( Vector *vecLoc );
	void NavReady( void );
	void PlayPainSound( void );
	void PointAt( Vector *vecLoc );
	void Remove( void );
	void Wiggle( void );
    void ExecuteSpawnInit( void ); // 對齊 0x48 虛擬回調
	int  BloodColor( void ) override;
	float GetModifiedDamage( float flDamage, int nHitGroup ); 
	Schedule_t* GetSchedule( void ) override;
	BOOL IsOnLadder( void );
	int  ObjectCaps( void ) override;
	int  TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType ) override;
    int  Save( CSave &save ) override;
    int  Restore( CRestore &restore ) override;
	void SetActivity( Activity NewActivity ) override;
	void SetDeathActivity( void ) override;
	void SetFlinchActivity( void ) override;
	void SetYawSpeed( void ) override;
	
private:
    BOOL           m_femaleFlag;         // 標記目前人質是否為女性兵種變體 (女兵/女學生)
    int            m_head;               // 儲存地圖傳入的外觀頭部部件索引
    int            m_State;              // 狀態機：0=STAND, 1=RUN 等人質當前思維狀態
    float          m_flNextChange;       // 狀態隨機改變冷卻計時器
    float          m_flFlinchTime;       // 遭到槍擊受驚硬直受干擾時鐘
    BOOL           m_bRescueMe;          // 標記人質目前是否正處於被成功帶入營救區的過渡期
    BOOL           m_rescued;            // 標記人質是否已被徹底成功營救註銷
    
    CBaseEntity*   m_hStoppedTargetEnt;  // 記錄命令其原地等待時的暫停主權目標指針
    float          m_flNextFullThink;    // 高級大腦全局尋路策略刷新冷卻線
    BOOL           m_bTouched;
    Vector         m_vPathToFollow[4];   // 尋路軌跡多段幾何座標緩衝陣列
    Vector         m_vOldPos;            // 🚀 老位置防卡死緩衝暫存器向量 (開局設定為 9999)
    Vector         m_vStart;             // 出生原始幾何原點座標，用於回合重置
    Vector         m_vStartAngles;       // 出生原始幾何角度向量，用於回合重置

    int            m_iHostageIndex;      // 人質獨立全局計數序號
    int            nTargetNode;          // 當前正在尋路奔跑的地圖節點 ID
    BOOL           m_fHasPath;           // 標記大腦目前是否成功獲取了導航軌跡
    float          m_flLastPathCheck;    // 上一次發動路徑完整性審查的時間戳
    float          m_flPathAcquired;     // 成功斬獲合法路徑的絕對時間戳
    float          m_flPathCheckInterval;// 尋路時鐘步進冷卻間隔 (0.1秒)
    float          m_flNextRadarTime;    // 隨機交錯的下一次雷達通訊無線電冷卻線
    float          m_voicePitch;         // 語音音調基數面 (預設 100.0)
    CLocalNav*     m_LocalNav;           // 👈 完美宣告，100% 圓滿閉環！
    int            m_nPathNodes;         // 快取陣列中當前合法的總節點數量
    Vector         vecNodes[16];         // 👈 完美歸位！100% 對齊 16 容量的三維空間節點幾何矩陣 (佔用 16 * 12 = 192 位元組)
	float          m_healthModifier;     // 👈 補上這行：記錄是否中彈或中鴆瀕死的重傷狀態標記 (0=健康)
    string_t       m_idleAnim;           // 👈 補上這行：人質跟隨隨行時被強制重定向的 MDL 動畫待命序列字串
	string_t       m_rescueTarget;
	string_t       m_rescueAnim;
	CCineMonster*  m_pCine;
    static TYPEDESCRIPTION m_SaveData[]; 
};

extern int g_iHostageNumber; // 全局人質序號計數器外置符號宣告

#endif // HOSTAGE_H
