#include "CometChatSendGroupMessageAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatSendGroupMessageAction* UCometChatSendGroupMessageAction::SendGroupMessageAsync(UObject* WorldContextObject, const FString& Guid, const FString& Text)
{
	UCometChatSendGroupMessageAction* Node = NewObject<UCometChatSendGroupMessageAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Guid = Guid;
	Node->Text = Text;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatSendGroupMessageAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(TEXT("SDK not configured."));
		SetReadyToDestroy();
		return;
	}

	Sdk->send_group_message(
		TCHAR_TO_UTF8(*Guid),
		TCHAR_TO_UTF8(*Text),
		[WeakThis = TWeakObjectPtr<UCometChatSendGroupMessageAction>(this)](const std::string& err, const chatsdk::Message& msg)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->OnSuccess.Broadcast(CometChatConversions::ToUnreal(msg));
			}
			else
			{
				Self->OnFailure.Broadcast(UTF8_TO_TCHAR(err.c_str()));
			}
			Self->SetReadyToDestroy();
		}
	);
}
