/******************************************************************************
 * Copyright (C) 2016 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "roomlistmodel.h"

#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtCore/QDebug>

#include "libqmatrixclient/events/event.h"
#include "libqmatrixclient/events/callinviteevent.h"
#include "libqmatrixclient/events/callcandidatesevent.h"
#include "libqmatrixclient/events/callanswerevent.h"
#include "libqmatrixclient/events/callhangupevent.h"
#include "libqmatrixclient/connection.h"
#include "libqmatrixclient/room.h"
#include "libqmatrixclient/logging.h"


using namespace QMatrixClient;

const int RoomEventStateRole = Qt::UserRole + 1;

RoomListModel::RoomListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_connection = 0;
}

RoomListModel::~RoomListModel()
{
}

void RoomListModel::setConnection(QMatrixClient::Connection* connection)
{
    beginResetModel();
    m_connection = connection;
    m_rooms.clear();

    connect( connection, &QMatrixClient::Connection::newRoom, this, &RoomListModel::addRoom );
    connect( connection, &QMatrixClient::Connection::leftRoom, this, &RoomListModel::removeRoom );

    for( QMatrixClient::Room* room: connection->roomMap().values() ) {
        connect( room, &QMatrixClient::Room::namesChanged, this, &RoomListModel::namesChanged );
        connect( room, &QMatrixClient::Room::callEvent, this, &RoomListModel::callEventChanged );
        m_rooms.append(room);
    }
    endResetModel();
}

QMatrixClient::Room* RoomListModel::roomAt(int row)
{
    return m_rooms.at(row);
}

void RoomListModel::removeRoom(QMatrixClient::Room* room)
{
    int position = m_rooms.indexOf(room);
    beginRemoveRows(QModelIndex(), position, position);
    m_rooms.removeAt(position);
    endRemoveRows();
}

void RoomListModel::addRoom(QMatrixClient::Room* room)
{
    beginInsertRows(QModelIndex(), m_rooms.count(), m_rooms.count());
    connect( room, &QMatrixClient::Room::namesChanged, this, &RoomListModel::namesChanged );
    connect( room, &QMatrixClient::Room::unreadMessagesChanged, this, &RoomListModel::unreadMessagesChanged );
    connect( room, &QMatrixClient::Room::highlightCountChanged, this, &RoomListModel::highlightCountChanged );

    connect( room, &QMatrixClient::Room::callEvent, this, &RoomListModel::callEventChanged );
    m_rooms.append(room);
    endInsertRows();
}

int RoomListModel::rowCount(const QModelIndex& parent) const
{
    if( parent.isValid() )
        return 0;
    return m_rooms.count();
}

QVariant RoomListModel::data(const QModelIndex& index, int role) const
{
    if( !index.isValid() )
        return QVariant();

    if( index.row() >= m_rooms.count() )
    {
        qDebug() << "UserListModel: something wrong here...";
        return QVariant();
    }
    QMatrixClient::Room* room = m_rooms.at(index.row());
    if( role == Qt::DisplayRole )
    {
        return room->displayName();
    }
    if ( role == RoomEventStateRole )
    {
        if (room->highlightCount() > 0) {
            return "highlight";
        } else if (room->hasUnreadMessages()) {
            return "unread";
        } else {
            return "normal";
        }
    }
    return QVariant();
}

QHash<int, QByteArray> RoomListModel::roleNames() const {
    return QHash<int, QByteArray>({
                      std::make_pair(Qt::DisplayRole, QByteArray("display")),
                      std::make_pair(RoomEventStateRole, QByteArray("roomEventState"))
          });
}

void RoomListModel::namesChanged(QMatrixClient::Room* room)
{
    int row = m_rooms.indexOf(room);
    emit dataChanged(index(row), index(row));
}

void RoomListModel::unreadMessagesChanged(QMatrixClient::Room* room)
{
    int row = m_rooms.indexOf(room);
    emit dataChanged(index(row), index(row));
}

void RoomListModel::highlightCountChanged(QMatrixClient::Room* room)
{
    int row = m_rooms.indexOf(room);
    emit dataChanged(index(row), index(row));
}

void RoomListModel::callEventChanged(QMatrixClient::Room* room, QMatrixClient::RoomEvent* event) {
  switch (event->type())
  {
      case EventType::CallInvite: {
        auto callInviteEvent = static_cast<CallInviteEvent*>(event);
        emit callEvent("invite", room, callInviteEvent->toJson());
        break;
      }
      case EventType::CallCandidates: {
        auto callCandidatesEvent = static_cast<CallCandidatesEvent*>(event);
        emit callEvent("candidates", room, callCandidatesEvent->toJson());
        break;
      }
      case EventType::CallAnswer: {
        auto callAnswerEvent = static_cast<CallAnswerEvent*>(event);
        qCDebug(MAIN) << callAnswerEvent->toJson();
        emit callEvent("answer", room, callAnswerEvent->toJson());
        break;
      }
      case EventType::CallHangup: {
        auto callHangupEvent = static_cast<CallHangupEvent*>(event);
        emit callEvent("hangup", room, callHangupEvent->toJson());
        break;
      }
      default: /* Ignore events of other types */;
  }
}
