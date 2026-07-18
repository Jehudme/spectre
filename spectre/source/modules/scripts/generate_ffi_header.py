import os
import subprocess
import sys

def main():
    if len(sys.argv) < 4:
        print("Usage: python3 generate_ffi_header.py <input_file> <output_file> <include_dirs...>")
        sys.exit(1)
        
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    include_dirs = sys.argv[3:]
    
    cmd = ["gcc", "-E", "-xc"]
    for inc in include_dirs:
        cmd.append(f"-I{inc}")
    cmd.append(input_file)

    print("Running GCC:", " ".join(cmd))
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, universal_newlines=True)
    except subprocess.CalledProcessError as e:
        print("GCC Error:", e.output)
        sys.exit(1)

    lines = output.split('\n')
    filtered_lines = []
    include_this = True
    
    brace_depth = 0
    extern_c_depths = set()
    
    for line in lines:
        if line.startswith('#'):
            parts = line.split()
            if len(parts) >= 3 and parts[0] == '#':
                filename = parts[2].strip('"')
                if "/usr/" in filename or "<built-in>" in filename or "<command-line>" in filename or "c++" in filename:
                    include_this = False
                else:
                    include_this = True
            continue
        
        if not include_this or not line.strip():
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
            
        # Count braces in the cleaned line to update depth
        open_braces = cleaned.count('{')
        close_braces = cleaned.count('}')
        
        # If this line is JUST a closing brace (or multiple), and it drops us out of an extern "C" depth
        if cleaned == "}" and (brace_depth - 1) in extern_c_depths:
            extern_c_depths.remove(brace_depth - 1)
            brace_depth -= 1
            continue
            
        brace_depth += open_braces
        brace_depth -= close_braces
        
        if brace_depth < 0:
            brace_depth = 0
            
        filtered_lines.append(cleaned)

    cdef_content = "\n".join(filtered_lines)
    
    # ensure output directory exists
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    
    with open(output_file, "w") as f:
        f.write("typedef int8_t i8;\ntypedef int16_t i16;\ntypedef int32_t i32;\ntypedef int64_t i64;\n")
        f.write("typedef uint8_t u8;\ntypedef uint16_t u16;\ntypedef uint32_t u32;\ntypedef uint64_t u64;\n")
        f.write("typedef float f32;\ntypedef double f64;\n")
        f.write(cdef_content)
        f.write('\n')
        
    print(f"Generated FFI header at {output_file}")

if __name__ == "__main__":
    main()
