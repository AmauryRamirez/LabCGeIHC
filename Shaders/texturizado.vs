#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;

out vec2 out_uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float offsetx;

void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	out_uv = in_uv;
	out_uv.x = out_uv.x + offsetx;

}

