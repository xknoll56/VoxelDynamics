#include <GL/glew.h>    
#include <GLFW/glfw3.h>
#include "VoxelDynamicsSimulation.h"
#include "Shaders.h"
#include <iostream>
#include <vector>
#include <random>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "FastNoiseLite.h"


VDuint WINDOW_WIDTH = 1268;
VDuint WINDOW_HEIGHT = 724;
float aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;


bool keys[1024];
bool mouse[32];
bool keysDown[1024];
bool mouseDown[32];
double mouseX;
double mouseY;
float dYaw = 0.0f;
float dPitch = 0.0f;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keys[key] = true;
        keysDown[key] = true;
    }

    if (action == GLFW_RELEASE)
    {
        keys[key] = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        mouse[button] = true;
        mouseDown[button] = true;
    }

    if (action == GLFW_RELEASE)
    {
        mouse[button] = false;
    }
}



struct Camera
{
    VDVector3 right, up, forward;
    float cameraSpeed = 1.0f;
    float pitch, yaw;
    VDVector3 position;
    VDMatrix4 view;

    struct CameraControls
    {
        unsigned int up;
        unsigned int down;
        unsigned int left;
        unsigned int right;
        unsigned int forward;
        unsigned int back;
        unsigned int run;

        void setDefault()
        {
            up = GLFW_KEY_END;
            down = GLFW_KEY_RIGHT_SHIFT;
            right = GLFW_KEY_RIGHT;
            left = GLFW_KEY_LEFT;
            forward = GLFW_KEY_UP;
            back = GLFW_KEY_DOWN;
            run = GLFW_KEY_RIGHT_SHIFT;
        }

        void setWASD()
        {
            up = GLFW_KEY_E;
            down = GLFW_KEY_Q;
            right = GLFW_KEY_D;
            left = GLFW_KEY_A;
            forward = GLFW_KEY_W;
            back = GLFW_KEY_S;
            run = GLFW_KEY_LEFT_SHIFT;
        }
    };

    CameraControls  controls;

    void setDirectionalVectors()
    {
        forward.z = cosf(yaw) * cosf(pitch);
        forward.y = sinf(pitch);
        forward.x = sinf(yaw) * cosf(pitch);
        forward.normalize();
        right = VDNormalize(VDCross(forward, VDVector3::up()));
        up = VDNormalize(VDCross(right, forward));
    }

    void setViewMatrix()
    {
        view = VDLookAt(position, position + forward, up);
    }

    void handleInputs(float dt)
    {
        float speed = cameraSpeed;
        if (keys[controls.run])
            speed *= 3.0f;
        if (keys[controls.forward])
        {
            position += forward * (speed * dt);
        }
        if (keys[controls.back])
        {
            position += forward * (-speed * dt);
        }
        if (keys[controls.right])
        {
            position += right * (speed * dt);
        }
        if (keys[controls.left])
        {
            position += right * (-speed * dt);
        }

        yaw -= dYaw * dt;
        pitch -= dPitch * dt;
        if (VDAbs(pitch) > PI * 0.475f)
            pitch = VDSign(pitch) * PI * 0.475f;
        setDirectionalVectors();
        setViewMatrix();
    }

    void updateViewAngles(float dt)
    {
        yaw -= dYaw * dt;
        pitch -= dPitch * dt;
        if (VDAbs(pitch) > PI * 0.475f)
            pitch = VDSign(pitch) * PI * 0.475f;
        setDirectionalVectors();
        setViewMatrix();
    }

    Camera(VDVector3 position)
    {
        pitch = 0.0f;
        yaw = 0.0f;
        setDirectionalVectors();
        this->position = position;
        setViewMatrix();
        controls.setDefault();
    }

    Camera() : Camera(VDVector3())
    {
        
    }
};

struct VertexBuffer
{
    GLuint VertexArrayId;
    GLuint vertexbufferId;
    int numVerts;

    void bind() const
    {
        glBindVertexArray(VertexArrayId);
    }

