#include "CometChatGroupChatBox.h"
#include "../CometChatConversions.h"
#include "Async/Async.h"
#include "Engine/GameInstance.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/BorderSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/SlateTypes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "ImageUtils.h"
#include "TimerManager.h"

// ============================================================
// Lifecycle
// ============================================================

void UCometChatGroupChatBox::NativeConstruct()
{
	Super::NativeConstruct();
	StartChatFlow();
}

void UCometChatGroupChatBox::NativeDestruct()
{
	UnsubscribeFromEvents();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypingTimeoutHandle);
		World->GetTimerManager().ClearTimer(LocalTypingEndHandle);
	}
	Super::NativeDestruct();
}

TSharedRef<SWidget> UCometChatGroupChatBox::RebuildWidget()
{
	if (!WidgetTree) WidgetTree = NewObject<UWidgetTree>(this, TEXT("ChatWidgetTree"));
	BuildChatUI();
	return Super::RebuildWidget();
}

UCometChatSubsystem* UCometChatGroupChatBox::GetChatSubsystem() const
{
	UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UCometChatSubsystem>() : nullptr;
}

// ============================================================
// Avatar download helper
// ============================================================

static void LoadAvatarFromUrl(const FString& Url, UImage* TargetImage)
{
	if (Url.IsEmpty() || !TargetImage) return;
	TWeakObjectPtr<UImage> WeakImg(TargetImage);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(Url);
	Req->SetVerb(TEXT("GET"));
	Req->OnProcessRequestComplete().BindLambda(
		[WeakImg](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
		{
			if (!bSuccess || !Response.IsValid() || !WeakImg.IsValid()) return;
			if (Response->GetResponseCode() < 200 || Response->GetResponseCode() >= 300) return;
			const TArray<uint8>& Data = Response->GetContent();
			if (Data.Num() == 0) return;
			AsyncTask(ENamedThreads::GameThread, [WeakImg, Data]()
			{
				if (!WeakImg.IsValid()) return;
				UTexture2D* Tex = FImageUtils::ImportBufferAsTexture2D(Data);
				if (Tex && WeakImg.IsValid()) WeakImg->SetBrushFromTexture(Tex);
			});
		});
	Req->ProcessRequest();
}

// ============================================================
// UI Building
// ============================================================

void UCometChatGroupChatBox::BuildChatUI()
{
	RootSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RootSizeBox"));
	RootSizeBox->SetWidthOverride(PanelWidth);
	RootSizeBox->SetHeightOverride(PanelHeight);
	WidgetTree->RootWidget = RootSizeBox;

	RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	RootSizeBox->AddChild(RootOverlay);

	PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PanelBorder"));
	PanelBorder->SetPadding(FMargin(PanelPadding));
	{
		FSlateBrush B;
		B.DrawAs = ESlateBrushDrawType::RoundedBox;
		B.TintColor = FSlateColor(PanelBackgroundColor);
		B.OutlineSettings.CornerRadii = FVector4(PanelCornerRadius, PanelCornerRadius, PanelCornerRadius, PanelCornerRadius);
		B.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		PanelBorder->SetBrush(B);
	}
	auto* PS = RootOverlay->AddChildToOverlay(PanelBorder);
	PS->SetHorizontalAlignment(HAlign_Fill);
	PS->SetVerticalAlignment(VAlign_Fill);

	PanelVBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PanelVBox"));
	PanelBorder->AddChild(PanelVBox);

	// Connection status banner (hidden by default)
	if (bEnableConnectionStatus)
	{
		ConnectionBanner = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ConnectionBanner"));
		ConnectionBanner->SetBrushColor(ConnectionBannerColor);
		ConnectionBanner->SetPadding(FMargin(8, 2));
		ConnectionBanner->SetVisibility(ESlateVisibility::Collapsed);
		ConnectionBannerText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ConnectionBannerText"));
		ConnectionBannerText->SetText(FText::FromString(TEXT("Reconnecting...")));
		ConnectionBannerText->SetColorAndOpacity(FSlateColor(ConnectionBannerTextColor));
		FSlateFontInfo CBF = ConnectionBannerText->GetFont();
		CBF.Size = 11;
		ConnectionBannerText->SetFont(CBF);
		ConnectionBanner->AddChild(ConnectionBannerText);
		auto* CBS = PanelVBox->AddChildToVerticalBox(ConnectionBanner);
		CBS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	}

	// Message scroll area
	MessageScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("MessageScrollBox"));
	MessageScrollBox->SetOrientation(Orient_Vertical);
	MessageScrollBox->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	auto* SS = PanelVBox->AddChildToVerticalBox(MessageScrollBox);
	SS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	// Typing indicator (below messages, above composer)
	if (bEnableTypingIndicator)
	{
		TypingIndicatorText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TypingIndicator"));
		TypingIndicatorText->SetText(FText::GetEmpty());
		TypingIndicatorText->SetColorAndOpacity(FSlateColor(TypingIndicatorColor));
		FSlateFontInfo TIF = TypingIndicatorText->GetFont();
		TIF.Size = static_cast<int32>(TypingIndicatorFontSize);
		TypingIndicatorText->SetFont(TIF);
		TypingIndicatorText->SetVisibility(ESlateVisibility::Collapsed);
		auto* TIS = PanelVBox->AddChildToVerticalBox(TypingIndicatorText);
		TIS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		TIS->SetPadding(FMargin(4, 2, 0, 2));
	}

	// Composer row
	UHorizontalBox* ComposerRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ComposerRow"));
	auto* CRS = PanelVBox->AddChildToVerticalBox(ComposerRow);
	CRS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	CRS->SetPadding(FMargin(0, 6, 0, 0));

	ComposerBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ComposerBorder"));
	ComposerBorder->SetPadding(FMargin(12, 4, 8, 4));
	{
		FSlateBrush CB;
		CB.DrawAs = ESlateBrushDrawType::RoundedBox;
		CB.TintColor = FSlateColor(ComposerBackgroundColor);
		CB.OutlineSettings.CornerRadii = FVector4(ComposerCornerRadius, ComposerCornerRadius, ComposerCornerRadius, ComposerCornerRadius);
		CB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		CB.OutlineSettings.Color = FSlateColor(ComposerBorderColor);
		CB.OutlineSettings.Width = 1.0f;
		ComposerBorder->SetBrush(CB);
	}
	auto* IBS = ComposerRow->AddChildToHorizontalBox(ComposerBorder);
	IBS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	IBS->SetVerticalAlignment(VAlign_Center);

	MessageInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("MessageInput"));
	MessageInput->SetHintText(FText::FromString(ComposerPlaceholderText));
	FEditableTextBoxStyle InputStyle = MessageInput->WidgetStyle;
	FSlateFontInfo InputFont = InputStyle.TextStyle.Font;
	InputFont.Size = static_cast<int32>(UsernameFontSize);
	InputStyle.TextStyle.Font = InputFont;
	MessageInput->WidgetStyle = InputStyle;
	MessageInput->OnTextCommitted.AddDynamic(this, &UCometChatGroupChatBox::OnInputCommitted);
	MessageInput->OnTextChanged.AddDynamic(this, &UCometChatGroupChatBox::OnInputChanged);
	ComposerBorder->AddChild(MessageInput);

	// Send button
	{
		USpacer* Sp = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
		Sp->SetSize(FVector2D(6, 0));
		ComposerRow->AddChildToHorizontalBox(Sp);
	}
	SendButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SendButton"));
	SendButton->OnClicked.AddDynamic(this, &UCometChatGroupChatBox::OnSendClicked);
	{
		FButtonStyle BtnStyle = SendButton->GetStyle();
		FSlateBrush RoundBrush;
		RoundBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		RoundBrush.TintColor = FSlateColor(SendButtonColor);
		RoundBrush.OutlineSettings.CornerRadii = FVector4(SendButtonCornerRadius, SendButtonCornerRadius, SendButtonCornerRadius, SendButtonCornerRadius);
		RoundBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		BtnStyle.Normal = RoundBrush;
		BtnStyle.Hovered = RoundBrush;
		BtnStyle.Pressed = RoundBrush;
		SendButton->SetStyle(BtnStyle);
	}
	USizeBox* SendSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	SendSB->SetWidthOverride(SendButtonWidth);
	SendSB->SetHeightOverride(SendButtonWidth);
	auto* SendSlot = ComposerRow->AddChildToHorizontalBox(SendSB);
	SendSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	SendSlot->SetVerticalAlignment(VAlign_Fill);
	SendSB->AddChild(SendButton);
	UOverlay* SendBtnOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
	SendButton->AddChild(SendBtnOverlay);
	UTextBlock* SendTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SendTxt->SetText(FText::FromString(SendButtonLabel));
	SendTxt->SetJustification(ETextJustify::Center);
	FSlateFontInfo SF = SendTxt->GetFont();
	SF.Size = static_cast<int32>(SendButtonFontSize);
	SendTxt->SetFont(SF);
	SendTxt->SetColorAndOpacity(FSlateColor(SendButtonTextColor));
	auto* SendTxtSlot = SendBtnOverlay->AddChildToOverlay(SendTxt);
	SendTxtSlot->SetHorizontalAlignment(HAlign_Center);
	SendTxtSlot->SetVerticalAlignment(VAlign_Center);

	// Options button
	if (bShowOptionsButton)
	{
		USpacer* Sp2 = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
		Sp2->SetSize(FVector2D(4, 0));
		ComposerRow->AddChildToHorizontalBox(Sp2);
		OptionsButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("OptionsButton"));
		OptionsButton->SetBackgroundColor(OptionsButtonColor);
		OptionsButton->OnClicked.AddDynamic(this, &UCometChatGroupChatBox::HandleOptionsClicked);
		USizeBox* OptSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		OptSB->SetWidthOverride(OptionsButtonSize);
		OptSB->SetHeightOverride(OptionsButtonSize);
		auto* OptSlot = ComposerRow->AddChildToHorizontalBox(OptSB);
		OptSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		OptSlot->SetVerticalAlignment(VAlign_Center);
		OptSB->AddChild(OptionsButton);
		UTextBlock* OptTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		OptTxt->SetText(FText::FromString(OptionsButtonLabel));
		OptTxt->SetJustification(ETextJustify::Center);
		FSlateFontInfo OF = OptTxt->GetFont();
		OF.Size = 16;
		OptTxt->SetFont(OF);
		OptTxt->SetColorAndOpacity(FSlateColor(OptionsButtonTextColor));
		OptionsButton->AddChild(OptTxt);
	}

	// Loading overlay
	OverlayBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("OverlayBorder"));
	OverlayBorder->SetBrushColor(OverlayBackgroundColor);
	OverlayBorder->SetPadding(FMargin(20));
	auto* OvS = RootOverlay->AddChildToOverlay(OverlayBorder);
	OvS->SetHorizontalAlignment(HAlign_Fill);
	OvS->SetVerticalAlignment(VAlign_Fill);
	OverlayText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OverlayText"));
	OverlayText->SetText(FText::FromString(TEXT("Connecting...")));
	OverlayText->SetJustification(ETextJustify::Center);
	FSlateFontInfo OvF = OverlayText->GetFont();
	OvF.Size = static_cast<int32>(OverlayFontSize);
	OverlayText->SetFont(OvF);
	OverlayText->SetColorAndOpacity(FSlateColor(OverlayTextColor));
	OverlayBorder->AddChild(OverlayText);
}


