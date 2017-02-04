#version 330

const int maxLights = 102;
const float SHADOW_OFFSET = 0.0001f;

uniform int zFail;
uniform int nonClosed;
uniform int lightIndex;

layout (shared) uniform LightBlock
{
	int numLights;
	vec4 position[maxLights];
	vec4 direction[maxLights];
	vec4 ambient;
	vec4 diffuse[maxLights];
	vec4 specular[maxLights];
	vec3 attenuation[maxLights];
	float cosCutoffAngle[maxLights];
	float secondaryCutoffAngle[maxLights];
};

layout (shared) uniform MatrixBlock
{
	mat4 modelMat;
	mat4 viewMat;
	mat4 modelViewMat;
	mat4 projectionMat;
	mat4 mvpMat;
	mat3 normalMat;
	vec3 eyePos;
};

uniform int thingy;

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 24) out;

in Vectors
{
	vec3 vertexWorldPosition;
	vec3 vertexWorldNormal;
} in_Vectors[];

vec3 worldPositions[6];
vec3 worldNormals[6];

vec3 innerEdges[3], outerEdges[6];
void calculateEdges();
vec3 calculateAveragePosition(vec3 position1, vec3 position2, vec3 position3);
void extrudeEdgeToInfinity(vec3 startPos, vec3 endPos, vec3 origin);
vec4 extrudedToInfinity(vec3 position, vec3 origin);
void emitTriangle(vec4 vert1, vec4 vert2, vec4 vert3);

void main()
{
	vec3 e1 = in_Vectors[2].vertexWorldPosition - in_Vectors[0].vertexWorldPosition;
	vec3 e2 = in_Vectors[4].vertexWorldPosition - in_Vectors[0].vertexWorldPosition;
	vec3 e3 = in_Vectors[1].vertexWorldPosition - in_Vectors[0].vertexWorldPosition;
	vec3 e4 = in_Vectors[3].vertexWorldPosition - in_Vectors[2].vertexWorldPosition;
	vec3 e5 = in_Vectors[4].vertexWorldPosition - in_Vectors[2].vertexWorldPosition;
	vec3 e6 = in_Vectors[5].vertexWorldPosition - in_Vectors[0].vertexWorldPosition;

	vec3 normal = cross(e1,e2);
	vec3 lightDir = position[lightIndex].xyz - in_Vectors[0].vertexWorldPosition;

	if (dot(normal, lightDir) > 0) 
	{

		normal = cross(e3,e1);
		
		if(nonClosed != 0)
		{
			if(in_Vectors[1].vertexWorldPosition.x < 1e-3 || in_Vectors[1].vertexWorldPosition.y < 1e-3 || in_Vectors[1].vertexWorldPosition.z < 1e-3)
			{
				extrudeEdgeToInfinity(in_Vectors[0].vertexWorldPosition, in_Vectors[2].vertexWorldPosition, position[lightIndex].xyz);
			}
		}
		else if (dot(normal, lightDir) <= 0) 
		{
				extrudeEdgeToInfinity(in_Vectors[0].vertexWorldPosition, in_Vectors[2].vertexWorldPosition, position[lightIndex].xyz);
		}

		normal = cross(e4,e5);
		lightDir = position[lightIndex].xyz - in_Vectors[2].vertexWorldPosition;
		
		if(nonClosed != 0)
		{
			if(in_Vectors[3].vertexWorldPosition.x < 1e-3 || in_Vectors[3].vertexWorldPosition.y < 1e-3 || in_Vectors[3].vertexWorldPosition.z < 1e-3)
			{
				extrudeEdgeToInfinity(in_Vectors[2].vertexWorldPosition, in_Vectors[4].vertexWorldPosition, position[lightIndex].xyz);
			}
		}
		else if (dot(normal, lightDir) <=0) 
		{
				extrudeEdgeToInfinity(in_Vectors[2].vertexWorldPosition, in_Vectors[4].vertexWorldPosition, position[lightIndex].xyz);
		}

		normal = cross(e2,e6);
		lightDir = position[lightIndex].xyz - in_Vectors[4].vertexWorldPosition;
		
		if(nonClosed != 0)
		{
			if(in_Vectors[5].vertexWorldPosition.x < 1e-3 || in_Vectors[5].vertexWorldPosition.y < 1e-3 || in_Vectors[5].vertexWorldPosition.z < 1e-3)
			{
				extrudeEdgeToInfinity(in_Vectors[4].vertexWorldPosition, in_Vectors[0].vertexWorldPosition, position[lightIndex].xyz);
			}
		}
		else if (dot(normal, lightDir) <= 0) 
		{
				extrudeEdgeToInfinity(in_Vectors[4].vertexWorldPosition, in_Vectors[0].vertexWorldPosition, position[lightIndex].xyz);
        }
		
		if(zFail != 0)
		{
			//Now we close the generated volume by creating the front and back caps for it.
			//Front cap
			vec3 direction1 = normalize(in_Vectors[0].vertexWorldPosition - position[lightIndex].xyz);
			vec3 direction2 = normalize(in_Vectors[2].vertexWorldPosition - position[lightIndex].xyz);
			vec3 direction3 = normalize(in_Vectors[4].vertexWorldPosition - position[lightIndex].xyz);
			emitTriangle(	vec4(in_Vectors[0].vertexWorldPosition + direction1*SHADOW_OFFSET, 1.0f), 
							vec4(in_Vectors[2].vertexWorldPosition + direction2*SHADOW_OFFSET, 1.0f), 
							vec4(in_Vectors[4].vertexWorldPosition + direction3*SHADOW_OFFSET, 1.0f)
						);
			EndPrimitive();
		}
    }
	else if(zFail != 0)
	{
			//Back cap
			vec4 position1 = extrudedToInfinity(in_Vectors[0].vertexWorldPosition, position[lightIndex].xyz);
			vec4 position2 = extrudedToInfinity(in_Vectors[2].vertexWorldPosition, position[lightIndex].xyz);
			vec4 position3 = extrudedToInfinity(in_Vectors[4].vertexWorldPosition, position[lightIndex].xyz);
			emitTriangle(position1, position2, position3);
			EndPrimitive();
	}
}

