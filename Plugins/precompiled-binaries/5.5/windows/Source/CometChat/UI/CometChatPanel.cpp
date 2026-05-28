#include "CometChatPanel.h"
#include "../CometChatConversions.h"
#include "Async/Async.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/Texture2DDynamic.h"
#include "TimerManager.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/WidgetSwitcherSlot.h"
#include "Blueprint/WidgetTree.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Modules/ModuleManager.h"
#include "TextureResource.h"

void UCometChatPanel::NativeConstruct()
{
	Super::NativeConstruct();
	// Only fetch data if already logged in — no login attempt, no freeze
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([this]() { InitSDK(); });
	}
}

void UCometChatPanel::NativeDestruct()
{
	auto* Sub = GetSubsystem();
	if (Sub)
	{
		Sub->OnTextMessageReceived.RemoveDynamic(this, &UCometChatPanel::HandleTextMessage);
		Sub->OnUserOnline.RemoveDynamic(this, &UCometChatPanel::HandleUserOnline);
		Sub->OnUserOffline.RemoveDynamic(this, &UCometChatPanel::HandleUserOffline);
		Sub->OnTypingStarted.RemoveDynamic(this, &UCometChatPanel::HandleTypingStarted);
		Sub->OnTypingEnded.RemoveDynamic(this, &UCometChatPanel::HandleTypingEnded);
	}
	Super::NativeDestruct();
}

TSharedRef<SWidget> UCometChatPanel::RebuildWidget()
{
	if (!WidgetTree) WidgetTree = NewObject<UWidgetTree>(this, TEXT("PanelWidgetTree"));
	BuildUI();
	return Super::RebuildWidget();
}

UCometChatSubsystem* UCometChatPanel::GetSubsystem() const
{
	UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UCometChatSubsystem>() : nullptr;
}

// ============================================================
// UI Building
// ============================================================

