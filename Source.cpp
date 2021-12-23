
#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>



// Переменные с индентификаторами ID
// ID шейдерной программы
GLuint Program;

struct Vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct Texture {
    GLfloat x;
    GLfloat y;
};

struct Normale
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct PolygPoint {
    Vertex vertex;
    Texture tex;
    Normale norm;
};

GLint Attrib_vertex;
GLint Attrib_texture;
GLint Attrib_normal;

GLint Unif_rotate;
GLint Unif_texture;
GLint Unif_move;
GLint Unif_scale;

//для абобуса
GLuint VBO;
GLuint VAO;

GLint textureHandle;
sf::Texture textureData_bus;

//для дороги
GLuint VBO_road;
GLuint VAO_road;

GLint textureHandle_road;
sf::Texture textureData_road;

//для травы
GLuint VBO_grass;
GLuint VAO_grass;

GLint textureHandle_grass;
sf::Texture textureData_grass;

//горизонт
GLuint VBO_horizon;
GLuint VAO_horizon;

GLint textureHandle_horizon;
sf::Texture textureData_horizon;

//трансформации абобуса
float rotate_bus[3] = { 0.0f, 0.0f, 0.0f };
float move_bus[3] = { 0.0f, -0.6f, 0.7f };
float scale_bus[3] = { 0.1f, 0.1f, 0.1f };

//трансформации дороги
float rotate_road[3] = { 0.0f, 0.0f, 0.0f };
float move_road[3] = { 0.0f, -0.7f, 0.0f };
float scale_road[3] = { 0.1f, 0.1f, 0.1f };

//трансформации травы
float rotate_grass[3] = { 0.0f, 0.0f, 0.0f };
float move_grass[3] = { -11.5f, -0.7f, 0.0f };
float scale_grass[3] = { 0.1f, 0.1f, 0.1f };
//правой
float rotate_grass_r[3] = { 0.0f, 3.14f, 0.0f };
float move_grass_r[3] = { 11.5f, -0.7f, 0.0f };
float scale_grass_r[3] = { 0.1f, 0.1f, 0.1f };

//горизонт
float rotate_horizon[3] = { 0.0f, 0.0f, 1.57f };
float move_horizon[3] = { 6.0f, 4.0f, 0.0f };
float scale_horizon[3] = { 8.0f, 4.0f, 4.0f };


//Структуры для освещения
//----------------------------------------------------
float viewPosition[3] = { 2.0, 2.0, 5.0 };

struct PointLight {
    float position[3] = { 0.0f, 5.0f, 0.0f };
    float ambient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float attenuation[3] = { 1.0, 0.0, 0.0 };
};

struct Material {
    float ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
    float diffuse[4] = { 0.8, 0.8, 0.8, 1.0 };
    float specular[4] = { 0.0, 0.0, 0.0, 1.0 };
    float emission[4] = { 0.0, 0.0, 0.0, 1.0 };
    float shininess = 0.0;
};

PointLight light;
Material material;

GLint trans_viewPos;
//light
GLint light_pos;
GLint light_amb;
GLint light_diff;
GLint light_spec;
GLint light_atten;
//material
GLint mat_amb;
GLint mat_diff;
GLint mat_spec;
GLint mat_emiss;
GLint mat_shine;



// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
    #version 330 core

    in vec3 vertex_pos;
    in vec2 texcoord;
    in vec3 normal;

	uniform vec3 rotate;
    uniform vec3 move;
    uniform vec3 scale;

    uniform vec3 viewPosition;

	uniform struct PointLight {
		vec3 position;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec3 attenuation;
	} light;
    
	out Vertex {
		vec2 texcoord;
		vec3 normal;
		vec3 lightDir;
		vec3 viewDir;
		float distance;
	} Vert;

    void main() {
 
        vec3 vertex = vertex_pos * mat3(
          cos(3.14159), 0, -sin(3.14159),
            0, 1, 0,
         -sin(3.14159), 0, cos(3.14159)  
        );
        
        vertex *= 
        mat3(
            1, 0, 0,
            0, cos(rotate[0]), -sin(rotate[0]),
            0, sin(rotate[0]), cos(rotate[0])
        ) 
        * mat3(
            cos(rotate[1]), 0, sin(rotate[1]),
            0, 1, 0,
            -sin(rotate[1]), 0, cos(rotate[1])
        ) 
        * mat3(
			cos(rotate[2]), -sin(rotate[2]), 0,
			sin(rotate[2]), cos(rotate[2]), 0,
			0, 0, 1
		) * mat3(
            scale[0], 0, 0,
            0, scale[1], 0,
            0, 0, scale[2]            
        );	

        vec4 vertexmove = vec4(vertex, 1.0) * mat4(
            1, 0, 0, move[0],
            0, 1, 0, move[1],
            0, 0, 1, move[2],
            0, 0, 0, 1
        );

        vec3 lightDir = light.position - vec3(vertexmove);
        
        float c = -1.0f;

        gl_Position = vec4(vertexmove[0], vertexmove[1] - 0.2, vertexmove[2], 1 - vertexmove[2] / c + 0.6);
        Vert.texcoord = texcoord;
        Vert.normal = normal * mat3(
            1, 0, 0,
            0, cos(rotate[0]), -sin(rotate[0]),
            0, sin(rotate[0]), cos(rotate[0])
        ) 
        * mat3(
            cos(rotate[1]), 0, sin(rotate[1]),
            0, 1, 0,
            -sin(rotate[1]), 0, cos(rotate[1])
        ) 
        * mat3(
			cos(rotate[2]), -sin(rotate[2]), 0,
			sin(rotate[2]), cos(rotate[2]), 0,
			0, 0, 1
		);
        Vert.lightDir = vec3(lightDir);
		Vert.viewDir = viewPosition - vec3(vertexmove);
		Vert.distance = length(lightDir);
    }
)";

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
    #version 330 core
    
    out vec4 color;

    uniform struct PointLight {
		vec3 position;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec3 attenuation;
	} light;

	uniform struct Material {
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec4 emission;
		float shininess;
	} material;

    uniform sampler2D textureData;

    in Vertex {
		vec2 texcoord;
		vec3 normal;
		vec3 lightDir;
		vec3 viewDir;
		float distance;
	} Vert;
	
    void main() {
		vec3 normal = normalize(Vert.normal);
		vec3 lightDir = normalize(Vert.lightDir);
		vec3 viewDir = normalize(Vert.viewDir);
	
		float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * Vert.distance + light.attenuation[2] * Vert.distance * Vert.distance);
		color = material.emission;
		color += material.ambient * light.ambient * attenuation;
		float Ndot = max(dot(normal, lightDir), 0.0);
		color += material.diffuse * light.diffuse * Ndot * attenuation;
		float RdotVpow = max(pow(dot(reflect(-lightDir, normal), viewDir), material.shininess), 0.0);
		color += material.specular * light.specular * RdotVpow * attenuation;
		color *= texture(textureData, Vert.texcoord);
    }
)";


void Init();
void Draw();
void Release();



int main() {
    sf::Window window(sf::VideoMode(900, 900), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // Инициализация glew
    glewInit();
    
    Init();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            // обработка нажатий клавиш
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case (sf::Keyboard::Right):
                    if(rotate_bus[1] <= 0.785398)
                        rotate_bus[1] += 0.05;
                        move_bus[0] += 0.08;
                        break;
                case (sf::Keyboard::Left):
                    if(rotate_bus[1] >= -0.785398)
                        rotate_bus[1] -= 0.05; 
                        move_bus[0] -= 0.08;
                        break;
                case (sf::Keyboard::D):
                    if (rotate_road[1] <= 0.785398)
                        rotate_road[1] += 0.05;  break;
                case (sf::Keyboard::A):
                    if (rotate_road[1] >= -0.785398)
                        rotate_road[1] -= 0.05; break;
                case (sf::Keyboard::L):
                    light.position[0] += 0.1;  break;
                case (sf::Keyboard::I):
                    light.position[2] += 0.1;  break;
                case (sf::Keyboard::U):
                    light.position[1] += 0.1;  break;
                case (sf::Keyboard::J):
                    light.position[0] -= 0.1;  break;
                case (sf::Keyboard::K):
                    light.position[2] -= 0.1;  break;
                case (sf::Keyboard::O):
                    light.position[1] -= 0.1;  break;
                default: break;
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                case (sf::Keyboard::Right):
                    rotate_bus[1] = 0.0f; break;
                case (sf::Keyboard::Left):
                    rotate_bus[1] = 0.0f; break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}


// Проверка ошибок OpenGL, если есть то вывод в консоль тип ошибки
void checkOpenGLerror() {
    GLenum errCode;
    // Коды ошибок можно смотреть тут
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// Функция печати лога шейдера
void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    char* infoLog;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        infoLog = new char[infologLen];
        if (infoLog == NULL)
        {
            std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
        std::cout << "InfoLog: " << infoLog << "\n\n\n";
        delete[] infoLog;
    }
}




std::vector<Vertex> verticies;
std::vector<Normale> normales;
std::vector<Texture> textures;
std::vector<PolygPoint> polypoints;

void ReadFile(std::string fname) {
    verticies.clear();
    normales.clear();
    textures.clear();
    polypoints.clear();
    std::ifstream infile(fname);
    std::string dscrp;
    while (infile >> dscrp)
    {
        if (dscrp == "v")
        {
            float x, y, z;
            infile >> x >> y >> z;
            verticies.push_back({ x, y, z });
        }
        if (dscrp == "vt")
        {
            float x, y;
            infile >> x >> y;
            textures.push_back({ x, y });
        }
        if (dscrp == "vn")
        {
            float x, y, z;
            infile >> x >> y >> z;
            normales.push_back({ x, y, z });
        }
        if (dscrp == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                char c;

                int vert_index;
                infile >> vert_index;
                infile >> c;

                int tex_index;
                infile >> tex_index;
                infile >> c;

                int norm_index;
                infile >> norm_index;

                polypoints.push_back({ verticies[vert_index - 1], textures[tex_index - 1], normales[norm_index - 1] });
            }
        }
    }
}

