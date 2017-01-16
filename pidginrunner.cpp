#include "pidginrunner.h"
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <KDebug>
#include <KIcon>

PidginRunner::PidginRunner(QObject *parent, const QVariantList& args)
    : Plasma::AbstractRunner(parent, args)
{
    Q_UNUSED(args);
    setObjectName("PidginRunner");
    
    c_icon = KIcon("pidgin");
    setIgnoredTypes(Plasma::RunnerContext::Directory | 
		    Plasma::RunnerContext::File |
                    Plasma::RunnerContext::NetworkLocation |
		    Plasma::RunnerContext::Executable |
		    Plasma::RunnerContext::ShellCommand);
    
    Plasma::RunnerSyntax syntax = Plasma::RunnerSyntax(":q:", i18n("Open pidgin im window to buddy."));
    syntax.addExampleQuery("pidgin :q:");
    addSyntax(syntax);
    syntax = Plasma::RunnerSyntax("<status>", i18n("Set Pidgin status"));
    syntax.addExampleQuery("setstatus <status>");
    addSyntax(syntax);
    
    setSpeed(AbstractRunner::SlowSpeed);
}

PidginRunner::~PidginRunner()
{
}


void PidginRunner::match(Plasma::RunnerContext &context)
{
   
  //don't be case-sensitive
    QString searchedName = context.query().toLower();
    
    /* Is set true if "pidgin contactname" syntax is used. Then searched terms of any length are 
       accepted. If false, only terms longer then 3 are accepted */
    bool searchAll = false;
    // By default we search for contacts
    bool searchStatus = false;
    
    // if syntax "pidgin contactname" is used, remove the "pidgin" from the beginning of the string
    if (searchedName.startsWith("pidgin ")) {
        searchAll = true;
        searchedName.remove("pidgin ", Qt::CaseInsensitive);
    }
    if (searchedName.startsWith("setstatus ")) {
	searchAll = true;
	searchedName.remove("setstatus ", Qt::CaseInsensitive);
	searchStatus = true;
    }
    
    // search only if term is longer then 3 chars
    if (not searchAll) {
	if (searchedName.size() < 3) {
	    return;
	}
    } else {
	if (searchedName.isEmpty()) {
	    return;
	}
    }
    
    

 // Test if we can open DBus connection to Pidgin
    QDBusInterface pidginDBusTest("im.pidgin.purple.PurpleService", "/im/pidgin/purple/PurpleObject", "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString>pidginReply = pidginDBusTest.call("Introspect");
    if (!pidginReply.isValid()) {
            return;
    }
    
    QList<Plasma::QueryMatch> matches;
    
    if (searchStatus == false) {
	// get list of all contact's IDs
	QDBusInterface pidginDBus("im.pidgin.purple.PurpleService", "/im/pidgin/purple/PurpleObject", "im.pidgin.purple.PurpleInterface");
	QDBusReply<QList<int> >pidginContacts = pidginDBus.call("PurpleBlistGetBuddies");
	
	//find all contacts that matches
	for (int i = 0; i < pidginContacts.value().size(); i++) {
	    /*Check if the current context is still valid, if not quit (another
	      context is already running) */
	    if (!context.isValid()) {
		return;
	    }
	    
	    //Buddy Id's
	    qint32 buddyId = pidginContacts.value().at(i);
	    
	    //Get Buddy Name
	    QDBusReply<QString>contactName = pidginDBus.call("PurpleBuddyGetName",buddyId);
	    //Get Buddy Alias
	    QDBusReply<QString>contactNameAlias = pidginDBus.call("PurpleBuddyGetAlias",buddyId);
	    
	    
	    //set a flag if we found a match!
	    bool matchedName = contactName.value().contains(searchedName, Qt::CaseInsensitive);
	    bool matchedAlias = contactNameAlias.value().contains(searchedName, Qt::CaseInsensitive);
	    bool matched = matchedName || matchedAlias;
	      
	    
	    // Yes it does!
	    if (matched) {
		Plasma::QueryMatch matchItem(this);

		matchItem.setIcon(c_icon);
		
		if (matchedName) {
		matchItem.setText(contactName);
		} else {
		  matchItem.setText(contactNameAlias);
		}
		
		//Status message
		// 1 = Online 0 = Offline
		QDBusReply<qint32>statusId = pidginDBus.call("PurpleBuddyIsOnline",buddyId);
		
		QString statusMessage = "Offline";
		if (statusId.value() == 1) {
		    statusMessage = "Online";
		}
		
		matchItem.setSubtext(statusMessage);
		
		//Store contact's ID
		QString buddyIdString = QString::number(buddyId);
		matchItem.setData(QString("buddy ").append(buddyIdString));
		
		if (contactName.value() == searchedName) {
		    matchItem.setType(Plasma::QueryMatch::ExactMatch);
		} else {
		    matchItem.setType(Plasma::QueryMatch::PossibleMatch);
		}
		matches.append(matchItem);
	    }
	}
    } else { //searchStatus==true
	if (QString("online").contains(searchedName,Qt::CaseInsensitive)) {
	    Plasma::QueryMatch match(this);
	    match.setIcon(KIcon("user-online"));
	    match.setText("Go online");
	    match.setSubtext("Set status 'online' for all Pidgin accounts");
	    match.setData("status online");
	    if (searchedName == "online") {
		match.setType(Plasma::QueryMatch::ExactMatch);
	    } else {
		match.setType(Plasma::QueryMatch::PossibleMatch);
	    }
	    matches.append(match);
	    
	}
	if (QString("offline").contains(searchedName, Qt::CaseInsensitive)) {
	    Plasma::QueryMatch match(this);
	    match.setIcon(KIcon("user-offline"));
	    match.setText("Go offline");
	    match.setSubtext("Set status 'offline' for all Pidgin accounts");
	    match.setData("status offline");
	    if (searchedName == "offline") {
		match.setType(Plasma::QueryMatch::ExactMatch);
	    } else {
		match.setType(Plasma::QueryMatch::PossibleMatch);
	    }
	    matches.append(match);
	    
	}
	if (QString("away").contains(searchedName, Qt::CaseInsensitive)) {
	    Plasma::QueryMatch match(this);
	    match.setIcon(KIcon("user-away"));
	    match.setText("Go away");
	    match.setSubtext("Set status 'away' for all Pidgin accounts");
	    match.setData("status away");
	    if (searchedName == "away") {
		match.setType(Plasma::QueryMatch::ExactMatch);
	    } else {
		match.setType(Plasma::QueryMatch::PossibleMatch);
	    }
	    matches.append(match);
	    
	}
	if (QString("busy").contains(searchedName, Qt::CaseInsensitive)) {
	    Plasma::QueryMatch match(this);
	    match.setIcon(KIcon("user-busy"));
	    match.setText("Go busy");
	    match.setSubtext("Set status 'busy' for all Pidgin accounts");
	    match.setData("status busy");
	    if (searchedName == "busy") {
		match.setType(Plasma::QueryMatch::ExactMatch);
	    } else {
		match.setType(Plasma::QueryMatch::PossibleMatch);
	    }
	    matches.append(match);	
	    
	}
	if (QString("invisible").contains(searchedName, Qt::CaseInsensitive)) {
	    Plasma::QueryMatch match(this);
	    match.setIcon(KIcon("user-invisible"));
	    match.setText("Go invisible");
	    match.setSubtext("Set status 'invisible' for all Pidgin accounts");
	    match.setData("status invisible");
	    if (searchedName == "invisible") {
		match.setType(Plasma::QueryMatch::ExactMatch);
	    } else {
		match.setType(Plasma::QueryMatch::PossibleMatch);
	    }
	    matches.append(match);
	}
    }     //end of if(searchStatus==false)
		
    context.addMatches(searchedName,matches);
}

void PidginRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)
    
    QString command = match.data().toString();
    
    // Test if we can open DBus connection to Pidgin
    QDBusInterface pidginDBusTest("im.pidgin.purple.PurpleService", "/im/pidgin/purple/PurpleObject", "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString>pidginReply = pidginDBusTest.call("Introspect");
    if (!pidginReply.isValid()) {
            return;
    }
    QDBusInterface pidginDBus("im.pidgin.purple.PurpleService", "/im/pidgin/purple/PurpleObject", "im.pidgin.purple.PurpleInterface");
    
    if (command.startsWith("buddy ")) {
	command.remove("buddy ");
	
	qint32 contactId = command.toInt();
	
	QDBusReply<qint32>acctId = pidginDBus.call("PurpleBuddyGetAccount", contactId);
	QDBusReply<QString>contactName = pidginDBus.call("PurpleBuddyGetName", contactId);
	
	QDBusReply<qint32>pidginReply = pidginDBus.call("PurpleConversationNew", 1, acctId.value(), contactName.value());
	if (!pidginReply.isValid()) {
	  return;
	} 
    } 
    else if (command.startsWith(("status "))) {
	  command.remove("status ");
	  
	  //get a list of status Id's
	  QDBusReply<QList<qint32> >pidginStatusList = pidginDBus.call("PurpleSavedstatusesGetAll");
	  
	  QHash<int, int> statusItems;
	  qint32 pidginStatusId;
	  
	  //get all the status title's for Id's and store them in an array
	  for (int i = 0; i < pidginStatusList.value().size(); i++) {
	    
	    QDBusReply<qint32>pidginStatusType = pidginDBus.call("PurpleSavedstatusGetType", pidginStatusList.value().at(i));
	    
	    //associate the status type to it's Id
	    statusItems.insert(pidginStatusType,pidginStatusList.value().at(i));
	  }
	  
	  //STATUS_OFFLINE = 1 STATUS_AVAILABLE = 2 STATUS_UNAVAILABLE = 3 STATUS_INVISIBLE = 4 STATUS_AWAY = 5
	    if (command.startsWith("online")) {
	      if (statusItems.contains(2)) {
		pidginStatusId = statusItems.value(2);
	      }
	    }
	    if (command.startsWith("offline")) {
	      if (statusItems.contains(1)) {
		pidginStatusId = statusItems.value(1);
	      }
	      
	    }if (command.startsWith("away")) {
	      if (statusItems.contains(5)) {
		pidginStatusId = statusItems.value(5);
	      }
	      
	    }if (command.startsWith("busy")) {
	      if (statusItems.contains(3)) {
		pidginStatusId = statusItems.value(3);
	      }
	      
	    }if (command.startsWith("invisible")) {
	      if (statusItems.contains(4)) {
		pidginStatusId = statusItems.value(4);
	      }
	      
	    }
	  
	  //set the status in Pidgin
	  QDBusReply<qint32>pidginReply = pidginDBus.call("PurpleSavedstatusActivate", pidginStatusId);
	}
    }
#include "pidginrunner.moc"
