#include <string>

const std::string vertexShader =
R"(
#version 130
out vec4 vCoords;
out vec2 texCoords;
flat out int atlasIndex;

void main() {
	atlasIndex = int(gl_Color.r * 255);
	vCoords = gl_ModelViewMatrix * gl_Vertex;
	texCoords = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
	gl_Position = gl_ProjectionMatrix * vCoords;
}
)";

const std::string fragmentShader =
R"(
#version 130

uniform sampler2D chunkAtlas;
uniform sampler2D tileAtlas;

uniform vec3 viewport;
uniform vec3 brush;

uniform float chunkSize;
uniform float tileSize;
uniform float padding;

uniform float time;

uniform float timeOfDay = 1.0;

uniform vec4 ambianceData = vec4(0.3, 0.3, 0.8, 0.3);
uniform vec4 lightData = vec4(1.0, 0.8, 0.2, 1.0);

uniform vec4 lineColorA = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 lineColorB = vec4(0.3, 0.3, 0.3, 1.0);

uniform float fadeDistanceA = 0.8;
uniform float fadeDistanceB = 0.4;
uniform float thicknessA = 0.05;
uniform float thicknessB = 0.01;
uniform float bias = 1.5;

uniform bool drawGrid = false;
uniform bool drawBrush = false;
uniform bool drawLighting = false;

in vec4 vCoords;
in vec2 texCoords;
flat in int atlasIndex;

float getMipLevel(vec2 uv)  {
    vec2 dx = dFdx(uv);
    vec2 dy = dFdy(uv);
    float d = max(dot(dx, dx), dot(dy, dy));
	return max( 0.5 * log2( d ) + bias, 0 );
}

vec2 worldToGlobal(vec2 world) {
	return world / (tileSize - padding * 2);
}

vec2 indexToCoords(int index) {
	vec2 atlasSize = textureSize(chunkAtlas, 0) / (chunkSize + 1.0);
	return vec2(index % int(atlasSize.x), index / int(atlasSize.y));
}

int sampleChunkAtlas(int index, ivec2 coords) {
	vec2 atlasCoords = indexToCoords(index);
	vec2 uv = (atlasCoords * (chunkSize + 1.0) + coords) / textureSize(chunkAtlas, 0);
	return int(texture2D(chunkAtlas, uv).r * 255.0);
}

vec4 sampleTileAtlas(vec2 tile, vec2 uv) {
	float mip = getMipLevel(uv * chunkSize * (tileSize - padding * 2));

	vec2 tilesetCount = textureSize(tileAtlas, 0) / tileSize;
	vec2 position = vec2(tile.x / tilesetCount.x, tile.y / tilesetCount.y); 
	
	vec2 offset = fract(uv * chunkSize);
	offset *= (tileSize - 2.0 * padding) / tileSize;
	offset += padding / tileSize;

	return texture2DLod(tileAtlas, position + offset / tilesetCount, mip);
}

vec2 getTile(int index, ivec2 coords) {
	int TL = sampleChunkAtlas(index, coords);
	int TR = sampleChunkAtlas(index, coords + ivec2(1, 0));
	int BL = sampleChunkAtlas(index, coords + ivec2(0, 1));
	int BR = sampleChunkAtlas(index, coords + ivec2(1, 1));

	int saddle = ((TL & 1) + (TR & 1) + (BL & 1) + (BR & 1) + 1) >> 2;
	int shape = ((TL >> 1) & 1) | (TR & 2) | ((BL & 2) << 1) | ((BR & 2) << 2);
	int ring = ((TL & ~1) + (TR & ~1) + (BL & ~1) + (BR & ~1)) >> 3;

	int row = (ring << 1) | saddle;
	int col = shape - (ring & 1);
	
	return vec2(col, row);
}

vec4 mixGrid(vec4 color, vec2 uv, float sizeA, float sizeB) {
	float aa = length(fwidth(uv));

	float halfA = sizeA / 2.0;
	float halfB = sizeB / 2.0;

	vec2 a = abs(mod(uv - halfA, sizeA) - halfA);
    vec2 b = abs(mod(uv - halfB, sizeB) - halfB);
	
	float distance = viewport.z - vCoords.z;
	float factorA = clamp(1.0 - distance / fadeDistanceA, 0.0, 1.0);
	float factorB = clamp(1.0 - distance / fadeDistanceB, 0.0, 1.0);

    color = mix(
		color, 
        lineColorB, 
        smoothstep(aa + thicknessB, thicknessB, min(b.x, b.y)) * factorB
    );

	color = mix(
		color, 
        lineColorA, 
        smoothstep(aa + thicknessA, thicknessA, min(a.x, a.y)) * factorA
    );	

	return color;
}

vec4 mixBrush(vec4 color, vec2 global) {
	float distance = length(brush.xy - global);

	float circle = 1.0 - step(brush.z, distance);
	vec4 brushColor = vec4(0.0, 0.0, 0.5, 1.0);
	float alpha = 0.2;

	return mix(color, brushColor, circle * alpha);
}

vec4 mixLighting(vec4 color, vec2 global) {
	float maxDistance = 40.0f + sin(time * 10);
	float distance = length(brush.xy - global);
	float value = 1.0 - smoothstep(0.0, maxDistance, distance);

	vec3 ambiance = mix(color.rgb * ambianceData.rgb * ambianceData.a, vec3(1.0), timeOfDay);
	vec3 light = mix(lightData.rgb * lightData.a * clamp(value, 0.0, 1.0), vec3(0.0), timeOfDay);

	vec3 intensity = ambiance + light;
	
	return color * vec4(intensity, 1.0);

}

void main() {
	vec2 position = texCoords * chunkSize;
	vec2 local = floor(position);
	vec2 global = worldToGlobal(vCoords.xy);

	vec2 tile = getTile(atlasIndex, ivec2(local));
	vec4 pixel = sampleTileAtlas(tile, texCoords);
	
	if(drawGrid)
		pixel = mixGrid(pixel, position, chunkSize, 1.0);

	if(drawBrush)
		pixel = mixBrush(pixel, global);
	
	pixel = mixLighting(pixel, global);

	gl_FragColor = pixel;
}   	
)";