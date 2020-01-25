#include "face.h"

#include <QMessageBox>

QJsonArray vectorToJson(const QVector3D &vector) {
    QJsonArray result;
    for (int dim = 0; dim < 3; dim++)
        result.append(QJsonValue(vector[dim]));
    return result;
}

QVector3D vectorFromJson(const QJsonArray &array) {
    if (array.count() != 3) {
        throw std::runtime_error("Invalid size for vector: " +
                                 std::to_string(array.count()));
    }
    QVector3D result;
    for (int dim = 0; dim < 3; dim++) {
        if (!array.at(dim).isDouble())
            throw std::runtime_error("Invalid value in vector at idx: " +
                                     std::to_string(dim));
        result[dim] = array.at(dim).toDouble();
    }
    return result;
}

QJsonObject Face::toJson() const {
    QJsonArray vertices_json;
    for (const QVector3D &v : vertices)
        vertices_json.append(vectorToJson(v));
    QJsonObject result;
    result["vertices"] = vertices_json;
    result["normal"] = vectorToJson(normal);
    result["color"] = c;
    return result;
}

void Face::fromJson(const QJsonObject &json) {
    for (const std::string &field : {"vertices", "normal", "color"}) {
        if (!json.contains(QString(field.data())))
            throw std::runtime_error("Missing field '" + field + "' in json file");
    }
    vertices.clear();
    for (const QJsonValue &vertex : json["vertices"].toArray()) {
        vertices.push_back(vectorFromJson(vertex.toArray()));
    }
    normal = vectorFromJson(json["normal"].toArray());
    c = json["color"].toDouble();
}

void FaceCollection::fromJson(const QJsonArray &json) {
    faces.clear();
    for (const QJsonValue &face : json) {
        Face new_face;
        new_face.fromJson(face.toObject());
        faces.push_back(new_face);
    }
}

