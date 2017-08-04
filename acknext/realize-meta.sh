#!/usr/bin/env lua

function __genOrderedIndex( t )
    local orderedIndex = {}
    for key in pairs(t) do
        table.insert( orderedIndex, key )
    end
    table.sort( orderedIndex )
    return orderedIndex
end

function orderedNext(t, state)
    -- Equivalent of the next function, but returns the keys in the alphabetic
    -- order. We use a temporary ordered key table that is stored in the
    -- table being iterated.

    local key = nil
    --print("orderedNext: state = "..tostring(state) )
    if state == nil then
        -- the first time, generate the index
        t.__orderedIndex = __genOrderedIndex( t )
        key = t.__orderedIndex[1]
    else
        -- fetch the next value
        for i = 1,#t.__orderedIndex do
            if t.__orderedIndex[i] == state then
                key = t.__orderedIndex[i+1]
            end
        end
    end

    if key then
        return key, t[key]
    end

    -- no more value to return, cleanup
    t.__orderedIndex = nil
    return
end

function orderedPairs(t)
    -- Equivalent of the pairs() function on tables. Allows to iterate
    -- in order
    return orderedNext, t, nil
end

--------------------------------------------------------

flags = { }
enum  = { }

dofile("meta/flags.lua")
dofile("meta/enums.lua")

-- Adjust flag names
for i,v in pairs(flags) do
	for i=1,#v do
		v[i] = v[i]:upper()
	end
end

f = io.open("include/acknext/ackdef.h", "w")
f:write [[#ifndef _ACKNEXT_ACKDEF_H_
#define _ACKNEXT_ACKDEF_H_

]]

function Range(min, max)
	local r = { }
	for i=min,max do
		r[i] = true
	end
	return r
end

function FirstKey(t,min,max)
	for i=min,max do
		if t[i] then
			return i
		end
	end
	return nil
end

do
	allocated = { }
	for i,v in orderedPairs(flags) do
		local values = Range(0, 63)
		for i,sym in ipairs(v) do
			if allocated[sym] then
				-- Remove allocation
				values[allocated[sym]] = nil
			end
		end
		for i,sym in ipairs(v) do
			if not allocated[sym] then
				local key = FirstKey(values, 0, 63)
				f:write(
					"#define ",
					sym,
					" (1<<",
					key,
					")\n"
				)
				
				-- Remove allocation
				values[key] = nil
				allocated[sym] = key
			end
		end
	end
end

f:write("\n")
fi = io.open("meta/defines.h", "r")
f:write(fi:read("*all"))
f:write("\n")
fi:close()

f:write [[

#endif // _ACKNEXT_ACKDEF_H_
]]
f:close()

f = io.open("include/acknext/ackenum.h", "w")
f:write [[#ifndef _ACKNEXT_ACKENUM_H_
#define _ACKNEXT_ACKENUM_H_

]]

for name,list in orderedPairs(enum) do
	print(name, #list)
	f:write("typedef enum ", name, " {\n")
	for i,v in ipairs(list) do
		f:write("\t")
		if list.prefix then
			f:write(list.prefix)
		end
		f:write(v:upper())
		if list.postfix then
			f:write(list.postfix)
		end
		f:write(",\n")
	end
	f:write("} ", name, ";\n");
	f:write("\n")
end

f:write [[

#endif // _ACKNEXT_ACKDEF_H_
]]
f:close()