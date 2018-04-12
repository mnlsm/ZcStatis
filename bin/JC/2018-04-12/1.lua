--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 176.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "过滤";
kMatchBets = {
    "20180412001;1;1,0,4.20;3,19,5.50;3,21,8.25",         --   布拉德福德   VS   什鲁斯伯里   
    "20180412002;1;6,0,1.98",         --莫斯科中央陆军   VS   阿森纳         
    "20180412003;-1;1,0,1.57",         --      萨尔茨堡   VS   拉齐奥         
    "20180412004;-1;1,3,4.20;3,1,8.50;3,3,8.00",         --            马赛   VS   莱比锡红牛   
    "20180412005;1;1,3,1.92;3,19,5.25;3,20,6.75",         --   里斯本竞技   VS   马德里竞技   
};


kMatchBetsLose = 1;
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
	local tj = "";
	local ok_count0 = 0;
	local ok_count1 = 0;
	
	local trace_prefix = "jc_dbg[";
	trace_prefix = trace_prefix .. "(" .. bonus .. "): ";
	
	ok_count0 = 0;
	ok_count0 = GetCodeCount(codes, 1, 0);
	if(ok_count0 > 1) then
		ret.info = trace_prefix .. "ok_count0=" .. (ok_count0);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end
	
	ok_count0 = 0;
	ok_count0 = GetMatchIdsCount( {"20180412002", "20180412003"}, codes);
	if(ok_count0 > 1) then
		ret.info = trace_prefix .. "ok_count0=" .. (ok_count0);
		trace(1, ret.info);
		ret.code = 1;
		return ret;	
	end
	
--[[

--]]	

	return ret;
end

