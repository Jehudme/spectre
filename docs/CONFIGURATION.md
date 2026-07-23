# Spectre Engine Configuration Guide

This document provides a comprehensive guide to configuring the Spectre Engine, creating components, prefabs, scenes, states, and global resources using JSON configuration files.

## 1. Global Configurations

All global configurations are located in `app://configs/` and loaded by various engine modules during startup.

### 1.1 `configuration.json`
The root configuration file that specifies which modules are loaded into the engine.

```json
{
    "engine": {
        "sandbox": [
            "sandbox-configuration@1.0.0",
            "sandbox-logs@1.0.0",
            "sandbox-filesystem@1.0.0",
            "spectre-components@1.0.0",
            "spectre-serializer@1.0.0",
            "spectre-runtime@1.0.0",
            "spectre-window@1.0.0",
            "spectre-renderer@1.0.0",
            "spectre-scripts@1.0.0",
            "spectre-prefabs@1.0.0",
            "spectre-scenes@1.0.0",
            "spectre-resources@1.0.0"
        ]
    }
}
```

### 1.2 `window.json`
Configures the main window dimensions, title, and vsync mode. 
Path: `configs/window.json`

```json
{
    "title": "Application Title",
    "width": 800,
    "height": 600,
    "vsync": true
}
```

### 1.3 `renderer.json`
Configures renderer settings like the default clear color.
Path: `configs/renderer.json`

```json
{
    "clear_color": {
        "r": 0,
        "g": 0,
        "b": 0,
        "a": 255
    }
}
```

### 1.4 `startup.json`
Specifies which states to push to the scene manager upon startup.
Path: `configs/startup.json`

```json
{
    "states": [
        "MainState"
    ]
}
```

### 1.5 `resources.json`
Defines global resources (textures, fonts, shaders) loaded into memory for the application. Each resource is given a key (name) used in components.
Path: `configs/resources.json`

```json
{
  "texture_name": {
    "type": "texture",
    "path": "app://resources/textures/image.png",
    "configurations": {
      "filtering": "point",
      "wrap_mode": "repeat"
    }
  },
  "font_name": {
    "type": "font",
    "path": "app://resources/fonts/font.ttf",
    "configurations": {
      "font_size": 64,
      "filtering": "point"
    }
  }
}
```
**Options**:
- **filtering**: `point`, `bilinear`, `trilinear`
- **wrap_mode**: `repeat`, `clamp`, `mirror_repeat`, `mirror_clamp`

---

## 2. Scenes and States

### 2.1 States
States act as logical groupings of scenes. A state can include multiple scenes to load into the world simultaneously.
Path: `scenes/states/<StateName>.json`

```json
{
    "name": "MainState",
    "scenes": [
        "MainScene"
    ]
}
```

### 2.2 Scenes
Scenes contain entity hierarchies, components, and references to prefabs.
Path: `scenes/scenes/<SceneName>.json`

```json
{
    "name": "MainScene",
    "hierarchies": {
        "EntityName": {
            "prefabs": [
                "RectanglePrefab"
            ],
            "components": {
                "Transform2D": {
                    "position_x": 100.0,
                    "position_y": 100.0
                }
            }
        }
    }
}
```

---

## 3. Prefabs

Prefabs act as reusable entity templates. They are constructed in exactly the same way as an entity in a scene's hierarchy.
Path: `scenes/prefabs/<PrefabName>.json`

```json
{
    "name": "PrefabName",
    "components": {
        "Renderable": {},
        "Transform2D": {
            "position_x": 0.0,
            "position_y": 0.0,
            "position_z": 0.0,
            "scale_x": 1.0,
            "scale_y": 1.0,
            "rotation": 0.0
        }
    }
}
```

---

## 4. Component Dictionary

Below is the complete dictionary of components available for use inside `scenes` and `prefabs`, along with all their parameters. Note that if a component doesn't take parameters, you must still initialize it as an empty object `{}`.

### Transform and Core

- **Transform2D**: Represents the 2D transform (position, scale, rotation) of an entity.
  ```json
  "Transform2D": {
      "position_x": 0.0,
      "position_y": 0.0,
      "position_z": 0.0,
      "scale_x": 1.0,
      "scale_y": 1.0,
      "rotation": 0.0
  }
  ```

- **Renderable**: Tag component. Any entity with a visual representation must have this component.
  ```json
  "Renderable": {}
  ```

### Renderer Shapes

- **RectangleRenderable**: Draws a filled rectangle.
  ```json
  "RectangleRenderable": {
      "width": 100.0,
      "height": 50.0,
      "color": { "r": 255, "g": 255, "b": 255, "a": 255 }
  }
  ```

- **CircleRenderable**: Draws a filled circle.
  ```json
  "CircleRenderable": {
      "radius": 50.0,
      "color": { "r": 255, "g": 255, "b": 255, "a": 255 }
  }
  ```

- **PolygoneRenderable**: Draws a regular polygon.
  ```json
  "PolygoneRenderable": {
      "radius": 50.0,
      "sides": 5,
      "rotation": 0.0,
      "color": { "r": 255, "g": 255, "b": 255, "a": 255 }
  }
  ```

- **LigneRenderable**: Draws a line segment between two points.
  ```json
  "LigneRenderable": {
      "start_x": 0.0,
      "start_y": 0.0,
      "end_x": 100.0,
      "end_y": 100.0,
      "thick": 2.0,
      "color": { "r": 255, "g": 255, "b": 255, "a": 255 }
  }
  ```

### Renderer Textures and Fonts

- **TextureRenderable**: Renders a loaded texture resource. The `name` must match a texture key in `resources.json`.
  ```json
  "TextureRenderable": {
      "name": "texture_name_from_resources",
      "width": 100.0,
      "height": 100.0,
      "tint": { "r": 255, "g": 255, "b": 255, "a": 255 },
      "flip_x": false,
      "flip_y": false
  }
  ```

- **TextRenderable**: Renders text using a loaded font resource. The `name` must match a font key in `resources.json`.
  ```json
  "TextRenderable": {
      "name": "font_name_from_resources",
      "content": "Text string",
      "font_size": 24.0,
      "spacing": 1.0,
      "tint": { "r": 255, "g": 255, "b": 255, "a": 255 },
      "bold": false,
      "italic": false
  }
  ```

### Scripts (Lua)

- **scripts**: Attaches Lua scripts to entity lifecycle events. Scripts run automatically during runtime.
  ```json
  "scripts": {
      "on_update": {
          "0": { "function": "on_update_script_name" }
      },
      "on_create": {
          "0": { "function": "on_create_script_name" }
      },
      "on_destroy": {
          "0": { "function": "on_destroy_script_name" }
      },
      "on_enter": {
          "0": { "function": "on_enter_script_name" }
      },
      "on_exit": {
          "0": { "function": "on_exit_script_name" }
      },
      "on_render": {
          "0": { "function": "on_render_script_name" }
      }
  }
  ```
  *Note*: The key `"0"` denotes the priority/index of the script, allowing multiple scripts to attach to the same event.