void FaceCollection::fromOBJ(const QString &path) {
    faces.clear();
    //qDebug()<<"we are in fromObj";

    bool errVDisplayed = false;
    bool errVTDisplayed = false;

    std::vector<QVector3D> v;
    std::vector<QVector3D> vt;
    std::vector<QVector3D> vn;


    //std::vector<Face> objFaces;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning()<<"fail to open the file";
        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                       QObject::tr("Failed to open the file.\n"),
                                       QMessageBox::Ok);
        return;
    }
    else {
        qDebug()<<"file open";
    }

    QTextStream lineStream(&file);
    while (!lineStream.atEnd())
    {
        //        QTextStream lineStream(&file, QIODevice::ReadOnly);
        //        QString token;
        //        lineStream >> token;
        //        if (token == QStringLiteral("v")) {
        //QByteArray line = file.readLine();

        QString line = lineStream.readLine();
        line = line.simplified();
        if(line.startsWith("v "))
        {
            float x, y, z;
            QStringList pieces = line.split( " " );
            if(pieces.size()==4||pieces.size()==5)
            {
                x = pieces.at(1).toFloat();
                y = pieces.at(2).toFloat();
                z = pieces.at(3).toFloat();
                v.push_back(QVector3D(x,y,z));
            }
            else {
                qWarning() << "Unsupported number of indices in a 3d vertice, here it's:"<<pieces.size()-1<<"instead of 3";
                if(!errVDisplayed){
                    int ret = QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                               QObject::tr("Unsupported number of indices in a 3d vertice.\n Do you want to display it anyways?"),
                                               QMessageBox::Yes, QMessageBox::No);
                    errVDisplayed = true;
                    switch (ret) {
                      case QMessageBox::Yes:
                          // Yes was clicked
                          continue;
                      case QMessageBox::No:
                          // No was clicked
                          return;
                    }
                }
            }


        }
        else if(line.startsWith("vt "))
        {
            float x, y, z;
            QStringList pieces = line.split( " " );
            if(pieces.size()==4||pieces.size()==5)
            {
                x = pieces.at(1).toFloat();
                y = pieces.at(2).toFloat();
                z = pieces.at(3).toFloat();
                vt.push_back(QVector3D(x,y,z));
            }
            else {
                qWarning() << "Unsupported number of indices in a 3d vertice texture, here it's:"<<pieces.size()-1<<"instead of 3";
                if(!errVTDisplayed){
                    QMessageBox::information(NULL, QObject::tr("FaceViewer"),
                                               QObject::tr("Unsupported number of indices in a 3d vertice texture.\n"),
                                               QMessageBox::Ok);
                    errVTDisplayed = true;
                }
            }
        }
        else if(line.startsWith("vn "))
        {
            float x, y, z;
            QStringList pieces = line.split( " " );
            if(pieces.size()==4||pieces.size()==5)
            {
                x = pieces.at(1).toFloat();
                y = pieces.at(2).toFloat();
                z = pieces.at(3).toFloat();
                vn.push_back(QVector3D(x,y,z));
            }
            else {
                qWarning() << "Unsupported number of indices in a 3d vertice normal, here it's:"<<pieces.size()-1<<"instead of 3";
                QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                               QObject::tr("Unsupported number of indices in a 3d vertice normal.\n"),
                                               QMessageBox::Ok);
                return;
            }
        }
        else if(line.startsWith("f "))
        {
            Face face;
            QVector3D normal = QVector3D(0,0,0);
            QStringList pieces = line.split( " " );
            if(pieces.size()<4)
            {
                qWarning() << "Unsupported number of vertices in a face";
                QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                               QObject::tr("Unsupported number of vertices in a face.\n"),
                                               QMessageBox::Ok);
                continue;
            }

            for(int i = 1; i< pieces.size();i++)
            {
                QString piece = pieces.at(i);

                QStringList indices = piece.split(QChar::fromLatin1('/'));

                if(indices.at(0).toInt()-1<0||indices.at(0).toInt()-1>=v.size())
                {
                    //TODO: error message show up
                    qWarning()<<"Be careful, this file content is invalid, the index of this face is wrong!";
                    QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                                   QObject::tr("Be careful, this file content is invalid, the index of this face is wrong!\n"),
                                                   QMessageBox::Ok);
                    return;
                }
                switch (indices.size()) {
                case 3:
                    face.hasNormal = true;
                    try {
                    face.vertices.push_back(v.at(indices.at(0).toInt()-1));
                } catch (int e) {
                        qWarning()<<"Error while push_back face vector:"<<e;
                        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                                       QObject::tr("Error while push_back face vector!\n"),
                                                       QMessageBox::Ok);
                    }
                    //process normal
                    //process texture
                    //...not needed here
                    break;
                case 2:
                    face.hasNormal = false;
                    try {
                    face.vertices.push_back(v.at(indices.at(0).toInt()-1));
                } catch (int e) {
                        qWarning()<<"error while push_back face vector:"<<e;
                        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                                       QObject::tr("Error while push_back face vector!\n"),
                                                       QMessageBox::Ok);
                    }
                    //process texture
                    //...not needed here
                    break;
                case 1:
                    face.hasNormal = false;
                    try {
                    face.vertices.push_back(v.at(indices.at(0).toInt()-1));
                } catch (int e) {
                        qWarning()<<"error while push_back face vector:"<<e;
                        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                                       QObject::tr("Error while push_back face vector!\n"),
                                                       QMessageBox::Ok);
                    }
                    break;
                default:
                    qWarning()<<"Unsurported format in a face elements";
                    QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                                   QObject::tr("Unsurported format in a face elements.\n"),
                                                   QMessageBox::Ok);
                    break;

                }
                // process normal
                if(face.hasNormal)
                {
                    if(indices.at(2).toInt()-1<0||indices.at(2).toInt()-1>=vn.size())
                    {
                        //TODO: error message show up
                        qWarning()<<"Be careful, this file content is invalid, the index of this face normal is wrong!";
                        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                                       QObject::tr("Be careful, this file content is invalid, the index of this face normal is wrong!\n"),
                                                       QMessageBox::Ok);
                        return;
                    }
                    else {
                        normal = normal + vn.at(indices.at(2).toInt()-1);
                    }
                }

            }
            if(pieces.size()!=0)
            {
                normal = normal/pieces.size();
            }

            faces.push_back(face);

        }


    }//end of while
    qDebug()<<"end of file";
    file.close();
    return;

}
void FaceCollection::fromSTL(const QString &path){
    faces.clear();

    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QVector3D normal_v;
    QVector3D point1;
    QVector3D point2;
    QVector3D point3;
    std::vector<QVector3D> tri_vertices;
    Face *face;

    int count=0;

    QString line;
    QTextStream in(&file);
    line = in.readLine();                   // solid

    while(true)
    {
        line = in.readLine().trimmed();     //facet or endsolid
        if(line.trimmed().startsWith("endsolid"))
        {
            break;
        }
        normal_v = stringToV3D(line);

        line = in.readLine();               //outer loop
        line = in.readLine().trimmed();     //vertex 1
        point1 = stringToV3D(line);

        line = in.readLine().trimmed();     //vertex 2
        point2 = stringToV3D(line);

        line = in.readLine().trimmed();     //vertex 3
        point3 = stringToV3D(line);

        line = in.readLine().trimmed();               //endloop

        line = in.readLine().trimmed();               //endfacet
        qDebug()<<line<<line.operator!=("endfacet");
        if(line.operator!=("endfacet"))
        {
            qWarning()<<"invalid stl file format ";
            QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                           QObject::tr("Invalid STL file format.\n"),
                                           QMessageBox::Ok);
            return;
        }

        tri_vertices.clear();
        tri_vertices.push_back(point1);
        tri_vertices.push_back(point2);
        tri_vertices.push_back(point3);

        setPointSize(point1);
        setPointSize(point2);
        setPointSize(point3);

        Face new_face;
        new_face.vertices = tri_vertices;
        new_face.normal = normal_v;
        new_face.c = 0.3f;
        faces.push_back(new_face);
        count++;
    }
    file.close();

}