// ============================================================
// Message Row Creation
// ============================================================

UHorizontalBox* UCometChatGroupChatBox::CreateMessageRow(const FCometChatMessage& Message)
{
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	bool bIsYou = (Message.SenderUid == CurrentUserUid);
	FString DisplayName = Message.SenderName.IsEmpty() ? Message.SenderUid : Message.SenderName;

	// Avatar with online indicator
	if (bShowAvatars)
	{
		UOverlay* AvOv = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
		USizeBox* AvSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		AvSB->SetWidthOverride(AvatarSize);
		AvSB->SetHeightOverride(AvatarSize);

		UBorder* AvCircle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		AvCircle->SetPadding(FMargin(0));
		{
			FSlateBrush AB;
			AB.DrawAs = ESlateBrushDrawType::RoundedBox;
			AB.TintColor = FSlateColor(AvatarPlaceholderColor);
			float R = AvatarSize * 0.5f;
			AB.OutlineSettings.CornerRadii = FVector4(R, R, R, R);
			AB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
			AvCircle->SetBrush(AB);
		}
		AvSB->AddChild(AvCircle);

		UOverlay* InitOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
		AvCircle->AddChild(InitOverlay);

		UTextBlock* InitTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		InitTxt->SetText(FText::FromString(DisplayName.Left(1).ToUpper()));
		InitTxt->SetJustification(ETextJustify::Center);
		FSlateFontInfo IF = InitTxt->GetFont();
		IF.Size = static_cast<int32>(AvatarSize * 0.4f);
		InitTxt->SetFont(IF);
		InitTxt->SetColorAndOpacity(FSlateColor(AvatarInitialColor));
		auto* InitSlot = InitOverlay->AddChildToOverlay(InitTxt);
		InitSlot->SetHorizontalAlignment(HAlign_Center);
		InitSlot->SetVerticalAlignment(VAlign_Center);

		// Avatar image
		UImage* AvImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		AvImg->SetVisibility(ESlateVisibility::Collapsed);
		auto* ImgSlot = InitOverlay->AddChildToOverlay(AvImg);
		ImgSlot->SetHorizontalAlignment(HAlign_Fill);
		ImgSlot->SetVerticalAlignment(VAlign_Fill);
		if (!Message.SenderAvatar.IsEmpty())
		{
			LoadAvatarFromUrl(Message.SenderAvatar, AvImg);
			AvImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		AvOv->AddChildToOverlay(AvSB);

		// Online indicator dot - driven by actual presence data
		if (bShowOnlineIndicator && bEnableOnlinePresence)
		{
			bool bIsOnline = OnlineUsers.Contains(Message.SenderUid) && OnlineUsers[Message.SenderUid];
			USizeBox* DotSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
			DotSB->SetWidthOverride(OnlineIndicatorSize);
			DotSB->SetHeightOverride(OnlineIndicatorSize);
			UBorder* Dot = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
			Dot->SetPadding(FMargin(0));
			{
				FSlateBrush DB;
				DB.DrawAs = ESlateBrushDrawType::RoundedBox;
				DB.TintColor = FSlateColor(bIsOnline ? OnlineIndicatorColor : OfflineIndicatorColor);
				float DR = OnlineIndicatorSize * 0.5f;
				DB.OutlineSettings.CornerRadii = FVector4(DR, DR, DR, DR);
				DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
				Dot->SetBrush(DB);
			}
			DotSB->AddChild(Dot);
			auto* DotSlot = AvOv->AddChildToOverlay(DotSB);
			DotSlot->SetHorizontalAlignment(HAlign_Right);
			DotSlot->SetVerticalAlignment(VAlign_Bottom);

			// Store reference for live updates (last dot per user wins)
			OnlineDotWidgets.Add(Message.SenderUid, Dot);
		}

		auto* AvSlot = Row->AddChildToHorizontalBox(AvOv);
		AvSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		AvSlot->SetVerticalAlignment(VAlign_Top);
		AvSlot->SetPadding(FMargin(0, 0, 10, 0));
	}

	// Username
	UTextBlock* NameTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	FString YouLabel = bIsYou ? TEXT(" (you)") : TEXT("");
	NameTxt->SetText(FText::FromString(FString::Printf(TEXT("%s%s"), *DisplayName, *YouLabel)));
	FSlateFontInfo NF = NameTxt->GetFont();
	NF.Size = static_cast<int32>(UsernameFontSize);
	NameTxt->SetFont(NF);
	NameTxt->SetColorAndOpacity(FSlateColor(bIsYou ? SenderNameColor : ReceiverNameColor));
	if (bEnableTextShadow) { NameTxt->SetShadowOffset(TextShadowOffset); NameTxt->SetShadowColorAndOpacity(TextShadowColor); }
	auto* NameSlot = Row->AddChildToHorizontalBox(NameTxt);
	NameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	NameSlot->SetVerticalAlignment(VAlign_Top);

	// Message text
	UTextBlock* MsgTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MsgTxt->SetText(FText::FromString(FString::Printf(TEXT(": %s"), *Message.Text)));
	MsgTxt->SetAutoWrapText(true);
	FSlateFontInfo MF = MsgTxt->GetFont();
	MF.Size = static_cast<int32>(MessageFontSize);
	MsgTxt->SetFont(MF);
	MsgTxt->SetColorAndOpacity(FSlateColor(MessageTextColor));
	if (bEnableTextShadow) { MsgTxt->SetShadowOffset(TextShadowOffset); MsgTxt->SetShadowColorAndOpacity(TextShadowColor); }
	auto* MsgSlot = Row->AddChildToHorizontalBox(MsgTxt);
	MsgSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MsgSlot->SetVerticalAlignment(VAlign_Top);

	// Timestamp (local time)
	if (bShowTimestamp && Message.SentAt > 0)
	{
		USpacer* TSp = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
		TSp->SetSize(FVector2D(TimestampLeftPadding, 0));
		Row->AddChildToHorizontalBox(TSp);
		UTextBlock* TimeTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		// Convert UTC unix timestamp to local time
		FDateTime UtcTime = FDateTime::FromUnixTimestamp(Message.SentAt);
		FDateTime LocalTime = UtcTime + FTimespan(FDateTime::Now() - FDateTime::UtcNow());
		TimeTxt->SetText(FText::FromString(LocalTime.ToString(TEXT("%H:%M"))));
		FSlateFontInfo TF = TimeTxt->GetFont();
		TF.Size = static_cast<int32>(TimestampFontSize);
		TimeTxt->SetFont(TF);
		TimeTxt->SetColorAndOpacity(FSlateColor(TimestampColor));
		auto* TS = Row->AddChildToHorizontalBox(TimeTxt);
		TS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		TS->SetVerticalAlignment(VAlign_Top);
	}

	return Row;
}

// ============================================================
// Message Management (with memory limit)
// ============================================================

void UCometChatGroupChatBox::AddMessageToList(const FCometChatMessage& Message)
{
	if (!MessageScrollBox) return;
	UHorizontalBox* Row = CreateMessageRow(Message);
	MessageScrollBox->AddChild(Row);
	if (auto* SCS = Cast<UScrollBoxSlot>(Row->Slot))
		SCS->SetPadding(FMargin(0, MessageSpacing * 0.5f, 0, MessageSpacing * 0.5f));

	MessageIds.Add(Message.Id);
	CurrentMessageCount++;
	EnforceMessageLimit();
}

void UCometChatGroupChatBox::AddSystemMessage(const FString& Text)
{
	if (!MessageScrollBox) return;
	UTextBlock* SysTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SysTxt->SetText(FText::FromString(Text));
	SysTxt->SetJustification(ETextJustify::Center);
	SysTxt->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.65f, 0.7f)));
	FSlateFontInfo SF = SysTxt->GetFont();
	SF.Size = 11;
	SysTxt->SetFont(SF);
	MessageScrollBox->AddChild(SysTxt);
	if (auto* SCS = Cast<UScrollBoxSlot>(SysTxt->Slot))
		SCS->SetPadding(FMargin(0, 4, 0, 4));

	CurrentMessageCount++;
	EnforceMessageLimit();
}

