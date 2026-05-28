#pragma once

#include "CometChatSubsystem.h"
#include "chatsdk/types.h"
#include "chatsdk/delegates.h"

namespace CometChatConversions
{
    // ============================================================
    // User
    // ============================================================

    inline FCometChatUser ToUnreal(const chatsdk::User& u)
    {
        FCometChatUser out;
        out.Uid = UTF8_TO_TCHAR(u.uid.c_str());
        out.Name = UTF8_TO_TCHAR(u.name.c_str());
        out.AvatarUrl = UTF8_TO_TCHAR(u.avatar_url.c_str());
        out.Status = UTF8_TO_TCHAR(u.status.c_str());
        out.Role = UTF8_TO_TCHAR(u.role.c_str());
        out.Metadata = UTF8_TO_TCHAR(u.metadata.c_str());
        out.Link = UTF8_TO_TCHAR(u.link.c_str());
        out.StatusMessage = UTF8_TO_TCHAR(u.status_message.c_str());
        out.LastActiveAt = u.last_active_at;
        out.bHasBlockedMe = u.has_blocked_me;
        out.bBlockedByMe = u.blocked_by_me;
        for (const auto& tag : u.tags) { out.Tags.Add(UTF8_TO_TCHAR(tag.c_str())); }
        out.DeactivatedAt = u.deactivated_at;
        return out;
    }

    inline chatsdk::User FromUnreal(const FCometChatUser& u)
    {
        chatsdk::User out;
        out.uid = TCHAR_TO_UTF8(*u.Uid);
        out.name = TCHAR_TO_UTF8(*u.Name);
        out.avatar_url = TCHAR_TO_UTF8(*u.AvatarUrl);
        out.status = TCHAR_TO_UTF8(*u.Status);
        out.role = TCHAR_TO_UTF8(*u.Role);
        out.metadata = TCHAR_TO_UTF8(*u.Metadata);
        out.link = TCHAR_TO_UTF8(*u.Link);
        out.status_message = TCHAR_TO_UTF8(*u.StatusMessage);
        out.last_active_at = u.LastActiveAt;
        out.has_blocked_me = u.bHasBlockedMe;
        out.blocked_by_me = u.bBlockedByMe;
        for (const auto& tag : u.Tags) { out.tags.push_back(TCHAR_TO_UTF8(*tag)); }
        out.deactivated_at = u.DeactivatedAt;
        return out;
    }

    // ============================================================
    // GroupMember
    // ============================================================

    inline FCometChatGroupMember ToUnreal(const chatsdk::GroupMember& gm)
    {
        FCometChatGroupMember out;
        out.User = ToUnreal(gm.user);
        out.Scope = UTF8_TO_TCHAR(gm.scope.c_str());
        out.JoinedAt = gm.joined_at;
        return out;
    }

    // ============================================================
    // Group
    // ============================================================

    inline FCometChatGroup ToUnreal(const chatsdk::Group& g)
    {
        FCometChatGroup out;
        out.Guid = UTF8_TO_TCHAR(g.guid.c_str());
        out.Name = UTF8_TO_TCHAR(g.name.c_str());
        out.Description = UTF8_TO_TCHAR(g.description.c_str());
        out.Type = UTF8_TO_TCHAR(g.type.c_str());
        out.Icon = UTF8_TO_TCHAR(g.icon.c_str());
        out.Owner = UTF8_TO_TCHAR(g.owner.c_str());
        out.Metadata = UTF8_TO_TCHAR(g.metadata.c_str());
        out.CreatedAt = g.created_at;
        out.UpdatedAt = g.updated_at;
        out.bHasJoined = g.has_joined;
        out.JoinedAt = g.joined_at;
        out.Scope = UTF8_TO_TCHAR(g.scope.c_str());
        out.MembersCount = g.members_count;
        for (const auto& tag : g.tags) { out.Tags.Add(UTF8_TO_TCHAR(tag.c_str())); }
        out.bIsBannedFromGroup = g.is_banned_from_group;
        return out;
    }

    // ============================================================
    // Attachment
    // ============================================================

