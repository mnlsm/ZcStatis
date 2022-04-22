--1
kMinBonus = 2038.0;
kMatchTitle = "竞彩足球 合买";
kMatchDesc = "过滤";
kMatchBetsLose = 0;
kAvgMultiple = 15;

kMatchBets = {
    "20220422002;-1;2,1,5.65;2,2,3.90;2,3,3.35;2,4,4.60",    --         卡尔斯  VS  因戈尔         
    "20220422003;-1;2,1,4.40;2,2,3.30;2,3,3.60;2,4,5.65",    --         特温特  VS  鹿特丹         
    "20220422004;-1;2,1,5.30;2,2,3.85;2,3,3.50;2,4,4.75",    --           埃门  VS  罗达J          
};


--[[
kMatchBetsFixed = {
    "20180307024;-1;6,3,1.47;6,1,3.80",         --蒙特雷   VS   克雷塔罗
    "20180307025;-1;6,3,2.08;6,1,3.00",         --西雅图海湾人   VS   瓜达拉哈拉
    "20180307026;-1;6,3,1.95;6,1,3.30",         --美洲狮   VS   普埃布拉大学
};
--]]

local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
end

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

function GetMatchCodeCount(id, codes, tid, code)
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
	local ret = {code=1, info="failed", bonus=params.betbouns};
	local codes = params.betcodes;
	local bonus = params.betbouns;
	local trace_prefix = "jc_dbg[";
	local jq1 =  GetCodeCount(codes, 2, 1); --选择进球数1的个数
	local jq2 =  GetCodeCount(codes, 2, 2); --选择进球数2的个数
	local jq3 =  GetCodeCount(codes, 2, 3); --选择进球数3的个数
	local jq4 =  GetCodeCount(codes, 2, 4); --选择进球数4的个数
	
	if(jq1 > 2) then
		ret.info = trace_prefix .."jq1>2]";
		trace(1, ret.info);
		return ret;
	end
	if(jq2 == 3) then
		ret.info = trace_prefix .."jq2=3]";
		trace(1, ret.info);
		return ret;
	end
	if(jq3 == 3) then
		ret.info = trace_prefix .."jq3=3]";
		trace(1, ret.info);
		return ret;
	end
	if(jq4 > 1) then
		ret.info = trace_prefix .."jq4>1]";
		trace(1, ret.info);
		return ret;
	end
	if(jq2 + jq3 >= 2) then
		ret.info = trace_prefix .. "ok 1]";
		trace(1, ret.info);
		ret.code = 0;
		return ret;
	end
	if(jq1 + jq2 + jq3 == 3) then
		ret.info = trace_prefix .. "ok 2]";
		trace(1, ret.info);
		ret.code = 0;
		return ret;
	end
	return ret;
end

--[[
	tid=1,  code=3,   tip=让胜                                      
	tid=1,  code=1,   tip=让平                                      
	tid=1,  code=0,   tip=让负                                      

	tid=6,  code=3,   tip=胜                                        
	tid=6,  code=1,   tip=平                                        
	tid=6,  code=0,   tip=负                                        

	tid=4,  code=0,   tip=胜/胜                                     
	tid=4,  code=1,   tip=胜/平                                     
	tid=4,  code=2,   tip=胜/负                                     
	tid=4,  code=3,   tip=平/胜                                     
	tid=4,  code=4,   tip=平/平                                     
	tid=4,  code=5,   tip=平/负                                     
	tid=4,  code=6,   tip=负/胜                                     
	tid=4,  code=7,   tip=负/平                                     
	tid=4,  code=8,   tip=负/负                                     

	tid=3,  code=0,   tip=1:0                                       
	tid=3,  code=1,   tip=2:0                                       
	tid=3,  code=2,   tip=2:1                                       
	tid=3,  code=3,   tip=3:0                                       
	tid=3,  code=4,   tip=3:1                                       
	tid=3,  code=5,   tip=3:2                                       
	tid=3,  code=6,   tip=4:0                                       
	tid=3,  code=7,   tip=4:1                                       
	tid=3,  code=8,   tip=4:2                                       
	tid=3,  code=9,   tip=5:0                                       
	tid=3,  code=10,  tip=5:1                                       
	tid=3,  code=11,  tip=5:2                                       
	tid=3,  code=12,  tip=胜其他                                    
	tid=3,  code=13,  tip=0:0                                       
	tid=3,  code=14,  tip=1:1                                       
	tid=3,  code=15,  tip=2:2                                       
	tid=3,  code=16,  tip=3:3                                       
	tid=3,  code=17,  tip=平其他                                    
	tid=3,  code=18,  tip=0:1                                       
	tid=3,  code=19,  tip=0:2                                       
	tid=3,  code=20,  tip=1:2                                       
	tid=3,  code=21,  tip=0:3                                       
	tid=3,  code=22,  tip=1:3                                       
	tid=3,  code=23,  tip=2:3                                       
	tid=3,  code=24,  tip=0:4                                       
	tid=3,  code=25,  tip=1:4                                       
	tid=3,  code=26,  tip=2:4                                       
	tid=3,  code=27,  tip=0:5                                       
	tid=3,  code=28,  tip=1:5                                       
	tid=3,  code=29,  tip=2:5                                       
	tid=3,  code=30,  tip=负其他       
								 
	tid=2,  code=0,   tip=0球                                       
	tid=2,  code=1,   tip=1球                                       
	tid=2,  code=2,   tip=2球                                       
	tid=2,  code=3,   tip=3球                                       
	tid=2,  code=4,   tip=4球                                       
	tid=2,  code=5,   tip=5球                                       
	tid=2,  code=6,   tip=6球                                       
	tid=2,  code=7,   tip=7+球                                      
--]]