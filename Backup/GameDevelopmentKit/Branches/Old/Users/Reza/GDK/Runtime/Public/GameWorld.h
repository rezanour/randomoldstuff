#pragma once

#include "Platform.h"
#include "GameObject.h"

namespace GDK
{
    class GameObject;
    class Camera;

    typedef std::shared_ptr<Camera> CameraPtr;
    typedef std::vector<GameObjectPtr> GameObjectList;

    class GameWorld;
    typedef std::shared_ptr<GameWorld> GameWorldPtr;

    class GameWorld : public NonCopyable
    {
    public:
        static GameWorldPtr CreateNew(_In_ const std::wstring& name);
        static GameWorldPtr Load(_In_ const std::wstring& name);

        // Enable/disable editing
        bool IsEditingEnabled() const;
        void EnableEditing(_In_ bool enable);

        // Identity
        const std::wstring& GetName() const;
        void SetName(_In_ const std::wstring& name);

        // Object management
        const GameObjectList& GetObjects() const;
        void AddObject(_In_ const GameObjectPtr& gameObject);
        void RemoveObject(_In_ const GameObjectPtr& gameObject);

        const CameraPtr& GetActiveCamera() const;
        void SetActiveCamera(_In_ const CameraPtr& camera);

        // Run simulation. Behavior depends on whether or not
        // world is in editing mode.
        void Update(_In_ double elapsedSeconds);

        // Draw using active camera
        void Draw();

        // Draw using a custom camera view (used by cut scenes and editor)
        void Draw(_In_ const Matrix& view, _In_ const Matrix& projection);

    private:
        GameWorld(_In_ const std::wstring& name, _In_ bool startEditing);

        std::wstring _name;
        std::vector<GameObjectPtr> _gameObjects;
        CameraPtr _activeCamera;
        bool _editingEnabled;
    };
}
