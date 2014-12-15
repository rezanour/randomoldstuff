#include "StdAfx.h"
#include "SceneObject.h"
#include "ITransformProvider.h"

namespace Lucid3D
{
    namespace Graphics
    {
        SceneObject::SceneObject(_In_ ITransformProvider* provider)
            : _provider(provider)
        {
            if (_provider)
            {
                _provider->AddRef();
            }
        }

        SceneObject::SceneObject(_In_ const Matrix& transform)
            : _provider(nullptr), _transform(transform)
        {
        }

        SceneObject::~SceneObject()
        {
            SafeRelease(_provider);
        }

        // Properties
        void SceneObject::SetTransform(_In_ const Matrix& transform)
        {
            SafeRelease(_provider);
            _transform = transform;
        }

        void SceneObject::GetTransform(_Out_ Matrix* transform) const
        {
            if (transform)
            {
                if (_provider)
                {
                    _provider->GetTransform(transform);
                }
                else
                {
                    *transform = _transform;
                }
            }
        }

        void SceneObject::SetProvider(_In_ ITransformProvider* provider)
        {
            _provider = provider;
            if (_provider)
            {
                _provider->AddRef();
            }
        }
    }
}
