--17194_26, kMaxLose=0, kCalcRen9=1                                       
kMaxLose = 0;
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info);
	end
end

local other_odds = {
	"W&luosen&8&12&10&31&10&31&31&31&31&31&31&10&30&31&31&31",
	--"W&laoniu1024&6&10&10&3&31&31&01&01&3&01&31&3&03&3&30&31",
	--"W&danlue&9&13&310&3&31&31&01&130&31&30&130&310&0&31&01&31",
};

function IsFilterLua(params)
	local ret = {code=0, info="ok"};
	local trace_prefix = params.codes .. ", filtered: ";
	local tj = "";
	local gvj = params.gvj * math.pow(10, 6);
	local cpos = {};
	for i = 1, #params.codes do
		cpos[i] = string.sub(params.codes, i , i);
		cpos[i] = tonumber(cpos[i]);
	end
	local plavg = params.plavg;
	local plscope = params.plscope;
	local plavgpos = params.plavgpos;
	--trace(1, "plscope:" .. plscope[1] .. "," .. plscope[2].. "," .. plscope[3].. "," .. plscope[4]);
	--trace(1, "plavg:" .. plavg[1] .. "," .. plavg[2].. "," .. plavg[3].. "," .. plavg[4].. "," .. plavg[5].. "," .. plavg[6]);
------------------------------------------------------------------------------------	
	if params.count3 < 4 or params.count3 > 9 then
		ret.code = 1;
		ret.info = trace_prefix .. "count3=" .. params.count3;
		trace(1, ret.info);
		return ret;
	end

	if params.count1 < 1 or params.count1 > 6 then
		ret.code = 1;
		ret.info = trace_prefix .. "count1=" .. params.count1;
		trace(1, ret.info);
		return ret;
	end

	if params.count0 < 2 or params.count0 > 7 then
		ret.code = 1;
		ret.info = trace_prefix .. "count0=" .. params.count0;
		trace(1, ret.info);
		return ret;
	end

	if params.lian3 > 5 then
		ret.code = 1;
		ret.info = trace_prefix .. "lian3=" .. params.lian3;
		trace(1, ret.info);
		return ret;
	end
	
	if params.lian1 > 4 then
		ret.code = 1;
		ret.info = trace_prefix .. "lian1=" .. params.lian3;
		trace(1, ret.info);
		return ret;
	end
	
	if params.breakcount < 5 or params.breakcount > 12 then
		ret.code = 1;
		ret.info = trace_prefix .. "breakcount=" .. params.breakcount;
		trace(1, ret.info);
		return ret;
	end
	
	--
	tj = "F|123456789ABCDE|13113131030011|3|9|";
	ret.code = IsFilterTJ(params.codes, tj);
	if(ret.code == 1) then
		ret.info = trace_prefix .. tj;
		trace(1, ret.info);
		return ret;
	end
	--
-----------------------------------------------------------------------------------------------------
	if(params.pl1 < 5) or (params.pl1 > 9) then
		ret.code = 1;
		ret.info = trace_prefix .. "pl1=" .. params.pl1;
		trace(1, ret.info);
		return ret;
	end
	
	if(params.pl2 < 2) or (params.pl2 > 7) then
		ret.code = 1;
		ret.info = trace_prefix .. "pl2=" .. params.pl2;
		trace(1, ret.info);
		return ret;
	end
	
	if(params.pl3 < 1) or (params.pl3 > 6) then
		ret.code = 1;
		ret.info = trace_prefix .. "pl3=" .. params.pl3;
		trace(1, ret.info);
		return ret;
	end

	local gvjMin = 0.15;
	local gvjMax = 5.00;
	local hotSum = plavg[1] + plavg[2] + plavg[3];
	local coldSum = plavg[4] + plavg[5] + plavg[6];
	local neverTrue = false;