void UCometChatGroupChatBox::EnforceMessageLimit()
{
	while (CurrentMessageCount > MaxMessagesInMemory && MessageScrollBox->GetChildrenCount() > 0)
	{
		UWidget* Oldest = MessageScrollBox->GetChildAt(0);
		MessageScrollBox->RemoveChild(Oldest);
		Oldest->RemoveFromParent();
		CurrentMessageCount--;
		if (MessageIds.Num() > 0) MessageIds.RemoveAt(0);
	}
}

void UCometChatGroupChatBox::ScrollToBottom() { if (MessageScrollBox) MessageScrollBox->ScrollToEnd(); }

// ============================================================
// Typing Indicator
// ============================================================

void UCometChatGroupChatBox::UpdateTypingIndicatorUI()
{
	if (!TypingIndicatorText) return;
	if (TypingUsers.Num() == 0)
	{
		TypingIndicatorText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FString TypingText;
	TArray<FString> Names;
	for (auto& Pair : TypingUsers) Names.Add(Pair.Value);

	if (Names.Num() == 1)
		TypingText = Names[0] + TEXT(" is typing...");
	else if (Names.Num() == 2)
		TypingText = Names[0] + TEXT(" and ") + Names[1] + TEXT(" are typing...");
	else
		TypingText = FString::Printf(TEXT("%d people are typing..."), Names.Num());

	TypingIndicatorText->SetText(FText::FromString(TypingText));
	TypingIndicatorText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UCometChatGroupChatBox::ClearTypingTimeout(const FString& Uid)
{
	TypingUsers.Remove(Uid);
	UpdateTypingIndicatorUI();
}

void UCometChatGroupChatBox::OnTypingTimeout()
{
	// Clear all typing users that haven't refreshed
	TypingUsers.Empty();
	UpdateTypingIndicatorUI();
}

// ============================================================
// State Management
// ============================================================

void UCometChatGroupChatBox::SetState(EChatState NewState, const FString& StatusMessage)
{
	CurrentState = NewState;
	if (NewState == EChatState::Ready) {
		if (OverlayBorder) OverlayBorder->SetVisibility(ESlateVisibility::Collapsed);
	} else {
		FString Label;
		switch (NewState) {
		case EChatState::Configuring:    Label = TEXT("Configuring..."); break;
		case EChatState::LoggingIn:      Label = TEXT("Logging in..."); break;
		case EChatState::JoiningGroup:   Label = TEXT("Joining group..."); break;
		case EChatState::LoadingHistory: Label = TEXT("Loading messages..."); break;
		case EChatState::Error:          Label = StatusMessage.IsEmpty() ? TEXT("Error") : StatusMessage; break;
		default: Label = TEXT("..."); break;
		}
		if (OverlayBorder) OverlayBorder->SetVisibility(ESlateVisibility::Visible);
		if (OverlayText) OverlayText->SetText(FText::FromString(Label));
	}
}

// ============================================================
// Chat Flow
// ============================================================

void UCometChatGroupChatBox::StartChatFlow()
{
	CurrentUserUid = UserUid;
	auto* Sub = GetChatSubsystem();
	if (!Sub) { SetState(EChatState::Error, TEXT("CometChat subsystem not found")); return; }
	SetState(EChatState::Configuring);
	Sub->Configure(AppId, Region);
	SubscribeToEvents();
	DoLogin();
}

bool UCometChatGroupChatBox::IsGroupMode() const
{
	return !GroupGuid.IsEmpty();
}

void UCometChatGroupChatBox::SubscribeToEvents()
{
	auto* Sub = GetChatSubsystem();
	if (!Sub) return;

	// Message listeners: use either moderated or standard, never both
	if (bUseModeratedMessageListener)
	{
		Sub->OnMessageModerated.AddDynamic(this, &UCometChatGroupChatBox::HandleMessageModerated);
	}
	else
	{
		Sub->OnTextMessageReceived.AddDynamic(this, &UCometChatGroupChatBox::HandleTextMessageReceived);
		Sub->OnMediaMessageReceived.AddDynamic(this, &UCometChatGroupChatBox::HandleMediaMessageReceived);
	}

	// Do NOT subscribe to legacy OnMessageReceived to avoid double messages

	if (bEnableMessageEdited)
		Sub->OnMessageEdited.AddDynamic(this, &UCometChatGroupChatBox::HandleMessageEdited);
	if (bEnableMessageDeleted)
		Sub->OnMessageDeleted.AddDynamic(this, &UCometChatGroupChatBox::HandleMessageDeleted);
	if (bEnableTypingIndicator)
	{
		Sub->OnTypingStarted.AddDynamic(this, &UCometChatGroupChatBox::HandleTypingStarted);
		Sub->OnTypingEnded.AddDynamic(this, &UCometChatGroupChatBox::HandleTypingEnded);
	}
	if (bEnableOnlinePresence)
	{
		Sub->OnUserOnline.AddDynamic(this, &UCometChatGroupChatBox::HandleUserOnline);
		Sub->OnUserOffline.AddDynamic(this, &UCometChatGroupChatBox::HandleUserOffline);
	}
	if (bEnableGroupMemberEvents)
	{
		Sub->OnGroupMemberJoined.AddDynamic(this, &UCometChatGroupChatBox::HandleGroupMemberJoined);
		Sub->OnGroupMemberLeft.AddDynamic(this, &UCometChatGroupChatBox::HandleGroupMemberLeft);
	}
	if (bEnableConnectionStatus)
	{
		Sub->OnConnected.AddDynamic(this, &UCometChatGroupChatBox::HandleConnected);
		Sub->OnDisconnected.AddDynamic(this, &UCometChatGroupChatBox::HandleDisconnected);
	}
}

void UCometChatGroupChatBox::UnsubscribeFromEvents()
{
	auto* Sub = GetChatSubsystem();
	if (!Sub) return;
	Sub->OnTextMessageReceived.RemoveDynamic(this, &UCometChatGroupChatBox::HandleTextMessageReceived);
	Sub->OnMediaMessageReceived.RemoveDynamic(this, &UCometChatGroupChatBox::HandleMediaMessageReceived);
	Sub->OnMessageModerated.RemoveDynamic(this, &UCometChatGroupChatBox::HandleMessageModerated);
	Sub->OnMessageEdited.RemoveDynamic(this, &UCometChatGroupChatBox::HandleMessageEdited);
	Sub->OnMessageDeleted.RemoveDynamic(this, &UCometChatGroupChatBox::HandleMessageDeleted);
	Sub->OnTypingStarted.RemoveDynamic(this, &UCometChatGroupChatBox::HandleTypingStarted);
	Sub->OnTypingEnded.RemoveDynamic(this, &UCometChatGroupChatBox::HandleTypingEnded);
	Sub->OnUserOnline.RemoveDynamic(this, &UCometChatGroupChatBox::HandleUserOnline);
	Sub->OnUserOffline.RemoveDynamic(this, &UCometChatGroupChatBox::HandleUserOffline);
	Sub->OnGroupMemberJoined.RemoveDynamic(this, &UCometChatGroupChatBox::HandleGroupMemberJoined);
	Sub->OnGroupMemberLeft.RemoveDynamic(this, &UCometChatGroupChatBox::HandleGroupMemberLeft);
	Sub->OnConnected.RemoveDynamic(this, &UCometChatGroupChatBox::HandleConnected);
	Sub->OnDisconnected.RemoveDynamic(this, &UCometChatGroupChatBox::HandleDisconnected);
}

void UCometChatGroupChatBox::DoLogin()
{
	SetState(EChatState::LoggingIn);
	auto* Sub = GetChatSubsystem();
	if (!Sub || !Sub->GetSdk()) { SetState(EChatState::Error, TEXT("SDK not initialized")); return; }
	Sub->GetSdk()->login(TCHAR_TO_UTF8(*UserUid), TCHAR_TO_UTF8(*AuthKey),
		[this](const std::string& err) {
			AsyncTask(ENamedThreads::GameThread, [this, err]() {
				if (err.empty()) {
					if (auto* S = GetChatSubsystem()) S->SetLoggedIn(true);
					DoJoinGroup();
				} else {
					SetState(EChatState::Error, FString::Printf(TEXT("Login failed: %s"), UTF8_TO_TCHAR(err.c_str())));
				}
			});
		});
}

void UCometChatGroupChatBox::DoJoinGroup()
{
	if (!IsGroupMode())
	{
		// 1:1 mode — skip join, go straight to history
		DoLoadHistory();
		return;
	}
	SetState(EChatState::JoiningGroup);
	auto* Sub = GetChatSubsystem();
	if (!Sub || !Sub->GetSdk()) { SetState(EChatState::Error, TEXT("SDK lost")); return; }
	Sub->GetSdk()->join_group(TCHAR_TO_UTF8(*GroupGuid),
		[this](const std::string& err) {
			AsyncTask(ENamedThreads::GameThread, [this, err]() {
				if (err.empty() || err.find("already") != std::string::npos || err.find("ERR_ALREADY") != std::string::npos || err.find("417") != std::string::npos)
					DoLoadHistory();
				else
					SetState(EChatState::Error, FString::Printf(TEXT("Join failed: %s"), UTF8_TO_TCHAR(err.c_str())));
			});
		});
}

void UCometChatGroupChatBox::DoLoadHistory()
{
	SetState(EChatState::LoadingHistory);
	auto* Sub = GetChatSubsystem();
	if (!Sub || !Sub->GetSdk()) { SetState(EChatState::Error, TEXT("SDK lost")); return; }

	if (IsGroupMode())
	{
		Sub->GetSdk()->get_group_messages(TCHAR_TO_UTF8(*GroupGuid), MessageLimit, 0,
			[this](const std::string& err, const chatsdk::PaginatedMessages& result) {
				AsyncTask(ENamedThreads::GameThread, [this, err, result]() {
					if (err.empty()) {
						for (size_t i = 0; i < result.messages.size(); ++i)
							AddMessageToList(CometChatConversions::ToUnreal(result.messages[i]));
						SetState(EChatState::Ready);
						ScrollToBottom();
						if (bEnableOnlinePresence) FetchOnlineMembers();
					} else {
						SetState(EChatState::Error, FString::Printf(TEXT("Load failed: %s"), UTF8_TO_TCHAR(err.c_str())));
					}
				});
			});
	}
	else
	{
		// 1:1 mode — fetch messages for the receiver
		Sub->GetSdk()->get_messages(TCHAR_TO_UTF8(*ReceiverUid), MessageLimit,
			[this](const std::string& err, const std::vector<chatsdk::Message>& msgs) {
				AsyncTask(ENamedThreads::GameThread, [this, err, msgs]() {
					if (err.empty()) {
						for (size_t i = 0; i < msgs.size(); ++i)
							AddMessageToList(CometChatConversions::ToUnreal(msgs[i]));
						SetState(EChatState::Ready);
						ScrollToBottom();
					} else {
						SetState(EChatState::Error, FString::Printf(TEXT("Load failed: %s"), UTF8_TO_TCHAR(err.c_str())));
					}
				});
			});
	}
}

// ============================================================
// Input Handlers
// ============================================================

void UCometChatGroupChatBox::OnSendClicked()
{
	if (!MessageInput || CurrentState != EChatState::Ready) return;
	FString Text = MessageInput->GetText().ToString().TrimStartAndEnd();
	if (Text.IsEmpty()) return;
	MessageInput->SetText(FText::GetEmpty());

	// End typing when sending
	if (bIsLocalUserTyping && bEnableTypingIndicator)
	{
		bIsLocalUserTyping = false;
		auto* Sub = GetChatSubsystem();
		if (Sub) Sub->EndTyping(FCometChatTypingIndicator{GroupGuid, TEXT("group"), TEXT(""), FCometChatUser{}});
	}

	auto* Sub = GetChatSubsystem();
	if (!Sub || !Sub->GetSdk()) return;

	if (IsGroupMode())
	{
		Sub->GetSdk()->send_group_message(TCHAR_TO_UTF8(*GroupGuid), TCHAR_TO_UTF8(*Text),
			[this](const std::string& err, const chatsdk::Message& msg) {
				AsyncTask(ENamedThreads::GameThread, [this, err, msg]() {
					if (err.empty()) { AddMessageToList(CometChatConversions::ToUnreal(msg)); ScrollToBottom(); }
				});
			});
	}
	else
	{
		Sub->GetSdk()->send_message(TCHAR_TO_UTF8(*ReceiverUid), TCHAR_TO_UTF8(*Text),
			[this](const std::string& err, const chatsdk::Message& msg) {
				AsyncTask(ENamedThreads::GameThread, [this, err, msg]() {
					if (err.empty()) { AddMessageToList(CometChatConversions::ToUnreal(msg)); ScrollToBottom(); }
				});
			});
	}
}

void UCometChatGroupChatBox::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter) OnSendClicked();
}