    inline FCometChatAttachment ToUnreal(const chatsdk::Attachment& a)
    {
        FCometChatAttachment out;
        out.Extension = UTF8_TO_TCHAR(a.extension.c_str());
        out.MimeType = UTF8_TO_TCHAR(a.mime_type.c_str());
        out.Name = UTF8_TO_TCHAR(a.name.c_str());
        out.Size = a.size;
        out.Url = UTF8_TO_TCHAR(a.url.c_str());
        return out;
    }

    // ============================================================
    // ReactionCount
    // ============================================================

    inline FCometChatReactionCount ToUnreal(const chatsdk::ReactionCount& r)
    {
        FCometChatReactionCount out;
        out.Reaction = UTF8_TO_TCHAR(r.reaction.c_str());
        out.Count = r.count;
        out.bReactedByMe = r.reacted_by_me;
        return out;
    }

    // ============================================================
    // Message
    // ============================================================

    inline ECometChatModerationStatus ToUnrealModerationStatus(const std::string& s)
    {
        if (s == "pending") return ECometChatModerationStatus::Pending;
        if (s == "approved") return ECometChatModerationStatus::Approved;
        if (s == "disapproved") return ECometChatModerationStatus::Disapproved;
        return ECometChatModerationStatus::Unmoderated;
    }

    inline FCometChatMessage ToUnreal(const chatsdk::Message& m)
    {
        FCometChatMessage out;
        out.Id = UTF8_TO_TCHAR(m.id.c_str());
        out.Muid = UTF8_TO_TCHAR(m.muid.c_str());
        out.SenderUid = UTF8_TO_TCHAR(m.sender_uid.c_str());
        out.ReceiverUid = UTF8_TO_TCHAR(m.receiver_uid.c_str());
        out.Text = UTF8_TO_TCHAR(m.text.c_str());
        out.SentAt = m.sent_at;
        out.Type = UTF8_TO_TCHAR(m.type.c_str());
        out.Category = UTF8_TO_TCHAR(m.category.c_str());
        out.ReceiverType = UTF8_TO_TCHAR(m.receiver_type.c_str());
        out.ConversationId = UTF8_TO_TCHAR(m.conversation_id.c_str());
        out.SenderName = UTF8_TO_TCHAR(m.sender_name.c_str());
        out.SenderAvatar = UTF8_TO_TCHAR(m.sender_avatar.c_str());
        out.UpdatedAt = m.updated_at;
        out.Metadata = UTF8_TO_TCHAR(m.metadata.c_str());
        out.DeliveredAt = m.delivered_at;
        out.ReadAt = m.read_at;
        out.DeletedAt = m.deleted_at;
        out.EditedAt = m.edited_at;
        out.DeletedBy = UTF8_TO_TCHAR(m.deleted_by.c_str());
        out.EditedBy = UTF8_TO_TCHAR(m.edited_by.c_str());
        out.ParentMessageId = m.parent_message_id;
        out.ReplyCount = m.reply_count;
        out.UnreadRepliesCount = m.unread_replies_count;
        for (const auto& u : m.mentioned_users) { out.MentionedUsers.Add(ToUnreal(u)); }
        out.bHasMentionedMe = m.has_mentioned_me;
        for (const auto& r : m.reactions) { out.Reactions.Add(ToUnreal(r)); }
        out.Attachment = ToUnreal(m.attachment);
        out.Caption = UTF8_TO_TCHAR(m.caption.c_str());
        out.CustomData = UTF8_TO_TCHAR(m.custom_data.c_str());
        out.SubType = UTF8_TO_TCHAR(m.sub_type.c_str());
        for (const auto& tag : m.tags) { out.Tags.Add(UTF8_TO_TCHAR(tag.c_str())); }
        out.ModerationStatus = ToUnrealModerationStatus(m.moderation_status);
        return out;
    }

    // ============================================================
    // Action
    // ============================================================

