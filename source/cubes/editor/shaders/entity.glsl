R"(
#section vertex
#version 410 core

// ______________________________________________________
layout (location = 0) in vec3 a_position;   // per vertex
layout (location = 1) in vec3 a_normal;     // per vertex
layout (location = 2) in vec2 a_uv_map;     // per vertex
layout (location = 3) in vec3 a_center;     // per entity
layout (location = 4) in uint a_texture_set_id; // per entity... to calc uv_map.y... per entity because each should use one set of textures
layout (location = 5) in uint a_sub_texture_id; // per vertex... to calc uv_map.x... per vertex because we want to know the vertex direction

out VS_OUT {
    vec3 normal;
    vec2 uv_map;
} vs_out;

// _________________________________
uniform mat4x4 u_view = mat4x4(1.0);
uniform mat4x4 u_clip = mat4x4(1.0);

mat4x4 transformation()
{
    return u_clip * u_view * mat4x4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(a_center,      1.0)
    );
}

vec2 calculate_uv_map(vec2 uv_map, vec2 virtual_texture_size, vec2 texture_size)
{
    // maybe fix values later... or fix cube's left and right faces uv_map
    const vec2 atlas_fixer = vec2(0.001, 0.0001);

    vec2 uv_map_min = vec2(
        virtual_texture_size.x * a_sub_texture_id / texture_size.x,
        virtual_texture_size.y * a_texture_set_id / texture_size.y
    ) + vec2(atlas_fixer); // trying reduce interpolation error with the atlas

    vec2 uv_map_max = vec2(
        virtual_texture_size.x * (a_sub_texture_id + 1) / texture_size.x,
        virtual_texture_size.y * (a_texture_set_id + 1) / texture_size.y
    ) - vec2(atlas_fixer); // trying reduce interpolation error with the atlas

    uv_map.x = uv_map.x == 0.0 ? uv_map_min.x : uv_map_max.x;
    uv_map.y = uv_map.y == 0.0 ? uv_map_min.y : uv_map_max.y;

    return vec2(uv_map);
}

uniform vec2 u_texture_size     = vec2(96.0, 32.0);
uniform vec2 u_sub_texture_size = vec2(16.0, 16.0);

void main()
{
    gl_Position   = transformation() * vec4(a_position, 1.0);
    vs_out.normal = a_normal;
    vs_out.uv_map = calculate_uv_map(a_uv_map, u_sub_texture_size, u_texture_size);
}

#section fragment
#version 410 core

in VS_OUT {
    vec3 normal;
    vec2 uv_map;
} fs_in;

out vec4 FragColor;
uniform sampler2D u_texture;

void main()
{
    //FragColor = vec4(1.0, 0.7, 0.3, 1.0);
    FragColor = texture(u_texture, fs_in.uv_map);
}
)"