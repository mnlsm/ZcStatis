--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 0.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "8串1过滤";
kMatchBets = {
    "20180304014;-1;6,1,3.30;6,0,4.80;1,3,3.16;1,1,3.20",         --热那亚   VS   卡利亚里
    "20180304015;-1;6,1,3.25;6,0,3.30;1,3,3.80;1,1,3.65",         --兹沃勒   VS   芬洛
    "20180304016;1;6,3,2.45;6,1,3.05;1,1,4.00;1,0,6.05",         --通德拉   VS   沙维什
    "20180304017;-1;6,1,3.00;6,0,2.58;1,3,6.05;1,1,4.10",         --布莱克本   VS   维冈竞技
    "20180304018;-1;6,1,3.30;6,0,2.56;1,3,4.90;1,1,4.00",         --海登海姆   VS   比勒费尔德
    "20180304019;-1;6,1,3.25;6,0,2.90;1,3,4.40;1,1,3.85",         --不伦瑞克   VS   雷根斯堡
    "20180304020;-1;6,1,3.15;6,0,3.25;1,3,4.20;1,1,3.65",         --杜塞尔多夫   VS   圣保利
    "20180304021;1;6,3,3.80;6,1,3.40;1,1,3.50;1,0,3.30",         --布赖顿   VS   阿森纳
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
	end
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	trace(1, trace_prefix .. "pan1=" .. pan1 .. ",pan2=" .. pan2 .. ",pan3=" .. pan3 .. ",pan4=" .. pan4 .. ",upper_pan=" .. upper_pan .. ",lower_pan=" .. lower_pan);
	
	if(pan1 ~= 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan1 =" .. (pan1);
		trace(1, ret.info);
		return ret;
	end
	
	if(pan2 ~= 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan2 =" .. (pan2);
		trace(1, ret.info);
		return ret;
	end
	
	if(pan3 ~= 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan3 =" .. (pan3);
		trace(1, ret.info);
		return ret;
	end
	
	if(pan4 ~= 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan4 =" .. (pan4);
		trace(1, ret.info);
		return ret;
	end
	
--[[	
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	trace(1, trace_prefix .. "pan1=" .. pan1 .. ",pan2=" .. pan2 .. ",pan3=" .. pan3 .. ",pan4=" .. pan4 .. ",upper_pan=" .. upper_pan .. ",lower_pan=" .. lower_pan);
	
	if(upper_pan < 3 or upper_pan > 4) then
		ret.code = 1;
		ret.info = trace_prefix .. "upper_pan=" .. upper_pan;
		trace(1, ret.info);
		return ret;
	end

	if(pan2 + pan3 < 3) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan2 + pan3=" .. (pan2 + pan3);
		trace(1, ret.info);
		return ret;
	end
	
	
	ok_count = 0;
	if(codes[1].code == 0 and codes[6].code == 3) then
		ret.code = 1;
		ret.info = trace_prefix .. "codes[1].code == 0 and codes[7].code == 3";
		trace(1, ret.info);
		return ret;
	end
	
	if(upper_pan < 6) then
		ret.code = 1;
		ret.info = trace_prefix .. "upper_pan=" .. upper_pan;
		trace(1, ret.info);
		return ret;
	end
	
	if(pan1 < 1 or pan1 > 3) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan1=" .. pan1;
		trace(1, ret.info);
		return ret;
	end
	
	if(pan2 < 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan2=" .. pan2;
		trace(1, ret.info);
		return ret;
	end
	
	if(pan3 < 1 or pan3 > 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan3=" .. pan3;
		trace(1, ret.info);
		return ret;
	end
	
	if(pan4 > 1) then
		ret.code = 1;
		ret.info = trace_prefix .. "pan4=" .. pan4;
		trace(1, ret.info);
		return ret;
	end
	
	if(bonus < 25000 or bonus > 75000) then
		ret.code = 1;
		ret.info = trace_prefix .. "bonus=" .. bonus;
		trace(1, ret.info);
		return ret;
	end
	
	
	if(bonus < 130 or bonus > 220) then
		ret.code = 1;
		ret.info = trace_prefix .. "bonus < 130";
		trace(1, ret.info);
		return ret;
	end
	
	
	if(codes[1].tid == 6 and codes[1].code == 3) then 
		ret.code = 1;
		ret.info = trace_prefix .. "codes[1].code=" .. codes[1].code;
		trace(1, ret.info);
		return ret;
	end
	

	local total_sum = 0; --进球总数
	for i = 1, #params.codes do
		codes[i] = string.sub(params.codes, i , i);
		codes[i] = tonumber(codes[i]);
		total_sum = total_sum + codes[i];
	end
	local match_codes = {};
	for i = 1, #params.match_codes do
		match_codes[i] = string.sub(params.match_codes, i , i);
		match_codes[i] = tonumber(match_codes[i]);
	end
	
	local count3 = params.count3;        			  --进球数为3的个数
	local count2 = params.count2;        			  --进球数为2的个数 
	local count1 = params.count1;        			  --进球数为1的个数
	local count0 = params.count0;        			  --进球数为0的个数
	
	local match_sum1 = codes[1] + codes[2]; 		  --比赛1进球和
	local match_sum2 = codes[3] + codes[4]; 		  --比赛2进球和
	local match_sum3 = codes[5] + codes[6]; 		  --比赛3进球和
	local match_sum4 = codes[7] + codes[8]; 		  --比赛4进球和
	
	local match_gap1 = math.abs(codes[1] - codes[2]); --比赛1进球差
	local match_gap2 = math.abs(codes[3] - codes[4]); --比赛2进球差
	local match_gap3 = math.abs(codes[5] - codes[6]); --比赛3进球差
	local match_gap4 = math.abs(codes[7] - codes[8]); --比赛4进球差
	
	
-----------------------------------------------------------------------------------
	ret.code = 0;
	if count3 < 1 or count3 > 3 then
		ret.code = 1;
		ret.info = trace_prefix .. "count3=" .. count3;
		trace(1, ret.info);
		return ret;
	end

	if count2 < 0 or count2 > 6 then
		ret.code = 1;
		ret.info = trace_prefix .. "count2=" .. count2;
		trace(1, ret.info);
		return ret;
	end

	if count1 < 0 or count1 > 6 then
		ret.code = 1;
		ret.info = trace_prefix .. "count1=" .. count1;
		trace(1, ret.info);
		return ret;
	end

	if count0 < 1 or count0 > 4 then
		ret.code = 1;
		ret.info = trace_prefix .. "count0=" .. count0;
		trace(1, ret.info);
		return ret;
	end
	
	if(count1 + count0 < 4) then
		ret.code = 1;
		ret.info = trace_prefix .. "count1 + count0=" .. count1 + count0;
		trace(1, ret.info);
		return ret;
	end
	
	if(total_sum > 11 or total_sum < 8) then 
		ret.code = 1;
		ret.info = trace_prefix .. "total_sum=" .. total_sum;
		trace(1, ret.info);
		return ret;
	end
	
	errorCount = 0;
	if(match_sum1 == 2 or match_sum1 == 3) then
		errorCount = errorCount + 1;
	end
	if(match_sum2 == 2 or match_sum2 == 3) then
		errorCount = errorCount + 1;
	end
	if(match_sum3 == 2 or match_sum3 == 3) then
		errorCount = errorCount + 1;
	end
	if(match_sum4 == 2 or match_sum4 == 3) then
		errorCount = errorCount + 1;
	end
	if(errorCount < 1) then
		ret.code = 1;
		ret.info = trace_prefix .. "match_sum[2,3]=" .. errorCount;
		trace(1, ret.info);
		return ret;
	end

	errorCount = 0;
----------------------------------------------------------------------------------------
	if(match_codes[1] ~= 3 and match_codes[2] ~= 0) then
		ret.code = 1;
		ret.info = trace_prefix .. "match_codes[1,2]=" .. match_codes[1]..","..match_codes[2];
		trace(1, ret.info);
		return ret;
	end

	if(match_codes[4] == 1) then
		ret.code = 1;
		ret.info = trace_prefix .. "match_codes[4]=" .. match_codes[4];
		trace(1, ret.info);
		return ret;
	end

	if(match_codes[3] == 0 and match_gap3 ~=1) then
		ret.code = 1;
		ret.info = trace_prefix .. "match_codes[3]=" .. match_codes[3];
		trace(1, ret.info);
		return ret;
	end
	
	if(match_gap3 > 2) then
		ret.code = 1;
		ret.info = trace_prefix .. "match_gap3=" .. match_gap3;
		trace(1, ret.info);
		return ret;
	end

	tj = "F|1234|3033|2|4|";
	ret.code = IsFilterTJ(params.match_codes, tj);
	if(ret.code == 1) then
		ret.info = trace_prefix .. tj;
		trace(1, ret.info);
		return ret;
	end
	
--]]	

	return ret;
end