    inline FCometChatAction ToUnreal(const chatsdk::Action& a)
    {
        FCometChatAction out;
        out.BaseMessage = ToUnreal(a.base_message);
        out.Action = UTF8_TO_TCHAR(a.action.c_str());
        out.ActionBy = ToUnreal(a.action_by);
        out.ActionOn = ToUnreal(a.action_on);
        out.ActionFor = UTF8_TO_TCHAR(a.action_for.c_str());
        out.OldScope = UTF8_TO_TCHAR(a.old_scope.c_str());
        out.NewScope = UTF8_TO_TCHAR(a.new_scope.c_str());
        return out;
    }

    // ============================================================
    // Conversation
    // ============================================================

    inline FCometChatConversation ToUnreal(const chatsdk::Conversation& c)
    {
        FCometChatConversation out;
        out.ConversationId = UTF8_TO_TCHAR(c.conversation_id.c_str());
        out.ConversationType = UTF8_TO_TCHAR(c.conversation_type.c_str());
        out.LastMessage = ToUnreal(c.last_message);
        out.ConversationWithUser = ToUnreal(c.conversation_with_user);
        out.ConversationWithGroup = ToUnreal(c.conversation_with_group);
        out.UnreadMessageCount = c.unread_message_count;
        out.UpdatedAt = c.updated_at;
        for (const auto& tag : c.tags) { out.Tags.Add(UTF8_TO_TCHAR(tag.c_str())); }
        out.UnreadMentionsCount = c.unread_mentions_count;
        out.LastReadMessageId = c.last_read_message_id;
        out.LatestMessageId = c.latest_message_id;
        return out;
    }

    // ============================================================
    // Call
    // ============================================================

    inline FCometChatCall ToUnreal(const chatsdk::Call& c)
    {
        FCometChatCall out;
        out.BaseMessage = ToUnreal(c.base_message);
        out.SessionId = UTF8_TO_TCHAR(c.session_id.c_str());
        out.CallStatus = UTF8_TO_TCHAR(c.call_status.c_str());
        out.Action = UTF8_TO_TCHAR(c.action.c_str());
        out.InitiatedAt = c.initiated_at;
        out.JoinedAt = c.joined_at;
        out.CallInitiator = ToUnreal(c.call_initiator);
        out.CallReceiverUser = ToUnreal(c.call_receiver_user);
        out.CallReceiverGroup = ToUnreal(c.call_receiver_group);
        return out;
    }

    // ============================================================
    // TransientMessage
    // ============================================================

    inline FCometChatTransientMessage ToUnreal(const chatsdk::TransientMessage& t)
    {
        FCometChatTransientMessage out;
        out.ReceiverId = UTF8_TO_TCHAR(t.receiver_id.c_str());
        out.ReceiverType = UTF8_TO_TCHAR(t.receiver_type.c_str());
        out.Data = UTF8_TO_TCHAR(t.data.c_str());
        out.Sender = ToUnreal(t.sender);
        return out;
    }

    // ============================================================
    // MessageReceipt
    // ============================================================

    inline FCometChatMessageReceipt ToUnreal(const chatsdk::MessageReceipt& r)
    {
        FCometChatMessageReceipt out;
        out.MessageId = UTF8_TO_TCHAR(r.message_id.c_str());
        out.Sender = ToUnreal(r.sender);
        out.ReceiverType = UTF8_TO_TCHAR(r.receiver_type.c_str());
        out.ReceiverId = UTF8_TO_TCHAR(r.receiver_id.c_str());
        out.Timestamp = r.timestamp;
        out.ReceiptType = UTF8_TO_TCHAR(r.receipt_type.c_str());
        out.DeliveredAt = r.delivered_at;
        out.ReadAt = r.read_at;
        return out;
    }

    // ============================================================
    // TypingIndicator
    // ============================================================

    inline FCometChatTypingIndicator ToUnreal(const chatsdk::TypingIndicator& t)
    {
        FCometChatTypingIndicator out;
        out.ReceiverId = UTF8_TO_TCHAR(t.receiver_id.c_str());
        out.ReceiverType = UTF8_TO_TCHAR(t.receiver_type.c_str());
        out.Metadata = UTF8_TO_TCHAR(t.metadata.c_str());
        out.Sender = ToUnreal(t.sender);
        return out;
    }

