#include "StdAfx.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "TaskManager.h"
#include "Task.h"
#include "ITypeObject.h"
#include "Message.h"
#include "Screen.h"
#include "ObjectPool.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    using namespace GDK;

    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    HRESULT hr = Executive::Startup();
    if (SUCCEEDED(hr))
    {
        void* p = Executive::GetMemoryManager()->Alloc(145);
        if (p)
        {
            Executive::GetMemoryManager()->Free(p);
        }

        Task* task = nullptr;
        hr = Task::Create(&task);
        if (SUCCEEDED(hr))
        {
            task->SetName("my custom task");

            OutputDebugStringA(task->GetName());
            OutputDebugString(L"\n");

            if (task->GetType()->GetID() == 1)
            {
                OutputDebugStringA(task->GetType()->GetName());
                OutputDebugString(L"\n");
            }

            hr = Executive::GetTaskManager()->AddTask(task);
            if (SUCCEEDED(hr))
            {
                hr = Executive::GetTaskManager()->RunOneFrame();
                if (SUCCEEDED(hr))
                {
                    hr = Executive::GetTaskManager()->CancelTask(task);
                    if (SUCCEEDED(hr))
                    {
                        hr = Executive::GetTaskManager()->CancelTask(task);
                        if (SUCCEEDED(hr))
                        {
                            // that should fail
                            OutputDebugString(L"Cancel twice didn't fail!\n");
                        }
                        else
                        {
                            hr = S_OK;
                        }
                    }
                }
            }

            task->Release();
        }

        if (SUCCEEDED(hr))
        {
            struct CustomMessage
            {
                uint32_t someValue;
                uint32_t otherValue;
            };

            Message* message = nullptr;
            hr = Message::Create(MessageType_Task, 3, sizeof(CustomMessage), &message);
            if (SUCCEEDED(hr))
            {
                CustomMessage* customMessage = reinterpret_cast<CustomMessage*>(message->GetBody());
                customMessage->someValue = 32;
                customMessage->otherValue = 64;

                const MessageHeader* header = message->GetHeader();

                if (header->BodySize != sizeof(CustomMessage))
                {
                    OutputDebugString(L"Message is corrupt!\n");
                    hr = E_UNEXPECTED;
                }

                message->Release();
            }
        }

        if (SUCCEEDED(hr))
        {
            Screen* screen;
            hr = Screen::Create(Executive::GetObjectNamespace(), &screen);
            if (SUCCEEDED(hr))
            {
                screen->Release();
            }
        }

        if (SUCCEEDED(hr))
        {
            ObjectPool* pool;
            hr = ObjectPool::Create(Executive::GetMemoryManager(), &pool);
            if (SUCCEEDED(hr))
            {
                void* foo = pool->Alloc(20, "foo");
                if (foo)
                {
                    pool->Free(foo);
                }

                struct Gooz
                {
                    Gooz()
                    {
                        OutputDebugString(L"Gooz()\n");
                    }
                    Gooz(int i)
                    {
                        i;
                        OutputDebugString(L"Gooz(int i)\n");
                    }
                    Gooz(float f, int i)
                    {
                        f;
                        i;
                        OutputDebugString(L"Gooz(float f, int i)\n");
                    }

                    ~Gooz()
                    {
                        OutputDebugString(L"~Gooz()\n");
                    }
                };

                Gooz* gooz = pool->Create<Gooz>(3.0f, 5);
                if (gooz)
                {
                    pool->Destroy(gooz);
                }

                delete pool;
            }
        }

        Executive::Shutdown();
    }

    return 0;
}
