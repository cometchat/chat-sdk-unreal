# Changelog

## 1.0.0-beta.2

## New
- Introduced `CometChatPanel`, a game-style chat UI with conversations, one-on-one chats, and group chats.
- Added support for typing indicators, transient messaging, moderation, and user/group search.
- Introduced listener support for messages, users, groups, calls, connections, login events, and AI assistant events.
- Added auth token persistence with automatic session restoration.
- Added manual socket connection management through the `bAutoEstablishSocketConnection` configuration option.
- Added Blueprint-callable async SDK configuration support.

## Enhancements
- Enhanced HTTP and WebSocket security by enabling certificate validation using the device trusted CA store across all platforms.
- Improved networking performance with non-blocking asynchronous HTTP transport for better Unreal Engine compatibility.
- Added support for Mac (`arm64`, `x86_64`), iOS (`arm64`), and Android (`arm64-v8a`, `armeabi-v7a`) platforms.
- Added prebuilt plugin support for Unreal Engine `5.5.x` and `5.7.x`.
- Improved multi-platform build workflows and updated platform-specific TLS implementations.

## Fixes
- Fixed editor freezes caused by blocking HTTP requests.
- Resolved duplicate message delivery from WebSocket events.
- Fixed message ordering, UTC timestamp handling, and presence parsing inconsistencies.
- Fixed real-time online/offline status updates.

## 1.0.0-beta.1

- Initial release of CometChat Unreal Engine plugin
- Login, logout, user authentication
- 1:1 and group text messaging
- Message history fetching
- Group create, join, leave operations
- Real-time event bridge for incoming messages
- Group chat box UI widget
- Platform support: Mac, Android (arm64-v8a, armeabi-v7a)
- Sample project with first-person shooter template integration
