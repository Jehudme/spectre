prefabs = {}

function prefabs.create_entity(entity_path) end
function prefabs.delete_entity(entity_path) end
function prefabs.rename_entity(old_entity_path, new_entity_name) end
function prefabs.duplicate_entity(entity_path) end
function prefabs.copy_entity(entity_path) end
function prefabs.paste_entity(entity_path) end
function prefabs.move_entity(old_entity_path, new_entity_path) end
function prefabs.list_children(entity_path) end
function prefabs.have_child(child_path) end
function prefabs.have_child_overload(child_path) end
function prefabs.find_entity(entity_path) end

function prefabs.add_component(component_path) end
function prefabs.remove_component(component_path) end
function prefabs.rename_component(old_component_path, new_component_name) end
function prefabs.copy_component(component_path) end
function prefabs.paste_component(component_path) end
function prefabs.list_components(component_path) end
function prefabs.have_component(component_path) end
function prefabs.have_component_overload(component_path) end
function prefabs.get_component_variabe(component_path) end
function prefabs.set_component_variabe(component_path, value) end

function prefabs.add_prefab(entity_path, prefab_name) end
function prefabs.remove_prefab(entity_path, prefab_name) end
function prefabs.list_prefabs(entity_path) end
function prefabs.have_prefab(entity_path, prefab_name) end
function prefabs.set_prefab_order(entity_path, prefab_name, position) end
