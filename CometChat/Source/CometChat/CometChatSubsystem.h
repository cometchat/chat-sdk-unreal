#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "chatsdk/cometchat.h"
#include "CometChatSubsystem.generated.h"

// ============================================================
// Unreal-side mirror types (Blueprint-friendly, no std:: exposure)
// ============================================================

USTRUCT(BlueprintType)
struct FCometChatUser
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Uid;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Name;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString AvatarUrl;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Status;
};

USTRUCT(BlueprintType)
struct FCometChatMessage
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Id;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString SenderUid;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ReceiverUid;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Text;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 SentAt = 0;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Type;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Category;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ReceiverType;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ConversationId;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString SenderName;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString SenderAvatar;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 UpdatedAt = 0;
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

USTRUCT(BlueprintType)
struct FCometChatGroup
{
    GENERATED_BODY()
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Guid;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Name;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Description;
    
    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    TArray<FString> MemberIds;
};

// ============================================================
// Additional mirror types for real-time events
// ============================================================

UENUM(BlueprintType)
enum class ECometChatConnectionState : uint8
{
    Connected      UMETA(DisplayName = "Connected"),
    Disconnected   UMETA(DisplayName = "Disconnected"),
    Reconnecting   UMETA(DisplayName = "Reconnecting")
};

UENUM(BlueprintType)
enum class ECometChatPresenceStatus : uint8
{
    Online   UMETA(DisplayName = "Online"),
    Offline  UMETA(DisplayName = "Offline"),
    Away     UMETA(DisplayName = "Away")
};

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
struct FCometChatTypingEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Uid;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString ConversationId;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    bool bIsTyping = false;
};

USTRUCT(BlueprintType)
struct FCometChatReceiptEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString MessageId;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Uid;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    FString Status;

    UPROPERTY(BlueprintReadOnly, Category = "CometChat")
    int64 Timestamp = 0;
};

// ============================================================
// Delegates for async callbacks (all fire on Game Thread)
// ============================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatMessageReceived, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatPresenceChanged, const FCometChatPresence&, Presence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatTypingChanged, const FCometChatTypingEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatReceiptReceived, const FCometChatReceiptEvent&, Event);
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

    // --- Configuration (call before Login) ---
    
    UFUNCTION(BlueprintCallable, Category = "CometChat|Config")
    void Configure(const FString& AppId, const FString& Region);

    // --- Authentication ---
    
    UFUNCTION(BlueprintPure, Category = "CometChat|Auth")
    bool IsLoggedIn() const;

    // --- Lifecycle ---
    
    UFUNCTION(BlueprintCallable, Category = "CometChat")
    void Shutdown();

    // --- C++-only accessors (used by latent async action nodes) ---

    /** Returns the raw SDK pointer, or nullptr if not configured. */
    chatsdk::ChatSDK* GetSdk() const;

    /** Called by Login/Logout latent nodes to update auth state. */
    void SetLoggedIn(bool bLoggedIn);

    // ============================================================
    // Events (all guaranteed to fire on Game Thread)
    // ============================================================
    
    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatMessageReceived OnMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatPresenceChanged OnPresenceChanged;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatTypingChanged OnTypingChanged;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatReceiptReceived OnReceiptReceived;

    UPROPERTY(BlueprintAssignable, Category = "CometChat|Events")
    FOnCometChatConnectionStateChanged OnConnectionStateChanged;

private:
    TSharedPtr<chatsdk::ChatSDK> Sdk;
    
    FString ConfiguredAppId;
    FString ConfiguredRegion;
    bool bIsLoggedIn = false;
};
