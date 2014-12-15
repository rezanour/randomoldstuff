#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CUIThread
 
IMPLEMENT_DYNCREATE(CUIThread, CWinThread)
 
BEGIN_MESSAGE_MAP(CUIThread, CWinThread)
    //{{AFX_MSG_MAP(CUIThread)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
 
CUIThread::CUIThread()
{
    m_bAutoDelete = FALSE;
 
    // kill event starts out in the signaled state
    m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
}
 
BOOL CUIThread::InitInstance()
{
    // wait for the kill notification.  service the 
// message queue.
    while (WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT)
        ;
     
    // avoid entering standard message loop by returning FALSE
    return FALSE;
}
 
void CUIThread::Delete()
{
    // calling the base here won't do anything but it is a 
// good habit
    CWinThread::Delete();
 
    // acknowledge receipt of kill notification
    VERIFY(SetEvent(m_hEventDead));
}
 
CUIThread::~CUIThread()
{
    CloseHandle(m_hEventKill);
    CloseHandle(m_hEventDead);
}
 
void CUIThread::KillThread()
{
    // Note: this function is called in the context of 
// other threads, not the thread itself.
 
    // reset the m_hEventKill which signals the thread 
// to shutdown
    VERIFY(SetEvent(m_hEventKill));
 
    // allow thread to run at higher priority during 
// kill process
    SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
    WaitForSingleObject(m_hEventDead, INFINITE);
    WaitForSingleObject(m_hThread, INFINITE);
 
    // now delete CWinThread object since no longer necessary
    delete this;
}
