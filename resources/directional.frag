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

    // Spotlight specific
    vec3 spot_direction[MAX_LIGHTS];
    float cos_cutoff[MAX_LIGHTS];
    float spot_exponent[MAX_LIGHTS];

    //attenuation
    float constant[MAX_LIGHTS];
    float linear[MAX_LIGHTS];
    float quadratic[MAX_LIGHTS];

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
        if (lights.position[i].w == 0.0){
            //directional
            ambient += lights.color[i] * lights.ambient_material[i] * lights.ambient_intensity[i];
            diffuse += lights.color[i] * max(dot(N, L), 0.0) * lights.diffuse_material[i] * lights.diffuse_intensity[i];
            specular += lights.color[i] * pow(max(dot(R, V), 0.0), lights.specular_shinines[i])
                * lights.specular_material[i] * lights.specular_intensity[i];
        }else if (lights.cos_cutoff[i] == 180.0){
            //point
            float d = length(L);
            float dist_attenuation = 1.0 / (lights.constant[i] + lights.linear[i] * d + lights.quadratic[i] * d * d);
            ambient += dist_attenuation * lights.color[i] * lights.ambient_material[i] * lights.ambient_intensity[i];
            diffuse += dist_attenuation * lights.color[i] * max(dot(N, L), 0.0) * lights.diffuse_material[i] * lights.diffuse_intensity[i];
            specular += lights.color[i] * pow(max(dot(R, V), 0.0), lights.specular_shinines[i])
                * lights.specular_material[i] * lights.specular_intensity[i];
        }else{
            //spot
            float d = length(L);
            float dist_attenuation = 1.0 / (lights.constant[i] + lights.linear[i] * d + lights.quadratic[i] * d * d);
            float spotEffect = dot(normalize(lights.spot_direction[i]), -L);
            float full_attenuation = 0.0;
            if (spotEffect > lights.cos_cutoff[i]){
                full_attenuation = dist_attenuation * pow(spotEffect, lights.spot_exponent[i]);
            }
            ambient += lights.color[i] * lights.ambient_material[i] * lights.ambient_intensity[i];
            diffuse += full_attenuation * max(dot(N, L), 0.0) * lights.diffuse_material[i] * lights.diffuse_intensity[i];
            specular += full_attenuation * lights.color[i] * pow(max(dot(R, V), 0.0), lights.specular_shinines[i])
                * lights.specular_material[i] * lights.specular_intensity[i];
        }
    }

    vec4 texColor = texture(tex0, fs_in.texCoord);
    FragColor = vec4( (ambient + diffuse) * texColor.rgb + specular, texColor.a);
}   

