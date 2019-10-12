#include "Effector.h"

namespace IK {

	Effector::Effector() {
		for (int i = 0; i < ConstraintType_Num; ++i) {
			constraint[i] = false;
		}
	}

	void Effector::SetConstraint(ConstraintType type) {
		switch (type) {
		case ConstraintType_Position:
			constraint[ConstraintType_Position_X] = true;
			constraint[ConstraintType_Position_Y] = true;
			constraint[ConstraintType_Position_Z] = true;
			break;
		case ConstraintType_Rotation:
			constraint[ConstraintType_Rotation_X] = true;
			constraint[ConstraintType_Rotation_Y] = true;
			constraint[ConstraintType_Rotation_Z] = true;
			break;
		default:
			constraint[type] = true;
		}
	}

} // namespace IK