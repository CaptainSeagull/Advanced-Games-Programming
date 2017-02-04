//Gouraud vertex shader
#version 330

struct materialStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

const int maxLights = 102;

vec3 lightToVert;
float lightToVertDistance;
vec3 normalVec;
vec3 reflection;
vec3 vertexToEye;
vec4 vertexPosition;

uniform materialStruct material;

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

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec2 in_TexCoord;

out VS_OUT
{
	vec4 colour;
	vec2 texCoord;
} vs_out;

vec4 calculateLightColour(int i)
{
	float attenuation = 1/(attenuation[i].x + attenuation[i].y*lightToVertDistance + 
						attenuation[i].z*lightToVertDistance*lightToVertDistance);
	
		//Calculate colour of light and object material
		//Ambient intensity
	vec4 ambientI = ambient * material.ambient;

		// Diffuse intensity
	vec4 diffuseI =  diffuse[i] * material.diffuse;
	diffuseI = diffuseI * max(dot(normalVec, -lightToVert), 0.0) * attenuation;

		// Specular intensity
	vec4 specularI = specular[i] * material.specular;
		//Specular Intensity = Ks*(R.V)^(alpha)*Is
	specularI = specularI * pow( max( dot( reflection, vertexToEye ), 0.0 ), material.shininess ) * attenuation;

	return ambientI + diffuseI + specularI;
}

vec4 calculateLight(int i)
{
		//Calculate the vector from the light source to the vertex in world coordinates. 
	lightToVert = vertexPosition.xyz - position[i].xyz;
		//Calculate the distance of the above vector. We do this before normalising or the length will always be one. 
	lightToVertDistance = length(lightToVert);
		//Normalise lightToVert to get the direction from the vertex to the light source.
	lightToVert = normalize(lightToVert);

		/*Calculate reflection light makes to the surface*/
	reflection = reflect(lightToVert, normalVec);

	if(cosCutoffAngle[i] == 0 && direction[i] == vec4(0.0, 0.0, 0.0, 0.0))
	{
		return calculateLightColour(i);
	}
	else
	{
			//Get dot product of spotlight direction and vertex vector. 
		float dotProduct = dot(lightToVert, normalize(direction[i].xyz));

		if(dotProduct > cosCutoffAngle[i])
		{
			return calculateLightColour(i);
		}
		else
			return vec4(0.0, 0.0, 0.0, 1.0);
	}
}

void calculateVectors()
{
		//Translate vertex position into world coordinates. 
	vertexPosition = modelMat * vec4(in_Position, 1.0);

		//Find vector pointing to vertex.  
	vertexToEye = normalize(eyePos - vertexPosition.xyz);

		//Normalise normal (haha) and transform into world coordinates. 
	normalVec = normalize(normalMat * in_Normal);
}

void main(void) 
{
	calculateVectors();

	vec4 lightColour = vec4(0.0, 0.0, 0.0, 1.0);

	for(int i=0; i<numLights; i++)
	{
		lightColour += vec4(calculateLight(i).rgb, 0.0);
	}

	vs_out.colour = lightColour;

	vs_out.texCoord = in_TexCoord;

    gl_Position = projectionMat * modelViewMat * vec4(in_Position, 1.0);
}