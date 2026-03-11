#pragma once

#include <string_view>

namespace ITURBO::Core {

    enum class BuildType {
        Debug,
        Release,
        RelWithDebInfo,
        MinSizeRel,
        Unknown
    };

    enum class ExecutionSpace {
        Serial,
        OpenMP,
        MPI,
        Hybrid
    };

    enum class FloatingPointPrecision {
        Single,
        Double
    };

    [[nodiscard]] constexpr std::string_view ToString(BuildType value) noexcept {
        switch (value) {
        case BuildType::Debug:
            return "Debug";
        case BuildType::Release:
            return "Release";
        case BuildType::RelWithDebInfo:
            return "RelWithDebInfo";
        case BuildType::MinSizeRel:
            return "MinSizeRel";
        default:
            return "Unknown";
        }
    }

    [[nodiscard]] constexpr std::string_view ToString(ExecutionSpace value) noexcept {
        switch (value) {
        case ExecutionSpace::Serial:
            return "Serial";
        case ExecutionSpace::OpenMP:
            return "OpenMP";
        case ExecutionSpace::MPI:
            return "MPI";
        case ExecutionSpace::Hybrid:
            return "Hybrid";
        default:
            return "Unknown";
        }
    }

    [[nodiscard]] constexpr std::string_view ToString(FloatingPointPrecision value) noexcept {
        switch (value) {
        case FloatingPointPrecision::Single:
            return "Single";
        case FloatingPointPrecision::Double:
            return "Double";
        default:
            return "Unknown";
        }
    }

} // namespace ITURBO::Core