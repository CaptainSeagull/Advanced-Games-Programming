// Fragment Shader – file "particles.frag"
#version 330
precision highp float; // needed only for version 1.30

in  vec3 ex_Color; 
in float height;
in float distanceToEye;
out vec4 out_Color; 
uniform sampler2D textureUnit0; 


void main(void)
{
	  out_Color = vec4(ex_Color, 1.0) * texture(textureUnit0, gl_PointCoord);

	  if(out_Color == vec4(0.0, 0.0, 0.0, 1.0))
		out_Color.a = 0.0f;
}
