import re

path = 'demo/resources/asteroid/resources/scripts/TestScript.lua'
with open(path, 'r') as f:
    content = f.read()

content = content.replace("props:set_integer(", "props:set_int64(")
content = content.replace("props:get_integer(", "props:get_int64(")
content = content.replace("props:set_boolean(", "props:set_bool(")
content = content.replace("props:get_boolean(", "props:get_bool(")

with open(path, 'w') as f:
    f.write(content)
