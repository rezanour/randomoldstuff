#pragma once

namespace Lucid3D
{
    namespace Graphics
    {
        struct ITransformProvider;

        class SceneObject : public IRefCounted
        {
        public:
            // Construction
            explicit SceneObject(_In_ ITransformProvider* provider = nullptr);
            explicit SceneObject(_In_ const Matrix& transform);
            ~SceneObject();

            // Properties
            void SetTransform(_In_ const Matrix& transform);
            void GetTransform(_Out_ Matrix* transform) const;

            void SetProvider(_In_ ITransformProvider* provider);

        private:
            ITransformProvider* _provider;
            Matrix              _transform;
        };
    }
}
