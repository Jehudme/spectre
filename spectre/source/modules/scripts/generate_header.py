import os
import subprocess
import sys
import tempfile

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 generate_header.py <output_dir> <include_dirs...>")
        sys.exit(1)
        
    output_dir = sys.argv[1]
    include_dirs = sys.argv[2:]
    
    # Create a temporary C file that includes all necessary headers
    fd, temp_path = tempfile.mkstemp(suffix=".h")
    with os.fdopen(fd, 'w') as f:
        f.write("#include <sandbox/sandbox.h>\n")
        f.write("#include <spectre/spectre.h>\n")
        f.write("#include <flecs.h>\n")
        
    cmd = ["gcc", "-E", "-xc", "-w", "-DSANDBOX_FFI_GENERATION"]
    for inc in include_dirs:
        cmd.append(f"-I{inc}")
    cmd.append(temp_path)

    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, universal_newlines=True)
    except subprocess.CalledProcessError as e:
        print("GCC Error:", e.output)
        os.remove(temp_path)
        sys.exit(1)
        
    os.remove(temp_path)

    lines = output.split('\n')
    
    categories = {
        "flecs": [],
        "sandbox": [],
        "spectre": []
    }
    
    current_category = None
    
    brace_depth = 0
    extern_c_depths = set()
    
    for line in lines:
        if line.startswith('#'):
            parts = line.split()
            if len(parts) >= 3 and parts[0] == '#':
                filename = parts[2].strip('"')
                if "/usr/" in filename or "<built-in>" in filename or "<command-line>" in filename or "c++" in filename:
                    current_category = None
                elif "flecs" in filename.lower():
                    current_category = "flecs"
                elif "sandbox" in filename.lower():
                    current_category = "sandbox"
                elif "spectre" in filename.lower():
                    current_category = "spectre"
                else:
                    current_category = None
            continue
        
        if current_category is None or not line.strip():
            continue
            
        cleaned = line.strip()
        
        if "SANDBOX_API" in cleaned:
            cleaned = cleaned.replace("SANDBOX_API ", "")
            
        if "__null" in cleaned:
            cleaned = cleaned.replace("__null", "0")
            
        if (cleaned.startswith("template") or 
            "namespace " in cleaned or 
            "class " in cleaned or
            "public:" in cleaned or
            "private:" in cleaned or
            "protected:" in cleaned or
            " const;" in cleaned or
            "&" in cleaned or
            "~" in cleaned or
            "::" in cleaned or
            "operator " in cleaned):
            cleaned = "// " + cleaned
            
        if "extern \"C\"" in cleaned:
            if "{" in cleaned:
                extern_c_depths.add(brace_depth)
                brace_depth += 1
            continue
            
        open_braces = cleaned.count('{')
        close_braces = cleaned.count('}')
        
        if cleaned == "}" and (brace_depth - 1) in extern_c_depths:
            extern_c_depths.remove(brace_depth - 1)
            brace_depth -= 1
            continue
            
        brace_depth += open_braces
        brace_depth -= close_braces
        
        if brace_depth < 0:
            brace_depth = 0
            
        categories[current_category].append(cleaned)

    os.makedirs(output_dir, exist_ok=True)
    
    typedefs = "typedef int8_t i8;\ntypedef int16_t i16;\ntypedef int32_t i32;\ntypedef int64_t i64;\n"
    typedefs += "typedef uint8_t u8;\ntypedef uint16_t u16;\ntypedef uint32_t u32;\ntypedef uint64_t u64;\n"
    typedefs += "typedef float f32;\ntypedef double f64;\n"
    typedefs += "typedef struct ecs_world_t ecs_world_t;\ntypedef uint64_t ecs_entity_t;\ntypedef struct ecs_query_t ecs_query_t;\n"
    typedefs += "typedef struct sandbox_properties_t sandbox_properties_t;\ntypedef sandbox_properties_t* sandbox_properties_handle_t;\n"
    typedefs += "typedef struct sandbox_service_info_t sandbox_service_info_t;\n\n"
    
    for cat in ["flecs", "sandbox", "spectre"]:
        with open(os.path.join(output_dir, f"{cat}.h"), "w") as f:
            f.write(typedefs)
            f.write("\n".join(categories[cat]))
            f.write('\n')
            
    print(f"Generated headers at {output_dir}")

if __name__ == "__main__":
    main()
