// threaded_queue.h
// A thread worker queue that is used like a collection

#pragma once

#ifdef WIN32

#include <Windows.h>
#include <concurrent_queue.h>
#include <array>
#include "win32handle.h"

namespace stde
{
    class thread_workItem
    {
    public:
        thread_workItem()  :
            _refCount(1)
        {
        }

        virtual ~thread_workItem() {}

        // Implemented by the caller to perform the work
        virtual HRESULT DoWork() = 0;

        // Override called when work item is about to be destroyed
        virtual void OnFinalRelease() {}

        void AddRef() { InterlockedIncrement(&_refCount); }

        void Release()
        {
            if (InterlockedDecrement(&_refCount) == 0)
            {
                OnFinalRelease();
                delete this;
            }
        }

    protected:
        long _refCount;
    };

    class threaded_queue sealed
    {
    public:
        threaded_queue() :
            _events(),
            _thread(),
            _threadId(0),
            _threadPaused(false)
        {
        }

        ~threaded_queue()
        {
            uninitialize();
        }

        HRESULT __fastcall push(_In_ thread_workItem* ptr)
        {
            HRESULT hr = S_OK;
            if (ptr == nullptr)
            {
                return E_INVALIDARG;
            }

            if (!_thread)
            {
                hr = initialize();
            }

            if (SUCCEEDED(hr))
            {
                ptr->AddRef();
                _workerItems.push(ptr);
            }

            // Kick thread to process this first item
            if (SUCCEEDED(hr) && _workerItems.unsafe_size() == 1)
            {
                reset_empty_event();
                signal_next_worker_event();
            }

            return hr;
        }

        bool __fastcall paused() const
        {
            return _threadPaused;
        }

        void __fastcall pause() 
        {
            _threadPaused = true;
        }

        void __fastcall resume()
        {
            // Update pause flag and kick thread to start it again
            _threadPaused = false;
            signal_next_worker_event();
        }

        size_t __fastcall size() const { return _workerItems.unsafe_size(); }

        void __fastcall signal_thread_started_event()   { signal_event(thread_worker_queue_started_event); }
        void __fastcall signal_thread_exited_event()    { signal_event(thread_worker_queue_exited_event); }

        bool __fastcall wait_for_queue_empty(DWORD millisecondTimeout)
        {
            return (WaitForSingleObject(_emptyEvent, millisecondTimeout) == WAIT_OBJECT_0);
        }

        DWORD __fastcall wait_for_events()
        {
            // Wait for only the exit and next events
            HANDLE events[] = { _events[0], _events[1] };
            return WaitForMultipleObjects(2, events, FALSE, INFINITE); 
        }

        void __fastcall run_next_worker_item()
        {
            if (!paused())
            {
                thread_workItem* pWorker = get_next_worker_item();
                if (pWorker != nullptr)
                {
                    pWorker->DoWork();
                    pWorker->Release();
                    signal_next_worker_event();
                }
            }
        }

        static unsigned long __stdcall ThreadProc(_In_ void* ptr)
        {
            threaded_queue* pThreadWorkerQueue = (threaded_queue*)ptr;
            if (pThreadWorkerQueue != nullptr)
            {
                bool exitThread = false;

                // Signal that this thread has started
                pThreadWorkerQueue->signal_thread_started_event();

                do
                {
                    // Wait for event signal
                    switch(pThreadWorkerQueue->wait_for_events())
                    {
                    case thread_worker_queue_exit_event:
                        exitThread = true;
                        break;

                    case thread_worker_queue_next_event:
                        pThreadWorkerQueue->run_next_worker_item();
                        break;

                    case thread_worker_queue_started_event:
                        break;

                    case thread_worker_queue_exited_event:
                        break;

                    default:
                        break;
                    }
                } while(!exitThread);

                // Signal that the thread has exited
                pThreadWorkerQueue->signal_thread_exited_event();
            }

            return 0;
        }

    private:

