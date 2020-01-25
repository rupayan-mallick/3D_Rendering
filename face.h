#pragma once

#pragma once

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector3D>

#include <QObject>
#include <QString>
#include <iostream>
#include <string>
#include <QDebug>


QJsonArray vectorToJson(const QVector3D &vector);
QVector3D vectorFromJson(const QJsonArray &array);

class Face {
public:
  std::vector<QVector3D> vertices;
  QVector3D normal;
  float c;
  bool hasNormal = true;//only consider .obj file maybe false

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &json);
};

class FaceCollection {
public:
  std::vector<Face> faces;
  float smallX, smallY, smallZ, largeX,largeY, largeZ;

  void fromJson(const QJsonArray &json);
  void fromPGM3d(const QString &path);
  QVector3D stringToV3D(QString line);
  void fromOBJ(const QString &path);
  void fromSTL(const QString &path);
  void setPointSize(QVector3D point);
  QVector3D getCenter();

};
