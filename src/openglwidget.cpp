#include "openglwidget.h"
#include <qdebug.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QKeyEvent>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#ifdef _DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/color.h>
#endif // _DEBUG
constexpr int FPS = 60;
constexpr glm::vec3 POSITIONDEFAULT(0.0f, 0.0f, 100.0f);
OpenGLWidget::OpenGLWidget(QWidget* parent) :
    QOpenGLWidget(parent), timer(nullptr), texture(nullptr), texture2(nullptr)
{
    model = glm::mat4(1.0f);
    /*
        注意：camera默认朝向为(0.0f,0.0f,-1.0f)，如果面片z轴坐标大于10会不在视野内。
        并且z值越小，距离越远，面片在屏幕上也越小
    */
    camera = Camera(POSITIONDEFAULT); //初始化相机位置

    setFocusPolicy(Qt::StrongFocus); //设置焦点策略,否则键盘事件不响应

    timer = std::make_unique<QTimer>(this);//timer只负责触发onTimeout()函数，不负责获取时间
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(1000 / FPS);

    timerKey = std::make_unique<QTimer>(this);//timer只负责触发onTimeout()函数，不负责获取时间
    connect(timerKey.get(), SIGNAL(timeout()), this, SLOT(onTimeoutKey()));

}

OpenGLWidget::~OpenGLWidget()
{
    if (timer != nullptr)
    {
        timer->stop();
        timer.release();
#ifdef _DEBUG
        fmt::print(fmt::fg(fmt::color::red), "Release timer successfully.\n");
#endif // _DEBUG
    }
    if (texture != nullptr)
    {
        texture->destroy();
        delete texture;
        texture = nullptr;
#ifdef _DEBUG
        fmt::print(fmt::fg(fmt::color::red), "Release texture successfully.\n");
#endif // _DEBUG
    }
    if (texture2 != nullptr)
    {
        texture2->destroy();
        delete texture2;
        texture2 = nullptr;
#ifdef _DEBUG
        fmt::print(fmt::fg(fmt::color::red), "Release texture2 successfully.\n");
#endif // _DEBUG
    }

    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    VAOs.clear();
    glDeleteBuffers(VBOs.size(), VBOs.data());
    VBOs.clear();
    glDeleteBuffers(EBOs.size(), EBOs.data());
    EBOs.clear();
#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::red), "Release VAOs, VBOs, EBOs successfully.\n");
#endif // _DEBUG
    shaderProgram.release();
#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::red), "Release shader successfully.\n");
#endif // _DEBUG

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // 设置渲染方式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    loadShaderProgram();
    loadTexture();

    setNewRect(0.0f, 0.0f, 0.0f); //默认创建一个矩形
}

void OpenGLWidget::resizeGL(int w, int h)
{
#ifdef _DEBUG
    fmt::print("INFO: resize to {}x{}\n", w, h);
#endif //_DEBUG
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
    glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glUniformMatrix4fv(shaderProgram.uniformLocation("model"), 1, GL_FALSE, &model[0][0]);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)this->width() / (float)this->height(), 0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(shaderProgram.uniformLocation("projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(shaderProgram.uniformLocation("view"), 1, GL_FALSE, &view[0][0]);
    texture->bind(0);
    texture->generateMipMaps();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture2->bind(1);
    texture2->generateMipMaps();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    for (size_t i = 0; i < VAOs.size(); i++)
    {
        glBindVertexArray(VAOs[i]);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_POINTS, indices.size() , GL_UNSIGNED_INT, 0);

    }
}

void OpenGLWidget::loadTexture()
{
    texture = new QOpenGLTexture(
        QImage("../resources/texture.png").mirrored());
    //输出texture的尺寸信息
#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::green), "load texture successfully.");
    fmt::print(" The shape is {}x{}px.\n", texture->width(), texture->height());
#endif // _DEBUG
    texture2 = new QOpenGLTexture(
        QImage("../resources/texture2.png").mirrored());
    //输出texture的尺寸信息
#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::green), "load texture2 successfully.");
    fmt::print(" The shape is {}x{}px.\n", texture2->width(), texture2->height());
#endif // _DEBUG
}

void OpenGLWidget::onTimeoutKey()
{
#ifdef _DEBUG
    fmt::print("Keys: {}\n", pressedKeys);
#endif // _DEBUG
    if (pressedKeys.empty())
    {
        timerKey->stop();
        return;
    }
    for (auto& key : pressedKeys)
    {
        switch (key)
        {
        case Qt::Key_W:
            camera.ProcessKeyboard(FORWARD, deltaTime);
            break;
        case Qt::Key_S:
            camera.ProcessKeyboard(BACKWARD, deltaTime);
            break;
        case Qt::Key_A:
            camera.ProcessKeyboard(LEFT, deltaTime);
            break;
        case Qt::Key_D:
            camera.ProcessKeyboard(RIGHT, deltaTime);
            break;
        case Qt::Key_Q:
            camera.ProcessKeyboard(UP, deltaTime);
            break;
        case Qt::Key_E:
            camera.ProcessKeyboard(DOWN, deltaTime);
            break;
        case Qt::Key_G:
            camera.Position = POSITIONDEFAULT;
            break;
        case Qt::Key_Escape:
            QCoreApplication::quit();
            break;
        default:
            break;
        }
    }
}


