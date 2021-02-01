#version 330 core

in vec2 uv;

uniform sampler2D terrainTex;

out vec4 color;

void main() {
	if (uv.x <= 0.003 || uv.x >= 0.997 || uv.y <= 0.003 || uv.y >= 0.997)
		discard;

	color = texture(terrainTex, uv);
}
