#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout (binding = 2, rgba8) uniform readonly image2D resultImage;


void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);

    outColor = vec4(imageLoad(resultImage, coord));
}


