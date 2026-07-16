function dump_api()
    for k, v in pairs(spectre_api) do
        print("spectre_api." .. tostring(k) .. " = " .. type(v))
    end
end
dump_api()