void UCometChatPanel::BuildUI()
{
	RootSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("Root"));
	RootSizeBox->SetWidthOverride(PanelWidth);
	RootSizeBox->SetHeightOverride(PanelHeight);
	WidgetTree->RootWidget = RootSizeBox;

	// Simple dark panel
	PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PanelBorder"));
	PanelBorder->SetPadding(FMargin(8));
	PanelBorder->SetBrushColor(PanelBackground);

	// Overlay to allow snackbar to float on top of content
	UOverlay* PanelOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("PanelOverlay"));
	RootSizeBox->AddChild(PanelOverlay);
	auto* BorderSlot = PanelOverlay->AddChildToOverlay(PanelBorder);
	BorderSlot->SetHorizontalAlignment(HAlign_Fill);
	BorderSlot->SetVerticalAlignment(VAlign_Fill);

	// Snackbar (hidden by default, shown on events)
	SnackbarBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Snackbar"));
	SnackbarBorder->SetPadding(FMargin(12, 8));
	{ FSlateBrush SB; SB.DrawAs = ESlateBrushDrawType::RoundedBox; SB.TintColor = FSlateColor(FLinearColor(0.8f, 0.1f, 0.1f, 0.9f)); SB.OutlineSettings.CornerRadii = FVector4(6,6,6,6); SB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; SnackbarBorder->SetBrush(SB); }
	SnackbarBorder->SetVisibility(ESlateVisibility::Collapsed);
	SnackbarText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SnackbarText"));
	SnackbarText->SetText(FText::GetEmpty());
	SnackbarText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	{ FSlateFontInfo F = SnackbarText->GetFont(); F.Size = 11; SnackbarText->SetFont(F); }
	SnackbarBorder->AddChild(SnackbarText);
	auto* SnackSlot = PanelOverlay->AddChildToOverlay(SnackbarBorder);
	SnackSlot->SetHorizontalAlignment(HAlign_Center);
	SnackSlot->SetVerticalAlignment(VAlign_Bottom);
	SnackSlot->SetPadding(FMargin(0, 0, 0, 12));

	MainVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MainVBox"));
	PanelBorder->AddChild(MainVBox);

	// Tab bar: 3 simple text buttons in a row
	TabBar = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("TabBar"));
	auto* TBS = MainVBox->AddChildToVerticalBox(TabBar);
	TBS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	TBS->SetPadding(FMargin(0, 0, 0, 4));

	TabMyGroups = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("TabMyGroups"));
	UTextBlock* T1 = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	T1->SetText(FText::FromString(TEXT("Groups")));
	T1->SetColorAndOpacity(FSlateColor(TextColor));
	{ FSlateFontInfo F = T1->GetFont(); F.Size = 11; T1->SetFont(F); }
	TabMyGroups->AddChild(T1);
	TabMyGroups->OnClicked.AddDynamic(this, &UCometChatPanel::OnTabMyGroupsClicked);
	auto* T1S = TabBar->AddChildToHorizontalBox(TabMyGroups);
	T1S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	TabPersonal = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("TabPersonal"));
	UTextBlock* T2 = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	T2->SetText(FText::FromString(TEXT("DMs")));
	T2->SetColorAndOpacity(FSlateColor(TextColor));
	{ FSlateFontInfo F = T2->GetFont(); F.Size = 11; T2->SetFont(F); }
	TabPersonal->AddChild(T2);
	TabPersonal->OnClicked.AddDynamic(this, &UCometChatPanel::OnTabPersonalClicked);
	auto* T2S = TabBar->AddChildToHorizontalBox(TabPersonal);
	T2S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	TabBrowse = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("TabBrowse"));
	UTextBlock* T3 = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	T3->SetText(FText::FromString(TEXT("Explore")));
	T3->SetColorAndOpacity(FSlateColor(TextColor));
	{ FSlateFontInfo F = T3->GetFont(); F.Size = 11; T3->SetFont(F); }
	TabBrowse->AddChild(T3);
	TabBrowse->OnClicked.AddDynamic(this, &UCometChatPanel::OnTabBrowseClicked);
	auto* T3S = TabBar->AddChildToHorizontalBox(TabBrowse);
	T3S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	// Search bar (styled, compact)
	USizeBox* SearchSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("SearchSB"));
	SearchSB->SetMaxDesiredHeight(28);
	SearchInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("SearchInput"));
	SearchInput->SetHintText(FText::FromString(TEXT("Search...")));
	SearchInput->SetForegroundColor(SearchBarTextColor);
	{
		FSlateBrush BgBrush;
		BgBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		BgBrush.TintColor = FSlateColor(SearchBarColor);
		BgBrush.OutlineSettings.CornerRadii = FVector4(10, 10, 10, 10);
		BgBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		BgBrush.OutlineSettings.Color = FSlateColor(FLinearColor(0.2f, 0.3f, 0.45f, 0.4f));
		BgBrush.OutlineSettings.Width = 1.0f;
		SearchInput->WidgetStyle.BackgroundImageNormal = BgBrush;
		SearchInput->WidgetStyle.BackgroundImageHovered = BgBrush;
		SearchInput->WidgetStyle.BackgroundImageFocused = BgBrush;
		SearchInput->WidgetStyle.BackgroundImageReadOnly = BgBrush;
		SearchInput->WidgetStyle.ForegroundColor = FSlateColor(SearchBarTextColor);
		SearchInput->WidgetStyle.TextStyle.Font.Size = static_cast<int32>(SearchBarFontSize);
		SearchInput->WidgetStyle.Padding = FMargin(8, 2);
	}
	SearchInput->OnTextChanged.AddDynamic(this, &UCometChatPanel::OnSearchTextChanged);
	SearchInput->OnTextCommitted.AddDynamic(this, &UCometChatPanel::OnSearchCommitted);
	SearchSB->AddChild(SearchInput);
	auto* SearchSlot = MainVBox->AddChildToVerticalBox(SearchSB);
	SearchSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	SearchSlot->SetPadding(FMargin(4, 2, 4, 4));

	// 3 scroll boxes (only one visible at a time via visibility toggle)
	MyGroupsList = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("MyGroupsList"));
	MyGroupsList->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	MyGroupsList->SetConsumeMouseWheel(EConsumeMouseWheel::Always);
	auto* MGL = MainVBox->AddChildToVerticalBox(MyGroupsList);
	MGL->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MGL->SetPadding(FMargin(4, 4, 4, 0));
	MGL->SetHorizontalAlignment(HAlign_Fill);

	PersonalList = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("PersonalList"));
	PersonalList->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	PersonalList->SetConsumeMouseWheel(EConsumeMouseWheel::Always);
	PersonalList->SetVisibility(ESlateVisibility::Collapsed);
	auto* PL = MainVBox->AddChildToVerticalBox(PersonalList);
	PL->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	PL->SetHorizontalAlignment(HAlign_Fill);

	BrowseGroupsList = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("BrowseList"));
	BrowseGroupsList->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	BrowseGroupsList->SetConsumeMouseWheel(EConsumeMouseWheel::Always);
	BrowseGroupsList->SetVisibility(ESlateVisibility::Collapsed);
	auto* BL = MainVBox->AddChildToVerticalBox(BrowseGroupsList);
	BL->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BL->SetHorizontalAlignment(HAlign_Fill);

	// Chat view container (hidden until a chat is opened)
	ChatContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ChatContainer"));
	ChatContainer->SetVisibility(ESlateVisibility::Collapsed);
	auto* CVS = MainVBox->AddChildToVerticalBox(ChatContainer);
	CVS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	// Chat header (back + title + online status)
	UBorder* ChatHeadBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	ChatHeadBorder->SetPadding(FMargin(8, 6, 8, 6));
	{ FSlateBrush HB; HB.DrawAs = ESlateBrushDrawType::RoundedBox; HB.TintColor = FSlateColor(FLinearColor(0.03f, 0.04f, 0.06f, 0.9f)); HB.OutlineSettings.CornerRadii = FVector4(6,6,6,6); HB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; HB.OutlineSettings.Color = FSlateColor(FLinearColor(0.15f, 0.2f, 0.3f, 0.5f)); HB.OutlineSettings.Width = 1.0f; ChatHeadBorder->SetBrush(HB); }
	auto* CHBSlot = ChatContainer->AddChildToVerticalBox(ChatHeadBorder);
	CHBSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	CHBSlot->SetPadding(FMargin(0, 0, 0, 4));
	UHorizontalBox* ChatHead = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	ChatHeadBorder->AddChild(ChatHead);
	BackButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Back"));
	BackButton->OnClicked.AddDynamic(this, &UCometChatPanel::OnBackClicked);
	{ FButtonStyle BS = BackButton->GetStyle(); FSlateBrush T; T.DrawAs = ESlateBrushDrawType::NoDrawType; BS.Normal = T; BS.Hovered = T; BS.Pressed = T; BackButton->SetStyle(BS); }
	UTextBlock* BackTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	BackTxt->SetText(FText::FromString(TEXT("<")));
	BackTxt->SetColorAndOpacity(FSlateColor(AccentColor));
	{ FSlateFontInfo F = BackTxt->GetFont(); F.Size = 18; BackTxt->SetFont(F); }
	BackButton->AddChild(BackTxt);
	auto* BackSlot = ChatHead->AddChildToHorizontalBox(BackButton);
	BackSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	BackSlot->SetVerticalAlignment(VAlign_Center);
	BackSlot->SetPadding(FMargin(0, 0, 8, 0));
	ChatHeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ChatTitle"));
	ChatHeaderText->SetText(FText::FromString(TEXT("Chat")));
	ChatHeaderText->SetColorAndOpacity(FSlateColor(TextColor));
	{ FSlateFontInfo F = ChatHeaderText->GetFont(); F.Size = 16; ChatHeaderText->SetFont(F); }
	auto* CHS = ChatHead->AddChildToHorizontalBox(ChatHeaderText);
	CHS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	CHS->SetVerticalAlignment(VAlign_Center);

	// Chat messages
	ChatMessages = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ChatMsgs"));
	ChatMessages->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	auto* CMS = ChatContainer->AddChildToVerticalBox(ChatMessages);
	CMS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	// Typing indicator (above composer)
	ChatTypingText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TypingText"));
	ChatTypingText->SetText(FText::GetEmpty());
	ChatTypingText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.7f, 0.9f, 0.8f)));
	{ FSlateFontInfo F = ChatTypingText->GetFont(); F.Size = 10; ChatTypingText->SetFont(F); }
	ChatTypingText->SetVisibility(ESlateVisibility::Collapsed);
	auto* TypSlot = ChatContainer->AddChildToVerticalBox(ChatTypingText);
	TypSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	TypSlot->SetPadding(FMargin(8, 2, 0, 0));

	// Chat composer (input + send) — styled
	UBorder* ComposerBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	ComposerBorder->SetPadding(FMargin(8, 6));
	{ FSlateBrush CB; CB.DrawAs = ESlateBrushDrawType::RoundedBox; CB.TintColor = FSlateColor(FLinearColor(0.06f, 0.07f, 0.1f, 0.9f)); CB.OutlineSettings.CornerRadii = FVector4(8,8,8,8); CB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; CB.OutlineSettings.Color = FSlateColor(FLinearColor(0.2f, 0.25f, 0.35f, 0.5f)); CB.OutlineSettings.Width = 1.0f; ComposerBorder->SetBrush(CB); }
	auto* CPS = ChatContainer->AddChildToVerticalBox(ComposerBorder);
	CPS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	CPS->SetPadding(FMargin(0, 4, 0, 0));
	UHorizontalBox* Composer = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	ComposerBorder->AddChild(Composer);

	ChatInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("ChatInput"));
	ChatInput->SetHintText(FText::FromString(TEXT("Type a message...")));
	ChatInput->SetForegroundColor(SearchBarTextColor);
	{
		FSlateBrush BgBrush;
		BgBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		BgBrush.TintColor = FSlateColor(SearchBarColor);
		BgBrush.OutlineSettings.CornerRadii = FVector4(8, 8, 8, 8);
		BgBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		BgBrush.OutlineSettings.Color = FSlateColor(FLinearColor(0, 0, 0, 0));
		BgBrush.OutlineSettings.Width = 0.0f;
		ChatInput->WidgetStyle.BackgroundImageNormal = BgBrush;
		ChatInput->WidgetStyle.BackgroundImageHovered = BgBrush;
		ChatInput->WidgetStyle.BackgroundImageFocused = BgBrush;
		ChatInput->WidgetStyle.BackgroundImageReadOnly = BgBrush;
		ChatInput->WidgetStyle.ForegroundColor = FSlateColor(SearchBarTextColor);
		ChatInput->WidgetStyle.TextStyle.Font.Size = static_cast<int32>(SearchBarFontSize);
		ChatInput->WidgetStyle.Padding = FMargin(8, 4);
	}
	ChatInput->OnTextCommitted.AddDynamic(this, &UCometChatPanel::OnChatInputCommitted);
	ChatInput->OnTextChanged.AddDynamic(this, &UCometChatPanel::OnChatInputTextChanged);
	auto* CIS = Composer->AddChildToHorizontalBox(ChatInput);
	CIS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	ChatSendButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ChatSend"));
	ChatSendButton->OnClicked.AddDynamic(this, &UCometChatPanel::OnChatSendClicked);
	{ FButtonStyle BS = ChatSendButton->GetStyle(); FSlateBrush SB; SB.DrawAs = ESlateBrushDrawType::RoundedBox; SB.TintColor = FSlateColor(AccentColor); SB.OutlineSettings.CornerRadii = FVector4(10,10,10,10); SB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; BS.Normal = SB; BS.Hovered = SB; BS.Pressed = SB; BS.NormalPadding = FMargin(8, 3); BS.PressedPadding = FMargin(8, 3); ChatSendButton->SetStyle(BS); }
	UTextBlock* SendTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SendTxt->SetText(FText::FromString(TEXT("Send")));
	SendTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	{ FSlateFontInfo F = SendTxt->GetFont(); F.Size = 9; SendTxt->SetFont(F); }
	ChatSendButton->AddChild(SendTxt);
	auto* SendS = Composer->AddChildToHorizontalBox(ChatSendButton);
	SendS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	SendS->SetPadding(FMargin(6, 0, 0, 0));

	// Apply initial tab styles
	UpdateTabStyles();
}

// Old methods kept as empty stubs (functionality moved to BuildUI)
void UCometChatPanel::BuildTabBar() {}
void UCometChatPanel::BuildListViews() {}
void UCometChatPanel::BuildChatView() {}
// ============================================================
// SDK Init & Data
// ============================================================

