# Resource Configurations

This document outlines the configuration properties available for various resource types in the Spectre engine.

## `shader`
- **Description**: Loads a shader from a source file (e.g., fragment shader).
- **Properties**:
  - *(No specific configuration properties at this time)*

## `texture`
- **Description**: Loads a 2D texture (e.g., `.png`, `.jpg`).
- **Properties**:
  - `filtering` (String): The texture filtering mode. Options:
    - `"point"`
    - `"bilinear"`
    - `"trilinear"`
  - `wrap_mode` (String): The texture wrap mode. Options:
    - `"repeat"`
    - `"clamp"`
    - `"mirror_repeat"`
    - `"mirror_clamp"`

## `font`
- **Description**: Loads a font (e.g., `.ttf`).
- **Properties**:
  - `font_size` (Integer): The font size. Default is 32.
  - `filtering` (String): The texture filtering mode applied to the font atlas. Options:
    - `"point"`
    - `"bilinear"`
    - `"trilinear"`
