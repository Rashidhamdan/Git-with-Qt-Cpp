#ifndef GITOBJECT_H
#define GITOBJECT_H

#include <QString>

class GitObject
{
public:
    virtual ~GitObject() = default;
    virtual QString type() const = 0;
};

#endif // GITOBJECT_H
