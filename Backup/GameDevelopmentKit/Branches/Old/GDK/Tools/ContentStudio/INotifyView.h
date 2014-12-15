#pragma once

GDKINTERFACE INotifyView : public IUnknown
{
    IMPL_GUID(0xf5e1e9dc, 0x9b57, 0x4c50, 0xb2, 0x70, 0x1b, 0x35, 0x63, 0x15, 0xce, 0x2a);

    virtual void OnDocumentChanged() = 0;
    virtual void OnContentSelectionChanged() = 0;
    virtual void OnContentSelectionOpened() = 0;
    virtual void OnContentFrameChanged(_In_ bool incrementedFrame) = 0;
};
