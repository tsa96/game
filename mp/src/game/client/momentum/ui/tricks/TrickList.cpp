#include "cbase.h"

#include "TrickList.h"

#include "momentum/mom_system_tricks.h"

#include "fmtstr.h"

#include "leaderboards/LeaderboardsContextMenu.h"

#include "vgui/ISurface.h"

#include "vgui_controls/ListPanel.h"
#include "vgui_controls/Button.h"

#include "tier0/memdbgon.h"

using namespace vgui;

TrickList::TrickList(IViewPort *pViewPort) : BaseClass(nullptr, PANEL_TRICK_LIST)
{
    SetSize(5, 5);

    SetProportional(true);
    // initialize dialog
    SetProportional(true);
    SetKeyBoardInputEnabled(false);
    SetMouseInputEnabled(false);
    // Create a "popup" so we can get the mouse to detach
    surface()->CreatePopup(GetVPanel(), false, false, false, false, false);

    const auto hScheme = scheme()->LoadSchemeFromFile("resource/TrickListScheme.res", "TrickListScheme");
    if (hScheme)
    {
        SetScheme(hScheme);
    }

    // Construct panels here
    m_pTrickListButton = new Button(this, "TrickListButton", "#MOM_Tricks_TrickList", this, "ShowTrickList");
    m_pWIPTrickListButton = new Button(this, "WIPTrickListButton", "#MOM_Tricks_WIPTrickList", this, "ShowWIPTrickList");
    m_pMapTeleListButton = new Button(this, "MapTeleListButton", "#MOM_Tricks_MapTeleList", this, "ShowMapTeleList");

    m_pTrickList = new ListPanel(this, "TrickList");
    m_pTrickList->SetRowHeightOnFontChange(false);
    m_pTrickList->SetRowHeight(GetScaledVal(20));
    m_pTrickList->SetMultiselectEnabled(false);
    m_pTrickList->SetAutoTallHeaderToFont(true);

    m_pContextMenu = new CLeaderboardsContextMenu(m_pTrickList);
    m_pContextMenu->SetAutoDelete(false);
    m_pContextMenu->AddActionSignalTarget(this);
    m_pContextMenu->SetVisible(false);

    m_pWIPTrickList = new ListPanel(this, "WIPTrickList");
    m_pWIPTrickList->SetRowHeightOnFontChange(false);
    m_pWIPTrickList->SetRowHeight(GetScaledVal(20));
    m_pWIPTrickList->SetMultiselectEnabled(false);
    m_pWIPTrickList->SetAutoTallHeaderToFont(true);

    m_pMapTeleList = new ListPanel(this, "MapTeleList");
    m_pMapTeleList->SetRowHeightOnFontChange(false);
    m_pMapTeleList->SetRowHeight(GetScaledVal(20));
    m_pMapTeleList->SetMultiselectEnabled(false);
    m_pMapTeleList->SetAutoTallHeaderToFont(true);

    LoadControlSettings("resource/ui/tricks/TrickList.res");

    InitTrickListHeaders();
    InitWIPTrickListHeaders();
    InitMapTeleListHeaders();
    m_pTrickList->ResetScrollBar();
    m_pWIPTrickList->ResetScrollBar();
    m_pMapTeleList->ResetScrollBar();

    m_pTrickList->SetVisible(true);
    m_pTrickListButton->SetSelected(true);
    m_pMapTeleList->SetVisible(false);
    m_pWIPTrickList->SetVisible(false);

    ListenForGameEvent("trick_data_loaded");
}

TrickList::~TrickList()
{
    if (m_pContextMenu)
        m_pContextMenu->DeletePanel();
}

void TrickList::Reset()
{
}

void TrickList::Update()
{
}

bool TrickList::NeedsUpdate()
{
    return false;
}

void TrickList::ShowPanel(bool bState)
{
    SetVisible(bState);

    if (m_pContextMenu && m_pContextMenu->IsVisible())
    {
        // Close the menu
        m_pContextMenu->OnKillFocus();
    }

    if (!bState)
    {
        SetMouseInputEnabled(false);
    }

    C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
    if (player)
    {
        if (bState)
        {
            player->m_Local.m_iHideHUD |= HIDEHUD_LEADERBOARDS;
        }
        else
        {
            player->m_Local.m_iHideHUD &= ~HIDEHUD_LEADERBOARDS;
        }
    }
}

