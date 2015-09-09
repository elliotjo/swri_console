#ifndef SWRI_CONSOLE_LOG_DATABASE_PROXY_MODEL_H_
#define SWRI_CONSOLE_LOG_DATABASE_PROXY_MODEL_H_

#include <QAbstractListModel>
#include <stdint.h>
#include <set>
#include <string>
#include <deque>

namespace swri_console
{
class LogDatabase;
class LogEntry;
class LogDatabaseProxyModel : public QAbstractListModel
{
  Q_OBJECT
  
 public:
  LogDatabaseProxyModel(LogDatabase *db);
  ~LogDatabaseProxyModel();

  void setNodeFilter(const std::set<std::string> &names);
  void setSeverityFilter(uint8_t severity_mask);
  
  virtual int rowCount(const QModelIndex &parent) const;
  virtual QVariant data(const QModelIndex &index, int role) const;

  void reset();

 Q_SIGNALS:
  void messagesAdded();
                      
 public Q_SLOTS:
  void processNewMessages();
  void processOldMessages();
  
 private:
  LogDatabase *db_;

  void scheduleIdleProcessing();
  
  bool acceptLogEntry(const LogEntry &item);

  size_t earliest_index_;
  size_t latest_index_;
  
  std::set<std::string> names_;
  uint8_t severity_mask_;

  std::deque<size_t> early_mapping_;
  std::deque<size_t> msg_mapping_;
};
}  // swri_console
#endif  // SWRI_CONSOLE_LOG_DATABASE_PROXY_MODEL_H_