--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 100.0;
kMatchTitle = "竞彩足球 合买";
kMatchDesc = "混合过滤";
kMatchBets = {
    "20180320001;-1;1,1,3.33;2,2,3.00;2,4,6.30",         --      罗奇代尔   VS   福利特伍德   
    "20180320002;1;1,1,3.35;2,2,3.00;2,4,6.50",         --      北安普敦   VS   什鲁斯伯里   
    "20180320003;-1;1,1,3.25;2,2,3.20;2,4,5.35",         --         圣保罗   VS   圣卡埃塔诺   
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
	ok_count = GetMatchResultCount(1, 1, codes);
	ret.info = trace_prefix .. "ok_count_1=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 2) then
		ret.code = 1;
		return ret;
	end
	
	ok_count = 0;
	ok_count = GetMatchResultCount(2, 2, codes); --2 qiu
	ret.info = trace_prefix .. "ok_count_2=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 2) then
		ret.code = 1;
		return ret;
	end
	
	ok_count = 0;
	ok_count = GetMatchResultCount(2, 4, codes); --4 qiu
	ret.info = trace_prefix .. "ok_count_3=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 2) then
		ret.code = 1;
		return ret;
	end
	
--[[

--]]	

	return ret;
end

