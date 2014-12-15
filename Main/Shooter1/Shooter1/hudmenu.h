#pragma once

class HudMenu : public Menu
{
public:
    HudMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
};