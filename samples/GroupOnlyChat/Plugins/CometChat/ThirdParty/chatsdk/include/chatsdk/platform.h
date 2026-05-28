#pragma once

// ============================================================
// platform.h — Compile-time platform detection for the Chat SDK.
//
// Defines exactly one CHATSDK_PLATFORM_* macro based on the
// target platform, plus a Platform enum and helper functions.
// ============================================================

// --- Compile-time platform detection ---

#if defined(__EMSCRIPTEN__)
    #define CHATSDK_PLATFORM_WEB 1
    #define CHATSDK_PLATFORM_IOS 0
    #define CHATSDK_PLATFORM_ANDROID 0
    #define CHATSDK_PLATFORM_DESKTOP 0

#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define CHATSDK_PLATFORM_WEB 0
        #define CHATSDK_PLATFORM_IOS 1
        #define CHATSDK_PLATFORM_ANDROID 0
        #define CHATSDK_PLATFORM_DESKTOP 0
    #else
        // macOS desktop falls through here
        #define CHATSDK_PLATFORM_WEB 0
        #define CHATSDK_PLATFORM_IOS 0
        #define CHATSDK_PLATFORM_ANDROID 0
        #define CHATSDK_PLATFORM_DESKTOP 1
    #endif

#elif defined(__ANDROID__)
    #define CHATSDK_PLATFORM_WEB 0
    #define CHATSDK_PLATFORM_IOS 0
    #define CHATSDK_PLATFORM_ANDROID 1
    #define CHATSDK_PLATFORM_DESKTOP 0

#else
    // Win32, Linux, macOS desktop (non-Apple path)
    #define CHATSDK_PLATFORM_WEB 0
    #define CHATSDK_PLATFORM_IOS 0
    #define CHATSDK_PLATFORM_ANDROID 0
    #define CHATSDK_PLATFORM_DESKTOP 1

#endif

// Verify exactly one platform macro is active
static_assert(
    CHATSDK_PLATFORM_DESKTOP + CHATSDK_PLATFORM_IOS +
    CHATSDK_PLATFORM_ANDROID + CHATSDK_PLATFORM_WEB == 1,
    "Exactly one CHATSDK_PLATFORM_* macro must be defined"
);

namespace chatsdk {

enum class Platform { Desktop, iOS, Android, Web };

constexpr Platform current_platform() {
#if CHATSDK_PLATFORM_WEB
    return Platform::Web;
#elif CHATSDK_PLATFORM_IOS
    return Platform::iOS;
#elif CHATSDK_PLATFORM_ANDROID
    return Platform::Android;
#else
    return Platform::Desktop;
#endif
}

constexpr const char* platform_name() {
    return current_platform() == Platform::Desktop ? "Desktop" :
           current_platform() == Platform::iOS     ? "iOS" :
           current_platform() == Platform::Android  ? "Android" :
           "Web";
}

} // namespace chatsdk
