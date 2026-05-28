#pragma once

#include "chatsdk/event_bridge.h"
#include "Async/Async.h"

/**
 * Thread-safe bridge: SDK background threads → Unreal Game Thread.
 * 
 * The SDK runs HTTP/WebSocket operations on background threads.
 * This dispatcher ensures all callbacks execute on the GameThread,
 * making it safe to interact with UObjects and Blueprints.
 */
class FGameThreadDispatcher : public chatsdk::EventDispatcher
{
public:
    void dispatch(std::function<void()> fn) override
    {
        // Marshal to game thread. The lambda captures fn by value,
        // so the SDK thread can safely return immediately.
        AsyncTask(ENamedThreads::GameThread, [fn = std::move(fn)]()
        {
            if (fn) fn();
        });
    }
};
