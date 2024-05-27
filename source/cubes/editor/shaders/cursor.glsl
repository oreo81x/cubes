R"(
#section vertex
#version 410 core

layout (location = 0) in vec3 a_position;

uniform mat4x4 u_view = mat4x4(1.0);
uniform mat4x4 u_clip = mat4x4(1.0);
uniform vec3 u_center = vec3(0.0);

mat4x4 transformation()
{
    return u_clip * u_view * mat4x4(
        vec4(1.01, 0.0, 0.0, 0.0),
        vec4(0.0, 1.01, 0.0, 0.0),
        vec4(0.0, 0.0, 1.01, 0.0),
        vec4(u_center,       1.0)
    );
}

void main()
{
    gl_Position = transformation() * vec4(a_position, 1.0);
}

#section fragment
#version 410 core

out vec4 FragColor;

uniform vec3 u_color = vec3(0.0, 1.0, 0.0);

void main()
{
    FragColor = vec4(u_color, 1.0);
}
)"