void UCometChatPanel::InitSDK()
{
	auto* Sub = GetSubsystem();
	if (!Sub) return;

	// Don't configure SDK here — let the login node or GroupChatBox handle it
	// Panel only works when SDK is already configured and logged in
	if (!Sub->GetSdk() || !Sub->IsLoggedIn())
	{
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: SDK not ready or not logged in, retrying in 1s..."));
		// Retry after a short delay
		if (UWorld* World = GetWorld())
		{
			FTimerHandle RetryHandle;
			World->GetTimerManager().SetTimer(RetryHandle, [this]() { InitSDK(); }, 1.0f, false);
		}
		return;
	}

	Sub->OnTextMessageReceived.AddDynamic(this, &UCometChatPanel::HandleTextMessage);
	Sub->OnUserOnline.AddDynamic(this, &UCometChatPanel::HandleUserOnline);
	Sub->OnUserOffline.AddDynamic(this, &UCometChatPanel::HandleUserOffline);
	Sub->OnTypingStarted.AddDynamic(this, &UCometChatPanel::HandleTypingStarted);
	Sub->OnTypingEnded.AddDynamic(this, &UCometChatPanel::HandleTypingEnded);

	// Auto-populate UserUid from the logged-in user if not manually set
	if (UserUid.IsEmpty())
	{
		FCometChatUser LoggedInUser = Sub->GetLoggedInUser();
		UserUid = LoggedInUser.Uid;
	}
	FString ConnStatus = UTF8_TO_TCHAR(Sub->GetSdk()->get_connection_status().c_str());
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: SDK ready, UserUid=%s, WebSocket=%s, fetching data..."), *UserUid, *ConnStatus);

	// Fetch all tabs in parallel (async, non-blocking via curl_multi)
	FetchMyGroups();
	FetchPersonalChats();
	FetchBrowseGroups();
}

void UCometChatPanel::FetchMyGroups()
{
	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk()) { UE_LOG(LogTemp, Warning, TEXT("CometChatPanel: No SDK available for FetchMyGroups")); return; }
	if (!Sub->IsLoggedIn()) { UE_LOG(LogTemp, Warning, TEXT("CometChatPanel: Not logged in, skipping FetchMyGroups")); return; }
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: FetchMyGroups starting... IsAuthenticated=%d"), Sub->GetSdk()->is_authenticated() ? 1 : 0);
	chatsdk::GroupsRequestBuilder req;
	req.limit = 50;
	req.joined_only = true;
	Sub->GetSdk()->fetch_groups(req, [this](const std::string& err, const std::vector<chatsdk::Group>& groups) {
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: FetchMyGroups callback - err='%hs', groups=%d"), err.c_str(), (int)groups.size());
		AsyncTask(ENamedThreads::GameThread, [this, groups]() {
			if (!MyGroupsList) return;
			MyGroupsList->ClearChildren();
			MyGroups.Empty();
			for (auto& g : groups) {
				FCometChatGroup ug = CometChatConversions::ToUnreal(g);
				MyGroups.Add(ug);
				UWidget* Item = CreateGroupListItem(ug, false);
				MyGroupsList->AddChild(Item);
				// Force left-align / fill width
				if (UPanelSlot* PSlot = Item->Slot)
				{
					if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot))
					{
						SBSlot->SetHorizontalAlignment(HAlign_Fill);
					}
				}
			}
			UE_LOG(LogTemp, Log, TEXT("CometChatPanel: MyGroups populated with %d items"), MyGroups.Num());
		});
	});
}

void UCometChatPanel::FetchPersonalChats()
{
	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk()) return;
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: FetchPersonalChats starting..."));
	chatsdk::ConversationsRequestBuilder req;
	req.limit = 50;
	req.conversation_type = "user";
	Sub->GetSdk()->fetch_conversations(req, [this](const std::string& err, const std::vector<chatsdk::Conversation>& convs) {
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: FetchPersonalChats callback - err='%hs', convs=%d"), err.c_str(), (int)convs.size());
		AsyncTask(ENamedThreads::GameThread, [this, convs]() {
			if (!PersonalList) return;
			PersonalList->ClearChildren();
			PersonalConversations.Empty();
			for (auto& c : convs) {
				FCometChatConversation uc = CometChatConversions::ToUnreal(c);
				PersonalConversations.Add(uc);
				UWidget* Item = CreatePersonalListItem(uc);
				PersonalList->AddChild(Item);
				if (UPanelSlot* PSlot = Item->Slot) { if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot)) SBSlot->SetHorizontalAlignment(HAlign_Fill); }
			}
		});
	});
}

void UCometChatPanel::FetchBrowseGroups()
{
	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk()) return;
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: FetchBrowseGroups starting..."));
	chatsdk::GroupsRequestBuilder req;
	req.limit = 50;
	req.joined_only = false;
	Sub->GetSdk()->fetch_groups(req, [this](const std::string& err, const std::vector<chatsdk::Group>& groups) {
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: FetchBrowseGroups callback - err='%hs', groups=%d"), err.c_str(), (int)groups.size());
		AsyncTask(ENamedThreads::GameThread, [this, groups]() {
			if (!BrowseGroupsList) return;
			BrowseGroupsList->ClearChildren();
			BrowseableGroups.Empty();
			for (auto& g : groups) {
				FCometChatGroup ug = CometChatConversions::ToUnreal(g);
				if (!ug.bHasJoined) {
					BrowseableGroups.Add(ug);
					UWidget* Item = CreateGroupListItem(ug, true);
					BrowseGroupsList->AddChild(Item);
					if (UPanelSlot* PSlot = Item->Slot) { if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot)) SBSlot->SetHorizontalAlignment(HAlign_Fill); }
				}
			}
		});
	});
}

// ============================================================
// List Item Creation (Game HUD style)
// ============================================================

static FString FormatTimestamp(int64 UnixTimestamp)
{
	if (UnixTimestamp <= 0) return TEXT("");
	FDateTime Time = FDateTime::FromUnixTimestamp(UnixTimestamp);
	FDateTime Now = FDateTime::Now();
	FTimespan Diff = Now - Time;
	if (Diff.GetTotalHours() < 24 && Time.GetDay() == Now.GetDay())
		return Time.ToString(TEXT("%H:%M"));
	if (Diff.GetTotalHours() < 48)
		return TEXT("Yesterday");
	return Time.ToString(TEXT("%d/%m"));
}

UWidget* UCometChatPanel::CreateGroupListItem(const FCometChatGroup& Group, bool bShowJoinButton)
{
	// The button IS the item — styled directly, no inner border wrapper
	UButton* ItemBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	{
		FButtonStyle BS = ItemBtn->GetStyle();
		FSlateBrush Normal;
		Normal.DrawAs = ESlateBrushDrawType::RoundedBox;
		Normal.TintColor = FSlateColor(FLinearColor(0.04f, 0.05f, 0.08f, 0.85f));
		Normal.OutlineSettings.CornerRadii = FVector4(6, 6, 6, 6);
		Normal.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Normal.OutlineSettings.Color = FSlateColor(FLinearColor(0.15f, 0.25f, 0.4f, 0.5f));
		Normal.OutlineSettings.Width = 1.0f;
		Normal.Margin = FMargin(0.25f); // Ensures content fills
		FSlateBrush Hovered = Normal;
		Hovered.TintColor = FSlateColor(FLinearColor(0.08f, 0.1f, 0.16f, 0.95f));
		Hovered.OutlineSettings.Color = FSlateColor(AccentColor);
		Hovered.OutlineSettings.Width = 1.5f;
		BS.Normal = Normal;
		BS.Hovered = Hovered;
		BS.Pressed = Hovered;
		BS.NormalPadding = FMargin(12, 8);
		BS.PressedPadding = FMargin(12, 8);
		ItemBtn->SetStyle(BS);
	}

	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	ItemBtn->AddChild(Row);
	// Force left-align content inside button
	if (UButtonSlot* BtnSlotInner = Cast<UButtonSlot>(Row->Slot))
	{
		BtnSlotInner->SetHorizontalAlignment(HAlign_Fill);
		BtnSlotInner->SetVerticalAlignment(VAlign_Center);
	}

	// Avatar circle
	USizeBox* IconSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	IconSB->SetWidthOverride(38); IconSB->SetHeightOverride(38);
	UBorder* IconCircle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	{ FSlateBrush CB; CB.DrawAs = ESlateBrushDrawType::RoundedBox; CB.TintColor = FSlateColor(FLinearColor(0.0f, 0.3f, 0.6f, 0.8f)); CB.OutlineSettings.CornerRadii = FVector4(19,19,19,19); CB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; CB.OutlineSettings.Color = FSlateColor(AccentColor); CB.OutlineSettings.Width = 1.5f; IconCircle->SetBrush(CB); }
	IconSB->AddChild(IconCircle);
	UOverlay* IconOv = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
	IconCircle->AddChild(IconOv);
	UTextBlock* IconTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	IconTxt->SetText(FText::FromString(Group.Name.Left(1).ToUpper()));
	IconTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	IconTxt->SetJustification(ETextJustify::Center);
	{ FSlateFontInfo F = IconTxt->GetFont(); F.Size = 15; IconTxt->SetFont(F); }
	auto* IOS = IconOv->AddChildToOverlay(IconTxt); IOS->SetHorizontalAlignment(HAlign_Center); IOS->SetVerticalAlignment(VAlign_Center);
	auto* IconSlot = Row->AddChildToHorizontalBox(IconSB);
	IconSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	IconSlot->SetVerticalAlignment(VAlign_Center);
	IconSlot->SetPadding(FMargin(0, 0, 10, 0));

	// Info (name + members)
	UVerticalBox* InfoVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UTextBlock* NameTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NameTxt->SetText(FText::FromString(Group.Name));
	NameTxt->SetColorAndOpacity(FSlateColor(TextColor));
	{ FSlateFontInfo F = NameTxt->GetFont(); F.Size = static_cast<int32>(FontSize); NameTxt->SetFont(F); }
	InfoVBox->AddChildToVerticalBox(NameTxt);
	UTextBlock* SubTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SubTxt->SetText(FText::FromString(FString::Printf(TEXT("%d members | %s"), Group.MembersCount, *Group.Type)));
	SubTxt->SetColorAndOpacity(FSlateColor(SubTextColor));
	{ FSlateFontInfo F = SubTxt->GetFont(); F.Size = static_cast<int32>(SmallFontSize); SubTxt->SetFont(F); }
	InfoVBox->AddChildToVerticalBox(SubTxt);
	auto* InfoSlot = Row->AddChildToHorizontalBox(InfoVBox);
	InfoSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	InfoSlot->SetVerticalAlignment(VAlign_Center);

	if (bShowJoinButton)
	{
		UButton* JoinBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		{ FButtonStyle JS = JoinBtn->GetStyle(); FSlateBrush JB; JB.DrawAs = ESlateBrushDrawType::RoundedBox; JB.TintColor = FSlateColor(JoinButtonColor); JB.OutlineSettings.CornerRadii = FVector4(12,12,12,12); JB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; JS.Normal = JB; JS.Hovered = JB; JS.Pressed = JB; JoinBtn->SetStyle(JS); }
		UTextBlock* JTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		JTxt->SetText(FText::FromString(TEXT("JOIN"))); JTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		{ FSlateFontInfo F = JTxt->GetFont(); F.Size = 10; JTxt->SetFont(F); }
		JoinBtn->AddChild(JTxt);
		USizeBox* JSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass()); JSB->SetWidthOverride(56); JSB->SetHeightOverride(26); JSB->AddChild(JoinBtn);
		auto* JSlot = Row->AddChildToHorizontalBox(JSB); JSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic)); JSlot->SetVerticalAlignment(VAlign_Center);
	}

	if (!bShowJoinButton) ItemBtn->OnClicked.AddDynamic(this, &UCometChatPanel::OnMyGroupItemClicked);
	else ItemBtn->OnClicked.AddDynamic(this, &UCometChatPanel::OnJoinButtonClicked);

	// Spacer for gap between items
	UVerticalBox* Wrapper = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	auto* BtnSlot = Wrapper->AddChildToVerticalBox(ItemBtn);
	BtnSlot->SetPadding(FMargin(0, 0, 0, 4));
	BtnSlot->SetHorizontalAlignment(HAlign_Fill);
	return Wrapper;
}

