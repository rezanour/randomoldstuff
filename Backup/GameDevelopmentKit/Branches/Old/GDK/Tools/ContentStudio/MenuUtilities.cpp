#include "stdafx.h"

void AddSubMenuItem(CMenu *pMenu, UINT cmdID, const WCHAR *string)
{
  ASSERT_VALID(pMenu);
  ASSERT(string);
  VERIFY(pMenu->AppendMenu(MF_STRING | MF_ENABLED, cmdID, string));
}

void InsertSubMenuItem(CMenu *pMenu, UINT itemIndex, UINT cmdID, const WCHAR *string)
{
  ASSERT_VALID(pMenu);
  ASSERT(string);
  VERIFY(pMenu->InsertMenu(itemIndex, MF_STRING | MF_ENABLED | MF_BYPOSITION, cmdID, string));
}

void RemoveSubMenuItem(CMenu *pMenu, UINT cmdID)
{
  ASSERT_VALID(pMenu);
  VERIFY(pMenu->RemoveMenu(cmdID, MF_BYCOMMAND));
}

CMenu *AddSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex, 
                  const WCHAR *string, MenuItemData *data /* = NULL */)
{
  ASSERT_VALID(pWnd);
  ASSERT(string);

  CMenu *pMenuBar = pWnd->GetMenu();
  ASSERT_VALID(pMenuBar);

  CMenu *pSubMenu = pMenuBar->GetSubMenu(mainMenuItem);
  ASSERT_VALID(pSubMenu);

  CMenu *pMenu = new CMenu;
  VERIFY(pMenu->CreatePopupMenu());
  VERIFY(pSubMenu->InsertMenu(itemIndex, MF_BYPOSITION | MF_POPUP, (UINT)pMenu->m_hMenu, string));

  if (data) 
    for (int i = 0; data[i].string; i++)
      AddSubMenuItem(pMenu, data[i].cmdID, data[i].string);

  return pMenu;
}

void RemoveSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex)
{
  ASSERT_VALID(pWnd);

  CMenu *pMenuBar = pWnd->GetMenu();
  ASSERT_VALID(pMenuBar);

  CMenu *pSubMenu = pMenuBar->GetSubMenu(mainMenuItem);
  ASSERT_VALID(pSubMenu);

  CMenu *pMenu = pSubMenu->GetSubMenu(itemIndex);
  ASSERT_VALID(pMenu);

  VERIFY(pSubMenu->RemoveMenu(itemIndex, MF_BYPOSITION));
  delete pMenu;
}

void ClearSubMenu(CFrameWnd *pWnd, UINT mainMenuItem, UINT itemIndex)
{
  ASSERT_VALID(pWnd);

  CMenu *pMenuBar = pWnd->GetMenu();
  ASSERT_VALID(pMenuBar);

  CMenu *pSubMenu = pMenuBar->GetSubMenu(mainMenuItem);
  ASSERT_VALID(pSubMenu);

  CMenu *pMenu = pSubMenu->GetSubMenu(itemIndex);
  ASSERT_VALID(pMenu);

  while (pSubMenu->RemoveMenu(0, MF_BYPOSITION));
}
