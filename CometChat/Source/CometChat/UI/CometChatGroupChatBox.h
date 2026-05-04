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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Config")
	FString GroupGuid;

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
	// Events
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

	void BuildChatUI();
	UHorizontalBox* CreateMessageRow(const FCometChatMessage& Message);
	void AddMessageToList(const FCometChatMessage& Message);
	void ScrollToBottom();
	void SetState(EChatState NewState, const FString& StatusMessage = TEXT(""));

	void StartChatFlow();
	void DoLogin();
	void DoJoinGroup();
	void DoLoadHistory();

	UFUNCTION() void OnSendClicked();
	UFUNCTION() void OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION() void HandleOptionsClicked();
	UFUNCTION() void OnMessageReceived(const FCometChatMessage& Message);

	UCometChatSubsystem* GetChatSubsystem() const;
};