void calculateEdges()
{
	//Inner edges
	innerEdges[0] = worldPositions[2] - worldPositions[0];
	innerEdges[1] = worldPositions[5] - worldPositions[2];
	innerEdges[2] = worldPositions[0] - worldPositions[5];

	outerEdges[0] = worldPositions[1] - worldPositions[0];
	outerEdges[1] = worldPositions[2] - worldPositions[1];
	outerEdges[2] = worldPositions[3] - worldPositions[2];
	outerEdges[3] = worldPositions[4] - worldPositions[3];
	outerEdges[4] = worldPositions[5] - worldPositions[4];				
	outerEdges[5] = worldPositions[0] - worldPositions[5]; 
}																	

vec3 calculateAveragePosition(vec3 position1, vec3 position2, vec3 position3)
{
	return (position1 + position2 + position3) / 3.0f;
}

void extrudeEdgeToInfinity(vec3 startPos, vec3 endPos, vec3 origin)
{
	vec3 directionStart = normalize(startPos - origin);
	vec3 directionEnd = normalize(endPos - origin);

	gl_Position = projectionMat * viewMat * vec4(startPos + directionStart * SHADOW_OFFSET, 1.0f);
	EmitVertex();
	gl_Position = projectionMat * viewMat * vec4(directionStart, 0.0f);
	EmitVertex();
	gl_Position = projectionMat * viewMat * vec4(directionEnd, 0.0f);
	EmitVertex();
	EndPrimitive();
	
	gl_Position = projectionMat * viewMat * vec4(startPos + directionStart * SHADOW_OFFSET, 1.0f);
	EmitVertex();
	gl_Position = projectionMat * viewMat * vec4(directionEnd, 0.0f);
	EmitVertex();
	gl_Position = projectionMat * viewMat * vec4(endPos + directionEnd * SHADOW_OFFSET, 1.0f);
	EmitVertex();
	EndPrimitive();
}

vec4 extrudedToInfinity(vec3 position, vec3 origin)
{
	vec3 direction = position - origin;
	return vec4(normalize(direction), 0.0f);
}

void emitTriangle(vec4 vert1, vec4 vert2, vec4 vert3)
{
	gl_Position = projectionMat * viewMat * vert1;
	EmitVertex();
	gl_Position = projectionMat * viewMat * vert2;
	EmitVertex();
	gl_Position = projectionMat * viewMat * vert3;
	EmitVertex();
}
