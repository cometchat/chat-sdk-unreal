#pragma once
// Public stable domain models. Engine-agnostic, std:: only.
// These types cross the SDK boundary — keep them POD-like and stable.

#include <string>
#include <vector>
#include <cstdint>

namespace chatsdk {

// ============================================================
// Core Models
// ============================================================

struct User {
    std::string uid;
    std::string name;
    std::string avatar_url;
    std::string status;          // "online", "offline"
    std::string role;
    std::string metadata;        // JSON string
    std::string link;
    std::string status_message;
    int64_t last_active_at = 0;
    bool has_blocked_me = false;
    bool blocked_by_me = false;
    std::vector<std::string> tags;
    int64_t deactivated_at = 0;
};

struct GroupMember {
    User user;
    std::string scope;           // "admin", "moderator", "participant"
    int64_t joined_at = 0;
};

struct Group {
    std::string guid;
    std::string name;
    std::string description;
    std::string type;            // "public", "private", "password"
    std::string password;
    std::string icon;
    std::string owner;
    std::string metadata;        // JSON string
    int64_t created_at = 0;
    int64_t updated_at = 0;
    bool has_joined = false;
    int64_t joined_at = 0;
    std::string scope;           // logged-in user's scope in this group
    int members_count = 0;
    std::vector<std::string> tags;
    bool is_banned_from_group = false;
    std::vector<std::string> member_ids; // kept for backward compat
};

struct Attachment {
    std::string extension;
    std::string mime_type;
    std::string name;
    int64_t size = 0;
    std::string url;
};

struct ReactionCount {
    std::string reaction;
    int count = 0;
    bool reacted_by_me = false;
};

struct Message {
    std::string id;
    std::string muid;
    std::string sender_uid;
    std::string receiver_uid;
    std::string text;
    int64_t sent_at = 0;
    std::string type;             // "text", "image", "video", "audio", "file", "custom"
    std::string category;         // "message", "action", "call", "custom", "interactive", "agentic"
    std::string receiver_type;    // "user" or "group"
    std::string conversation_id;
    std::string sender_name;
    std::string sender_avatar;
    int64_t updated_at = 0;
    std::string metadata;         // JSON string
    int64_t delivered_at = 0;
    int64_t read_at = 0;
    int64_t read_by_me_at = 0;
    int64_t delivered_to_me_at = 0;
    int64_t deleted_at = 0;
    int64_t edited_at = 0;
    std::string deleted_by;
    std::string edited_by;
    int64_t parent_message_id = 0;
    int reply_count = 0;
    int unread_replies_count = 0;
    std::vector<User> mentioned_users;
    bool has_mentioned_me = false;
    std::vector<ReactionCount> reactions;
    // Attachment (for media messages)
    Attachment attachment;
    std::string caption;
    // Custom message data
    std::string custom_data;      // JSON string
    std::string sub_type;
    // Tags
    std::vector<std::string> tags;
    // Moderation
    std::string moderation_status; // "unmoderated", "pending", "approved", "disapproved"
    // Raw JSON
    std::string raw_message;      // JSON string
};

struct Action {
    Message base_message;
    std::string action;
    User action_by;
    User action_on;
    std::string action_for;       // group guid or user uid
    std::string old_scope;
    std::string new_scope;
};

struct Conversation {
    std::string conversation_id;
    std::string conversation_type; // "user" or "group"
    Message last_message;
    User conversation_with_user;
    Group conversation_with_group;
    int unread_message_count = 0;
    int64_t updated_at = 0;
    std::vector<std::string> tags;
    int unread_mentions_count = 0;
    int64_t last_read_message_id = 0;
    int64_t latest_message_id = 0;
};

struct TransientMessage {
    std::string receiver_id;
    std::string receiver_type;    // "user" or "group"
    std::string data;             // JSON string
    User sender;
};

// ============================================================
// Call Models
// ============================================================

struct Call {
    Message base_message;
    std::string session_id;
    std::string call_status;      // "initiated", "ongoing", "unanswered", "rejected", "busy", "cancelled", "ended"
    std::string action;
    std::string raw_data;         // JSON string
    int64_t initiated_at = 0;
    int64_t joined_at = 0;
    User call_initiator;
    User call_receiver_user;
    Group call_receiver_group;
};

struct AudioMode {
    std::string mode;
    bool is_selected = false;
};

// ============================================================
// Reactions
// ============================================================

struct Reaction {
    std::string reaction;
    User reacted_by;
    int64_t reacted_at = 0;
};

struct ReactionEvent {
    std::string reaction;
    std::string receiver_id;
    std::string receiver_type;
    std::string conversation_id;
    int64_t parent_message_id = 0;
    Message message;
};

// ============================================================
// Receipts & Typing
// ============================================================

struct MessageReceipt {
    std::string message_id;
    User sender;
    std::string receiver_type;
    std::string receiver_id;
    int64_t timestamp = 0;
    std::string receipt_type;     // "delivered", "read"
    int64_t delivered_at = 0;
    int64_t read_at = 0;
};

struct TypingIndicator {
    std::string receiver_id;
    std::string receiver_type;    // "user" or "group"
    std::string metadata;         // JSON string
    User sender;
};

// ============================================================
// Interactive Messages
// ============================================================

struct InteractionReceipt {
    std::string message_id;
    User sender;
    std::string receiver_type;
    std::string receiver_id;
    std::string element_id;
    int64_t interacted_at = 0;
};

// ============================================================
// Moderation
// ============================================================

struct FlagDetail {
    std::string reason_id;
    std::string remark;
};

struct FlagReason {
    std::string id;
    std::string name;
    std::string description;
    int64_t created_at = 0;
    int64_t updated_at = 0;
};

enum class ModerationStatus {
    Unmoderated,
    Pending,
    Approved,
    Disapproved
};

// ============================================================
// AI / Agentic
// ============================================================

struct AIAssistantMessage {
    Message base_message;
};

struct AIToolArgumentMessage {
    Message base_message;
};

struct AIToolResultMessage {
    Message base_message;
};

enum class AIAssistantEventType {
    ToolStarted,
    ToolEnded,
    ToolResult,
    ContentReceived,
    RunStarted,
    RunFinished
};

struct AIAssistantEvent {
    AIAssistantEventType type;
    std::string data;             // JSON string with event-specific payload
    std::string conversation_id;
    std::string sender_uid;
};

// ============================================================
// Pagination
// ============================================================

struct Pagination {
    int total = 0;
    int count = 0;
    int per_page = 0;
    int current_page = 0;
    int total_pages = 0;
    bool has_more = false;
    int64_t next_cursor = 0;
};

struct PaginatedMessages {
    std::vector<Message> messages;
    Pagination pagination;
};

// ============================================================
// Presence (kept for backward compat)
// ============================================================

enum class PresenceStatus { Online, Offline, Away };

struct Presence {
    std::string uid;
    PresenceStatus status = PresenceStatus::Offline;
    int64_t last_active_at = 0;
};

// ============================================================
// Connection
// ============================================================

enum class ConnectionState { Connected, Connecting, Disconnected, FeatureThrottled };

// ============================================================
// AppSettings
// ============================================================

enum class PresenceSubscriptionType {
    None,
    AllUsers,
    Roles,
    Friends
};

struct AppSettings {
    std::string region;
    PresenceSubscriptionType subscription_type = PresenceSubscriptionType::None;
    std::vector<std::string> roles;
    std::string admin_host;
    std::string client_host;
    bool auto_establish_socket_connection = true;
};

// ============================================================
// Errors
// ============================================================

struct CometChatError {
    std::string code;
    std::string message;
};

} // namespace chatsdk
