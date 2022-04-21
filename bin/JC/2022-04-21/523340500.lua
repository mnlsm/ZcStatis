--1
kMinBonus = 1027.0;
kMatchTitle = "竞猜足球 合买";
kMatchDesc = "过滤";
kMatchBetsLose = 0;
kAvgMultiple = 8;

kMatchBets = {
    "20220421002;-1;2,1,3.70;2,2,3.05;2,3,3.80;2,4,6.90",         --         西班牙  VS  巴列卡         
    "20220421003;1;2,1,7.50;2,2,4.30;2,3,3.40;2,4,4.25",         --         代格福  VS  哈马比         
    "20220421004;1;2,1,4.40;2,2,3.30;2,3,3.65;2,4,5.75",         --         伯恩利  VS  南安普         
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
	local ret = {code=0, info="ok", bonus=params.betbouns};
	local codes = params.betcodes;
	local bonus = params.betbouns;
	local trace_prefix = "jc_dbg[";
	
--	local m1_1 =  GetMatchCodeCount("20220421001", codes, 6, 1);   --m1, 平的个数
--	local m1_bf02 =  GetMatchCodeCount("20220421001", codes, 3, 19); --m1, 选择0:2的个数
	
	local m4_jq1 =  GetMatchCodeCount("20220421004", codes, 2, 1); --m2, 选择进球数1的个数
	local m4_jq4 =  GetMatchCodeCount("20220421004", codes, 2, 4); --m2, 选择进球数4的个数

	local m2_jq1 =  GetMatchCodeCount("20220421002", codes, 2, 1); --m2, 选择进球数1的个数
	local m2_jq4 =  GetMatchCodeCount("20220421002", codes, 2, 4); --m2, 选择进球数4的个数
	
	local m3_jq1 =  GetMatchCodeCount("20220421003", codes, 2, 1); --m3, 选择进球数1的个数
	local m3_jq4 =  GetMatchCodeCount("20220421003", codes, 2, 4); --m3, 选择进球数4的个数
	
	local total = m4_jq1 + m4_jq4 + m2_jq1 + m2_jq4 + m3_jq1 + m3_jq4;

	if(total > 1) then
		ret.info = trace_prefix .. "total=" .. (total) .. "]";
		trace(1, ret.info);
		ret.code = 1;
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