//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef CLIENTTIMESDISPLAY_H
#define CLIENTTIMESDISPLAY_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "steam/steam_api.h"

#include "GameEventListener.h"

#include "momentum/mom_shareddefs.h"
#include <KeyValues.h>
#include <game/client/iviewport.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/SectionedListPanel.h>
#include "ReplayContextMenu.h"

#define TYPE_NOTEAM 0 // NOTEAM must be zero :)
#define TYPE_TEAM 1   // a section for a single team
#define TYPE_PLAYERS 2
#define TYPE_SPECTATORS 3 // a section for a spectator group
#define TYPE_BLANK 4

#define SCALE(num) scheme()->GetProportionalScaledValueEx(GetScheme(), (num))

#define DELAY_NEXT_UPDATE 10.0f // Delay for the next API update, in seconds
#define MIN_ONLINE_UPDATE_INTERVAL 15.0f //The amount of seconds minimum between online checks
#define MAX_ONLINE_UPDATE_INTERVAL 45.0f //The amount of seconds maximum between online checks
//-----------------------------------------------------------------------------
// Purpose: Game ScoreBoard
//-----------------------------------------------------------------------------
class CClientTimesDisplay : public vgui::EditablePanel, public IViewPortPanel, public CGameEventListener
{
  private:
    DECLARE_CLASS_SIMPLE(CClientTimesDisplay, vgui::EditablePanel);

  protected:
    // column widths at 640
    enum
    {
        NAME_WIDTH = 160,
        SCORE_WIDTH = 60,
        DEATH_WIDTH = 60,
        PING_WIDTH = 80,
        VOICE_WIDTH = 0,
        FRIENDS_WIDTH = 0
    };
    // total = 340

  public:
    CClientTimesDisplay(IViewPort *pViewPort);
    ~CClientTimesDisplay();

    const char *GetName(void) override { return PANEL_TIMES; }

    void SetData(KeyValues *data) override{};

    void Reset() override;
    void Update() override;
    void Update(bool pFullUpdate);
    void Reset(bool pFullReset);
    bool NeedsUpdate(void) override;

    bool HasInputElements(void) override { return true; }

    void ShowPanel(bool bShow) override;

    virtual bool ShowAvatars() { return IsPC(); }

    // both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
    vgui::VPANEL GetVPanel(void) override { return BaseClass::GetVPanel(); }

    bool IsVisible() override { return BaseClass::IsVisible(); }

    void SetParent(vgui::VPANEL parent) override { BaseClass::SetParent(parent); }

    // IGameEventListener interface:
    void FireGameEvent(IGameEvent *event) override;

    virtual void UpdatePlayerAvatar(int playerIndex, KeyValues *kv);

    void UpdateLeaderboardPlayerAvatar(uint64, KeyValues *kv);

    CReplayContextMenu *GetLeaderboardReplayContextMenu(vgui::Panel *pParent);

  protected:
    MESSAGE_FUNC_INT(OnPollHideCode, "PollHideCode", code);
    MESSAGE_FUNC_PARAMS(OnItemContextMenu, "ItemContextMenu", data);//Catching from SectionedListPanel
    MESSAGE_FUNC_CHARPTR(OnContextWatchReplay, "ContextWatchReplay", runName);
    MESSAGE_FUNC_UINT64(OnContextVisitProfile, "ContextVisitProfile", profile);

    STEAM_CALLBACK(CClientTimesDisplay, OnPersonaStateChange, PersonaStateChange_t);

    int TryAddAvatar(CSteamID);

    // functions to override
    virtual bool GetPlayerTimes(KeyValues *outPlayerInfo, bool fullUpdate);
    virtual void InitScoreboardSections();
    virtual void UpdateTeamInfo();
    virtual void UpdatePlayerInfo(KeyValues *outPlayerInfo, bool fullUpdate);
    void OnThink() override;
    virtual void AddHeader(vgui::Label *pMapSummary);      // add the start header of the scoreboard
    virtual int GetAdditionalHeight() { return 0; }

    // sorts players within a section
    static bool StaticLocalTimeSortFunc(vgui::SectionedListPanel *list, int itemID1, int itemID2);
    static bool StaticOnlineTimeSortFunc(vgui::SectionedListPanel *list, int itemID1, int itemID2);

    void ApplySchemeSettings(vgui::IScheme *pScheme) override;

    virtual void PostApplySchemeSettings(vgui::IScheme *pScheme);

    // finds the player in the scoreboard
    int FindItemIDForPlayerIndex(int playerIndex);
    // finds a local time in the scoreboard
    int FindItemIDForLocalTime(KeyValues *kvRef);
    // finds an online time in the scoreboard
    int FindItemIDForOnlineTime(int runID);

    int m_iNumTeams;

    vgui::SectionedListPanel *m_pPlayerList;
    int m_iSectionId; // the current section we are entering into

    float m_fNextUpdateTime;

    void MoveLabelToFront(const char *textEntryName);
    void MoveToCenterOfScreen();

