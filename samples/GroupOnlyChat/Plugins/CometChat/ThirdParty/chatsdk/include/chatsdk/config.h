#pragma once
#include <string>
#include <functional>
#include <memory>

namespace chatsdk {

// Forward declarations for transport interfaces (defined in private/transport/)
namespace internal {
    class IHttpTransport;
    class IWebSocketTransport;
}

// Factory typedefs for optional custom transport injection.
// When set, the SDK uses the returned transport instead of the platform default.
using HttpTransportFactory = std::function<std::unique_ptr<internal::IHttpTransport>()>;
using WebSocketTransportFactory = std::function<std::unique_ptr<internal::IWebSocketTransport>()>;

struct Config {
    std::string base_url;
    std::string app_id;
    std::string region;
    bool enable_websocket = true;
    int http_timeout_ms = 30000;

    // Optional: override default platform transport.
    // When null (default), the SDK uses create_default_transports().
    HttpTransportFactory http_transport_factory;
    WebSocketTransportFactory ws_transport_factory;
};

} // namespace chatsdk