    void init(float* vertexData, int numVerts)
    {
        this->numVerts = numVerts;
        glGenVertexArrays(1, &VertexArrayId);
        glBindVertexArray(VertexArrayId);

        
        glGenBuffers(1, &vertexbufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8*numVerts, vertexData, GL_STATIC_DRAW);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            8 * sizeof(float),                  // stride
            (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            8 * sizeof(float),                  // stride
            (void*)(3 * sizeof(float))            // array buffer offset
        );

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            8 * sizeof(float),                  // stride
            (void*)(6 * sizeof(float))            // array buffer offset
        );
    }

    void initLines(float* vertexData, int numVerts)
    {
        this->numVerts = numVerts;
        glGenVertexArrays(1, &VertexArrayId);
        glBindVertexArray(VertexArrayId);


        glGenBuffers(1, &vertexbufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVerts, vertexData, GL_STATIC_DRAW);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            3 * sizeof(float),                  // stride
            (void*)0            // array buffer offset
        );
    }

    void draw() const 
    {
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
    }

    void draw(GLuint drawMode) const
    {
        glDrawArrays(drawMode, 0, numVerts);
    }

    //~VertexBuffer()
    //{
    //    glDeleteBuffers(1, &vertexbufferId);
    //    glDeleteVertexArrays(1, &VertexArrayId);
    //}
};

Shader shader;
Shader wireShader;
Shader instancedShader;
VDMatrix4 viewProjection;
VDMatrix4 proj;
VertexBuffer vbOrigin;
VertexBuffer vbPositiveQuadrant;
VertexBuffer vbWire;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
    proj = VDPerspective(aspect, PI * 0.25f, 0.1f, 1000.0f);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}


struct VoxelRenderResource
{
    VDuint arrayPos;
    VDVoxel* pVoxel;
};


struct InstanceBufferData
{
    std::vector<VDVector3> instancePositions;
    std::vector<int> instanceTexts;
    VDList<VDuint> freeIndices;

    void reserve(VDuint capacity)
    {
        instancePositions.reserve(capacity);
        instanceTexts.reserve(capacity);
    }

    InstanceBufferData(VDuint capacity)
    {
        reserve(capacity);
    }

    InstanceBufferData()
    {
        reserve(20);
    }

    VDuint insert(VDVector3 position, int textInd)
    {
        //First checck if there are any free indices
        if (freeIndices.count > 0)
        {
            VDuint ind = freeIndices.pop();
            instancePositions[ind] = position;
            instanceTexts[ind] = textInd;
            return ind;
        }

        if (instancePositions.size() >= instancePositions.capacity())
        {
            //double the capacity
            instancePositions.reserve(2 * instancePositions.size());
            instanceTexts.reserve(2 * instanceTexts.size());
        }
        instancePositions.push_back(position);
        instanceTexts.push_back(textInd);
        return instancePositions.size() - 1;
    }

    void insertVoxel(VDVoxel* pVoxel, int textInd = 0)
    {
        VDuint ind = insert(pVoxel->lowPosition, textInd);
        pVoxel->userData.value = ind;
    }

    void remove(VDuint index)
    {
        freeIndices.insert(index);
        // Set the texture to -2 to signify unnoccupied to the shader
        instanceTexts[index] = -2;
    }

    void removeVoxel(VDVoxel* pVoxel)
    {
        VDuint index = pVoxel->userData.value;
        remove(index);
    }
};


struct InstanceBuffer : VertexBuffer
{
    GLuint instanceVBO;
    GLuint instanceTextsVBO;
    unsigned int numInstances;
    VDuint capacity;
    InstanceBufferData data;



    void init(const VertexBuffer& vertBuffer, std::vector<VDVector3> instancePositions, std::vector<int> instanceTexts, VDuint capacity)
    {
        
        glGenVertexArrays(1, &VertexArrayId);
        glBindVertexArray(VertexArrayId);
        vertexbufferId = vertBuffer.vertexbufferId;


        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            8 * sizeof(float),                  // stride
            (void*)0            // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            8 * sizeof(float),                  // stride
            (void*)(3 * sizeof(float))            // array buffer offset
        );

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
        glVertexAttribPointer(
            2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            8 * sizeof(float),                  // stride
            (void*)(6 * sizeof(float))            // array buffer offset
        );

        numVerts = vertBuffer.numVerts;