UWidget* UCometChatPanel::CreatePersonalListItem(const FCometChatConversation& Conv)
{
	FString DisplayName = Conv.ConversationWithUser.Name.IsEmpty() ? Conv.ConversationWithUser.Uid : Conv.ConversationWithUser.Name;
	bool bOnline = OnlineUsers.Contains(Conv.ConversationWithUser.Uid) && OnlineUsers[Conv.ConversationWithUser.Uid];

	// Button IS the item
	UButton* ItemBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	{
		FButtonStyle BS = ItemBtn->GetStyle();
		FSlateBrush Normal;
		Normal.DrawAs = ESlateBrushDrawType::RoundedBox;
		Normal.TintColor = FSlateColor(FLinearColor(0.04f, 0.05f, 0.08f, 0.85f));
		Normal.OutlineSettings.CornerRadii = FVector4(6, 6, 6, 6);
		Normal.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Normal.OutlineSettings.Color = FSlateColor(FLinearColor(0.15f, 0.25f, 0.4f, 0.5f));
		Normal.OutlineSettings.Width = 1.0f;
		Normal.Margin = FMargin(0.25f);
		FSlateBrush Hovered = Normal;
		Hovered.TintColor = FSlateColor(FLinearColor(0.08f, 0.1f, 0.16f, 0.95f));
		Hovered.OutlineSettings.Color = FSlateColor(AccentColor);
		Hovered.OutlineSettings.Width = 1.5f;
		BS.Normal = Normal; BS.Hovered = Hovered; BS.Pressed = Hovered;
		BS.NormalPadding = FMargin(12, 8); BS.PressedPadding = FMargin(12, 8);
		ItemBtn->SetStyle(BS);
	}

	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	ItemBtn->AddChild(Row);
	// Force left-align content inside button
	if (UButtonSlot* BtnSlotInner = Cast<UButtonSlot>(Row->Slot))
	{
		BtnSlotInner->SetHorizontalAlignment(HAlign_Fill);
		BtnSlotInner->SetVerticalAlignment(VAlign_Center);
	}

	// Avatar with online ring
	UOverlay* AvOv = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
	USizeBox* AvSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass()); AvSB->SetWidthOverride(38); AvSB->SetHeightOverride(38);
	UBorder* AvCircle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	{ FSlateBrush AB; AB.DrawAs = ESlateBrushDrawType::RoundedBox; AB.TintColor = FSlateColor(FLinearColor(0.2f, 0.25f, 0.35f, 1.0f)); AB.OutlineSettings.CornerRadii = FVector4(19,19,19,19); AB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; AB.OutlineSettings.Color = FSlateColor(bOnline ? OnlineColor : FLinearColor(0.3f, 0.35f, 0.4f, 0.6f)); AB.OutlineSettings.Width = bOnline ? 2.0f : 1.0f; AvCircle->SetBrush(AB); }
	AvSB->AddChild(AvCircle);
	UOverlay* InitOv = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass()); AvCircle->AddChild(InitOv);
	UTextBlock* InitTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	InitTxt->SetText(FText::FromString(DisplayName.Left(1).ToUpper()));
	InitTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White)); InitTxt->SetJustification(ETextJustify::Center);
	{ FSlateFontInfo F = InitTxt->GetFont(); F.Size = 15; InitTxt->SetFont(F); }
	auto* AIS = InitOv->AddChildToOverlay(InitTxt); AIS->SetHorizontalAlignment(HAlign_Center); AIS->SetVerticalAlignment(VAlign_Center);
	AvOv->AddChildToOverlay(AvSB);
	if (bOnline) { USizeBox* DSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass()); DSB->SetWidthOverride(11); DSB->SetHeightOverride(11); UBorder* Dot = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass()); FSlateBrush DB; DB.DrawAs = ESlateBrushDrawType::RoundedBox; DB.TintColor = FSlateColor(OnlineColor); DB.OutlineSettings.CornerRadii = FVector4(6,6,6,6); DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; DB.OutlineSettings.Color = FSlateColor(PanelBackground); DB.OutlineSettings.Width = 2.0f; Dot->SetBrush(DB); DSB->AddChild(Dot); auto* DS = AvOv->AddChildToOverlay(DSB); DS->SetHorizontalAlignment(HAlign_Right); DS->SetVerticalAlignment(VAlign_Bottom); }
	auto* AvSlot = Row->AddChildToHorizontalBox(AvOv); AvSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic)); AvSlot->SetVerticalAlignment(VAlign_Center); AvSlot->SetPadding(FMargin(0, 0, 10, 0));

	// Name + last message
	UVerticalBox* InfoVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UTextBlock* NameTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NameTxt->SetText(FText::FromString(DisplayName)); NameTxt->SetColorAndOpacity(FSlateColor(TextColor));
	{ FSlateFontInfo F = NameTxt->GetFont(); F.Size = static_cast<int32>(FontSize); NameTxt->SetFont(F); }
	InfoVBox->AddChildToVerticalBox(NameTxt);
	FString LastMsg = Conv.LastMessage.Text.IsEmpty() ? TEXT("No messages yet") : Conv.LastMessage.Text.Left(35);
	UTextBlock* MsgTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MsgTxt->SetText(FText::FromString(LastMsg)); MsgTxt->SetColorAndOpacity(FSlateColor(SubTextColor));
	{ FSlateFontInfo F = MsgTxt->GetFont(); F.Size = static_cast<int32>(SmallFontSize); MsgTxt->SetFont(F); }
	InfoVBox->AddChildToVerticalBox(MsgTxt);
	auto* InfoSlot = Row->AddChildToHorizontalBox(InfoVBox); InfoSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill)); InfoSlot->SetVerticalAlignment(VAlign_Center);

	// Right: timestamp + unread
	UVerticalBox* RightVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	FString TimeStr = FormatTimestamp(Conv.UpdatedAt);
	if (!TimeStr.IsEmpty()) { UTextBlock* TT = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()); TT->SetText(FText::FromString(TimeStr)); TT->SetColorAndOpacity(FSlateColor(SubTextColor)); TT->SetJustification(ETextJustify::Right); FSlateFontInfo F = TT->GetFont(); F.Size = 10; TT->SetFont(F); RightVBox->AddChildToVerticalBox(TT); }
	if (Conv.UnreadMessageCount > 0) { UBorder* Badge = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass()); FSlateBrush BB; BB.DrawAs = ESlateBrushDrawType::RoundedBox; BB.TintColor = FSlateColor(AccentColor); BB.OutlineSettings.CornerRadii = FVector4(9,9,9,9); BB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; Badge->SetBrush(BB); Badge->SetPadding(FMargin(5, 2)); UTextBlock* BT = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()); BT->SetText(FText::FromString(FString::FromInt(Conv.UnreadMessageCount))); BT->SetColorAndOpacity(FSlateColor(FLinearColor::White)); BT->SetJustification(ETextJustify::Center); FSlateFontInfo F = BT->GetFont(); F.Size = 9; BT->SetFont(F); Badge->AddChild(BT); auto* BSlot = RightVBox->AddChildToVerticalBox(Badge); BSlot->SetHorizontalAlignment(HAlign_Right); BSlot->SetPadding(FMargin(0, 3, 0, 0)); }
	auto* RSlot = Row->AddChildToHorizontalBox(RightVBox); RSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic)); RSlot->SetVerticalAlignment(VAlign_Center); RSlot->SetPadding(FMargin(8, 0, 0, 0));

	ItemBtn->OnClicked.AddDynamic(this, &UCometChatPanel::OnPersonalItemClicked);

	// Spacer wrapper
	UVerticalBox* Wrapper = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	auto* BtnSlot = Wrapper->AddChildToVerticalBox(ItemBtn);
	BtnSlot->SetPadding(FMargin(0, 0, 0, 4));
	BtnSlot->SetHorizontalAlignment(HAlign_Fill);
	return Wrapper;
}

