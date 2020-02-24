#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 100) out;

in int typ[];
in float rot[];
in vec3 world_normal[];

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;

uniform float radians;
uniform float got_hit;

out vec3 world_position;
out vec3 world_normal2;

layout(location = 0) out vec2 texture_coord;

vec3 vpos = gl_in[0].gl_Position.xyz;
//vec3 forward = normalize(eye_position - vpos);
//vec3 right = normalize(cross(forward, vec3(0, 1, 0)));
//vec3 up = normalize(cross(forward, right));
vec3 up = vec3(0, 1, 0);
vec3 right = vec3(1, 0, 0);
vec3 forward = vec3(0, 0, 1);

float ds = 0.05;
float depth = 0.02;

uniform float time_dilation;

mat4 trans(vec3 pozitie)
{
	mat4 tr = mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						pozitie.x, pozitie.y, pozitie.z, 1);
	return tr;
}

mat4 rotateOZ(float rad) {
	mat4 rotate = mat4(cos(rad), sin(rad), 0, 0,
					-sin(rad), cos(rad), 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
	return rotate;
}

mat4 rotateOX(float rad) {
	mat4 rotate = mat4(1, 0, 0, 0,
						0, cos(rad), sin(rad), 0,
						0, -sin(rad), cos(rad), 0,
						0, 0, 0, 1);
	return rotate;
}

void EmitPoint(vec2 offset)
{
	vec3 pos = right * offset.x + up * offset.y + vpos;
	gl_Position = Projection * View * vec4(pos, 1.0);
	EmitVertex();
}

vec3 EmitPoint2(vec3 offset)
{
	vec3 pos = right * offset.x + up * offset.y + forward * offset.z + vpos, newPos = pos;
	float rotationOZ = (rot[0] / 2), rotationOX = (rot[0]);
	//float rotationOZ = (radians / 2 + rot[0] / 2) * time_dilation, rotationOX = (radians + rot[0]) * time_dilation;
	//gl_Position = Projection * View * translate * rotate * trans(vec3(0, -ds/3, 0)) * vec4(pos, 1.0);
	if (typ[0] == 1) {
		if (got_hit != 0) {
			gl_Position = Projection * View * trans(vec3(0, ds/3, -depth/2) + vpos) * rotateOZ(rotationOZ) * rotateOX(rotationOX) * trans(vec3(0, -ds/3, depth/2) - vpos) * vec4(pos, 1.0);
			newPos = (trans(vec3(0, ds/3, -depth/2) + vpos) * rotateOZ(rotationOZ) * rotateOX(rotationOX) * trans(vec3(0, -ds/3, depth/2) - vpos) * vec4(pos, 1.0)).xyz;
			world_position = newPos;
			//gl_Position = Projection * View * trans(vec3(0, ds/3, -depth/2) + vpos) * trans(vec3(0, -ds/3, depth/2) - vpos) * vec4(pos, 1.0);
			//world_normal2 = normalize(mat3(trans(vec3(0, ds/3, -depth/2) + vpos) * trans(vec3(0, -ds/3, depth/2) - vpos)) * world_normal[0]);
		} else {
			gl_Position = Projection * View * vec4(pos, 1.0);
		}
	} else {
		if (got_hit != 0) {
			gl_Position = Projection * View * trans(vec3(0, 2 * ds/3, -depth/2) + vpos) * rotateOZ(rotationOZ) * rotateOX(rotationOX) * trans(vec3(0, -2 * ds/3, depth/2) - vpos) * vec4(pos, 1.0);
			newPos = (trans(vec3(0, 2 * ds/3, -depth/2) + vpos) * rotateOZ(rotationOZ) * rotateOX(rotationOX) * trans(vec3(0, -2 * ds/3, depth/2) - vpos) * vec4(pos, 1.0)).xyz;
			world_position = newPos;
			//gl_Position = Projection * View * trans(vec3(0, 2 * ds/3, -depth/2) + vpos) * trans(vec3(0, -2 * ds/3, depth/2) - vpos) * vec4(pos, 1.0);
			//world_normal2 = normalize(mat3(trans(vec3(0, 2 * ds/3, -depth/2) + vpos) * trans(vec3(0, -2 * ds/3, depth/2) - vpos)) * world_normal[0]);
		} else {
			gl_Position = Projection * View * vec4(pos, 1.0);
		}
	}
	world_position = newPos;
	EmitVertex();
	return newPos;
}

void main()
{
	// Create a quad using triangle strip
	//
	//	3---------2
	//  | \       |
	//  |    \    |
	//  |       \ |
	//  0---------1
	//
	// Triangles: (0, 1, 3), (1, 2, 3)

	/*texture_coord = vec2(0, 0);
	vec2 offset= vec2(-ds, -ds);
	EmitPoint(offset);

	texture_coord = vec2(1, 0);
	offset= vec2(ds, -ds);
	EmitPoint(offset);

	texture_coord = vec2(0, 1);
	offset= vec2(-ds, ds);
	EmitPoint(offset);*/

	/*texture_coord = vec2(1, 1);
	offset= vec2(ds, ds);
	EmitPoint(offset);*/

	vec3 offset, p1, p2, p3;

	if (typ[0] == 1) {
		/*texture_coord = vec2(0, 0);
		offset = vec3(0, 0, 0);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(ds, 0, 0);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(0, ds, 0);
		p3 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p2 - p1, p3 - p1));

		texture_coord = vec2(0, 0);
		offset = vec3(ds, 0, -depth);
		p1 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p1 - p2, p3 - p2));

		texture_coord = vec2(1, 1);
		offset = vec3(0, ds, -depth);
		p2 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p1 - p3, p2 - p3));

		texture_coord = vec2(0, 1);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p2 - p1, p3 - p1));

		texture_coord = vec2(1, 0);
		offset = vec3(0, ds, 0);
		p1 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p1 - p2, p3 - p2));

		texture_coord = vec2(1, 1);
		offset = vec3(0, 0, 0);
		p2 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p1 - p3, p2 - p3));

		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p2 - p1, p3 - p1));

		texture_coord = vec2(1, 0);
		offset = vec3(ds, 0, 0);
		p1 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p1 - p2, p3 - p2));

		texture_coord = vec2(0, 1);
		offset = vec3(ds, 0, -depth);
		p2 = EmitPoint2(offset);

		world_normal2 = normalize(cross(p1 - p3, p2 - p3));*/

		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, 0);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(ds, 0, 0);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(0, ds, 0);
		p3 = EmitPoint2(offset);

		world_normal2 = -normalize(cross(p2 - p1, p3 - p1));
		EndPrimitive();

		texture_coord = vec2(1, 0);
		offset = vec3(ds, 0, 0);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(0, ds, 0);
		p3 = EmitPoint2(offset);

		texture_coord = vec2(0, 0);
		offset = vec3(ds, 0, -depth);
		p1 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p2, p3 - p2));
		EndPrimitive();

		texture_coord = vec2(0, 1);
		offset = vec3(0, ds, 0);
		p3 = EmitPoint2(offset);

		texture_coord = vec2(0, 0);
		offset = vec3(ds, 0, -depth);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 1);
		offset = vec3(0, ds, -depth);
		p2 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p3, p2 - p3));
		EndPrimitive();

		texture_coord = vec2(0, 0);
		offset = vec3(ds, 0, -depth);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 1);
		offset = vec3(0, ds, -depth);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p2 - p1, p3 - p1));
		EndPrimitive();

		texture_coord = vec2(1, 1);
		offset = vec3(0, ds, -depth);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(0, ds, 0);
		p1 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p2, p3 - p2));
		EndPrimitive();

		texture_coord = vec2(0, 1);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(0, ds, 0);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 1);
		offset = vec3(0, 0, 0);
		p2 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p3, p2 - p3));
		EndPrimitive();

		texture_coord = vec2(1, 0);
		offset = vec3(0, ds, 0);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 1);
		offset = vec3(0, 0, 0);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p2 - p1, p3 - p1));
		EndPrimitive();

		texture_coord = vec2(1, 1);
		offset = vec3(0, 0, 0);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(ds, 0, 0);
		p1 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p2, p3 - p2));
		EndPrimitive();

		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(ds, 0, 0);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(ds, 0, -depth);
		p2 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p3, p2 - p3));
		EndPrimitive();

	} else {
		//vpos *= vec3(1, 1, -1);
		//vpos.z *= -1;
		
		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, 0);
		p1 = EmitPoint2(offset);

		texture_coord = vec2(1, 0);
		offset = vec3(0, ds, 0);
		p2 = EmitPoint2(offset);

		texture_coord = vec2(0, 1);
		offset = vec3(-ds, ds, 0);
		p3 = EmitPoint2(offset);

		world_normal2 = -normalize(cross(p2 - p1, p3 - p1));

		texture_coord = vec2(0, 0);
		offset = vec3(0, ds, -depth);
		p1 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p2, p3 - p2));

		texture_coord = vec2(1, 1);
		offset = vec3(-ds, ds, -depth);
		p2 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p3, p2 - p3));

		texture_coord = vec2(0, 1);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p2 - p1, p3 - p1));

		texture_coord = vec2(1, 0);
		offset = vec3(-ds, ds, 0);
		p1 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p2, p3 - p2));

		texture_coord = vec2(1, 1);
		offset = vec3(0, 0, 0);
		p2 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p3, p2 - p3));

		texture_coord = vec2(0, 0);
		offset = vec3(0, 0, -depth);
		p3 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p2 - p1, p3 - p1));

		texture_coord = vec2(1, 0);
		offset = vec3(0, ds, 0);
		p1 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p2, p3 - p2));

		texture_coord = vec2(0, 1);
		offset = vec3(0, ds, -depth);
		p2 = EmitPoint2(offset);

		//world_normal2 = normalize(cross(p1 - p3, p2 - p3));
	}
}
