import re

path = 'demo/resources/asteroid/resources/scripts/TestScript.lua'
with open(path, 'r') as f:
    content = f.read()

new_content = content.replace(
    'local PlayerStats = spectre.components.find_component(world, "PlayerStats")',
    'local PlayerStats = spectre.components.find_component(world, "PlayerStats")\n  sandbox.logs.info(world, "[TestScript] PlayerStats ID: " .. tostring(PlayerStats))'
)
new_content = new_content.replace(
    'local PlayerStatsSerializer = spectre.serializer.find_serializer(world, "PlayerStats")',
    'local PlayerStatsSerializer = spectre.serializer.find_serializer(world, "PlayerStats")\n      sandbox.logs.info(world, "[TestScript] PlayerStatsSerializer ID: " .. tostring(PlayerStatsSerializer))'
)

with open(path, 'w') as f:
    f.write(new_content)
