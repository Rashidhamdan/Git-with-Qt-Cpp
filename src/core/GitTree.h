#ifndef GITTREE_H
#define GITTREE_H

#include "GitObject.h"

class GitTree : public GitObject
{
public:
    GitTree();
    QString type() const override;
};

#endif // GITTREE_H
