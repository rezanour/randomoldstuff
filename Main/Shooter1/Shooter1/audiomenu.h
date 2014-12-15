#pragma once

class AudioMenu : public Menu
{
public:
    AudioMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
    void OnRightSelected(_In_ MenuItem item) override;
    void OnLeftSelected(_In_ MenuItem item) override;

private:

    void UpdateMenuText();
};