QVector3D FaceCollection::stringToV3D(QString line)
{
    QVector3D coordinate;
        std::vector<float> point;

        QStringList strList = line.split(" ");

        int count = strList.size();
        if((strList.at(0).operator==("facet")&&count == 5) || (strList.at(0).operator==("vertex")&&count == 4))
        {

            for(int i=count-3;i<count;i++)
            {
                point.push_back(strList[i].toFloat());

            }
            coordinate.setX(point[0]);
            coordinate.setY(point[1]);
            coordinate.setZ(point[2]);
            return coordinate;

        }
        else {
            qWarning()<<"invalid stl file format!";
            QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                           QObject::tr("Invalid STL file format.\n"),
                                           QMessageBox::Ok);
            //return to the choose window
        }
}
void FaceCollection::setPointSize(QVector3D point){

    float pointX = point.x();
    float pointY = point.y();
    float pointZ = point.z();

    if(pointX < smallX)
       smallX = pointX;
    else if(pointX > largeX)
       largeX = pointX;

    if(pointY < smallY)
       smallY = pointY;
    else if(pointY > largeY)
       largeY = pointY;

    if(pointZ < smallZ)
       smallZ = pointZ;
    else if(pointZ > largeZ)
       largeZ = pointZ;
}

QVector3D FaceCollection::getCenter(){
    QVector3D center;
    center.setX( (largeX + smallX)/2 );
    center.setY( (largeY + smallY)/2 );
    center.setZ( (largeZ + smallZ)/2 );
    return center;
}

