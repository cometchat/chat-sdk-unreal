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

void UCometChatGroupChatBox::NativeConstruct()
{
	Super::NativeConstruct();
	StartChatFlow();
}

void UCometChatGroupChatBox::NativeDestruct() { Super::NativeDestruct(); }

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

void UCometChatGroupChatBox::BuildChatUI()
{
	RootSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RootSizeBox"));
	RootSizeBox->SetWidthOverride(PanelWidth);
	RootSizeBox->SetHeightOverride(PanelHeight);
	WidgetTree->RootWidget = RootSizeBox;

	RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	RootSizeBox->AddChild(RootOverlay);

	// Panel with rounded corners
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

	// Message scroll
	MessageScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("MessageScrollBox"));
	MessageScrollBox->SetOrientation(Orient_Vertical);
	MessageScrollBox->SetScrollBarVisibility(ESlateVisibility::Collapsed);
	auto* SS = PanelVBox->AddChildToVerticalBox(MessageScrollBox);
	SS->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	// Composer row
	UHorizontalBox* ComposerRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ComposerRow"));
	auto* CRS = PanelVBox->AddChildToVerticalBox(ComposerRow);
	CRS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	CRS->SetPadding(FMargin(0, 6, 0, 0));

	// Composer input with rounded border
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
	// Match composer text size to username font size
	FEditableTextBoxStyle InputStyle = MessageInput->WidgetStyle;
	FSlateFontInfo InputFont = InputStyle.TextStyle.Font;
	InputFont.Size = static_cast<int32>(UsernameFontSize);
	InputStyle.TextStyle.Font = InputFont;
	MessageInput->WidgetStyle = InputStyle;
	MessageInput->OnTextCommitted.AddDynamic(this, &UCometChatGroupChatBox::OnInputCommitted);
	ComposerBorder->AddChild(MessageInput);

	// Send button
	{
		USpacer* Sp = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
		Sp->SetSize(FVector2D(6, 0));
		ComposerRow->AddChildToHorizontalBox(Sp);
	}
	SendButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("SendButton"));
	SendButton->SetBackgroundColor(SendButtonColor);
	SendButton->OnClicked.AddDynamic(this, &UCometChatGroupChatBox::OnSendClicked);
	// Make send button round
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
	SendSB->SetHeightOverride(SendButtonWidth); // Square for round button
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
// Avatar image download helper
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
				if (Tex && WeakImg.IsValid())
				{
					WeakImg->SetBrushFromTexture(Tex);
				}
			});
		});
	Req->ProcessRequest();
}

// ============================================================
// Message row: [Avatar] [Username + Message (fill)] [Timestamp (auto)]
// All items VAlign_Top, HAlign_Left
// ============================================================

