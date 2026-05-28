#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Ticker.h"
#include "chatsdk/cometchat.h"
#include "CometChatSubsystem.generated.h"

// ============================================================
// Enums
// ============================================================

UENUM(BlueprintType)
enum class ECometChatConnectionState : uint8
{
    Connected      UMETA(DisplayName = "Connected"),
    Connecting     UMETA(DisplayName = "Connecting"),
    Disconnected   UMETA(DisplayName = "Disconnected"),
    FeatureThrottled UMETA(DisplayName = "FeatureThrottled")
};

UENUM(BlueprintType)
enum class ECometChatPresenceStatus : uint8
{
    Online   UMETA(DisplayName = "Online"),
    Offline  UMETA(DisplayName = "Offline"),
    Away     UMETA(DisplayName = "Away")
};

UENUM(BlueprintType)
enum class ECometChatModerationStatus : uint8
{
    Unmoderated  UMETA(DisplayName = "Unmoderated"),
    Pending      UMETA(DisplayName = "Pending"),
    Approved     UMETA(DisplayName = "Approved"),
    Disapproved  UMETA(DisplayName = "Disapproved")
};

// ============================================================
// Blueprint-friendly mirror types
// ============================================================

USTRUCT(BlueprintType)
struct FCometChatUser
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Uid;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString AvatarUrl;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Status;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Role;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Metadata;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Link;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString StatusMessage;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 LastActiveAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasBlockedMe = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bBlockedByMe = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 DeactivatedAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatGroupMember
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser User;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Scope;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 JoinedAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatGroup
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Guid;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Type;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Icon;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Owner;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Metadata;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 CreatedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 UpdatedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasJoined = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 JoinedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Scope;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 MembersCount = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bIsBannedFromGroup = false;
};

USTRUCT(BlueprintType)
struct FCometChatAttachment
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Extension;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString MimeType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 Size = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Url;
};

USTRUCT(BlueprintType)
struct FCometChatReactionCount
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Reaction;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Count = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bReactedByMe = false;
};


USTRUCT(BlueprintType)
struct FCometChatMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Id;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Muid;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SenderUid;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverUid;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Text;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 SentAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Type;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Category;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ConversationId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SenderName;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SenderAvatar;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 UpdatedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Metadata;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 DeliveredAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 ReadAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 DeletedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 EditedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString DeletedBy;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString EditedBy;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 ParentMessageId = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 ReplyCount = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 UnreadRepliesCount = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FCometChatUser> MentionedUsers;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasMentionedMe = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FCometChatReactionCount> Reactions;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatAttachment Attachment;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Caption;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString CustomData;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SubType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    ECometChatModerationStatus ModerationStatus = ECometChatModerationStatus::Unmoderated;
};

USTRUCT(BlueprintType)
struct FCometChatAction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatMessage BaseMessage;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Action;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser ActionBy;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser ActionOn;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ActionFor;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString OldScope;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString NewScope;
};

USTRUCT(BlueprintType)
struct FCometChatConversation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ConversationId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ConversationType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatMessage LastMessage;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser ConversationWithUser;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatGroup ConversationWithGroup;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 UnreadMessageCount = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 UpdatedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 UnreadMentionsCount = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 LastReadMessageId = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 LatestMessageId = 0;
};

USTRUCT(BlueprintType)
struct FCometChatCall
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatMessage BaseMessage;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SessionId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString CallStatus;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Action;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 InitiatedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 JoinedAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser CallInitiator;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser CallReceiverUser;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatGroup CallReceiverGroup;
};

USTRUCT(BlueprintType)
struct FCometChatTransientMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Data;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser Sender;
};

USTRUCT(BlueprintType)
struct FCometChatMessageReceipt
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString MessageId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser Sender;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 Timestamp = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiptType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 DeliveredAt = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 ReadAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatTypingIndicator
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Metadata;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser Sender;
};

USTRUCT(BlueprintType)
struct FCometChatReactionEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Reaction;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ConversationId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 ParentMessageId = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatMessage Message;
};

USTRUCT(BlueprintType)
struct FCometChatInteractionReceipt
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString MessageId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FCometChatUser Sender;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReceiverId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ElementId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 InteractedAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatFlagDetail
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ReasonId;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Remark;
};

