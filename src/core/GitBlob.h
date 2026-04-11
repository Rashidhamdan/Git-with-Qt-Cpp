#ifndef GITBLOB_H
#define GITBLOB_H

#include "GitObject.h"

class GitBlob : public GitObject
{
public:
    GitBlob();
    QString type() const override;
};

#endif // GITBLOB_H
