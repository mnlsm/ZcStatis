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
    "20180505001;-1;1,3,3.42;1,1,3.55",         --   札幌冈萨多   VS   大阪钢巴      
    "20180505002;-1;6,1,3.40;6,0,3.80;1,1,3.30",         --      柏太阳神   VS   磐田喜悦      
    "20180505003;-1;6,1,3.60;6,0,3.90;1,1,3.35",         --      川崎前锋   VS   东京FC          
    "20180505004;-1;6,1,3.13;6,0,2.47",         --          大邱FC   VS   庆南FC          
    "20180505005;1;6,3,2.90;6,1,3.15",         --         仁川联   VS   济州联         
    "20180505006;1;6,0,1.38",         --      全南天龙   VS   全北现代      
    "20180505007;-1;6,3,2.23;6,1,3.05",         --      蔚山现代   VS   浦项制铁      
    "20180505008;-1;6,1,3.85;1,3,2.62;1,1,3.30",         --      大阪樱花   VS   长崎航海      
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

function GetMatchCodeCount(codes, id, tid, code)
	local found_count = 0;
	for j = 1, #codes do
		if(id == codes[j].id) then
			if (tid == codes[j].tid and code == codes[j].code) then
				found_count = found_count + 1;
			end
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
	local tj = "";
	local ok_count0 = 0;
	local ok_count1 = 0;
	local ok_count2 = 0;
	
	local trace_prefix = "jc_dbg[";
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	
	ok_count0 = 0;
	ok_count0 = GetCodeCount(codes, 6, 1);
	if(ok_count0 < 1 or ok_count0 > 3) then
		ret.info = trace_prefix .. "ok_count0=" .. (ok_count0);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end
	
	ok_count1 = 0;
	ok_count1 = GetCodeCount(codes, 1, 1);
	if(ok_count1 > 3 or ok_count1 < 1) then
		ret.info = trace_prefix .. "ok_count1=" .. (ok_count1);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end	
	
	if(ok_count0 + ok_count1 > 4) then
		ret.info = trace_prefix .. "ok_count0 + ok_count1=" .. (ok_count1 + ok_count0);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end	
	
	ok_count0 = GetMatchCodeCount(codes, "20180505003", 6, 0);
	ok_count1 = GetMatchCodeCount(codes, "20180505008", 1, 3);
	ok_count2 = GetMatchCodeCount(codes, "20180505002", 6, 1);
	if(ok_count0 + ok_count1 + ok_count2> 1) then
		ret.info = trace_prefix .. "3 ok_count0 + ok_count1 + ok_count2=" .. (ok_count1 + ok_count0 + ok_count2);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end		
	
--[[

--]]	

	return ret;
end

