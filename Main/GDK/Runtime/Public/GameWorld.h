#pragma once

#include "Platform.h"
#include "RuntimeObject.h"

namespace GDK
{
    class GraphicsDevice;
    class AudioDevice;
    class GameObject;
    class GameWorldContent;
    class Matrix;
    struct GameTime;
    struct Ray;

    // Signature for create methods the game registers so that it can new up the appropriate derived class.
    // The game should not do any further initialization here, as that will be done by the create factory
    // and the game will get callbacks accordingly. This is further supported by the lack of creation parameters
    // on the create call.
    typedef std::shared_ptr<GameObject> (*GameObjectCreateMethod)(_In_ const std::wstring& typeName);

    struct GameObjectCreateParameters
    {
        GameObjectCreateParameters() {}

        // Optional overrides for factory, geometry, and texture.
        // If not specified, default factory, mesh.geometry and primary.texture under the object path will be used
        GameObjectCreateParameters(_In_ GameObjectCreateMethod factory, _In_ const std::wstring& geometry, _In_ const std::wstring& texture) :
            factory(factory), geometry(geometry), texture(texture)
        {}

        GameObjectCreateMethod factory;
        std::wstring geometry;
        std::wstring texture;
    };

    struct GameWorldDevices
    {
        GameWorldDevices(_In_ const std::shared_ptr<GraphicsDevice>& graphics, _In_ const std::shared_ptr<AudioDevice>& audio) :
            graphicsDevice(graphics), audioDevice(audio)
        {}

        std::shared_ptr<GraphicsDevice> graphicsDevice;
        std::shared_ptr<AudioDevice> audioDevice;
    };

    class GameWorld : public RuntimeObject<GameWorld>
    {
    public:
        //
        // GameObject type registration and creation
        //
        static void RegisterDefaultFactory(_In_ GameObjectCreateMethod defaultFactory);
        static void RegisterObjectType(_In_ const std::wstring& typeName, _In_ const GameObjectCreateParameters& parameters);
        static const GameObjectCreateParameters& GetRegisteredTypeInfo(_In_ const std::wstring& typeName);
        static std::vector<std::wstring> GetRegisteredObjectTypes();

        //
        // GameWorld creation
        //

        // Creates a blank new world for editing
        static std::shared_ptr<GameWorld> Create(_In_ const GameWorldDevices& devices, _In_ const std::wstring& name);

        // Loads a previously saved world file, can be for editing or runtime
        static std::shared_ptr<GameWorld> Create(_In_ const GameWorldDevices& devices, _In_ const std::shared_ptr<GameWorldContent>& content, _In_ bool enableEditing);

        // Clones an existing game world, allowing you to switch editing mode. Used to create an in-game world from an in-progress editing session
        static std::shared_ptr<GameWorld> Create(_In_ const GameWorldDevices& devices, _In_ const std::shared_ptr<GameWorld>& existingWorld, _In_ bool enableEditing);

        //
        // Object creation and manipulation
        //

        // Create a new object. To destroy an object, call Destroy() on the object directly
        std::shared_ptr<GameObject> CreateObject(_In_ const std::wstring& typeName);

        // HACK - Temp solution to unblock Reza's physics work until we fix game object design pattern
        std::shared_ptr<GameObject> CreateObjectEx(_In_ const std::wstring& geometryName, _In_ const std::wstring& textureName);

        // Get list of all game objects. NOTE: This is only available in editing mode
        std::vector<std::shared_ptr<GameObject>> GetGameObjects() const;

        // Find the nearest object along a ray. NOTE: This is only available in editing mode
        std::shared_ptr<GameObject> PickGameObject(_In_ const Ray& ray) const;

        //
        // Drawing of the world
        //

        virtual void Draw(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;

        //
        // Is this an editing session?
        //
        bool IsEditing() const;

        //
        // During update and draw, you can query these parameters
        //

        virtual const GameTime& GetTime() const = 0;
        virtual const Matrix& GetViewMatrix() const = 0;
        virtual const Matrix& GetProjectionMatrix() const = 0;

    protected:
        GameWorld(_In_ const GameWorldDevices& devices, _In_ bool enableEditing);

        void VerifyEditing(_In_ bool editingEnabled) const;

        friend class GameObject;
        const std::shared_ptr<GraphicsDevice>& GetGraphicsDevice() const;

        std::vector<std::shared_ptr<GameObject>> _gameObjects;

    private:
        GameWorldDevices _devices;
        bool _editingEnabled;
    };
}
