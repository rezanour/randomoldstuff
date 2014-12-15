#include "Precomp.h"
#include "Input.h"
#include "InputHelper.h" 

_Use_decl_annotations_
bool DismissButtonPressed(std::shared_ptr<IInput> input)
{
    return (input->IsKeyJustPressed(VK_ESCAPE) || input->IsControllerButtonJustPressed(INPUT_GAMEPAD_B));
}

_Use_decl_annotations_
bool SelectButtonPressed(std::shared_ptr<IInput> input)
{
    return (input->IsKeyJustPressed(VK_RETURN) || input->IsControllerButtonJustPressed(INPUT_GAMEPAD_A));
}

_Use_decl_annotations_
bool NextButtonPressed(std::shared_ptr<IInput> input)
{
    return (input->IsKeyJustPressed(VK_DOWN) || 
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_DPAD_DOWN) ||
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_LEFT_THUMB_DOWN) ||
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_THUMB_DOWN));
}

_Use_decl_annotations_
bool PrevItemButtonPressed(std::shared_ptr<IInput> input)
{
    return (input->IsKeyJustPressed(VK_UP) || 
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_DPAD_UP) ||
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_LEFT_THUMB_UP) ||
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_THUMB_UP));
}

_Use_decl_annotations_
bool FireButtonPressed(std::shared_ptr<IInput> input)
{
    return (input->IsKeyJustPressed(VK_RETURN) || 
        input->IsControllerButtonJustPressed(INPUT_GAMEPAD_RIGHT_TRIGGER));
}

_Use_decl_annotations_
uint32_t NextItemWrapped(uint32_t currentItem, uint32_t maxItems)
{
    uint32_t item = currentItem;
    item++;
    if (item >= maxItems)
    {
        item = 0;
    }

    return item;
}

_Use_decl_annotations_
uint32_t PrevItemWrapped(uint32_t currentItem, uint32_t maxItems)
{
    uint32_t item = currentItem;
    if (item == 0)
    {
        item = maxItems -1;
    }
    else
    {
        item--;
    }

    return item;
}