void OpenGLWidget::loadShaderProgram()
{
    int success;
    char infoLog[512];
    std::string vertexShaderPath = "../shader/rectangle.vert";
    std::string fragmentShaderPath = "../shader/rectangle.frag";

    success = shaderProgram.addShaderFromSourceFile(
        QOpenGLShader::Vertex, vertexShaderPath.c_str());
    //success = shaderProgram.addCacheableShaderFromSourceCode(
    //    QOpenGLShader::Vertex, vertexShaderSource);
    if (!success) {
        qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << shaderProgram.log();
    }
    success = shaderProgram.addShaderFromSourceFile(
        QOpenGLShader::Fragment, fragmentShaderPath.c_str());
    if (!success) {
        qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << shaderProgram.log();
    }
    success = shaderProgram.link();
    if (!success) {
        qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << shaderProgram.log();
    }

#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::green), "Shader Compiled Successfully!!!\n");
#endif // _DEBUG
    shaderProgram.bind();
    shaderProgram.setUniformValue("ourTexture", 0);
    shaderProgram.setUniformValue("ourTexture2", 1);
}

void OpenGLWidget::setNewRect(float dx, float dy, float dz)
{
    makeCurrent();
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices = this->vertices;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].position[0] += dx;
        vertices[i].position[1] += dy;
        vertices[i].position[2] += dz;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    VAOs.emplace_back(VAO);
    VBOs.emplace_back(VBO);
    EBOs.emplace_back(EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type),
        vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(decltype(indices)::value_type),
        indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type),
        (void*)(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type),
        (void*)(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type),
        (void*)(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    doneCurrent();
    update();
#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::green), "VAO, VBO, EBO Created Successfully!!!\n");
    fmt::print("{}, {}, {}\n", VAOs, VBOs, EBOs);

#endif // _DEBUG

}

void OpenGLWidget::cleanAllRects()
{
#ifdef _DEBUG
    fmt::print(fmt::fg(fmt::color::yellow), "VAOs, VBOs and EBOs Cleaned Successfully!!!\n");

#endif // _DEBUG

    makeCurrent();
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    VAOs.clear();
    glDeleteBuffers(VBOs.size(), VBOs.data());
    VBOs.clear();
    glDeleteBuffers(EBOs.size(), EBOs.data());
    EBOs.clear();
    doneCurrent();
    update();
}

void OpenGLWidget::onTimeout() {
    ////获取当前毫秒数
    int currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    static int lastTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    deltaTime = (currentTime * 1.0f - lastTime);
    lastTime = currentTime;
#ifdef _DEBUG
    fmt::print("Tick Time: {}ms\n", deltaTime);
#endif // _DEBUG
    update();
}

/*
* 重写键盘事件
*/
void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    QOpenGLWidget::keyPressEvent(event);

    //按键按下，key值放入容器，如果是长按触发的repeat就不判断
    if (!event->isAutoRepeat())
    {
        pressedKeys.insert(event->key());
    }
    //判断是否运行，不然一直触发就一直不能timeout
    if (!timerKey->isActive())
    {
        timerKey->start(5);
        return;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
    QOpenGLWidget::keyReleaseEvent(event);
    if (!event->isAutoRepeat())
    {
        pressedKeys.erase(event->key());
    }
    if (pressedKeys.empty())
    {
        timerKey->stop();
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    QOpenGLWidget::mouseMoveEvent(event);
    //获得鼠标变化坐标
    static bool firstMouse = true;
    static float lastX, lastY;
    if (firstMouse)
    {
        lastX = event->x();
        lastY = event->y();
        firstMouse = false;
    }
#ifdef _DEBUG
    fmt::print(
        "last pos: ({},{}), current pos: ({},{})\n",
        lastX, lastY, event->x(), event->y());
#endif // _DEBUG

    float xoffset = event->x() - lastX;
    float yoffset = lastY - event->y(); // reversed since y-coordinates go from bottom to top

    lastX = event->x();
    lastY = event->y();

    camera.ProcessMouseMovement(xoffset, yoffset);

}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    QOpenGLWidget::wheelEvent(event);
    camera.ProcessMouseScroll(event->delta() / 120);
}
