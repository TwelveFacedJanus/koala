#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../types.h"
#include "../async.h"
#include "../../external/ant_logger/src/ant_logger.h"
#include "components.h"

float camera_angle_h = 0.0f; // горизонтальный угол
float camera_angle_v = 0.0f; // вертикальный угол
float camera_zoom = 2.5f;    // расстояние до куба (zoom)
float cube_angle = 0.0f;     // угол вращения куба
int mouse_dragging = 0;
double last_mouse_x = 0.0, last_mouse_y = 0.0;
float camera_velocity_h = 0.0f, camera_velocity_v = 0.0f;

// Позиция источника света (глобально, чтобы использовать и для сферы)
GLfloat light_pos[] = { 1.0f, 1.0f, 2.0f, 1.0f };

typedef struct KoalaUIWindow {
#ifdef _glfw3_h_
    GLFWwindow* window;
#endif
} KoalaUIWindow;

void terminate_koala_ui(KoalaUIWindow* koalaui) {
#ifdef _glfw3_h_
    if (koalaui->window != NULL) {
        glfwDestroyWindow(koalaui->window);
        ANT_LOG_DEBUG("Window destroyed");
    }
    glfwTerminate();
    ANT_LOG_DEBUG("GLFW terminated");
#endif
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

void setup_lighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_dif[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_dif);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT)  camera_angle_h -= 5.0f;
        if (key == GLFW_KEY_RIGHT) camera_angle_h += 5.0f;
        if (key == GLFW_KEY_UP)    camera_angle_v -= 5.0f;
        if (key == GLFW_KEY_DOWN)  camera_angle_v += 5.0f;
        if (key == GLFW_KEY_W)     camera_zoom -= 0.1f;
        if (key == GLFW_KEY_S)     camera_zoom += 0.1f;
        if (camera_zoom < 0.5f)   camera_zoom = 0.5f;
        if (camera_zoom > 10.0f)  camera_zoom = 10.0f;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_dragging = 1;
            glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
        } else if (action == GLFW_RELEASE) {
            mouse_dragging = 0;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse_dragging) {
        double dx = xpos - last_mouse_x;
        double dy = ypos - last_mouse_y;
        camera_velocity_h = (float)dx * 0.2f;
        camera_velocity_v = (float)dy * 0.2f;
        last_mouse_x = xpos;
        last_mouse_y = ypos;
    }
}

Result koala_create_window() {
    Result res;
    if (!glfwInit()) {
        res.is_ok = 0;
        GENERIC_SET(res.err, "Failed to initialize GLFW");
        ANT_LOG_ERROR("Failed to initialize GLFW");
        res.err.type = GENERIC_TYPE_STR;
        return res;
    }
    GLFWwindow* window = glfwCreateWindow(800, 600, "KOALA UI (GLFW).", NULL, NULL);
    if (window == NULL) {
        res.is_ok = 0;
        GENERIC_SET(res.err, "Failed to create glfw window");
        ANT_LOG_ERROR("Failed to create glfw window");
        res.err.type = GENERIC_TYPE_STR;
        return res;
    }
    glfwMakeContextCurrent(window);
    glewInit();
    ANT_LOG_DEBUG("GLEW initialized");
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double fovY = 45.0;
    double aspect = (double)width / height;
    double zNear = 0.1, zFar = 100.0;
    double fH = tan(fovY / 360.0 * M_PI) * zNear;
    double fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    setup_lighting();
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    ANT_LOG_DEBUG("GLFW window created");
    res.is_ok = 1;
    GENERIC_SET(res.ok, window);
    res.ok.type = GENERIC_TYPE_PTR;
    return res;
}

// --- Минимальный OBJ loader ---
#define MAX_OBJ_VERTS 100000
#define MAX_OBJ_FACES 200000
float obj_verts[MAX_OBJ_VERTS][3];
#define MAX_MTLS 128
typedef struct {
    char name[64];
    float kd[3]; // Diffuse color
} Material;
Material mtls[MAX_MTLS];
int mtl_count = 0;

typedef struct {
    int v[3];
    int mtl_idx;
} Face;
Face obj_faces[MAX_OBJ_FACES];
int obj_face_count = 0;

int find_mtl(const char* name) {
    for (int i = 0; i < mtl_count; ++i)
        if (strcmp(mtls[i].name, name) == 0) return i;
    return -1;
}

void load_mtl(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return;
    char line[256], current_name[64] = "";
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "newmtl ", 7) == 0) {
            sscanf(line + 7, "%63s", current_name);
            strncpy(mtls[mtl_count].name, current_name, 63);
            mtls[mtl_count].kd[0] = mtls[mtl_count].kd[1] = mtls[mtl_count].kd[2] = 0.8f;
            mtl_count++;
        } else if (strncmp(line, "Kd ", 3) == 0 && mtl_count > 0) {
            float r, g, b;
            if (sscanf(line + 3, "%f %f %f", &r, &g, &b) == 3) {
                mtls[mtl_count-1].kd[0] = r;
                mtls[mtl_count-1].kd[1] = g;
                mtls[mtl_count-1].kd[2] = b;
            }
        }
    }
    fclose(f);
}