// ============================================================
// Tab Switching
// ============================================================

void UCometChatPanel::SwitchTab(EActiveTab Tab)
{
	ActiveTab = Tab;
	CurrentView = EPanelView::List;
	// Visibility-based tab switching (no WidgetSwitcher)
	if (MyGroupsList) MyGroupsList->SetVisibility(Tab == EActiveTab::MyGroups ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (PersonalList) PersonalList->SetVisibility(Tab == EActiveTab::Personal ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (BrowseGroupsList) BrowseGroupsList->SetVisibility(Tab == EActiveTab::BrowseGroups ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (TabBar) TabBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (ChatContainer) ChatContainer->SetVisibility(ESlateVisibility::Collapsed);
	UpdateTabStyles();
}

void UCometChatPanel::UpdateTabStyles()
{
	auto SetTabColor = [this](UButton* Btn, bool bActive)
	{
		if (!Btn) return;
		FButtonStyle Style = Btn->GetStyle();
		FSlateBrush B;
		B.DrawAs = ESlateBrushDrawType::RoundedBox;
		B.TintColor = FSlateColor(bActive ? FLinearColor(0.0f, 0.4f, 0.8f, 0.15f) : FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
		B.OutlineSettings.CornerRadii = FVector4(4, 4, 4, 4);
		B.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		if (bActive) { B.OutlineSettings.Color = FSlateColor(AccentColor); B.OutlineSettings.Width = 0.0f; }
		Style.Normal = B; Style.Hovered = B; Style.Pressed = B;
		Style.NormalPadding = FMargin(8, 6);
		Style.PressedPadding = FMargin(8, 6);
		Btn->SetStyle(Style);
		// Update text color
		if (UTextBlock* Txt = Cast<UTextBlock>(Btn->GetChildAt(0)))
		{
			Txt->SetColorAndOpacity(FSlateColor(bActive ? AccentColor : SubTextColor));
		}
	};
	SetTabColor(TabMyGroups, ActiveTab == EActiveTab::MyGroups);
	SetTabColor(TabPersonal, ActiveTab == EActiveTab::Personal);
	SetTabColor(TabBrowse, ActiveTab == EActiveTab::BrowseGroups);
}

void UCometChatPanel::ShowChatView(const FString& Id, const FString& Type, const FString& Name)
{
	// End typing in previous chat if any
	SendTypingEnded();
	CurrentTypers.Empty();

	ActiveChatId = Id;
	ActiveChatType = Type;
	ActiveChatName = Name;
	CurrentView = EPanelView::Chat;
	// Hide lists and tabs, show chat
	if (MyGroupsList) MyGroupsList->SetVisibility(ESlateVisibility::Collapsed);
	if (PersonalList) PersonalList->SetVisibility(ESlateVisibility::Collapsed);
	if (BrowseGroupsList) BrowseGroupsList->SetVisibility(ESlateVisibility::Collapsed);
	if (TabBar) TabBar->SetVisibility(ESlateVisibility::Collapsed);
	if (ChatContainer) ChatContainer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (ChatHeaderText) ChatHeaderText->SetText(FText::FromString(Name));
	if (ChatMessages) ChatMessages->ClearChildren();
	if (SearchInput) { SearchInput->SetText(FText::GetEmpty()); SearchInput->SetHintText(FText::FromString(TEXT("Search messages..."))); }
	SearchQuery.Empty();
	ChatMessageIds.Empty();
	ChatMessageCount = 0;
	LoadChatHistory();
}

void UCometChatPanel::ShowListView()
{
	// Send end_typing if we were typing
	SendTypingEnded();
	CurrentTypers.Empty();

	CurrentView = EPanelView::List;
	ActiveChatId.Empty();
	if (TabBar) TabBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (ChatContainer) ChatContainer->SetVisibility(ESlateVisibility::Collapsed);
	if (SearchInput) { SearchInput->SetText(FText::GetEmpty()); SearchInput->SetHintText(FText::FromString(TEXT("Search..."))); }
	SearchQuery.Empty();
	// Restore active tab visibility
	if (MyGroupsList) MyGroupsList->SetVisibility(ActiveTab == EActiveTab::MyGroups ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (PersonalList) PersonalList->SetVisibility(ActiveTab == EActiveTab::Personal ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (BrowseGroupsList) BrowseGroupsList->SetVisibility(ActiveTab == EActiveTab::BrowseGroups ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	UpdateTabStyles();
}

// ============================================================
// Chat
// ============================================================

void UCometChatPanel::LoadChatHistory()
{
	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk()) return;

	// Build request with filters
	chatsdk::MessagesRequestBuilder req;
	req.limit = 30;
	if (ActiveChatType == TEXT("group"))
		req.guid = TCHAR_TO_UTF8(*ActiveChatId);
	else
		req.uid = TCHAR_TO_UTF8(*ActiveChatId);

	// Type filters
	std::vector<std::string> types;
	if (bShowTextMessages) types.push_back("text");
	if (bShowImageMessages) types.push_back("image");
	if (bShowVideoMessages) types.push_back("video");
	if (bShowAudioMessages) types.push_back("audio");
	if (bShowFileMessages) types.push_back("file");
	if (!types.empty()) req.types = types;

	// Category filters
	std::vector<std::string> categories;
	categories.push_back("message"); // always include base messages
	if (bShowCustomMessages) categories.push_back("custom");
	if (bShowActionMessages) categories.push_back("action");
	if (bShowCallMessages) categories.push_back("call");
	req.categories = categories;

	// Visibility filters
	req.hide_messages_from_blocked_users = bHideBlockedUserMessages;
	req.hide_deleted = bHideDeletedMessages;
	req.hide_replies = bHideReplies;

	// Content filters
	req.has_attachments = bOnlyWithAttachments;
	req.has_links = bOnlyWithLinks;
	req.has_mentions = bOnlyWithMentions;
	req.has_reactions = bOnlyWithReactions;
	req.unread = bOnlyUnread;

	Sub->GetSdk()->fetch_previous_messages(req,
		[this](const std::string& err, const std::vector<chatsdk::Message>& msgs) {
			AsyncTask(ENamedThreads::GameThread, [this, msgs]() {
				for (size_t i = 0; i < msgs.size(); ++i)
					AddChatMessage(CometChatConversions::ToUnreal(msgs[i]));
				if (ChatMessages) ChatMessages->ScrollToEnd();
			});
		});
}

void UCometChatPanel::SendChatMessage()
{
	if (!ChatInput) return;
	FString Text = ChatInput->GetText().ToString().TrimStartAndEnd();
	if (Text.IsEmpty()) return;
	ChatInput->SetText(FText::GetEmpty());

	// Stop typing indicator since we're sending
	SendTypingEnded();

	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk()) return;

	if (ActiveChatType == TEXT("group"))
	{
		Sub->GetSdk()->send_group_message(TCHAR_TO_UTF8(*ActiveChatId), TCHAR_TO_UTF8(*Text),
			[this](const std::string& err, const chatsdk::Message& msg) {
				AsyncTask(ENamedThreads::GameThread, [this, msg]() {
					AddChatMessage(CometChatConversions::ToUnreal(msg));
					if (ChatMessages) ChatMessages->ScrollToEnd();
				});
			});
	}
	else
	{
		Sub->GetSdk()->send_message(TCHAR_TO_UTF8(*ActiveChatId), TCHAR_TO_UTF8(*Text),
			[this](const std::string& err, const chatsdk::Message& msg) {
				AsyncTask(ENamedThreads::GameThread, [this, msg]() {
					AddChatMessage(CometChatConversions::ToUnreal(msg));
					if (ChatMessages) ChatMessages->ScrollToEnd();
				});
			});
	}
}

void UCometChatPanel::LoadAvatarImage(const FString& Url, UImage* TargetImage, UTextBlock* FallbackText)
{
	if (Url.IsEmpty() || !TargetImage) return;

	// Check cache first
	if (TWeakObjectPtr<UTexture2DDynamic>* CachedPtr = AvatarCache.Find(Url))
	{
		if (CachedPtr->IsValid())
		{
			TargetImage->SetBrushFromTextureDynamic(CachedPtr->Get());
			TargetImage->SetDesiredSizeOverride(FVector2D(ChatAvatarSize, ChatAvatarSize));
			TargetImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (FallbackText) FallbackText->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
	}

	// Download the image
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));

	TWeakObjectPtr<UImage> WeakImage(TargetImage);
	TWeakObjectPtr<UTextBlock> WeakFallback(FallbackText);
	FString CacheKey = Url;

	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, WeakImage, WeakFallback, CacheKey](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
		{
			if (!bSuccess || !Response.IsValid() || Response->GetResponseCode() != 200) return;

			const TArray<uint8>& ImageData = Response->GetContent();
			if (ImageData.Num() == 0) return;

			// Detect image format
			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
			EImageFormat Format = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
			if (Format == EImageFormat::Invalid) return;

			TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);
			if (!ImageWrapper.IsValid()) return;
			if (!ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num())) return;

			TArray<uint8> RawData;
			if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData)) return;

			int32 Width = ImageWrapper->GetWidth();
			int32 Height = ImageWrapper->GetHeight();

			AsyncTask(ENamedThreads::GameThread, [this, WeakImage, WeakFallback, CacheKey, RawData, Width, Height]()
			{
				if (!WeakImage.IsValid()) return;

				UTexture2DDynamic* Texture = UTexture2DDynamic::Create(Width, Height);
				if (!Texture) return;

				Texture->SRGB = true;
				Texture->UpdateResource();

				FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
				if (TextureResource)
				{
					ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
						[TextureResource, RawData, Width, Height](FRHICommandListImmediate& RHICmdList)
						{
							TextureResource->WriteRawToTexture_RenderThread(RawData);
						});
				}

				AvatarCache.Add(CacheKey, Texture);

				WeakImage->SetBrushFromTextureDynamic(Texture);
				WeakImage->SetDesiredSizeOverride(FVector2D(ChatAvatarSize, ChatAvatarSize));
				WeakImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				if (WeakFallback.IsValid())
				{
					WeakFallback->SetVisibility(ESlateVisibility::Collapsed);
				}
			});
		});

	HttpRequest->ProcessRequest();
}

