#version 430
layout(location=0) in vec3 inPosition;
layout(location=0) out vec4 outColor;

struct Bone{
	mat4 matrix;
	vec4 len;
};

layout(std140, binding = 0) buffer VertexBuffer{
	mat4 matVP;
};

struct Light {
	vec3 position;
	vec4 color;
	vec4 ambient;
};

layout(std140, binding = 1) buffer LightData{
	Light light;
};

layout(std140, binding = 2) buffer BoneInstanceData{
	Bone bones[];
};

void main(){
	vec4 worldPos = vec4(inPosition, 1.0);
	worldPos.y *= bones[gl_InstanceID].len.x;
	worldPos = (bones[gl_InstanceID].matrix * worldPos);
	// worldPos.xyz += bones[gl_InstanceID].position.xyz;
	gl_Position = matVP * worldPos;
	
	vec3 dist = light.position - worldPos.xyz;
	float lightPower = dot(dist,dist);
	outColor = vec4(1.0);

	outColor.rgb *= (light.color.rgb / lightPower);	
	outColor.rgb += light.ambient.rgb;
}