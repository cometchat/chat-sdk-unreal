#include "CometChatSubsystem.h"
#include "CometChatConversions.h"
#include "CometChatEventBridge.h"
#include "CometChatAuthSave.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogCometChat, Log, All);

const FString UCometChatSubsystem::AuthSaveSlotName = TEXT("CometChatAuth");

using namespace CometChatConversions;

// ============================================================
// Subsystem Lifecycle
// ============================================================

void UCometChatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogCometChat, Log, TEXT("CometChat Subsystem Initialized"));
}

void UCometChatSubsystem::Deinitialize()
{
    UE_LOG(LogCometChat, Log, TEXT("CometChat Subsystem Deinitializing"));

    // Remove tick delegate
    if (TickHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
        TickHandle.Reset();
    }

    if (Sdk.IsValid())
    {
        Sdk->shutdown();
        Sdk.Reset();
    }
    Super::Deinitialize();
}

// ============================================================
// Configuration
// ============================================================

void UCometChatSubsystem::Configure(const FString& AppId, const FString& Region)
{
    FCometChatAppSettings Settings;
    Settings.Region = Region;
    ConfigureWithSettings(AppId, Settings);
}

void UCometChatSubsystem::ConfigureWithSettings(const FString& AppId, const FCometChatAppSettings& Settings)
{
    UE_LOG(LogCometChat, Log, TEXT("ConfigureWithSettings - AppId: %s, Region: %s"), *AppId, *Settings.Region);

    ConfiguredAppId = AppId;
    ConfiguredRegion = Settings.Region;

    if (Sdk.IsValid())
    {
        Sdk->shutdown();
        Sdk.Reset();
    }

    chatsdk::Config config;
    config.app_id = TCHAR_TO_UTF8(*AppId);
    config.region = TCHAR_TO_UTF8(*Settings.Region);
    config.enable_websocket = Settings.bAutoEstablishSocketConnection;

    auto dispatcher = std::make_shared<FGameThreadDispatcher>();
    Sdk = MakeShared<chatsdk::ChatSDK>(config, dispatcher);

    UE_LOG(LogCometChat, Log, TEXT("SDK created successfully"));

    // Set up tick pump for async HTTP (curl_multi)
    if (TickHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
    }
    TickHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([this](float DeltaTime) -> bool {
            if (Sdk.IsValid())
            {
                Sdk->tick(); // No-op when no pending requests (curl_multi_perform on empty set)
            }
            return true; // keep ticking
        }),
        0.0f // tick every frame
    );

    WireListeners();
}

// ============================================================
// Wire all real-time listeners to Unreal delegates
// ============================================================

