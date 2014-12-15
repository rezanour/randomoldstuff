#pragma once

class ControlsMenu : public Menu
{
public:
    ControlsMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
};