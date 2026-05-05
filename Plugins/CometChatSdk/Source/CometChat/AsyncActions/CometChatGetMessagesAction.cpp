#include "CometChatGetMessagesAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatGetMessagesAction* UCometChatGetMessagesAction::GetMessagesAsync(UObject* WorldContextObject, const FString& Uid, int32 Limit)
{
	UCometChatGetMessagesAction* Node = NewObject<UCometChatGetMessagesAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Uid = Uid;
	Node->Limit = Limit;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatGetMessagesAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(TEXT("SDK not configured."));
		SetReadyToDestroy();
		return;
	}

	Sdk->get_messages(
		TCHAR_TO_UTF8(*Uid),
		Limit,
		[WeakThis = TWeakObjectPtr<UCometChatGetMessagesAction>(this)](const std::string& err, const std::vector<chatsdk::Message>& msgs)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				TArray<FCometChatMessage> Out;
				Out.Reserve(msgs.size());
				for (const auto& m : msgs)
				{
					Out.Add(CometChatConversions::ToUnreal(m));
				}
				Self->OnSuccess.Broadcast(Out);
			}
			else
			{
				Self->OnFailure.Broadcast(UTF8_TO_TCHAR(err.c_str()));
			}
			Self->SetReadyToDestroy();
		}
	);
}
