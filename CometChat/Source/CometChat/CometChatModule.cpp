#include "CometChatModule.h"

IMPLEMENT_MODULE(FCometChatModule, CometChat)

void FCometChatModule::StartupModule()
{
    // Static library is linked via Build.cs — no runtime loading needed.
}

void FCometChatModule::ShutdownModule()
{
    // Cleanup handled by UCometChatSubsystem::Deinitialize()
}
