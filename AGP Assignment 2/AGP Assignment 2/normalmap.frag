// Phong fragment shader phongEnvMap.frag matched with phong-tex.vert
#version 330

// Some drivers require the following
precision highp float;

struct lightStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct materialStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

// Uniforms
uniform lightStruct light;
uniform materialStruct material;
uniform sampler2D texMap;
uniform sampler2D normalMap;

// Inputs
in vec4 ex_N;
in vec4 ex_V;
in vec4 ex_L;
in vec2 ex_TexCoord;
in float ex_D;

in vec3 eyeTan;
in vec3 lightTan;

layout(location = 0) out vec4 out_Color;

void main(void) {

	vec3 N = normalize( (texture( normalMap, ex_TexCoord ).rgb-0.5) * 2 );

	vec4 ambientI = light.ambient * material.ambient;


	vec4 diffuseI = light.diffuse * material.diffuse;
	diffuseI = diffuseI * max(dot(N,normalize(lightTan)),0);

	vec3 R = normalize(reflect(-lightTan,N));

	vec4 specularI = light.specular * material.specular;
	specularI = specularI * pow(max(dot(R,-eyeTan),0), material.shininess); // + or - eyeTan??

	vec4 litColor = vec4( (ambientI + (diffuseI + specularI)/1.0).rgb, 1.0f );

	out_Color = texture(texMap, ex_TexCoord) * litColor;
}