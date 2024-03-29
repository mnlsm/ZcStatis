--1
kMatchTitle = "竞彩足球";
kMatchDesc = "过滤";
kMatchBetsLose = 0;
kAvgMultiple = 0;
kMinBonus = -5000000.0;

kBetsRankRatioRange = "0.0-1.0";


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

function GetCodeSum(codes, tid)
	local sum = 0;
	for j = 1, #codes do
		if(tid == codes[j].tid) then
			sum = sum + codes[j].code;
		end
	end
	return sum;
end 

function GetIndexCodeCount(index, codes, tid, code)
	local found_count = 0;
	if index >= 1 and index <= #codes then
		if (tid == codes[index].tid and code == codes[index].code) then
			found_count = found_count + 1;
		end
	end
	return found_count;
end 

function GetIndexPanCount(index, codes, pan)
	local found_count = 0;
	if index >= 1 and index <= #codes then
		if (pan == codes[index].pan) then
			found_count = found_count + 1;
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
	local trace_subfix = "]\r\n";
	local jq0 =  GetCodeCount(codes, 2, 0); --选择进球数0的个数
	local jq1 =  GetCodeCount(codes, 2, 1); --选择进球数1的个数
	local jq2 =  GetCodeCount(codes, 2, 2); --选择进球数2的个数
	local jq3 =  GetCodeCount(codes, 2, 3); --选择进球数3的个数
	local jq4 =  GetCodeCount(codes, 2, 4); --选择进球数4的个数
	local jq5 =  GetCodeCount(codes, 2, 5); --选择进球数5的个数
	local jq6 =  GetCodeCount(codes, 2, 6); --选择进球数6的个数
	local jq7 =  GetCodeCount(codes, 2, 7); --选择进球数7的个数
	local jq_sum = GetCodeSum(codes, 2);    --选择进球总数

	local pan_1_1 =  GetIndexPanCount(1, codes, 1);  --场次1的pan1的个数
	local pan_1_2 =  GetIndexPanCount(1, codes, 2);  --场次1的pan2的个数
	local pan_1_3 =  GetIndexPanCount(1, codes, 3);  --场次1的pan3的个数
	local pan_1_4 =  GetIndexPanCount(1, codes, 4);  --场次1的pan4的个数
	local pan_1_up =  pan_1_1 + pan_1_2;			  --场次1的上pan的个数
	local pan_2_1 =  GetIndexPanCount(2, codes, 1);  --场次2的pan1的个数
	local pan_2_2 =  GetIndexPanCount(2, codes, 2);  --场次2的pan2的个数
	local pan_2_3 =  GetIndexPanCount(2, codes, 3);  --场次2的pan3的个数
	local pan_2_4 =  GetIndexPanCount(2, codes, 4);  --场次2的pan4的个数
	local pan_2_up =  pan_2_1 + pan_2_2;			  --场次2的上pan的个数
	local pan_3_1 =  GetIndexPanCount(3, codes, 1);  --场次3的pan1的个数
	local pan_3_2 =  GetIndexPanCount(3, codes, 2);  --场次3的pan2的个数
	local pan_3_3 =  GetIndexPanCount(3, codes, 3);  --场次3的pan3的个数
	local pan_3_4 =  GetIndexPanCount(3, codes, 4);  --场次3的pan4的个数
	local pan_3_up =  pan_3_1 + pan_3_2;			  --场次3的上pan的个数
	local pan_4_1 =  GetIndexPanCount(4, codes, 1);  --场次4的pan1的个数
	local pan_4_2 =  GetIndexPanCount(4, codes, 2);  --场次4的pan2的个数
	local pan_4_3 =  GetIndexPanCount(4, codes, 3);  --场次4的pan3的个数
	local pan_4_4 =  GetIndexPanCount(4, codes, 4);  --场次4的pan4的个数
	local pan_4_up =  pan_4_1 + pan_4_2;			  --场次4的上pan的个数
	local pan_5_1 =  GetIndexPanCount(5, codes, 1);  --场次5的pan1的个数
	local pan_5_2 =  GetIndexPanCount(5, codes, 2);  --场次5的pan2的个数
	local pan_5_3 =  GetIndexPanCount(5, codes, 3);  --场次5的pan3的个数
	local pan_5_4 =  GetIndexPanCount(5, codes, 4);  --场次5的pan4的个数
	local pan_5_up =  pan_5_1 + pan_5_2;			  --场次5的上pan的个数
	local pan_6_1 =  GetIndexPanCount(6, codes, 1);  --场次6的pan1的个数
	local pan_6_2 =  GetIndexPanCount(6, codes, 2);  --场次6的pan2的个数
	local pan_6_3 =  GetIndexPanCount(6, codes, 3);  --场次6的pan3的个数
	local pan_6_4 =  GetIndexPanCount(6, codes, 4);  --场次6的pan4的个数
	local pan_6_up =  pan_6_1 + pan_6_2;			  --场次6的上pan的个数
	local pan_7_1 =  GetIndexPanCount(7, codes, 1);  --场次7的pan1的个数
	local pan_7_2 =  GetIndexPanCount(7, codes, 2);  --场次7的pan2的个数
	local pan_7_3 =  GetIndexPanCount(7, codes, 3);  --场次7的pan3的个数
	local pan_7_4 =  GetIndexPanCount(7, codes, 4);  --场次7的pan4的个数
	local pan_7_up =  pan_7_1 + pan_7_2;			  --场次7的上pan的个数
	local pan_8_1 =  GetIndexPanCount(8, codes, 1);  --场次8的pan1的个数
	local pan_8_2 =  GetIndexPanCount(8, codes, 2);  --场次8的pan2的个数
	local pan_8_3 =  GetIndexPanCount(8, codes, 3);  --场次8的pan3的个数
	local pan_8_4 =  GetIndexPanCount(8, codes, 4);  --场次8的pan4的个数
	local pan_8_up =  pan_8_1 + pan_8_2;			  --场次8的上pan的个数

	local pan1_sum = pan_1_1 + pan_2_1 + pan_3_1 + pan_4_1 + pan_5_1 + pan_6_1 + pan_7_1 + pan_8_1;
	local pan2_sum = pan_1_2 + pan_2_2 + pan_3_2 + pan_4_2 + pan_5_2 + pan_6_2 + pan_7_2 + pan_8_2;
	local pan3_sum = pan_1_3 + pan_2_3 + pan_3_3 + pan_4_3 + pan_5_3 + pan_6_3 + pan_7_3 + pan_8_3;
	local pan4_sum = pan_1_4 + pan_2_4 + pan_3_4 + pan_4_4 + pan_5_4 + pan_6_4 + pan_7_4 + pan_8_4;
	local pan_up_sum = pan1_sum + pan2_sum;
	local pan_down_sum = pan3_sum + pan4_sum;





--[[	
	if(jq1 > 2) then
		ret.code = 1;
		ret.info = trace_prefix .."jq1>2" .. trace_subfix;
		trace(1, ret.info);
		return ret;
	end

--]]	
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