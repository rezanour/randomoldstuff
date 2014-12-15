#pragma once

class MainMenu : public Menu
{
public:
    MainMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
};