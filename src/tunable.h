/*
 * Stormphrax, a UCI chess engine
 * Copyright (C) 2024 Ciekce
 *
 * Stormphrax is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stormphrax is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stormphrax. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "types.h"

#include <string>
#include <array>
#include <functional>

#include "util/range.h"
#include "util/multi_array.h"

#ifndef SP_EXTERNAL_TUNE
	#define SP_EXTERNAL_TUNE 0
#endif

namespace stormphrax::tunable
{
	auto init() -> void;

	// [noisy][depth][legal moves]
	extern util::MultiArray<i32, 2, 256, 256> g_lmrTable;

	auto updateQuietLmrTable() -> void;
	auto updateNoisyLmrTable() -> void;

	// [improving][clamped depth]
	extern util::MultiArray<i32, 2, 16> g_lmpTable;
	auto updateLmpTable() -> void;

#define SP_TUNABLE_ASSERTS(Default, Min, Max, Step) \
	static_assert(Default >= Min); \
	static_assert(Default <= Max); \
	static_assert(Min < Max); \
	static_assert(Min + Step <= Max); \
	static_assert(Step >= 0.5);

#if SP_EXTERNAL_TUNE
	struct TunableParam
	{
		std::string name;
		std::string lowerName;
		i32 defaultValue;
		i32 value;
		util::Range<i32> range;
		f64 step;
		std::function<void()> callback;
	};

	auto addTunableParam(const std::string &name, i32 value,
		i32 min, i32 max, f64 step, std::function<void()> callback) -> TunableParam &;

	#define SP_TUNABLE_PARAM(Name, Default, Min, Max, Step) \
	    SP_TUNABLE_ASSERTS(Default, Min, Max, Step) \
		inline TunableParam &param_##Name = addTunableParam(#Name, Default, Min, Max, Step, nullptr); \
		inline auto Name() { return param_##Name.value; }

	#define SP_TUNABLE_PARAM_CALLBACK(Name, Default, Min, Max, Step, Callback) \
	    SP_TUNABLE_ASSERTS(Default, Min, Max, Step) \
		inline TunableParam &param_##Name = addTunableParam(#Name, Default, Min, Max, Step, Callback); \
		inline auto Name() { return param_##Name.value; }
#else
	#define SP_TUNABLE_PARAM(Name, Default, Min, Max, Step) \
		SP_TUNABLE_ASSERTS(Default, Min, Max, Step) \
		constexpr auto Name() -> i32 { return Default; }
	#define SP_TUNABLE_PARAM_CALLBACK(Name, Default, Min, Max, Step, Callback) \
		SP_TUNABLE_PARAM(Name, Default, Min, Max, Step)
#endif

	SP_TUNABLE_PARAM(defaultMovesToGo, 19, 12, 40, 1)
	SP_TUNABLE_PARAM(incrementScale, 83, 50, 100, 5)
	SP_TUNABLE_PARAM(softTimeScale, 68, 50, 100, 5)
	SP_TUNABLE_PARAM(hardTimeScale, 56, 20, 100, 5)

	SP_TUNABLE_PARAM(nodeTmBase, 263, 150, 300, 10)
	SP_TUNABLE_PARAM(nodeTmScale, 170, 100, 250, 10)
	SP_TUNABLE_PARAM(nodeTmScaleMin, 102, 1, 1000, 100)

	SP_TUNABLE_PARAM(bmStabilityTmMin, 75, 40, 100, 3)
	SP_TUNABLE_PARAM(bmStabilityTmMax, 240, 120, 1000, 40)
	SP_TUNABLE_PARAM(bmStabilityTmScale, 911, 200, 1500, 65)
	SP_TUNABLE_PARAM(bmStabilityTmOffset, 80, 50, 200, 8)
	SP_TUNABLE_PARAM(bmStabilityTmPower, -270, -400, -150, 13)

	SP_TUNABLE_PARAM(scoreTrendTmMin, 60, 40, 100, 3)
	SP_TUNABLE_PARAM(scoreTrendTmMax, 170, 120, 1000, 40)
	SP_TUNABLE_PARAM(scoreTrendTmScoreScale, 458, 10, 1000, 50)
	SP_TUNABLE_PARAM(scoreTrendTmStretch, 80, 10, 200, 10)
	SP_TUNABLE_PARAM(scoreTrendTmScale, 41, 10, 90, 4)
	SP_TUNABLE_PARAM(scoreTrendTmPositiveScale, 109, 50, 200, 7.5)
	SP_TUNABLE_PARAM(scoreTrendTmNegativeScale, 104, 50, 200, 7.5)

	SP_TUNABLE_PARAM(timeScaleMin, 70, 1, 1000, 100)

	SP_TUNABLE_PARAM(pawnCorrhistWeight, 133, 32, 384, 18)
	SP_TUNABLE_PARAM(stmNonPawnCorrhistWeight, 142, 32, 384, 18)
	SP_TUNABLE_PARAM(nstmNonPawnCorrhistWeight, 142, 32, 384, 18)
	SP_TUNABLE_PARAM(majorCorrhistWeight, 129, 32, 384, 18)
	SP_TUNABLE_PARAM(contCorrhistWeight, 134, 32, 384, 18)

	SP_TUNABLE_PARAM(minAspDepth, 3, 1, 10, 1)
	SP_TUNABLE_PARAM(initialAspWindow, 16, 4, 50, 4)
	SP_TUNABLE_PARAM(aspWideningFactor, 17, 1, 24, 1)

	SP_TUNABLE_PARAM(maxAspFailHighReduction, 3, 1, 5, 1)

	SP_TUNABLE_PARAM(goodNoisySeeOffset, 15, -384, 384, 40)

	SP_TUNABLE_PARAM(ttReplacementDepthOffset, 4, 0, 6, 0.5)
	SP_TUNABLE_PARAM(ttReplacementPvOffset, 2, 0, 6, 0.5)

	SP_TUNABLE_PARAM(maxTtNonCutoffExtDepth, 6, 0, 12, 0.5)

	SP_TUNABLE_PARAM(minIirDepth, 3, 3, 6, 0.5)
	SP_TUNABLE_PARAM(iirBadEntryDepthOffset, 3, 2, 6, 0.5)

	SP_TUNABLE_PARAM(maxRfpDepth, 6, 4, 12, 0.5)
	SP_TUNABLE_PARAM(rfpMargin, 71, 25, 150, 5)

	SP_TUNABLE_PARAM(maxRazoringDepth, 4, 2, 6, 0.5)
	SP_TUNABLE_PARAM(razoringMargin, 315, 100, 350, 40)

	SP_TUNABLE_PARAM(minNmpDepth, 4, 3, 8, 0.5)
	SP_TUNABLE_PARAM(nmpBaseReduction, 4, 2, 5, 0.5)
	SP_TUNABLE_PARAM(nmpDepthReductionDiv, 5, 1, 8, 1)
	SP_TUNABLE_PARAM(nmpEvalReductionScale, 206, 50, 300, 25)
	SP_TUNABLE_PARAM(maxNmpEvalReduction, 2, 2, 5, 1)

	SP_TUNABLE_PARAM(minProbcutDepth, 7, 4, 8, 0.5)
	SP_TUNABLE_PARAM(probcutMargin, 303, 150, 400, 13)
	SP_TUNABLE_PARAM(probcutReduction, 3, 2, 5, 0.5)
	SP_TUNABLE_PARAM(probcutSeeScale, 17, 6, 24, 1)

	SP_TUNABLE_PARAM_CALLBACK(lmpBaseMoves, 3, 2, 5, 0.5, updateLmpTable)

	SP_TUNABLE_PARAM(maxFpDepth, 8, 4, 12, 0.5)
	SP_TUNABLE_PARAM(fpMargin, 261, 120, 350, 45)
	SP_TUNABLE_PARAM(fpScale, 68, 40, 80, 8)

	SP_TUNABLE_PARAM(maxHistoryPruningDepth, 5, 2, 8, 0.5)
	SP_TUNABLE_PARAM(historyPruningMargin, -2314, -4000, -1000, 175)
	SP_TUNABLE_PARAM(historyPruningOffset, -1157, -4000, 4000, 400)

	SP_TUNABLE_PARAM(seePruningThresholdQuiet, -16, -80, -15, 12)
	SP_TUNABLE_PARAM(seePruningThresholdNoisy, -112, -120, -40, 20)

	SP_TUNABLE_PARAM(minSeDepth, 8, 4, 10, 0.5)
	SP_TUNABLE_PARAM(seTtDepthMargin, 5, 2, 6, 1)
	SP_TUNABLE_PARAM(sBetaMargin, 14, 4, 64, 12)

	SP_TUNABLE_PARAM(multiExtLimit, 9, 4, 24, 4)

	SP_TUNABLE_PARAM(doubleExtMargin, 11, 0, 32, 5)
	SP_TUNABLE_PARAM(tripleExtMargin, 105, 10, 150, 7)

	SP_TUNABLE_PARAM(minLmrDepth, 2, 2, 5, 1)
	SP_TUNABLE_PARAM(lmrMinMovesRoot, 5, 0, 5, 1)
	SP_TUNABLE_PARAM(lmrMinMovesPv, 4, 0, 5, 1)
	SP_TUNABLE_PARAM(lmrMinMovesNonPv, 2, 0, 5, 1)

	SP_TUNABLE_PARAM_CALLBACK(quietLmrBase, 83, 50, 120, 15, updateQuietLmrTable)
	SP_TUNABLE_PARAM_CALLBACK(quietLmrDivisor, 218, 100, 300, 10, updateQuietLmrTable)

	SP_TUNABLE_PARAM_CALLBACK(noisyLmrBase, -12, -50, 75, 10, updateNoisyLmrTable)
	SP_TUNABLE_PARAM_CALLBACK(noisyLmrDivisor, 248, 150, 350, 10, updateNoisyLmrTable)

	SP_TUNABLE_PARAM(lmrNonPvReductionScale, 131, 32, 384, 12)
	SP_TUNABLE_PARAM(lmrTtpvReductionScale, 130, 32, 384, 12)
	SP_TUNABLE_PARAM(lmrImprovingReductionScale, 148, 32, 384, 12)
	SP_TUNABLE_PARAM(lmrCheckReductionScale, 111, 32, 384, 12)
	SP_TUNABLE_PARAM(lmrCutnodeReductionScale, 257, 32, 384, 12)

	SP_TUNABLE_PARAM(lmrHistoryDivisor, 10835, 4096, 16384, 650)

	SP_TUNABLE_PARAM(lmrDeeperBase, 38, 20, 100, 6)
	SP_TUNABLE_PARAM(lmrDeeperScale, 4, 3, 12, 1)

	SP_TUNABLE_PARAM(maxHistory, 15769, 8192, 32768, 256)

	SP_TUNABLE_PARAM(maxHistoryBonus, 2576, 1024, 4096, 256)
	SP_TUNABLE_PARAM(historyBonusDepthScale, 280, 128, 512, 32)
	SP_TUNABLE_PARAM(historyBonusOffset, 432, 128, 768, 64)

	SP_TUNABLE_PARAM(maxHistoryPenalty, 1239, 1024, 4096, 256)
	SP_TUNABLE_PARAM(historyPenaltyDepthScale, 343, 128, 512, 32)
	SP_TUNABLE_PARAM(historyPenaltyOffset, 161, 128, 768, 64)

	SP_TUNABLE_PARAM(qsearchFpMargin, 135, 50, 400, 17)
	SP_TUNABLE_PARAM(qsearchMaxMoves, 2, 1, 5, 0.5)
	SP_TUNABLE_PARAM(qsearchSeeThreshold, -97, -2000, 200, 100)

#undef SP_TUNABLE_PARAM
#undef SP_TUNABLE_PARAM_CALLBACK
#undef SP_TUNABLE_ASSERTS
}
