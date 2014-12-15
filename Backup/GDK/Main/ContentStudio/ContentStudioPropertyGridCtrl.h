#pragma once

class CContentStudioPropertyGridCtrl : public CMFCPropertyGridCtrl
{
    DECLARE_DYNAMIC(CContentStudioPropertyGridCtrl)

public:
    CContentStudioPropertyGridCtrl();
    virtual ~CContentStudioPropertyGridCtrl();

protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL EditItem(CMFCPropertyGridProperty* pProp, LPPOINT lptClick = NULL);
    virtual BOOL EndEditItem(BOOL bUpdateData = TRUE);
    virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
    virtual BOOL ValidateItemData(CMFCPropertyGridProperty* pProp);
};