void UCometChatGroupChatBox::OnInputChanged(const FText& Text)
{
	if (!bEnableTypingIndicator) return;
	auto* Sub = GetChatSubsystem();
	if (!Sub) return;

	if (!Text.IsEmpty() && !bIsLocalUserTyping)
	{
		bIsLocalUserTyping = true;
		Sub->StartTyping(FCometChatTypingIndicator{GroupGuid, TEXT("group"), TEXT(""), FCometChatUser{}});
	}

	// Reset the end-typing timer (fires after 3 seconds of no input)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LocalTypingEndHandle);
		if (!Text.IsEmpty())
		{
			World->GetTimerManager().SetTimer(LocalTypingEndHandle, [this, Sub]()
			{
				bIsLocalUserTyping = false;
				Sub->EndTyping(FCometChatTypingIndicator{GroupGuid, TEXT("group"), TEXT(""), FCometChatUser{}});
			}, 3.0f, false);
		}
		else
		{
			bIsLocalUserTyping = false;
			Sub->EndTyping(FCometChatTypingIndicator{GroupGuid, TEXT("group"), TEXT(""), FCometChatUser{}});
		}
	}
}

void UCometChatGroupChatBox::HandleOptionsClicked() { OnOptionsClicked.Broadcast(); }

// ============================================================
// Event Handlers
// ============================================================