    inline chatsdk::TypingIndicator FromUnreal(const FCometChatTypingIndicator& t)
    {
        chatsdk::TypingIndicator out;
        out.receiver_id = TCHAR_TO_UTF8(*t.ReceiverId);
        out.receiver_type = TCHAR_TO_UTF8(*t.ReceiverType);
        out.metadata = TCHAR_TO_UTF8(*t.Metadata);
        return out;
    }

    // ============================================================
    // ReactionEvent
    // ============================================================

    inline FCometChatReactionEvent ToUnreal(const chatsdk::ReactionEvent& r)
    {
        FCometChatReactionEvent out;
        out.Reaction = UTF8_TO_TCHAR(r.reaction.c_str());
        out.ReceiverId = UTF8_TO_TCHAR(r.receiver_id.c_str());
        out.ReceiverType = UTF8_TO_TCHAR(r.receiver_type.c_str());
        out.ConversationId = UTF8_TO_TCHAR(r.conversation_id.c_str());
        out.ParentMessageId = r.parent_message_id;
        out.Message = ToUnreal(r.message);
        return out;
    }

    // ============================================================
    // InteractionReceipt
    // ============================================================

    inline FCometChatInteractionReceipt ToUnreal(const chatsdk::InteractionReceipt& ir)
    {
        FCometChatInteractionReceipt out;
        out.MessageId = UTF8_TO_TCHAR(ir.message_id.c_str());
        out.Sender = ToUnreal(ir.sender);
        out.ReceiverType = UTF8_TO_TCHAR(ir.receiver_type.c_str());
        out.ReceiverId = UTF8_TO_TCHAR(ir.receiver_id.c_str());
        out.ElementId = UTF8_TO_TCHAR(ir.element_id.c_str());
        out.InteractedAt = ir.interacted_at;
        return out;
    }

    // ============================================================
    // FlagDetail / FlagReason
    // ============================================================

    inline chatsdk::FlagDetail FromUnreal(const FCometChatFlagDetail& f)
    {
        chatsdk::FlagDetail out;
        out.reason_id = TCHAR_TO_UTF8(*f.ReasonId);
        out.remark = TCHAR_TO_UTF8(*f.Remark);
        return out;
    }

    inline FCometChatFlagReason ToUnreal(const chatsdk::FlagReason& f)
    {
        FCometChatFlagReason out;
        out.Id = UTF8_TO_TCHAR(f.id.c_str());
        out.Name = UTF8_TO_TCHAR(f.name.c_str());
        out.Description = UTF8_TO_TCHAR(f.description.c_str());
        out.CreatedAt = f.created_at;
        out.UpdatedAt = f.updated_at;
        return out;
    }

    // ============================================================
    // Reaction
    // ============================================================

    inline FCometChatReaction ToUnreal(const chatsdk::Reaction& r)
    {
        FCometChatReaction out;
        out.Reaction = UTF8_TO_TCHAR(r.reaction.c_str());
        out.ReactedBy = ToUnreal(r.reacted_by);
        out.ReactedAt = r.reacted_at;
        return out;
    }

    // ============================================================
    // AIAssistantEvent
    // ============================================================

    inline FCometChatAIAssistantEvent ToUnreal(const chatsdk::AIAssistantEvent& e)
    {
        FCometChatAIAssistantEvent out;
        switch (e.type)
        {
        case chatsdk::AIAssistantEventType::ToolStarted: out.EventType = TEXT("ToolStarted"); break;
        case chatsdk::AIAssistantEventType::ToolEnded: out.EventType = TEXT("ToolEnded"); break;
        case chatsdk::AIAssistantEventType::ToolResult: out.EventType = TEXT("ToolResult"); break;
        case chatsdk::AIAssistantEventType::ContentReceived: out.EventType = TEXT("ContentReceived"); break;
        case chatsdk::AIAssistantEventType::RunStarted: out.EventType = TEXT("RunStarted"); break;
        case chatsdk::AIAssistantEventType::RunFinished: out.EventType = TEXT("RunFinished"); break;
        }
        out.Data = UTF8_TO_TCHAR(e.data.c_str());
        out.ConversationId = UTF8_TO_TCHAR(e.conversation_id.c_str());
        out.SenderUid = UTF8_TO_TCHAR(e.sender_uid.c_str());
        return out;
    }

