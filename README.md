<p align="center">
  <img alt="CometChat" src="https://assets.cometchat.io/website/images/logos/banner.png">
</p>

# CometChat Chat SDK for Unreal Engine

CometChat Unreal Engine plugin for adding real-time chat to your Unreal projects. Supports Mac, Windows, Linux, iOS, and Android.

## Repository Structure

```
├── CometChat/              ← Plugin (copy to YourProject/Plugins/)
│   ├── CometChat.uplugin
│   ├── Source/
│   └── ThirdParty/
│
└── CometChatSDKExample/    ← Sample UE5 project
    ├── CometChatSDKExample.uproject
    ├── Config/
    ├── Content/
    └── Plugins/
        └── CometChat/
```

## Prerequisites

- Unreal Engine 5.7+
- C++ development tools for your platform
- CometChat account with App ID, Auth Key, and Region

## Quick Start

### Use the Plugin in Your Project

1. Clone this repo or download the `CometChat/` folder
2. Copy `CometChat/` into your project's `Plugins/` directory
3. Enable the plugin in your `.uproject`:

```json
{
  "Plugins": [
    {
      "Name": "CometChat",
      "Enabled": true
    }
  ]
}
```

4. Regenerate project files and build

### Run the Sample Project

1. Clone this repo
2. Open `CometChatSDKExample/CometChatSDKExample.uproject` in Unreal Editor
3. The CometChat plugin is already included in `Plugins/`
4. Configure your App ID, Auth Key, and Region
5. Play in editor to test

## Plugin Architecture

The plugin provides:

- `UCometChatSubsystem` — Game instance subsystem for CometChat operations
- Async Blueprint actions for login, logout, send message, fetch messages, groups
- `CometChatEventBridge` — Real-time event callbacks
- `CometChatGroupChatBox` — Ready-to-use group chat UI widget
- Native C++ chat SDK via `ThirdParty/chatsdk/` (prebuilt static libraries)

### Available Blueprint Actions

| Action | Description |
|--------|-------------|
| Login | Authenticate a user with UID and Auth Key |
| Logout | End the current session |
| Get User | Fetch user details by UID |
| Send Message | Send a text message to a user |
| Send Group Message | Send a text message to a group |
| Get Messages | Fetch message history for a user conversation |
| Get Group Messages | Fetch message history for a group |
| Create Group | Create a new group |
| Join Group | Join an existing group |
| Leave Group | Leave a group |


