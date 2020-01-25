#pragma once

#include <QFile>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QString>

#include "face.h"

class GLWidget : public QOpenGLWidget {
public:
  Q_OBJECT
public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget() override;
  QSize sizeHint() const override { return QSize(400, 400); }

  void loadFaces(const QString &path);
public slots:
    void setAlpha(int x);
    void setZSorting(bool);
    void clearParams();
    void setColor(bool);
    void setEdge(bool);

protected:
  void initializeGL() override;
  void paintGL() override;
  QPointF firstPos, lastPos;
  float xRot, yRot, zRot, dX, dY, zoom, alpha, speedup;
  int _selection;
  bool isShift, zSort;
  bool drawEdge, highlightFace, volumeColor;
  GLenum rendering_mode;
  std::vector<QVector3D> vertices_collection;
  void wheelEvent(QWheelEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void qNormalizeAngle(int &angle);
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void setTranslation(int x, int y);
  virtual int pnpoly(std::vector<QVector3D> poly, QPoint p);

  FaceCollection face_collection;
  std::vector<Face> init_faces;

  QMatrix4x4 H;
  QMatrix4x4 P;
  GLint view[4];

};
