#pragma once

#include <Platform.h>

namespace GDK
{
    struct IGameObjectEdit;
    struct VisualInfo;
    struct DeviceContext;
    struct Ray;
    class Matrix;
    class GameWorldContent;

    struct IGameWorldEdit
    {
        virtual bool IsEditing() const = 0;

        virtual const std::wstring& GetName() const = 0;
        virtual void SetName(_In_ const std::wstring& name) = 0;

        virtual std::shared_ptr<IGameObjectEdit> CreateModel(_In_ const std::vector<VisualInfo>& visualInfos) = 0;
        virtual const std::vector<VisualInfo>& GetModel(_In_ uint32_t index) const = 0;

        virtual std::shared_ptr<IGameObjectEdit> CreateObjectEdit(_In_ const std::map<std::wstring, std::wstring>& properties) = 0;
        virtual void RemoveObject(_In_ const std::shared_ptr<IGameObjectEdit>& object) = 0;

        virtual void Draw(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;

        virtual std::shared_ptr<IGameObjectEdit> PickClosestObject(_In_ const Ray& ray) = 0;

        virtual std::shared_ptr<GameWorldContent> SaveToContent() const = 0;
    };

    namespace GameWorld
    {
        std::shared_ptr<IGameWorldEdit> Create(_In_ const std::wstring& name, _In_ const DeviceContext& deviceContext);
        std::shared_ptr<IGameWorldEdit> Create(_In_ const std::shared_ptr<GameWorldContent>& content, _In_ const DeviceContext& deviceContext);
    }
}
