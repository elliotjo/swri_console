// *****************************************************************************
//
// Copyright (c) 2015, Southwest Research Institute® (SwRI®)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Southwest Research Institute® (SwRI®) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL Southwest Research Institute® BE LIABLE 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// *****************************************************************************
#include <swri_console/save_file_dialog.h>

#include <QCheckBox>
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QDebug>

namespace swri_console
{
SaveFileDialog::SaveFileDialog(
  QWidget *parent)
  :
  QFileDialog(parent, "Save Logs"),
  bag_filter_(tr("Bag Files (*.bag)")),
  txt_filter_(tr("Txt Files (*.txt)"))
{
  setOption(QFileDialog::DontUseNativeDialog);

  {
    QString default_name =
      "console-" +
      QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + 
      ".bag";
    selectFile(QDir::homePath() + QDir::separator() + default_name);
  }

  {
    QStringList filters;
    filters.append(bag_filter_);
    filters.append(txt_filter_);
    setNameFilters(filters);
    QObject::connect(this, SIGNAL(filterSelected(const QString&)),
                     this, SLOT(handleFilterSelected(const QString&)));
  }
  
  auto *options_widget = new QWidget(this);
  {
    auto *grid = dynamic_cast<QGridLayout*>(layout());
    if (grid) {
      int row = grid->rowCount();
      grid->addWidget(options_widget, row, 0, 1, grid->columnCount());
      grid->setRowStretch(row, 0);
    }
  }
  
  auto *hbox = new QHBoxLayout(options_widget);  
  auto *selection_group = new QGroupBox("Selection");
  hbox->addWidget(selection_group);
  {
    auto *vbox = new QVBoxLayout(selection_group);
    all_logs_ = new QRadioButton("Save all logs", this);
    all_logs_in_sessions_ = new QRadioButton("Save all logs in selected sessions", this);
    all_logs_in_sessions_->setChecked(true);
    selected_logs_ = new QRadioButton("Save selected logs only", this);

    vbox->addWidget(all_logs_);
    vbox->addWidget(all_logs_in_sessions_);
    vbox->addWidget(selected_logs_);
  }
  
  auto *options_group = new QGroupBox("Options");
  hbox->addWidget(options_group);
  {
    auto *vbox = new QVBoxLayout(options_group);
    compression_ = new QCheckBox("Compress bag files", this);
    compression_->setChecked(true);
    compression_->setToolTip("Enable rosbag's built-in compression (BZ2)");

    include_session_headers_ = new QCheckBox("Include session separators", this);
    include_session_headers_->setChecked(true);
    include_session_headers_->setToolTip(
      "Include a log message for the start\n"
      "of each session to track where logs\n"
      "originated from and allow them to be\n"
      "separated out again when loading the\n"
      "bag file.");

    include_extended_info_ = new QCheckBox("Include extended info", this);
    include_extended_info_->setChecked(true);
    include_extended_info_->setToolTip(
      "Include extended info (node, file,\n"
      "function, line, etc) for each log when\n"
      "exporting to text files.");
    
    vbox->addWidget(compression_);
    vbox->addWidget(include_session_headers_);
    vbox->addWidget(include_extended_info_);
  }
  
  hbox->addStretch(1.0);

  setNameFilter(bag_filter_);
}

SaveFileDialog::~SaveFileDialog()
{
}

void SaveFileDialog::handleFilterSelected(const QString &filter)
{
  if (filter == bag_filter_) {
    compression_->setEnabled(true);
    include_session_headers_->setEnabled(true);
    include_extended_info_->setEnabled(false);
  } else {
    compression_->setEnabled(false);
    include_session_headers_->setEnabled(true);
    include_extended_info_->setEnabled(true);
  }
}

bool SaveFileDialog::includeAll() const
{
  return all_logs_->isChecked();
}

bool SaveFileDialog::includeSessions() const
{
  return all_logs_in_sessions_->isChecked();
}

bool SaveFileDialog::includeSelected() const
{
  return selected_logs_->isChecked();
}

bool SaveFileDialog::compression() const
{
  return compression_->isChecked();
}

bool SaveFileDialog::sessionHeaders() const
{
  return include_session_headers_->isChecked();
}

bool SaveFileDialog::extendedInfo() const
{
  return include_extended_info_->isChecked();
}
}  // namespace swri_console