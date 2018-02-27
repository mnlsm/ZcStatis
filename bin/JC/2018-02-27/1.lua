--1

kMatchTitle = "hemai";
kMatchDesc = "come on, go!";
kMatchBets = {
    "20180227002;-1;6,1,3.35;2,1,4.20;2,3,3.50",         --赫尔城   VS   巴恩斯利
    "20180227004;-1;6,1,3.45;2,1,4.70;2,3,3.40",         --彼得堡联   VS   沃尔索尔
    "20180227005;1;6,1,3.40;2,1,4.30;2,3,3.65",         --布里斯托尔流浪   VS   维冈竞技
    "20180227007;1;6,0,1.40",         --圣约翰斯通   VS   格拉斯哥流浪者
};

local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end


function IsFilterLua(params)
	local errorCount = 0;
	local ret = {code=0, info="ok"};
	local codes = params.betcodes;
	local trace_prefix = "jc_dbg[";
	local tj = "";
	local odds = 0.0;
	
	for i = 1, #codes do
		if (i == 1) then
			odds = codes[i].odds;
		else
			odds = odds * codes[i].odds;
		end
		trace_prefix = trace_prefix .. codes[i].tid .. "-" .. codes[i].code;-- .. ";"  
		if(i == #codes) then
			trace_prefix = trace_prefix .. "]"
		else
			trace_prefix = trace_prefix .. ";"
		end
	end
	odds = 2 * odds;
	trace_prefix = trace_prefix .. "(" .. odds .. "): ";
	
	if(odds < 130 or odds > 220) then
		ret.code = 1;
		ret.info = trace_prefix .. "odds < 130";
		trace(1, ret.info);
		return ret;
	end
	
--[[	
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

