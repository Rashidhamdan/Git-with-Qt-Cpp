#include "GitRepository.h"

GitRepository::GitRepository(const QString& path)
    : repoPath(path)
{
}

QString GitRepository::getPath() const
{
    return repoPath;
}
