--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 517.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "过滤";
kMatchBets = {
    "20180415038;-2;3,1,4.60;3,2,4.30;3,4,5.80",         --   马德里竞技   VS   莱万特         
    "20180415040;-2;3,1,6.75;3,2,5.25;3,4,6.25",         --   曼彻斯特联   VS   西布罗姆维奇
    "20180415046;-2;3,1,7.00;3,2,5.50;3,4,6.10",         --      尤文图斯   VS   桑普多利亚   
    "20180415064;-1;1,3,1.62;2,1,8.00;3,3,7.30",         --巴黎圣日尔曼   VS   摩纳哥         
};



kMatchBetsLose = 0;
--[[
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

function GetCodeCount(codes, tid, code)
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
	local ok_count0 = 0;
	local ok_count1 = 0;
	
	local trace_prefix = "jc_dbg[";
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	
	ok_count0 = 0;
	ok_count0 = GetCodeCount(codes, 3, 1);
	ok_count1 = 0;
	ok_count1 = GetCodeCount(codes, 2, 1);
	ret.info = trace_prefix .. "ok_count0 + ok_count1=" .. (ok_count0 + ok_count1);
	trace(1, ret.info);
	if(ok_count0 + ok_count1 > 2) then
		ret.code = 1;
		return ret;
	end
	
	ok_count0 = 0;
	ok_count0 = GetCodeCount(codes, 3, 2);
	if(ok_count0 > 2) then
		ret.info = trace_prefix .. "ok_count0=" .. (ok_count0);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end
	
	ok_count0 = 0;
	ok_count0 = GetCodeCount(codes, 3, 3);
	if(ok_count0 > 2) then
		ret.info = trace_prefix .. "ok_count0=" .. (ok_count0);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end	
	
--[[

--]]	

	return ret;
end

