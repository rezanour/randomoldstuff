#pragma once

class GraphicsMenu : public Menu
{
public:
    GraphicsMenu();
    void OnSelected(_In_ MenuItem item) override;
    void OnDismiss() override;
};