#include "stdafx.h"
#include "ContentStudioPropertyGridCtrl.h"

IMPLEMENT_DYNAMIC(CContentStudioPropertyGridCtrl, CMFCPropertyGridCtrl)

CContentStudioPropertyGridCtrl::CContentStudioPropertyGridCtrl()
{

}

CContentStudioPropertyGridCtrl::~CContentStudioPropertyGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CContentStudioPropertyGridCtrl, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()



BOOL CContentStudioPropertyGridCtrl::EditItem(CMFCPropertyGridProperty* pProp, LPPOINT lptClick)
{
    // TODO: Add your specialized code here and/or call the base class

    return CMFCPropertyGridCtrl::EditItem(pProp, lptClick);
}


BOOL CContentStudioPropertyGridCtrl::EndEditItem(BOOL bUpdateData)
{
    // TODO: Add your specialized code here and/or call the base class

    return CMFCPropertyGridCtrl::EndEditItem(bUpdateData);
}


void CContentStudioPropertyGridCtrl::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
    // TODO: Add your specialized code here and/or call the base class

    return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}


BOOL CContentStudioPropertyGridCtrl::ValidateItemData(CMFCPropertyGridProperty* pProp)
{
    // TODO: Add your specialized code here and/or call the base class

    return CMFCPropertyGridCtrl::ValidateItemData(pProp);
}