        this->numInstances = instancePositions.size();
        this->capacity = capacity;
        //glBindVertexArray(0);
        //Create the instance buffer
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        std::vector<VDVector3> dummydata;
        dummydata.reserve(capacity);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VDVector3) * capacity, dummydata.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glVertexAttribPointer(
            3,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            sizeof(VDVector3),                // stride
            (void*)0                          // array buffer offset
        );


        //Create the instance buffer

        glGenBuffers(1, &instanceTextsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceTextsVBO);
        std::vector<int> dummyindices;
        dummyindices.reserve(capacity);
        glBufferData(GL_ARRAY_BUFFER, sizeof(int) * capacity, dummyindices.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, instanceTextsVBO);
        glVertexAttribIPointer(
            4,                // attribute index. This must match the layout in the shader.
            1,                // size: 1 component (the texture index)
            GL_INT,           // type: integer
            sizeof(int),      // stride: byte offset between consecutive instances
            (void*)0          // offset: start at the beginning of the buffer
        );

        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 0);
        glVertexAttribDivisor(2, 0);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);

        data.instancePositions = instancePositions;
        data.instanceTexts = instanceTexts;

        // Now update the date
        updateInstanceBuffer();

        glBindVertexArray(0);
    }

    void updateInstanceBuffer()
    {
        numInstances = data.instancePositions.size();
        if (numInstances < capacity)
        {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VDVector3) * data.instancePositions.size(), data.instancePositions.data());

            glBindBuffer(GL_ARRAY_BUFFER, instanceTextsVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(int) * data.instanceTexts.size(), data.instanceTexts.data());
        }
    }

    int getTextIndValueAtIndex(VDuint index)
    {
        glBindBuffer(GL_ARRAY_BUFFER, instanceTextsVBO);
        int data;
        glGetBufferSubData(GL_ARRAY_BUFFER, index*sizeof(int), sizeof(int), &data);
        return data;
    }

    VDVector3 getPositionValueAtIndex(VDuint index)
    {
        VDVector3 data;
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glGetBufferSubData(GL_ARRAY_BUFFER, index * sizeof(VDVector3), sizeof(VDVector3), &data);
        return data;
    }

    void updateInstanceBufferAtIndex(VDuint index, VDVector3 position, int texInd)
    {
        data.instancePositions[index] = position;
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, index*sizeof(VDVector3), sizeof(VDVector3), &position);

        data.instanceTexts[index] = texInd;
        glBindBuffer(GL_ARRAY_BUFFER, instanceTextsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, index*sizeof(int), sizeof(int), &texInd);
    }

    //void bind() const
    //{
    //    VertexBuffer::bind();
    //    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferId);
    //    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    //    glBindBuffer(GL_ARRAY_BUFFER, instanceTextsVBO);
    //}

    void draw() const
    {
        glDrawArraysInstanced(GL_TRIANGLES, 0, numVerts, numInstances);
    }

    static InstanceBuffer instanceBufferFromChunk(VDGrid& chunk, VDuint capacity)
    {
        std::vector<int> texts;
        VDList<VDVoxel*> voxelList = chunk.getOccupiedVoxels();
        voxelList.autoFree = true;
        std::vector<VDVoxel*> occupiedVector = voxelList.toVector();
        std::vector<VDVector3> posesVec;
        
        InstanceBuffer ib;
        for (int i = 0; i < occupiedVector.size(); i++)
        {
            posesVec.push_back(occupiedVector[i]->lowPosition);
            texts.push_back(0);
            occupiedVector[i]->userData = (VDPointer)i;
            chunk.setVoxel(occupiedVector[i]->index, *occupiedVector[i]);
        }
        ib.init(vbPositiveQuadrant, posesVec, texts, capacity);
        return ib;
    }


};

struct TextureArray
{
    GLuint textureArray;
    GLuint textureUnit;

    void init(std::vector<std::string> textureNames, std::string dirPath, GLuint textureUnit)
    {
        // Create the texture array
        glGenTextures(1, &textureArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

        // Allocate storage
        int layerCount = textureNames.size(); // for example

        // Load each texture into the array
        for (int i = 0; i < layerCount; ++i) {
            int w, h, nrChannels;
            unsigned char* data = stbi_load((dirPath + textureNames[i]).c_str(), &w, &h, &nrChannels, 0);
            if (!data) {
                std::cerr << "Failed to load texture" << std::endl;
                // Handle the error
            }
            if (i == 0)
                glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, w, h, layerCount);
            // Upload the texture data to the array
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

            // Free the image memory
            stbi_image_free(data);
        }

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Bind the texture array to a texture unit
        this->textureUnit = textureUnit;
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    }

