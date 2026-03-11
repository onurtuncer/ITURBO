#pragma once

#include <string>

#include "ITURBO/Core/Export.h"

namespace ITURBO::Core {

    enum class ErrorCode {
        None = 0,
        InvalidArgument,
        InvalidState,
        FileIO,
        NotImplemented,
        OutOfRange,
        NumericalFailure,
        LinearSolverFailure,
        MeshError,
        ConfigurationError,
        Unknown
    };

    class ITURBO_CORE_API Error final {
    public:
        Error() = default;

        Error(ErrorCode code, std::string message);

        [[nodiscard]] ErrorCode Code() const noexcept;
        [[nodiscard]] const std::string& Message() const noexcept;
        [[nodiscard]] bool Ok() const noexcept;

    private:
        ErrorCode m_Code{ ErrorCode::None };
        std::string m_Message{};
    };

    [[nodiscard]] ITURBO_CORE_API const char* ToString(ErrorCode code) noexcept;

} // namespace ITURBO::Core