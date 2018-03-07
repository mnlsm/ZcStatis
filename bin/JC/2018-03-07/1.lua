--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end


kMatchBetsLose = 2;
kMinBonus = 0.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "7串1过滤";

kMatchBets = {
    "20180307011;-1;6,3,2.20",         --横滨水手   VS   东京FC
    "20180307012;-1;6,3,1.37",         --上海上港   VS   蔚山现代
    "20180307013;-2;1,3,2.09",         --曼彻斯特城   VS   巴塞尔
    "20180307014;-1;1,3,4.00",         --托特纳姆热刺   VS   尤文图斯
    "20180307015;1;6,3,3.75",         --利兹联   VS   狼队
};

kMatchBetsFixed = {
    "20180307024;-1;6,3,1.47;6,1,3.80",         --蒙特雷   VS   克雷塔罗
    "20180307025;-1;6,3,2.08;6,1,3.00",         --西雅图海湾人   VS   瓜达拉哈拉
    "20180307026;-1;6,3,1.95;6,1,3.30",         --美洲狮   VS   普埃布拉大学
};


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
	local ok_count = 0;
	
	local odds_4 = 0;
	local odds_5 = 0;
	
	
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
			odds_4 = odds_5 + 1;
		else
		end
	end
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	trace(1, trace_prefix .. "pan1=" .. pan1 .. ",pan2=" .. pan2 .. ",pan3=" .. pan3 .. ",pan4=" .. pan4 .. ",upper_pan=" .. upper_pan .. ",lower_pan=" .. lower_pan);
	ok_count = 0;


	
--[[

--]]	

	return ret;
end