    void initCheckersTexture(GLuint textureUnit, int width = 4, int height = 4, int checkerSize = 16)
    {
        // Create the texture array
        glGenTextures(1, &textureArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

        // Generate the checkers pattern
        int w = width * checkerSize;
        int h = height * checkerSize;
        std::vector<unsigned char> data(w * h * 4);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int checkerX = x / checkerSize;
                int checkerY = y / checkerSize;
                bool isWhite = (checkerX + checkerY) % 2 == 0;

                int index = (y * w + x) * 4;
                data[index] = isWhite ? 255 : 0;     // Red
                data[index + 1] = isWhite ? 255 : 0; // Green
                data[index + 2] = isWhite ? 255 : 0; // Blue
                data[index + 3] = 255;               // Alpha
            }
        }

        // Allocate storage and upload the texture
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, w, h, 1);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Bind the texture array to a texture unit
        this->textureUnit = textureUnit;
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    }

    void bind() const
    {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    }

};

TextureArray texArr;
TextureArray oreTexArr;

void movePositionWithArrows(const Camera& camera, VDVector3& position, float dt, float speed)
{
    VDVector3 forwardXZ = { camera.forward.x, 0.0f, camera.forward.z };
    forwardXZ.normalize();
    if (keys[GLFW_KEY_UP])
    {
        position += forwardXZ * dt * speed;
    }
    if (keys[GLFW_KEY_RIGHT])
    {
        position += camera.right * dt * speed;
    }
    if (keys[GLFW_KEY_LEFT])
    {
        position += camera.right * dt * -speed;
    }
    if (keys[GLFW_KEY_DOWN])
    {
        position += forwardXZ * dt * -speed;
    }
    if (keys[GLFW_KEY_END])
    {
        position += VDVector3::up() * dt * speed;
    }
    if (keys[GLFW_KEY_RIGHT_SHIFT])
    {
        position += VDVector3::up() * dt * -speed;
    }
}

void moveBodyWithArrows(const Camera& camera, VDBody& body, float dt, float speed)
{
    VDVector3 forwardXZ = { camera.forward.x, 0.0f, camera.forward.z };
    forwardXZ.normalize();
    if (keys[GLFW_KEY_UP])
    {
        body.translate(forwardXZ * dt * speed);
    }
    if (keys[GLFW_KEY_RIGHT])
    {
        body.translate(camera.right * dt * speed);
    }
    if (keys[GLFW_KEY_LEFT])
    {
        body.translate(camera.right * dt * -speed);
    }
    if (keys[GLFW_KEY_DOWN])
    {
        body.translate(forwardXZ * dt * -speed);
    }
    if (keysDown[GLFW_KEY_SPACE])
    {
        body.deltaMomentums.insert(VDVector3::up() * 5.0f);
    }
}

void moveAgentWithArrows(const Camera& camera, VDAgentController& agent, float dt, float speed)
{
    VDVector3 forwardXZ = { camera.forward.x, 0.0f, camera.forward.z };
    forwardXZ.normalize();
    float accel = 1.0f;
    if (keys[GLFW_KEY_LEFT_SHIFT])
    {
        accel = 2.0f;
    }
    if (keys[GLFW_KEY_W])
    {
        agent.move(forwardXZ , dt*accel);
    }
    if (keys[GLFW_KEY_D])
    {
        agent.move(camera.right, dt * accel);
    }
    if (keys[GLFW_KEY_A])
    {
        agent.move(camera.right, -dt * accel);
    }
    if (keys[GLFW_KEY_S])
    {
        agent.move(forwardXZ, -dt * accel);
    }
    if (keysDown[GLFW_KEY_SPACE])
    {
        agent.jump(5.0f);
    }
}



void drawTranslatedVertexBuffer(const VertexBuffer& vertexBuffer, VDVector3 translation, VDVector3 color)
{
    shader.use();
    shader.setUniformFloat("colorMix", 1.0f);
    shader.setUniformVector3("solidColor", color);
    shader.setUniformMatrix4("mvp", VDTranslation(translation)*viewProjection);
    vertexBuffer.bind();
    vertexBuffer.draw();
}

