
#pragma once

#include <string_view>

#include "ITURBO/Core/Exception.h"

namespace ITURBO::Core {

    inline void Assert(bool condition, std::string_view message) {
        if (!condition) {
            throw Exception(ErrorCode::InvalidState, std::string(message));
        }
    }

    inline void AssertArgument(bool condition, std::string_view message) {
        if (!condition) {
            throw Exception(ErrorCode::InvalidArgument, std::string(message));
        }
    }

    inline void AssertRange(bool condition, std::string_view message) {
        if (!condition) {
            throw Exception(ErrorCode::OutOfRange, std::string(message));
        }
    }

} // namespace ITURBO::Core