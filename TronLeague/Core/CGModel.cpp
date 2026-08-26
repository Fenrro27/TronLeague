#include "CGModel.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "CGCamera.h"
#include "CGScene.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <algorithm>

//
// FUNCIÓN: CGModel::initialize(int w, int h)
//
// PROPÓSITO: Inicializa el modelo 3D, shaders, cámara y escena
//
void CGModel::initialize(int w, int h)
{
    // Crea el programa gráfico para la escena
    sceneProgram = new CGShaderProgram("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
    if (!sceneProgram->IsLinked()) return;

    // Crea el programa gráfico para el skybox
    skyboxProgram = new CGShaderProgram("shaders/SkyboxVertexShader.glsl", "shaders/SkyboxFragmentShader.glsl");
    if (!skyboxProgram->IsLinked()) return;

    sceneProgram->Use();

    // Crea el skybox
    skybox = new CGSkybox();

    // Crea la moto Tron
    motoTron = new TronLightCycle();

    // Crea la pelota
    figPelota = new CGSphere(24, 48, ballRadius);

    // Crea la escena con la moto y la pelota
    scene = new CGScene(motoTron, figPelota);

    // Crea la cámara en 3ª persona con perspectiva equilibrada
    camera = new CGCamera(motoTron, figPelota, 22.0f, 6.5f);

    // Inicializa posiciones de los objetos
    resetPositions();

    // Crea el Framebuffer del mapa de sombras
    bool frameBufferStatus = InitShadowMap();
    if (!frameBufferStatus)
    {
        std::cerr << "Advertencia: No se pudo inicializar el Shadow Map Framebuffer" << std::endl;
    }

    // Configura el viewport y la matriz de proyección en perspectiva real
    resize(w, h);

    // Opciones globales de renderizado OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//
// FUNCIÓN: CGModel::finalize()
//
// PROPÓSITO: Libera los recursos dinámicos del modelo 3D
//
void CGModel::finalize()
{
    delete camera;
    camera = nullptr;

    delete scene;
    scene = nullptr;

    delete skybox;
    skybox = nullptr;

    delete sceneProgram;
    sceneProgram = nullptr;

    delete skyboxProgram;
    skyboxProgram = nullptr;

    if (shadowFBO)
    {
        glDeleteFramebuffers(1, &shadowFBO);
        shadowFBO = 0;
    }
    if (depthTexId)
    {
        glDeleteTextures(1, &depthTexId);
        depthTexId = 0;
    }
}

//
// FUNCIÓN: CGModel::resize(int w, int h)
//
// PROPÓSITO: Asigna el viewport y la proyección en perspectiva equilibrada (FOV 60°)
//
void CGModel::resize(int w, int h)
{
    if (h == 0) h = 1;
    wndWidth = w;
    wndHeight = h;

    glViewport(0, 0, w, h);
    float aspectRatio = (float)w / (float)h;
    projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.5f, 2000.0f);
}

//
// FUNCIÓN: CGModel::render()
//
// PROPÓSITO: Genera la imagen completa 3D (ShadowMap + Skybox + Escena)
//
void CGModel::render()
{
    // ========================================================
    // 1. GENERACIÓN DEL SHADOW MAP
    // ========================================================
    sceneProgram->Use();

    glm::mat4 lightViewMatrix = scene->GetLightViewMatrix();
    glm::mat4 lightPerspective = glm::ortho(-450.0f, 450.0f, -450.0f, 450.0f, -500.0f, 500.0f);
    glm::mat4 lightMVP = lightPerspective * lightViewMatrix;

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sceneProgram->SetUniformSubroutine(GL_FRAGMENT_SHADER, "recordDepth");
    glCullFace(GL_FRONT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1f, 4.0f);

    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    scene->Draw(sceneProgram, lightPerspective, lightViewMatrix, lightMVP);

    glDisable(GL_POLYGON_OFFSET_FILL);

    // ========================================================
    // 2. RENDERIZADO DEL SKYBOX
    // ========================================================
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glViewport(0, 0, wndWidth, wndHeight);

    glm::mat4 view = camera->ViewMatrix();
    skyboxProgram->Use();
    skybox->Draw(skyboxProgram, projection, view);

    // ========================================================
    // 3. RENDERIZADO DE LA ESCENA CON ILUMINACIÓN Y SOMBRAS
    // ========================================================
    sceneProgram->Use();
    sceneProgram->SetUniformSubroutine(GL_FRAGMENT_SHADER, "shadeWithShadow");
    sceneProgram->SetUniformI("ShadowMap", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexId);

    glm::mat4 viewMatrix = camera->ViewMatrix();
    scene->Draw(sceneProgram, projection, viewMatrix, lightMVP);
}

//
// FUNCIÓN: CGModel::renderUI()
//
// PROPÓSITO: Renderiza la interfaz gráfica HUD con Dear ImGui
//
void CGModel::renderUI()
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoDecoration | 
                                ImGuiWindowFlags_AlwaysAutoResize | 
                                ImGuiWindowFlags_NoSavedSettings | 
                                ImGuiWindowFlags_NoFocusOnAppearing | 
                                ImGuiWindowFlags_NoNav |
                                ImGuiWindowFlags_NoMove;

    // -------------------------------------------------------------
    // 1. MARCADOR SUPERIOR (Arriba al Centro)
    // -------------------------------------------------------------
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 20.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.80f);

    if (ImGui::Begin("Scoreboard", nullptr, hudFlags))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.95f, 1.0f, 1.0f));
        ImGui::SetWindowFontScale(1.6f);
        ImGui::Text(" ⚽ GOLES: %d ", score);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
    }
    ImGui::End();

    // -------------------------------------------------------------
    // 2. BANNER DE CELEBRACIÓN DE GOL
    // -------------------------------------------------------------
    if (goalCelebrationTimer > 0.0f)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.35f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.88f);

        if (ImGui::Begin("GoalCelebration", nullptr, hudFlags))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.1f, 1.0f));
            ImGui::SetWindowFontScale(2.4f);
            ImGui::Text("  ¡¡¡GOOOOOOOL!!!  ");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.6f, 1.0f), "      ¡Punto anotado! Saque de centro      ");
        }
        ImGui::End();
    }

    // -------------------------------------------------------------
    // 3. MONITOR DE RENDIMIENTO / FPS (Esquina Superior Derecha)
    // -------------------------------------------------------------
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 20.0f, 20.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.75f);

    if (ImGui::Begin("FPSOverlay", nullptr, hudFlags))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.6f, 1.0f), "⚡ %.1f FPS", io.Framerate);
        ImGui::TextColored(ImVec4(0.7f, 0.85f, 1.0f, 0.8f), "(%.2f ms)", 1000.0f / (io.Framerate > 0.0f ? io.Framerate : 60.0f));
    }
    ImGui::End();

    // -------------------------------------------------------------
    // 4. ACELERADOR VERTICAL (Esquina Inferior Izquierda)
    // -------------------------------------------------------------
    ImGui::SetNextWindowPos(ImVec2(20.0f, io.DisplaySize.y - 20.0f), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.80f);

    if (ImGui::Begin("SpeedGauge", nullptr, hudFlags))
    {
        ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f), "⚡ ACELERADOR");
        ImGui::Spacing();

        float speedPct = std::clamp(std::abs(motoVelocity) / maxForwardSpeed, 0.0f, 1.0f);

        const float barWidth = 24.0f;
        const float barHeight = 140.0f;

        ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImVec2 p1 = ImVec2(p0.x + barWidth, p0.y + barHeight);

        // Fondo de la barra
        drawList->AddRectFilled(p0, p1, IM_COL32(10, 20, 30, 200), 4.0f);
        drawList->AddRect(p0, p1, IM_COL32(0, 200, 255, 255), 4.0f);

        // Llenado de abajo hacia arriba
        if (speedPct > 0.01f)
        {
            float fillY = p1.y - (barHeight * speedPct);
            ImVec2 fillP0 = ImVec2(p0.x + 2.0f, fillY);
            ImVec2 fillP1 = ImVec2(p1.x - 2.0f, p1.y - 2.0f);

            ImU32 barColor = (motoVelocity >= 0.0f)
                ? IM_COL32((int)(speedPct * 255), (int)(220 - speedPct * 50), 255, 240)
                : IM_COL32(255, 100, 100, 240);

            drawList->AddRectFilled(fillP0, fillP1, barColor, 2.0f);
        }

        // Marcas de nivel
        for (int i = 1; i <= 3; ++i)
        {
            float tickY = p1.y - (barHeight * 0.25f * i);
            drawList->AddLine(ImVec2(p1.x + 3.0f, tickY), ImVec2(p1.x + 8.0f, tickY), IM_COL32(0, 200, 255, 150));
        }

        ImGui::Dummy(ImVec2(barWidth + 45.0f, barHeight));

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "MAX");
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "50%%");
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "0");
        ImGui::EndGroup();

        ImGui::Spacing();
        char speedText[32];
        std::snprintf(speedText, sizeof(speedText), "%.1f km/h (%.0f%%)", std::abs(motoVelocity) * 2.2f, speedPct * 100.0f);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", speedText);
    }
    ImGui::End();

    // -------------------------------------------------------------
    // 5. PANEL DE CONTROLES (Esquina Inferior Derecha)
    // -------------------------------------------------------------
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 20.0f, io.DisplaySize.y - 20.0f), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.80f);

    if (ImGui::Begin("🎮 Controles", nullptr, hudFlags))
    {
        ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f), "Conducción:");
        ImGui::BulletText("S : Acelerar");
        ImGui::BulletText("W : Frenar / Reversa");
        ImGui::BulletText("A / D : Girar moto");
        
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f), "Cámara:");
        ImGui::BulletText("Ratón : Orbitar");
        ImGui::BulletText("C : Moto <-> Balón");
        
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 0.9f, 1.0f, 1.0f), "Sistema:");
        ImGui::BulletText("F12 : Pantalla Completa");
        ImGui::BulletText("ESC : Salir");
    }
    ImGui::End();
}