UHorizontalBox* UCometChatGroupChatBox::CreateMessageRow(const FCometChatMessage& Message)
{
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	bool bIsYou = (Message.SenderUid == CurrentUserUid);
	FString DisplayName = Message.SenderName.IsEmpty() ? Message.SenderUid : Message.SenderName;

	// --- Circular avatar ---
	if (bShowAvatars)
	{
		UOverlay* AvOv = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());

		USizeBox* AvSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		AvSB->SetWidthOverride(AvatarSize);
		AvSB->SetHeightOverride(AvatarSize);

		// Circular background
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

		// Use Overlay to center the initial letter inside the circle
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

		// Avatar image (overlaid on top, will replace initial when loaded)
		UImage* AvImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		AvImg->SetVisibility(ESlateVisibility::Collapsed); // hidden until image loads
		{
			// Circular clip via RoundedBox brush on the image
			FSlateBrush ImgBrush;
			ImgBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
			float R = AvatarSize * 0.5f;
			ImgBrush.OutlineSettings.CornerRadii = FVector4(R, R, R, R);
			ImgBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
			AvImg->SetBrush(ImgBrush);
		}
		auto* ImgSlot = InitOverlay->AddChildToOverlay(AvImg);
		ImgSlot->SetHorizontalAlignment(HAlign_Fill);
		ImgSlot->SetVerticalAlignment(VAlign_Fill);

		// Start async avatar download if URL available
		if (!Message.SenderAvatar.IsEmpty())
		{
			TWeakObjectPtr<UImage> WeakAvImg(AvImg);
			LoadAvatarFromUrl(Message.SenderAvatar, AvImg);
			// Make visible once texture is set (handled in LoadAvatarFromUrl callback)
			// For now just make it visible — it'll show once texture arrives
			AvImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		AvOv->AddChildToOverlay(AvSB);

		// Online indicator dot (circular)
		if (bShowOnlineIndicator)
		{
			USizeBox* DotSB = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
			DotSB->SetWidthOverride(OnlineIndicatorSize);
			DotSB->SetHeightOverride(OnlineIndicatorSize);
			UBorder* Dot = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
			Dot->SetPadding(FMargin(0));
			{
				FSlateBrush DB;
				DB.DrawAs = ESlateBrushDrawType::RoundedBox;
				DB.TintColor = FSlateColor(OnlineIndicatorColor);
				float DR = OnlineIndicatorSize * 0.5f;
				DB.OutlineSettings.CornerRadii = FVector4(DR, DR, DR, DR);
				DB.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
				Dot->SetBrush(DB);
			}
			DotSB->AddChild(Dot);
			auto* DotSlot = AvOv->AddChildToOverlay(DotSB);
			DotSlot->SetHorizontalAlignment(HAlign_Left);
			DotSlot->SetVerticalAlignment(VAlign_Bottom);
		}

		auto* AvSlot = Row->AddChildToHorizontalBox(AvOv);
		AvSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		AvSlot->SetVerticalAlignment(VAlign_Top);
		AvSlot->SetHorizontalAlignment(HAlign_Left);
		AvSlot->SetPadding(FMargin(0, 0, 10, 0));
	}

	// --- Text content: [Avatar + Username] grouped, then [message (wraps)], then [timestamp] ---
	// Group avatar and username vertically centered together
	UHorizontalBox* AvatarNameGroup = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

	// Re-parent avatar into the group (move from Row to AvatarNameGroup)
	// Avatar was already added to Row above — we need to restructure
	// Actually, let's not add avatar to Row directly. Remove the avatar section above and rebuild here.

	// Username (colored)
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

	// ": message" (white, wraps)
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

	// Timestamp
	if (bShowTimestamp && Message.SentAt > 0)
	{
		USpacer* TSp = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
		TSp->SetSize(FVector2D(TimestampLeftPadding, 0));
		auto* TSpSlot = Row->AddChildToHorizontalBox(TSp);
		TSpSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

		UTextBlock* TimeTxt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		FDateTime Time = FDateTime::FromUnixTimestamp(Message.SentAt);
		TimeTxt->SetText(FText::FromString(Time.ToString(TEXT("%H:%M"))));
		FSlateFontInfo TF = TimeTxt->GetFont();
		TF.Size = static_cast<int32>(TimestampFontSize);
		TimeTxt->SetFont(TF);
		TimeTxt->SetColorAndOpacity(FSlateColor(TimestampColor));
		if (bEnableTextShadow) { TimeTxt->SetShadowOffset(TextShadowOffset); TimeTxt->SetShadowColorAndOpacity(TextShadowColor); }
		auto* TS = Row->AddChildToHorizontalBox(TimeTxt);
		TS->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		TS->SetVerticalAlignment(VAlign_Top);
	}

	return Row;
}

void UCometChatGroupChatBox::AddMessageToList(const FCometChatMessage& Message)
{
	if (!MessageScrollBox) return;
	UHorizontalBox* Row = CreateMessageRow(Message);
	MessageScrollBox->AddChild(Row);
	if (auto* SCS = Cast<UScrollBoxSlot>(Row->Slot))
		SCS->SetPadding(FMargin(0, MessageSpacing * 0.5f, 0, MessageSpacing * 0.5f));
}