void FaceCollection::fromPGM3d(const QString &path) {
    faces.clear();
    QFile file(path);
    int Nx, Ny, Nz;
    float MColor; // Mc is maximal intensity value for voxels
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning()<<"fail to open the file";
        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                       QObject::tr("Failed to open the file. \n"),
                                       QMessageBox::Ok);
        return;
    }
    else {
        qDebug()<<"file open";
    }

    QTextStream lineStream(&file);
    QString line = lineStream.readLine();
    line = line.simplified();

    if(!line.operator==("PGM3D"))
    {
        qWarning()<<"Invalid hearder info";
        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                       QObject::tr("Invalid hearder info. \n"),
                                       QMessageBox::Ok);
        return;
    }

    line = lineStream.readLine();
    line = line.simplified();

    QStringList Ns = line.split( " " );

    if(Ns.size()!=3)
    {
        qWarning()<<"Invalid Size info";
        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                       QObject::tr("Invalid Size info.\n"),
                                       QMessageBox::Ok);
        return;
    }
    else{
        Nx = Ns.at(0).toInt();
        Ny = Ns.at(1).toInt();
        Nz = Ns.at(2).toInt();
    }

    line = lineStream.readLine();
    line = line.simplified();
    MColor = line.toFloat();
    if(MColor<=0||MColor>255)
    {
        qWarning()<<"Invalid maximum intensity value info";
        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                       QObject::tr("Invalid maximum intensity value info.\n"),
                                       QMessageBox::Ok);
        return;
    }
    QVector<float> cube1d;
    while (!lineStream.atEnd())
    {
        line = lineStream.readLine();
        cube1d.append(line.simplified().toFloat());
    }
    file.close();
    if(cube1d.size()!=Nx*Ny*Nz)
    {
        qWarning()<<"data size invalid";
        QMessageBox::critical(NULL, QObject::tr("FaceViewer"),
                                       QObject::tr("data size invalid.\n"),
                                       QMessageBox::Ok);
        return;
    }
    else {
        float cube3d[Nx][Ny][Nz];
        int n = 0;
        // read the data into 3d vector
        for(int k = 0;k < Nz; k++)
        {
            for(int j = 0;j < Ny; j++)
            {
                for(int i = 0; i < Nx; i++)
                {
                    cube3d[i][j][k] = cube1d[n];
                    n++;
                }
            }
        }

        // compare the faces

        for(int i = 0;i < Nx; i++)
        {
            for(int j = 0;j < Ny; j++)
            {
                for(int k = 0; k < Nz; k++)
                {

                    if(i!=Nx-1)
                    {
                        //compare(i,j,k)and(i+1,j,k)
                        if(cube3d[i][j][k] != cube3d[i+1][j][k])
                        {
                            Face face;

                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0, k-Nz/2.0));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0+1));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0, k-Nz/2.0+1));

                            setPointSize(QVector3D(i-Nx/2.0+1, j-Ny/2.0, k-Nz/2.0));
                            setPointSize(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0));
                            setPointSize(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0+1));
                            setPointSize(QVector3D(i-Nx/2.0+1, j-Ny/2.0, k-Nz/2.0+1));

                            face.normal = QVector3D(1,0,0);
                            face.c = cube3d[i][j][k]/MColor;
                            faces.push_back(face);



                            face.normal = QVector3D(-1,0,0);
                            face.c = cube3d[i+1][j][k]/MColor;
                            faces.push_back(face);

                        }
                    }

                    if(j != Ny-1)
                    {
                        //compare(i,j,k)and(i,j+1,k)
                        if(cube3d[i][j][k] != cube3d[i][j+1][k])
                        {
                            Face face;
                            face.vertices.push_back(QVector3D(i-Nx/2.0, j-Ny/2.0+1, k-Nz/2.0));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0+1));
                            face.vertices.push_back(QVector3D(i-Nx/2.0, j-Ny/2.0+1, k-Nz/2.0+1));

                            face.normal = QVector3D(0,1,0);
                            face.c = cube3d[i][j][k]/MColor;
                            faces.push_back(face);

                            face.normal = QVector3D(0,-1,0);
                            face.c = cube3d[i][j+1][k]/MColor;
                            faces.push_back(face);

                        }
                    }
                    if(k != Nz-1)
                    {
                        //compare(i,j,k)and(i,j,k+1)
                        if(cube3d[i][j][k] != cube3d[i][j][k+1])
                        {
                            Face face;
                            face.vertices.push_back(QVector3D(i-Nx/2.0, j-Ny/2.0, k-Nz/2.0+1));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0, k-Nz/2.0+1));
                            face.vertices.push_back(QVector3D(i-Nx/2.0+1, j-Ny/2.0+1, k-Nz/2.0+1));
                            face.vertices.push_back(QVector3D(i-Nx/2.0, j-Ny/2.0+1, k-Nz/2.0+1));

                            face.normal = QVector3D(0,0,1);
                            face.c = cube3d[i][j][k]/MColor;
                            faces.push_back(face);

                            face.normal = QVector3D(0,0,-1);
                            face.c = cube3d[i][j][k+1]/MColor;
                            faces.push_back(face);

                        }
                    }
                }
            }
        }

    }//end of else cube1.size ok
}
