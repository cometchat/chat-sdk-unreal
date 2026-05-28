#include "CometChatSubsystem.h"
#include "CometChatConversions.h"
#include "CometChatEventBridge.h"

DEFINE_LOG_CATEGORY_STATIC(LogCometChat, Log, All);

// ============================================================
// std ↔ Unreal conversion helpers (shared via CometChatConversions.h)
// ============================================================

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
    UE_LOG(LogCometChat, Log, TEXT("Configure called - AppId: %s, Region: %s"), *AppId, *Region);
    
    ConfiguredAppId = AppId;
    ConfiguredRegion = Region;
    
    // Reset existing SDK if any
    if (Sdk.IsValid())
    {
        Sdk->shutdown();
        Sdk.Reset();
    }
    
    // Create SDK on game thread
    chatsdk::Config config;
    config.app_id = TCHAR_TO_UTF8(*AppId);
    config.region = TCHAR_TO_UTF8(*Region);
    config.enable_websocket = true;
    
    UE_LOG(LogCometChat, Log, TEXT("Creating SDK with app_id=%s, region=%s"), 
        UTF8_TO_TCHAR(config.app_id.c_str()), 
        UTF8_TO_TCHAR(config.region.c_str()));
    
    auto dispatcher = std::make_shared<FGameThreadDispatcher>();
    Sdk = MakeShared<chatsdk::ChatSDK>(config, dispatcher);
    
    UE_LOG(LogCometChat, Log, TEXT("SDK created successfully"));

    // --- Wire real-time push event listeners ---
    Sdk->on_message_received([this](const chatsdk::Message& msg)
    {
        OnMessageReceived.Broadcast(ToUnreal(msg));
    });

    Sdk->on_presence_changed([this](const chatsdk::Presence& p)
    {
        OnPresenceChanged.Broadcast(ToUnreal(p));
    });

    Sdk->on_typing_changed([this](const chatsdk::TypingEvent& t)
    {
        OnTypingChanged.Broadcast(ToUnreal(t));
    });

    Sdk->on_receipt_received([this](const chatsdk::ReceiptEvent& r)
    {
        OnReceiptReceived.Broadcast(ToUnreal(r));
    });

    Sdk->on_connection_state_changed([this](chatsdk::ConnectionState s)
    {
        OnConnectionStateChanged.Broadcast(ToUnreal(s));
    });
}

bool UCometChatSubsystem::IsLoggedIn() const
{
    return bIsLoggedIn;
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
// C++-only accessors (used by latent async action nodes)
// ============================================================

chatsdk::ChatSDK* UCometChatSubsystem::GetSdk() const
{
    return Sdk.IsValid() ? Sdk.Get() : nullptr;
}

void UCometChatSubsystem::SetLoggedIn(bool bLoggedIn)
{
    bIsLoggedIn = bLoggedIn;
}
