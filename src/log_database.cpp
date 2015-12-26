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

#include <swri_console/log_database.h>

namespace swri_console
{
LogDatabase::LogDatabase()
  :
  min_time_(ros::TIME_MAX)
{
}

LogDatabase::~LogDatabase()
{
}

void LogDatabase::clear()
{
  std::map<std::string, size_t>::iterator iter;
  msg_counts_.clear();
  log_.clear();
  Q_EMIT databaseCleared();
}

void LogDatabase::queueMessage(const rosgraph_msgs::LogConstPtr msg)
{
  if (msg->header.stamp < min_time_) {
    min_time_ = msg->header.stamp;
    Q_EMIT minTimeUpdated();
  }
  
  msg_counts_[msg->name]++;

  LogEntry log;
  log.stamp = msg->header.stamp;
  log.level = msg->level;
  log.node = msg->name;
  log.file = msg->file;
  log.function = msg->function;
  log.line = msg->line;
  log.seq = msg->header.seq;

  QStringList text = QString(msg->msg.c_str()).split('\n');
  // Remove empty lines from the back.
  while(text.size() && text.back().isEmpty()) { text.pop_back(); }
  // Remove empty lines from the front.
  while(text.size() && text.front().isEmpty()) { text.pop_front(); }  
  log.text = text;

  log_.push_back(log);  
}
}  // namespace swri_console
