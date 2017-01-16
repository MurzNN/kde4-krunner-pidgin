
#ifndef PIDGINRUNNER_H
#define PIDGINRUNNER_H

#include <plasma/abstractrunner.h>
#include <KIcon>
#include <QMetaType>
#include <QVariant>
#include <QHash>

Q_DECLARE_METATYPE(QList<int>)

// Define our plasma Runner
class PidginRunner : public Plasma::AbstractRunner {
    Q_OBJECT

public:
    // Basic Create/Destroy
    PidginRunner( QObject *parent, const QVariantList& args );
    ~PidginRunner();

    void match(Plasma::RunnerContext &context);
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match);
    
private:
    KIcon c_icon;
};
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_RUNNER(pidginrunner, PidginRunner)

#endif
