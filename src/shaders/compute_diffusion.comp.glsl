#version 460 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba32f) uniform readonly image2D srcImage;
layout (binding = 1, rgba32f) uniform writeonly image2D dstImage;

uniform float uTimeStep;
uniform int uWidth;
uniform int uHeight;

void main()
{
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    
    if (gid.x >= uWidth || gid.y >= uHeight)
        return;
    
    // Load center pixel (red channel contains the value)
    vec4 centerColor = imageLoad(srcImage, gid);
    float center = centerColor.r;
    
    float sum = 0.0;
    int count = 0;

    // Sample neighbors (Moore neighborhood - 8-connected)
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0)
                continue;

            ivec2 neighbor = gid + ivec2(dx, dy);
            
            // Clamp to boundaries
            neighbor = clamp(neighbor, ivec2(0), ivec2(uWidth - 1, uHeight - 1));
            
            vec4 neighborColor = imageLoad(srcImage, neighbor);
            sum += neighborColor.r;
            count++;
        }
    }

    float avg = sum / float(count);
    
    // Simple relaxation: blend center with average of neighbors
    float diffusionRate = uTimeStep * 0.5;
    float newValue = mix(center, avg, diffusionRate);
    
    // Write to destination image (red channel, keep other channels)
    vec4 outputColor = vec4(newValue, centerColor.g, centerColor.b, centerColor.a);
    imageStore(dstImage, gid, outputColor);
}
