#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "CometChatPanel.h"
#include "CometChatButton.generated.h"

/**
 * UCometChatButton — Floating chat button that opens the CometChat panel.
 *
 * Add this widget to your HUD or viewport. When tapped/clicked, it creates
 * and shows the full CometChatPanel. Tapping again (or the panel's back)
 * hides the panel.
 */
UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="CometChat Button"))
class COMETCHAT_API UCometChatButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// ============================================================
	// SDK Configuration (passed to the panel when opened)
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
	// Button Style
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Button Style")
	float ButtonSize = 56.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Button Style")
	FLinearColor ButtonColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Button Style")
	FLinearColor ButtonHoverColor = FLinearColor(0.0f, 0.6f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Button Style")
	FLinearColor IconColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CometChat|Button Style")
	float IconFontSize = 20.0f;

	/** Toggle the panel open/closed from Blueprint */
	UFUNCTION(BlueprintCallable, Category="CometChat")
	void ToggleChatPanel();

	/** Check if panel is currently visible */
	UFUNCTION(BlueprintPure, Category="CometChat")
	bool IsPanelOpen() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY() TObjectPtr<USizeBox> RootSizeBox;
	UPROPERTY() TObjectPtr<UButton> ChatButton;
	UPROPERTY() TObjectPtr<UTextBlock> ButtonIcon;
	UPROPERTY() TObjectPtr<UCometChatPanel> ChatPanel;

	bool bPanelOpen = false;

	void BuildUI();

	UFUNCTION()
	void OnChatButtonClicked();

	void OpenPanel();
	void ClosePanel();
};
