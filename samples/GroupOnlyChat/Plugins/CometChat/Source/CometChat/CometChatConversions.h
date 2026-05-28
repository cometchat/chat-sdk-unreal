#pragma once

#include "CometChatSubsystem.h"
#include "chatsdk/types.h"
#include "chatsdk/delegates.h"

namespace CometChatConversions
{
    inline FCometChatMessage ToUnreal(const chatsdk::Message& m)
    {
        FCometChatMessage out;
        out.Id = UTF8_TO_TCHAR(m.id.c_str());
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
        return out;
    }

    inline FCometChatUser ToUnreal(const chatsdk::User& u)
    {
        FCometChatUser out;
        out.Uid = UTF8_TO_TCHAR(u.uid.c_str());
        out.Name = UTF8_TO_TCHAR(u.name.c_str());
        out.AvatarUrl = UTF8_TO_TCHAR(u.avatar_url.c_str());
        out.Status = UTF8_TO_TCHAR(u.status.c_str());
        return out;
    }

    inline FCometChatGroup ToUnreal(const chatsdk::Group& g)
    {
        FCometChatGroup out;
        out.Guid = UTF8_TO_TCHAR(g.guid.c_str());
        out.Name = UTF8_TO_TCHAR(g.name.c_str());
        out.Description = UTF8_TO_TCHAR(g.description.c_str());
        for (const auto& id : g.member_ids)
        {
            out.MemberIds.Add(UTF8_TO_TCHAR(id.c_str()));
        }
        return out;
    }

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
        out.Uid          = UTF8_TO_TCHAR(p.uid.c_str());
        out.Status       = ToUnreal(p.status);
        out.LastActiveAt = p.last_active_at;
        return out;
    }

    inline FCometChatTypingEvent ToUnreal(const chatsdk::TypingEvent& t)
    {
        FCometChatTypingEvent out;
        out.Uid            = UTF8_TO_TCHAR(t.uid.c_str());
        out.ConversationId = UTF8_TO_TCHAR(t.conversation_id.c_str());
        out.bIsTyping      = t.is_typing;
        return out;
    }

    inline FCometChatReceiptEvent ToUnreal(const chatsdk::ReceiptEvent& r)
    {
        FCometChatReceiptEvent out;
        out.MessageId = UTF8_TO_TCHAR(r.message_id.c_str());
        out.Uid       = UTF8_TO_TCHAR(r.uid.c_str());
        out.Status    = UTF8_TO_TCHAR(r.status.c_str());
        out.Timestamp = r.timestamp;
        return out;
    }

    inline ECometChatConnectionState ToUnreal(chatsdk::ConnectionState s)
    {
        switch (s)
        {
        case chatsdk::ConnectionState::Connected:    return ECometChatConnectionState::Connected;
        case chatsdk::ConnectionState::Reconnecting: return ECometChatConnectionState::Reconnecting;
        default:                                     return ECometChatConnectionState::Disconnected;
        }
    }
}
