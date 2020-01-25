#include <QFile>
#include <QString>
#include <QTransform>
#include <QtGui>
#include <QKeyEvent>
#include "glwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <iostream>


#include <QApplication>
#include <QDebug>
#include <QColorDialog>

float r =100.0,g=0.0,b=0.0;
int count_set_color=-1;

// Vertix Comparator
struct sortFaceByZ {
    sortFaceByZ(QMatrix4x4 M, QMatrix4x4 PP) { this->H = M; this->P = PP;}
    bool operator () (const Face &face1, const Face &face2){

        float face1_z = 0.0f, face2_z = 0.0f;
        for (unsigned int i=0; i<4; i++){
            QVector3D face1_cam = P.transposed() * H.transposed() * face1.vertices[i];
            QVector3D face2_cam = P.transposed() * H.transposed() * face2.vertices[i];
            face1_z += face1_cam.z();
            face2_z += face2_cam.z();
        }
        return face1_z > face2_z;
    }
    QMatrix4x4 H;
    QMatrix4x4 P;
};

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    xRot = 0;
    yRot = 0;
    zRot = 1;
    dX = 0;
    dY = 0;
    zoom = 1;
    speedup = 1;
    isShift = false;
    zSort = false;
    rendering_mode = GL_POLYGON;
    H = QMatrix4x4();
    P = QMatrix4x4();
}

GLWidget::~GLWidget() {}

void GLWidget::loadFaces(const QString &path) {
    clearParams();
    QFile data_file(path);

    while (!data_file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open file");
        QMessageBox::critical(this, tr("FaceViewer"),
                                       tr("Failed to open file.\n"),
                                       QMessageBox::Ok);
        QString file_name;
        // TODO: open a dialog to let the user choose a json file
        file_name = QFileDialog::getOpenFileName(this,
            tr("Open JSON"), "../resources", tr("Json, STL, PGM3D Files (*.json *.stl *.pgm3d *.obj)"));
        return loadFaces(file_name);
//        exit(-1);
    }
    /*check the file extension: pmg3d, json, stl, obj, others*/
    QFileInfo fInfo(data_file);
    QString extension = fInfo.suffix();

    if(!QString::compare(extension,"json"))
    {
        QByteArray json_data = data_file.readAll();
        QJsonDocument json_document(QJsonDocument::fromJson(json_data));
        face_collection.fromJson(json_document.array());
        rendering_mode = GL_QUADS;
    }
    else if(!QString::compare(extension,"obj")){
        //TO DO
        face_collection.fromOBJ(path);
        rendering_mode = GL_POLYGON;
    }
    else if(!QString::compare(extension,"pgm3d")){
        //TO DO
        face_collection.fromPGM3d(path);
        rendering_mode = GL_POLYGON;
    }
    else if(!QString::compare(extension,"stl")){
        //TO DO
        face_collection.fromSTL(path);
        rendering_mode = GL_TRIANGLES;
        zoom = 256;
    }
    else {
        qWarning("Unknown file format");
        QMessageBox::critical(this, tr("FaceViewer"),
                                       tr("Unknown file format.\n"),
                                       QMessageBox::Ok);
    }

    while(face_collection.faces.empty()){
        QString file_name;
        // TODO: open a dialog to let the user choose a json file
        file_name = QFileDialog::getOpenFileName(this,
            tr("Open JSON"), "../resources", tr("Json, STL, PGM3D Files (*.json *.stl *.pgm3d *.obj)"));
        return loadFaces(file_name);
    }
    init_faces = face_collection.faces;
    // TODO: update the display inside the widget

    update();
}


