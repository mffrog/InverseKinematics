#include "InverseKinematics.h"
namespace IK {
	InverseKinematics::~InverseKinematics() {
		for (auto& e : effectors) {
			delete e;
		}
	}
	Effector* InverseKinematics::AddEffector() {
		Effector* e = new Effector();
		effectors.push_back(e);
		return e;
	}
	Effector* InverseKinematics::GetEffector(size_t index) {
		return effectors[index];
	}

	void InverseKinematics::Fix(int loop, float accuracy) {
		for (int i = 0; i < loop; ++i) {
			Solve(accuracy);
		}
	}

	void InverseKinematics::Solve(float accuracy) {
		const mff::Vector3<float> forward(0.0f,1.0f,0.0f);

		Effector* movedEffector = nullptr;
		//動いたエフェクターを探す
		for (auto& e : effectors) {
			if (e->GetVelocity().LengthSq() > 0.0f) {
				movedEffector = e;
			}
		}

		if (!movedEffector) {
			return;
		}

		//とりあえず動かないことにする
		Effector* rootEffector = movedEffector;
		std::vector<Effector*> relationEffectors;
		while (rootEffector->GetParent()){
			rootEffector = rootEffector->GetParent();
			relationEffectors.push_back(rootEffector);
		}

		using vec3 = mff::Vector3<float>;
		using quat = mff::Quaternion<float>;
		vec3 targetVelocity = movedEffector->GetVelocity();
		float targetVelLen = targetVelocity.Length();
		vec3 target = movedEffector->GetMoveTarget();
		vec3 tip = movedEffector->GetPosition();

		struct EffData {
			vec3 v;
			float v_length;
			float d;
			Effector* e;
			float weight;
		};

		std::vector<EffData> data;
		float sum = 0.0f;
		for (auto& e : relationEffectors) {
			EffData d;
			d.e = e;
			vec3 sub = tip - e->GetPosition();
			d.d = sub.Length();
			d.v = mff::Normalize(cross(vec3(0.0f, 0.0f, 1.0f), sub))* e->GetWeight();
			sum = dot(d.v, d.v);
			data.push_back(d);
		}
		sum += accuracy;
		//printf("sum %f\n",sum);

		sum = 1.0f / sum;
		for (auto& d : data) {
			float theta = sum * dot(d.v,targetVelocity);
			d.e->SetQuat(mff::MakeQuaternion(vec3(0.0f,0.0f,1.0f), theta) * d.e->GetQuaternion());
		}
		for (int i = relationEffectors.size() - 1; i >= 0; --i) {
			if (Effector* parent = relationEffectors[i]->GetParent()) {
				relationEffectors[i]->SetPosition(parent->GetPosition() + parent->GetWorldQuaternion().rotate(vec3(0.0f, 4.0f, 0.0f)));
			}
		}
		Effector* parent = movedEffector->GetParent();
		movedEffector->SetPosition(parent->GetPosition() + parent->GetWorldQuaternion().rotate(vec3(0.0f, 4.0f, 0.0f)));
		//printf("gosa %f\n", (movedEffector->GetPosition() - target).Length());
	}
}
