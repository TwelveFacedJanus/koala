#include <GL/glew.h>
#include <math.h>
#include "components.h"

void draw_cube() {
    glBegin(GL_QUADS);
    // Front face (z = 1.0f)
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    // Back face (z = -1.0f)
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    // Top face (y = 1.0f)
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    // Bottom face (y = -1.0f)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    // Right face (x = 1.0f)
    glColor3f(1.0f, 0.0f, 1.0f); // Magenta
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    // Left face (x = -1.0f)
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glEnd();
}

void draw_sphere(float cx, float cy, float cz, float r, int lats, int longs) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.2f); // желтый источник света
    for (int i = 0; i <= lats; i++) {
        float lat0 = M_PI * (-0.5 + (float)(i - 1) / lats);
        float z0  = sin(lat0);
        float zr0 =  cos(lat0);

        float lat1 = M_PI * (-0.5 + (float)i / lats);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++) {
            float lng = 2 * M_PI * (float)(j - 1) / longs;
            float x = cos(lng);
            float y = sin(lng);

            glVertex3f(cx + r * x * zr0, cy + r * y * zr0, cz + r * z0);
            glVertex3f(cx + r * x * zr1, cy + r * y * zr1, cz + r * z1);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
} 