void drawTranslatedBox(VDVector3 translation, VDVector3 color)
{
    drawTranslatedVertexBuffer(vbOrigin, translation, color);
}

void drawVertexBuffer(const VertexBuffer& vertexBuffer, VDVector3 translation, VDVector3 euler, VDVector3 scale, VDVector3 color, GLenum mode = GL_TRIANGLES)
{
    shader.use();
    shader.setUniformFloat("colorMix", 1.0f);
    shader.setUniformVector3("solidColor", color);
    VDMatrix4 model =  VDScale(scale) * VDRotate(euler) * VDTranslation(translation);
    shader.setUniformMatrix4("mvp", model * viewProjection);
    vertexBuffer.bind();
    vertexBuffer.draw(mode);
}

void drawSolidAABB(const VDAABB& aabb, VDVector3 color)
{
    shader.use();
    shader.setUniformFloat("colorMix", 1.0f);
    shader.setUniformVector3("solidColor", color);
    VDMatrix4 model = VDScale(aabb.halfExtents*2.0f) * VDTranslation(aabb.position);
    shader.setUniformMatrix4("mvp", model * viewProjection);
    vbOrigin.bind();
    vbOrigin.draw();
}

void drawTranslatedWireFrameVertexBuffer(const VertexBuffer& vertexBuffer, VDVector3 translation, VDVector3 color)
{
    wireShader.use();
    wireShader.setUniformVector3("solidColor", color);
    wireShader.setUniformMatrix4("mvp", VDTranslation(translation)*viewProjection);
    vertexBuffer.bind();
    vertexBuffer.draw(GL_LINES);
}

void drawVoxel(const VDVoxel& voxel, VDVector3 color)
{
    wireShader.use();
    wireShader.setUniformVector3("solidColor", color);
    VDVector3 translation = voxel.toAABB().position;
    wireShader.setUniformMatrix4("mvp", VDTranslation(translation) * viewProjection);
    vbWire.bind();
    vbWire.draw(GL_LINES);
}

void drawWireFrameVertexBuffer(const VertexBuffer& vertexBuffer, VDVector3 translation, VDVector3 euler, VDVector3 scale, VDVector3 color)
{
    wireShader.use();
    wireShader.setUniformVector3("solidColor", color);
    VDMatrix4 model = VDScale(scale) * VDRotate(euler) * VDTranslation(translation);
    wireShader.setUniformMatrix4("mvp", model * viewProjection);
    vertexBuffer.bind();
    vertexBuffer.draw(GL_LINES);
}

void drawChunkOutline(const VDGrid& chunk, VDVector3 color, bool fill = false)
{
    VDVector3 halfExtents((float)chunk.gridSize * 0.5f,
        (float)chunk.gridSize * 0.5f, (float)chunk.gridSize * 0.5f);
    VDVector3 fullExtents = halfExtents * 2.0f;
    VertexBuffer vb = vbWire;
    GLenum mode = GL_LINES;
    if (fill)
    {
        vb = vbOrigin;
        mode = GL_TRIANGLES;
    }
    drawVertexBuffer(vb, chunk.low + halfExtents, { 0,0,0 }, fullExtents, color, mode);
}

void drawSpace(const VDSpace& space, VDVector3 color)
{
    VDVector3 halfExtents((float)space.gridSize * 0.5f,
        (float)space.gridSize * 0.5f, (float)space.gridSize * 0.5f);
    VDVector3 fullExtents = halfExtents * 2.0f;
    for (VDuint z = 0; z < space.horizontalGrids; z++)
    {
        for (VDuint y = 0; y < space.verticalGrids; y++)
        {
            for (VDuint x = 0; x < space.horizontalGrids; x++)
            {
                VDVector3i anchor = space.anchor + VDVector3i(x, y, z) * space.gridSize;
                drawWireFrameVertexBuffer(vbWire, anchor + halfExtents, { 0,0,0 }, fullExtents, color);
            }
        }
    }
}

