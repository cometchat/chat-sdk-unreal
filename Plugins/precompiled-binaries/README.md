# Precompiled Binaries

Precompiled plugin binaries for the CometChat Unreal Engine plugin. These allow you to use the plugin without compiling from source.

## Downloads

| UE Version | Platform | Download |
|------------|----------|----------|
| 5.5.4 | Mac, Windows | `curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/v1.0.0-beta.1/CometChat-UE5.5.4-precompiled.zip'` |
| 5.7.2 | Mac | `curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/v1.0.0-beta.1/CometChat-UE5.7.2-precompiled.zip'` |

## Version History

| Plugin Version | UE 5.5.4 | UE 5.7.2 | Date |
|---------------|-----------|-----------|------|
| 1.0.0-beta.1 | ✅ Mac, Windows | ✅ Mac | 2026-05-05 |

## Installation

1. Download the precompiled zip for your engine version and platform:

   **UE 5.5.4 (Mac + Windows):**
   ```bash
   curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/v1.0.0-beta.1/CometChat-UE5.5.4-precompiled.zip'
   ```

   **UE 5.7.2 (Mac):**
   ```bash
   curl -1sLf -O 'https://dl.cloudsmith.io/public/cometchat/cometchat/raw/versions/v1.0.0-beta.1/CometChat-UE5.7.2-precompiled.zip'
   ```

2. Extract the zip contents into your project's `Plugins/CometChat/` directory (merge with the source plugin from `Plugins/CometChatSdk/`)
3. The extracted files overlay on top of the source plugin, adding `Binaries/` and `Intermediate/` folders
4. Open your project in Unreal Editor — no compilation needed

## Building from Source (Alternative)

If precompiled binaries are not available for your platform, you can build from source:

1. Copy `Plugins/CometChatSdk/` into your project's `Plugins/CometChat/`
2. Regenerate project files and build
