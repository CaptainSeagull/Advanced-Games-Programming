#version 330

const int maxLights = 102;
const int lightIndex = 0;

layout (shared) uniform LightBlock
{
	int numLights;
	vec4 position[maxLights];
	vec4 direction[maxLights];
	vec4 ambient[maxLights];
	vec4 diffuse[maxLights];
	vec4 specular[maxLights];
	vec3 attenuation[maxLights];
	float cosCutoffAngle[maxLights];
	float secondaryCutoffAngle[maxLights];
};

layout (triangles_adjacency) in;
layout (line_strip, max_vertices = 6) out;

in Vectors
{
	vec3 vertexPosition;
	vec3 vertexToEye;
	vec3 normalVec;
} in_Vectors[];

void emitLine(int startIndex, int endIndex)
{
	gl_Position = gl_in[startIndex].gl_Position;
	EmitVertex();

	gl_Position = gl_in[endIndex].gl_Position;
	EmitVertex();

	EndPrimitive();
}

void main()
{
	vec3 e1 = in_Vectors[2].vertexPosition - in_Vectors[0].vertexPosition;
	vec3 e2 = in_Vectors[4].vertexPosition - in_Vectors[0].vertexPosition;
	vec3 e3 = in_Vectors[1].vertexPosition - in_Vectors[0].vertexPosition;
	vec3 e4 = in_Vectors[3].vertexPosition - in_Vectors[2].vertexPosition;
	vec3 e5 = in_Vectors[4].vertexPosition - in_Vectors[2].vertexPosition;
	vec3 e6 = in_Vectors[5].vertexPosition - in_Vectors[0].vertexPosition;

    vec3 normal = cross(e1,e2);
    vec3 lightDir = position[lightIndex].xyz - in_Vectors[0].vertexPosition;

    if (dot(normal, lightDir) > 0.00001) {

        normal = cross(e3,e1);

        if (dot(normal, lightDir) <= 0) {
            emitLine(0, 2);
        }

        normal = cross(e4,e5);
        lightDir = position[lightIndex].xyz - in_Vectors[2].vertexPosition;

        if (dot(normal, lightDir) <=0) {
            emitLine(2, 4);
        }

        normal = cross(e2,e6);
        lightDir = position[lightIndex].xyz - in_Vectors[4].vertexPosition;

        if (dot(normal, lightDir) <= 0) {
            emitLine(4, 0);
        }
    }
}