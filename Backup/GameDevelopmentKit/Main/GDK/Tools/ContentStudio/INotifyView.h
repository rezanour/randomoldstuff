#pragma once

class INotifyView
{
public:
    virtual void OnDocumentChanged() = 0;
    virtual void OnContentSelectionChanged() = 0;
    virtual void OnContentSelectionOpened() = 0;
    virtual void OnContentFrameChanged(_In_ bool incrementedFrame) = 0;
};
