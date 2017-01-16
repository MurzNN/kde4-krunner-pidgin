# kde4-krunner-pidgin
Pidgin plugin for KDE4 Krunner to quick search contacts in buddy list.

For KDE5 Plasma2 version of Pidgin plugin for KRunner - look here: https://github.com/freexploit/plasma-runner-pidgin

Runner PidginRunner
----------------------
License: GPL
Author: Todd Kennedy <toddkenn@gmail.com>
Current Version: 0.3

Description: Pidgin Runner is a KDE Runner that allows you to lookup a Pidgin buddy and send them an IM. You can also set your Pidgin status. Press ALT-F2 To activate and simply type the Pidgin buddy's name in KRunner to look them up and then select the name of the buddy to activate the chat window. You can also set you pidgin status by typing setstatus <status> where status is one of five available: online, offline, away, busy, or invisible.

This code is based on the work done by Dan Vratil <vratil@progdansoft.com> on the kopete contacts runner project.

You need KDE 4.2 or higher

-- Build instructions --

  tar -xvf source.tar.gz
  cd runner-pidgin-0.1
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=/usr ..
  make
  #as root:
  make install

You might need to run kbuildsycoca4
in order to get the .desktop file recognized.

Restart Plasma to load the runner. The best way to do that is to just logout and log back in.

Change Log
---------------------------
0.1 Initial Version

0.2 Enabled search on buddyName or buddyAlias

0.3 Show additional contact info and account information in search results