void UCometChatGroupChatBox::OnMessageReceived(const FCometChatMessage& Message)
{
	// Legacy handler - only used if new typed handlers don't catch it
}

void UCometChatGroupChatBox::HandleTextMessageReceived(const FCometChatMessage& Message)
{
	if (Message.SenderUid == CurrentUserUid) return;

	if (IsGroupMode())
	{
		// Group mode: check if message is for our group
		if (Message.ReceiverType == TEXT("group") && Message.ConversationId.Contains(GroupGuid))
		{
			AddMessageToList(Message);
			ScrollToBottom();
		}
	}
	else
	{
		// 1:1 mode: check if message is from our receiver
		if (Message.ReceiverType == TEXT("user") && Message.SenderUid == ReceiverUid)
		{
			AddMessageToList(Message);
			ScrollToBottom();
		}
	}

	// Clear typing for this user
	if (TypingUsers.Contains(Message.SenderUid))
	{
		TypingUsers.Remove(Message.SenderUid);
		UpdateTypingIndicatorUI();
	}
}

void UCometChatGroupChatBox::HandleMediaMessageReceived(const FCometChatMessage& Message)
{
	if (Message.ReceiverType == TEXT("group") && Message.ConversationId.Contains(GroupGuid) && Message.SenderUid != CurrentUserUid)
	{
		// Show media placeholder
		FCometChatMessage MediaMsg = Message;
		MediaMsg.Text = TEXT("[") + Message.Type + TEXT("]");
		AddMessageToList(MediaMsg);
		ScrollToBottom();
	}
}

