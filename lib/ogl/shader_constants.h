#pragma once


struct ShaderType{
    const char * vshader;
    const char * fshader;
};


inline  constexpr ShaderType TEXTURE_SHADER{
   .vshader = R"(
      #version 450 core
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aCol;
      layout (location = 2) in vec3 aNorm;
      layout (location = 3) in vec2 aTexCoord;

      layout (std140) uniform UniformBufferObject {
         mat4 model;
         mat4 view;
         mat4 proj;
      }ubo;

      out vec3 ourColor;
      out vec2 TexCoord;
      void main()
      {
         ourColor = aCol;
         gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);
         TexCoord = aTexCoord;
      }
   )",

   .fshader = R"(
      #version 450 core

      in vec3 ourColor;
      in vec2 TexCoord;
      out vec4 FragColor;

      uniform sampler2D ourTexture;

      void main()
      {
         FragColor = texture(ourTexture, TexCoord);
      }
      )"
};

inline  constexpr ShaderType PHONG_SHADER{
   .vshader = R"(
      #version 450 core
      layout (location = 0) in vec3 aPos;
      layout (location = 1) in vec3 aCol;
      layout (location = 2) in vec3 aNorm;
      
      layout (std140) uniform UniformBufferObject {
         mat4 model;
         mat4 view;
         mat4 proj;
      }ubo;

      out vec3 vertexColor;
      out vec3 Normal;
      out vec3 FragPos;

      void main()
      {
         vertexColor = aCol;
         FragPos = vec3(ubo.model * vec4(aPos, 1.0));
         Normal = mat3(transpose(inverse(ubo.model))) * aNorm; 
         gl_Position = ubo.proj * ubo.view * vec4(FragPos, 1.0);
      }
      )",

   .fshader = R"(
      #version 450 core
      out vec4 FragColor;

      in vec3 vertexColor;
      in vec3 Normal;
      in vec3 FragPos;
   
      uniform vec3 viewPos; 

      void main()
      {
         vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
         vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

         // ambient
         float ambientStrength = 0.1;
         vec3 ambient = ambientStrength * lightColor;

         // diffuse 
         vec3 norm = normalize(Normal);
         vec3 lightDir = normalize(lightPos - FragPos);
         float diff = max(dot(norm, lightDir), 0.0);
         vec3 diffuse = diff * lightColor;
         
         // specular
         float specularStrength = 0.5;
         vec3 viewDir = normalize(viewPos - FragPos);
         vec3 reflectDir = reflect(-lightDir, norm);  
         float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
         vec3 specular = specularStrength * spec * lightColor;  
            
         vec3 result = (ambient + diffuse + specular) * vertexColor;
         FragColor = vec4(result, 1.0);
      }
      )"
};


