#pragma once

struct IInput;

bool DismissButtonPressed(_In_ std::shared_ptr<IInput> input);
bool SelectButtonPressed(_In_ std::shared_ptr<IInput> input);
bool NextButtonPressed(_In_ std::shared_ptr<IInput> input);
bool PrevItemButtonPressed(_In_ std::shared_ptr<IInput> input);
bool FireButtonPressed(_In_ std::shared_ptr<IInput> input);
uint32_t NextItemWrapped(_In_ uint32_t currentItem, _In_ uint32_t maxItems);
uint32_t PrevItemWrapped(_In_ uint32_t currentItem, _In_ uint32_t maxItems);