void load_obj(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) { fprintf(stderr, "Cannot open OBJ file: %s\n", filename); return; }
    char line[256];
    char current_mtl[64] = "";
    int current_mtl_idx = -1;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "mtllib ", 7) == 0) {
            char mtlfile[128];
            sscanf(line + 7, "%127s", mtlfile);
            load_mtl(mtlfile);
        } else if (strncmp(line, "usemtl ", 7) == 0) {
            sscanf(line + 7, "%63s", current_mtl);
            current_mtl_idx = find_mtl(current_mtl);
        } else if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;
            if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3 && obj_face_count < MAX_OBJ_FACES) {
                obj_verts[obj_face_count][0] = x;
                obj_verts[obj_face_count][1] = y;
                obj_verts[obj_face_count][2] = z;
                obj_face_count++;
            }
        } else if (line[0] == 'f' && line[1] == ' ') {
            int v[3];
            // Пробуем f v1 v2 v3
            if (sscanf(line, "f %d %d %d", &v[0], &v[1], &v[2]) == 3 && obj_face_count < MAX_OBJ_FACES) {
                if (v[0] > 0 && v[1] > 0 && v[2] > 0 && v[0] <= obj_face_count && v[1] <= obj_face_count && v[2] <= obj_face_count) {
                    obj_faces[obj_face_count].v[0] = v[0] - 1;
                    obj_faces[obj_face_count].v[1] = v[1] - 1;
                    obj_faces[obj_face_count].v[2] = v[2] - 1;
                    obj_faces[obj_face_count].mtl_idx = current_mtl_idx;
                    obj_face_count++;
                }
            } else {
                // Пробуем f v1/... v2/... v3/...
                int v0, v1, v2;
                if (sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v0, &v1, &v2) == 3 && obj_face_count < MAX_OBJ_FACES) {
                    if (v0 > 0 && v1 > 0 && v2 > 0 && v0 <= obj_face_count && v1 <= obj_face_count && v2 <= obj_face_count) {
                        obj_faces[obj_face_count].v[0] = v0 - 1;
                        obj_faces[obj_face_count].v[1] = v1 - 1;
                        obj_faces[obj_face_count].v[2] = v2 - 1;
                        obj_faces[obj_face_count].mtl_idx = current_mtl_idx;
                        obj_face_count++;
                    }
                }
            }
        }
    }
    fclose(f);
    printf("Loaded OBJ: %d verts, %d faces\n", obj_face_count, obj_face_count);
}

void draw_obj_model() {
    glDisable(GL_LIGHTING); // временно отключаем освещение для проверки цвета
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < obj_face_count; ++i) {
        int mtl_idx = obj_faces[i].mtl_idx;
        if (mtl_idx >= 0 && mtl_idx < mtl_count)
            glColor3fv(mtls[mtl_idx].kd);
        else
            glColor3f(0.8f, 0.8f, 0.8f);
        for (int j = 0; j < 3; ++j) {
            int idx = obj_faces[i].v[j];
            if (idx < 0 || idx >= obj_face_count) continue;
            glVertex3fv(obj_verts[idx]);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING); // включаем обратно
}

void draw_grid_axes() {
    int grid_half = 20;
    float step = 1.0f;
    float half = grid_half * step;
    // Сетка (полупрозрачная)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);
    glColor4f(0.6f, 0.6f, 0.6f, 0.4f);
    glBegin(GL_LINES);
    for (int i = -grid_half; i <= grid_half; ++i) {
        if (i == 0) continue; // центральные оси отдельно
        // Линии параллельно X (по Z)
        glVertex3f(-half, 0.0f, i * step);
        glVertex3f( half, 0.0f, i * step);
        // Линии параллельно Z (по X)
        glVertex3f(i * step, 0.0f, -half);
        glVertex3f(i * step, 0.0f,  half);
    }
    glEnd();
    glDisable(GL_BLEND);

    // Центральные оси X и Z (жирные и яркие)
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    // X (красная)
    glColor3f(1.0f, 0.2f, 0.2f);
    glVertex3f(-half, 0.0f, 0.0f);
    glVertex3f( half, 0.0f, 0.0f);
    // Z (синяя)
    glColor3f(0.2f, 0.2f, 1.0f);
    glVertex3f(0.0f, 0.0f, -half);
    glVertex3f(0.0f, 0.0f,  half);
    glEnd();

    // Ось Y (зелёная, вертикальная)
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glColor3f(0.2f, 1.0f, 0.2f);
    glVertex3f(0.0f, -half, 0.0f);
    glVertex3f(0.0f,  half, 0.0f);
    glEnd();

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void run_koala_ui(KoalaUIWindow* koalaui) {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (!glfwWindowShouldClose(koalaui->window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        // Плавное вращение камеры
        camera_angle_h += camera_velocity_h;
        camera_angle_v += camera_velocity_v;
        camera_velocity_h *= 0.85f;
        camera_velocity_v *= 0.85f;
        glRotatef(camera_angle_v, 1.0f, 0.0f, 0.0f);
        glRotatef(camera_angle_h, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -camera_zoom);
        draw_grid_axes();
        draw_sphere(light_pos[0], light_pos[1], light_pos[2], 0.08f, 12, 12);
        glRotatef(cube_angle, 1.0f, 1.0f, 0.0f);
        draw_obj_model();
        cube_angle += 0.5f;
        glfwSwapBuffers(koalaui->window);
        glfwPollEvents();
    }
    terminate_koala_ui(koalaui);
}

KoalaUIWindow* initialize_koala_ui() {
    Result res = koala_create_window();
    KoalaUIWindow* koalaui = malloc(sizeof(KoalaUIWindow));
    if (!res.is_ok) {
        ANT_LOG_ERROR("Failed to create window");
        return NULL;
    }
    ANT_LOG_INFO("Window created");
    koalaui->window = GENERIC_GET(GLFWwindow*, res.ok);
    return koalaui;
}

int main() {
    ant_log_init(NULL);
    ant_log_set_level(ANT_LOG_DEBUG);
    ant_log_enable_colors(true);
    ant_log_enable_async(true); // асинхронный режим

    load_obj("./gorgona_model.obj");
    KoalaUIWindow* koalaui = initialize_koala_ui();
    run_koala_ui(koalaui);
}
