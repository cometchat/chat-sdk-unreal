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

// ============================================================
// Request Builders (search, pagination, filters)
// ============================================================

struct UsersRequestBuilder {
    int limit = 30;
    std::string search_keyword;
    std::vector<std::string> search_in;  // "name", "uid"
    std::string user_status;             // "available", "offline", or empty for all
    bool hide_blocked_users = false;
    bool friends_only = false;
    std::vector<std::string> roles;
    std::vector<std::string> tags;
    bool with_tags = false;
    std::vector<std::string> uids;
    std::string sort_by;                 // "status", "name"
    std::string sort_order;              // "asc", "desc"
    int page = 0;
};

struct GroupsRequestBuilder {
    int limit = 30;
    std::string search_keyword;
    bool joined_only = false;
    std::vector<std::string> tags;
    bool with_tags = false;
    int page = 0;
};

struct MessagesRequestBuilder {
    int limit = 30;
    std::string uid;
    std::string guid;
    int64_t message_id = -1;
    int64_t timestamp = -1;
    bool unread = false;
    bool hide_messages_from_blocked_users = false;
    std::string search_keyword;
    int64_t updated_after = -1;
    bool updates_only = false;
    std::vector<std::string> categories;
    std::vector<std::string> types;
    int64_t parent_message_id = -1;
    bool hide_replies = false;
    bool hide_deleted = false;
    std::vector<std::string> tags;
    bool with_tags = false;
    bool mentions_with_tag_info = false;
    bool mentions_with_blocked_info = false;
    bool interaction_goal_completed_only = false;
    bool has_attachments = false;
    bool has_links = false;
    bool has_mentions = false;
    bool has_reactions = false;
    std::vector<std::string> mentioned_uids;
    bool with_parent = false;
    bool hide_quoted_messages = false;
};

struct ConversationsRequestBuilder {
    int limit = 30;
    std::string conversation_type;       // "user", "group", or empty for all
    bool with_user_and_group_tags = false;
    std::vector<std::string> tags;
    bool with_tags = false;
    std::vector<std::string> user_tags;
    std::vector<std::string> group_tags;
    bool include_blocked_users = false;
    bool with_blocked_info = false;
    std::string search_keyword;
    bool unread = false;
    int page = 0;
    bool hide_agentic = false;
    bool only_agentic = false;
};

struct GroupMembersRequestBuilder {
    std::string guid;
    int limit = 30;
    std::string search_keyword;
    std::vector<std::string> scopes;
    std::string status;                  // "available", "offline", or empty
    int page = 0;
};

struct BannedGroupMembersRequestBuilder {
    std::string guid;
    int limit = 30;
    std::string search_keyword;
    std::vector<std::string> scopes;
    int page = 0;
};

struct BlockedUsersRequestBuilder {
    int limit = 30;
    std::string search_keyword;
    std::string direction;               // "blockedByMe", "hasBlockedMe", "both"
    int page = 0;
};

struct ReactionsRequestBuilder {
    int limit = 10;
    int64_t message_id = -1;
    std::string reaction;
};

