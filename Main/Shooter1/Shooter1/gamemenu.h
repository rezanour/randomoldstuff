#pragma once

class GameMenu : public Menu
{
public:
    GameMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
};