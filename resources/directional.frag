#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;

// Material properties
//uniform vec3 ambient_intensity, diffuse_intensity, specular_intensity;
//uniform vec3 ambient_material, diffuse_material, specular_material;
//uniform float specular_shinines;

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

uniform s_lights lights;

// active texture unit
uniform sampler2D tex0;

// Input from vertex shader
in VS_OUT {
    vec3 N;
    vec3 L[MAX_LIGHTS];
    vec3 V;
    vec2 texCoord;
} fs_in;

void main(void) {
    vec3 accumulator = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

	for(int i = 0; i < MAX_LIGHTS; i++) {
		// Normalize the incoming N, L and V vectors
        vec3 N = normalize(fs_in.N);
        vec3 L = normalize(fs_in.L[i]);
        vec3 V = normalize(fs_in.V);

        // Calculate R by reflecting -L around the plane defined by N
        vec3 R = reflect(-L, N);

        // Calculate the ambient, diffuse and specular contributions
        ambient += lights.ambient_material[i] * lights.ambient_intensity[i];
        diffuse += max(dot(N, L), 0.0) * lights.diffuse_material[i] * lights.diffuse_intensity[i];
        specular += pow(max(dot(R, V), 0.0), lights.specular_shinines[i])
            * lights.specular_material[i] * lights.specular_intensity[i];
    }


    FragColor = vec4( (ambient + diffuse) * texture(tex0, fs_in.texCoord).rgb + specular, 1.0);
}

