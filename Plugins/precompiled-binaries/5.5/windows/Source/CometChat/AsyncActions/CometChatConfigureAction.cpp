#include "CometChatConfigureAction.h"
#include "../CometChatAuthSave.h"
#include "Kismet/GameplayStatics.h"

UCometChatConfigureAction* UCometChatConfigureAction::CometChatConfigureAsync(UObject* WorldContextObject, const FString& AppId, const FString& Region)
{
	FCometChatAppSettings Settings;
	Settings.Region = Region;
	return CometChatConfigureWithSettingsAsync(WorldContextObject, AppId, Settings);
}

UCometChatConfigureAction* UCometChatConfigureAction::CometChatConfigureWithSettingsAsync(UObject* WorldContextObject, const FString& AppId, const FCometChatAppSettings& Settings)
{
	UCometChatConfigureAction* Node = NewObject<UCometChatConfigureAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->AppId = AppId;
	Node->Settings = Settings;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatConfigureAction::Activate()
{
	UCometChatSubsystem* Sub = GetSubsystem();
	if (!Sub)
	{
		OnFailure.Broadcast(MakeError(TEXT("Could not get CometChat Subsystem."), TEXT("ERR_SUBSYSTEM")));
		SetReadyToDestroy();
		return;
	}

	// Configure the SDK (synchronous — sets up SDK instance, tick pump, listeners)
	Sub->ConfigureWithSettings(AppId, Settings);

	chatsdk::ChatSDK* Sdk = Sub->GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK configuration failed."), TEXT("ERR_CONFIG")));
		SetReadyToDestroy();
		return;
	}

	// Check if there's a saved session to restore
	if (!Sub->HasSavedSession())
	{
		// No saved session — SDK is configured, user needs to login manually
		OnSuccess.Broadcast(FCometChatUser());
		SetReadyToDestroy();
		return;
	}

	// Load saved token and attempt auto-login
	UCometChatAuthSave* Loaded = Cast<UCometChatAuthSave>(
		UGameplayStatics::LoadGameFromSlot(UCometChatSubsystem::AuthSaveSlotName, 0));

	if (!Loaded || Loaded->AuthToken.IsEmpty())
	{
		// Corrupted save — clear it and succeed with no user
		Sub->ClearSavedSession();
		OnSuccess.Broadcast(FCometChatUser());
		SetReadyToDestroy();
		return;
	}

	// Restore session with saved auth token
	Sdk->login_with_auth_token(
		TCHAR_TO_UTF8(*Loaded->AuthToken),
		[WeakThis = TWeakObjectPtr<UCometChatConfigureAction>(this)](const std::string& err)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			UCometChatSubsystem* Sub = Self->GetSubsystem();

			if (err.empty())
			{
				// Session restored successfully
				Sub->SetLoggedIn(true);
				Self->OnSuccess.Broadcast(Sub->GetLoggedInUser());
			}
			else
			{
				// Token expired/invalid — clear saved session, still succeed (SDK is configured)
				Sub->ClearSavedSession();
				Self->OnSuccess.Broadcast(FCometChatUser());
			}
			Self->SetReadyToDestroy();
		}
	);
}