void GLWidget::initializeGL() {
    // TODO: initialize the OpenGL to display 3d data
    glClearColor(0.60f, 0.8f, 0.7f, 1); // Set background color to black and opaque
    glClearDepth(1.0);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glEnable(GL_NORMALIZE);
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


int count_c=0, coun=0;
float color_array[20][20];


void GLWidget::paintGL() {
    QSize viewport_size = size();
    glViewport(0, 0, viewport_size.width(), viewport_size.height());

    glMatrixMode(GL_PROJECTION);
    // TODO: define the point of view
    glLoadIdentity();

//    QVector3D c = face_collection.getCenter();
//    if(face_collection.smallX < face_collection.largeX && face_collection.smallY < face_collection.largeY && face_collection.smallZ <= face_collection.largeZ){
//        GLdouble diamX = face_collection.largeX - face_collection.smallX;
//        GLdouble diamY = face_collection.largeY - face_collection.smallY;
//        GLdouble diamZ = face_collection.largeZ - face_collection.smallZ;
//        GLdouble zNear = face_collection.smallZ, zFar = face_collection.largeZ;

//        GLdouble left = c.x() - face_collection.smallX;
//        GLdouble right = c.x() + face_collection.largeX;
//        GLdouble bottom = c.y() - face_collection.smallY;
//        GLdouble top = c.y() + face_collection.largeY;
//        glOrtho(left, right, bottom, top, zNear, zFar);
//    } else {
//        glOrtho(0,viewport_size.width(),0,viewport_size.height()/6,-164,164);
        glOrtho(-viewport_size.width()/6,viewport_size.width()/6,-viewport_size.height()/6,viewport_size.height()/6,-164,164);
//    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(dX * speedup / 8.0f, dY * speedup /8.0f, 0.0);
    glScalef(zoom , zoom , zoom );
    glRotatef(speedup * xRot / 16.0f, 1.0, 0.0, 0.0);
    glRotatef(speedup * yRot / 16.0f, 0.0, 1.0, 0.0);
    glRotatef(speedup * zRot / 16.0f, 0.0, 0.0, 1.0);

    //glRotatef(180.0f - (xRot / 16.0f), 1.0, 0.0, 0.0);
    float MM[16];
    float PP[16];
    glGetFloatv(GL_MODELVIEW_MATRIX , MM);
    glGetFloatv(GL_PROJECTION_MATRIX , PP);

    H = QMatrix4x4(MM);
    P = QMatrix4x4(PP);

    /**
     * Highlighting/Selecting faces
     */
    glGetIntegerv(GL_VIEWPORT, view);
    QVector3D wP;
    if(highlightFace)
        wP = QVector3D(lastPos.x(), view[3]-lastPos.y(), 0.1f).unproject(P, H, QRect(view[0], view[1], view[2], view[3]));

//    glBegin(GL_TRIANGLES);
//    glColor3d(1,0,1);
//    glVertex3f(p.x(), view[3]-p.y(),0);
//    glVertex3f(p.x()+60, view[3]-p.y(),0);
//    glVertex3f(p.x()+30, view[3]-p.y()+100,0);
//    glEnd();

    // TODO: sort the faces
    if(zSort)
        std::sort(face_collection.faces.begin(), face_collection.faces.end(), sortFaceByZ(H,P));
    // TODO: draw the model

    for(unsigned int j = 0; j < face_collection.faces.size(); j++)
    {
        Face face = face_collection.faces[j];
        if (zSort && face.hasNormal){
//            qDebug()<<face.normal;
            QVector3D norm = H.transposed() * P.transposed() * face.normal;
            if( norm.z() >= 0 )
                continue;
        } else if(zSort){
            glCullFace(GL_FRONT);
        }
        std::vector<QVector3D> vertices;
        float x,y,z;
        vertices = face.vertices;

        //////////////////////////////////////////////////////////////
        int f_color=0;

        if(count_set_color>=0 & coun> 0)
        {
            color_array[count_set_color][2] = r;
            color_array[count_set_color][3] = g;
            color_array[count_set_color][4] = b;
        }
        if(count_set_color>=count_c)
        {
              count_set_color=0;
        }
        if(coun==0)

        {
            count_set_color=-1;
            color_array[0][0] = face.c;
            color_array[0][1] = 1.0;
            f_color = color_array[0][1];
        }

        else
        {
            float c_n=0;
            float c_f=0.0;
           for (int j=0; j<count_c;j++)
           {
               if(face.c==color_array[j][0])
               {
                    f_color = color_array[j][1];
               }
               else
               {
                   c_n++;
                   c_f++;
                }
           }

           if(count_c==c_n)
           {
               color_array[count_c][1]=c_n;
               color_array[count_c][0]=face.c;
               count_c ++;
               f_color = c_n;
           }
        }
        float f_r=face.c,f_g=face.c,f_b=face.c;

        if(volumeColor && f_color>=1)
        {

                f_r =  color_array[f_color-1][2]/255;
                f_g =  color_array[f_color-1][3]/255;
                f_b =  color_array[f_color-1][4]/255;
        }
        //qDebug() << f_b<<f_g<<f_r;

        coun++;
        //qDebug()<<count_c;


        ////////////////////////////////////////////////////////////////////

        glBegin(rendering_mode);

        /**
         * Highlighting/Selecting faces
         * Check if the point lies in the vertice using the ray-casting algorithm
         */
        if(!highlightFace || !pnpoly(vertices, QPoint(wP.x(), wP.y())))
            glColor4f(f_r,f_g,f_b,alpha);
        else
            glColor4f(1,1,1,alpha);

        for(unsigned int i = 0; i < vertices.size(); i++)
        {
            x = vertices[i].x();
            y = vertices[i].y();
            z = vertices[i].z();
            glVertex3f(x,y,z);
        }
        glEnd();

        /**
         * Edge drawing
         */
        if(drawEdge){
            glBegin(GL_LINE_STRIP);

            glColor4f(0.8,0.2,0,alpha);

            for(unsigned int i = 0; i < vertices.size(); i++)
            {
                x = vertices[i].x();
                y = vertices[i].y();
                z = vertices[i].z();
                glVertex3f(x,y,z);
            }
            glEnd();
        }

    }

    glPopMatrix();
}
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier))
    {
        highlightFace = true;
        update();
    }

}
void GLWidget::mouseReleaseEvent(QMouseEvent *event){
    highlightFace = false;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    // TODO:
    // - Capture mouse motions

    float dx = event->pos().x()-lastPos.x();
    float dy = event->pos().y()-lastPos.y();

    // - If left click is pressed: rotate
    if(event->buttons() & Qt::LeftButton)
    {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
        //setZRotation(zRot + 8 * dx);

    }

    // - If right click is pressed: translate
    if(event->buttons() & Qt::RightButton)
    {
        //qDebug() << "Right Click";
        setTranslation(dx, dy);
    }
    lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    // TODO: zoom
//    float x = event->x();
//    float y = event->y();
    QPoint numDegrees = event->angleDelta() / 8;

    if(numDegrees.y() < 0){
        zoom = (zoom-0.2f<0.0f)? 0 : zoom-0.2f;
        if (isShift) { zoom /= 2;}
    } else if(numDegrees.y()>0){
        zoom += 0.2f;
        if (isShift) { zoom *= speedup;}
    }
    update();
    event->accept();
}
void GLWidget::keyPressEvent(QKeyEvent *event){

    if(event->key() ==Qt::Key_Shift)
    {
        speedup = 3;
        isShift = true;
    }
    event->accept();
}

