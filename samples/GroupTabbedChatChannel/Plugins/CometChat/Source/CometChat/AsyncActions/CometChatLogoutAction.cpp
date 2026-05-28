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
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
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
				Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
			}
			Self->SetReadyToDestroy();
		}
	);
}
