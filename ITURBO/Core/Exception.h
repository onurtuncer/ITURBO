
#pragma once

#include <stdexcept>
#include <string>

#include "ITURBO/Core/Error.h"
#include "ITURBO/Core/Export.h"

namespace ITURBO::Core {

    class ITURBO_CORE_API Exception : public std::runtime_error {
    public:
        explicit Exception(const std::string& message);
        Exception(ErrorCode code, const std::string& message);

        [[nodiscard]] ErrorCode Code() const noexcept;

    private:
        ErrorCode m_Code{ ErrorCode::Unknown };
    };

} // namespace ITURBO::Core