//
// FUNCIÓN: CGModel::resetPositions()
//
// PROPÓSITO: Resetea la posición y cinemática de la moto y del balón (mirando al campo rival)
//
void CGModel::resetPositions()
{
    // Resetea estado físico de la moto en su campo mirando hacia +Z (hacia el balón)
    motoPos = glm::vec3(0.0f, 3.2f, -80.0f);
    motoHeading = 0.0f;
    motoRoll = 0.0f;
    motoVelocity = 0.0f;

    glm::mat4 motoMatrix = glm::translate(glm::mat4(1.0f), motoPos);
    motoMatrix = glm::rotate(motoMatrix, glm::radians(motoHeading + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    motoTron->SetLocation(motoMatrix);

    // Resetea estado físico del balón en el centro del campo
    ballPos = glm::vec3(0.0f, 10.0f, 40.0f);
    ballVelocity = glm::vec3(0.0f);
    ballRotation = glm::mat4(1.0f);

    glm::mat4 ballMatrix = glm::translate(glm::mat4(1.0f), ballPos) * ballRotation;
    figPelota->setLocation(ballMatrix);

    if (camera)
    {
        camera->UpdatePosition(0.016f, 0.0f, motoHeading);
    }
}

//
// FUNCIÓN: CGModel::update(float dt, GLFWwindow* window)
//
// PROPÓSITO: Motor físico continuo
//
void CGModel::update(float dt, GLFWwindow* window)
{
    if (dt <= 0.0f) dt = 0.016f;
    if (dt > 0.05f) dt = 0.05f;

    // Temporizador de celebración
    if (goalCelebrationTimer > 0.0f)
    {
        goalCelebrationTimer -= dt;
        if (goalCelebrationTimer < 0.0f) goalCelebrationTimer = 0.0f;
    }

    // ========================================================
    // 1. DINÁMICA VEHICULAR DE LA MOTO TRON
    // ========================================================
    bool throttle = false;
    bool brake = false;
    bool turnLeft = false;
    bool turnRight = false;

    if (window != nullptr)
    {
        throttle = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
        brake = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
        turnLeft = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
        turnRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    }

    // Aceleración y frenada longitudinal
    float engineAccel = 0.0f;
    if (throttle) engineAccel += 58.0f;
    if (brake) engineAccel -= 50.0f;

    // Resistencia aerodinámica y rodadura
    float aeroDrag = 0.0085f * motoVelocity * std::abs(motoVelocity);
    float rollingFriction = 7.0f;

    if (std::abs(engineAccel) > 0.01f)
    {
        motoVelocity += (engineAccel - aeroDrag) * dt;
    }
    else
    {
        if (motoVelocity > 0.0f)
        {
            motoVelocity -= (rollingFriction + aeroDrag) * dt;
            if (motoVelocity < 0.0f) motoVelocity = 0.0f;
        }
        else if (motoVelocity < 0.0f)
        {
            motoVelocity += (rollingFriction - aeroDrag) * dt;
            if (motoVelocity > 0.0f) motoVelocity = 0.0f;
        }
    }

    // Límites de velocidad
    if (motoVelocity > maxForwardSpeed) motoVelocity = maxForwardSpeed;
    if (motoVelocity < maxReverseSpeed) motoVelocity = maxReverseSpeed;

    // Dirección y ángulo de giro progresivo según velocidad
    float steerInput = 0.0f;
    if (turnLeft) steerInput += 1.0f;
    if (turnRight) steerInput -= 1.0f;

    float speedRatio = std::abs(motoVelocity) / maxForwardSpeed;
    float steerResponsiveness = std::clamp(std::abs(motoVelocity) / 8.0f, 0.0f, 1.0f);
    if (speedRatio > 0.7f)
    {
        steerResponsiveness = std::max(0.60f, 1.0f - (speedRatio - 0.7f) * 0.7f);
    }

    float turnRate = 85.0f * steerInput * steerResponsiveness;
    if (motoVelocity < 0.0f) turnRate = -turnRate;

    motoHeading += turnRate * dt;

    // Inclinación lateral en curvas
    float targetRoll = -steerInput * speedRatio * 20.0f;
    float rollSmoothing = 1.0f - std::exp(-10.0f * dt);
    motoRoll = glm::mix(motoRoll, targetRoll, rollSmoothing);

    // Integración de posición de la moto (avance hacia +Z cuando heading = 0)
    float radHeading = glm::radians(motoHeading);
    glm::vec3 forwardDir = glm::vec3(std::sin(radHeading), 0.0f, std::cos(radHeading));
    motoPos += forwardDir * motoVelocity * dt;

    // Restricciones de límites para la moto
    ApplyConstraintsToMoto();

    // Actualización de la matriz de la moto
    glm::mat4 motoMatrix = glm::translate(glm::mat4(1.0f), motoPos);
    motoMatrix = glm::rotate(motoMatrix, glm::radians(motoHeading + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    motoMatrix = glm::rotate(motoMatrix, glm::radians(motoRoll), glm::vec3(0.0f, 0.0f, 1.0f));
    motoTron->SetLocation(motoMatrix);

    // ========================================================
    // 2. FÍSICAS Y DINÁMICAS DEL BALÓN
    // ========================================================
    ApplyConstraintsToBall(dt);

    // ========================================================
    // 3. COLISIÓN E IMPULSO MOTO-BALÓN
    // ========================================================
    HandleMotoBallCollision();

    // ========================================================
    // 4. SEGUIMIENTO SUAVE DE CÁMARA
    // ========================================================
    camera->UpdatePosition(dt, speedRatio, motoHeading);
    CameraConstraints();
}

//
// FUNCIÓN: CGModel::ApplyConstraintsToBall(float dt)
//
// PROPÓSITO: Integración de gravedad, aerodinámica, rotación 3D y rebotes
//
void CGModel::ApplyConstraintsToBall(float dt)
{
    const float gravity = -38.0f;
    ballVelocity.y += gravity * dt;

    float airDrag = 0.009f * glm::length(ballVelocity);
    ballVelocity -= ballVelocity * airDrag * dt;

    ballPos += ballVelocity * dt;

    const float groundHeight = 0.0f;
    const float restitution = 0.82f;

    // Rebote en el suelo
    if (ballPos.y < groundHeight + ballRadius)
    {
        ballPos.y = groundHeight + ballRadius;
        if (std::abs(ballVelocity.y) < 1.2f)
        {
            ballVelocity.y = 0.0f;
        }
        else
        {
            ballVelocity.y = -ballVelocity.y * restitution;
        }

        ballVelocity.x *= (1.0f - 0.70f * dt);
        ballVelocity.z *= (1.0f - 0.70f * dt);
    }

    // Límites de los muros del estadio
    const float minX = -110.0f + ballRadius;
    const float maxX = 110.0f - ballRadius;
    const float minZ = -350.0f + ballRadius;
    float maxZ = 350.0f - ballRadius;

    // Apertura de la portería
    bool insideGoalMouth = (ballPos.x > -23.0f && ballPos.x < 23.0f && ballPos.y < 16.0f);
    if (insideGoalMouth)
    {
        maxZ = 368.0f - ballRadius;
    }

    if (ballPos.x > maxX) { ballPos.x = maxX; ballVelocity.x = -std::abs(ballVelocity.x) * restitution; }
    if (ballPos.x < minX) { ballPos.x = minX; ballVelocity.x = std::abs(ballVelocity.x) * restitution; }
    if (ballPos.z < minZ) { ballPos.z = minZ; ballVelocity.z = std::abs(ballVelocity.z) * restitution; }

    if (ballPos.z > maxZ)
    {
        if (!insideGoalMouth)
        {
            ballPos.z = maxZ;
            ballVelocity.z = -std::abs(ballVelocity.z) * restitution;
        }
    }

    // Rotación visual 3D
    glm::vec3 horizVel = glm::vec3(ballVelocity.x, 0.0f, ballVelocity.z);
    float horizSpeed = glm::length(horizVel);
    if (horizSpeed > 0.05f)
    {
        glm::vec3 rollAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), horizVel));
        float rollAngle = (horizSpeed * dt) / ballRadius;
        glm::mat4 deltaRot = glm::rotate(glm::mat4(1.0f), rollAngle, rollAxis);
        ballRotation = deltaRot * ballRotation;
    }

    glm::mat4 ballMatrix = glm::translate(glm::mat4(1.0f), ballPos) * ballRotation;
    figPelota->setLocation(ballMatrix);

    // Detección de gol
    if (ballPos.z - ballRadius > 350.0f && ballPos.y < 16.0f && ballPos.x > -23.0f && ballPos.x < 23.0f)
    {
        score++;
        goalCelebrationTimer = 2.8f;
        std::cout << "\n========================================" << std::endl;
        std::cout << "        ¡¡¡HAS ANOTADO GOL!!!          " << std::endl;
        std::cout << "        MARCADOR: " << score << " GOLES           " << std::endl;
        std::cout << "========================================\n" << std::endl;
        resetPositions();
    }
}

//
// FUNCIÓN: CGModel::HandleMotoBallCollision()
//
// PROPÓSITO: Colisión elástica entre la moto y el balón
//
void CGModel::HandleMotoBallCollision()
{
    glm::vec3 motoCenter = motoPos;
    motoCenter.y += 1.0f;
    const float motoRadius = 6.5f;

    glm::vec3 diff = ballPos - motoCenter;
    float dist = glm::length(diff);
    float minDist = ballRadius + motoRadius;

    if (dist < minDist && dist > 0.001f)
    {
        glm::vec3 normal = diff / dist;
        ballPos = motoCenter + normal * minDist;

        float radHeading = glm::radians(motoHeading);
        glm::vec3 forwardDir = glm::vec3(std::sin(radHeading), 0.0f, std::cos(radHeading));
        glm::vec3 motoVelVec = forwardDir * motoVelocity;

        glm::vec3 relVel = ballVelocity - motoVelVec;
        float velAlongNormal = glm::dot(relVel, normal);

        if (velAlongNormal < 0.0f)
        {
            float impulse = -(1.0f + 0.88f) * velAlongNormal;
            ballVelocity += normal * impulse;

            float forwardPush = std::max(motoVelocity, 0.0f) * 0.95f;
            ballVelocity += forwardDir * forwardPush + glm::vec3(0.0f, 10.0f, 0.0f);
        }
    }
}

//
// FUNCIÓN: CGModel::ApplyConstraintsToMoto()
//
// PROPÓSITO: Restricciones de límites de la pista para la moto
//
void CGModel::ApplyConstraintsToMoto()
{
    const float minX = -104.0f;
    const float maxX = 104.0f;
    const float minZ = -344.0f;
    const float maxZ = 344.0f;

    if (motoPos.x > maxX) { motoPos.x = maxX; motoVelocity *= 0.5f; }
    if (motoPos.x < minX) { motoPos.x = minX; motoVelocity *= 0.5f; }
    if (motoPos.z > maxZ) { motoPos.z = maxZ; motoVelocity *= 0.5f; }
    if (motoPos.z < minZ) { motoPos.z = minZ; motoVelocity *= 0.5f; }
}

//
// FUNCIÓN: CGModel::key_pressed(int key)
//
// PROPÓSITO: Teclas de sistema y cambio de cámara
//
void CGModel::key_pressed(int key)
{
    switch (key)
    {
    case GLFW_KEY_C:
        camera->changeTarget();
        break;
    case GLFW_KEY_ESCAPE:
        closeApplication(false);
        break;
    }
}

void CGModel::closeApplication(bool gol)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window != nullptr) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void CGModel::mouse_button(int button, int action)
{
}

