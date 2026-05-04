#pragma once
// Public stable domain models. Engine-agnostic, std:: only.
// These types cross the SDK boundary — keep them POD-like and stable.

#include <string>
#include <vector>

namespace chatsdk {

struct User {
    std::string uid;
    std::string name;
    std::string avatar_url;
    std::string status;
};

struct Message {
    std::string id;
    std::string sender_uid;
    std::string receiver_uid;
    std::string text;
    int64_t sent_at = 0;
    std::string type;             // "text", "image", "file", "custom", etc.
    std::string category;         // "message", "action", "call", "custom"
    std::string receiver_type;    // "user" or "group"
    std::string conversation_id;  // e.g. "group_1730120681927"
    std::string sender_name;      // denormalized sender display name
    std::string sender_avatar;    // denormalized sender avatar URL
    int64_t updated_at = 0;
};

struct Group {
    std::string guid;
    std::string name;
    std::string description;
    std::vector<std::string> member_ids;
};

struct Pagination {
    int total = 0;           // total messages available
    int count = 0;           // messages returned in this page
    int per_page = 0;        // page size requested
    int current_page = 0;    // current page number
    int total_pages = 0;     // total pages available
    bool has_more = false;   // convenience: are there more pages?
    int64_t next_cursor = 0; // message ID to pass as before_message_id for next page
};

struct PaginatedMessages {
    std::vector<Message> messages;
    Pagination pagination;
};

enum class PresenceStatus { Online, Offline, Away };

struct Presence {
    std::string uid;
    PresenceStatus status = PresenceStatus::Offline;
    int64_t last_active_at = 0;
};

} // namespace chatsdk
