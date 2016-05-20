#pragma once

#include "cbase.h"

//This class handles networking all of the overlap between players and 
//replay files. From an OOP standpoint, this is more efficient than having
//two classes network the same variables.

#ifdef CLIENT_DLL 
#define CMOMRunEntityData C_MOMRunEntityData
EXTERN_RECV_TABLE(DT_MOM_RunEntData);
#endif

class CMOMRunEntityData
{
    DECLARE_CLASS_NOBASE(CMOMRunEntityData);
    DECLARE_EMBEDDED_NETWORKVAR();
public:

    CMOMRunEntityData();

#ifdef GAME_DLL
    //DECLARE_SERVERCLASS();
    

    CNetworkVar(bool, m_bAutoBhop);// Is the player using auto bhop?
    CNetworkVar(int, m_iSuccessiveBhops); //How many successive bhops this player has
    CNetworkVar(float, m_flStrafeSync); //eyeangle based, perfect strafes / total strafes
    CNetworkVar(float, m_flStrafeSync2); //acceleration based, strafes speed gained / total strafes
    CNetworkVar(float, m_flLastJumpVel); //Last jump velocity of the player
    CNetworkVar(int, m_iRunFlags);//The run flags (W only/HSW/Scroll etc) of the player
    CNetworkVar(bool, m_bIsInZone);//This is true if the player is in a CTriggerTimerStage zone
    CNetworkVar(bool, m_bMapFinished);//Did the player finish the map?
    CNetworkVar(int, m_iCurrentZone);//Current stage/checkpoint the player is on


#elif defined CLIENT_DLL
    //DECLARE_CLIENTCLASS();

    bool m_bAutoBhop, m_bIsInZone, m_bMapFinished;
    float m_flStrafeSync, m_flStrafeSync2, m_flLastJumpVel;
    int m_iSuccessiveBhops, m_iRunFlags, m_iCurrentZone;
#endif

};

#ifdef GAME_DLL
EXTERN_SEND_TABLE(DT_MOM_RunEntData);
#endif