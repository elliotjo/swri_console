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
#include <swri_console/session.h>

#include <QDebug>

namespace swri_console
{
LogDatabase::LogDatabase()
{
}

LogDatabase::~LogDatabase()
{
}

void LogDatabase::clear()
{
  sessions_.clear();
  session_ids_.clear();
  node_name_from_id_.clear();
  node_id_from_name_.clear();
  node_ids_.clear();  
  Q_EMIT databaseCleared();
}

int LogDatabase::createSession(const QString &name)
{
  // Find an available id
  int sid = sessions_.size();
  while (sessions_.count(sid) != 0) { sid++; }

  Session &session = sessions_[sid];
  session.id_ = sid;
  session.name_ = name;
  session.db_ = this;

  session_ids_.push_back(sid);
  Q_EMIT sessionAdded(sid);
  return sid;
}

void LogDatabase::deleteSession(int sid)
{
  if (sessions_.count(sid) == 0) {
    qWarning("Attempt to delete invalid session %d", sid);
    return;
  }

  sessions_.erase(sid);
  auto iter = std::find(session_ids_.begin(), session_ids_.end(), sid);
  if (iter != session_ids_.end()) {
    session_ids_.erase(iter);
  } else {
    qWarning("Unexpected inconsistency: session %d was not found in session id vector.", sid);
  } 

  Q_EMIT sessionDeleted(sid);  
}

void LogDatabase::renameSession(int sid, const QString &name)
{
  // It feels weird that this functionality is provided by LogDatabase
  // instead of the session, but otherwise we have to either emit
  // signals from the session (and deal with how those signals should
  // be connected up), or we have to emit the LogDatabase's signal
  // from Session, which seems icky.  Or we have to provide a method
  // for session to notify the database that the sesion was renamed,
  // at which point we might as well just do the renaming here...
  Session &session = this->session(sid);
  if (!session.isValid()) {
    return;
  }

  session.name_ = name;
  Q_EMIT sessionRenamed(sid);  
}

int LogDatabase::indexOfSession(int sid)
{
  for (size_t i = 0; i < session_ids_.size(); i++) {
    if (session_ids_[i] == sid) {
      return i;
    }
  }
  return -1;
}

void LogDatabase::moveSession(int move_sid, int before_sid)
{
  if (move_sid == before_sid) {
    qWarning("I don't know how to move a session to after itself.");
    return;
  }
  
  int move_idx = indexOfSession(move_sid);
  if (move_idx < 0) {
    qWarning("Failed to move session: Could not find move_sid=%d in id vector.", move_sid);
    return;
  }

  if (before_sid < 0) {
    if (move_idx == 0) {
      return;
    }
  } else {
    // This is to check for missing before_sid and for no-op
    // conditions.  We will find the index again later after the
    // move_sid is removed from the list.
    int next_idx = indexOfSession(before_sid);
    if (next_idx < 0) {
      qWarning("Failed to move sesion: Could not find before_sid=%d in id vector.", before_sid);
      return;
    }
    
    if (next_idx+1 == move_idx) {
      // No change.
      return;
    }
  }
  
  // Remove the moving sid.  
  for (size_t i = move_idx; i+1 < session_ids_.size(); i++) {
    session_ids_[i] = session_ids_[i+1];
  }
  session_ids_.back() = -1;

  int next_idx;
  if (before_sid < 0) {
    next_idx = 0;
  } else {
    next_idx = indexOfSession(before_sid)+1;
  }
  
  // Scoot everything after the destination back one.
  for (size_t i = session_ids_.size()-1; i > next_idx; i--) {
    session_ids_[i] = session_ids_[i-1];
  }

  // Put the SID in the right place.
  session_ids_[next_idx] = move_sid;
  
  Q_EMIT sessionMoved(move_sid);
}

Session& LogDatabase::session(int sid)
{
  if (sessions_.count(sid)) {
    return sessions_.at(sid);
  }

  qWarning("Request for invalid session %d", sid);
  invalid_session_ = Session();
  return invalid_session_;
}

const Session& LogDatabase::session(int sid) const
{
  if (sessions_.count(sid)) {
    return sessions_.at(sid);
  }

  qWarning("Request for invalid session %d", sid);
  invalid_session_ = Session();
  return invalid_session_;
}

int LogDatabase::lookupNode(const std::string &name)
{
  if (node_id_from_name_.count(name) == 0) {    
    int nid = node_name_from_id_.size();
    while (node_name_from_id_.count(nid) != 0) { nid++; }

    node_name_from_id_[nid] = QString::fromStdString(name);
    node_id_from_name_[name] = nid;

    // Rebuild the node id vector from the map.  We're using the fact
    // that std::map orders it's keys to get the node names in
    // alphabetical order. 
    node_ids_.clear();
    for (auto const &it : node_id_from_name_) {
      node_ids_.push_back(it.second);
    }
    
    Q_EMIT nodeAdded(nid);
  }

  return node_id_from_name_.at(name);
}

QString LogDatabase::nodeName(int nid) const
{
  if (node_name_from_id_.count(nid)) {
    return node_name_from_id_.at(nid);
  }

  qWarning("Request for invalid node %d", nid);
  return QString("<invalid node %1").arg(nid);
}

int LogDatabase::lookupOrigin(const int nid, const rosgraph_msgs::Log &log)
{
  LogOrigin origin;
  origin.severity = log.level;
  origin.node_id = nid;
  origin.file = log.file;
  origin.function = log.function;
  origin.line = log.line;

  auto it = origin_id_from_value_.find(origin);
  if (it == origin_id_from_value_.end()) {
    int oid = origin_value_from_id_.size();
    while (origin_value_from_id_.count(oid) != 0) { oid++; }

    origin_value_from_id_[oid] = origin;
    origin_id_from_value_[origin] = oid;

    qWarning("origin count: %zu", origin_value_from_id_.size());
    return oid;
  } else {
    return it->second;
  }    
}

uint8_t LogDatabase::originSeverity(int oid) const
{
  if (origin_value_from_id_.count(oid)) {
    return origin_value_from_id_.at(oid).severity;
  }

  qWarning("Request for invalid origin %d", oid);
  return 0xFF;
}

int LogDatabase::originNodeId(int oid) const
{
  if (origin_value_from_id_.count(oid)) {
    return origin_value_from_id_.at(oid).node_id;
  }

  qWarning("Request for invalid origin %d", oid);
  return 0xFF;
}

QString LogDatabase::originNodeName(int oid) const
{
  if (origin_value_from_id_.count(oid)) {
    return nodeName(origin_value_from_id_.at(oid).node_id);
  }

  qWarning("Request for invalid origin %d", oid);
  return QString("<Invalid origin %1").arg(oid);
}

QString LogDatabase::originFile(int oid) const
{
  if (origin_value_from_id_.count(oid)) {
    return QString::fromStdString(origin_value_from_id_.at(oid).file);
  }

  qWarning("Request for invalid origin %d", oid);
  return QString("<Invalid origin %1").arg(oid);
}

QString LogDatabase::originFunction(int oid) const
{
  if (origin_value_from_id_.count(oid)) {
    return QString::fromStdString(origin_value_from_id_.at(oid).function);
  }

  qWarning("Request for invalid origin %d", oid);
  return QString("<Invalid origin %1").arg(oid);
}

uint32_t LogDatabase::originLine(int oid) const
{
  if (origin_value_from_id_.count(oid)) {
    return origin_value_from_id_.at(oid).line;
  }

  qWarning("Request for invalid origin %d", oid);
  return 0;
}
}  // namespace swri_console
