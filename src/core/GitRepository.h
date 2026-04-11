#ifndef GITREPOSITORY_H
#define GITREPOSITORY_H

#include <QString>

class GitRepository
{
public:
    GitRepository(const QString& path);
    QString getPath() const;

private:
    QString repoPath;
};

#endif // GITREPOSITORY_H
