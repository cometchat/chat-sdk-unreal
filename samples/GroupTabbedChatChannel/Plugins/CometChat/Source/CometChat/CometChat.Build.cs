using UnrealBuildTool;
using System.IO;

public class CometChat : ModuleRules
{
    public CometChat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine",
            "ApplicationCore",
            "UMG",
            "Slate",
            "SlateCore",
            "InputCore",
            "HTTP",
            "ImageWrapper",
            "RenderCore",
            "RHI"
        });
        
        // Path to the SDK third-party libraries
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "chatsdk");
        
        // Include SDK headers
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));
        
        // Link the static library based on platform
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Win64", "chatsdk.lib"));
            
            // If using libcurl, link it too
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Win64", "libcurl.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Mac", "libchat-sdk.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Mac", "libixwebsocket.a"));
            
            // Use UE5's bundled OpenSSL (universal binary)
            string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
            string OpenSSLPath = Path.Combine(EngineDir, "Source", "ThirdParty", "OpenSSL", "1.1.1t", "lib", "Mac");
            PublicAdditionalLibraries.Add(Path.Combine(OpenSSLPath, "libssl.a"));
            PublicAdditionalLibraries.Add(Path.Combine(OpenSSLPath, "libcrypto.a"));
            
            // System libraries for curl
            PublicSystemLibraries.Add("curl");
            PublicSystemLibraries.Add("z");
            
            // macOS system frameworks for networking
            PublicFrameworks.AddRange(new string[] { "Security", "SystemConfiguration", "CoreFoundation" });
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Linux", "libchatsdk.a"));
            PublicAdditionalLibraries.Add("curl");
            PublicAdditionalLibraries.Add("ssl");
            PublicAdditionalLibraries.Add("crypto");
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "IOS", "libchat-sdk.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "IOS", "libixwebsocket.a"));
            
            // iOS uses system curl and SecureTransport — no need to bundle OpenSSL
            PublicSystemLibraries.Add("curl");
            PublicSystemLibraries.Add("z");
            
            // Apple frameworks for networking and TLS
            PublicFrameworks.AddRange(new string[] { 
                "Security", "CFNetwork", "SystemConfiguration", "CoreFoundation" 
            });
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            // Link per-ABI static library
            string AndroidABI = "arm64-v8a"; // UE5 primarily targets arm64
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Android", AndroidABI, "libchat-sdk.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Android", AndroidABI, "libixwebsocket.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Android", AndroidABI, "libcurl.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Android", AndroidABI, "libmbedtls.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Android", AndroidABI, "libmbedx509.a"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Android", AndroidABI, "libmbedcrypto.a"));
            
            PublicSystemLibraries.Add("log");
            PublicSystemLibraries.Add("z");
        }
    }
}
