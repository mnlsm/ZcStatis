--1
local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

kMinBonus = 500.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "过滤";
kMatchBetsLose = 0;

kMatchBets = {
    "20220419002;1;2,1,4.30;2,2,3.25;2,3,3.60;2,4,5.80",         --索肖 VS 图卢兹
    "20220419003;-1;2,1,4.10;2,2,3.20;2,3,3.65;2,4,6.00",         --欧塞尔 VS 第戎
    "20220419004;-1;2,1,3.85;2,2,3.10;2,3,3.75;2,4,6.50",         --甘冈 VS 波城
};



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
	local jq2 =  GetCodeCount(codes, 2, 2);
	local jq3 =  GetCodeCount(codes, 2, 3);

	if(jq2 + jq3 <= 1) then
		ret.info = trace_prefix .. "jq1=" .. (jq1);
		trace(1, ret.info);
		ret.code = 1;
		return ret;
	end
	

	return ret;
end