    vgui::ImageList *m_pImageList;
    vgui::Panel *m_pHeader;
    vgui::Panel *m_pPlayerStats;
    vgui::Panel *m_pLeaderboards;
    vgui::Label *m_lMapSummary;
    vgui::Label *m_lPlayerName;
    vgui::Label *m_lPlayerMapRank;
    vgui::Label *m_lPlayerPersonalBest;
    vgui::Label *m_lPlayerGlobalRank;
    vgui::Label *m_lPlayerExperience;
    vgui::Label *m_lLoadingOnlineTimes;
    vgui::SectionedListPanel *m_pOnlineLeaderboards;
    vgui::SectionedListPanel *m_pLocalLeaderboards;
    vgui::SectionedListPanel *m_pFriendsLeaderboards;
    vgui::ImagePanel *m_pPlayerAvatar;
    vgui::ImagePanel *m_pMomentumLogo;

    CUtlMap<CSteamID, int> m_mapAvatarsToImageList;

    CPanelAnimationVar(int, m_iAvatarWidth, "avatar_width", "34"); // Avatar width doesn't scale with resolution
    CPanelAnimationVarAliasType(int, m_iNameWidth, "name_width", "136", "proportional_int");
    CPanelAnimationVarAliasType(int, m_iClassWidth, "class_width", "35", "proportional_int");
    CPanelAnimationVarAliasType(int, m_iScoreWidth, "score_width", "35", "proportional_int");
    CPanelAnimationVarAliasType(int, m_iDeathWidth, "death_width", "35", "proportional_int");
    CPanelAnimationVarAliasType(int, m_iPingWidth, "ping_width", "23", "proportional_int");

    // Online API Pre-Alpha functions
    void GetOnlineTimesCallback(HTTPRequestCompleted_t *pCallback, bool bIOFailure);
    CCallResult<CClientTimesDisplay, HTTPRequestCompleted_t> cbGetOnlineTimesCallback;
    void GetGetPlayerDataForMapCallback(HTTPRequestCompleted_t *pCallback, bool bIOFailure);
    CCallResult<CClientTimesDisplay, HTTPRequestCompleted_t> cbGetGetPlayerDataForMapCallback;

    void CreateAndSendHTTPReq(const char*, CCallResult<CClientTimesDisplay, HTTPRequestCompleted_t>*,
        CCallResult<CClientTimesDisplay, HTTPRequestCompleted_t>::func_t);

private:
    int			m_iPlayerIndexSymbol;
    int			m_iDesiredHeight;

    float       m_fLastHeaderUpdate;
    bool        m_bFirstHeaderUpdate;

    float m_flLastOnlineTimeUpdate;
    bool m_bFirstOnlineTimesUpdate;

    IViewPort	*m_pViewPort;
    ButtonCode_t m_nCloseKey;
    struct Time
    {
        float time_sec, rate;
        time_t date;

        explicit Time(KeyValues *kv)
        {
            time_sec = Q_atof(kv->GetName());
            rate = kv->GetFloat("rate", gpGlobals->interval_per_tick);
            date = static_cast<time_t>(kv->GetInt("date", 0));
        };
    };
    void ConvertOnlineTimes(KeyValues *kv, float seconds);
    struct TimeOnline
    {
        int rank, id, avatar;
        float time_sec, rate;
        uint64 steamid;
        time_t date;
        const char* personaname;

        // entry
        // -rank
        // -avatar
        // -personaname
        // -steamid
        KeyValues *m_kv;
        
        explicit TimeOnline(KeyValues* kv)
        {
            m_kv = kv;
            id = kv->GetInt("id", -1);
            rank = kv->GetInt("rank", 0);
            time_sec = kv->GetFloat("time", -1);
            personaname = kv->GetString("personaname", "Unknown");
            rate = kv->GetFloat("rate", 100);
            date = static_cast<time_t>(Q_atoi(kv->GetString("date", "0")));
            steamid = kv->GetUint64("steamid", 0);
            avatar = kv->GetInt("avatar", 0);
        };

        ~TimeOnline() 
        {
            if (m_kv)
                m_kv->deleteThis();
            m_kv = nullptr;
            personaname = nullptr;
        }
    };

    CUtlVector<Time> m_vLocalTimes;
    
    CUtlVector<TimeOnline*> m_vOnlineTimes;

    bool m_bLocalTimesLoaded = false;
    bool m_bLocalTimesNeedUpdate = false;
    bool m_bOnlineNeedUpdate = false;
    bool m_bOnlineTimesLoaded = false;
    //widths[0] == WIDTH FOR DATE
    //widths[1] == WIDTH FOR RANK
    //widths[2] == WIDTH FOR TIME
    int m_aiColumnWidths[3];

    // methods
    void FillScoreBoard();
    void FillScoreBoard(bool pFullUpdate);
    void LoadLocalTimes(KeyValues *kv);
    void LoadOnlineTimes();
    void ConvertLocalTimes(KeyValues*);
    //Place m_vOnlineTimes into m_pOnlineLeaderboards
    void OnlineTimesVectorToLeaderboards();


    CReplayContextMenu *m_pLeaderboardReplayCMenu;

    CUtlMap<uint64, const char*> m_umMapNames;
};
#endif // CLIENTSCOREBOARDDIALOG_H