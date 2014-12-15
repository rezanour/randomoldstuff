#pragma once

class CUIThread : public CWinThread
{
public:
    DECLARE_DYNCREATE(CUIThread)
    CUIThread();
 
// Attributes
public:
    HANDLE m_hEventKill;
    HANDLE m_hEventDead;
 
// Operations
public:
    void KillThread();
 
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGDIThread)
    //}}AFX_VIRTUAL
 
// Implementation
public:
    virtual ~CUIThread();
    virtual void Delete();
 
protected:
    virtual BOOL InitInstance();
 
    // Generated message map functions
    //{{AFX_MSG(CUIThread)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
 
    DECLARE_MESSAGE_MAP()
};
