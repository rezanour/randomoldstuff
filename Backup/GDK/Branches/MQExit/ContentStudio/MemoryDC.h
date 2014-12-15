#pragma once

class CMemoryDC : public CDC 
{
public:
    CMemoryDC(CDC* pDC) : 
      CDC(), 
      m_backBuffer(NULL), 
      m_pDC(pDC)
    {
        m_bUseMemDC = !pDC->IsPrinting();

        if (m_bUseMemDC)
        {
            CreateCompatibleDC(pDC);
            pDC->GetClipBox(&m_rect);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_backBuffer = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        } 
        else 
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
        
    virtual ~CMemoryDC()
    {
        if (m_bUseMemDC) 
        {
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), this, m_rect.left, m_rect.top, SRCCOPY);
            SelectObject(m_backBuffer);
        } 
        else 
        {
            m_hDC = m_hAttribDC = NULL;
        }
    }

    CMemoryDC* operator->() {return this;}
    operator CMemoryDC*() {return this;}

private:
    CBitmap  m_bitmap;
    CBitmap* m_backBuffer;
    CDC*     m_pDC;
    CRect    m_rect;
    BOOL     m_bUseMemDC;
};