    // ============================================================
    // Error
    // ============================================================

    inline FCometChatError ToUnreal(const chatsdk::CometChatError& e)
    {
        FCometChatError out;
        out.Code = UTF8_TO_TCHAR(e.code.c_str());
        out.Message = UTF8_TO_TCHAR(e.message.c_str());
        return out;
    }

    // ============================================================
    // Presence (legacy)
    // ============================================================

    inline ECometChatPresenceStatus ToUnreal(chatsdk::PresenceStatus s)
    {
        switch (s)
        {
        case chatsdk::PresenceStatus::Online:  return ECometChatPresenceStatus::Online;
        case chatsdk::PresenceStatus::Away:    return ECometChatPresenceStatus::Away;
        default:                               return ECometChatPresenceStatus::Offline;
        }
    }

    inline FCometChatPresence ToUnreal(const chatsdk::Presence& p)
    {
        FCometChatPresence out;
        out.Uid = UTF8_TO_TCHAR(p.uid.c_str());
        out.Status = ToUnreal(p.status);
        out.LastActiveAt = p.last_active_at;
        return out;
    }

    // ============================================================
    // ConnectionState
    // ============================================================

    inline ECometChatConnectionState ToUnreal(chatsdk::ConnectionState s)
    {
        switch (s)
        {
        case chatsdk::ConnectionState::Connected:        return ECometChatConnectionState::Connected;
        case chatsdk::ConnectionState::Connecting:       return ECometChatConnectionState::Connecting;
        case chatsdk::ConnectionState::FeatureThrottled: return ECometChatConnectionState::FeatureThrottled;
        default:                                         return ECometChatConnectionState::Disconnected;
        }
    }

    // ============================================================
    // Request Builder conversions (Unreal -> C++ SDK)
    // ============================================================

    inline chatsdk::UsersRequestBuilder FromUnreal(const FCometChatUsersRequest& r)
    {
        chatsdk::UsersRequestBuilder out;
        out.limit = r.Limit;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        for (const auto& s : r.SearchIn) { out.search_in.push_back(TCHAR_TO_UTF8(*s)); }
        out.user_status = TCHAR_TO_UTF8(*r.UserStatus);
        out.hide_blocked_users = r.bHideBlockedUsers;
        out.friends_only = r.bFriendsOnly;
        for (const auto& role : r.Roles) { out.roles.push_back(TCHAR_TO_UTF8(*role)); }
        for (const auto& tag : r.Tags) { out.tags.push_back(TCHAR_TO_UTF8(*tag)); }
        out.with_tags = r.bWithTags;
        for (const auto& uid : r.UIDs) { out.uids.push_back(TCHAR_TO_UTF8(*uid)); }
        out.sort_by = TCHAR_TO_UTF8(*r.SortBy);
        out.sort_order = TCHAR_TO_UTF8(*r.SortOrder);
        out.page = r.Page;
        return out;
    }

    inline chatsdk::GroupsRequestBuilder FromUnreal(const FCometChatGroupsRequest& r)
    {
        chatsdk::GroupsRequestBuilder out;
        out.limit = r.Limit;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        out.joined_only = r.bJoinedOnly;
        for (const auto& tag : r.Tags) { out.tags.push_back(TCHAR_TO_UTF8(*tag)); }
        out.with_tags = r.bWithTags;
        out.page = r.Page;
        return out;
    }

