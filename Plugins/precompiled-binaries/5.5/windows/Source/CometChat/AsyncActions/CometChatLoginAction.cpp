#include "CometChatLoginAction.h"
#include "chatsdk/cometchat.h"

UCometChatLoginAction* UCometChatLoginAction::LoginAsync(UObject* WorldContextObject, const FString& Uid, const FString& AuthKey)
{
	UCometChatLoginAction* Node = NewObject<UCometChatLoginAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Uid = Uid;
	Node->AuthKey = AuthKey;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatLoginAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured. Call Configure first."), TEXT("ERR_SDK")));
		SetReadyToDestroy();
		return;
	}

	Sdk->login(
		TCHAR_TO_UTF8(*Uid),
		TCHAR_TO_UTF8(*AuthKey),
		[WeakThis = TWeakObjectPtr<UCometChatLoginAction>(this)](const std::string& err)
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
