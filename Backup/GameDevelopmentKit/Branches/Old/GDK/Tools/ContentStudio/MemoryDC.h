#pragma once

class MemoryDC : public CDC 
{
public:
    MemoryDC(CDC* pDC) : 
      CDC(), 
      _backBuffer(nullptr), 
      _pDC(pDC)
    {
        _useMemDC = !pDC->IsPrinting();

        if (_useMemDC)
        {
            CreateCompatibleDC(pDC);
            pDC->GetClipBox(&_rect);
            _bitmap.CreateCompatibleBitmap(pDC, _rect.Width(), _rect.Height());
            _backBuffer = SelectObject(&_bitmap);
            SetWindowOrg(_rect.left, _rect.top);
        } 
        else 
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
        
    virtual ~MemoryDC()
    {
        if (_useMemDC) 
        {
            _pDC->BitBlt(_rect.left, _rect.top, _rect.Width(), _rect.Height(), this, _rect.left, _rect.top, SRCCOPY);
            SelectObject(_backBuffer);
        } 
        else 
        {
            m_hDC = m_hAttribDC = NULL;
        }
    }

    MemoryDC* operator->() {return this;}
    operator MemoryDC*() {return this;}

private:
    CBitmap  _bitmap;
    CBitmap* _backBuffer;
    CDC*     _pDC;
    CRect    _rect;
    BOOL     _useMemDC;
};