# CometChat SDK Example — Unreal Engine

A sample Unreal Engine 5.7 project demonstrating the CometChat Chat SDK plugin. Includes a first-person shooter template with an integrated group chat UI.

## How to Run

1. Open `GroupTabbedChatChannel.uproject` in Unreal Editor 5.7+
2. The CometChat plugin is pre-installed in `Plugins/CometChat/`
3. Configure your CometChat credentials (App ID, Auth Key, Region)
4. Play in editor

## What's Included

- First-person shooter template with CometChat integration
- Group chat box widget (`CometChatPanel`)
- Blueprint examples for login, messaging, and group operations

## Project Structure

```
GroupTabbedChatChannel/
├── GroupTabbedChatChannel.uproject
├── Config/                  ← Engine and input settings
├── Content/                 ← Blueprints, UI, maps, assets
└── Plugins/
    └── CometChat/           ← CometChat plugin (bundled)
```

