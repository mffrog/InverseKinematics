#pragma once
#include "Effector/Effector.h"
#include <vector>

namespace IK {
	class InverseKinematics {
	public:
		~InverseKinematics();
		Effector* AddEffector();
		Effector* GetEffector(size_t index);
		void Fix(int loop, float accuracy);
	private:
		void Solve(float accuracy);
		std::vector<Effector*> effectors;
	};
} // namespace IK