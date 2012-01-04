/* qshutdown, a program to shutdown/reboot/suspend/hibernate the system
 * Copyright (C) 2010-2011 Christian Metscher <hakaishi@web.de>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INFO_H
#define INFO_H

#include <QTextEdit>

class Info : public QTextEdit{
     Q_OBJECT

    public:
     Info(QWidget *parent = 0);
     ~Info();
     bool getClosed();

    private:
     bool isClosed;

    signals:
     void starting();
     void finishing();

    protected:
     virtual void showEvent(QShowEvent* show_);
     virtual void closeEvent(QCloseEvent* close_);
};

#endif //INFO_H