void TrickList::OnCommand(const char *command)
{
    if (FStrEq(command, "ShowTrickList"))
    {
        m_pMapTeleListButton->SetSelected(false);
        m_pWIPTrickListButton->SetSelected(false);

        m_pTrickList->SetVisible(true);
        m_pMapTeleList->SetVisible(false);
        m_pWIPTrickList->SetVisible(false);
    }
    else if (FStrEq(command, "ShowWIPTrickList"))
    {
        m_pMapTeleListButton->SetSelected(false);
        m_pTrickListButton->SetSelected(false);

        m_pWIPTrickList->SetVisible(true);
        m_pMapTeleList->SetVisible(false);
        m_pTrickList->SetVisible(false);
    }
    else if (FStrEq(command, "ShowMapTeleList"))
    {
        m_pWIPTrickListButton->SetSelected(false);
        m_pTrickListButton->SetSelected(false);

        m_pMapTeleList->SetVisible(true);
        m_pTrickList->SetVisible(false);
        m_pWIPTrickList->SetVisible(false);
    }
    else
    {
        BaseClass::OnCommand(command);
    }
}

void TrickList::ApplySchemeSettings(IScheme *pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

}

void TrickList::FireGameEvent(IGameEvent *event)
{
    ParseTrickList();
}

void TrickList::OnItemContextMenu(Panel *panel, int itemID)
{
    if (panel == m_pTrickList)
    {
        const auto pKVData = m_pTrickList->GetItem(itemID);
        if (!pKVData)
            return;

        m_pContextMenu->OnKillFocus();
        m_pContextMenu->DeleteAllItems();

        KeyValues *pKv = new KeyValues("ContextTrickTele");
        pKv->SetInt("trickID", m_pTrickList->GetItemUserData(itemID));
        m_pContextMenu->AddMenuItem("TrickTele", "#MOM_Tricks_Teleport", pKv, this);

        /*if (pOurMap && FStrEq(pMap, pOurMap))
        {
            if (!bSpectating)
            {
                pKv = new KeyValues("ContextSpectate");
                pKv->SetUint64("target", steamID);
                m_pContextMenu->AddMenuItem("SpectateLobbyMember", "#MOM_Lobby_Spectate", pKv, this);

                pKv = new KeyValues("ContextTeleport");
                pKv->SetUint64("target", steamID);
                m_pContextMenu->AddMenuItem("TeleportToLobbyMember", "#MOM_Lobby_TeleportTo", pKv, this);
            }

            pKv = new KeyValues("ContextReqSavelocs");
            pKv->SetUint64("target", steamID);
            m_pContextMenu->AddMenuItem("ReqSavelocs", "#MOM_Saveloc_Frame", pKv, this);
        }
        else if (!(bMainMenu || bCredits || bBackground))
        {
            pKv = new KeyValues("ContextGoToMap");
            pKv->SetString("map", pMap);
            m_pContextMenu->AddMenuItem("GoToMap", "#MOM_Lobby_GoToMap", pKv, this);
        }

        if (ownerID == locID)
        {
            m_pContextMenu->AddSeparator();
            pKv = new KeyValues("ContextMakeOwner");
            pKv->SetUint64("target", steamID);
            m_pContextMenu->AddMenuItem("MakeLobbyOwner", "#MOM_Lobby_MakeOwner", pKv, this);
        }

        // MOM_TODO: More options here, such as:
        // hiding decals (maybe toggle paint, bullets separately?)
        // etc
        m_pContextMenu->AddSeparator();
        // Visit profile
        pKv = new KeyValues("ContextVisitProfile");
        pKv->SetUint64("profile", steamID);
        m_pContextMenu->AddMenuItem("VisitProfile", "#MOM_Leaderboards_SteamProfile", pKv, this);*/

        m_pContextMenu->ShowMenu();
    }
    
}

void TrickList::OnItemSelected()
{
    if (m_pMapTeleList->IsVisible())
    {
        const auto selectedItemID = m_pMapTeleList->GetSelectedItem(0);
        const auto pKv = m_pMapTeleList->GetItem(selectedItemID);

        engine->ClientCmd(CFmtStr("mom_tricks_map_tele %i\n", pKv->GetInt("num")).Get());

        m_pMapTeleList->ClearSelectedItems();
    }
    else if (m_pTrickList->IsVisible())
    {
        const auto selectedItemID = m_pTrickList->GetSelectedItem(0);
        const auto iTrickID = m_pTrickList->GetItemUserData(selectedItemID);

        engine->ClientCmd(CFmtStr("mom_tricks_track_trick %i", iTrickID));

        // MOM_TODO keep track of this tracked trick for panel rendering things (outline?)
    }
}