void drawInstanceBuffer(const InstanceBuffer& instanceBuffer, const TextureArray& texArr)
{
    instancedShader.use();
    texArr.bind();
    instancedShader.setUniformInt("textureArray", 0);
    instancedShader.setUniformVector3("solidColor", { 0,1,0 });
    instancedShader.setUniformMatrix4("mvp", viewProjection);
    instanceBuffer.bind();
    instanceBuffer.draw();
}

void drawInstanceBuffer(const InstanceBuffer& instanceBuffer, VDVector3 color)
{
    instancedShader.use();
    instancedShader.setUniformInt("textureArray", 0);
    instancedShader.setUniformVector3("solidColor", color);
    instancedShader.setUniformMatrix4("mvp", viewProjection);
    instanceBuffer.bind();
    instanceBuffer.draw();
}

void serializeChunk(VDGrid& chunk, std::string outPath)
{
    std::ofstream outputFile(outPath);

    // Check if the file was successfully opened
    if (!outputFile) {
        std::cerr << "Error: Could not open the file for writing!" << std::endl;
    }

    // Write some data to the file
    outputFile << chunk.gridSize << std::endl;
    outputFile << chunk.low.x << " " << chunk.low.y << " " << chunk.low.z << std::endl;

    for (VDuint z = 0; z < chunk.gridSize; z++)
    {
        for (VDuint y = 0; y < chunk.gridSize; y++)
        {
            std::string row = "";
            bool rowEmpty = true;
            for (VDuint x = 0; x < chunk.gridSize; x++)
            {
                if (chunk.getOccupied(x, y, z))
                {
                    row += "1";
                    rowEmpty = false;
                }
                else
                {
                    row += "0";
                }
            }
            if (!rowEmpty)
            {
                outputFile << y << " " << z << " " << row << std::endl;
            }
        }
    }

    // Close the file stream
    outputFile.close();
}

std::vector<std::string> split(const std::string& str)
{
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string word;
    while (stream >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

VDGrid deserializeChunk(std::string inPath)
{
    std::ifstream inputFile(inPath);

    if (!inputFile) {
        std::cerr << "Error: Could not open the file for reading!" << std::endl;
        return VDGrid(0, VDVector3(), 0);
    }

    std::string line;

    std::getline(inputFile, line);
    VDuint gridSize = (VDuint)std::stoul(line);
    std::getline(inputFile, line);
    std::vector<std::string> splitString = split(line);
    VDVector3 anchor(std::stof(splitString[0]), std::stof(splitString[1]), std::stof(splitString[2]));
    VDGrid chunk(gridSize, anchor, 0);
    while (std::getline(inputFile, line))
    {
        std::vector<std::string> splitLine = split(line);
        VDuint y = (VDuint)std::stoul(splitLine[0]);
        VDuint z = (VDuint)std::stoul(splitLine[1]);
        for (VDuint x = 0; x < gridSize; x++)
        {
            std::string occupiedS = "0";
            occupiedS[0] = splitLine[2][x];
            VDuint occupied = (VDuint)std::stoul(occupiedS);
            if (occupied)
            {
                chunk.setOccupied(x, y, z);
            }
        }

    }

    inputFile.close();
    return chunk;
}

VDGrid generateChunkWithNoise(VDuint chunkIndex, VDuint gridSize, VDVector3 low, VDuint hMax, int seed = 0)
{
	VDGrid chunk(gridSize, low, chunkIndex);
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(seed);
	int index = 0;
	for (int y = 0; y < gridSize; y++)
	{
		for (int x = 0; x < gridSize; x++)
		{
            VDuint h = (1 + noise.GetNoise((float)x, (float)y)) * (float)hMax;
			for (int k = 0; k <= h; k++)
			{
				chunk.setOccupied(x, k, y);
			}

		}
	}
    return chunk;
}

struct Scene
{
    bool freeCamera = true;
    Scene()
    {
        camera = Camera(VDVector3(0, 2, -6));
        camera.cameraSpeed = 3.0f;
        camera.controls.setWASD();
    }
    Camera camera;
    virtual void init() = 0;
    virtual void update(float dt)
    {
        if (freeCamera)
            camera.handleInputs(dt);
        if (keysDown[GLFW_KEY_0])
            freeCamera = !freeCamera;
    }
    virtual void draw(float dt)
    {
        
    }
};
