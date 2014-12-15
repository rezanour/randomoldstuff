#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_SCENEMANAGER_H_
#define _GDK_SCENEMANAGER_H_

namespace GDK
{
    class SceneManager : private stde::non_copyable
    {
    public:
        SceneManager();
        ~SceneManager();

        // Updating
        void Step();

    private:
    };

} // GDK

#endif // _GDK_SCENEMANAGER_H_
