#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;   
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos; 
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 luzDir;
uniform int soloAmbiente;
uniform int usarTextura;
uniform int piscina;
uniform int instante;

// texture sampler
uniform sampler2D texturas[17];

void main()
{
    int tex = 16;
    if (piscina == 1){
        tex = instante;
    }

    if (soloAmbiente == 0){

        // Ambiente
        float ambientI = 0.5;
        vec3 ambient = ambientI * lightColor;

        // angulo de 15 grados
        vec3 fd = normalize(vec3((FragPos - lightPos)));

        //vec3 ld = luzDir;
        vec3 ld = normalize(luzDir);

        if (acos(dot(fd,ld)) < radians(15.0)) { 
            // Dentro del foco
  	
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


            if (usarTextura == 0){
                vec3 result = (ambient + diffuse + specular) * Color;
                FragColor = vec4(result, 1.0);
            }
            else{
                vec4 Textura = texture(texturas[tex], TexCoord);
                Textura = vec4(Textura.rgb, Textura.a * 0.5);
                FragColor = vec4((ambient + diffuse + specular), 1.0) * Textura;
            }

        } 
        else{
            // Fuera del foco (no hay difusa ni especular)

            if (usarTextura == 0){
                vec3 result = (ambient) * Color;
                FragColor = vec4(result, 1.0);
            }
            else{
                vec4 Textura = texture(texturas[tex], TexCoord);
                Textura = vec4(Textura.rgb, Textura.a * 0.5);
                FragColor = vec4((ambient), 1.0) * Textura;
            }
        }
    }

    else{   // Modo de iluminacion: solo luz ambiente
    
        if (usarTextura == 0){
            vec3 result = (lightColor) * Color;
            FragColor = vec4(result, 1.0);
        }
        else{
            vec4 Textura = texture(texturas[tex], TexCoord);
            Textura = vec4(Textura.rgb, Textura.a * 0.5);
            FragColor = vec4(lightColor, 1.0) * Textura;
        }
        
    }
    



}