void UCometChatGroupChatBox::ScrollToBottom() { if (MessageScrollBox) MessageScrollBox->ScrollToEnd(); }

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

void UCometChatGroupChatBox::StartChatFlow()
{
	CurrentUserUid = UserUid;
	auto* Sub = GetChatSubsystem();
	if (!Sub) { SetState(EChatState::Error, TEXT("CometChat subsystem not found")); return; }
	SetState(EChatState::Configuring);
	Sub->Configure(AppId, Region);
	Sub->OnMessageReceived.AddDynamic(this, &UCometChatGroupChatBox::OnMessageReceived);
	DoLogin();
}

void UCometChatGroupChatBox::DoLogin()
{
	SetState(EChatState::LoggingIn);
	auto* Sub = GetChatSubsystem();
	if (!Sub) { SetState(EChatState::Error, TEXT("Subsystem lost")); return; }
	auto* Sdk = Sub->GetSdk();
	if (!Sdk) { SetState(EChatState::Error, TEXT("SDK not initialized")); return; }
	Sdk->login(TCHAR_TO_UTF8(*UserUid), TCHAR_TO_UTF8(*AuthKey),
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
	SetState(EChatState::JoiningGroup);
	auto* Sub = GetChatSubsystem();
	if (!Sub) { SetState(EChatState::Error, TEXT("Subsystem lost")); return; }
	auto* Sdk = Sub->GetSdk();
	if (!Sdk) { SetState(EChatState::Error, TEXT("SDK not initialized")); return; }
	Sdk->join_group(TCHAR_TO_UTF8(*GroupGuid),
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
	if (!Sub) { SetState(EChatState::Error, TEXT("Subsystem lost")); return; }
	auto* Sdk = Sub->GetSdk();
	if (!Sdk) { SetState(EChatState::Error, TEXT("SDK not initialized")); return; }
	Sdk->get_group_messages(TCHAR_TO_UTF8(*GroupGuid), MessageLimit, 0,
		[this](const std::string& err, const chatsdk::PaginatedMessages& result) {
			AsyncTask(ENamedThreads::GameThread, [this, err, result]() {
				if (err.empty()) {
					for (int32 i = static_cast<int32>(result.messages.size()) - 1; i >= 0; --i)
						AddMessageToList(CometChatConversions::ToUnreal(result.messages[i]));
					SetState(EChatState::Ready);
					ScrollToBottom();
				} else {
					SetState(EChatState::Error, FString::Printf(TEXT("Load failed: %s"), UTF8_TO_TCHAR(err.c_str())));
				}
			});
		});
}

void UCometChatGroupChatBox::OnSendClicked()
{
	if (!MessageInput || CurrentState != EChatState::Ready) return;
	FString Text = MessageInput->GetText().ToString().TrimStartAndEnd();
	if (Text.IsEmpty()) return;
	MessageInput->SetText(FText::GetEmpty());
	auto* Sub = GetChatSubsystem();
	if (!Sub) return;
	auto* Sdk = Sub->GetSdk();
	if (!Sdk) return;
	Sdk->send_group_message(TCHAR_TO_UTF8(*GroupGuid), TCHAR_TO_UTF8(*Text),
		[this](const std::string& err, const chatsdk::Message& msg) {
			AsyncTask(ENamedThreads::GameThread, [this, err, msg]() {
				if (err.empty()) { AddMessageToList(CometChatConversions::ToUnreal(msg)); ScrollToBottom(); }
			});
		});
}

void UCometChatGroupChatBox::OnInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter) OnSendClicked();
}

void UCometChatGroupChatBox::HandleOptionsClicked() { OnOptionsClicked.Broadcast(); }

void UCometChatGroupChatBox::OnMessageReceived(const FCometChatMessage& Message)
{
	if (Message.ReceiverType == TEXT("group") && Message.ConversationId.Contains(GroupGuid) && Message.SenderUid != CurrentUserUid)
	{
		AddMessageToList(Message);
		ScrollToBottom();
	}
}
