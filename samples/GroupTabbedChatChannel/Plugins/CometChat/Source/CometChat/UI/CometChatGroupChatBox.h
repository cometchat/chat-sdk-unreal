#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/Image.h"
#include "../CometChatSubsystem.h"
#include "CometChatGroupChatBox.generated.h"

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="CometChat Group Chat Box"))
class COMETCHAT_API UCometChatGroupChatBox : public UUserWidget
{
	GENERATED_BODY()

public:
	// ============================================================
	// SDK Configuration
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString AppId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString Region = TEXT("us");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString AuthKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString UserUid;

	/** For group chat mode. Leave empty for 1:1 chat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString GroupGuid;

	/** For 1:1 chat mode. Set the receiver's UID here. Leave empty for group chat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString ReceiverUid;

	// ============================================================
	// Feature Toggles (configurable listeners)
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableTypingIndicator = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableOnlinePresence = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableMessageEdited = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableMessageDeleted = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableGroupMemberEvents = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableDeliveryReceipts = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bEnableConnectionStatus = true;

	/** When true, uses OnMessageModerated instead of OnTextMessageReceived for incoming messages. Off by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	bool bUseModeratedMessageListener = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	float TypingTimeoutSeconds = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Features")
	int32 MaxMessagesInMemory = 100;

	// ============================================================
	// Panel Layout
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	float PanelWidth = 520.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	float PanelHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	FLinearColor PanelBackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.35f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	float PanelCornerRadius = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	float PanelPadding = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	int32 MaxVisibleMessages = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Panel")
	int32 MessageLimit = 30;

	// ============================================================
	// Message Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	FLinearColor SenderNameColor = FLinearColor(1.0f, 0.9f, 0.2f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	FLinearColor ReceiverNameColor = FLinearColor(0.3f, 0.6f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	FLinearColor YouLabelColor = FLinearColor(0.4f, 0.8f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	float UsernameFontSize = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	FLinearColor MessageTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	float MessageFontSize = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	float MessageSpacing = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	bool bEnableTextShadow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	FLinearColor TextShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Messages")
	FVector2D TextShadowOffset = FVector2D(1.0f, 1.0f);

	// ============================================================
	// Typing Indicator Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Typing")
	FLinearColor TypingIndicatorColor = FLinearColor(0.6f, 0.6f, 0.65f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Typing")
	float TypingIndicatorFontSize = 12.0f;

	// ============================================================
	// Connection Status Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Connection")
	FLinearColor ConnectionBannerColor = FLinearColor(0.8f, 0.2f, 0.2f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Connection")
	FLinearColor ConnectionBannerTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// ============================================================
	// Timestamp Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Timestamp")
	bool bShowTimestamp = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Timestamp")
	FLinearColor TimestampColor = FLinearColor(0.6f, 0.6f, 0.65f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Timestamp")
	float TimestampFontSize = 11.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Timestamp")
	float TimestampLeftPadding = 8.0f;

	// ============================================================
	// Avatar Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	bool bShowAvatars = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	float AvatarSize = 36.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	FLinearColor AvatarPlaceholderColor = FLinearColor(0.4f, 0.4f, 0.5f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	bool bShowOnlineIndicator = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	FLinearColor OnlineIndicatorColor = FLinearColor(0.2f, 0.9f, 0.3f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	float OnlineIndicatorSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	FLinearColor OfflineIndicatorColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.6f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Avatar")
	FLinearColor AvatarInitialColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// ============================================================
	// Composer Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Composer")
	FLinearColor ComposerBackgroundColor = FLinearColor(0.15f, 0.15f, 0.18f, 0.85f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Composer")
	FLinearColor ComposerBorderColor = FLinearColor(0.5f, 0.5f, 0.55f, 0.6f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Composer")
	float ComposerCornerRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Composer")
	FString ComposerPlaceholderText = TEXT("Type a message...");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Composer")
	FLinearColor ComposerTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.8f);

	// ============================================================
	// Send Button
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|SendButton")
	FString SendButtonLabel = TEXT(">");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|SendButton")
	FLinearColor SendButtonColor = FLinearColor(0.2f, 0.45f, 0.9f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|SendButton")
	FLinearColor SendButtonTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|SendButton")
	float SendButtonWidth = 42.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|SendButton")
	float SendButtonFontSize = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|SendButton")
	float SendButtonCornerRadius = 20.0f;

	// ============================================================
	// Options Button
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|OptionsButton")
	bool bShowOptionsButton = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|OptionsButton")
	FString OptionsButtonLabel = TEXT("...");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|OptionsButton")
	float OptionsButtonSize = 32.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|OptionsButton")
	FLinearColor OptionsButtonColor = FLinearColor(0.4f, 0.4f, 0.45f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|OptionsButton")
	FLinearColor OptionsButtonTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// ============================================================
	// Loading Overlay
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Overlay")
	FLinearColor OverlayBackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.6f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Overlay")
	FLinearColor OverlayTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Overlay")
	float OverlayFontSize = 14.0f;

	// ============================================================
	// Events (Blueprint-assignable)
	// ============================================================

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOptionsClicked);

	UPROPERTY(BlueprintAssignable, Category="CometChat|Events")
	FOnOptionsClicked OnOptionsClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	enum class EChatState : uint8
	{
		Uninitialized, Configuring, LoggingIn, JoiningGroup, LoadingHistory, Ready, Error
	};

	EChatState CurrentState = EChatState::Uninitialized;
	FString CurrentUserUid;

	// --- UI Widgets ---
	UPROPERTY() TObjectPtr<USizeBox> RootSizeBox;
	UPROPERTY() TObjectPtr<UBorder> PanelBorder;
	UPROPERTY() TObjectPtr<UVerticalBox> PanelVBox;
	UPROPERTY() TObjectPtr<UScrollBox> MessageScrollBox;
	UPROPERTY() TObjectPtr<UBorder> ComposerBorder;
	UPROPERTY() TObjectPtr<UEditableTextBox> MessageInput;
	UPROPERTY() TObjectPtr<UButton> SendButton;
	UPROPERTY() TObjectPtr<UButton> OptionsButton;
	UPROPERTY() TObjectPtr<UTextBlock> OverlayText;
	UPROPERTY() TObjectPtr<UBorder> OverlayBorder;
	UPROPERTY() TObjectPtr<UOverlay> RootOverlay;
	UPROPERTY() TObjectPtr<UTextBlock> TypingIndicatorText;
	UPROPERTY() TObjectPtr<UBorder> ConnectionBanner;
	UPROPERTY() TObjectPtr<UTextBlock> ConnectionBannerText;

	// --- State tracking ---
	TMap<FString, bool> OnlineUsers;
	TMap<FString, TWeakObjectPtr<UBorder>> OnlineDotWidgets; // uid -> dot border widget for live updates
	TMap<FString, FString> TypingUsers; // uid -> display name
	TArray<FString> MessageIds; // ordered list of message IDs for memory management
	int32 CurrentMessageCount = 0;
	FTimerHandle TypingTimeoutHandle;
	bool bIsLocalUserTyping = false;
	FTimerHandle LocalTypingEndHandle;

	// --- UI Building ---
	void BuildChatUI();
	UHorizontalBox* CreateMessageRow(const FCometChatMessage& Message);
	void AddMessageToList(const FCometChatMessage& Message);
	void AddSystemMessage(const FString& Text);
	void ScrollToBottom();
	void SetState(EChatState NewState, const FString& StatusMessage = TEXT(""));
	void EnforceMessageLimit();
	void UpdateTypingIndicatorUI();
	void UpdateOnlineIndicator(const FString& Uid, bool bOnline);

	// --- Chat Flow ---
	void StartChatFlow();
	bool IsGroupMode() const;
	void DoLogin();
	void DoJoinGroup();
	void DoLoadHistory();
	void SubscribeToEvents();
	void UnsubscribeFromEvents();

	// --- Input Handlers ---
	UFUNCTION() void OnSendClicked();
	UFUNCTION() void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION() void OnInputChanged(const FText& Text);
	UFUNCTION() void HandleOptionsClicked();

	// --- Event Handlers (new typed listeners) ---
	UFUNCTION() void HandleTextMessageReceived(const FCometChatMessage& Message);
	UFUNCTION() void HandleMediaMessageReceived(const FCometChatMessage& Message);
	UFUNCTION() void HandleMessageModerated(const FCometChatMessage& Message);
	UFUNCTION() void HandleMessageEdited(const FCometChatMessage& Message);
	UFUNCTION() void HandleMessageDeleted(const FCometChatMessage& Message);
	UFUNCTION() void HandleTypingStarted(const FCometChatTypingIndicator& Indicator);
	UFUNCTION() void HandleTypingEnded(const FCometChatTypingIndicator& Indicator);
	UFUNCTION() void HandleUserOnline(const FCometChatUser& User);
	UFUNCTION() void HandleUserOffline(const FCometChatUser& User);
	UFUNCTION() void HandleGroupMemberJoined(const FCometChatAction& Action, const FCometChatUser& User, const FCometChatGroup& Group);
	UFUNCTION() void HandleGroupMemberLeft(const FCometChatAction& Action, const FCometChatUser& User, const FCometChatGroup& Group);
	UFUNCTION() void HandleConnected();
	UFUNCTION() void HandleDisconnected();

	// --- Legacy handler (backward compat) ---
	UFUNCTION() void OnMessageReceived(const FCometChatMessage& Message);

	void ClearTypingTimeout(const FString& Uid);
	void OnTypingTimeout();
	void FetchOnlineMembers();

	UCometChatSubsystem* GetChatSubsystem() const;
};
