<p align="center">
  <img alt="CometChat" src="https://assets.cometchat.io/website/images/logos/banner.png">
</p>

# CometChat Chat SDK for Unreal Engine (Beta)

CometChat Unreal Engine plugin for adding real-time chat to your Unreal projects. Supports Mac, Windows, Linux, iOS, and Android.

---

## Getting Started

To set up the CometChat Chat SDK and utilize CometChat for your chat functionality, you'll need to follow these steps:

1. Registration: Go to the [CometChat Dashboard](https://app.cometchat.com/) and sign up for an account.
2. After registering, log into your CometChat account and create a new app. Once created, CometChat will generate an Auth Key and App ID for you. Keep these credentials secure as you'll need them later.
3. Check the [Key Concepts](https://www.cometchat.com/docs/fundamentals/key-concepts) to understand the basic components of CometChat.
4. Refer to the [Overview](https://www.cometchat.com/docs/sdk/unreal/overview) for a high-level understanding of the Unreal SDK.
5. Follow the [Setup Guide](https://www.cometchat.com/docs/sdk/unreal/setup) for step-by-step integration instructions.

## Supported Engine Versions

| Version | Platforms Available |
|---------|-------------------|
| UE 5.5.4 | Mac, Windows |
| UE 5.7.2 | Mac |

## Repository Structure

```
├── Plugins/
│   ├── CometChatSdk/                      ← Plugin source + ThirdParty libs
│   │   ├── CometChat.uplugin
│   │   ├── Config/
│   │   ├── Source/
│   │   └── ThirdParty/chatsdk/            ← Prebuilt static libraries (all platforms)
│   │
│   └── precompiled-binaries/
│       └── README.md                      ← Download links for precompiled binaries (hosted on Cloudsmith)
│
├── CometChatSDKExample/                   ← Sample UE5 project
│   ├── CometChatSDKExample.uproject
│   ├── Config/
│   └── Content/
│
├── README.md
├── CHANGELOG.md
└── LICENSE
```

## Prerequisites

- Unreal Engine 5.5.4 or 5.7.2
- C++ development tools for your platform
- CometChat account with App ID, Auth Key, and Region

## Installation

### Option 1: Use Precompiled Binaries (No Build Required)

1. Copy `Plugins/CometChatSdk/` into your project's `Plugins/CometChat/` directory
2. Download the precompiled binaries for your engine version:

   **UE 5.5.4 (Mac + Windows):**
   ```bash
   curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/v1.0.0-beta.1/CometChat-UE5.5.4-precompiled.zip'
   ```

   **UE 5.7.2 (Mac):**
   ```bash
   curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/v1.0.0-beta.1/CometChat-UE5.7.2-precompiled.zip'
   ```

3. Extract the zip contents into your project's `Plugins/CometChat/` directory (merging with existing)
4. Enable the plugin in your `.uproject`:

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

5. Open your project — no compilation needed

### Option 2: Build from Source

1. Copy `Plugins/CometChatSdk/` into your project's `Plugins/CometChat/` directory
2. Regenerate project files and build

### Run the Sample Project

1. Clone this repo
2. Copy the plugin (with precompiled binaries for your version) into `CometChatSDKExample/Plugins/CometChat/`
3. Open `CometChatSDKExample/CometChatSDKExample.uproject` in Unreal Editor
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
| Login with Auth Token | Authenticate using an auth token |
| Logout | End the current session |
| Get User | Fetch user details by UID |
| Send Message | Send a text message to a user |
| Send Group Message | Send a text message to a group |
| Get Messages | Fetch message history for a user conversation |
| Get Group Messages | Fetch message history for a group |
| Create Group | Create a new group |
| Join Group | Join an existing group |
| Leave Group | Leave a group |

---

## Help and Support

For issues running the project or integrating with the CometChat SDK, consult our [documentation](https://www.cometchat.com/docs/sdk/unreal/overview) or create a [support ticket](https://help.cometchat.com/hc/en-us) or seek real-time support via the [CometChat Dashboard](https://app.cometchat.com/).
