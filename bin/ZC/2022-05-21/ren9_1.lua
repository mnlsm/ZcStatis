--1
kMatchTitle = "足彩脚本";
kMatchDesc = "过滤";
kMatchBetsLose = 0;
kZcKaiJiangResult="03030311030133";
kAvgMultiple = 0;
kMinBonus = -5000000.0;
kBetsRankRatioRange = "0.4-0.65";
kMatchBets = {
    "01;1;1;6,3,4.20;6,1,3.60;6,0,1.85",      --             弗赖堡  VS  莱比锡             
    "03;-1;-1;6,3,1.22;6,1,6.00",             --             亚特兰  VS  恩波利             
    "04;-1;-1;6,3,1.60;6,1,3.90;6,0,5.50",    --             拉齐奥  VS  维罗纳             
    "08;1;1;6,3,2.80;6,1,3.40",               --               南特  VS  埃蒂安             
    "09;1;1;6,3,4.00;6,1,3.50;6,0,1.91",      --               里尔  VS  雷恩               
    "10;-1;-1;6,3,2.10;6,1,3.39;6,0,3.41",    --               马赛  VS  斯特堡             
    "11;-1;-1;6,3,1.17",                      --             日尔曼  VS  梅斯               
    "12;1;1;6,3,3.60;6,1,3.50;6,0,2.00",      --               朗斯  VS  摩纳哥             
    "14;1;1;6,3,4.20;6,1,3.50;6,0,1.85",      --               兰斯  VS  尼斯               
};


local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info);
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
function GetIndexCode(index, codes, tid)
	local found_code = -100;
	if index >= 1 and index <= #codes then
		if (tid == codes[index].tid) then
			found_code = codes[index].code;
		end
	end
	return found_code;
end 
function GetCodesText(codes)
	local result = "";
	for index = 1, #codes do
		result = result .. codes[index].code
	end
	return result;
