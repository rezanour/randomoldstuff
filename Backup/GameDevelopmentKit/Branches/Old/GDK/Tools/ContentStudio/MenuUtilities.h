#pragma once

struct MenuItemData {
  UINT cmdID;
  const WCHAR *string;
};

void AddSubMenuItem(CMenu *pMenu, UINT cmdID, const WCHAR *string);
void InsertSubMenuItem(CMenu *pMenu, UINT itemIndex, UINT cmdID, const WCHAR *string);
void RemoveSubMenuItem(CMenu *pMenu, UINT cmdID);
CMenu *AddSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex, const WCHAR *string, MenuItemData *data = NULL);
void RemoveSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex);
void ClearSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex);