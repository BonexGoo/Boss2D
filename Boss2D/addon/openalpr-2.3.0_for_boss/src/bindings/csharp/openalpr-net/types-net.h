#pragma once

#include "alpr.h" // alpr
#include BOSS_OPENALPR_U_config_h //original-code:"config.h" // alpr

using namespace alpr;

namespace openalprnet
{
	namespace types
	{
		public enum class AlprDetectorTypeNet : int {
			DetectorLbpCpu = alpr::DETECTOR_LBP_CPU,
			DetectorLbpGpu = alpr::DETECTOR_LBP_GPU,
			DetectorLbpMorphCpu = alpr::DETECTOR_MORPH_CPU,
			DetectorLbpOpencl = alpr::DETECTOR_LBP_OPENCL
		};
	}
}