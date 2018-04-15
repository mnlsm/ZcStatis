--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMatchBetsLose = 0;
kMinBonus = 2500.0;
kMatchTitle = "竞彩足球 合买";
kMatchDesc = "混合过滤";
kMatchBets = {
    "20180406002;-1;2,4,5.40;3,2,10.00;3,3,7.50;3,15,6.60",         --   杜塞尔多夫   VS   波鸿            
    "20180406005;-1;2,4,5.10;3,2,10.00;3,3,7.50;3,15,6.50",         --         布尔格   VS   图尔            
    "20180406006;-1;2,4,5.30;3,2,6.80;3,3,7.00;3,15,7.00",         --         克莱蒙   VS   尼奥尔         
    "20180406027;-1;2,4,6.00;3,2,7.50;3,3,7.50;3,15,5.90",         --   拉科鲁尼亚   VS   马拉加         
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

function GetMatchResultCountEx(ids, tid, code, codes)
	local found_count = 0;
	for i = 1, #ids do
		for j = 1, #codes do
			if(tid == codes[j].tid and code == codes[j].code and ids[i] == codes[j].id) then
				found_count = found_count + 1;
			end
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
	local ok_count0 = 0;
	local ok_count1 = 0;
	local ok_count2 = 0;
	local ok_count3 = 0;
	
	
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
	
	ok_count0 = 0;
	ok_count0 = GetMatchResultCount(2, 4, codes);
	ret.info = trace_prefix .. "ok_count0=" .. (ok_count0);
	trace(1, ret.info);
	if(ok_count0 > 2) then
		ret.code = 1;
		return ret;
	end	

	ok_count1 = 0;
	ok_count1 = GetMatchResultCount(3, 2, codes);
	ret.info = trace_prefix .. "ok_count1=" .. (ok_count1);
	trace(1, ret.info);
	if(ok_count1 > 2) then
		ret.code = 1;
		return ret;
	end	
	
	ok_count2 = 0;
	ok_count2 = GetMatchResultCount(3, 3, codes);
	ret.info = trace_prefix .. "ok_count2=" .. (ok_count2);
	trace(1, ret.info);
	if(ok_count2 > 3) then
		ret.code = 1;
		return ret;
	end	
	
	ok_count3 = 0;
	ok_count3 = GetMatchResultCount(3, 15, codes);
	ret.info = trace_prefix .. "ok_count3=" .. (ok_count3);
	trace(1, ret.info);
	if(ok_count3 > 2) then
		ret.code = 1;
		return ret;
	end	

	ok_count = 0;
	ok_count = GetMatchResultCountEx({"20180406002", "20180406005"}, 3, 2, codes);
	ret.info = trace_prefix .. "ok_count=" .. (ok_count);
	trace(1, ret.info);
	if(ok_count > 1) then
		ret.code = 1;
		return ret;
	end	
	
	
--[[	

--]]	

	return ret;
end