// ============================================================
// ChatSDK — Main facade
// ============================================================

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

    /// Must be called every frame to pump async HTTP requests.
    /// Returns number of pending requests.
    int tick();

    // --- Init with AppSettings ---
    static void init(const std::string& app_id, const AppSettings& settings, VoidCallback cb);

    // --- Auth ---
    void login(const std::string& uid, const std::string& auth_key, VoidCallback cb);
    void login_with_auth_token(const std::string& auth_token, VoidCallback cb);
    void logout(VoidCallback cb);
    bool is_authenticated() const;
    User get_logged_in_user() const;
    std::string get_user_auth_token() const;

    // --- Connection (Manual Socket Mode) ---
    void connect(VoidCallback cb);
    void disconnect(VoidCallback cb);
    std::string get_connection_status() const;  // "connected", "connecting", "disconnected"
    void ping(VoidCallback cb);

    // --- Users ---
    void get_user(const std::string& uid, ResultCallback<User> cb);
    void fetch_users(const UsersRequestBuilder& request, ResultCallback<std::vector<User>> cb);
    void fetch_blocked_users(const BlockedUsersRequestBuilder& request, ResultCallback<std::vector<User>> cb);
    void block_users(const std::vector<std::string>& uids, VoidCallback cb);
    void unblock_users(const std::vector<std::string>& uids, VoidCallback cb);

    // --- Messages ---
    void send_message(const std::string& receiver_uid, const std::string& text,
                      ResultCallback<Message> cb);
    void send_media_message(const Message& message, ResultCallback<Message> cb);
    void send_custom_message(const Message& message, ResultCallback<Message> cb);
    void edit_message(const Message& message, ResultCallback<Message> cb);
    void delete_message(int64_t message_id, ResultCallback<Message> cb);
    void send_transient_message(const TransientMessage& message);

    void get_messages(const std::string& uid, int limit,
                      ResultCallback<std::vector<Message>> cb);
    void fetch_previous_messages(const MessagesRequestBuilder& request, ResultCallback<std::vector<Message>> cb);
    void fetch_next_messages(const MessagesRequestBuilder& request, ResultCallback<std::vector<Message>> cb);
    void get_message_receipts(int64_t message_id, ResultCallback<std::vector<MessageReceipt>> cb);
    void get_unread_message_count(ResultCallback<std::string> cb); // JSON map

    // --- Conversations ---
    void fetch_conversations(const ConversationsRequestBuilder& request, ResultCallback<std::vector<Conversation>> cb);
    void get_conversation(const std::string& conversation_with, const std::string& conversation_type, ResultCallback<Conversation> cb);
    void delete_conversation(const std::string& conversation_with, VoidCallback cb);

    // --- Groups ---
    void create_group(const std::string& name, const std::vector<std::string>& member_ids,
                      ResultCallback<Group> cb);
    void get_group(const std::string& guid, ResultCallback<Group> cb);
    void update_group(const Group& group, ResultCallback<Group> cb);
    void delete_group(const std::string& guid, VoidCallback cb);
    void join_group(const std::string& guid, VoidCallback cb);
    void join_group(const std::string& guid, const std::string& password, VoidCallback cb);
    void leave_group(const std::string& guid, VoidCallback cb);
    void kick_group_member(const std::string& uid, const std::string& guid, VoidCallback cb);
    void ban_group_member(const std::string& uid, const std::string& guid, VoidCallback cb);
    void unban_group_member(const std::string& uid, const std::string& guid, VoidCallback cb);
    void update_group_member_scope(const std::string& uid, const std::string& guid, const std::string& scope, VoidCallback cb);
    void fetch_groups(const GroupsRequestBuilder& request, ResultCallback<std::vector<Group>> cb);
    void fetch_group_members(const GroupMembersRequestBuilder& request, ResultCallback<std::vector<GroupMember>> cb);
    void fetch_banned_group_members(const BannedGroupMembersRequestBuilder& request, ResultCallback<std::vector<GroupMember>> cb);

    void send_group_message(const std::string& guid, const std::string& text,
                            ResultCallback<Message> cb);
    void get_group_messages(const std::string& guid, int limit,
                            ResultCallback<std::vector<Message>> cb);
    void get_group_messages(const std::string& guid, int limit,
                            int64_t before_message_id,
                            ResultCallback<PaginatedMessages> cb);

    // --- Receipts & Typing ---
    void mark_as_read(const Message& message, VoidCallback cb);
    void mark_as_delivered(const Message& message, VoidCallback cb);
    void mark_as_unread(const Message& message, ResultCallback<Conversation> cb);
    void mark_conversation_as_read(const std::string& conversation_with, const std::string& conversation_type, VoidCallback cb);
    void start_typing(const TypingIndicator& indicator);
    void end_typing(const TypingIndicator& indicator);

    // --- Reactions ---
    void add_reaction(int64_t message_id, const std::string& reaction, ResultCallback<Message> cb);
    void remove_reaction(int64_t message_id, const std::string& reaction, ResultCallback<Message> cb);
    void fetch_reactions(const ReactionsRequestBuilder& request, ResultCallback<std::vector<Reaction>> cb);

    // --- Moderation ---
    void flag_message(int64_t message_id, const FlagDetail& flag_detail, VoidCallback cb);
    void get_flag_reasons(ResultCallback<std::vector<FlagReason>> cb);

    // --- Calls ---
    void initiate_call(const Call& call, ResultCallback<Call> cb);
    void accept_call(const std::string& session_id, ResultCallback<Call> cb);
    void reject_call(const std::string& session_id, ResultCallback<Call> cb);
    void end_call(const std::string& session_id, ResultCallback<Call> cb);

    // ============================================================
    // Listener Registration (string-based IDs, matching Android SDK)
    // ============================================================

    // --- MessageListener ---
    void add_message_listener(const std::string& listener_id, const MessageListenerCallbacks& callbacks);
    void remove_message_listener(const std::string& listener_id);

    // --- UserListener ---
    void add_user_listener(const std::string& listener_id, const UserListenerCallbacks& callbacks);
    void remove_user_listener(const std::string& listener_id);

    // --- GroupListener ---
    void add_group_listener(const std::string& listener_id, const GroupListenerCallbacks& callbacks);
    void remove_group_listener(const std::string& listener_id);

    // --- CallListener ---
    void add_call_listener(const std::string& listener_id, const CallListenerCallbacks& callbacks);
    void remove_call_listener(const std::string& listener_id);

    // --- ConnectionListener ---
    void add_connection_listener(const std::string& listener_id, const ConnectionListenerCallbacks& callbacks);
    void remove_connection_listener(const std::string& listener_id);

    // --- LoginListener ---
    void add_login_listener(const std::string& listener_id, const LoginListenerCallbacks& callbacks);
    void remove_login_listener(const std::string& listener_id);

    // --- AIAssistantListener ---
    void add_ai_assistant_listener(const std::string& listener_id, OnAIAssistantEventReceived callback);
    void remove_ai_assistant_listener(const std::string& listener_id);

    // --- Legacy single-handler listeners (backward compat) ---
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
