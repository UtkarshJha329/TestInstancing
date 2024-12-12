
#include <iostream>
#include <chrono>
#include <future>

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "Plane.h"

#define GLSL_VERSION            430

#include <vector>

#include "CubeMeshData.h"
#include "DrawMeshInstancedFlattenedTransforms.h"

void PlaneFacingDir(Vector3 dir, GenerativeMesh & curMesh);

static void DrawChunk(Shader instanceShader, Material instancedMaterial
    , std::vector<int> &megaArrayOfAllPosition
    , GenerativeMesh& renderQuad);

Vector3 up = { 0, 1, 0 };
Vector3 down = { 0, -1, 0 };
Vector3 front = { 0, 0, 1 };
Vector3 back = { 0, 0, -1 };
Vector3 right = { 1, 0, 0 };
Vector3 left = { -1, 0, 0 };

std::vector<DrawArraysIndirectCommand> drawArraysIndirectCommands;

int main()
{
    InitWindow(800, 450, "raylib [core] example - basic window");

    Camera camera = { { 31.0f, 31.0f, 31.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };

    std::vector<int> megaArrayOfAlPositions;

    for (int i = 0; i < 1; i++)
    {
        int curRandPosition = GetRandomValue(0, 31) << 10 | GetRandomValue(0, 31) << 5 | GetRandomValue(0, 31);
        megaArrayOfAlPositions.push_back(curRandPosition);
    }

    GenerativeMesh renderQuad = { 0 };
    PlaneFacingDir(up, renderQuad);

    // Load lighting instanceShader
    Shader instanceShader = LoadShader(TextFormat("Shaders/lighting_instancing.vert", GLSL_VERSION),
        TextFormat("Shaders/lighting.frag", GLSL_VERSION));

    // Get instanceShader locations
    instanceShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(instanceShader, "mvp");
    instanceShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(instanceShader, "viewPos");
    instanceShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(instanceShader, "instanceTransform");

    Material instancedMaterial = LoadMaterialDefault();
    instancedMaterial.shader = instanceShader;
    
    DisableCursor();

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);

        float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
        SetShaderValue(instancedMaterial.shader, instancedMaterial.shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        rlEnableShader(instancedMaterial.shader.id);
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            DrawChunk(instanceShader, instancedMaterial, megaArrayOfAlPositions, renderQuad);

            DrawGrid(10, 1.0);
            EndMode3D();
            DrawFPS(40, 40);

        EndDrawing();
    }

    UnloadShader(instanceShader);
    CloseWindow();
    return 0;
}

static void DrawChunk(Shader instanceShader, Material instancedMaterial
    , std::vector<int> &megaArrayOfAllPositions
    , GenerativeMesh &renderQuad) {

    DrawArraysIndirectCommand curCommand1 = { 4, megaArrayOfAllPositions.size(), 0, 0 };
    drawArraysIndirectCommands.push_back(curCommand1);

    rlEnableVertexArray(renderQuad.mesh.vaoId);
    unsigned int indirectBuffer = rlLoadDrawBufferIndirect(drawArraysIndirectCommands.size(), drawArraysIndirectCommands.data(), false);
    
    DrawMeshMultiInstancedDrawIndirect(renderQuad, instancedMaterial, megaArrayOfAllPositions.data(), megaArrayOfAllPositions.size(), indirectBuffer, drawArraysIndirectCommands.data(), drawArraysIndirectCommands.size());

    rlUnloadVertexBuffer(indirectBuffer);

}

void PlaneFacingDir(Vector3 dir, GenerativeMesh &curMesh) {

    curMesh.mesh.vertices = (float*)MemAlloc(4 * 3 * sizeof(float));
    curMesh.mesh.texcoords = (float*)MemAlloc(4 * 2 * sizeof(float));
    //curMesh.indices = (unsigned short*)MemAlloc(6 * sizeof(unsigned short*));

    if (dir.x == 0 && dir.y == 1 && dir.z == 0) {
        //FaceIndicesTop(curMesh.indices, 0);
        FaceVerticesTop(curMesh.mesh.vertices, 0, 0, 0);
    }
    else if (dir.x == 0 && dir.y == -1 && dir.z == 0) {
        //FaceIndicesBottom(curMesh.indices, 0);
        FaceVerticesBottom(curMesh.mesh.vertices, 0, 0, 0);
    }
    else if (dir.x == 0 && dir.y == 0 && dir.z == 1) {
        //FaceIndicesFront(curMesh.indices, 0);
        FaceVerticesFront(curMesh.mesh.vertices, 0, 0, 0);
    }
    else if (dir.x == 0 && dir.y == 0 && dir.z == -1) {
        //FaceIndicesBack(curMesh.indices, 0);
        FaceVerticesBack(curMesh.mesh.vertices, 0, 0, 0);
    }
    else if (dir.x == 1 && dir.y == 0 && dir.z == 0) {
        //FaceIndicesRight(curMesh.indices, 0);
        FaceVerticesRight(curMesh.mesh.vertices, 0, 0, 0);
    }
    else if (dir.x == -1 && dir.y == 0 && dir.z == 0) {
        //FaceIndicesLeft(curMesh.indices, 0);
        FaceVerticesLeft(curMesh.mesh.vertices, 0, 0, 0);
    }
    TexCoords(curMesh.mesh.texcoords);

    curMesh.mesh.triangleCount = 2;
    curMesh.mesh.vertexCount = 4;

    UploadMesh(&curMesh.mesh, false);
}