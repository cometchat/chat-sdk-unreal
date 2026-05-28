#include "CometChatLoginWithAuthTokenAction.h"
#include "chatsdk/cometchat.h"

UCometChatLoginWithAuthTokenAction* UCometChatLoginWithAuthTokenAction::LoginWithAuthTokenAsync(UObject* WorldContextObject, const FString& AuthToken)
{
	UCometChatLoginWithAuthTokenAction* Node = NewObject<UCometChatLoginWithAuthTokenAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->AuthToken = AuthToken;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatLoginWithAuthTokenAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured. Call Configure first."), TEXT("ERR_SDK")));
		SetReadyToDestroy();
		return;
	}

	Sdk->login_with_auth_token(
		TCHAR_TO_UTF8(*AuthToken),
		[WeakThis = TWeakObjectPtr<UCometChatLoginWithAuthTokenAction>(this)](const std::string& err)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->GetSubsystem()->SetLoggedIn(true);
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