int size_bus_vertex = 0;
int size_road_vertex = 0;
int size_grass_vertex = 0;
int size_horizon_vertex = 0;

void InitVBO()
{
    std::vector<float> points;
    ReadFile("bus2.obj");

    for (int i = 0; i < polypoints.size(); i++)
    {
        points.push_back(polypoints[i].vertex.x);
        points.push_back(polypoints[i].vertex.y);
        points.push_back(polypoints[i].vertex.z);
        points.push_back(polypoints[i].tex.x);
        points.push_back(1 - polypoints[i].tex.y);
        points.push_back(polypoints[i].norm.x);
        points.push_back(polypoints[i].norm.y);
        points.push_back(polypoints[i].norm.z);
        size_bus_vertex += 1;
    }

    // Передаем вершины в буфер
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_texture);
    glEnableVertexAttribArray(Attrib_normal);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(Attrib_texture, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(Attrib_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    
    points.clear();

    //Дорога
    //--------------------------------------------------------
    ReadFile("road.obj");
    
    for (int i = 0; i < polypoints.size(); i++)
    {
        points.push_back(polypoints[i].vertex.x);
        points.push_back(polypoints[i].vertex.y);
        points.push_back(polypoints[i].vertex.z);
        points.push_back(polypoints[i].tex.x);
        points.push_back(polypoints[i].tex.y);
        points.push_back(polypoints[i].norm.x);
        points.push_back(polypoints[i].norm.y);
        points.push_back(polypoints[i].norm.z);
        size_road_vertex += 1;
    }

    // Передаем вершины в буфер
    glGenBuffers(1, &VBO_road);
    glGenVertexArrays(1, &VAO_road);

    glBindVertexArray(VAO_road);

    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_texture);
    glEnableVertexAttribArray(Attrib_normal);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(Attrib_texture, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(Attrib_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

    points.clear();
    //---------------------------------------------------------
    //Трава
    ReadFile("grass.obj");

    for (int i = 0; i < polypoints.size(); i++)
    {
        points.push_back(polypoints[i].vertex.x);
        points.push_back(polypoints[i].vertex.y);
        points.push_back(polypoints[i].vertex.z);
        points.push_back(polypoints[i].tex.x);
        points.push_back(polypoints[i].tex.y);
        points.push_back(polypoints[i].norm.x);
        points.push_back(polypoints[i].norm.y);
        points.push_back(polypoints[i].norm.z);
        size_grass_vertex += 1;
    }

    // Передаем вершины в буфер
    glGenBuffers(1, &VBO_grass);
    glGenVertexArrays(1, &VAO_grass);

    glBindVertexArray(VAO_grass);

    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_texture);
    glEnableVertexAttribArray(Attrib_normal);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_grass);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(Attrib_texture, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(Attrib_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

    points.clear();

    //Горизонт
    ReadFile("box.obj");

    for (int i = 0; i < polypoints.size(); i++)
    {
        points.push_back(polypoints[i].vertex.x);
        points.push_back(polypoints[i].vertex.y);
        points.push_back(polypoints[i].vertex.z);
        points.push_back(1 - polypoints[i].tex.x);
        points.push_back(polypoints[i].tex.y);
        points.push_back(polypoints[i].norm.x);
        points.push_back(polypoints[i].norm.y);
        points.push_back(polypoints[i].norm.z);
        size_horizon_vertex += 1;
    }

    // Передаем вершины в буфер
    glGenBuffers(1, &VBO_horizon);
    glGenVertexArrays(1, &VAO_horizon);

    glBindVertexArray(VAO_horizon);

    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_texture);
    glEnableVertexAttribArray(Attrib_normal);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_horizon);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(Attrib_texture, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(Attrib_normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

    points.clear();
    checkOpenGLerror();
}



void InitShader() {
    // Создаем вершинный шейдер
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // Передаем исходный код
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    // Компилируем шейдер
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);

    // Создаем фрагментный шейдер
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Передаем исходный код
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    // Компилируем шейдер
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);

    // Создаем программу и прикрепляем шейдеры к ней
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);

    // Линкуем шейдерную программу
    glLinkProgram(Program);
    // Проверяем статус сборки
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }
    //АТРИБУТЫ
    //-----------------------------------------------------------------
    Attrib_vertex = glGetAttribLocation(Program, "vertex_pos");
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib vertex_pos" << std::endl;
        return;
    }
    Attrib_texture = glGetAttribLocation(Program, "texcoord");
    if (Attrib_texture == -1)
    {
        std::cout << "could not bind attrib texcoord" << std::endl;
        return;
    }
    Attrib_normal = glGetAttribLocation(Program, "normal");
    if (Attrib_normal == -1)
    {
        std::cout << "could not bind attrib normal" << std::endl;
        return;
    }

    //-----------------------------------------------------------------
    //ЮНИФОРМЫ
    //-----------------------------------------------------------------
    //ставлю юниформу вращения
    Unif_rotate = glGetUniformLocation(Program, "rotate");
    if (Unif_rotate == -1)
    {
        std::cout << "could not bind uniform rotate " << std::endl;
        return;
    }
    Unif_move = glGetUniformLocation(Program, "move");
    if (Unif_move == -1)
    {
        std::cout << "could not bind uniform move" << std::endl;
        return;
    }
    //ставлю юниформу текстуры
    Unif_scale = glGetUniformLocation(Program, "scale");
    if (Unif_scale == -1)
    {
        std::cout << "could not bind uniform scale " << std::endl;
        return;
    }
    Unif_texture = glGetUniformLocation(Program, "textureData");
    if (Unif_texture == -1)
    {
        std::cout << "could not bind uniform textureData" << std::endl;
        return;
    }
    //ставлю юниформы преобразований
    trans_viewPos = glGetUniformLocation(Program, "viewPosition");
    if (trans_viewPos == -1)
    {
        std::cout << "could not bind uniform viewPosition " << std::endl;
        return;
    }
    //ставлю юниформы точки света
    light_pos = glGetUniformLocation(Program, "light.position");
    if (light_pos == -1)
    {
        std::cout << "could not bind uniform light.position " << std::endl;
        return;
    }
    light_amb = glGetUniformLocation(Program, "light.ambient");
    if (light_amb == -1)
    {
        std::cout << "could not bind uniform light.ambient " << std::endl;
        return;
    }
    light_diff = glGetUniformLocation(Program, "light.diffuse");
    if (light_diff == -1)
    {
        std::cout << "could not bind uniform light.diffuse " << std::endl;
        return;
    }
    light_spec = glGetUniformLocation(Program, "light.specular");
    if (light_spec == -1)
    {
        std::cout << "could not bind uniform light.specular " << std::endl;
        return;
    }
    light_atten = glGetUniformLocation(Program, "light.attenuation");
    if (light_atten == -1)
    {
        std::cout << "could not bind uniform light.attenuation " << std::endl;
        return;
    }
    //материал
    mat_amb = glGetUniformLocation(Program, "material.ambient");
    if (mat_amb == -1)
    {
        std::cout << "could not bind uniform material.ambient " << std::endl;
        return;
    }
    mat_diff = glGetUniformLocation(Program, "material.diffuse");
    if (mat_diff == -1)
    {
        std::cout << "could not bind uniform material.diffuse" << std::endl;
        return;
    }
    mat_spec = glGetUniformLocation(Program, "material.specular");
    if (mat_spec == -1)
    {
        std::cout << "could not bind uniform material.specular" << std::endl;
        return;
    }
    mat_emiss = glGetUniformLocation(Program, "material.emission");
    if (mat_emiss == -1)
    {
        std::cout << "could not bind uniform material.emission" << std::endl;
        return;
    }
    mat_shine = glGetUniformLocation(Program, "material.shininess");
    if (mat_shine == -1)
    {
        std::cout << "could not bind uniform material.shininess" << std::endl;
        return;
    }
    //-----------------------------------------------------------------
    checkOpenGLerror();
}

