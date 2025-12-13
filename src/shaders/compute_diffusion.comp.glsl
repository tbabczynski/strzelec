#version 460 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (std430, binding = 0) buffer StateBuffer
{
    float state[];
};

uniform float uTimeStep;
uniform int uFrame;

// Fixed simulation dimensions (can be passed as uniform if needed)
const ivec2 dims = ivec2(256, 256);

void main()
{
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    
    if (gid.x >= dims.x || gid.y >= dims.y)
        return;

    int idx = gid.y * dims.x + gid.x;
    
    // Simple diffusion/relaxation on red channel
    float center = state[idx];
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
            neighbor = clamp(neighbor, ivec2(0), dims - ivec2(1));
            
            int nidx = neighbor.y * dims.x + neighbor.x;
            sum += state[nidx];
            count++;
        }
    }

    float avg = sum / float(count);
    
    // Simple relaxation: blend center with average of neighbors
    float diffusionRate = uTimeStep * 0.5;
    float newValue = mix(center, avg, diffusionRate);
    
    // Write back (use barrier if reading/writing in multiple passes)
    state[idx] = newValue;
}
