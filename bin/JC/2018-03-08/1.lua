--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 0.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "过滤";
kMatchBets = {
    "20180308001;-1;6,3,2.02",         --AC米兰   VS   阿森纳
    "20180308002;-2;6,3,1.15",         --马德里竞技   VS   莫斯科火车头
    "20180308003;-1;6,3,1.40",         --多特蒙德   VS   萨尔茨堡
    "20180308004;-1;6,3,2.45",         --莫斯科中央陆军   VS   里昂
    "20180308005;-1;6,3,1.72",         --马赛   VS   毕尔巴鄂竞技
    "20180308007;-1;6,3,1.38",         --里斯本竞技   VS   比尔森
    "20180308008;-1;6,3,1.35",         --拉齐奥   VS   基辅迪纳摩
    "20180308012;-1;6,3,2.02",         --卡利美洲   VS   防御与正义
};

kMatchBetsLose = 2;
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
		if(tid == codes[j].id and code == codes[j].code) then
			found_count = found_count + 1;
			break;
		end
	end
	return found_count;
end 


function IsFilterLua(params)
	local errorCount = 0;
	local ret = {code=0, info="ok", bonus=params.betbouns};
	local codes = params.betcodes;
	local bonus = params.betbouns;
	local upper_pan = 0;
	local lower_pan = 0;
	local pan1 = 0;
	local pan2 = 0;
	local pan3 = 0;
	local pan4 = 0;
	local tj = "";
	local odds_4 = 0;
	local odds_5 = 0;
	local ok_count = 0;
	
	local trace_prefix = "jc_dbg[";
	for i = 1, #codes do
		trace_prefix = trace_prefix .. codes[i].tid .. "-" .. codes[i].code;-- .. ";"  
		if(i == #codes) then
			trace_prefix = trace_prefix .. "]"
		else
			trace_prefix = trace_prefix .. ";"
		end
		if(codes[i].pan == -1) then
			upper_pan = upper_pan + 1;
		elseif(codes[i].pan == -2) then
			lower_pan = lower_pan + 1;
		elseif(codes[i].pan == 1) then
			upper_pan = upper_pan + 1;
			pan1 = pan1 + 1;
		elseif(codes[i].pan == 2) then
			upper_pan = upper_pan + 1;
			pan2 = pan2 + 1;
		elseif(codes[i].pan == 3) then
			lower_pan = lower_pan + 1;
			pan3 = pan3 + 1;
		elseif(codes[i].pan == 4) then
			lower_pan = lower_pan + 1;
			pan4 = pan4 + 1;
		else	
		end
		if(codes[i].odds > 5) then
			odds_5 = odds_5 + 1;
		elseif(codes[i].odds > 4) then
			odds_4 = odds_4 + 1;
		else
		end
	end
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	trace(1, trace_prefix .. "pan1=" .. pan1 .. ",pan2=" .. pan2 .. ",pan3=" .. pan3 .. ",pan4=" .. pan4 .. ",upper_pan=" .. upper_pan .. ",lower_pan=" .. lower_pan);
	ok_count = 0;

	ok_count = GetMatchIdsCount( {"20180308001", "20180308004"}, codes);
	ret.info = trace_prefix .. "ok_count_1=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 1) then
		ret.code = 1;
		return ret;
	end
	
--[[

--]]	

	return ret;
end