void GLWidget::keyReleaseEvent(QKeyEvent *event){
    if(event->key() ==Qt::Key_Shift)
    {
        speedup = 1;
        isShift = false;
    }
    event->accept();
}
void GLWidget::qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        update();

    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        update();
    }
}

void GLWidget::setTranslation(int x, int y){
    dX += x;
    dY -= y;
    update();
}
void GLWidget::setAlpha(int x){
    alpha = x/200.0f;

    update();
}
void GLWidget::setZSorting(bool z){
    zSort = z;
    if(!zSort)
        face_collection.faces = init_faces;
    update();
}


void GLWidget::clearParams(){
    glMatrixMode(GL_MODELVIEW);
    float m[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    glPushMatrix();
    glLoadMatrixf(m);

//    glLoadIdentity();
    xRot = 0;
    yRot = 0;
    zRot = 0;
    dX = 0;
    dY = 0;
    zoom = 1;
    speedup = 1;
    update();
}

int GLWidget::pnpoly(std::vector<QVector3D> poly, QPoint p)
{
  int nvert = poly.size();
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((poly[i].y()>p.y()) != (poly[j].y()>p.y())) &&
     (p.x() < (poly[j].x()-poly[i].x()) * (p.y()-poly[i].y()) / (poly[j].y()-poly[i].y()) + poly[i].x()) )
       c = !c;
  }
  return c;
}


/////////////////////////////////////////////////////
class QColorDialogTester : public QWidget
{
public:

};
void GLWidget::setEdge(bool b){
    drawEdge = b;
    update();
}
void GLWidget::setColor(bool v){
    volumeColor = v;
    if(volumeColor){
        for(int i=0; i< count_c; i++){
            QColorDialogTester color_test;
              //color_test.onColor();
            bool ok;
            QColor color;
            QString clrDialogTitle =  QString("Select Color for Volume %1").arg(i+1);
            color = QColorDialog::getColor(Qt::red,NULL, clrDialogTitle).rgb();
            QString ColorHex=color.name().remove('#');
            int ColorInt = ColorHex.toInt(&ok,16);
            QString ColorBin= ColorHex.setNum(ColorInt, 10);

            r = ColorInt >> 16 ;
            g = (ColorInt & 0x00ff00) >> 8;
            b = (ColorInt & 0x0000ff) ;
            //lab->setText(ColorBin);
            if( color.isValid() )
            {
                count_set_color++;
                if(count_c>0)
                    qDebug() <<" Volume "<< count_set_color%count_c+1;
                qDebug() << "Color Choosen :";
                qDebug() << "r : " << r;
                qDebug() << "g : " << g;
                qDebug() << "b : " << b;

            }
            update();
        }
    }
    if(!v)
        update();

}
/////////////////////////////////////