void UCometChatPanel::AddChatMessage(const FCometChatMessage& Message)
{
	if (!ChatMessages) return;

	// Deduplicate: skip if this message ID was already added
	if (!Message.Id.IsEmpty() && ChatMessageIds.Contains(Message.Id))
	{
		return;
	}
	if (!Message.Id.IsEmpty())
	{
		ChatMessageIds.Add(Message.Id);
	}

	bool bIsYou = (Message.SenderUid == UserUid);
	FString Name = Message.SenderName.IsEmpty() ? Message.SenderUid : Message.SenderName;
	if (Name.Len() > 10) Name = Name.Left(10);
	FString TimeStr;
	if (Message.SentAt > 0)
	{
		FDateTime Time = FDateTime::FromUnixTimestamp(Message.SentAt);
		TimeStr = Time.ToString(TEXT("%H:%M"));
	}

	// Single row: [Avatar] [Username] [Message] [Timestamp] — all top-left aligned
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

	// Avatar (circle with user image or fallback letter)
	USizeBox* AvSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	AvSB->SetWidthOverride(ChatAvatarSize); AvSB->SetHeightOverride(ChatAvatarSize);
	UOverlay* AvOv = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
	AvSB->AddChild(AvOv);

	float HalfAvatar = ChatAvatarSize * 0.5f;
	UBorder* AvCircle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	{ FSlateBrush AB; AB.DrawAs = ESlateBrushDrawType::RoundedBox; AB.TintColor = FSlateColor(bIsYou ? FLinearColor(0.0f, 0.35f, 0.7f, 0.9f) : FLinearColor(0.25f, 0.3f, 0.4f, 0.9f)); AB.OutlineSettings.CornerRadii = FVector4(HalfAvatar,HalfAvatar,HalfAvatar,HalfAvatar); AB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius; AvCircle->SetBrush(AB); }
	auto* CircleSlot = AvOv->AddChildToOverlay(AvCircle);
	CircleSlot->SetHorizontalAlignment(HAlign_Fill);
	CircleSlot->SetVerticalAlignment(VAlign_Fill);

	UImage* AvImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	AvImg->SetVisibility(ESlateVisibility::Collapsed);
	AvImg->SetDesiredSizeOverride(FVector2D(ChatAvatarSize, ChatAvatarSize));
	auto* ImgSlot = AvOv->AddChildToOverlay(AvImg);
	ImgSlot->SetHorizontalAlignment(HAlign_Fill);
	ImgSlot->SetVerticalAlignment(VAlign_Fill);

	UTextBlock* AvTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	AvTxt->SetText(FText::FromString(Name.Left(1).ToUpper()));
	AvTxt->SetColorAndOpacity(FSlateColor(FLinearColor::White)); AvTxt->SetJustification(ETextJustify::Center);
	{ FSlateFontInfo F = AvTxt->GetFont(); F.Size = FMath::Max(6, static_cast<int32>(ChatAvatarSize * 0.4f)); AvTxt->SetFont(F); }
	auto* AVS = AvOv->AddChildToOverlay(AvTxt); AVS->SetHorizontalAlignment(HAlign_Center); AVS->SetVerticalAlignment(VAlign_Center);

	if (!Message.SenderAvatar.IsEmpty())
	{
		LoadAvatarImage(Message.SenderAvatar, AvImg, AvTxt);
	}

	auto* AvSlot = Row->AddChildToHorizontalBox(AvSB);
	AvSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	AvSlot->SetVerticalAlignment(VAlign_Top);
	AvSlot->SetHorizontalAlignment(HAlign_Left);
	AvSlot->SetPadding(FMargin(0, 0, 6, 0));

	// Username
	UTextBlock* NameTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NameTxt->SetText(FText::FromString(Name));
	NameTxt->SetColorAndOpacity(FSlateColor(bIsYou ? ChatUsernameOwnColor : ChatUsernameOtherColor));
	{ FSlateFontInfo F = NameTxt->GetFont(); F.Size = static_cast<int32>(ChatUsernameFontSize); NameTxt->SetFont(F); }
	auto* NSlot = Row->AddChildToHorizontalBox(NameTxt);
	NSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	NSlot->SetVerticalAlignment(VAlign_Top);
	NSlot->SetHorizontalAlignment(HAlign_Left);
	NSlot->SetPadding(FMargin(0, 0, 6, 0));

	// Message text (fills remaining space, top-left aligned)
	UTextBlock* MsgTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MsgTxt->SetText(FText::FromString(Message.Text));
	MsgTxt->SetAutoWrapText(true);
	MsgTxt->SetColorAndOpacity(FSlateColor(ChatMessageTextColor));
	{ FSlateFontInfo F = MsgTxt->GetFont(); F.Size = static_cast<int32>(ChatMessageFontSize); MsgTxt->SetFont(F); }
	auto* MSlot = Row->AddChildToHorizontalBox(MsgTxt);
	MSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MSlot->SetVerticalAlignment(VAlign_Top);
	MSlot->SetHorizontalAlignment(HAlign_Left);

	// Timestamp
	if (!TimeStr.IsEmpty())
	{
		UTextBlock* TimeTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		TimeTxt->SetText(FText::FromString(TimeStr));
		TimeTxt->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.55f, 0.6f, 0.7f)));
		{ FSlateFontInfo F = TimeTxt->GetFont(); F.Size = 8; TimeTxt->SetFont(F); }
		auto* TSlot = Row->AddChildToHorizontalBox(TimeTxt);
		TSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		TSlot->SetVerticalAlignment(VAlign_Top);
		TSlot->SetHorizontalAlignment(HAlign_Left);
		TSlot->SetPadding(FMargin(6, 0, 0, 0));
	}

	// Add with vertical padding between messages
	ChatMessages->AddChild(Row);
	if (UPanelSlot* PSlot = Row->Slot)
	{
		if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot))
		{
			SBSlot->SetPadding(FMargin(4, 4, 4, 4));
		}
	}

	ChatMessageIds.Add(Message.Id);
	ChatMessageCount++;
	EnforceChatMessageLimit();
}

