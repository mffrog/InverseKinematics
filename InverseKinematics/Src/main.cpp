#include <iostream>
#include "Renderer/Renderer.h"
#include "Math/Vector/Vector2.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector4.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/MathFunctions.h"
#include "Math/Matrix/Transformation.h"

#include "InerseKinematics/InverseKinematics.h"

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

using vec3 = mff::Vector3<float>;
using vec4 = mff::Vector4<float>;
using quat = mff::Quaternion<float>;
struct Vertex {
	vec3 position;
};

struct VertexBuffer {
	mff::Matrix4x4<float> vertexBuffer;
};

GLuint indices[] = {
	0,1,2,
};

const Vertex vertices[] = {
	{ {  0.0f, 0.5f,0.0f } },
	{ { -0.5f,-0.5f,0.0f } },
	{ {  0.5f,-0.5f,0.0f } },
};

const Vertex quadrangular_pyramid_vertices[] = {
	{ { 0.0f,1.0f, 0.0f } },
	{ {-1.0f,0.0f, 1.0f } },
	{ { 1.0f,0.0f, 1.0f } },
	{ { 1.0f,0.0f,-1.0f } },
	{ {-1.0f,0.0f,-1.0f } },
};

const GLuint quadrangular_pyramid_indices[] = {
	0,1,2,
	0,2,3,
	0,3,4,
	0,4,1,
	1,3,2,
	1,4,3
};

struct LightData {
	vec3 position;
	vec4 color;
	vec4 ambient;
};

struct Camera {
	vec3 position;
	vec3 forward;
	vec3 up;
};

struct Bone {
	mff::Matrix4x4<float> matrix;
	vec4 len;
};

#define GetInput(k) flags[IK::Key::k]

struct Plane {
	vec3 normal;
	vec3 point;
};

vec3 GetRightVector(const vec3& direction) {
	return mff::Normalize(mff::cross(direction, vec3(0.0f, 1.0f, 0.0f)));
}

