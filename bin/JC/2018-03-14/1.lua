--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 180.0;
kMatchTitle = "竞彩足球 合买";
kMatchDesc = "混合过滤";
kMatchBets = {
    "20180314015;-1;1,3,2.17;3,1,5.00;3,3,7.25",         --      尤文图斯   VS   亚特兰大      
    "20180314016;-1;1,3,2.09;3,1,7.00;3,3,7.00",         --      巴塞罗那   VS   切尔西         
    "20180314018;-1;1,3,2.15;3,1,6.50;3,3,7.00",         --      玻利瓦尔   VS   科洛科洛      
    "20180314026;-1;1,3,2.78;3,1,5.40;3,3,7.00",         --   瓜达拉哈拉   VS   西雅图海湾人
};

--[[
kMatchBetsLose = 0;
kMatchBetsFixed = {
    "20180307024;-1;6,3,1.47;6,1,3.80",         --蒙特雷   VS   克雷塔罗
    "20180307025;-1;6,3,2.08;6,1,3.00",         --西雅图海湾人   VS   瓜达拉哈拉
    "20180307026;-1;6,3,1.95;6,1,3.30",         --美洲狮   VS   普埃布拉大学
};
--]]

function GetMatchIdsCount(ids, codes)
	local found_count = 0;
	for i = 1, #ids do
		for j = 1, #codes do
			if(ids[i] == codes[j].id) then
				found_count = found_count + 1;
				break;
			end
		end
	end
	return found_count;
end 

function GetMatchResultCount(tid, code, codes)
	local found_count = 0;
	for j = 1, #codes do
		if(tid == codes[j].tid and code == codes[j].code) then
			found_count = found_count + 1;
		end
	end
	return found_count;
end 

function IsFilterLua(params)
	local errorCount = 0;
	local ret = {code=0, info="ok", bonus=params.betbouns};
	local codes = params.betcodes;
	local bonus = params.betbouns;
	local tj = "";
	local ok_count = 0;
	
	local trace_prefix = "jc_dbg[";
	for i = 1, #codes do
		trace_prefix = trace_prefix .. codes[i].tid .. "-" .. codes[i].code;-- .. ";"  
		if(i == #codes) then
			trace_prefix = trace_prefix .. "]"
		else
			trace_prefix = trace_prefix .. ";"
		end

	end
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	ok_count = 0;
	ok_count = GetMatchResultCount(1, 3, codes);
	ret.info = trace_prefix .. "ok_count_1=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 3) then
		ret.code = 1;
		return ret;
	end
	
	ok_count = 0;
	ok_count = GetMatchResultCount(3, 1, codes); --1:0
	ret.info = trace_prefix .. "ok_count_2=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 2) then
		ret.code = 1;
		return ret;
	end
	
	ok_count = 0;
	ok_count = GetMatchResultCount(3, 3, codes); --2:1
	ret.info = trace_prefix .. "ok_count_3=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 3) then
		ret.code = 1;
		return ret;
	end
	
--[[

--]]	

	return ret;
end

