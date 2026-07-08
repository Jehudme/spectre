#pragma once

#include <sandbox/sdk/properties.hpp>
#include <spectre/services/renderer_components.h>
#include <string>
#include <optional>

namespace spectre::utilities {

    // ─────────────────────────────────────────────────────────────────────────
    // Color helpers
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief Reads a color from a properties node using the {r, g, b, a} object format.
     *        Returns std::nullopt if the node is missing or incomplete.
     *
     * @param props  The parent properties node.
     * @param key    The key of the color sub-object.
     */
    inline std::optional<spectre_color_t> try_read_color(sandbox::properties& props, const std::string& key) {
        auto color_node = props.sub(key);
        if (!color_node.is_valid()) return std::nullopt;

        if (!color_node.has("r") || !color_node.has("g") || !color_node.has("b") || !color_node.has("a")) {
            return std::nullopt;
        }

        return spectre_color_t{
            static_cast<float>(color_node.get<double>("r").value_or(1.0)),
            static_cast<float>(color_node.get<double>("g").value_or(1.0)),
            static_cast<float>(color_node.get<double>("b").value_or(1.0)),
            static_cast<float>(color_node.get<double>("a").value_or(1.0))
        };
    }

    /**
     * @brief Reads a color, falling back to a provided default if the node is
     *        absent or malformed. Prefer try_read_color() when you want to
     *        detect the absence explicitly.
     */
    inline spectre_color_t read_color_or(sandbox::properties& props, const std::string& key, spectre_color_t fallback) {
        return try_read_color(props, key).value_or(fallback);
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Vec2 helpers (using {x, y} object format — arrays are unreliable in Glaze)
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief Reads a 2D vector from an {x, y} object node.
     *        Returns std::nullopt if absent or malformed.
     */
    inline std::optional<std::pair<float, float>> try_read_vec2(sandbox::properties& props, const std::string& key) {
        auto node = props.sub(key);
        if (!node.is_valid() || !node.has("x") || !node.has("y")) return std::nullopt;

        return std::make_pair(
            static_cast<float>(node.get<double>("x").value_or(0.0)),
            static_cast<float>(node.get<double>("y").value_or(0.0))
        );
    }

} // namespace spectre::utilities
