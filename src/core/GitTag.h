#ifndef GITTAG_H
#define GITTAG_H

#include "GitObject.h"

class GitTag : public GitObject
{
public:
    GitTag();
    QString type() const override;
};

#endif // GITTAG_H