        HRESULT __fastcall initialize()
        {
            HRESULT hr = S_OK;
            if (_thread == nullptr)
            {
                // Create terminate event
                if (SUCCEEDED(hr))
                {
                    _events[thread_worker_queue_exit_event].attach(CreateEvent(NULL, FALSE, FALSE, NULL));
                    if (!_events[thread_worker_queue_exit_event])
                    {
                        hr = HRESULT_FROM_WIN32(::GetLastError());
                    }
                }

                // Create next work item event
                if (SUCCEEDED(hr))
                {
                    _events[thread_worker_queue_next_event].attach(CreateEvent(NULL, FALSE, FALSE, NULL));
                    if (!_events[thread_worker_queue_next_event])
                    {
                        hr = HRESULT_FROM_WIN32(::GetLastError());
                    }
                }

                // Create next work item event
                if (SUCCEEDED(hr))
                {
                    _events[thread_worker_queue_started_event].attach(CreateEvent(NULL, FALSE, FALSE, NULL));
                    if (!_events[thread_worker_queue_started_event])
                    {
                        hr = HRESULT_FROM_WIN32(::GetLastError());
                    }
                }

                // Create exited event
                if (SUCCEEDED(hr))
                {
                    _events[thread_worker_queue_exited_event].attach(CreateEvent(NULL, FALSE, FALSE, NULL));
                    if (!_events[thread_worker_queue_exited_event])
                    {
                        hr = HRESULT_FROM_WIN32(::GetLastError());
                    }
                }

                // Create queue empty event
                if (SUCCEEDED(hr))
                {
                    _emptyEvent.attach(CreateEvent(NULL, FALSE, TRUE, NULL));
                    if (!_emptyEvent)
                    {
                        hr = HRESULT_FROM_WIN32(::GetLastError());
                    }
                }

                if (SUCCEEDED(hr))
                {
                    // Create thread
                    _thread.attach(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadProc, this, 0, &_threadId));
                    if (!_thread)
                    {
                        hr = HRESULT_FROM_WIN32(::GetLastError());
                    }
                    else
                    {
                        // Start thread
                        wait_for_thread_started_event();
                    }
                }
            }

            return hr;
        }

        void __fastcall uninitialize()
        {
            pause();

            // Signal thread to exit
            signal_exit_event();

            // Wait for thread to properly exit
            wait_for_thread_exited_event();

            remove_all_worker_items();

            // event handles & thread will be released when they this object goes out of scope
        }

        thread_workItem* __fastcall get_next_worker_item()
        {
            thread_workItem* pWorkerItem = nullptr;
            if (!_workerItems.try_pop(pWorkerItem))
            {
                set_empty_event();
            }

            return pWorkerItem;
        }

        void __fastcall remove_all_worker_items()
        {
            thread_workItem* pWorkerItem = nullptr;
            pWorkerItem = get_next_worker_item();

            while(pWorkerItem != nullptr)
            {
                pWorkerItem->Release();
                pWorkerItem = get_next_worker_item();
            }
        }

        void __fastcall reset_empty_event()
        {
            if (_emptyEvent)
            {
                ResetEvent(_emptyEvent);
            }
        }

        void __fastcall set_empty_event()
        {
            if (_emptyEvent)
            {
                SetEvent(_emptyEvent);
            }
        }

        void __fastcall signal_next_worker_event()  { signal_event(thread_worker_queue_next_event); }
        void __fastcall signal_exit_event()         { signal_event(thread_worker_queue_exit_event); }

        void __fastcall signal_event(int index)
        {
            if (_events[index] != nullptr)
            {
                SetEvent(_events[index]);
            }
        }

        void __fastcall wait_for_thread_exited_event()    { wait_for_event(thread_worker_queue_exited_event); }
        void __fastcall wait_for_thread_started_event()   { wait_for_event(thread_worker_queue_started_event); }

        void __fastcall wait_for_event(int index)
        {
            if (_events[index] != nullptr)
            {
                WaitForSingleObject(_events[index], INFINITE);
            }
        }

        static const int thread_worker_queue_exit_event    = 0;
        static const int thread_worker_queue_next_event    = 1;
        static const int thread_worker_queue_started_event = 2;
        static const int thread_worker_queue_exited_event  = 3;

        Concurrency::concurrent_queue<thread_workItem*> _workerItems;
        std::array<stde::event_handle, 4> _events;
        stde::event_handle _emptyEvent;
        stde::thread_handle _thread;
        DWORD  _threadId;
        bool _threadPaused;
    };
}

#endif