void TrickList::OnContextTrickTele(int trickID)
{
    engine->ClientCmd(CFmtStr("mom_tricks_tele_to_trick %i", trickID));
}

void TrickList::ParseTrickList()
{
    m_pTrickList->RemoveAll();
    m_pWIPTrickList->RemoveAll();
    m_pMapTeleList->RemoveAll();

    const auto iTrickCount = g_pTrickSystem->GetTrickCount();
    for (int i = 0; i < iTrickCount; i++)
    {
        const auto pTrick = g_pTrickSystem->GetTrick(i);

        const auto pKv = new KeyValues("Trick");
        pKv->SetString("name", pTrick->GetName());
        pKv->SetInt("steps", pTrick->StepCount());
        pKv->SetInt("difficulty", pTrick->GetDifficulty());

        m_pTrickList->AddItem(pKv, pTrick->GetID(), false, true);
    }

    const auto iMapTeleCount = g_pTrickSystem->GetMapTeleCount();
    for (int i = 0; i < iMapTeleCount; i++)
    {
        const auto pMapTele = g_pTrickSystem->GetMapTele(i);

        const auto pKv = new KeyValues("MapTele");
        pKv->SetInt("num", i+1);
        pKv->SetString("name", pMapTele->m_szName);

        m_pMapTeleList->AddItem(pKv, 0, false, true);
    }

    // Post process
}

static int SortNameFunc(ListPanel *pListPanel, const ListPanelItem &item1, const ListPanelItem &item2)
{
    return Q_strcmp(item1.kv->GetString("name"), item2.kv->GetString("name"));
}

static int SortNumFunc(ListPanel *pListPanel, const ListPanelItem &item1, const ListPanelItem &item2)
{
    return item1.kv->GetInt("num") < item2.kv->GetInt("num");
}

void TrickList::InitTrickListHeaders()
{
    // m_pTrickList->SetImageList(m_pImageListLobby, false);
    /*m_pTrickList->AddColumnHeader(0, "isOwner", "", GetScaledVal(30),
                                   ListPanel::COLUMN_IMAGE | ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_DISABLED);
    m_pTrickList->AddColumnHeader(1, "avatar", "", GetScaledVal(30),
                                   ListPanel::COLUMN_IMAGE | ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_DISABLED);*/

    m_pTrickList->AddColumnHeader(0, "name", "#MOM_Name", GetScaledVal(90), GetScaledVal(30), GetScaledVal(100), 0);
    m_pTrickList->AddColumnHeader(1, "steps", "Steps LOCALIZE ME", GetScaledVal(120), 0);
    m_pTrickList->AddColumnHeader(2, "difficulty", "#MOM_MapSelector_Difficulty", GetScaledVal(30));

    m_pTrickList->SetSortFunc(0, SortNameFunc);
    m_pTrickList->SetSortColumn(0);
}

void TrickList::InitWIPTrickListHeaders()
{
    m_pWIPTrickList->AddColumnHeader(0, "name", "#MOM_Name", GetScaledVal(90), GetScaledVal(30), GetScaledVal(100), 0);
    m_pWIPTrickList->AddColumnHeader(1, "steps", "#MOM_Tricks_Steps", GetScaledVal(120), 0);
    m_pWIPTrickList->AddColumnHeader(2, "difficulty", "#MOM_MapSelector_Difficulty", GetScaledVal(30));

    m_pWIPTrickList->SetSortFunc(0, SortNameFunc);
    m_pWIPTrickList->SetSortColumn(0);
}

void TrickList::InitMapTeleListHeaders()
{
    m_pMapTeleList->AddColumnHeader(0, "num", "", GetScaledVal(90), GetScaledVal(30), GetScaledVal(100), 0);
    m_pMapTeleList->AddColumnHeader(1, "name", "#MOM_Name", GetScaledVal(120), 0);

    m_pMapTeleList->SetSortFunc(0, SortNumFunc);
    m_pMapTeleList->SetSortColumn(0);
}
