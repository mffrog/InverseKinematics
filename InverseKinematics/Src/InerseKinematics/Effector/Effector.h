#pragma once
#include "../../Math/Vector/Vector3.h"
#include "../../Math/Quaternion/Quaternion.h"
namespace IK {
	enum ConstraintType {
		ConstraintType_Position_X,
		ConstraintType_Position_Y,
		ConstraintType_Position_Z,

		ConstraintType_Rotation_X,
		ConstraintType_Rotation_Y,
		ConstraintType_Rotation_Z,

		ConstraintType_Position,
		ConstraintType_Rotation,

		ConstraintType_Num,
	};
	class Effector {
	public:
		Effector();
		void MoveTo(const mff::Vector3<float>& target) { 
			moved = true;
			movePosition = target;
		}
		void SetPosition(const mff::Vector3<float>& pos) { position = pos;}
		void SetQuat(const mff::Quaternion<float>& q) { quat = q; }
		mff::Vector3<float> GetPosition() const { return position; }
		mff::Quaternion<float> GetQuaternion() const { return quat; }
		mff::Quaternion<float> GetWorldQuaternion() const { return parent ? parent->GetWorldQuaternion() * quat : quat; }
		void SetConstraint(ConstraintType type);
		mff::Vector3<float> GetVelocity() const { 
			return (moved) ? movePosition - position : mff::Vector3<float>(0.0f); }
		void Solved() {
			moved = false;
		}
		void SetParent(Effector* e) { parent = e; }
		Effector* GetParent() const { return parent; }
		mff::Vector3<float> GetMoveTarget() const { return movePosition; }
		void SetWeight(float w) { weight = w; }
		float GetWeight() const { return weight; }
	private:
		bool constraint[ConstraintType_Num];
		mff::Vector3<float> movePosition;
		mff::Vector3<float> position;
		mff::Vector3<float> top;
		mff::Quaternion<float> quat;
		Effector* parent = nullptr;
		bool moved = false;
		float weight = 1.0f;
	};

} //namespace IK