void UCometChatGroupChatBox::HandleMessageEdited(const FCometChatMessage& Message)
{
	if (!bEnableMessageEdited) return;
	// Find and update the message in the scroll box by ID
	int32 Idx = MessageIds.Find(Message.Id);
	if (Idx != INDEX_NONE && MessageScrollBox && Idx < MessageScrollBox->GetChildrenCount())
	{
		// Replace the row with updated content
		MessageScrollBox->RemoveChildAt(Idx);
		UHorizontalBox* NewRow = CreateMessageRow(Message);
		MessageScrollBox->InsertChildAt(Idx, NewRow);
	}
}

void UCometChatGroupChatBox::HandleMessageDeleted(const FCometChatMessage& Message)
{
	if (!bEnableMessageDeleted) return;
	int32 Idx = MessageIds.Find(Message.Id);
	if (Idx != INDEX_NONE && MessageScrollBox && Idx < MessageScrollBox->GetChildrenCount())
	{
		// Replace with "[deleted]" text
		FCometChatMessage DeletedMsg = Message;
		DeletedMsg.Text = TEXT("[This message was deleted]");
		MessageScrollBox->RemoveChildAt(Idx);
		UHorizontalBox* NewRow = CreateMessageRow(DeletedMsg);
		MessageScrollBox->InsertChildAt(Idx, NewRow);
	}
}