USTRUCT(BlueprintType)
struct FCometChatFlagReason
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Id;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Description;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 CreatedAt = 0;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 UpdatedAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatReaction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Reaction;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FCometChatUser ReactedBy;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 ReactedAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatAIAssistantEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString EventType;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Data;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ConversationId;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString SenderUid;
};

USTRUCT(BlueprintType)
struct FCometChatError
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Code;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Message;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Details;
};

// Legacy structs kept for backward compat
USTRUCT(BlueprintType)
struct FCometChatPresence
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Uid;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    ECometChatPresenceStatus Status = ECometChatPresenceStatus::Offline;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 LastActiveAt = 0;
};

USTRUCT(BlueprintType)
struct FCometChatPagination
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int32 Total = 0;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int32 Count = 0;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int32 PerPage = 0;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int32 CurrentPage = 0;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int32 TotalPages = 0;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    bool HasMore = false;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int32 NextCursor = 0;
};


// ============================================================
// Request Builder Structs (for search, pagination, filters)
// ============================================================

USTRUCT(BlueprintType)
struct FCometChatUsersRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> SearchIn;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString UserStatus;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHideBlockedUsers = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bFriendsOnly = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Roles;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithTags = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> UIDs;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SortBy;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SortOrder;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Page = 0;
};

USTRUCT(BlueprintType)
struct FCometChatGroupsRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bJoinedOnly = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithTags = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Page = 0;
};

USTRUCT(BlueprintType)
struct FCometChatMessagesRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString UID;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString GUID;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 MessageId = -1;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 Timestamp = -1;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bUnread = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHideMessagesFromBlockedUsers = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 UpdatedAfter = -1;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bUpdatesOnly = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Categories;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Types;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 ParentMessageId = -1;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHideReplies = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHideDeleted = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithTags = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bMentionsWithTagInfo = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bMentionsWithBlockedInfo = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bInteractionGoalCompletedOnly = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasAttachments = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasLinks = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasMentions = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHasReactions = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> MentionedUIDs;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithParent = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHideQuotedMessages = false;
};

USTRUCT(BlueprintType)
struct FCometChatConversationsRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ConversationType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithUserAndGroupTags = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithTags = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> UserTags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> GroupTags;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bIncludeBlockedUsers = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bWithBlockedInfo = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bUnread = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Page = 0;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bHideAgentic = false;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bOnlyAgentic = false;
};

USTRUCT(BlueprintType)
struct FCometChatGroupMembersRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString GUID;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Scopes;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Status;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Page = 0;
};

USTRUCT(BlueprintType)
struct FCometChatBannedMembersRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString GUID;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Scopes;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Page = 0;
};

USTRUCT(BlueprintType)
struct FCometChatBlockedUsersRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 30;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SearchKeyword;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Direction;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Page = 0;
};

USTRUCT(BlueprintType)
struct FCometChatReactionsRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int32 Limit = 10;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    int64 MessageId = -1;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Reaction;
};

USTRUCT(BlueprintType)
struct FCometChatAppSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString Region;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString SubscriptionType;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    TArray<FString> Roles;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString AdminHost;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    FString ClientHost;

    UPROPERTY(BlueprintReadWrite, Category = "CometChat")
    bool bAutoEstablishSocketConnection = true;
};


// ============================================================
// Delegates for async callbacks and real-time events
// ============================================================

// --- MessageListener delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatTextMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMediaMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatCustomMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatInteractiveMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatInteractionGoalCompleted, const FCometChatInteractionReceipt&, Receipt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatTypingStarted, const FCometChatTypingIndicator&, Indicator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatTypingEnded, const FCometChatTypingIndicator&, Indicator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessagesDelivered, const FCometChatMessageReceipt&, Receipt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessagesRead, const FCometChatMessageReceipt&, Receipt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessagesDeliveredToAll, const FCometChatMessageReceipt&, Receipt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessagesReadByAll, const FCometChatMessageReceipt&, Receipt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageEdited, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageDeleted, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatTransientMessageReceived, const FCometChatTransientMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageReactionAdded, const FCometChatReactionEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageReactionRemoved, const FCometChatReactionEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageModerated, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatAIAssistantMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatAIToolResultReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatAIToolArgumentsReceived, const FCometChatMessage&, Message);

