#include "CometChatSendMessageAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatSendMessageAction* UCometChatSendMessageAction::SendMessageAsync(UObject* WorldContextObject, const FString& ReceiverUid, const FString& Text)
{
	UCometChatSendMessageAction* Node = NewObject<UCometChatSendMessageAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->ReceiverUid = ReceiverUid;
	Node->Text = Text;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatSendMessageAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
		SetReadyToDestroy();
		return;
	}

	Sdk->send_message(
		TCHAR_TO_UTF8(*ReceiverUid),
		TCHAR_TO_UTF8(*Text),
		[WeakThis = TWeakObjectPtr<UCometChatSendMessageAction>(this)](const std::string& err, const chatsdk::Message& msg)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->OnSuccess.Broadcast(CometChatConversions::ToUnreal(msg));
			}
			else
			{
				Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
			}
			Self->SetReadyToDestroy();
		}
	);
}