void UCometChatGroupChatBox::HandleTypingStarted(const FCometChatTypingIndicator& Indicator)
{
	if (!bEnableTypingIndicator) return;
	if (Indicator.ReceiverId != GroupGuid) return;
	if (Indicator.Sender.Uid == CurrentUserUid) return;

	FString Name = Indicator.Sender.Name.IsEmpty() ? Indicator.Sender.Uid : Indicator.Sender.Name;
	TypingUsers.Add(Indicator.Sender.Uid, Name);
	UpdateTypingIndicatorUI();

	// Set timeout to auto-clear
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypingTimeoutHandle);
		World->GetTimerManager().SetTimer(TypingTimeoutHandle, this, &UCometChatGroupChatBox::OnTypingTimeout, TypingTimeoutSeconds, false);
	}
}

void UCometChatGroupChatBox::HandleTypingEnded(const FCometChatTypingIndicator& Indicator)
{
	if (!bEnableTypingIndicator) return;
	if (Indicator.Sender.Uid == CurrentUserUid) return;
	TypingUsers.Remove(Indicator.Sender.Uid);
	UpdateTypingIndicatorUI();
}

void UCometChatGroupChatBox::HandleUserOnline(const FCometChatUser& User)
{
	if (!bEnableOnlinePresence) return;
	OnlineUsers.Add(User.Uid, true);
	// Update dot widget if we have one for this user
	if (TWeakObjectPtr<UBorder>* DotPtr = OnlineDotWidgets.Find(User.Uid))
	{
		if (DotPtr->IsValid())
		{
			FSlateBrush DB;
			DB.DrawAs = ESlateBrushDrawType::RoundedBox;
			DB.TintColor = FSlateColor(OnlineIndicatorColor);
			float DR = OnlineIndicatorSize * 0.5f;
			DB.OutlineSettings.CornerRadii = FVector4(DR, DR, DR, DR);
			DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
			DotPtr->Get()->SetBrush(DB);
		}
	}
}

