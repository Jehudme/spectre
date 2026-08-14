local sandbox = require("sandbox")
local ecs = require("ecs")

local search = {}

-- Score parameters similar to fzy
local SCORE_MIN = -999999
local SCORE_MAX = 999999
local SCORE_GAP_LEADING = -0.005
local SCORE_GAP_TRAILING = -0.005
local SCORE_GAP_INNER = -0.01
local SCORE_MATCH_CONSECUTIVE = 1.0
local SCORE_MATCH_BOUNDARY = 0.8
local SCORE_MATCH_CAPITAL = 0.7
local SCORE_MATCH_NORMAL = 0.5

-- Helper to check if string matches fuzzy pattern (order of characters)
function search.is_match(str, query)
	if query == "" then return true end
	local query_len = #query
	local str_len = #str
	if query_len > str_len then return false end
	
	local q_idx = 1
	local query_lower = query:lower()
	local str_lower = str:lower()
	
	for i = 1, str_len do
		if str_lower:sub(i, i) == query_lower:sub(q_idx, q_idx) then
			q_idx = q_idx + 1
			if q_idx > query_len then
				return true
			end
		end
	end
	return false
end

-- Fuzzy score calculation (fzy algorithm DP)
function search.score(str, query)
	if query == "" then return 0.0 end
	if not search.is_match(str, query) then return SCORE_MIN end
	
	local query_len = #query
	local str_len = #str
	if query_len == str_len then return SCORE_MAX end
	
	local query_lower = query:lower()
	local str_lower = str:lower()
	
	local D = {}
	local M = {}
	for i = 1, query_len do
		D[i] = {}
		M[i] = {}
	end
	
	local bonus = {}
	local last_char = "/"
	for j = 1, str_len do
		local c = str:sub(j, j)
		local b = 0.0
		if last_char == "/" or last_char == "\\" or last_char == " " or last_char == "_" or last_char == "-" or last_char == "." then
			b = SCORE_MATCH_BOUNDARY
		elseif last_char:match("%a") == nil then
			b = SCORE_MATCH_BOUNDARY
		elseif c:match("%u") and last_char:match("%l") then
			b = SCORE_MATCH_CAPITAL
		end
		bonus[j] = b
		last_char = c
	end
	
	for i = 1, query_len do
		local q_c = query_lower:sub(i, i)
		local prev_m = M[i-1]
		local prev_d = D[i-1]
		
		for j = 1, str_len do
			local s_c = str_lower:sub(j, j)
			local score = SCORE_MIN
			
			if q_c == s_c then
				score = SCORE_MATCH_NORMAL + bonus[j]
				if i > 1 then
					local prev_match = prev_d and prev_d[j-1] or SCORE_MIN
					local prev_any = prev_m and prev_m[j-1] or SCORE_MIN
					
					local consecutive = prev_match + SCORE_MATCH_CONSECUTIVE
					local non_consecutive = prev_any + SCORE_MATCH_NORMAL + bonus[j]
					if consecutive > non_consecutive then
						score = consecutive
					else
						score = non_consecutive
					end
				else
					score = score + (j - 1) * SCORE_GAP_LEADING
				end
			end
			
			D[i][j] = score
			
			local score_any = SCORE_MIN
			if j > 1 then
				score_any = M[i][j-1] + SCORE_GAP_INNER
			end
			if score > score_any then
				score_any = score
			end
			M[i][j] = score_any
		end
	end
	
	local best_score = SCORE_MIN
	for j = 1, str_len do
		local d_score = D[query_len][j]
		if d_score and d_score > SCORE_MIN then
			local final_score = d_score + (str_len - j) * SCORE_GAP_TRAILING
			if final_score > best_score then
				best_score = final_score
			end
		end
	end
	return best_score
end

---Filter and sort a list of items using fuzzy search and a specified sort mode.
---@param items table Array of items to search.
---@param query string The search query.
---@param options table? Extra options:
---  - key_getter: function(item) -> string (extracts search string)
---  - sort_mode: string ("score", "alphabetical", "type")
---  - type_getter: function(item) -> string
---  - custom_group_order: table
---@return table Array of filtered and sorted items.
function search.filter(items, query, options)
	options = options or {}
	local key_getter = options.key_getter or function(item)
		if type(item) == "table" then
			return item.name or tostring(item)
		end
		return tostring(item)
	end
	local sort_mode = options.sort_mode or "score"
	local type_getter = options.type_getter
	local custom_group_order = options.custom_group_order

	-- 1. Filter items and calculate scores
	local matched = {}
	for _, item in ipairs(items) do
		local key = key_getter(item)
		if query == "" or search.is_match(key, query) then
			local score = (query == "") and 0.0 or search.score(key, query)
			table.insert(matched, {
				item = item,
				key = key,
				score = score,
			})
		end
	end

	-- Helper to determine group priority
	local group_priority = {}
	if custom_group_order then
		for idx, grp in ipairs(custom_group_order) do
			group_priority[grp] = idx
		end
	end

	local function get_group_priority(item_wrapper)
		if not type_getter then return 0 end
		local t = type_getter(item_wrapper.item)
		if not t then return 999999 end
		local p = group_priority[t]
		if p then return p end
		return 999999
	end

	-- 2. Sort according to mode
	table.sort(matched, function(a, b)
		if sort_mode == "type" then
			local p_a = get_group_priority(a)
			local p_b = get_group_priority(b)
			if p_a ~= p_b then
				return p_a < p_b
			end
		end

		if sort_mode == "alphabetical" then
			return a.key:lower() < b.key:lower()
		else
			-- Default: score (highest first)
			if a.score ~= b.score then
				return a.score > b.score
			end
			return a.key:lower() < b.key:lower()
		end
	end)

	-- 3. Extract original items
	local result = {}
	for _, wrapper in ipairs(matched) do
		table.insert(result, wrapper.item)
	end
	return result
end

function search.run_test()
	local world = ecs.from_ptr(g_world)
	sandbox.logs.info(world, "[Search Test] ===== STARTING ALL TESTS =====")

	-- Test 1: is_match
	if search.is_match("my_prefab.json", "mpb") and not search.is_match("my_prefab.json", "xyz") then
		sandbox.logs.info(world, "[Search Test] Passed is_match basic tests")
	else
		sandbox.logs.error(world, "[Search Test] Failed is_match basic tests")
	end

	-- Test 2: score ordering
	local score1 = search.score("my_prefab.json", "prefab")
	local score2 = search.score("other_file.json", "prefab")
	if score1 > score2 then
		sandbox.logs.info(world, "[Search Test] Passed score alignment check")
	else
		sandbox.logs.error(world, "[Search Test] Failed score alignment check")
	end

	-- Test 3: filter and sorting
	local test_items = {
		{ name = "my_prefab.json", type = "file" },
		{ name = "scenes", type = "directory" },
		{ name = "prefabs", type = "directory" },
		{ name = "test.lua", type = "file" },
	}

	local res = search.filter(test_items, "pre", {
		key_getter = function(item) return item.name end,
		sort_mode = "type",
		type_getter = function(item) return item.type end,
		custom_group_order = { "directory", "file" }
	})

	if #res == 2 and res[1].name == "prefabs" and res[2].name == "my_prefab.json" then
		sandbox.logs.info(world, "[Search Test] Passed type-priority group filter and sorting")
	else
		sandbox.logs.error(world, "[Search Test] Failed type-priority group filter and sorting")
	end

	sandbox.logs.info(world, "[Search Test] ===== ALL TESTS FINISHED =====")
	return true
end

return search
