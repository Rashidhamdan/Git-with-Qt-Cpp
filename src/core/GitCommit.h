#ifndef GITCOMMIT_H
#define GITCOMMIT_H

#include "GitObject.h"

class GitCommit : public GitObject
{
public:
    GitCommit();
    QString type() const override;
};

#endif // GITCOMMIT_H
