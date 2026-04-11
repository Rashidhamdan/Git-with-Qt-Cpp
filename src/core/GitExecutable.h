#ifndef GITEXECUTABLE_H
#define GITEXECUTABLE_H

#include <QString>

class GitExecutable
{
public:
    static QString path();
    static bool isAvailable();
};

#endif // GITEXECUTABLE_H