void UCometChatGroupChatBox::HandleUserOffline(const FCometChatUser& User)
{
	if (!bEnableOnlinePresence) return;
	OnlineUsers.Add(User.Uid, false);
	// Update dot widget if we have one for this user
	if (TWeakObjectPtr<UBorder>* DotPtr = OnlineDotWidgets.Find(User.Uid))
	{
		if (DotPtr->IsValid())
		{
			FSlateBrush DB;
			DB.DrawAs = ESlateBrushDrawType::RoundedBox;
			DB.TintColor = FSlateColor(OfflineIndicatorColor);
			float DR = OnlineIndicatorSize * 0.5f;
			DB.OutlineSettings.CornerRadii = FVector4(DR, DR, DR, DR);
			DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
			DotPtr->Get()->SetBrush(DB);
		}
	}
}

void UCometChatGroupChatBox::HandleGroupMemberJoined(const FCometChatAction& Action, const FCometChatUser& User, const FCometChatGroup& Group)
{
	if (!bEnableGroupMemberEvents) return;
	if (Group.Guid != GroupGuid) return;
	FString Name = User.Name.IsEmpty() ? User.Uid : User.Name;
	AddSystemMessage(Name + TEXT(" joined the group"));
	ScrollToBottom();
	OnlineUsers.Add(User.Uid, true);
}

void UCometChatGroupChatBox::HandleGroupMemberLeft(const FCometChatAction& Action, const FCometChatUser& User, const FCometChatGroup& Group)
{
	if (!bEnableGroupMemberEvents) return;
	if (Group.Guid != GroupGuid) return;
	FString Name = User.Name.IsEmpty() ? User.Uid : User.Name;
	AddSystemMessage(Name + TEXT(" left the group"));
	ScrollToBottom();
	OnlineUsers.Remove(User.Uid);
}

void UCometChatGroupChatBox::HandleConnected()
{
	if (!bEnableConnectionStatus || !ConnectionBanner) return;
	ConnectionBanner->SetVisibility(ESlateVisibility::Collapsed);
}

void UCometChatGroupChatBox::HandleDisconnected()
{
	if (!bEnableConnectionStatus || !ConnectionBanner) return;
	if (ConnectionBannerText) ConnectionBannerText->SetText(FText::FromString(TEXT("Reconnecting...")));
	ConnectionBanner->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UCometChatGroupChatBox::HandleMessageModerated(const FCometChatMessage& Message)
{
	// When using moderated listener, this replaces the standard message received handler
	if (!bUseModeratedMessageListener) return;
	if (Message.ReceiverType == TEXT("group") && Message.ConversationId.Contains(GroupGuid) && Message.SenderUid != CurrentUserUid)
	{
		AddMessageToList(Message);
		ScrollToBottom();
		if (TypingUsers.Contains(Message.SenderUid))
		{
			TypingUsers.Remove(Message.SenderUid);
			UpdateTypingIndicatorUI();
		}
	}
}

void UCometChatGroupChatBox::FetchOnlineMembers()
{
	auto* Sub = GetChatSubsystem();
	if (!Sub || !Sub->GetSdk()) return;

	// Fetch group members with status=available to get who's online
	chatsdk::GroupMembersRequestBuilder request;
	request.guid = TCHAR_TO_UTF8(*GroupGuid);
	request.limit = 100;
	request.status = "available";

	Sub->GetSdk()->fetch_group_members(request,
		[this](const std::string& err, const std::vector<chatsdk::GroupMember>& members) {
			AsyncTask(ENamedThreads::GameThread, [this, err, members]() {
				if (err.empty()) {
					// Mark all fetched members as online
					for (const auto& m : members) {
						FString Uid = UTF8_TO_TCHAR(m.user.uid.c_str());
						OnlineUsers.Add(Uid, true);
						// Update existing dot widgets
						if (TWeakObjectPtr<UBorder>* DotPtr = OnlineDotWidgets.Find(Uid))
						{
							if (DotPtr->IsValid())
							{
								FSlateBrush DB;
								DB.DrawAs = ESlateBrushDrawType::RoundedBox;
								DB.TintColor = FSlateColor(OnlineIndicatorColor);
								float DR = OnlineIndicatorSize * 0.5f;
								DB.OutlineSettings.CornerRadii = FVector4(DR, DR, DR, DR);
								DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
								DotPtr->Get()->SetBrush(DB);
							}
						}
					}
				}
			});
		});
}

void UCometChatGroupChatBox::UpdateOnlineIndicator(const FString& Uid, bool bOnline)
{
	OnlineUsers.Add(Uid, bOnline);
	// Update dot widget
	if (TWeakObjectPtr<UBorder>* DotPtr = OnlineDotWidgets.Find(Uid))
	{
		if (DotPtr->IsValid())
		{
			FSlateBrush DB;
			DB.DrawAs = ESlateBrushDrawType::RoundedBox;
			DB.TintColor = FSlateColor(bOnline ? OnlineIndicatorColor : OfflineIndicatorColor);
			float DR = OnlineIndicatorSize * 0.5f;
			DB.OutlineSettings.CornerRadii = FVector4(DR, DR, DR, DR);
			DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
			DotPtr->Get()->SetBrush(DB);
		}
	}
}
