#pragma once

#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

#include "基礎.h"
#include "樣本.h"

namespace 番荔枝::特征編碼
{
	class 類別_預測器
	{
	public:
		static void 預測(
			std::vector<std::tuple<std::string, std::shared_ptr<std::vector<std::string>>>>& 預測向量
			, const std::vector<std::shared_ptr<類別_樣本>>& 查詢樣本向量
			, const std::vector<std::shared_ptr<類別_樣本>>& 畫廊樣本向量
			, const std::vector<int>& 欄向量
		)
		{
			const auto 執行緒數 = 28;
			std::thread 執行緒陣列[執行緒數];
			std::vector<std::tuple<std::string, std::shared_ptr<std::vector<std::string>>>> 預測向量陣列[執行緒數];
			for (auto 子 = 0; 子 < 執行緒數; 子++)
			{
				執行緒陣列[子] = std::thread([查詢樣本向量, 畫廊樣本向量, 欄向量](auto 執行緒序号, auto 預測向量陣列)
					{
						auto 檔案名相似度元組陣列 = new std::tuple<std::string, double>[畫廊樣本向量.size()];
						for (auto 子 = size_t(執行緒序号); 子 < 查詢樣本向量.size(); 子 += 執行緒數)
						{
							if (子 > 2048)
								break;

							if (子 % 1024 == 0)
								std::cout << 取得時間() << "\t已預測" << 子 << "箇樣本......" << std::endl;

							auto& 樣本 = *查詢樣本向量[子].get();

							for (auto 丑 = size_t(0); 丑 < 畫廊樣本向量.size(); 丑++)
							{
								auto& 畫廊樣本 = *畫廊樣本向量[丑].get();
								auto 相似度 = 計算相似度(樣本, 畫廊樣本, 欄向量);

								檔案名相似度元組陣列[丑] = std::tuple<std::string, double>(畫廊樣本.檔案名, 相似度);
							}
							std::sort(檔案名相似度元組陣列, 檔案名相似度元組陣列 + 畫廊樣本向量.size(), [](const auto& 子元組, const auto& 丑元組) { return std::get<1>(子元組) > std::get<1>(丑元組); });

							auto 預測向量指針 = std::make_shared<std::vector<std::string>>();
							for (auto 丑 = 0; 丑 < 100; 丑++)
								預測向量指針->push_back(std::get<0>(檔案名相似度元組陣列[丑]));
							預測向量陣列[執行緒序号].push_back(std::tuple<std::string, std::shared_ptr<std::vector<std::string>>>(樣本.檔案名, 預測向量指針));
						}
						delete[] 檔案名相似度元組陣列;
					}, 子, 預測向量陣列);
			}

			for (auto 子 = 0; 子 < 執行緒數; 子++)
			{
				執行緒陣列[子].join();
				for (auto 丑 : 預測向量陣列[子])
					預測向量.push_back(丑);
			}
		}

	private:
		static double 計算相似度(const 類別_樣本& 子樣本, const 類別_樣本& 丑樣本, const std::vector<int>& 欄向量)
		{
			auto 相似度分子 = 0.0;
			for (auto 子 : 欄向量)
				相似度分子 += 子樣本.特征[子] * 丑樣本.特征[子];

			return 相似度分子 / sqrt(子樣本.特征平方和 * 丑樣本.特征平方和);
		}

		//static double 計算相似度(const 類別_樣本& 子樣本, const 類別_樣本& 丑樣本, const std::vector<int>& 欄向量)
		//{
		//	auto 距離 = 0.0;
		//	for (auto 子 : 欄向量)
		//	{
		//		auto 差 = 子樣本.特征[子] - 丑樣本.特征[子];
		//		距離 += 差 * 差;
		//	}

		//	return 1 / (1 + 距離);
		//
	};
}