--[[
	local plb635 = (params.pl1 == 6 and params.pl2 == 3 and params.pl3 == 5);
	local plb644 = (params.pl1 == 6 and params.pl2 == 4 and params.pl3 == 4);
	local plb653 = (params.pl1 == 6 and params.pl2 == 5 and params.pl3 == 3);
	local plb662 = (params.pl1 == 6 and params.pl2 == 6 and params.pl3 == 2);
	local plb671 = (params.pl1 == 6 and params.pl2 == 7 and params.pl3 == 1);

	local plb725 = (params.pl1 == 7 and params.pl2 == 2 and params.pl3 == 5);
	local plb734 = (params.pl1 == 7 and params.pl2 == 3 and params.pl3 == 4);
	local plb743 = (params.pl1 == 7 and params.pl2 == 4 and params.pl3 == 3);
	local plb752 = (params.pl1 == 7 and params.pl2 == 5 and params.pl3 == 2);
	local plb761 = (params.pl1 == 7 and params.pl2 == 6 and params.pl3 == 1);
	
	local plb824 = (params.pl1 == 8 and params.pl2 == 2 and params.pl3 == 4);
	local plb833 = (params.pl1 == 8 and params.pl2 == 3 and params.pl3 == 3);
	local plb842 = (params.pl1 == 8 and params.pl2 == 4 and params.pl3 == 2);
	local plb851 = (params.pl1 == 8 and params.pl2 == 5 and params.pl3 == 1);
	
	local plb923 = (params.pl1 == 9 and params.pl2 == 2 and params.pl3 == 3);
	local plb932 = (params.pl1 == 9 and params.pl2 == 3 and params.pl3 == 2);
	local plb941 = (params.pl1 == 9 and params.pl2 == 4 and params.pl3 == 1);

	local plbA32 = (params.pl1 == 10 and params.pl2 == 2 and params.pl3 == 2);

	if (not (plb932 or plb941 or plb833 or plb842 or plb743 or plb752 or plb662)) and neverTrue then
		ret.code = 1;
		ret.info = trace_prefix .. "plb=(" .. params.pl1 .. ")";
		trace(1, ret.info);
		return ret;
	end
--]]	
	if (plavg[1] < 5 or plavg[1] > 7) then
		ret.code = 1;
		ret.info = trace_prefix .. "plavg[1]=(" .. plavg[1] .. ")";
		trace(1, ret.info);
		return ret;
	end

	if (plavg[3] < 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "lavg[3]=(" .. plavg[3] .. ")";
		trace(1, ret.info);
		return ret;
	end

	if (plavg[6] > 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "plavg[6]=(" .. plavg[6] .. ")";
		trace(1, ret.info);
		return ret;
	end
	
	if(hotSum < 8) then
		ret.code = 1;
		ret.info = trace_prefix .. "hotSum=(" .. hotSum .. ")";
		trace(1, ret.info);
		return ret;
	end
	
	
	
	for i,v in ipairs(other_odds) do
		tj = v;
		ret.code = IsFilterTJ(params.codes, tj);
		if(ret.code == 1) then
			ret.info = trace_prefix .. tj;
			trace(1, ret.info);
			return ret;
		end
	end
	
	--[[
	if (plavgpos[12] ~= 6) then
		ret.code = 1;
		ret.info = trace_prefix .. "plavgpos[12]=(" .. plavgpos[12] .. ")";
		trace(1, ret.info);
		return ret;
	end
	
	if (not (plavg[1] == 5 and plavg[2] == 2 and plavg[3] == 2 and
		plavg[4] == 2 and plavg[5] == 2 and plavg[6] == 1)) then
		ret.code = 1;
		ret.info = trace_prefix .. "plavg";
		trace(1, ret.info);
		return ret;
	end
	
	tj = "F|2E|03|1|2|";
	ret.code = IsFilterTJ(params.codes, tj);
	if(ret.code == 1) then
		ret.info = trace_prefix .. tj;
		trace(1, ret.info);
		return ret;
	end
	--]]
	if(gvj < gvjMin or gvj > gvjMax) then
		ret.code = 1;
		ret.info = trace_prefix .. "gvj=(" .. tostring(gvj) .. ") [" .. gvjMin .. " , " .. gvjMax .. "]";
		trace(1, ret.info);
		return ret;
	end
	
	return ret;
end

------------------------------------------------------------------------------------------------------------------------
kCalcRen9 = 0;
kRen9Pos = "12345678A" .. ",2479ABCDE";



function IsFilterLua9(params)
	local ret = {code=0, info="ok"};
	local trace_prefix = params.codes .. ", filtered: ";
	local tj = "";
	local cpos = {};
	for i = 1, #params.codes do
		cpos[i] = string.sub(params.codes, i , i);
		cpos[i] = tonumber(cpos[i]);
	end
	local plavg = params.plavg;
	local plavgpos = params.plavgpos;
	if(cpos[2] == 8 or cpos[4] == 8 or cpos[7] == 8 or cpos[10] == 8) then
		ret.code=1;
		trace(1, params.codes .. " p1,p5,p6");
	end
	
	if(cpos[1] ~= 8 and cpos[11] ~= 8) then
		ret.code=1;
		trace(1, params.codes .. " group 1");
	end
	
	return ret;
end