end
function IsFilterLua(params)
	local errorCount = 0;
	local ret = {code=0, info="ok", bonus=params.betbouns};
	local codes = params.betcodes;
	local bonus = params.betbouns;
	local trace_prefix = "jc_dbg[";
	local trace_subfix = "]\r\n";
	local jq0_sum =  GetCodeCount(codes, 2, 0); --选择进球数0的个数
	local jq1_sum =  GetCodeCount(codes, 2, 1); --选择进球数1的个数
	local jq2_sum =  GetCodeCount(codes, 2, 2); --选择进球数2的个数
	local jq3_sum =  GetCodeCount(codes, 2, 3); --选择进球数3的个数
	local jq4_sum =  GetCodeCount(codes, 2, 4); --选择进球数4的个数
	local jq5_sum =  GetCodeCount(codes, 2, 5); --选择进球数5的个数
	local jq6_sum =  GetCodeCount(codes, 2, 6); --选择进球数6的个数
	local jq7_sum =  GetCodeCount(codes, 2, 7); --选择进球数7的个数
	local jq_all_sum = GetCodeSum(codes, 2);    --选择进球总数
	local codes_text = GetCodesText(codes);

	-- [1], [01], [德国杯], [弗赖堡  VS  莱比锡]
	local code_1_3 = GetIndexCodeCount(1, codes, 6, 3);         --场次1的3的个数[4.20]
	local code_1_1 = GetIndexCodeCount(1, codes, 6, 1);         --场次1的1的个数[3.60]
	local code_1_0 = GetIndexCodeCount(1, codes, 6, 0);         --场次1的0的个数[1.85]
	local pan_1_3 = GetIndexPanCount(1, codes, 3);              --场次1的下盘1的个数
	local pan_1_4 = GetIndexPanCount(1, codes, 4);              --场次1的下盘2的个数
	local pan_1_up = GetIndexPanCount(1, codes, -1);            --场次1的上盘的个数
	local pan_1_down = pan_1_3 + pan_1_4;                       --场次1的下盘的个数

	-- [2], [03], [意甲], [亚特兰  VS  恩波利]
	local code_2_3 = GetIndexCodeCount(2, codes, 6, 3);         --场次2的3的个数[1.22]
	local code_2_1 = GetIndexCodeCount(2, codes, 6, 1);         --场次2的1的个数[6.00]
	local code_2_0 = GetIndexCodeCount(2, codes, 6, 0);         --场次2的0的个数[13.00]
	local pan_2_3 = GetIndexPanCount(2, codes, 3);              --场次2的下盘1的个数
	local pan_2_4 = GetIndexPanCount(2, codes, 4);              --场次2的下盘2的个数
	local pan_2_up = GetIndexPanCount(2, codes, -1);            --场次2的上盘的个数
	local pan_2_down = pan_2_3 + pan_2_4;                       --场次2的下盘的个数

	-- [3], [04], [意甲], [拉齐奥  VS  维罗纳]
	local code_3_3 = GetIndexCodeCount(3, codes, 6, 3);         --场次3的3的个数[1.60]
	local code_3_1 = GetIndexCodeCount(3, codes, 6, 1);         --场次3的1的个数[3.90]
	local code_3_0 = GetIndexCodeCount(3, codes, 6, 0);         --场次3的0的个数[5.50]
	local pan_3_3 = GetIndexPanCount(3, codes, 3);              --场次3的下盘1的个数
	local pan_3_4 = GetIndexPanCount(3, codes, 4);              --场次3的下盘2的个数
	local pan_3_up = GetIndexPanCount(3, codes, -1);            --场次3的上盘的个数
	local pan_3_down = pan_3_3 + pan_3_4;                       --场次3的下盘的个数

	-- [4], [08], [法甲], [南特  VS  埃蒂安]
	local code_4_3 = GetIndexCodeCount(4, codes, 6, 3);         --场次4的3的个数[2.80]
	local code_4_1 = GetIndexCodeCount(4, codes, 6, 1);         --场次4的1的个数[3.40]
	local code_4_0 = GetIndexCodeCount(4, codes, 6, 0);         --场次4的0的个数[2.45]
	local pan_4_3 = GetIndexPanCount(4, codes, 3);              --场次4的下盘1的个数
	local pan_4_4 = GetIndexPanCount(4, codes, 4);              --场次4的下盘2的个数
	local pan_4_up = GetIndexPanCount(4, codes, -1);            --场次4的上盘的个数
	local pan_4_down = pan_4_3 + pan_4_4;                       --场次4的下盘的个数

	-- [5], [09], [法甲], [里尔  VS  雷恩]
	local code_5_3 = GetIndexCodeCount(5, codes, 6, 3);         --场次5的3的个数[4.00]
	local code_5_1 = GetIndexCodeCount(5, codes, 6, 1);         --场次5的1的个数[3.50]
	local code_5_0 = GetIndexCodeCount(5, codes, 6, 0);         --场次5的0的个数[1.91]
	local pan_5_3 = GetIndexPanCount(5, codes, 3);              --场次5的下盘1的个数
	local pan_5_4 = GetIndexPanCount(5, codes, 4);              --场次5的下盘2的个数
	local pan_5_up = GetIndexPanCount(5, codes, -1);            --场次5的上盘的个数
	local pan_5_down = pan_5_3 + pan_5_4;                       --场次5的下盘的个数

	-- [6], [10], [法甲], [马赛  VS  斯特堡]
	local code_6_3 = GetIndexCodeCount(6, codes, 6, 3);         --场次6的3的个数[2.10]
	local code_6_1 = GetIndexCodeCount(6, codes, 6, 1);         --场次6的1的个数[3.39]
	local code_6_0 = GetIndexCodeCount(6, codes, 6, 0);         --场次6的0的个数[3.41]
	local pan_6_3 = GetIndexPanCount(6, codes, 3);              --场次6的下盘1的个数
	local pan_6_4 = GetIndexPanCount(6, codes, 4);              --场次6的下盘2的个数
	local pan_6_up = GetIndexPanCount(6, codes, -1);            --场次6的上盘的个数
	local pan_6_down = pan_6_3 + pan_6_4;                       --场次6的下盘的个数

	-- [7], [11], [法甲], [日尔曼  VS  梅斯]
	local code_7_3 = GetIndexCodeCount(7, codes, 6, 3);         --场次7的3的个数[1.17]
	local code_7_1 = GetIndexCodeCount(7, codes, 6, 1);         --场次7的1的个数[6.50]
	local code_7_0 = GetIndexCodeCount(7, codes, 6, 0);         --场次7的0的个数[19.00]
	local pan_7_3 = GetIndexPanCount(7, codes, 3);              --场次7的下盘1的个数
	local pan_7_4 = GetIndexPanCount(7, codes, 4);              --场次7的下盘2的个数
	local pan_7_up = GetIndexPanCount(7, codes, -1);            --场次7的上盘的个数
	local pan_7_down = pan_7_3 + pan_7_4;                       --场次7的下盘的个数

	-- [8], [12], [法甲], [朗斯  VS  摩纳哥]
	local code_8_3 = GetIndexCodeCount(8, codes, 6, 3);         --场次8的3的个数[3.60]
	local code_8_1 = GetIndexCodeCount(8, codes, 6, 1);         --场次8的1的个数[3.50]
	local code_8_0 = GetIndexCodeCount(8, codes, 6, 0);         --场次8的0的个数[2.00]
	local pan_8_3 = GetIndexPanCount(8, codes, 3);              --场次8的下盘1的个数
	local pan_8_4 = GetIndexPanCount(8, codes, 4);              --场次8的下盘2的个数
	local pan_8_up = GetIndexPanCount(8, codes, -1);            --场次8的上盘的个数
	local pan_8_down = pan_8_3 + pan_8_4;                       --场次8的下盘的个数

	-- [9], [14], [法甲], [兰斯  VS  尼斯]
	local code_9_3 = GetIndexCodeCount(9, codes, 6, 3);         --场次9的3的个数[4.20]
	local code_9_1 = GetIndexCodeCount(9, codes, 6, 1);         --场次9的1的个数[3.50]
	local code_9_0 = GetIndexCodeCount(9, codes, 6, 0);         --场次9的0的个数[1.85]
	local pan_9_3 = GetIndexPanCount(9, codes, 3);              --场次9的下盘1的个数
	local pan_9_4 = GetIndexPanCount(9, codes, 4);              --场次9的下盘2的个数
	local pan_9_up = GetIndexPanCount(9, codes, -1);            --场次9的上盘的个数
	local pan_9_down = pan_9_3 + pan_9_4;                       --场次9的下盘的个数

	 -- stat items
	local code_0_sum = code_1_0 + code_2_0 + code_3_0 + code_4_0 + code_5_0 + code_6_0 + code_7_0 + code_8_0 + code_9_0;
	local code_1_sum = code_1_1 + code_2_1 + code_3_1 + code_4_1 + code_5_1 + code_6_1 + code_7_1 + code_8_1 + code_9_1;
	local code_3_sum = code_1_3 + code_2_3 + code_3_3 + code_4_3 + code_5_3 + code_6_3 + code_7_3 + code_8_3 + code_9_3;
	local pan_3_sum = pan_1_3 + pan_2_3 + pan_3_3 + pan_4_3 + pan_5_3 + pan_6_3 + pan_7_3 + pan_8_3 + pan_9_3;
	local pan_4_sum = pan_1_4 + pan_2_4 + pan_3_4 + pan_4_4 + pan_5_4 + pan_6_4 + pan_7_4 + pan_8_4 + pan_9_4;
	local pan_down_sum = pan_1_down + pan_2_down + pan_3_down + pan_4_down + pan_5_down + pan_6_down + pan_7_down + pan_8_down + pan_9_down;
	local pan_up_sum = pan_1_up + pan_2_up + pan_3_up + pan_4_up + pan_5_up + pan_6_up + pan_7_up + pan_8_up + pan_9_up;
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
