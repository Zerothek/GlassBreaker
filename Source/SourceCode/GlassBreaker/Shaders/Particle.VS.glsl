#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

out int typ;
out float rot;

// Uniform properties
uniform mat4 Model;

uniform mat4 rotateOX;

uniform float got_hit;
uniform float time_dilation;

out vec3 world_normal;
uniform float radians;

struct Particle
{
	vec4 position;
	vec4 speed;
	vec4 iposition;
	vec4 ispeed;
	vec4 type;
};

layout(std430, binding = 0) buffer particles {
	Particle data[];
};

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	world_normal = v_normal;
	vec3 pos = data[gl_VertexID].position.xyz;
	vec3 spd = data[gl_VertexID].speed.xyz;
	typ = int(data[gl_VertexID].type.x);
	rot = data[gl_VertexID].type.z;

	float dt = 0.001 * time_dilation;

	if (got_hit != 0) {

		pos = pos + spd * dt + vec3(0, -0.9, 0) * dt * dt/2.0f ;
		spd = spd + vec3(0, -0.9, -0.3) * dt;
		rot += radians * time_dilation;

		if(pos.y < (-40 + rand(pos.xy) * 20))
		{
			pos = data[gl_VertexID].iposition.xyz;
			spd = data[gl_VertexID].ispeed.xyz;
		}

		data[gl_VertexID].position.xyz =  pos;
		data[gl_VertexID].speed.xyz =  spd;
		data[gl_VertexID].type.z = rot;
	}

	gl_Position = Model * vec4(pos, 1);
}


