#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <vector>
#include <QGLWidget>
#include <opencv.hpp>
#include <Eigen/Eigen>

using namespace std;
using namespace cv;
using namespace Eigen;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);

    QPixmap getGLViewAsPixmap();
    Mat getGLViewAsMat();

    void setVertices(const VectorXd vertex);
    void setFaces(const vector<Vector3i> face);
    void setLines(const vector<vector<Vector3d> > line);
    void setLineColors(const vector<Vector3d> color);
    void setPoints(const vector<Vector3d> point);
    void setDrawScale(const double scale);
    void setViewScale(const double scale);
    void setDirection(const string axis);
    void setOrthoRange(const VectorXd range);
    void setOrigin(const Vector3d origin);

    void showAxis(bool state);
    void showLight(bool state);
    void showLines(bool state);
    void showPolygon(bool state);
    void showPoints(bool state);

signals:

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

protected:
    void initializeGL();
    void resizeGL(int width, int heigth);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void drawAxis();
    void drawLight();
    void drawPolygon();
    void drawLines();
    void drawPoints();

private:
    int calcNormal();

    double viewScale;
    VectorXd orthoRange;

    VectorXd vertices;
    vector<Vector3i> faces;
    vector<vector<Vector3d> > lines;
    vector<Vector3d> line_colors;
    vector<Vector3d> points;

    int xRot;
    int yRot;
    int zRot;

    double xOrigin;
    double yOrigin;
    double zOrigin;

    double glScale;
    QPoint lastPos;

    double p1[3];
    double p2[3];
    double p3[3];
    double n[3];

    bool state_Axis;
    bool state_Light;
    bool state_Mesh;
    bool state_Lines;
    bool state_Polygon;
    bool state_Points;
};

#endif // GLWIDGET_H
