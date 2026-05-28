#pragma once
// Thread-safe event dispatch interface.
//
// The SDK fires events from background threads (HTTP/WebSocket).
// Consumers provide an EventDispatcher to marshal callbacks to their
// preferred thread (e.g., Unreal game thread, main thread, etc.).
//
// Default: inline dispatch (same thread). Override for engine integration.

#include <functional>
#include <memory>

namespace chatsdk {

// Consumers implement this to control which thread callbacks run on.
class EventDispatcher {
public:
    virtual ~EventDispatcher() = default;

    // Schedule a callback for execution on the consumer's thread.
    // The SDK guarantees the callback is safe to call from any thread.
    virtual void dispatch(std::function<void()> fn) = 0;
};

// Default dispatcher: executes inline (caller's thread).
class InlineDispatcher : public EventDispatcher {
public:
    void dispatch(std::function<void()> fn) override { if (fn) fn(); }
};

} // namespace chatsdk