void InitTexture()
{
    const char* filename = "bus2.png";
    if (!textureData_bus.loadFromFile(filename))
    {
        return;
    }
    textureHandle = textureData_bus.getNativeHandle();

    filename = "road.png";
    if (!textureData_road.loadFromFile(filename))
    {
        return;
    }
    textureHandle_road = textureData_road.getNativeHandle();

    filename = "grass.png";
    if (!textureData_grass.loadFromFile(filename))
    {
        return;
    }
    textureHandle_grass = textureData_grass.getNativeHandle();

    filename = "skybox.png";
    if (!textureData_horizon.loadFromFile(filename))
    {
        return;
    }
    textureHandle_horizon = textureData_horizon.getNativeHandle();
}

void Init() {
    InitShader();
    InitVBO();
    InitTexture();
}

void Draw() {
    // Устанавливаем шейдерную программу текущей
    glUseProgram(Program);
    // Включаем массивы атрибутов

    glEnable(GL_DEPTH_TEST);

    //видимая позиция
    glUniform3fv(trans_viewPos, 1, viewPosition);
    //свет
    glUniform3fv(light_pos, 1, light.position);
    glUniform4fv(light_amb, 1, light.ambient);
    glUniform4fv(light_diff, 1, light.diffuse);
    glUniform4fv(light_spec, 1, light.specular);
    glUniform3fv(light_atten, 1, light.attenuation);
    //материал
    glUniform4fv(mat_amb, 1, material.ambient);
    glUniform4fv(mat_diff, 1, material.diffuse);
    glUniform4fv(mat_spec, 1, material.specular);
    glUniform4fv(mat_emiss, 1, material.emission);
    glUniform1f(mat_shine, material.shininess);


    //рисую абобус
    glUniform3fv(Unif_rotate, 1, rotate_bus);
    glUniform3fv(Unif_move, 1, move_bus);
    glUniform3fv(Unif_scale, 1, scale_bus);
    glActiveTexture(GL_TEXTURE0);
    sf::Texture::bind(&textureData_bus);
    glUniform1i(Unif_texture, 0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, size_bus_vertex);

    //рисую дорогу
    glUniform3fv(Unif_rotate, 1, rotate_road);
    glUniform3fv(Unif_move, 1, move_road);
    glUniform3fv(Unif_scale, 1, scale_road);
    glActiveTexture(GL_TEXTURE0);
    sf::Texture::bind(&textureData_road);
    glUniform1i(Unif_texture, 0);
    glBindVertexArray(VAO_road);
    glDrawArrays(GL_TRIANGLES, 0, size_road_vertex);

    //рисую траву
    glUniform3fv(Unif_rotate, 1, rotate_grass);
    glUniform3fv(Unif_move, 1, move_grass);
    glUniform3fv(Unif_scale, 1, scale_grass);
    glActiveTexture(GL_TEXTURE0);
    sf::Texture::bind(&textureData_grass);
    glUniform1i(Unif_texture, 0);
    glBindVertexArray(VAO_grass);
    glDrawArrays(GL_TRIANGLES, 0, size_grass_vertex);

    glUniform3fv(Unif_rotate, 1, rotate_grass_r);
    glUniform3fv(Unif_move, 1, move_grass_r);
    glUniform3fv(Unif_scale, 1, scale_grass_r);
    glDrawArrays(GL_TRIANGLES, 0, size_grass_vertex);

    //рисую горизонт
    glUniform3fv(Unif_rotate, 1, rotate_horizon);
    glUniform3fv(Unif_move, 1, move_horizon);
    glUniform3fv(Unif_scale, 1, scale_horizon);
    glActiveTexture(GL_TEXTURE0);
    sf::Texture::bind(&textureData_horizon);
    glUniform1i(Unif_texture, 0);
    glBindVertexArray(VAO_horizon);
    glDrawArrays(GL_TRIANGLES, 0, size_horizon_vertex);

    glUseProgram(0);
    checkOpenGLerror();
}


// Освобождение шейдеров
void ReleaseShader() {
    // Передавая ноль, мы отключаем шейдрную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

// Освобождение буфера
void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO_road);
    glDeleteBuffers(1, &VBO_road);
    glDeleteVertexArrays(1, &VAO_grass);
    glDeleteBuffers(1, &VBO_grass);
    glDeleteVertexArrays(1, &VAO_horizon);
    glDeleteBuffers(1, &VBO_horizon);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}
