#pragma once

struct TreeModel {
    GLuint vao, vbo;

    TreeModel()
    {
        glm::vec4 brown{43.0 / 255.0, 20.0 / 255.0, 20.0 / 255.0, 1.0};
        glm::vec4 green{10.0 / 255.0, 127.0 / 255.0, 18.0 / 255.0, 1.0};
        glm::vec4 greener{18.0 / 255.0, 74.0 / 255.0, 9.0 / 255.0, 1.0};
        // auto vertInit = {
        float vertInit[] = {
            0.0f,      0.0f,      0.0f,  // front
            brown.x,   brown.y,   brown.z,   brown.w,   1.0f,    0.0f,      0.0f,      brown.x,   brown.y,
            brown.z,   brown.w,   1.0f,      0.0f,      1.0f,    brown.x,   brown.y,   brown.z,   brown.w,

            0.0f,      0.0f,      0.0f,      brown.x,   brown.y, brown.z,   brown.w,   0.0f,      0.0f,
            1.0f,      brown.x,   brown.y,   brown.z,   brown.w, 1.0f,      0.0f,      1.0f,      brown.x,
            brown.y,   brown.z,   brown.w,

            0.0f,      1.0f,      0.0f,  // back
            brown.x,   brown.y,   brown.z,   brown.w,   1.0f,    1.0f,      0.0f,      brown.x,   brown.y,
            brown.z,   brown.w,   1.0f,      1.0f,      1.0f,    brown.x,   brown.y,   brown.z,   brown.w,

            0.0f,      1.0f,      0.0f,      brown.x,   brown.y, brown.z,   brown.w,   0.0f,      1.0f,
            1.0f,      brown.x,   brown.y,   brown.z,   brown.w, 1.0f,      1.0f,      1.0f,      brown.x,
            brown.y,   brown.z,   brown.w,

            0.0f,      0.0f,      0.0f,  // left
            brown.x,   brown.y,   brown.z,   brown.w,   0.0f,    1.0f,      0.0f,      brown.x,   brown.y,
            brown.z,   brown.w,   0.0f,      1.0f,      1.0f,    brown.x,   brown.y,   brown.z,   brown.w,

            0.0f,      0.0f,      0.0f,      brown.x,   brown.y, brown.z,   brown.w,   0.0f,      0.0f,
            1.0f,      brown.x,   brown.y,   brown.z,   brown.w, 0.0f,      1.0f,      1.0f,      brown.x,
            brown.y,   brown.z,   brown.w,

            1.0f,      0.0f,      0.0f,  // right
            brown.x,   brown.y,   brown.z,   brown.w,   1.0f,    1.0f,      0.0f,      brown.x,   brown.y,
            brown.z,   brown.w,   1.0f,      1.0f,      1.0f,    brown.x,   brown.y,   brown.z,   brown.w,

            1.0f,      0.0f,      0.0f,      brown.x,   brown.y, brown.z,   brown.w,   1.0f,      0.0f,
            1.0f,      brown.x,   brown.y,   brown.z,   brown.w, 1.0f,      1.0f,      1.0f,      brown.x,
            brown.y,   brown.z,   brown.w,

            -0.5f,     0.5f,      1.0f,  // vertical 1
            green.x,   green.y,   green.z,   green.w,   1.5f,    0.5f,      1.0f,      green.x,   green.y,
            green.z,   green.w,   0.5f,      0.5f,      2.0f,    green.x,   green.y,   green.z,   green.w,

            0.5f,      -0.5f,     1.0f,  // vertical 2
            greener.x, greener.y, greener.z, greener.w, 0.5f,    1.5f,      1.0f,      greener.x, greener.y,
            greener.z, greener.w, 0.5f,      0.5f,      2.0f,    greener.x, greener.y, greener.z, greener.w,
        };

        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, 5 * 2 * 3 * 7 * sizeof(float), vertInit, GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, 12 * 3 * 7 * sizeof(float), vertInit, GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, 21 * 10 * sizeof(float), vertInit, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

/*

    Shader treeShader{"src/graphics/shaders/tree/vert.v", "src/graphics/shaders/tree/frag.f", "", ""};

    // glm::vec3 treePos = glm::vec3(mapWidth * 1.0 * 2.0, mapHeight * 1.4f * 2.0f, 0.0f);
    TreeModel treeModel;
    glBindVertexArray(treeModel.vao);

    // glm::vec3 treePos = glm::vec3(-10.0f, 0.0f, 0.0f);
    float treePos[] = {-10.0f, 0.0f, 10.0f, -5.0f, 0.0f, 10.0f, -2.0f, 0.0f, 10.0f};
    std::vector<glm::mat4> tMat;
    auto terrainPix = terrainTexture.GetPixels();
    auto heightPix = heightMap.GetPixels();
    for (int y = 0; y < terrainMapHeight; y += 2) {
        for (int x = 0; x < terrainMapWidth; ++x) {
            int index = y * terrainMapWidth * 4 + x * 4;
            int hIndex = y * terrainMapWidth * 3 + x * 3;
            // if (terrainPix[index] == 16 && terrainPix[index + 1] == 60 && terrainPix[index + 2] == 9) {
            // if ((terrainPix[index] >= 14 && terrainPix[index + 1] >= 58 && terrainPix[index + 2] >= 6) &&
            //    (terrainPix[index] <= 18 && terrainPix[index + 1] <= 62 && terrainPix[index + 2] <= 11)) {
            if (terrainPix[index] == 41 && terrainPix[index + 1] == 155 && terrainPix[index + 2] == 22 &&
                sin(x * y) > 0.7) {
                auto treeMl = glm::mat4(1.0);
                treeMl = glm::translate(treeMl,
                                        glm::vec3{(double)x * scale, (double)y * scale, 1.0 * heightPix[hIndex]});
                // treeMl = glm::translate(treeMl, glm::vec3{ 0.0 + x * ((double)mapWidth / terrainMapWidth) *
                // scale, 0.0 + y * ((double)mapHeight / terrainMapHeight) * scale, 130.0});
                treeMl = glm::scale(treeMl, glm::vec3{5.0, 5.0, 10.0});
                tMat.push_back(treeMl);
                // Log((int)heightPix[index]);
            }
            else if (terrainPix[index] == 18 && terrainPix[index + 1] == 74 && terrainPix[index + 2] == 9 &&
                     sin(x * y) > 0.5) {
                auto treeMl = glm::mat4(1.0);
                treeMl = glm::translate(treeMl,
                                        glm::vec3{(double)x * scale, (double)y * scale, 1.0 * heightPix[hIndex]});
                treeMl = glm::scale(treeMl, glm::vec3{5.0, 5.0, 10.0});
                tMat.push_back(treeMl);
            }
        }
    }
    int amount = tMat.size();
    Log("trees amount = " << amount);
    GLuint treePosBuffer;
    glGenBuffers(1, &treePosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, treePosBuffer);
    // glBufferData(GL_ARRAY_BUFFER, amount * 3 * sizeof(float), treePos, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, amount * 1 * sizeof(glm::mat4), tMat.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glBindVertexArray(0);





        // tree
        glUseProgram(treeShader.GetProgram());

        // glUniform1iv(glGetUniformLocation(waterShader.GetProgram(), "tex"), 32, texID);

        glUniformMatrix4fv(glGetUniformLocation(treeShader.GetProgram(), "matrix"), 1, GL_FALSE,
                           glm::value_ptr(matrix));
        //        glUniformMatrix4fv(glGetUniformLocation(treeShader.GetProgram(), "ml"), 1, GL_FALSE,
        //                           glm::value_ptr(treeMl));
        glBindVertexArray(treeModel.vao);
        // glBindBuffer(GL_ARRAY_BUFFER, treeModel.vbo);
        // glBindBuffer(GL_ARRAY_BUFFER, treePosBuffer);
        // glDrawElementsInstanced(GL_TRIANGLES, 0, GL_UNSIGNED_INT, 0, amount);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3 * 2 * 5, amount);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 3 * 2 * 12, amount);

        // ~treee




*/
