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
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "../CometChatSubsystem.h"
#include "CometChatPanel.generated.h"

class UTexture2DDynamic;

/**
 * UCometChatPanel — Game-style tabbed chat panel.
 * 
 * Layout: Dark translucent HUD with glowing accent tabs.
 * Tabs: [My Groups] [Personal] [Browse Groups]
 * Selecting an item opens the chat view inline.
 */
UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="CometChat Panel"))
class COMETCHAT_API UCometChatPanel : public UUserWidget
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

	// ============================================================
	// Panel Style (Game HUD look)
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float PanelWidth = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float PanelHeight = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor PanelBackground = FLinearColor(0.02f, 0.02f, 0.05f, 0.92f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor AccentColor = FLinearColor(0.0f, 0.6f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor AccentGlow = FLinearColor(0.0f, 0.4f, 0.8f, 0.4f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor TabInactiveColor = FLinearColor(0.15f, 0.15f, 0.2f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor TabActiveColor = FLinearColor(0.0f, 0.5f, 0.9f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor TextColor = FLinearColor(0.9f, 0.92f, 0.95f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor SubTextColor = FLinearColor(0.5f, 0.55f, 0.6f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor ListItemHoverColor = FLinearColor(0.08f, 0.12f, 0.2f, 0.8f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor SearchBarColor = FLinearColor(0.06f, 0.07f, 0.1f, 0.9f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float SearchBarFontSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor SearchBarTextColor = FLinearColor(0.85f, 0.87f, 0.9f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor OnlineColor = FLinearColor(0.1f, 0.95f, 0.4f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	FLinearColor JoinButtonColor = FLinearColor(0.1f, 0.7f, 0.3f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float BorderWidth = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float CornerRadius = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float FontSize = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float SmallFontSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float ListItemHeight = 52.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style")
	float SidebarWidth = 220.0f;

	// Chat message text styling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style|Messages")
	float ChatMessageFontSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style|Messages")
	FLinearColor ChatMessageTextColor = FLinearColor(0.85f, 0.87f, 0.9f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style|Messages")
	float ChatAvatarSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style|Messages")
	float ChatUsernameFontSize = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style|Messages")
	FLinearColor ChatUsernameOwnColor = FLinearColor(0.0f, 0.6f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Style|Messages")
	FLinearColor ChatUsernameOtherColor = FLinearColor(0.9f, 0.7f, 0.2f, 1.0f);

	// ============================================================
	// Message Filters
	// ============================================================

	/** Show text messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Types")
	bool bShowTextMessages = true;

	/** Show image messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Types")
	bool bShowImageMessages = true;

	/** Show video messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Types")
	bool bShowVideoMessages = true;

	/** Show audio messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Types")
	bool bShowAudioMessages = true;

	/** Show file messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Types")
	bool bShowFileMessages = true;

	/** Show custom messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Categories")
	bool bShowCustomMessages = true;

	/** Show action messages (member joined, left, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Categories")
	bool bShowActionMessages = false;

	/** Show call messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Categories")
	bool bShowCallMessages = false;

	/** Hide messages from blocked users */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Visibility")
	bool bHideBlockedUserMessages = false;

	/** Hide deleted messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Visibility")
	bool bHideDeletedMessages = true;

	/** Hide thread replies from main chat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Visibility")
	bool bHideReplies = true;

	/** Only show messages with attachments */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Content")
	bool bOnlyWithAttachments = false;

	/** Only show messages with links */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Content")
	bool bOnlyWithLinks = false;

	/** Only show messages with mentions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Content")
	bool bOnlyWithMentions = false;

	/** Only show messages with reactions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Content")
	bool bOnlyWithReactions = false;

	/** Only show unread messages */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Filters|Content")
	bool bOnlyUnread = false;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	enum class EActiveTab : uint8 { MyGroups, Personal, BrowseGroups };
	enum class EPanelView : uint8 { List, Chat };

	EActiveTab ActiveTab = EActiveTab::MyGroups;
	EPanelView CurrentView = EPanelView::List;
	FString ActiveChatId;       // GUID or UID of active chat
	FString ActiveChatType;     // "group" or "user"
	FString ActiveChatName;

	// --- Widgets ---
	UPROPERTY() TObjectPtr<USizeBox> RootSizeBox;
	UPROPERTY() TObjectPtr<UOverlay> RootOverlay;
	UPROPERTY() TObjectPtr<UBorder> PanelBorder;
	UPROPERTY() TObjectPtr<UVerticalBox> MainVBox;
	UPROPERTY() TObjectPtr<UHorizontalBox> TabBar;
	UPROPERTY() TObjectPtr<UButton> TabMyGroups;
	UPROPERTY() TObjectPtr<UButton> TabPersonal;
	UPROPERTY() TObjectPtr<UButton> TabBrowse;
	UPROPERTY() TObjectPtr<UEditableTextBox> SearchInput;
	UPROPERTY() TObjectPtr<UWidgetSwitcher> ContentSwitcher;
	UPROPERTY() TObjectPtr<UScrollBox> MyGroupsList;
	UPROPERTY() TObjectPtr<UScrollBox> PersonalList;
	UPROPERTY() TObjectPtr<UScrollBox> BrowseGroupsList;
	UPROPERTY() TObjectPtr<UVerticalBox> ChatContainer;
	UPROPERTY() TObjectPtr<UBorder> ChatHeader;
	UPROPERTY() TObjectPtr<UTextBlock> ChatHeaderText;
	UPROPERTY() TObjectPtr<UButton> BackButton;
	UPROPERTY() TObjectPtr<UScrollBox> ChatMessages;
	UPROPERTY() TObjectPtr<UEditableTextBox> ChatInput;
	UPROPERTY() TObjectPtr<UButton> ChatSendButton;
	UPROPERTY() TObjectPtr<UTextBlock> ChatTypingText;

	// Snackbar / toast notification
	UPROPERTY() TObjectPtr<UBorder> SnackbarBorder;
	UPROPERTY() TObjectPtr<UTextBlock> SnackbarText;
	FTimerHandle SnackbarTimerHandle;

	// --- State ---
	TArray<FCometChatGroup> MyGroups;
	TArray<FCometChatConversation> PersonalConversations;
	TArray<FCometChatGroup> BrowseableGroups;
	TMap<FString, bool> OnlineUsers;
	TArray<FString> ChatMessageIds;
	int32 ChatMessageCount = 0;
	FString SearchQuery;

	// --- Build ---
	void BuildUI();
	void BuildTabBar();
	void BuildListViews();
	void BuildChatView();
	void SwitchTab(EActiveTab Tab);
	void ShowChatView(const FString& Id, const FString& Type, const FString& Name);
	void ShowListView();
	void UpdateTabStyles();

	// --- Data ---
	void InitSDK();
	void FetchMyGroups();
	void FetchPersonalChats();
	void FetchBrowseGroups();
	void LoadChatHistory();
	void SendChatMessage();

	// --- List Item Creation ---
	UWidget* CreateGroupListItem(const FCometChatGroup& Group, bool bShowJoinButton);
	UWidget* CreatePersonalListItem(const FCometChatConversation& Conv);

	// --- Handlers ---
	UFUNCTION() void OnTabMyGroupsClicked();
	UFUNCTION() void OnTabPersonalClicked();
	UFUNCTION() void OnTabBrowseClicked();
	UFUNCTION() void OnBackClicked();
	UFUNCTION() void OnChatSendClicked();
	UFUNCTION() void OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION() void OnMyGroupItemClicked();
	UFUNCTION() void OnPersonalItemClicked();
	UFUNCTION() void OnJoinButtonClicked();
	UFUNCTION() void OnSearchTextChanged(const FText& Text);
	UFUNCTION() void OnSearchCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void OnGroupItemClicked(FString Guid, FString Name);
	void OnPersonalItemClicked(FString Uid, FString Name);
	void OnJoinGroupClicked(FString Guid);
	void FilterCurrentList();

	// --- Event Handlers ---
	UFUNCTION() void HandleTextMessage(const FCometChatMessage& Message);
	UFUNCTION() void HandleUserOnline(const FCometChatUser& User);
	UFUNCTION() void HandleUserOffline(const FCometChatUser& User);
	UFUNCTION() void HandleTypingStarted(const FCometChatTypingIndicator& Indicator);
	UFUNCTION() void HandleTypingEnded(const FCometChatTypingIndicator& Indicator);

	void AddChatMessage(const FCometChatMessage& Message);
	void EnforceChatMessageLimit();

	// Avatar image loading
	void LoadAvatarImage(const FString& Url, UImage* TargetImage, UTextBlock* FallbackText);
	TMap<FString, TWeakObjectPtr<UTexture2DDynamic>> AvatarCache;

	// Typing indicator management
	void SendTypingStarted();
	void SendTypingEnded();
	void UpdateTypingDisplay();
	UFUNCTION() void OnChatInputTextChanged(const FText& Text);
	TSet<FString> CurrentTypers;  // UIDs of users currently typing in active chat
	FTimerHandle TypingSendTimerHandle;  // Timer to auto-send end_typing
	FTimerHandle TypingTimeoutHandle;   // Timer to auto-clear remote typing indicator
	bool bIsLocalTyping = false;

	// Snackbar
	void ShowSnackbar(const FString& Message, FLinearColor BgColor = FLinearColor(0.8f, 0.1f, 0.1f, 0.9f));

	UCometChatSubsystem* GetSubsystem() const;
};
