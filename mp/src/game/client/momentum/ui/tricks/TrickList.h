#pragma once

#include "mom_shareddefs.h"
#include "game/client/iviewport.h"
#include "vgui_controls/EditablePanel.h"

class CLeaderboardsContextMenu;

class TrickList : public vgui::EditablePanel, public IViewPortPanel, public CGameEventListener
{
public:
    DECLARE_CLASS_SIMPLE(TrickList, vgui::EditablePanel);
    TrickList(IViewPort *pViewPort);
    ~TrickList();

    const char *GetName() override { return PANEL_TRICK_LIST; }
    void SetData(KeyValues *data) override {};
    void Reset() override;
    void Update() override;
    bool NeedsUpdate() override;
    void ShowPanel(bool state) override;

    vgui::VPANEL GetVPanel() override { return BaseClass::GetVPanel(); }
    bool IsVisible() override { return BaseClass::IsVisible(); }
    void SetParent(vgui::VPANEL parent) override { BaseClass::SetParent(parent); }
    bool HasInputElements() override { return true; }

    void OnCommand(const char *command) override;

    void ApplySchemeSettings(vgui::IScheme *pScheme) override;

    void FireGameEvent(IGameEvent *event) override;

    MESSAGE_FUNC_PTR_INT(OnItemContextMenu, "OpenContextMenu", panel, itemID);
    MESSAGE_FUNC(OnItemSelected, "ItemSelected");

    MESSAGE_FUNC_INT(OnContextTrickTele, "ContextTrickTele", trickID);

private:
    void InitTrickListHeaders();
    void InitWIPTrickListHeaders();
    void InitMapTeleListHeaders();
    void ParseTrickList();

    vgui::ListPanel *m_pTrickList, *m_pWIPTrickList, *m_pMapTeleList;
    vgui::Button *m_pTrickListButton, *m_pWIPTrickListButton, *m_pMapTeleListButton;
    CLeaderboardsContextMenu *m_pContextMenu;
};

extern TrickList *g_pTrickEditor;