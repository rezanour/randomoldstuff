#pragma once

class OptionsMenu : public Menu
{
public:
    OptionsMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
};