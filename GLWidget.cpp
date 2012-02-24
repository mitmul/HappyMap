#include "GLWidget.h"
#include "QtOpenGL"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      viewScale(2000.0),
      xRot(0),
      yRot(0),
      zRot(0),
      xOrigin(0.0),
      yOrigin(0.0),
      zOrigin(0.0),
      glScale(1.0),
      state_Axis(true),
      state_Light(true),
      state_Mesh(false),
      state_Lines(false),
      state_Polygon(true),
      state_Points(false)
{
    orthoRange.resize(6);
    orthoRange << -viewScale , viewScale,   // left, right
            -viewScale , viewScale,                 // bottom, top
            -viewScale , viewScale;                 // znear, zfar
}

static void qNormalizeAngle(int &angle)
{
    while(angle < 0 )
        angle += 360 * 16;

    while(angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);

    if(angle != xRot)
    {
        xRot = angle;
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);

    if(angle != yRot)
    {
        yRot = angle;
        updateGL();
    }
}
void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != zRot)
    {
        zRot = angle;
        updateGL();
    }
}

void GLWidget::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
}

void GLWidget::resizeGL(int width,int height)
{
    glViewport(0, 0, width, height);
    glLoadIdentity();
}

void GLWidget::drawAxis()
{
    if(state_Axis == true)
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        //X軸
        glColor3f(viewScale, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(-viewScale, 0.0, 0.0);
        glVertex3f(viewScale, 0.0, 0.0);
        glVertex3f(viewScale * 0.9, viewScale * 0.1, 0.0);
        glVertex3f(viewScale * 0.9, -viewScale * 0.1, 0.0);
        glEnd();

        //Y軸
        glColor3f(0.0, viewScale, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, -viewScale, 0.0);
        glVertex3f(0.0, viewScale, 0.0);
        glVertex3f(-viewScale * 0.1, viewScale * 0.9, 0.0);
        glVertex3f(viewScale * 0.1, viewScale * 0.9, 0.0);
        glEnd();

        //Z軸
        glColor3f(0.0, 0.0, viewScale);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, -viewScale);
        glVertex3f(0.0, 0.0, viewScale);
        glVertex3f(0.0, -viewScale * 0.1, viewScale * 0.9);
        glVertex3f(0.0, viewScale * 0.1, viewScale * 0.9);
        glEnd();
    }
}

void GLWidget::drawLight()
{
    if(state_Light == true)
    {
        //光源の位置
        GLfloat light0_pos[] = { orthoRange[0], orthoRange[3], orthoRange[4], 1.0 };
        GLfloat light1_pos[] = { orthoRange[1], orthoRange[2], orthoRange[5], 1.0 };

        //光源の色
        GLfloat light_color[] = { 1.0, 1.0, 1.0, 1.0 };

        //0番目の光源の設定
        glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
        glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }
}

void GLWidget::drawPolygon()
{
    if(state_Mesh)
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // デフォルト緑
        glColor3f(0.0, 0.5, 0.0);
    }

    if(!state_Light)
        glColor3f(1.0, 1.0, 1.0);

    if(state_Mesh || state_Polygon)
    {
        for(int i = 0; i < (int)faces.size(); ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                p1[j] = vertices((faces.at(i)(0)) * 3 + j);
                p2[j] = vertices((faces.at(i)(1)) * 3 + j);
                p3[j] = vertices((faces.at(i)(2)) * 3 + j);
            }

            if(state_Mesh)
            {
                glBegin(GL_LINE_LOOP);
                glVertex3f(p1[0], p1[1], p1[2]);
                glVertex3f(p2[0], p2[1], p2[2]);
                glVertex3f(p3[0], p3[1], p3[2]);
                glEnd();
            }
            if(state_Polygon)
            {
                if(calcNormal() == 1)
                    glNormal3f(n[0], n[1], n[2]);

                glBegin(GL_POLYGON);
                glVertex3f(p1[0], p1[1], p1[2]);
                glVertex3f(p2[0], p2[1], p2[2]);
                glVertex3f(p3[0], p3[1], p3[2]);
                glEnd();
            }
        }
    }
}

