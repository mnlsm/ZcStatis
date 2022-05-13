--1
kMatchTitle = "传统足彩";
kMatchDesc = "过滤";
kMatchBetsLose = 0;
kAvgMultiple = 0;
kMinBonus = -5000000.0;

kBetsRankRatioRange = "0.0-1.0";



kMatchBets = {
    "03;0;6,3,1.30;6,1,5.80",    --               多特  VS  赫塔               
    "02;0;6,3,2.60;6,0,2.45",    --               勒沃  VS  弗赖堡             
    "04;0;6,3,1.44;6,1,4.60",    --             柏林联  VS  波鸿               
    "05;0;6,3,2.45;6,0,2.62",    --             斯图加  VS  科隆               
    "01;0;6,3,3.50;6,0,2.00",    --             切尔西  VS  利物浦             
    "06;0;6,3,2.15;6,1,3.50",    --             维罗纳  VS  都灵               
    "07;0;6,3,1.62;6,0,5.00",    --             乌迪内  VS  斯佩齐             
    "08;0;6,3,1.25;6,1,5.80",    --               罗马  VS  威尼斯             
    "12;0;6,3,2.25;6,0,3.00",    --             波尔多  VS  洛里昂             
    "13;0;6,3,1.35;6,1,4.80",    --               里昂  VS  南特               
    "11;0;6,3,2.80;6,1,3.25",    --               梅斯  VS  昂热               
    "14;0;6,3,2.05;6,0,3.60",    --               雷恩  VS  马赛               
    "09;0;6,3,2.00;6,0,3.80",    --             埃蒂安  VS  兰斯               
    "10;0;6,3,3.10;6,1,3.25",    --             特鲁瓦  VS  朗斯               
};

kMatchBetsFixed = {
};



local function trace(level, info) 
	if(level > 0) then
		dbgview_print(info.."\r\n");
	end
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

function IsFilterLua(params)
	local errorCount = 0;
	local ret = {code=0, info="ok", bonus=params.betbouns};
	local codes = params.betcodes;
	local bonus = params.betbouns;
	local trace_prefix = "jc_dbg[";
	local spf_1_3 =  GetIndexCodeCount(1, codes, 6, 3);  --场次1的3的个数
	local spf_1_1 =  GetIndexCodeCount(1, codes, 6, 1);  --场次1的1的个数
	local spf_1_0 =  GetIndexCodeCount(1, codes, 6, 0);  --场次1的0的个数
	local spf_2_3 =  GetIndexCodeCount(2, codes, 6, 3);  --场次2的3的个数
	local spf_2_1 =  GetIndexCodeCount(2, codes, 6, 1);  --场次2的1的个数
	local spf_2_0 =  GetIndexCodeCount(2, codes, 6, 0);  --场次2的0的个数
	local spf_3_3 =  GetIndexCodeCount(3, codes, 6, 3);  --场次3的3的个数
	local spf_3_1 =  GetIndexCodeCount(3, codes, 6, 1);  --场次3的1的个数
	local spf_3_0 =  GetIndexCodeCount(3, codes, 6, 0);  --场次3的0的个数
	local spf_4_3 =  GetIndexCodeCount(4, codes, 6, 3);  --场次4的3的个数
	local spf_4_1 =  GetIndexCodeCount(4, codes, 6, 1);  --场次4的1的个数
	local spf_4_0 =  GetIndexCodeCount(4, codes, 6, 0);  --场次4的0的个数
	local spf_5_3 =  GetIndexCodeCount(5, codes, 6, 3);  --场次5的3的个数
	local spf_5_1 =  GetIndexCodeCount(5, codes, 6, 1);  --场次5的1的个数
	local spf_5_0 =  GetIndexCodeCount(5, codes, 6, 0);  --场次5的0的个数
	local spf_6_3 =  GetIndexCodeCount(6, codes, 6, 3);  --场次6的3的个数
	local spf_6_1 =  GetIndexCodeCount(6, codes, 6, 1);  --场次6的1的个数
	local spf_6_0 =  GetIndexCodeCount(6, codes, 6, 0);  --场次6的0的个数
	local spf_7_3 =  GetIndexCodeCount(7, codes, 6, 3);  --场次7的3的个数
	local spf_7_1 =  GetIndexCodeCount(7, codes, 6, 1);  --场次7的1的个数
	local spf_7_0 =  GetIndexCodeCount(7, codes, 6, 0);  --场次7的0的个数
	local spf_8_3 =  GetIndexCodeCount(8, codes, 6, 3);  --场次8的3的个数
	local spf_8_1 =  GetIndexCodeCount(8, codes, 6, 1);  --场次8的1的个数
	local spf_8_0 =  GetIndexCodeCount(8, codes, 6, 0);  --场次8的0的个数
	local spf_9_3 =  GetIndexCodeCount(9, codes, 6, 3);  --场次9的3的个数
	local spf_9_1 =  GetIndexCodeCount(9, codes, 6, 1);  --场次9的1的个数
	local spf_9_0 =  GetIndexCodeCount(9, codes, 6, 0);  --场次9的0的个数
	local spf_a_3 =  GetIndexCodeCount(10, codes, 6, 3); --场次10的3的个数
	local spf_a_1 =  GetIndexCodeCount(10, codes, 6, 1); --场次10的1的个数
	local spf_a_0 =  GetIndexCodeCount(10, codes, 6, 0); --场次10的0的个数
	local spf_b_3 =  GetIndexCodeCount(11, codes, 6, 3); --场次11的3的个数
	local spf_b_1 =  GetIndexCodeCount(11, codes, 6, 1); --场次11的1的个数
	local spf_b_0 =  GetIndexCodeCount(11, codes, 6, 0); --场次11的0的个数
	local spf_c_3 =  GetIndexCodeCount(12, codes, 6, 3); --场次12的3的个数
	local spf_c_1 =  GetIndexCodeCount(12, codes, 6, 1); --场次12的1的个数
	local spf_c_0 =  GetIndexCodeCount(12, codes, 6, 0); --场次12的0的个数
	local spf_d_3 =  GetIndexCodeCount(13, codes, 6, 3); --场次13的3的个数
	local spf_d_1 =  GetIndexCodeCount(13, codes, 6, 1); --场次13的1的个数
	local spf_d_0 =  GetIndexCodeCount(13, codes, 6, 0); --场次13的0的个数
	local spf_e_3 =  GetIndexCodeCount(14, codes, 6, 3); --场次14的3的个数
	local spf_e_1 =  GetIndexCodeCount(14, codes, 6, 1); --场次14的1的个数
	local spf_e_0 =  GetIndexCodeCount(14, codes, 6, 0); --场次14的0的个数
	local sum_3 =   spf_1_3 + spf_2_3 + spf_3_3 + spf_4_3 + spf_5_3 + spf_6_3 + spf_7_3
	sum_3 = sum_3 + spf_8_3 + spf_9_3 + spf_a_3 + spf_b_3 + spf_c_3 + spf_d_3 + spf_e_3
	
	
	if(sum_3 > 3) then
		ret.code = 1;
		ret.info = trace_prefix .."sum_3>3]";
		trace(1, ret.info);
		return ret;
	end


	
--[[	
	if(spf_1_3 > 2) then
		ret.code = 1;
		ret.info = trace_prefix .."spf_1_3>2]";
		trace(1, ret.info);
		return ret;
	end
--]]	
	
	
	return ret;
end

