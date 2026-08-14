scenes = {}

function scenes.create_entity(entity_path) end
function scenes.create_entity(entity_path) end
function scenes.rename_entity(old_entity_path, new_entity_name) end
function scenes.duplicate_entity(entity_path) end
function scenes.copy_entity(entity_path) end
function scenes.paste_entity(entity_path) end
function scenes.move_entity(old_entity_path, new_entity_path) end
function scenes.list_children(entity_path) end
function scenes.have_child(child_path) end
function scenes.have_child_overload(child_path) end

function scenes.add_component(component_path) end
function scenes.remove_component(component_path) end
function scenes.rename_component(old_component_path, new_component_name) end
function scenes.copy_component(component_path) end
function scenes.paste_component(component_path) end
function scenes.list_components(component_path) end
function scenes.have_component(component_path) end
function scenes.have_component_overload(component_path) end
function scenes.get_component_variabe(component_path) end
function scenes.set_component_variabe(component_path, value) end

function scenes.add_prefab(entity_path, prefab_name) end
function scenes.remove_prefab(entity_path, prefab_name) end
function scenes.list_prefabs(entity_path) end
function scenes.have_prefab(entity_path, prefab_name) end
function scenes.set_prefab_order(entity_path, prefab_name, position) end