void UCometChatPanel::EnforceChatMessageLimit()
{
	while (ChatMessageCount > 100 && ChatMessages && ChatMessages->GetChildrenCount() > 0)
	{
		ChatMessages->RemoveChildAt(0);
		ChatMessageCount--;
		if (ChatMessageIds.Num() > 0) ChatMessageIds.RemoveAt(0);
	}
}

// ============================================================
// Handlers
// ============================================================

void UCometChatPanel::OnTabMyGroupsClicked() { SwitchTab(EActiveTab::MyGroups); }
void UCometChatPanel::OnTabPersonalClicked() { SwitchTab(EActiveTab::Personal); }
void UCometChatPanel::OnTabBrowseClicked() { SwitchTab(EActiveTab::BrowseGroups); }
void UCometChatPanel::OnBackClicked() { ShowListView(); }
void UCometChatPanel::OnChatSendClicked() { SendChatMessage(); }
void UCometChatPanel::OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter) SendChatMessage();
}

void UCometChatPanel::OnMyGroupItemClicked()
{
	if (!MyGroupsList) return;
	for (int32 i = 0; i < MyGroupsList->GetChildrenCount(); ++i)
	{
		UVerticalBox* Wrapper = Cast<UVerticalBox>(MyGroupsList->GetChildAt(i));
		UButton* Btn = Wrapper ? Cast<UButton>(Wrapper->GetChildAt(0)) : nullptr;
		if (Btn && (Btn->IsPressed() || Btn->IsHovered()))
		{
			if (MyGroups.IsValidIndex(i))
			{
				OnGroupItemClicked(MyGroups[i].Guid, MyGroups[i].Name);
				return;
			}
		}
	}
}

void UCometChatPanel::OnPersonalItemClicked()
{
	if (!PersonalList) return;
	for (int32 i = 0; i < PersonalList->GetChildrenCount() && i < PersonalConversations.Num(); ++i)
	{
		UVerticalBox* Wrapper = Cast<UVerticalBox>(PersonalList->GetChildAt(i));
		UButton* Btn = Wrapper ? Cast<UButton>(Wrapper->GetChildAt(0)) : nullptr;
		if (Btn && (Btn->IsPressed() || Btn->IsHovered()))
		{
			FString Uid = PersonalConversations[i].ConversationWithUser.Uid;
			FString Name = PersonalConversations[i].ConversationWithUser.Name;
			if (Name.IsEmpty()) Name = Uid;
			OnPersonalItemClicked(Uid, Name);
			return;
		}
	}
}

void UCometChatPanel::OnJoinButtonClicked()
{
	if (!BrowseGroupsList) return;
	for (int32 i = 0; i < BrowseGroupsList->GetChildrenCount() && i < BrowseableGroups.Num(); ++i)
	{
		UVerticalBox* Wrapper = Cast<UVerticalBox>(BrowseGroupsList->GetChildAt(i));
		UButton* Btn = Wrapper ? Cast<UButton>(Wrapper->GetChildAt(0)) : nullptr;
		if (Btn && (Btn->IsPressed() || Btn->IsHovered()))
		{
			OnJoinGroupClicked(BrowseableGroups[i].Guid);
			return;
		}
	}
}

void UCometChatPanel::OnGroupItemClicked(FString Guid, FString Name)
{
	ShowChatView(Guid, TEXT("group"), Name);
}

void UCometChatPanel::OnPersonalItemClicked(FString Uid, FString Name)
{
	ShowChatView(Uid, TEXT("user"), Name);
}

void UCometChatPanel::OnJoinGroupClicked(FString Guid)
{
	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk()) return;

	// Find the group name for opening chat later
	FString GroupName;
	for (const FCometChatGroup& G : BrowseableGroups)
	{
		if (G.Guid == Guid) { GroupName = G.Name; break; }
	}

	Sub->GetSdk()->join_group(TCHAR_TO_UTF8(*Guid), [this, Guid, GroupName](const std::string& err) {
		AsyncTask(ENamedThreads::GameThread, [this, Guid, GroupName, err]() {
			if (err.empty())
			{
				// Success — refresh lists and open the chat
				FetchMyGroups();
				FetchBrowseGroups();
				ShowSnackbar(TEXT("Joined group successfully!"), FLinearColor(0.1f, 0.7f, 0.3f, 0.9f));
				ShowChatView(Guid, TEXT("group"), GroupName);
			}
			else
			{
				// Failure — show error snackbar
				FString ErrMsg = FString::Printf(TEXT("Failed to join: %hs"), err.c_str());
				ShowSnackbar(ErrMsg, FLinearColor(0.8f, 0.1f, 0.1f, 0.9f));
			}
		});
	});
}

void UCometChatPanel::ShowSnackbar(const FString& Message, FLinearColor BgColor)
{
	if (!SnackbarBorder || !SnackbarText) return;

	SnackbarText->SetText(FText::FromString(Message));

	// Update background color
	FSlateBrush SB;
	SB.DrawAs = ESlateBrushDrawType::RoundedBox;
	SB.TintColor = FSlateColor(BgColor);
	SB.OutlineSettings.CornerRadii = FVector4(6, 6, 6, 6);
	SB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	SnackbarBorder->SetBrush(SB);

	SnackbarBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// Auto-hide after 3 seconds
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SnackbarTimerHandle);
		World->GetTimerManager().SetTimer(SnackbarTimerHandle, [this]()
		{
			if (SnackbarBorder) SnackbarBorder->SetVisibility(ESlateVisibility::Collapsed);
		}, 3.0f, false);
	}
}

void UCometChatPanel::HandleTextMessage(const FCometChatMessage& Message)
{
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: HandleTextMessage - SenderUid=%s, UserUid=%s, ReceiverType=%s, ConversationId=%s, ActiveChatId=%s"),
		*Message.SenderUid, *UserUid, *Message.ReceiverType, *Message.ConversationId, *ActiveChatId);

	if (CurrentView != EPanelView::Chat) return;
	bool bMatch = false;
	if (ActiveChatType == TEXT("group") && Message.ReceiverType == TEXT("group") && Message.ConversationId.Contains(ActiveChatId))
		bMatch = true;
	else if (ActiveChatType == TEXT("user") && Message.ReceiverType == TEXT("user") && (Message.SenderUid == ActiveChatId))
		bMatch = true;

	if (bMatch && Message.SenderUid != UserUid)
	{
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: HandleTextMessage - ADDING message from %s (not our own)"), *Message.SenderUid);
		// Hide typing indicator when message arrives
		FString SenderName = Message.SenderName.IsEmpty() ? Message.SenderUid : Message.SenderName;
		CurrentTypers.Remove(SenderName.Left(10));
		UpdateTypingDisplay();
		AddChatMessage(Message);
		if (ChatMessages) ChatMessages->ScrollToEnd();
	}
	else if (bMatch)
	{
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: HandleTextMessage - SKIPPING own message (SenderUid matches UserUid)"));
	}
}

void UCometChatPanel::HandleUserOnline(const FCometChatUser& User)
{
	OnlineUsers.Add(User.Uid, true);
}

void UCometChatPanel::HandleUserOffline(const FCometChatUser& User)
{
	OnlineUsers.Add(User.Uid, false);
}

