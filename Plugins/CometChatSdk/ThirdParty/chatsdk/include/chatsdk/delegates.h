#pragma once
// Callback interfaces for SDK events.
// Consumers register these to receive async notifications.
// Engine-agnostic — no Unreal types here.

#include "chatsdk/types.h"
#include <functional>
#include <string>

namespace chatsdk {

// Fired when a real-time message arrives
using OnMessageReceived = std::function<void(const Message&)>;

// Fired when a user's presence changes
using OnPresenceChanged = std::function<void(const Presence&)>;

// Fired when someone starts/stops typing
struct TypingEvent {
    std::string uid;
    std::string conversation_id;
    bool is_typing = false;
};
using OnTypingChanged = std::function<void(const TypingEvent&)>;

// Fired on message delivery/read receipts
struct ReceiptEvent {
    std::string message_id;
    std::string uid;
    std::string status; // "delivered", "read"
    int64_t timestamp = 0;
};
using OnReceiptReceived = std::function<void(const ReceiptEvent&)>;

// Connection state
enum class ConnectionState { Connected, Disconnected, Reconnecting };
using OnConnectionStateChanged = std::function<void(ConnectionState)>;

} // namespace chatsdk
