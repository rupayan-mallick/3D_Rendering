#pragma once

#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>

#include "glwidget.h"

class ViewerWidget : public QWidget {
public:
  Q_OBJECT
public:
  ViewerWidget();

  QGridLayout *layout;
  QHBoxLayout *slider_layout;
  QHBoxLayout *misc_layout;
  QPushButton *load_file_button;
  QSlider *alpha_slider;
  QLabel *alpha_label;
  QLabel *z_sort_label;
  QPushButton *z_sorting;
  QPushButton *clear_view;
  QPushButton *set_color;
  QPushButton *edge_btn;
  GLWidget *gl_widget;
public slots:
  void loadFile();



};