void UCometChatPanel::HandleTypingStarted(const FCometChatTypingIndicator& Indicator)
{
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: TypingStarted - Sender=%s, ReceiverId=%s, ReceiverType=%s, ActiveChatId=%s, ActiveChatType=%s, CurrentView=%d"),
		*Indicator.Sender.Uid, *Indicator.ReceiverId, *Indicator.ReceiverType, *ActiveChatId, *ActiveChatType, (int)CurrentView);

	if (CurrentView != EPanelView::Chat) return;
	// Don't show our own typing
	if (Indicator.Sender.Uid == UserUid) return;

	// Check if this typing event is relevant to the active chat
	// The SDK may send receiver_id as group GUID, conversation ID, or user UID
	bool bRelevant = false;
	if (ActiveChatType == TEXT("group"))
	{
		// Match if ReceiverId equals or contains the group GUID
		bRelevant = (Indicator.ReceiverId == ActiveChatId) || Indicator.ReceiverId.Contains(ActiveChatId);
	}
	else if (ActiveChatType == TEXT("user"))
	{
		// For DMs: the sender UID should match who we're chatting with
		bRelevant = (Indicator.Sender.Uid == ActiveChatId);
	}

	if (!bRelevant)
	{
		UE_LOG(LogTemp, Log, TEXT("CometChatPanel: TypingStarted - NOT relevant, skipping"));
		return;
	}

	FString Name = Indicator.Sender.Name.IsEmpty() ? Indicator.Sender.Uid : Indicator.Sender.Name;
	if (Name.Len() > 10) Name = Name.Left(10);
	CurrentTypers.Add(Name);
	UpdateTypingDisplay();
	UE_LOG(LogTemp, Log, TEXT("CometChatPanel: TypingStarted - showing indicator for %s"), *Name);

	// Auto-clear typing indicator after 5 seconds if no end_typing received
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypingTimeoutHandle);
		World->GetTimerManager().SetTimer(TypingTimeoutHandle, [this]()
		{
			CurrentTypers.Empty();
			UpdateTypingDisplay();
		}, 5.0f, false);
	}
}

void UCometChatPanel::HandleTypingEnded(const FCometChatTypingIndicator& Indicator)
{
	if (CurrentView != EPanelView::Chat) return;
	if (Indicator.Sender.Uid == UserUid) return;

	// Same permissive matching as HandleTypingStarted
	bool bRelevant = false;
	if (ActiveChatType == TEXT("group"))
	{
		bRelevant = (Indicator.ReceiverId == ActiveChatId) || Indicator.ReceiverId.Contains(ActiveChatId);
	}
	else if (ActiveChatType == TEXT("user"))
	{
		bRelevant = (Indicator.Sender.Uid == ActiveChatId);
	}
	if (!bRelevant) return;

	FString Name = Indicator.Sender.Name.IsEmpty() ? Indicator.Sender.Uid : Indicator.Sender.Name;
	if (Name.Len() > 10) Name = Name.Left(10);
	CurrentTypers.Remove(Name);
	UpdateTypingDisplay();
}

void UCometChatPanel::UpdateTypingDisplay()
{
	if (!ChatTypingText) return;

	if (CurrentTypers.Num() == 0)
	{
		ChatTypingText->SetText(FText::GetEmpty());
		ChatTypingText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FString DisplayText;
	if (CurrentTypers.Num() == 1)
	{
		for (const FString& Name : CurrentTypers)
		{
			DisplayText = Name + TEXT(" is typing...");
			break;
		}
	}
	else if (CurrentTypers.Num() == 2)
	{
		TArray<FString> Names = CurrentTypers.Array();
		DisplayText = Names[0] + TEXT(" and ") + Names[1] + TEXT(" are typing...");
	}
	else
	{
		DisplayText = FString::Printf(TEXT("%d people are typing..."), CurrentTypers.Num());
	}

	ChatTypingText->SetText(FText::FromString(DisplayText));
	ChatTypingText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UCometChatPanel::OnChatInputTextChanged(const FText& Text)
{
	if (CurrentView != EPanelView::Chat) return;
	if (Text.IsEmpty())
	{
		SendTypingEnded();
		return;
	}
	SendTypingStarted();
}

void UCometChatPanel::SendTypingStarted()
{
	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk() || ActiveChatId.IsEmpty()) return;

	if (!bIsLocalTyping)
	{
		bIsLocalTyping = true;
		chatsdk::TypingIndicator indicator;
		indicator.receiver_id = TCHAR_TO_UTF8(*ActiveChatId);
		indicator.receiver_type = TCHAR_TO_UTF8(*ActiveChatType);
		Sub->GetSdk()->start_typing(indicator);
	}

	// Reset the end-typing timer (fires 2s after last keystroke)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypingSendTimerHandle);
		World->GetTimerManager().SetTimer(TypingSendTimerHandle, [this]() { SendTypingEnded(); }, 2.0f, false);
	}
}

void UCometChatPanel::SendTypingEnded()
{
	if (!bIsLocalTyping) return;
	bIsLocalTyping = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypingSendTimerHandle);
	}

	auto* Sub = GetSubsystem();
	if (!Sub || !Sub->GetSdk() || ActiveChatId.IsEmpty()) return;

	chatsdk::TypingIndicator indicator;
	indicator.receiver_id = TCHAR_TO_UTF8(*ActiveChatId);
	indicator.receiver_type = TCHAR_TO_UTF8(*ActiveChatType);
	Sub->GetSdk()->end_typing(indicator);
}

void UCometChatPanel::OnSearchTextChanged(const FText& Text)
{
	SearchQuery = Text.ToString().TrimStartAndEnd();
	FilterCurrentList();
}

void UCometChatPanel::OnSearchCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	SearchQuery = Text.ToString().TrimStartAndEnd();
	FilterCurrentList();
}

void UCometChatPanel::FilterCurrentList()
{
	FString Query = SearchQuery.ToLower();

	// If in chat view, search messages via API
	if (CurrentView == EPanelView::Chat)
	{
		if (Query.IsEmpty())
		{
			// Reload full history
			LoadChatHistory();
			return;
		}
		// Search via API using MessagesRequestBuilder with search_keyword
		auto* Sub = GetSubsystem();
		if (!Sub || !Sub->GetSdk()) return;
		chatsdk::MessagesRequestBuilder req;
		req.limit = 30;
		req.search_keyword = TCHAR_TO_UTF8(*SearchQuery);
		if (ActiveChatType == TEXT("group")) req.guid = TCHAR_TO_UTF8(*ActiveChatId);
		else req.uid = TCHAR_TO_UTF8(*ActiveChatId);
		Sub->GetSdk()->fetch_previous_messages(req, [this](const std::string& err, const std::vector<chatsdk::Message>& msgs) {
			AsyncTask(ENamedThreads::GameThread, [this, msgs]() {
				if (!ChatMessages) return;
				ChatMessages->ClearChildren();
				ChatMessageIds.Empty();
				ChatMessageCount = 0;
				for (auto& m : msgs)
					AddChatMessage(CometChatConversions::ToUnreal(m));
				if (ChatMessages) ChatMessages->ScrollToEnd();
			});
		});
		return;
	}

	// List view filtering
	if (ActiveTab == EActiveTab::MyGroups && MyGroupsList)
	{
		MyGroupsList->ClearChildren();
		for (auto& g : MyGroups)
		{
			if (!Query.IsEmpty() && !g.Name.ToLower().Contains(Query)) continue;
			UWidget* Item = CreateGroupListItem(g, false);
			MyGroupsList->AddChild(Item);
			if (UPanelSlot* PSlot = Item->Slot) { if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot)) SBSlot->SetHorizontalAlignment(HAlign_Fill); }
		}
	}
	else if (ActiveTab == EActiveTab::Personal && PersonalList)
	{
		PersonalList->ClearChildren();
		for (auto& c : PersonalConversations)
		{
			FString Name = c.ConversationWithUser.Name.IsEmpty() ? c.ConversationWithUser.Uid : c.ConversationWithUser.Name;
			if (!Query.IsEmpty() && !Name.ToLower().Contains(Query)) continue;
			UWidget* Item = CreatePersonalListItem(c);
			PersonalList->AddChild(Item);
			if (UPanelSlot* PSlot = Item->Slot) { if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot)) SBSlot->SetHorizontalAlignment(HAlign_Fill); }
		}
	}
	else if (ActiveTab == EActiveTab::BrowseGroups && BrowseGroupsList)
	{
		BrowseGroupsList->ClearChildren();
		for (auto& g : BrowseableGroups)
		{
			if (!Query.IsEmpty() && !g.Name.ToLower().Contains(Query)) continue;
			UWidget* Item = CreateGroupListItem(g, true);
			BrowseGroupsList->AddChild(Item);
			if (UPanelSlot* PSlot = Item->Slot) { if (UScrollBoxSlot* SBSlot = Cast<UScrollBoxSlot>(PSlot)) SBSlot->SetHorizontalAlignment(HAlign_Fill); }
		}
	}
}
