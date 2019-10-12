#version 430
layout(location=0) in vec3 inPosition;
layout(location=0) out vec4 outColor;

layout(std140, binding = 0) buffer VertexBuffer{
	mat4 matVP;
};

struct LightData{
	vec3 position;
	vec4 color;
	vec4 ambient;
};

layout(std140, binding = 1) buffer Light{
	LightData light;
};

void main(){
	
	vec3 worldPos = inPosition;
	vec3 dist = light.position - worldPos;
	float lightPower = dot(dist,dist);
	gl_Position = matVP * vec4(worldPos, 1.0);

	outColor = vec4(1.0);
	outColor.rgb *= light.color.rgb / lightPower;
	outColor.rgb += light.ambient.rgb;
}