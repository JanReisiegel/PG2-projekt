#version 460 core

#define MAX_LIGHTS 16

struct s_lights {
	vec4 position[MAX_LIGHTS];
	vec3 color[MAX_LIGHTS];
	vec3 ambient_intensity[MAX_LIGHTS];
	vec3 diffuse_intensity[MAX_LIGHTS];
	vec3 specular_intensity[MAX_LIGHTS];
	vec3 ambient_material[MAX_LIGHTS];
	vec3 diffuse_material[MAX_LIGHTS];
	vec3 specular_material[MAX_LIGHTS];
	float specular_shinines[MAX_LIGHTS];
};

// Vertex attributes
in vec4 aPos;
in vec3 aNorm;
in vec2 aTex;

// Matrices 
uniform mat4 uM_m;
uniform mat4 uV_m;
uniform mat4 uP_m;

uniform s_lights lights;

// Light properties
//uniform vec3 light_position = vec3(0.0,10.0,0.0);

// Outputs to the fragment shader
out VS_OUT {
    vec3 N;
    vec3 L[MAX_LIGHTS];
    vec3 V;
    vec2 texCoord;
} vs_out;

void main(void) {
    // Create Model-View matrix
    mat4 mv_m = uV_m * uM_m;

    // Calculate view-space coordinate - in P point 
    // we are computing the color
    vec4 P = mv_m * aPos;

    

    // Calculate normal in view space
    vs_out.N = mat3(mv_m) * aNorm;
     // Calculate view-space light vector
    for (int i = 0; i < lights.position.length(); i++) {
        // Calculate light vector in view space
        vec3 light_position_view = vec3(uV_m * lights.position[i]);
        vs_out.L[i] = light_position_view - P.xyz;
    }
    // Calculate view vector (negative of the view-space position)
    vs_out.V = -P.xyz;

	// Assigns the texture coordinates from the Vertex Data to "texCoord"
    vs_out.texCoord = aTex;

    // Calculate the clip-space position of each vertex
    gl_Position = uP_m * P;
}

