#pragma once
// ============================================================
// cometchat.h — Single facade entrypoint for the Chat SDK.
//
// This is the ONLY header most consumers need to include.
// All internal implementation is hidden behind the pimpl.
// ============================================================

#include "chatsdk/config.h"
#include "chatsdk/platform.h"
#include "chatsdk/types.h"
#include "chatsdk/errors.h"
#include "chatsdk/delegates.h"
#include "chatsdk/event_bridge.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace chatsdk {

// Result callback pattern: (error_message, result). Empty error = success.
template <typename T>
using ResultCallback = std::function<void(const std::string& error, const T& result)>;
using VoidCallback = std::function<void(const std::string& error)>;

class ChatSDK {
public:
    // --- Lifecycle ---
    explicit ChatSDK(const Config& config,
                     std::shared_ptr<EventDispatcher> dispatcher = std::make_shared<InlineDispatcher>());
    ~ChatSDK();

    // Non-copyable, movable
    ChatSDK(const ChatSDK&) = delete;
    ChatSDK& operator=(const ChatSDK&) = delete;
    ChatSDK(ChatSDK&&) noexcept;
    ChatSDK& operator=(ChatSDK&&) noexcept;

    void shutdown();

    // --- Auth ---
    void login(const std::string& uid, const std::string& auth_key, VoidCallback cb);
    void login_with_auth_token(const std::string& auth_token, VoidCallback cb);
    void logout(VoidCallback cb);
    bool is_authenticated() const;

    // --- Users ---
    void get_user(const std::string& uid, ResultCallback<User> cb);

    // --- Messages ---
    void send_message(const std::string& receiver_uid, const std::string& text,
                      ResultCallback<Message> cb);
    void get_messages(const std::string& uid, int limit,
                      ResultCallback<std::vector<Message>> cb);

    // --- Groups ---
    void create_group(const std::string& name, const std::vector<std::string>& member_ids,
                      ResultCallback<Group> cb);
    void join_group(const std::string& guid, VoidCallback cb);
    void leave_group(const std::string& guid, VoidCallback cb);
    void send_group_message(const std::string& guid, const std::string& text,
                            ResultCallback<Message> cb);
    void get_group_messages(const std::string& guid, int limit,
                            ResultCallback<std::vector<Message>> cb);
    void get_group_messages(const std::string& guid, int limit,
                            int64_t before_message_id,
                            ResultCallback<PaginatedMessages> cb);

    // --- Real-time event listeners ---
    void on_message_received(OnMessageReceived handler);
    void on_presence_changed(OnPresenceChanged handler);
    void on_typing_changed(OnTypingChanged handler);
    void on_receipt_received(OnReceiptReceived handler);
    void on_connection_state_changed(OnConnectionStateChanged handler);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace chatsdk
