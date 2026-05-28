#include "CometChatModule.h"

#if PLATFORM_ANDROID
#include <jni.h>
#include "Android/AndroidApplication.h"

// Forward declare — defined in android_ca_certs.cpp (linked from libchat-sdk.a)
namespace chatsdk { void android_set_java_vm(JavaVM* vm); }
#endif

IMPLEMENT_MODULE(FCometChatModule, CometChat)

void FCometChatModule::StartupModule()
{
#if PLATFORM_ANDROID
    // Get JNIEnv from UE's Android application layer, then extract JavaVM.
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();
    if (Env)
    {
        JavaVM* Vm = nullptr;
        Env->GetJavaVM(&Vm);
        if (Vm)
        {
            chatsdk::android_set_java_vm(Vm);
        }
    }
#endif
}

void FCometChatModule::ShutdownModule()
{
    // Cleanup handled by UCometChatSubsystem::Deinitialize()
}
