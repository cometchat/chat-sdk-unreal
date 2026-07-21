<p align="center">
  <img alt="CometChat" src="https://assets.cometchat.io/website/images/logos/banner.png">
</p>

# CometChat Chat SDK for Unreal Engine

CometChat Unreal Engine plugin for adding real-time chat to your Unreal projects. Supports Mac, Windows, iOS, and Android.

---

## Getting Started

To set up the CometChat Chat SDK and utilize CometChat for your chat functionality, you'll need to follow these steps:

1. Registration: Go to the [CometChat Dashboard](https://app.cometchat.com/) and sign up for an account.
2. After registering, log into your CometChat account and create a new app. Once created, CometChat will generate an Auth Key and App ID for you. Keep these credentials secure as you'll need them later.
3. Check the [Key Concepts](https://www.cometchat.com/docs/fundamentals/key-concepts) to understand the basic components of CometChat.
4. Refer to the [Overview](https://www.cometchat.com/docs/sdk/unreal/overview) for a high-level understanding of the Unreal SDK.
5. Follow the [Setup Guide](https://www.cometchat.com/docs/sdk/unreal/setup) for step-by-step integration instructions.

## Supported Engine Versions

| Version | Precompiled Binaries Available |
|---------|-------------------|
| UE 5.5 | Mac, Windows |
| UE 5.7 | Mac |

Building from source additionally supports iOS and Android on both engine versions.

## Repository Structure

```
├── Plugins/
│   └── CometChatSdk/                      ← Plugin source + ThirdParty libs (the SDK)
│       ├── CometChat.uplugin
│       ├── Config/
│       ├── Source/
│       └── ThirdParty/chatsdk/            ← Prebuilt static libraries (all platforms)
│
├── samples/                               ← Both samples reference Plugins/CometChatSdk
│   ├── GroupOnlyChat/                     ← Simple group chat sample
│   │   ├── GroupOnlyChat.uproject
│   │   ├── Config/
│   │   └── Content/
│   │
│   └── GroupTabbedChatChannel/            ← Tabbed chat channel sample with conversations
│       ├── GroupTabbedChatChannel.uproject
│       ├── Config/
│       └── Content/
│
├── README.md
├── CHANGELOG.md
└── LICENSE
```

## Prerequisites

- Unreal Engine 5.5 or 5.7
- C++ development tools for your platform
- CometChat account with App ID, Auth Key, and Region

## Installation

### Option 1: Use Precompiled Binaries (No Build Required)

Precompiled builds are published to Cloudsmith — they are not stored in this repo.

1. Download the package for your engine version:

   **UE 5.5 (Mac + Windows):**
   ```bash
   curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/1.0.0/CometChat-UE5.5-precompiled.zip'
   ```

   **UE 5.7 (Mac):**
   ```bash
   curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/1.0.0/CometChat-UE5.7-precompiled.zip'
   ```

2. Extract and copy the folder for your platform into your project's `Plugins/CometChat/`
   directory, so that `CometChat.uplugin` sits directly inside it:

   | Package | Folder to copy |
   |---------|----------------|
   | `CometChat-UE5.5-precompiled.zip` | `5.5/mac/` or `5.5/windows/` |
   | `CometChat-UE5.7-precompiled.zip` | `5.7/` |

   Each folder is a complete, self-contained plugin — it already includes the source,
   `ThirdParty/` static libraries, and the prebuilt editor binary. Do not merge it with
   `Plugins/CometChatSdk/` from this repo.

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

4. Open your project — no compilation needed

### Option 2: Build from Source

1. Copy `Plugins/CometChatSdk/` into your project's `Plugins/CometChat/` directory
2. Regenerate project files and build

This is the path to use for iOS and Android, which compile from source and link the
bundled static libraries automatically.

### Run the Sample Projects

This repo includes two sample projects:

- **GroupOnlyChat** — A simple group chat integration with a first-person shooter template
- **GroupTabbedChatChannel** — A tabbed chat panel with conversations, one-on-one, and group messaging

To run a sample:

1. Clone this repo
2. Open `samples/<SampleName>/<SampleName>.uproject` in Unreal Editor
3. Configure your App ID, Auth Key, and Region
4. Play in editor to test

Both samples reference `Plugins/CometChatSdk/` at the repo root via
`AdditionalPluginDirectories` in their `.uproject`, so there is nothing to copy —
they build the SDK from source alongside the sample.

To run a sample against the precompiled binaries instead, replace the contents of
`Plugins/CometChatSdk/` with the platform folder from the Cloudsmith package.

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