void GLWidget::drawLines()
{
    if(state_Lines)
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        if(line_colors.size() != lines.size())
            glColor3f(1.0, 1.0, 1.0);

        for(int i = 0; i < lines.size(); ++i)
        {
            Vector3d start = lines.at(i).at(0);
            Vector3d end = lines.at(i).at(1);

            if(line_colors.size() == lines.size())
            {
                double r = line_colors.at(i)(0);
                double g = line_colors.at(i)(1);
                double b = line_colors.at(i)(2);
                glColor3f(r, g, b);
            }

            glBegin(GL_LINES);
            glVertex3f(start(0), start(1), start(2));
            glVertex3f(end(0), end(1), end(2));
            glEnd();
        }
    }
}

void GLWidget::drawPoints()
{
    if(state_Points)
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        glColor3f(1.0, 0.0, 0.0);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        for(int i = 0; i < points.size(); ++i)
        {
            double x = points.at(i)(0);
            double y = points.at(i)(1);
            double z = points.at(i)(2);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}

void GLWidget::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    //拡大縮小・平行移動・回転を反映
    glScalef(glScale, glScale, glScale);
    glTranslatef(xOrigin, yOrigin, zOrigin);

    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

    // 正射影
    glOrtho(orthoRange[0], orthoRange[1],
            orthoRange[2], orthoRange[3],
            orthoRange[4], orthoRange[5]);

    //座標軸を描画
    drawAxis();

    //光源を設定
    drawLight();

    //objectを描画
    drawPolygon();

    // 線を描画
    drawLines();

    // 点を描画
    drawPoints();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        setXRotation(xRot + 8  *dy);
        setYRotation(yRot + 8  *dx);
    }
    else if(event->buttons() & Qt::RightButton)

    {
        setXRotation(xRot + 8  *dy);
        setZRotation(zRot + 8  *dx);
    }
    lastPos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    // 移動量
    double shiftAmount = 0.1;

    if(e->key() == Qt::Key_X)
    {
        setXRotation(0);
        setYRotation(90  *16);
        setZRotation(0);
    }
    if(e->key() == Qt::Key_Y)
    {
        setXRotation(-90  *16);
        setYRotation(0);
        setZRotation(0);
    }
    if(e->key() == Qt::Key_Z)
    {
        setXRotation(0);
        setYRotation(0);
        setZRotation(0);
    }
    if(e->key() == Qt::Key_Left)
        xOrigin = xOrigin - shiftAmount;
    if(e->key() == Qt::Key_Up)
        yOrigin = yOrigin + shiftAmount;
    if(e->key() == Qt::Key_Right)
        xOrigin = xOrigin + shiftAmount;
    if(e->key() == Qt::Key_Down)
        yOrigin = yOrigin - shiftAmount;
    if(e->key() == Qt::Key_B)
        orthoRange *= 1.0 - shiftAmount;
    if(e->key() == Qt::Key_S)
        orthoRange *= 1.0 + shiftAmount;
    if(e->key() == Qt::Key_A)
    {
        if(state_Axis)
            state_Axis = false;
        else
            state_Axis = true;
    }
    if(e->key() == Qt::Key_L)
    {
        if(state_Light)
            state_Light = false;
        else
            state_Light = true;
    }
    if(e->key() == Qt::Key_M)
    {
        if(state_Mesh)
            state_Mesh = false;
        else
            state_Mesh = true;
    }
    if(e->key() == Qt::Key_I)
    {
        if(state_Lines)
            state_Lines = false;
        else
            state_Lines = true;
    }
    if(e->key() == Qt::Key_P)
    {
        if(state_Polygon)
            state_Polygon = false;
        else
            state_Polygon = true;
    }
    if(e->key() == Qt::Key_O)
    {
        if(state_Points)
            state_Points = false;
        else
            state_Points = true;
    }

    updateGL();
}

