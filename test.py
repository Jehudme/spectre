import re
valid = re.compile(r'^[a-zA-Z_][a-zA-Z0-9_]*$')
print(valid.match('value)'))
print(valid.match('_Bool'))
