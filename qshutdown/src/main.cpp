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
#include "gui.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QTextStream>

bool verbose = false; //this is a global variable needed in power.cpp

int main(int argc, char *argv[]){

     // here the QT_program is created
     QApplication app(argc, argv);

     //Qt translations
     QTranslator qtTranslator;
     qtTranslator.load("qt_" + QLocale::system().name(),
       QLibraryInfo::location(QLibraryInfo::TranslationsPath));
     app.installTranslator(&qtTranslator);

     //My translations
     QTranslator translator;
     QString trDir = "/usr/share/qshutdown/translations/";
     translator.load(trDir + "qshutdown_" + QLocale::system().name());
     app.installTranslator(&translator);

     QTextStream myOutput(stdout);

     QString infoStr = QString(QObject::tr("qshutdown will show itself 3 times as a warning "
       "if there are less than 70 seconds left.<br/><br/>This program uses qdbus to send a "
       "shutdown/reboot/suspend/hibernate request to either the gnome- or "
       "kde-session-manager, to HAL/ConsoleKit/DeviceKit/UPower and if none of these works, the "
       "command 'sudo shutdown' will be used (note that when sending the shutdown request to HAL or "
       "ConsoleKit, or the shutdown command is used, the Session will never be saved. If the "
       "shutdown command is used, the program will only be able to shutdown and reboot). So "
       "if nothing happens when the shutdown- or reboot-time is reached, it means that one "
       "lacks the rights for the shutdown command. In this case one can do the following:"
       "<br/><br/>Paste the following in a terminal:<pre>EDITOR=nano sudo -E visudo</pre>and "
       "add this line:<pre>* ALL = NOPASSWD:/sbin/shutdown</pre>whereas * replaces the "
       "username or %groupname.<br/><br/>The maximum Number of countdown_minutes is "
       "1440 (24 hours).<br/>The configurationfile (and logfile) is located at "
       "<i>~/.qshutdown/</i> (under Linux/Unix).<br/><br/><b>For admins:</b><br/>If you want "
       "qshutdown to run with \"parental lock\" for every user, you can do "
       "\"sudo cp /usr/share/qshutdown/autostart/99qshutdown /etc/X11/Xsession.d/\" "
       "and set the option Lock_all in /root/.qshutdown/qshutdown.conf to true. Note that "
       "qshutdown has to start once to generate the qshutdown.conf. Furthermore there is a "
       "need to add the following line to the sudoers (as above):"
       "<pre>* ALL = NOPASSWD:/usr/bin/qshutdown</pre><br/>If you should ever forget "
       "your set password, just remove the whole line starting with \"Password\" manually from "
       "the qshutdown.conf.<br/><br/><b>Hints on usage:</b><br/>If you want qshutdown to stop "
       "\"bugging\" you, just remove the hook from "
       "\"warnings on?\".<br/><br/><b>Hotkeys:</b><table border=\"1\"><tr><td>Ctrl+I</td><td>(this)"
       " information window</td></tr><tr><td>Ctrl+Q</td><td>Quit</td></tr><tr><td>Ctrl+P</td>"
       "<td>Prefereces</td></tr><tr><td>Ctrl+L</td><td>write the run time once into the logfile (works "
       "only if qshutdown quits. To make it permanent set it in the preferences.)</td></tr><tr>"
       "<td>Ctrl+S</td><td>set to shutdown</td></tr><tr><td>Ctrl+R</td><td>set to restart</td></tr>"
       "<tr><td>Ctrl+U</td><td>set to suspend to RAM</td></tr><tr><td>Ctrl+H</td><td>set to hibernate"
       "</td></tr><tr><td>Ctrl+E</td><td>stop countdown (only if the countdown has started and the admin "
       "didn't restrict the access)</td></tr><tr><td>Shift+E</td><td>to edit the config file (for this "
       "a password is necessary. If you are a user, you can set an \"empty password\" (leave the "
       "password field empty)).</td></tr></table>"));
     QTextDocumentFragment infoDoc;
     infoDoc = QTextDocumentFragment::fromHtml(infoStr);
     QString info = infoDoc.toPlainText();

     //Get the arguments passed from the terminal
     QStringList args = QApplication::arguments();
     args.removeFirst(); //this is just the program
     for(int i=1; i<args.size();args.removeLast());
     if(!args.empty()){
       QString arg = args[0];
       if(arg[0] == '-'){
         if(arg.contains("h") || args.contains("--help"))
           myOutput << "\nqshutdown [ options ... ]\n\nOptions are:\n\t-h "
                    << "or --help\tPrints this message.\n\t-i\t\tPrints "
                    << "information.\n\t-v\t\tPrints all errors and warnings.\n"
                    << endl;
         if(arg.contains("i"))
           myOutput << info << endl;
       }
       if(!((arg[0] == '-') && (arg.contains("h") || args.contains("-help")
          || arg.contains("i") || arg.contains("v"))))
         myOutput << "Wrong options! Try qshutdown -h" << endl;
       if(args.first() == "-v")
         verbose = true;
     }
     if(!args.empty() && !(args.first() == "-v"))
       exit(0);

     Gui window; //Create the main widget

     #ifndef Q_OS_WIN32
       QDBusConnection::sessionBus().registerObject(OBJECT_NAME, &window,
         QDBusConnection::ExportScriptableSlots);
       if(QDBusConnection::sessionBus().registerService(SERVICE_NAME)){
         //if possible register qshutdown as a service in the session bus
     #endif //Q_OS_WIN32
         window.information = infoDoc.toHtml();
         window.loadSettings();
         window.center();
         window.show();
         app.setQuitOnLastWindowClosed(false);

         return app.exec();

     #ifndef Q_OS_WIN32
       }
       else{ //if registering qshutdown fails (also because it is already
             // registered, show window
         myOutput << "\nqshutdown is already running!\n" << endl;
         QDBusInterface iface(SERVICE_NAME, OBJECT_NAME, "org.qshutdown.iface",
           QDBusConnection::sessionBus(), &window);
         QDBusMessage response = iface.call("showRunningProgram");
         if(response.type() == QDBusMessage::ErrorMessage){
           if(verbose)
             myOutput << "QDBusInterface " << iface.interface() << " seems to be valid... -> "
                      << (iface.isValid()? "true":"false") << "\nW: " << response.errorName()
                      << ": " << response.errorMessage() << "\n\nYou can ignore this." << endl;
           QDBusInterface iface2(SERVICE_NAME, OBJECT_NAME, "local.Gui",
             QDBusConnection::sessionBus(), &window);
           QDBusMessage response2 = iface2.call("showRunningProgram");
           if(response2.type() == QDBusMessage::ErrorMessage)
             myOutput << "QDBusInterface " << iface2.interface() << " seems to be valid... -> "
                      << (iface2.isValid()? "true":"false") << "\nW: " << response2.errorName()
                      << ": " << response2.errorMessage() << "\n\nPlease report this." << endl;
         }
       }
     #endif //Q_OS_WIN32
}