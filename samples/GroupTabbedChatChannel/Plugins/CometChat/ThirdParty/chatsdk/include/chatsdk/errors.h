#pragma once
#include <stdexcept>
#include <string>

namespace chatsdk {

class ChatError : public std::runtime_error {
public:
    explicit ChatError(const std::string& msg) : std::runtime_error(msg) {}
};

class AuthError : public std::runtime_error {
public:
    explicit AuthError(const std::string& msg) : std::runtime_error(msg) {}
};

} // namespace chatsdk
