#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 scale;

uniform vec2 center;
uniform vec4 currentPos;

uniform sampler2D texHLevel;
uniform sampler2D texHLevel1;

uniform sampler2D vtex;
uniform sampler2D texHeightmap;

uniform int showBlend;

uniform int xzratio;
uniform int signbitX;
uniform int complementX;
uniform int signbitZ;
uniform int complementZ;

out vec2 uv;


void main() {
	const float VIEWCHUNKNUMBER = 32.0;
	const float CHUNKREALSIZE = 5120;
	const float MAXSCALE = CHUNKREALSIZE * VIEWCHUNKNUMBER / 2.0;
	const float CHUNKNUMBER = 32.0;
	const float CHUNKSIZE = 512;

	vec3 position1;
	position1.x = position.x * xzratio + position.z * (1 - xzratio);
	position1.z = position.x * (1 - xzratio) + position.z * xzratio;
	position1.y = position.y;
	position1.x = complementX + signbitX * position1.x;
	position1.z = complementZ + signbitZ * position1.z;
	
	vec4 pos = scale * vec4(position1, 1.0);
	vec2 tCenter = center * MAXSCALE;
	pos.xz = pos.xz + tCenter;

	uv = pos.xz / CHUNKREALSIZE / CHUNKNUMBER + currentPos.xy;
	float level = texture(texHLevel, uv).r;
	float ratio = fract(level);
	level -= ratio;
	vec4 scaleBias = textureLod(vtex, uv, level);
	level = texture(texHLevel1, uv).r;
	level *= 255;
	vec4 scaleBias1 = textureLod(vtex, uv, level);
	vec2 pCoor = uv * scaleBias.x + scaleBias.zw;
	vec2 pCoor1 = uv * scaleBias1.x + scaleBias1.zw;

	float h = texture(texHeightmap, pCoor).r;
	float h1 = texture(texHeightmap, pCoor1).r;

	h1 *= 6553.5f;
	h *= 6553.5f;
	pos.y = mix(h, h1, ratio);

	
	gl_Position = projection * view * model * pos;
	
}