// --- UserListener delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatUserOnline, const FCometChatUser&, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatUserOffline, const FCometChatUser&, User);

// --- GroupListener delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCometChatGroupMemberJoined, const FCometChatAction&, Action, const FCometChatUser&, JoinedUser, const FCometChatGroup&, JoinedGroup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCometChatGroupMemberLeft, const FCometChatAction&, Action, const FCometChatUser&, LeftUser, const FCometChatGroup&, LeftGroup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCometChatGroupMemberKicked, const FCometChatAction&, Action, const FCometChatUser&, KickedUser, const FCometChatUser&, KickedBy, const FCometChatGroup&, KickedFrom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCometChatGroupMemberBanned, const FCometChatAction&, Action, const FCometChatUser&, BannedUser, const FCometChatUser&, BannedBy, const FCometChatGroup&, BannedFrom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCometChatGroupMemberUnbanned, const FCometChatAction&, Action, const FCometChatUser&, UnbannedUser, const FCometChatUser&, UnbannedBy, const FCometChatGroup&, UnbannedFrom);

// Scope changed needs 6 params - use a struct wrapper
USTRUCT(BlueprintType)
struct FCometChatScopeChangeEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FCometChatAction Action;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FCometChatUser UpdatedBy;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FCometChatUser UpdatedUser;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ScopeChangedTo;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ScopeChangedFrom;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FCometChatGroup Group;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGroupMemberScopeChanged, const FCometChatScopeChangeEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCometChatMemberAddedToGroup, const FCometChatAction&, Action, const FCometChatUser&, AddedBy, const FCometChatUser&, UserAdded, const FCometChatGroup&, AddedTo);

// --- CallListener delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatIncomingCallReceived, const FCometChatCall&, Call);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatOutgoingCallAccepted, const FCometChatCall&, Call);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatOutgoingCallRejected, const FCometChatCall&, Call);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatIncomingCallCancelled, const FCometChatCall&, Call);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatCallEndedMessageReceived, const FCometChatCall&, Call);

// --- ConnectionListener delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatConnecting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatDisconnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatFeatureThrottled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatConnectionError, const FCometChatError&, Error);

// --- LoginListener delegates ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLoginEvent, const FCometChatUser&, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLoginFailedEvent, const FCometChatError&, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatLogoutEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLogoutFailedEvent, const FCometChatError&, Error);

// --- AIAssistantListener delegate ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatAIAssistantEvent, const FCometChatAIAssistantEvent&, Event);

// --- Legacy delegates (backward compat) ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatPresenceChanged, const FCometChatPresence&, Presence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatConnectionStateChanged, ECometChatConnectionState, State);

// ============================================================
// UCometChatSubsystem — Main interface for CometChat in Unreal
// ============================================================

