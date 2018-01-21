--18007

kMatchScores = {
    "1,2,3", 		--拉齐奥
    "0", 		--切沃
    "0,1", 		--卡利亚里
    "0,1,2,3", 		--AC米兰
    "0,1", 	 	--国际米兰
    "0", 		--罗马
    "0,1,2", 		--萨索洛
    "0,1,2,3",   	--都灵
};

local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end


function IsFilterLua(params)
	local errorCount = 0;
	local ret = {code=0, info="ok"};
	local trace_prefix = params.codes .. ", filtered: ";
	local tj = "";
	local codes = {};
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
	if count3 < 0 or count3 > 3 then
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

	if count1 < 1 or count1 > 6 then
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
	
	if(total_sum > 11 or total_sum < 5) then 
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

----------------------------------------------------------------------------------------
	errorCount = 0;
	if (match_codes[2] ~= 0) then
		errorCount = errorCount + 1;
	end
	if (match_codes[4] ~= 3) then
		errorCount = errorCount + 1;
	end
	if(errorCount < 1) then
		ret.code = 1;
		ret.info = trace_prefix .. "lengmen=" .. errorCount;
		trace(1, ret.info);
		return ret;
	end
	
	tj = "F|34|00|0|1|";
	ret.code = IsFilterTJ(params.match_codes, tj);
	if(ret.code == 1) then
		ret.info = trace_prefix .. tj;
		trace(1, ret.info);
		return ret;
	end
	
	return ret;
end

