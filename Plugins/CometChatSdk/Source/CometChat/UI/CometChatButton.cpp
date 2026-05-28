#include "CometChatButton.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

void UCometChatButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCometChatButton::NativeDestruct()
{
	if (ChatPanel)
	{
		ChatPanel->RemoveFromParent();
		ChatPanel = nullptr;
	}
	Super::NativeDestruct();
}

TSharedRef<SWidget> UCometChatButton::RebuildWidget()
{
	RootSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BtnRootSize"));
	WidgetTree->RootWidget = RootSizeBox;

	BuildUI();

	return Super::RebuildWidget();
}

void UCometChatButton::BuildUI()
{
	if (!RootSizeBox) return;

	RootSizeBox->SetWidthOverride(ButtonSize);
	RootSizeBox->SetHeightOverride(ButtonSize);

	// Create the button
	ChatButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ChatBtn"));
	RootSizeBox->AddChild(ChatButton);

	// Style the button as a circle-ish colored button
	FButtonStyle BtnStyle;
	FSlateBrush NormalBrush;
	NormalBrush.TintColor = FSlateColor(ButtonColor);
	NormalBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	NormalBrush.OutlineSettings.CornerRadii = FVector4(ButtonSize * 0.5f, ButtonSize * 0.5f, ButtonSize * 0.5f, ButtonSize * 0.5f);
	NormalBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;

	FSlateBrush HoverBrush = NormalBrush;
	HoverBrush.TintColor = FSlateColor(ButtonHoverColor);

	FSlateBrush PressedBrush = NormalBrush;
	PressedBrush.TintColor = FSlateColor(ButtonColor * 0.8f);

	BtnStyle.SetNormal(NormalBrush);
	BtnStyle.SetHovered(HoverBrush);
	BtnStyle.SetPressed(PressedBrush);

	ChatButton->SetStyle(BtnStyle);
	ChatButton->OnClicked.AddDynamic(this, &UCometChatButton::OnChatButtonClicked);

	// Chat icon text (using a simple chat bubble unicode or text)
	ButtonIcon = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BtnIcon"));
	ChatButton->AddChild(ButtonIcon);

	ButtonIcon->SetText(FText::FromString(TEXT("\U0001F4AC"))); // 💬 speech bubble emoji
	ButtonIcon->SetColorAndOpacity(FSlateColor(IconColor));

	FSlateFontInfo IconFont = ButtonIcon->GetFont();
	IconFont.Size = static_cast<int32>(IconFontSize);
	ButtonIcon->SetFont(IconFont);
	ButtonIcon->SetJustification(ETextJustify::Center);
}

void UCometChatButton::OnChatButtonClicked()
{
	ToggleChatPanel();
}

void UCometChatButton::ToggleChatPanel()
{
	if (bPanelOpen)
	{
		ClosePanel();
	}
	else
	{
		OpenPanel();
	}
}

bool UCometChatButton::IsPanelOpen() const
{
	return bPanelOpen;
}

void UCometChatButton::OpenPanel()
{
	if (bPanelOpen) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	// Create the panel widget if it doesn't exist
	if (!ChatPanel)
	{
		ChatPanel = CreateWidget<UCometChatPanel>(PC, UCometChatPanel::StaticClass());
		if (!ChatPanel) return;

		// Pass SDK config to the panel
		ChatPanel->AppId = AppId;
		ChatPanel->Region = Region;
		ChatPanel->AuthKey = AuthKey;
		ChatPanel->UserUid = UserUid;
	}

	ChatPanel->AddToViewport(10); // Higher ZOrder so it appears above other HUD elements
	bPanelOpen = true;

	// Show mouse cursor for interaction
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}

void UCometChatButton::ClosePanel()
{
	if (!bPanelOpen) return;

	if (ChatPanel)
	{
		ChatPanel->RemoveFromParent();
	}

	bPanelOpen = false;

	// Restore game-only input
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}