void UCometChatSubsystem::WireListeners()
{
    if (!Sdk.IsValid()) return;

    // --- MessageListener ---
    chatsdk::MessageListenerCallbacks msgCallbacks;

    msgCallbacks.on_text_message_received = [this](const chatsdk::Message& msg) {
        OnTextMessageReceived.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_media_message_received = [this](const chatsdk::Message& msg) {
        OnMediaMessageReceived.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_custom_message_received = [this](const chatsdk::Message& msg) {
        OnCustomMessageReceived.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_interactive_message_received = [this](const chatsdk::Message& msg) {
        OnInteractiveMessageReceived.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_interaction_goal_completed = [this](const chatsdk::InteractionReceipt& r) {
        OnInteractionGoalCompleted.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_typing_started = [this](const chatsdk::TypingIndicator& t) {
        UE_LOG(LogTemp, Log, TEXT("CometChatSubsystem: on_typing_started received - sender=%hs, receiver_id=%hs"), t.sender.uid.c_str(), t.receiver_id.c_str());
        OnTypingStarted.Broadcast(ToUnreal(t));
    };
    msgCallbacks.on_typing_ended = [this](const chatsdk::TypingIndicator& t) {
        UE_LOG(LogTemp, Log, TEXT("CometChatSubsystem: on_typing_ended received - sender=%hs, receiver_id=%hs"), t.sender.uid.c_str(), t.receiver_id.c_str());
        OnTypingEnded.Broadcast(ToUnreal(t));
    };
    msgCallbacks.on_messages_delivered = [this](const chatsdk::MessageReceipt& r) {
        OnMessagesDelivered.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_messages_read = [this](const chatsdk::MessageReceipt& r) {
        OnMessagesRead.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_messages_delivered_to_all = [this](const chatsdk::MessageReceipt& r) {
        OnMessagesDeliveredToAll.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_messages_read_by_all = [this](const chatsdk::MessageReceipt& r) {
        OnMessagesReadByAll.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_message_edited = [this](const chatsdk::Message& msg) {
        OnMessageEdited.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_message_deleted = [this](const chatsdk::Message& msg) {
        OnMessageDeleted.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_transient_message_received = [this](const chatsdk::TransientMessage& t) {
        OnTransientMessageReceived.Broadcast(ToUnreal(t));
    };
    msgCallbacks.on_message_reaction_added = [this](const chatsdk::ReactionEvent& r) {
        OnMessageReactionAdded.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_message_reaction_removed = [this](const chatsdk::ReactionEvent& r) {
        OnMessageReactionRemoved.Broadcast(ToUnreal(r));
    };
    msgCallbacks.on_message_moderated = [this](const chatsdk::Message& msg) {
        OnMessageModerated.Broadcast(ToUnreal(msg));
    };
    msgCallbacks.on_ai_assistant_message_received = [this](const chatsdk::AIAssistantMessage& msg) {
        OnAIAssistantMessageReceived.Broadcast(ToUnreal(msg.base_message));
    };
    msgCallbacks.on_ai_tool_result_received = [this](const chatsdk::AIToolResultMessage& msg) {
        OnAIToolResultReceived.Broadcast(ToUnreal(msg.base_message));
    };
    msgCallbacks.on_ai_tool_arguments_received = [this](const chatsdk::AIToolArgumentMessage& msg) {
        OnAIToolArgumentsReceived.Broadcast(ToUnreal(msg.base_message));
    };

    Sdk->add_message_listener("UnrealSubsystem_Msg", msgCallbacks);

    // --- UserListener (wire from presence events) ---
    chatsdk::UserListenerCallbacks userCallbacks;
    userCallbacks.on_user_online = [this](const chatsdk::User& u) {
        OnUserOnline.Broadcast(ToUnreal(u));
    };
    userCallbacks.on_user_offline = [this](const chatsdk::User& u) {
        OnUserOffline.Broadcast(ToUnreal(u));
    };

    Sdk->add_user_listener("UnrealSubsystem_User", userCallbacks);

    // Also wire legacy presence to fire OnUserOnline/OnUserOffline
    Sdk->on_presence_changed([this](const chatsdk::Presence& p) {
        OnPresenceChanged.Broadcast(ToUnreal(p));
        // Also fire typed user listeners based on presence status
        FCometChatUser u;
        u.Uid = UTF8_TO_TCHAR(p.uid.c_str());
        u.Status = (p.status == chatsdk::PresenceStatus::Online) ? TEXT("online") : TEXT("offline");
        u.LastActiveAt = p.last_active_at;
        if (p.status == chatsdk::PresenceStatus::Online)
            OnUserOnline.Broadcast(u);
        else
            OnUserOffline.Broadcast(u);
    });

    // --- GroupListener ---
    chatsdk::GroupListenerCallbacks groupCallbacks;
    groupCallbacks.on_group_member_joined = [this](const chatsdk::Action& a, const chatsdk::User& u, const chatsdk::Group& g) {
        OnGroupMemberJoined.Broadcast(ToUnreal(a), ToUnreal(u), ToUnreal(g));
    };
    groupCallbacks.on_group_member_left = [this](const chatsdk::Action& a, const chatsdk::User& u, const chatsdk::Group& g) {
        OnGroupMemberLeft.Broadcast(ToUnreal(a), ToUnreal(u), ToUnreal(g));
    };
    groupCallbacks.on_group_member_kicked = [this](const chatsdk::Action& a, const chatsdk::User& kicked, const chatsdk::User& by, const chatsdk::Group& g) {
        OnGroupMemberKicked.Broadcast(ToUnreal(a), ToUnreal(kicked), ToUnreal(by), ToUnreal(g));
    };
    groupCallbacks.on_group_member_banned = [this](const chatsdk::Action& a, const chatsdk::User& banned, const chatsdk::User& by, const chatsdk::Group& g) {
        OnGroupMemberBanned.Broadcast(ToUnreal(a), ToUnreal(banned), ToUnreal(by), ToUnreal(g));
    };
    groupCallbacks.on_group_member_unbanned = [this](const chatsdk::Action& a, const chatsdk::User& unbanned, const chatsdk::User& by, const chatsdk::Group& g) {
        OnGroupMemberUnbanned.Broadcast(ToUnreal(a), ToUnreal(unbanned), ToUnreal(by), ToUnreal(g));
    };
    groupCallbacks.on_group_member_scope_changed = [this](const chatsdk::Action& a, const chatsdk::User& updatedBy, const chatsdk::User& updatedUser, const std::string& to, const std::string& from, const chatsdk::Group& g) {
        FCometChatScopeChangeEvent evt;
        evt.Action = ToUnreal(a);
        evt.UpdatedBy = ToUnreal(updatedBy);
        evt.UpdatedUser = ToUnreal(updatedUser);
        evt.ScopeChangedTo = UTF8_TO_TCHAR(to.c_str());
        evt.ScopeChangedFrom = UTF8_TO_TCHAR(from.c_str());
        evt.Group = ToUnreal(g);
        OnGroupMemberScopeChanged.Broadcast(evt);
    };
    groupCallbacks.on_member_added_to_group = [this](const chatsdk::Action& a, const chatsdk::User& addedBy, const chatsdk::User& userAdded, const chatsdk::Group& g) {
        OnMemberAddedToGroup.Broadcast(ToUnreal(a), ToUnreal(addedBy), ToUnreal(userAdded), ToUnreal(g));
    };

    Sdk->add_group_listener("UnrealSubsystem_Group", groupCallbacks);

    // --- CallListener ---
    chatsdk::CallListenerCallbacks callCallbacks;
    callCallbacks.on_incoming_call_received = [this](const chatsdk::Call& c) {
        OnIncomingCallReceived.Broadcast(ToUnreal(c));
    };
    callCallbacks.on_outgoing_call_accepted = [this](const chatsdk::Call& c) {
        OnOutgoingCallAccepted.Broadcast(ToUnreal(c));
    };
    callCallbacks.on_outgoing_call_rejected = [this](const chatsdk::Call& c) {
        OnOutgoingCallRejected.Broadcast(ToUnreal(c));
    };
    callCallbacks.on_incoming_call_cancelled = [this](const chatsdk::Call& c) {
        OnIncomingCallCancelled.Broadcast(ToUnreal(c));
    };
    callCallbacks.on_call_ended_message_received = [this](const chatsdk::Call& c) {
        OnCallEndedMessageReceived.Broadcast(ToUnreal(c));
    };

    Sdk->add_call_listener("UnrealSubsystem_Call", callCallbacks);

    // --- ConnectionListener ---
    chatsdk::ConnectionListenerCallbacks connCallbacks;
    connCallbacks.on_connected = [this]() {
        OnConnected.Broadcast();
        OnConnectionStateChanged.Broadcast(ECometChatConnectionState::Connected);
    };
    connCallbacks.on_connecting = [this]() {
        OnConnecting.Broadcast();
        OnConnectionStateChanged.Broadcast(ECometChatConnectionState::Connecting);
    };
    connCallbacks.on_disconnected = [this]() {
        OnDisconnected.Broadcast();
        OnConnectionStateChanged.Broadcast(ECometChatConnectionState::Disconnected);
    };
    connCallbacks.on_feature_throttled = [this]() {
        OnFeatureThrottled.Broadcast();
        OnConnectionStateChanged.Broadcast(ECometChatConnectionState::FeatureThrottled);
    };
    connCallbacks.on_connection_error = [this](const chatsdk::CometChatError& e) {
        OnConnectionError.Broadcast(ToUnreal(e));
    };

    Sdk->add_connection_listener("UnrealSubsystem_Conn", connCallbacks);

    // --- LoginListener ---
    chatsdk::LoginListenerCallbacks loginCallbacks;
    loginCallbacks.on_login_success = [this](const chatsdk::User& u) {
        bIsLoggedIn = true;
        SaveAuthToken();
        OnLoginSuccess.Broadcast(ToUnreal(u));
    };
    loginCallbacks.on_login_failure = [this](const chatsdk::CometChatError& e) {
        OnLoginFailure.Broadcast(ToUnreal(e));
    };
    loginCallbacks.on_logout_success = [this]() {
        bIsLoggedIn = false;
        ClearSavedSession();
        OnLogoutSuccess.Broadcast();
    };
    loginCallbacks.on_logout_failure = [this](const chatsdk::CometChatError& e) {
        OnLogoutFailure.Broadcast(ToUnreal(e));
    };

    Sdk->add_login_listener("UnrealSubsystem_Login", loginCallbacks);

    // --- AIAssistantListener ---
    Sdk->add_ai_assistant_listener("UnrealSubsystem_AI", [this](const chatsdk::AIAssistantEvent& e) {
        OnAIAssistantEvent.Broadcast(ToUnreal(e));
    });

    // --- Legacy presence already wired above with OnUserOnline/OnUserOffline ---
}

// ============================================================
// Authentication
// ============================================================

bool UCometChatSubsystem::IsLoggedIn() const
{
    return bIsLoggedIn;
}

FCometChatUser UCometChatSubsystem::GetLoggedInUser() const
{
    if (Sdk.IsValid() && Sdk->is_authenticated())
    {
        return ToUnreal(Sdk->get_logged_in_user());
    }
    return FCometChatUser();
}

// ============================================================
// Connection (Manual Socket Mode)
// ============================================================

ECometChatConnectionState UCometChatSubsystem::GetConnectionStatus() const
{
    if (!Sdk.IsValid()) return ECometChatConnectionState::Disconnected;

    std::string status = Sdk->get_connection_status();
    if (status == "connected") return ECometChatConnectionState::Connected;
    if (status == "connecting") return ECometChatConnectionState::Connecting;
    return ECometChatConnectionState::Disconnected;
}

// ============================================================
// Typing Indicators (fire-and-forget)
// ============================================================

void UCometChatSubsystem::StartTyping(const FCometChatTypingIndicator& Indicator)
{
    if (Sdk.IsValid())
    {
        Sdk->start_typing(FromUnreal(Indicator));
    }
}

void UCometChatSubsystem::EndTyping(const FCometChatTypingIndicator& Indicator)
{
    if (Sdk.IsValid())
    {
        Sdk->end_typing(FromUnreal(Indicator));
    }
}

// ============================================================
// Transient Messages (fire-and-forget)
// ============================================================

void UCometChatSubsystem::SendTransientMessage(const FCometChatTransientMessage& Message)
{
    if (Sdk.IsValid())
    {
        chatsdk::TransientMessage tm;
        tm.receiver_id = TCHAR_TO_UTF8(*Message.ReceiverId);
        tm.receiver_type = TCHAR_TO_UTF8(*Message.ReceiverType);
        tm.data = TCHAR_TO_UTF8(*Message.Data);
        Sdk->send_transient_message(tm);
    }
}

// ============================================================
// Lifecycle
// ============================================================

void UCometChatSubsystem::Shutdown()
{
    UE_LOG(LogCometChat, Log, TEXT("Shutdown called"));
    if (Sdk.IsValid())
    {
        Sdk->shutdown();
        bIsLoggedIn = false;
    }
}

// ============================================================
// C++-only accessors
// ============================================================

chatsdk::ChatSDK* UCometChatSubsystem::GetSdk() const
{
    return Sdk.IsValid() ? Sdk.Get() : nullptr;
}

void UCometChatSubsystem::SetLoggedIn(bool bLoggedIn)
{
    bIsLoggedIn = bLoggedIn;
}

// ============================================================
// Session Persistence
// ============================================================

void UCometChatSubsystem::SaveAuthToken()
{
    if (!Sdk.IsValid() || !Sdk->is_authenticated()) return;

    std::string Token = Sdk->get_user_auth_token();
    if (Token.empty()) return;

    UCometChatAuthSave* Save = NewObject<UCometChatAuthSave>();
    Save->AuthToken = UTF8_TO_TCHAR(Token.c_str());
    Save->UserId = UTF8_TO_TCHAR(Sdk->get_logged_in_user().uid.c_str());

    if (UGameplayStatics::SaveGameToSlot(Save, AuthSaveSlotName, 0))
    {
        UE_LOG(LogCometChat, Log, TEXT("Auth token saved for user: %s"), *Save->UserId);
    }
    else
    {
        UE_LOG(LogCometChat, Warning, TEXT("Failed to save auth token"));
    }
}

bool UCometChatSubsystem::TryAutoLogin()
{
    if (!Sdk.IsValid())
    {
        UE_LOG(LogCometChat, Warning, TEXT("TryAutoLogin: SDK not configured. Call Configure first."));
        return false;
    }

    if (bIsLoggedIn)
    {
        UE_LOG(LogCometChat, Log, TEXT("TryAutoLogin: Already logged in, skipping."));
        return false;
    }

    if (!UGameplayStatics::DoesSaveGameExist(AuthSaveSlotName, 0))
    {
        UE_LOG(LogCometChat, Log, TEXT("TryAutoLogin: No saved session found."));
        return false;
    }

    UCometChatAuthSave* Loaded = Cast<UCometChatAuthSave>(
        UGameplayStatics::LoadGameFromSlot(AuthSaveSlotName, 0));

    if (!Loaded || Loaded->AuthToken.IsEmpty())
    {
        UE_LOG(LogCometChat, Log, TEXT("TryAutoLogin: Saved session is empty or invalid."));
        return false;
    }

    UE_LOG(LogCometChat, Log, TEXT("TryAutoLogin: Restoring session for user: %s"), *Loaded->UserId);

    Sdk->login_with_auth_token(
        TCHAR_TO_UTF8(*Loaded->AuthToken),
        [this](const std::string& err)
        {
            if (err.empty())
            {
                UE_LOG(LogCometChat, Log, TEXT("TryAutoLogin: Session restored successfully."));
                // bIsLoggedIn is set by the LoginListener callback
            }
            else
            {
                UE_LOG(LogCometChat, Warning, TEXT("TryAutoLogin: Token expired or invalid — clearing saved session. Error: %hs"), err.c_str());
                ClearSavedSession();
            }
        }
    );

    return true;
}

bool UCometChatSubsystem::HasSavedSession() const
{
    return UGameplayStatics::DoesSaveGameExist(AuthSaveSlotName, 0);
}

void UCometChatSubsystem::ClearSavedSession()
{
    if (UGameplayStatics::DoesSaveGameExist(AuthSaveSlotName, 0))
    {
        UGameplayStatics::DeleteGameInSlot(AuthSaveSlotName, 0);
        UE_LOG(LogCometChat, Log, TEXT("Saved auth session cleared."));
    }
}
