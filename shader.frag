#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;   

out vec4 FragColor;

uniform vec3 viewPos; 
uniform vec3 lightPos; 
uniform vec3 lightColor;
//uniform vec3 objectColor;
uniform vec3 luzDir;
uniform int soloAmbiente;

void main()
{
    if (soloAmbiente == 0){

        //vec3 luzDir = vec3(0,-1,0);

        // Ambiente
        float ambientI = 0.5;
        vec3 ambient = ambientI * lightColor;

        // angulo de 15 grados
        //vec3 ld = normalize(-lightPos);
        vec3 fd = normalize(vec3((FragPos - lightPos)));

        //vec3 ld = luzDir;
        vec3 ld = normalize(luzDir);

        if (acos(dot(fd,ld)) < radians(15.0)) { 
  	
            // Difusa
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;

            // Especular
	        float specularStrength = 1.0;
	        vec3 viewDir = normalize(viewPos - FragPos);
	        vec3 reflectDir = reflect(-lightDir, norm);
	        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
	        vec3 specular = specularStrength * spec * lightColor;

            
   	        vec3 result = (ambient + diffuse + specular) * Color;
            FragColor = vec4(result, 1.0);

        } 
        else{
            vec3 result = (ambient) * Color;
            FragColor = vec4(result, 1.0);
        }
    }

    else{
        vec3 result = (lightColor) * Color;
        FragColor = vec4(result, 1.0);
    }
    



}

