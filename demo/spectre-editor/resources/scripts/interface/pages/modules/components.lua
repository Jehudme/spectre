components = {}

function components.create(component_name) end
function components.register(component_name, component_shema) end
function components.delete(component_name) end
function components.rename(old_name, new_name) end
function components.duplicate(component_name) end
function components.list() end
function components.find(component_name) end

function components.create_variable(component_name, variable_name) end
function components.delete_variable(component_name, variable_name) end
function components.rename_variable(component_name, old_variable_name, new_variable_name) end
function components.duplicate_variable(component_name, variable_name) end
function components.list_variables(component_name) end
function components.set_variable_type(component_name, variable_name, value) end
function components.get_variable_type(component_name, variable_name) end
function components.set_variable_value(component_name, variable_name, value) end
function components.get_variable_value(component_name, variable_name) end