    inline chatsdk::MessagesRequestBuilder FromUnreal(const FCometChatMessagesRequest& r)
    {
        chatsdk::MessagesRequestBuilder out;
        out.limit = r.Limit;
        out.uid = TCHAR_TO_UTF8(*r.UID);
        out.guid = TCHAR_TO_UTF8(*r.GUID);
        out.message_id = r.MessageId;
        out.timestamp = r.Timestamp;
        out.unread = r.bUnread;
        out.hide_messages_from_blocked_users = r.bHideMessagesFromBlockedUsers;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        out.updated_after = r.UpdatedAfter;
        out.updates_only = r.bUpdatesOnly;
        for (const auto& c : r.Categories) { out.categories.push_back(TCHAR_TO_UTF8(*c)); }
        for (const auto& t : r.Types) { out.types.push_back(TCHAR_TO_UTF8(*t)); }
        out.parent_message_id = r.ParentMessageId;
        out.hide_replies = r.bHideReplies;
        out.hide_deleted = r.bHideDeleted;
        for (const auto& tag : r.Tags) { out.tags.push_back(TCHAR_TO_UTF8(*tag)); }
        out.with_tags = r.bWithTags;
        out.mentions_with_tag_info = r.bMentionsWithTagInfo;
        out.mentions_with_blocked_info = r.bMentionsWithBlockedInfo;
        out.interaction_goal_completed_only = r.bInteractionGoalCompletedOnly;
        out.has_attachments = r.bHasAttachments;
        out.has_links = r.bHasLinks;
        out.has_mentions = r.bHasMentions;
        out.has_reactions = r.bHasReactions;
        for (const auto& uid : r.MentionedUIDs) { out.mentioned_uids.push_back(TCHAR_TO_UTF8(*uid)); }
        out.with_parent = r.bWithParent;
        out.hide_quoted_messages = r.bHideQuotedMessages;
        return out;
    }

    inline chatsdk::ConversationsRequestBuilder FromUnreal(const FCometChatConversationsRequest& r)
    {
        chatsdk::ConversationsRequestBuilder out;
        out.limit = r.Limit;
        out.conversation_type = TCHAR_TO_UTF8(*r.ConversationType);
        out.with_user_and_group_tags = r.bWithUserAndGroupTags;
        for (const auto& tag : r.Tags) { out.tags.push_back(TCHAR_TO_UTF8(*tag)); }
        out.with_tags = r.bWithTags;
        for (const auto& tag : r.UserTags) { out.user_tags.push_back(TCHAR_TO_UTF8(*tag)); }
        for (const auto& tag : r.GroupTags) { out.group_tags.push_back(TCHAR_TO_UTF8(*tag)); }
        out.include_blocked_users = r.bIncludeBlockedUsers;
        out.with_blocked_info = r.bWithBlockedInfo;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        out.unread = r.bUnread;
        out.page = r.Page;
        out.hide_agentic = r.bHideAgentic;
        out.only_agentic = r.bOnlyAgentic;
        return out;
    }

    inline chatsdk::GroupMembersRequestBuilder FromUnreal(const FCometChatGroupMembersRequest& r)
    {
        chatsdk::GroupMembersRequestBuilder out;
        out.guid = TCHAR_TO_UTF8(*r.GUID);
        out.limit = r.Limit;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        for (const auto& s : r.Scopes) { out.scopes.push_back(TCHAR_TO_UTF8(*s)); }
        out.status = TCHAR_TO_UTF8(*r.Status);
        out.page = r.Page;
        return out;
    }

    inline chatsdk::BannedGroupMembersRequestBuilder FromUnreal(const FCometChatBannedMembersRequest& r)
    {
        chatsdk::BannedGroupMembersRequestBuilder out;
        out.guid = TCHAR_TO_UTF8(*r.GUID);
        out.limit = r.Limit;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        for (const auto& s : r.Scopes) { out.scopes.push_back(TCHAR_TO_UTF8(*s)); }
        out.page = r.Page;
        return out;
    }

    inline chatsdk::BlockedUsersRequestBuilder FromUnreal(const FCometChatBlockedUsersRequest& r)
    {
        chatsdk::BlockedUsersRequestBuilder out;
        out.limit = r.Limit;
        out.search_keyword = TCHAR_TO_UTF8(*r.SearchKeyword);
        out.direction = TCHAR_TO_UTF8(*r.Direction);
        out.page = r.Page;
        return out;
    }

    inline chatsdk::ReactionsRequestBuilder FromUnreal(const FCometChatReactionsRequest& r)
    {
        chatsdk::ReactionsRequestBuilder out;
        out.limit = r.Limit;
        out.message_id = r.MessageId;
        out.reaction = TCHAR_TO_UTF8(*r.Reaction);
        return out;
    }
}
