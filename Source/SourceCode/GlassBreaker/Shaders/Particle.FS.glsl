#version 430

layout(location = 0) in vec2 texture_coord;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

uniform vec3 camera_position;
in vec3 world_position;
vec3 RazaIncidenta = world_position - camera_position;

in vec3 world_normal2;

layout(location = 0) out vec4 out_color;

uniform float wireframe;

vec3 myRefract(float refractive_index)
{
    // TODO - compute the refraction color value
	vec3 refr = refract( RazaIncidenta, world_normal2, refractive_index);
	//vec3 refr = refract( RazaIncidenta, vec3(0, 0, 1), refractive_index);
	return refr;
}

void main()
{
	//out_color = texture(texture_cubemap, myRefract(1.33));
	if (wireframe == 2) {
		out_color = vec4(world_normal2, 0); 
	}
	else if (wireframe == 1) {
		vec3 color = texture(texture_1, texture_coord).xyz;
		out_color = vec4(color, 1);
	} else {
		out_color = texture(texture_cubemap, myRefract(1.33));
	}
	/*vec3 color = texture(texture_1, texture_coord).xyz;
	color = vec3(1, 0, 0);
	out_color = vec4(color, 1);*/
	//out_color = vec4(world_normal2, 0); 
}