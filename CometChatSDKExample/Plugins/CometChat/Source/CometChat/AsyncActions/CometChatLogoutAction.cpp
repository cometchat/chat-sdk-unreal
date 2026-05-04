#include "CometChatLogoutAction.h"
#include "chatsdk/cometchat.h"

UCometChatLogoutAction* UCometChatLogoutAction::LogoutAsync(UObject* WorldContextObject)
{
	UCometChatLogoutAction* Node = NewObject<UCometChatLogoutAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatLogoutAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(TEXT("SDK not configured."));
		SetReadyToDestroy();
		return;
	}

	Sdk->logout(
		[WeakThis = TWeakObjectPtr<UCometChatLogoutAction>(this)](const std::string& err)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->GetSubsystem()->SetLoggedIn(false);
				Self->OnSuccess.Broadcast();
			}
			else
			{
				Self->OnFailure.Broadcast(UTF8_TO_TCHAR(err.c_str()));
			}
			Self->SetReadyToDestroy();
		}
	);
}