//
// FUNCIÓN: CGModel::mouse_move(double xpos, double ypos)
//
// PROPÓSITO: Control de orientación de cámara con el ratón
//
void CGModel::mouse_move(double xpos, double ypos)
{
    static bool firstMouse = true;
    static double lastX = xpos;
    static double lastY = ypos;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.08f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->TurnRight(xoffset);
}

//
// FUNCIÓN: CGModel::CameraConstraints()
//
// PROPÓSITO: Limita la cámara al espacio de juego
//
void CGModel::CameraConstraints()
{
    glm::vec3 pos = camera->GetPosition();
    int constraint = 0;
    float groundHeight = 0.0f;

    if (pos.y < groundHeight + 1.5f) { pos.y = groundHeight + 1.5f; constraint = 1; }
    if (pos.y > groundHeight + 50.0f) { pos.y = groundHeight + 50.0f; constraint = 1; }

    float minX = -120.0f;
    float maxX = 120.0f;
    float minZ = -365.0f;
    float maxZ = 365.0f;

    if (pos.x > maxX) { pos.x = maxX; constraint = 1; }
    if (pos.x < minX) { pos.x = minX; constraint = 1; }
    if (pos.z > maxZ) { pos.z = maxZ; constraint = 1; }
    if (pos.z < minZ) { pos.z = minZ; constraint = 1; }

    if (constraint == 1)
    {
        camera->SetPosition(pos.x, pos.y, pos.z);
    }
}

//
// FUNCIÓN: CGModel::InitShadowMap()
//
// PROPÓSITO: Inicializa el FBO y textura de profundidad para mapa de sombras
//
bool CGModel::InitShadowMap()
{
    GLfloat border[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    glGenTextures(1, &depthTexId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE,
        SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexId, 0);

    glDrawBuffer(GL_NONE);

    bool result = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return result;
}
