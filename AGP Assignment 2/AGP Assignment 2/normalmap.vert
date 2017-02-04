// phongTexMap.vert
// Vertex shader for use with a Phong or other reflection model fragment shader
// Calculates and passes on V, L, N vectors for use in fragment shader, phong2.frag
#version 330

uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 lightPosition;
uniform mat4 modelMatrix;
uniform vec3 cameraPos;

in  vec3 in_Position;
in  vec3 in_Normal;
out vec3 ex_N;
out vec3 ex_V;
out vec3 ex_L;

in vec2 in_TexCoord;
layout(location = 5) in vec4 tangent;

out vec2 ex_TexCoord;
out float ex_D;

out vec3 eyeTan;
out vec3 lightTan;


// multiply each vertex position by the MVP matrix
// and find V, L, N vectors for the fragment shader
void main(void) {

	
	vec4 vertexPosition = modelview * vec4(in_Position,1.0);
	ex_D = distance(vertexPosition,lightPosition);

	mat3 normalmatrix = transpose(inverse(mat3(modelview)));
	
	ex_N = normalize(normalmatrix * in_Normal);

	vec3 worldPos = (modelMatrix * vec4(in_Position,1.0)).xyz;

	ex_V = normalize(-vertexPosition).xyz;

	ex_L = normalize(lightPosition.xyz - vertexPosition.xyz);

	ex_TexCoord = in_TexCoord;

    gl_Position = projection * vertexPosition;

	vec3 tan = (normalmatrix * tangent.xyz);
	vec3 bitan = cross(ex_N,tan) * tangent.w;
	
	mat3 TBN = transpose(mat3(tan, bitan, ex_N));
	eyeTan = TBN * ex_V;
	lightTan = TBN * ex_L;
}