int GLWidget::calcNormal()
{
    double v1[3];
    double v2[3];
    double cross[3];
    double length;

    //v1 = p2 - p1を求める
    for(int i = 0; i < 3; i++)
        v1[i] = p2[i] - p1[i];

    //v2 = p3 - p1を求める
    for(int i = 0; i < 3; i++)
        v2[i] = p3[i] - p1[i];

    //外積v2×v1（= cross）を求める
    for(int i = 0; i < 3; i++)
        cross[i] = v2[(i + 1) % 3] * v1[(i + 2) % 3] - v2[(i + 2) % 3] * v1[(i + 1) % 3];

    //外積v2×v1の長さ|v2×v1|（= length）を求める
    length = sqrt(cross[0]  *cross[0] + cross[1]  *cross[1] + cross[2]  *cross[2]);

    //長さ|v2×v1|が0のときは法線ベクトルは求められない
    if(length == 0.0)
        return 0;

    else
    {
        //外積v2×v1を長さ|v2×v1|で割って法線ベクトルnを求める
        for(int i = 0; i < 3; i++)
            n[i] = cross[i] / length;

        return 1;
    }
}

QPixmap GLWidget::getGLViewAsPixmap()
{
    QPixmap view = QPixmap(width(), height());
    view = renderPixmap();
    return view;
}

Mat GLWidget::getGLViewAsMat()
{
    QPixmap view = QPixmap(width(), height());
    view = renderPixmap();

    Mat viewMat(height(), width(), CV_8UC3);
    for(int i = 0; i < viewMat.rows; ++i)
    {
        uchar *p = view.toImage().scanLine(i);
        for(int j = 0; j < viewMat.cols; ++j)
        {
            viewMat.at<Vec3b>(i, j)[0] = *(p + 0);
            viewMat.at<Vec3b>(i, j)[1] = *(p + 1);
            viewMat.at<Vec3b>(i, j)[2] = *(p + 2);

            // アルファチャンネルは無視
            p += 4;
        }
    }

    return viewMat;
}

void GLWidget::setVertices(const VectorXd vertex)
{
    vertices = vertex;
    updateGL();
}

void GLWidget::setFaces(const vector<Vector3i> face)
{
    faces = face;
    updateGL();
}

void GLWidget::setLines(const vector<vector<Vector3d> > line)
{
    lines = line;
    updateGL();
}

void GLWidget::setLineColors(const vector<Vector3d> color)
{
    line_colors = color;
}

void GLWidget::setPoints(const vector<Vector3d> point)
{
    points = point;
    updateGL();
}

void GLWidget::showAxis(bool state)
{
    state_Axis = state;
    updateGL();
}

void GLWidget::showLight(bool state)
{
    state_Light = state;
    updateGL();
}

void GLWidget::setDrawScale(const double scale)
{
    glScale = scale;
    updateGL();
}

void GLWidget::setViewScale(const double scale)
{
    viewScale = scale;
    updateGL();
}

void GLWidget::setDirection(const string axis)
{
    if(axis == "x" || axis == "X")
    {
        setXRotation(0);
        setYRotation(90  *16);
        setZRotation(0);
    }
    if(axis == "y" || axis == "Y")
    {
        setXRotation(-90  *16);
        setYRotation(0);
        setZRotation(0);
    }
    if(axis == "z" || axis == "Z")
    {
        setXRotation(0);
        setYRotation(0);
        setZRotation(0);
    }

    updateGL();
}

void GLWidget::setOrthoRange(const VectorXd range)
{
    orthoRange = range;
    updateGL();
}

void GLWidget::setOrigin(const Vector3d origin)
{
    xOrigin = origin(0);
    yOrigin = origin(1);
    zOrigin = origin(2);
    updateGL();
}

void GLWidget::showLines(bool state)
{
    state_Lines = state;
    updateGL();
}

void GLWidget::showPolygon(bool state)
{
    state_Polygon = state;
    updateGL();
}

void GLWidget::showPoints(bool state)
{
    state_Points = state;
    updateGL();
}
