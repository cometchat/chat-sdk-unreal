#pragma once
// Callback interfaces for SDK events.
// Consumers register these to receive async notifications.
// Engine-agnostic — no Unreal types here.

#include "chatsdk/types.h"
#include <functional>
#include <string>
#include <vector>

namespace chatsdk {

// ============================================================
// Result callback pattern
// ============================================================

template <typename T>
using ResultCallback = std::function<void(const std::string& error, const T& result)>;
using VoidCallback = std::function<void(const std::string& error)>;

// ============================================================
// MessageListener callbacks
// ============================================================

using OnTextMessageReceived = std::function<void(const Message&)>;
using OnMediaMessageReceived = std::function<void(const Message&)>;
using OnCustomMessageReceived = std::function<void(const Message&)>;
using OnInteractiveMessageReceived = std::function<void(const Message&)>;
using OnInteractionGoalCompleted = std::function<void(const InteractionReceipt&)>;
using OnTypingStarted = std::function<void(const TypingIndicator&)>;
using OnTypingEnded = std::function<void(const TypingIndicator&)>;
using OnMessagesDelivered = std::function<void(const MessageReceipt&)>;
using OnMessagesRead = std::function<void(const MessageReceipt&)>;
using OnMessagesDeliveredToAll = std::function<void(const MessageReceipt&)>;
using OnMessagesReadByAll = std::function<void(const MessageReceipt&)>;
using OnMessageEdited = std::function<void(const Message&)>;
using OnMessageDeleted = std::function<void(const Message&)>;
using OnTransientMessageReceived = std::function<void(const TransientMessage&)>;
using OnMessageReactionAdded = std::function<void(const ReactionEvent&)>;
using OnMessageReactionRemoved = std::function<void(const ReactionEvent&)>;
using OnMessageModerated = std::function<void(const Message&)>;
using OnAIAssistantMessageReceived = std::function<void(const AIAssistantMessage&)>;
using OnAIToolResultReceived = std::function<void(const AIToolResultMessage&)>;
using OnAIToolArgumentsReceived = std::function<void(const AIToolArgumentMessage&)>;

// Legacy combined handler (kept for backward compat)
using OnMessageReceived = std::function<void(const Message&)>;

// ============================================================
// UserListener callbacks
// ============================================================

using OnUserOnline = std::function<void(const User&)>;
using OnUserOffline = std::function<void(const User&)>;

// ============================================================
// GroupListener callbacks
// ============================================================

using OnGroupMemberJoined = std::function<void(const Action&, const User&, const Group&)>;
using OnGroupMemberLeft = std::function<void(const Action&, const User&, const Group&)>;
using OnGroupMemberKicked = std::function<void(const Action&, const User& kicked_user, const User& kicked_by, const Group&)>;
using OnGroupMemberBanned = std::function<void(const Action&, const User& banned_user, const User& banned_by, const Group&)>;
using OnGroupMemberUnbanned = std::function<void(const Action&, const User& unbanned_user, const User& unbanned_by, const Group&)>;
using OnGroupMemberScopeChanged = std::function<void(const Action&, const User& updated_by, const User& updated_user, const std::string& scope_changed_to, const std::string& scope_changed_from, const Group&)>;
using OnMemberAddedToGroup = std::function<void(const Action&, const User& added_by, const User& user_added, const Group&)>;

// ============================================================
// CallListener callbacks
// ============================================================

using OnIncomingCallReceived = std::function<void(const Call&)>;
using OnOutgoingCallAccepted = std::function<void(const Call&)>;
using OnOutgoingCallRejected = std::function<void(const Call&)>;
using OnIncomingCallCancelled = std::function<void(const Call&)>;
using OnCallEndedMessageReceived = std::function<void(const Call&)>;

// ============================================================
// ConnectionListener callbacks
// ============================================================

using OnConnected = std::function<void()>;
using OnConnecting = std::function<void()>;
using OnDisconnected = std::function<void()>;
using OnFeatureThrottled = std::function<void()>;
using OnConnectionError = std::function<void(const CometChatError&)>;

// Legacy combined handler (kept for backward compat)
using OnConnectionStateChanged = std::function<void(ConnectionState)>;

// ============================================================
// LoginListener callbacks
// ============================================================

using OnLoginSuccess = std::function<void(const User&)>;
using OnLoginFailure = std::function<void(const CometChatError&)>;
using OnLogoutSuccess = std::function<void()>;
using OnLogoutFailure = std::function<void(const CometChatError&)>;

// ============================================================
// AIAssistantListener callbacks
// ============================================================

using OnAIAssistantEventReceived = std::function<void(const AIAssistantEvent&)>;

// ============================================================
// OngoingCallListener callbacks
// ============================================================

using OnCallUserJoined = std::function<void(const User&)>;
using OnCallUserLeft = std::function<void(const User&)>;
using OnCallError = std::function<void(const CometChatError&)>;
using OnCallEnded = std::function<void(const Call&)>;
using OnCallUserListUpdated = std::function<void(const std::vector<User>&)>;
using OnAudioModesUpdated = std::function<void(const std::vector<AudioMode>&)>;
using OnRecordingStarted = std::function<void(const User&)>;
using OnRecordingStopped = std::function<void(const User&)>;
using OnUserMuted = std::function<void(const User& muted_user, const User& muted_by)>;
using OnCallSwitchedToVideo = std::function<void(const std::string& session_id, const User& initiated_by, const User& accepted_by)>;

// ============================================================
// Presence (legacy — kept for backward compat)
// ============================================================

using OnPresenceChanged = std::function<void(const Presence&)>;

// Legacy typing (kept for backward compat)
struct TypingEvent {
    std::string uid;
    std::string conversation_id;
    bool is_typing = false;
};
using OnTypingChanged = std::function<void(const TypingEvent&)>;

// Legacy receipt (kept for backward compat)
struct ReceiptEvent {
    std::string message_id;
    std::string uid;
    std::string status; // "delivered", "read"
    int64_t timestamp = 0;
};
using OnReceiptReceived = std::function<void(const ReceiptEvent&)>;

// ============================================================
// Listener structs (for grouped registration)
// ============================================================

struct MessageListenerCallbacks {
    OnTextMessageReceived on_text_message_received;
    OnMediaMessageReceived on_media_message_received;
    OnCustomMessageReceived on_custom_message_received;
    OnInteractiveMessageReceived on_interactive_message_received;
    OnInteractionGoalCompleted on_interaction_goal_completed;
    OnTypingStarted on_typing_started;
    OnTypingEnded on_typing_ended;
    OnMessagesDelivered on_messages_delivered;
    OnMessagesRead on_messages_read;
    OnMessagesDeliveredToAll on_messages_delivered_to_all;
    OnMessagesReadByAll on_messages_read_by_all;
    OnMessageEdited on_message_edited;
    OnMessageDeleted on_message_deleted;
    OnTransientMessageReceived on_transient_message_received;
    OnMessageReactionAdded on_message_reaction_added;
    OnMessageReactionRemoved on_message_reaction_removed;
    OnMessageModerated on_message_moderated;
    OnAIAssistantMessageReceived on_ai_assistant_message_received;
    OnAIToolResultReceived on_ai_tool_result_received;
    OnAIToolArgumentsReceived on_ai_tool_arguments_received;
};

struct UserListenerCallbacks {
    OnUserOnline on_user_online;
    OnUserOffline on_user_offline;
};

struct GroupListenerCallbacks {
    OnGroupMemberJoined on_group_member_joined;
    OnGroupMemberLeft on_group_member_left;
    OnGroupMemberKicked on_group_member_kicked;
    OnGroupMemberBanned on_group_member_banned;
    OnGroupMemberUnbanned on_group_member_unbanned;
    OnGroupMemberScopeChanged on_group_member_scope_changed;
    OnMemberAddedToGroup on_member_added_to_group;
};

struct CallListenerCallbacks {
    OnIncomingCallReceived on_incoming_call_received;
    OnOutgoingCallAccepted on_outgoing_call_accepted;
    OnOutgoingCallRejected on_outgoing_call_rejected;
    OnIncomingCallCancelled on_incoming_call_cancelled;
    OnCallEndedMessageReceived on_call_ended_message_received;
};

struct ConnectionListenerCallbacks {
    OnConnected on_connected;
    OnConnecting on_connecting;
    OnDisconnected on_disconnected;
    OnFeatureThrottled on_feature_throttled;
    OnConnectionError on_connection_error;
};

struct LoginListenerCallbacks {
    OnLoginSuccess on_login_success;
    OnLoginFailure on_login_failure;
    OnLogoutSuccess on_logout_success;
    OnLogoutFailure on_logout_failure;
};

struct OngoingCallListenerCallbacks {
    OnCallUserJoined on_user_joined;
    OnCallUserLeft on_user_left;
    OnCallError on_error;
    OnCallEnded on_call_ended;
    OnCallUserListUpdated on_user_list_updated;
    OnAudioModesUpdated on_audio_modes_updated;
    OnRecordingStarted on_recording_started;
    OnRecordingStopped on_recording_stopped;
    OnUserMuted on_user_muted;
    OnCallSwitchedToVideo on_call_switched_to_video;
};

} // namespace chatsdk
