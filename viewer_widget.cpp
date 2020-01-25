#include "viewer_widget.h"

#include <QFileDialog>

ViewerWidget::ViewerWidget() {
  layout = new QGridLayout(this);
  load_file_button = new QPushButton("Load file");
  layout->addWidget(load_file_button, 0, 0);
  connect(load_file_button, SIGNAL(released()), this, SLOT(loadFile()));
  gl_widget = new GLWidget();
  gl_widget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  layout->addWidget(gl_widget, 1, 0);
  alpha_label = new QLabel("Alpha:", this);
  alpha_label->setFixedWidth(90);
  alpha_slider = new QSlider(Qt::Horizontal, gl_widget);
  clear_view = new QPushButton("Reset View", this);
  set_color = new QPushButton("Set Volume Color", this);
  set_color->setCheckable(true);
  edge_btn = new QPushButton("Draw Edges", this);
  edge_btn->setCheckable(true);
  slider_layout = new QHBoxLayout(this);
  slider_layout->addWidget(alpha_label, Qt::AlignLeft);
  slider_layout->addWidget(alpha_slider);
  slider_layout->addWidget(clear_view);
  misc_layout = new QHBoxLayout(this);
  misc_layout->addWidget(edge_btn);

//  z_sort_label = new QLabel("Z-Sorting?: ", this);
  z_sorting = new QPushButton("Z-Sorting", this);
  z_sorting->setCheckable(true);
  layout->addLayout(slider_layout,2,0);
  misc_layout->addWidget(z_sorting);
  misc_layout->addWidget(set_color);

  layout->addLayout(misc_layout,3,0);
  connect(alpha_slider, SIGNAL(valueChanged(int)), gl_widget, SLOT(setAlpha(int)));
  connect(z_sorting, SIGNAL(toggled(bool)), gl_widget, SLOT(setZSorting(bool)));
  connect(clear_view,  SIGNAL(clicked()), gl_widget, SLOT(clearParams()));
  connect(set_color,  SIGNAL(toggled(bool)), gl_widget, SLOT(setColor(bool)));
  connect(edge_btn, SIGNAL(toggled(bool)), gl_widget, SLOT(setEdge(bool)));
  alpha_slider->setValue(60);
  alpha_slider->setTickInterval(5);
}

void ViewerWidget::loadFile() {
  QString file_name;
  // TODO: open a dialog to let the user choose a json file
  file_name = QFileDialog::getOpenFileName(this,
      tr("Open JSON"), "../resources", tr("Json, STL, PGM3D Files (*.json *.stl *.pgm3d *.obj)"));
  qDebug()<<file_name;
  gl_widget->loadFaces(file_name);

}