UCLASS()
class COMETCHAT_API UCometChatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // --- Configuration (C++ only — Blueprint users should use CometChatConfigureAsync node) ---

    void Configure(const FString& AppId, const FString& Region);

    void ConfigureWithSettings(const FString& AppId, const FCometChatAppSettings& Settings);

    // --- Authentication ---

    UFUNCTION(BlueprintPure, Category = "CometChat|Auth")
    bool IsLoggedIn() const;

    UFUNCTION(BlueprintPure, Category = "CometChat|Auth")
    FCometChatUser GetLoggedInUser() const;

    // --- Connection (Manual Socket Mode) ---

    UFUNCTION(BlueprintPure, Category = "CometChat|Connection")
    ECometChatConnectionState GetConnectionStatus() const;

    // --- Typing Indicators (fire-and-forget) ---

    UFUNCTION(BlueprintCallable, Category = "CometChat|Typing")
    void StartTyping(const FCometChatTypingIndicator& Indicator);

    UFUNCTION(BlueprintCallable, Category = "CometChat|Typing")
    void EndTyping(const FCometChatTypingIndicator& Indicator);

    // --- Transient Messages (fire-and-forget) ---

    UFUNCTION(BlueprintCallable, Category = "CometChat|Messaging")
    void SendTransientMessage(const FCometChatTransientMessage& Message);

    // --- Lifecycle ---

    UFUNCTION(BlueprintCallable, Category = "CometChat")
    void Shutdown();

    // --- C++-only accessors (used by latent async action nodes) ---

    chatsdk::ChatSDK* GetSdk() const;
    void SetLoggedIn(bool bLoggedIn);
    bool HasSavedSession() const;
    void ClearSavedSession();
    static const FString AuthSaveSlotName;

    // ============================================================
    // MessageListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatTextMessageReceived OnTextMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMediaMessageReceived OnMediaMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatCustomMessageReceived OnCustomMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatInteractiveMessageReceived OnInteractiveMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatInteractionGoalCompleted OnInteractionGoalCompleted;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatTypingStarted OnTypingStarted;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatTypingEnded OnTypingEnded;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessagesDelivered OnMessagesDelivered;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessagesRead OnMessagesRead;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessagesDeliveredToAll OnMessagesDeliveredToAll;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessagesReadByAll OnMessagesReadByAll;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessageEdited OnMessageEdited;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessageDeleted OnMessageDeleted;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatTransientMessageReceived OnTransientMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessageReactionAdded OnMessageReactionAdded;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessageReactionRemoved OnMessageReactionRemoved;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatMessageModerated OnMessageModerated;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatAIAssistantMessageReceived OnAIAssistantMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatAIToolResultReceived OnAIToolResultReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Message")
    FOnCometChatAIToolArgumentsReceived OnAIToolArgumentsReceived;

    // ============================================================
    // UserListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|User")
    FOnCometChatUserOnline OnUserOnline;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|User")
    FOnCometChatUserOffline OnUserOffline;

    // ============================================================
    // GroupListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatGroupMemberJoined OnGroupMemberJoined;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatGroupMemberLeft OnGroupMemberLeft;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatGroupMemberKicked OnGroupMemberKicked;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatGroupMemberBanned OnGroupMemberBanned;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatGroupMemberUnbanned OnGroupMemberUnbanned;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatGroupMemberScopeChanged OnGroupMemberScopeChanged;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Group")
    FOnCometChatMemberAddedToGroup OnMemberAddedToGroup;

    // ============================================================
    // CallListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Call")
    FOnCometChatIncomingCallReceived OnIncomingCallReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Call")
    FOnCometChatOutgoingCallAccepted OnOutgoingCallAccepted;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Call")
    FOnCometChatOutgoingCallRejected OnOutgoingCallRejected;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Call")
    FOnCometChatIncomingCallCancelled OnIncomingCallCancelled;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Call")
    FOnCometChatCallEndedMessageReceived OnCallEndedMessageReceived;

    // ============================================================
    // ConnectionListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Connection")
    FOnCometChatConnected OnConnected;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Connection")
    FOnCometChatConnecting OnConnecting;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Connection")
    FOnCometChatDisconnected OnDisconnected;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Connection")
    FOnCometChatFeatureThrottled OnFeatureThrottled;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Connection")
    FOnCometChatConnectionError OnConnectionError;

    // ============================================================
    // LoginListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Login")
    FOnCometChatLoginEvent OnLoginSuccess;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Login")
    FOnCometChatLoginFailedEvent OnLoginFailure;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Login")
    FOnCometChatLogoutEvent OnLogoutSuccess;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|Login")
    FOnCometChatLogoutFailedEvent OnLogoutFailure;

    // ============================================================
    // AIAssistantListener Events
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events|AI")
    FOnCometChatAIAssistantEvent OnAIAssistantEvent;

    // ============================================================
    // Legacy Events (backward compat)
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatMessageReceived OnMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatPresenceChanged OnPresenceChanged;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatConnectionStateChanged OnConnectionStateChanged;

private:
    TSharedPtr<chatsdk::ChatSDK> Sdk;

    FString ConfiguredAppId;
    FString ConfiguredRegion;
    bool bIsLoggedIn = false;

    FTSTicker::FDelegateHandle TickHandle;

    void WireListeners();

    // Session persistence (internal)
    void SaveAuthToken();
    bool TryAutoLogin();
};