int main() {
	const int Width = 960;
	const int Height = 720;
	const char* title = "window";


	bool flags[IK::Key::KeyNum];
	for (int i = 0; i < IK::Key::KeyNum; ++i) {
		flags[i] = false;
	}
	Camera camera;
	camera.position = vec3(0.0f, 0.0f, 10.0f);
	camera.forward = vec3(.0f, .0f, -1.0f);
	camera.up = vec3(0.0f, 1.0f, 0.0f);

	mff::Vector2<double> mousePos;
	mff::Vector2<double> mouseCurPos;
	mff::Vector2<double> mouseVelocity(0, 0);

	float fov = 45.0f;
	float aspect = static_cast<float>(Width) / static_cast<float>(Height);
	float horizonRotAngle = -atanf(tanf(fov * 0.5f) * aspect) * 2.0f;
	float verticalRotAngle = -ToRadian(fov);
	float velocity = 0.5f;

	bool mousePosInit = false;
	bool cursorVisible = true;
	bool barrier = false;
	
	float boneLen = 4.0f;
	IK::InverseKinematics ik;
	IK::Effector* root = ik.AddEffector();
	IK::Effector* second = ik.AddEffector();
	IK::Effector* third = ik.AddEffector();
	IK::Effector* forth = ik.AddEffector();
	IK::Effector* fifth = ik.AddEffector();

	second->SetPosition(vec3(0.0f, boneLen, 0.0));
	third->SetPosition(vec3(0.0f, boneLen * 2.0f, 0.0f));
	forth->SetPosition(vec3(0.0f, boneLen * 4.0f, 0.0f));
	fifth->SetPosition(vec3(0.0f, boneLen * 6.0f, 0.0f));
	root->SetWeight(1.0f);
	second->SetWeight(1.0f);
	third->SetWeight(1.0f);
	forth->SetWeight(1.0f);
	fifth->SetWeight(1.0f);

	second->SetParent(root);
	third->SetParent(second);
	forth->SetParent(third);
	fifth->SetParent(forth);

	Bone bones[4];

	vec3 targetPosition;

	bool use_barrier = true;
	bool useIK = true;

	auto Input = [&] {
		if (!cursorVisible) {
			if (GetInput(W)) {
				camera.position += camera.forward * velocity;
			}
			if (GetInput(S)) {
				camera.position -= camera.forward * velocity;
			}
			if (GetInput(D)) {
				camera.position += GetRightVector(camera.forward) * velocity;
			}
			if (GetInput(A)) {
				camera.position -= GetRightVector(camera.forward) * velocity;
			}
			if (GetInput(Q)) {
				camera.position -= vec3(0.0f, velocity, 0.0f);
			}
			if (GetInput(E)) {
				camera.position += vec3(0.0f, velocity, 0.0f);
			}
		}
		if (GetInput(Esc)) {
			IK::Renderer::GetInstance().CloseWindow();
		}
		if (GetInput(Enter)) {
			if (use_barrier) {
				useIK = !useIK;
				use_barrier = false;
			}
		}
		else {
			use_barrier = true;
		}
		if (GetInput(Space)) {
			if (barrier) {
				cursorVisible = !cursorVisible;
				IK::Renderer::GetInstance().SetCursorVisible(cursorVisible);
				barrier = false;
			}
		}
		else {
			barrier = true;
		}
	};

	auto MouseFunc = [&] {
		if (!cursorVisible) {
			/*mouseVelocity = mouseCurPos - mousePos;
			quat horizon = mff::MakeQuaternion(vec3(0.0f, 1.0f, 0.0f), static_cast<float>(horizonRotAngle * mouseVelocity.x / Width));
			quat vertical = mff::MakeQuaternion(GetRightVector(camera.forward), static_cast<float>(verticalRotAngle * mouseVelocity.y / Height));
			camera.forward = vertical.rotate(camera.forward);
			camera.forward = horizon.rotate(camera.forward);
			mousePos = mff::Vector2<double>(Width * 0.5, Height * 0.5);
			mouseCurPos = mousePos;
			IK::Renderer::GetInstance().SetMousePos(Width * 0.5, Height * 0.5);*/
		}
		else {
			mousePos = mouseCurPos;
			auto tmp = mousePos / mff::Vector2<double>(Width,Height);
			tmp *= 2.0;
			tmp -= mff::Vector2<double>(1.0);
			tmp.y *= -1.0;
			double aspect = (double)Width / (double)Height;
			double tangent = tan(ToRadian(fov / 2.0)) * camera.position.z;
			targetPosition = vec3(tmp.x * aspect * tangent,tmp.y * tangent,0.0f);
			//targetPosition = vec3(8.0f, 0.0f, 0.0f);
		}
	};

	IK::Renderer& renderer = IK::Renderer::GetInstance();
	if (!renderer.Init(Width, Height, title)) {
		return 1;
	}

	auto Key = [&](IK::Key key, bool action) {
		if (key == IK::Key::Esc) {
			if (action) {
				IK::Renderer::GetInstance().CloseWindow();
			}
		}
		flags[key] = action;
	};
	auto mouse = [&](double x, double y) {
		mouseCurPos = mff::Vector2<double>(x, y);
		if (!mousePosInit) {
			mousePos = mouseCurPos;
			mousePosInit = true;
		}
	};

	renderer.SetKeyCallback(Key);
	renderer.SetMouseCallback(mouse);
	
	IK::Buffer quadrangular_pyramid_vbo;
	if (!quadrangular_pyramid_vbo.Init(sizeof(quadrangular_pyramid_vertices), IK::BufferType::Vertex, (void*)quadrangular_pyramid_vertices)) {
		return 1;
	}

	IK::Buffer quadrangular_pyramid_ibo;
	if (!quadrangular_pyramid_ibo.Init(sizeof(quadrangular_pyramid_indices), IK::BufferType::Index, (void*)quadrangular_pyramid_indices)) {
		return 1;
	}

	IK::VAO quadrangular_pyramid_vao;
	if (!quadrangular_pyramid_vao.Init(quadrangular_pyramid_vbo.GetId(), quadrangular_pyramid_ibo.GetId())) {
		return 1;
	}
	quadrangular_pyramid_vao.Bind();
	quadrangular_pyramid_vao.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position));


	IK::Shader shader;
	if (!shader.LoadShader("Res/Shader/VertexShader.vert", "Res/Shader/FragmentShader.frag")) {
		return 1;
	}
	
	IK::Shader boneShader;
	if (!boneShader.LoadShader("Res/Shader/BoneVertex.vert", "Res/Shader/FragmentShader.frag")) {
		return 1;
	}
	
	IK::ResourceBuffer vertexBuffer;
	if (!vertexBuffer.Init(sizeof(VertexBuffer), IK::BufferType::Generic)) {
		return 1;
	}

	IK::ResourceBuffer lightBuffer;
	if (!lightBuffer.Init(sizeof(LightData), IK::BufferType::Generic)) {
		return 1;
	}
	LightData light;
	light.color = vec4(1.0f) * 10.0f;
	light.ambient = vec4(0.2f);
	lightBuffer.Bind(1);

	IK::ResourceBuffer boneBuffer;
	if (!boneBuffer.Init(sizeof(bones), IK::BufferType::Generic)) {
		return 1;
	}
	boneBuffer.Bind(2);

	printf("bones size %d\n", sizeof(bones));

	while (!renderer.WindowShouldClose()) {
		Input();
		MouseFunc();
		mff::Matrix4x4<float> mat = mff::Perspective(ToRadian(45.0f), (float)Width / (float)Height, 0.1f, 1000.0f);
		mat = mat * mff::LookTo(camera.position, -camera.forward, camera.up);
		mat = mff::Transpose(mat);
		vertexBuffer.Update(&mat, sizeof(mat));
		vertexBuffer.Bind(0);

		light.position = camera.position;
		lightBuffer.Update(&light, sizeof(light));

		fifth->MoveTo(targetPosition);

		//second->SetPosition(vec3(0.0f, boneLen, 0.0));
		//third->SetPosition(vec3(0.0f, boneLen * 2.0f, 0.0f));

		if (useIK) {
			ik.Fix(100, 100.0f);
		}
		/*second->SetPosition(root->GetQuaternion().rotate(vec3(0.0f,boneLen,0.0f)));
		third->SetPosition(second->GetPosition() + second->GetWorldQuaternion().rotate(vec3(0.0f,boneLen,0.0f)));
		forth->SetPosition(third->GetPosition() + third->GetWorldQuaternion().rotate(vec3(0.0f, boneLen, 0.0f)));*/

		/*printf("second : ");
		print(second->GetPosition());
		printf("third : ");
		print(third->GetPosition());
		printf("target : ");
		print(targetPosition);
		printf("\n\n");*/
		bones[0].len.x = boneLen;
		bones[0].matrix = mff::ConvertToMat(root->GetQuaternion());
		vec3 bonePos = root->GetPosition();
		
		bones[0].matrix[0][3] = bonePos.x;
		bones[0].matrix[1][3] = bonePos.y;
		bones[0].matrix[2][3] = bonePos.z;
		bones[0].matrix = mff::Transpose(bones[0].matrix);

		bones[1].len.x = boneLen;
		bones[1].matrix = mff::ConvertToMat(second->GetWorldQuaternion());
		bonePos = second->GetPosition();
		bones[1].matrix[0][3] = bonePos.x;
		bones[1].matrix[1][3] = bonePos.y;
		bones[1].matrix[2][3] = bonePos.z;
		bones[1].matrix = mff::Transpose(bones[1].matrix);

		bones[2].len.x = boneLen;
		bones[2].matrix = mff::ConvertToMat(third->GetWorldQuaternion());
		bonePos = third->GetPosition();
		bones[2].matrix[0][3] = bonePos.x;
		bones[2].matrix[1][3] = bonePos.y;
		bones[2].matrix[2][3] = bonePos.z;
		bones[2].matrix = mff::Transpose(bones[2].matrix);

		bones[3].len.x = boneLen;
		bones[3].matrix = mff::ConvertToMat(forth->GetWorldQuaternion());
		bonePos = forth->GetPosition();
		bones[3].matrix[0][3] = bonePos.x;
		bones[3].matrix[1][3] = bonePos.y;
		bones[3].matrix[2][3] = bonePos.z;
		bones[3].matrix = mff::Transpose(bones[3].matrix);

		boneBuffer.Update(bones, sizeof(bones));

		renderer.ClearBuffer();

		//vao.Bind();
		//shader.Use();
		//glDrawElements(GL_TRIANGLES, ARRAY_SIZE(indices), GL_UNSIGNED_INT, 0);
		quadrangular_pyramid_vao.Bind();
		//glDrawElements(GL_TRIANGLES, ARRAY_SIZE(quadrangular_pyramid_indices), GL_UNSIGNED_INT, 0);

		boneShader.Use();
		glDrawElementsInstanced(GL_TRIANGLES, ARRAY_SIZE(quadrangular_pyramid_indices), GL_UNSIGNED_INT, 0, 4);

		renderer.SwapBuffers();
	}

	return 0;
}