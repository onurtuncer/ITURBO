
#include "TurboCFD/Core/Error.h"

namespace TurboCFD::Core {

    Error::Error(ErrorCode code, std::string message)
        : m_Code(code)
        , m_Message(std::move(message)) {
    }

    ErrorCode Error::Code() const noexcept {
        return m_Code;
    }

    const std::string& Error::Message() const noexcept {
        return m_Message;
    }

    bool Error::Ok() const noexcept {
        return m_Code == ErrorCode::None;
    }

    const char* ToString(ErrorCode code) noexcept {
        switch (code) {
        case ErrorCode::None:
            return "None";
        case ErrorCode::InvalidArgument:
            return "InvalidArgument";
        case ErrorCode::InvalidState:
            return "InvalidState";
        case ErrorCode::FileIO:
            return "FileIO";
        case ErrorCode::NotImplemented:
            return "NotImplemented";
        case ErrorCode::OutOfRange:
            return "OutOfRange";
        case ErrorCode::NumericalFailure:
            return "NumericalFailure";
        case ErrorCode::LinearSolverFailure:
            return "LinearSolverFailure";
        case ErrorCode::MeshError:
            return "MeshError";
        case ErrorCode::ConfigurationError:
            return "ConfigurationError";
        default:
            return "Unknown";
        }
    }

} // namespace TurboCFD::Core