local ffi = require("ffi")

pcall(function()
    ffi.cdef([[
        typedef struct spectre_color_t { float r, g, b, a; } spectre_color_t;
        typedef struct spectre_renderable_t { char dummy; } spectre_renderable_t;
        typedef struct spectre_2D_transform_component_t {
            float position_x, position_y, position_z;
            float scale_x, scale_y;
            float origin_x, origin_y;
            float rotation;
        } spectre_2D_transform_component_t;
        typedef struct spectre_rectange_renderable_t {
            float width, height;
            spectre_color_t fill_color;
            spectre_color_t outline_color;
            float outline_thickness;
        } spectre_rectange_renderable_t;
        typedef struct spectre_circle_renderable_t {
            float radius;
            spectre_color_t fill_color;
            spectre_color_t outline_color;
            float outline_thickness;
        } spectre_circle_renderable_t;
        typedef struct spectre_polygone_renderable_t {
            float radius;
            uint32_t point_count;
            spectre_color_t fill_color;
            spectre_color_t outline_color;
            float outline_thickness;
        } spectre_polygone_renderable_t;
        typedef struct spectre_custom_polygone_renderable_t {
            float *vertices;
            uint32_t vertex_count;
            spectre_color_t fill_color;
            spectre_color_t outline_color;
            float outline_thickness;
        } spectre_custom_polygone_renderable_t;
        typedef struct spectre_ligne_renderable_t {
            double position_x1, position_y1, position_x2, position_y2;
            spectre_color_t color;
            float thickness;
        } spectre_ligne_renderable_t;
        typedef struct spectre_input_state_t {
            uint64_t keys_down[8];
            uint64_t keys_pressed[8];
            uint64_t keys_released[8];
            float mouse_position[2];
            float mouse_delta[2];
        } spectre_input_state_t;
        typedef struct spectre_window_component_t {
            uint32_t width, height;
            int32_t position_x, position_y;
            uint32_t min_width, min_height;
            uint32_t max_width, max_height;
            const char *title;
            bool vsync, fullscreen, borderless, resizable, always_on_top;
            bool minimized, maximized, visible, focused, hovered;
            bool cursor_visible, cursor_locked;
            void *native_handle;
        } spectre_window_component_t;
    ]])
end)

local spectre = {}

function spectre.init(world)
    local components = {
        Color = "spectre_color_t",
        Renderable = "spectre_renderable_t",
        Transform2D = "spectre_2D_transform_component_t",
        RectangeRenderable = "spectre_rectange_renderable_t",
        CircleRenderable = "spectre_circle_renderable_t",
        PolygoneRenderable = "spectre_polygone_renderable_t",
        CustomPolygoneRenderable = "spectre_custom_polygone_renderable_t",
        LigneRenderable = "spectre_ligne_renderable_t",
        InputState = "spectre_input_state_t",
        WindowComponent = "spectre_window_component_t"
    }

    for name, c_type in pairs(components) do
        local id = world:lookup(name)
        if id then
            world:bind_struct(id, c_type)
            spectre[name] = id
        end
    end
end

return spectre
