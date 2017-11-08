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

#ifndef ROOMLISTMODEL_H
#define ROOMLISTMODEL_H

#include <QtCore/QAbstractListModel>

#include "events/event.h"
#include "libqmatrixclient/events/event.h"

namespace QMatrixClient
{
    class Connection;
    class Room;
}

class RoomListModel: public QAbstractListModel
{
        Q_OBJECT
    public:
        RoomListModel(QObject* parent=0);
        virtual ~RoomListModel();

        Q_INVOKABLE void setConnection(QMatrixClient::Connection* connection);
        Q_INVOKABLE QMatrixClient::Room* roomAt(int row);

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        Q_INVOKABLE int rowCount(const QModelIndex& parent=QModelIndex()) const override;

        QHash<int, QByteArray> roleNames() const override;

    signals:
          void callEvent(const QString& type, QMatrixClient::Room* room, const QJsonObject& event);

    private slots:
        void namesChanged(QMatrixClient::Room* room);
        void unreadMessagesChanged(QMatrixClient::Room* room);
        void addRoom(QMatrixClient::Room* room);
        void highlightCountChanged(QMatrixClient::Room* room);

        void callEventChanged(QMatrixClient::Room* room, QMatrixClient::RoomEvent* event);

    private:
        QMatrixClient::Connection* m_connection;
        QList<QMatrixClient::Room*> m_rooms;
        void removeRoom(QMatrixClient::Room* room);
};

#endif // ROOMLISTMODEL_H
