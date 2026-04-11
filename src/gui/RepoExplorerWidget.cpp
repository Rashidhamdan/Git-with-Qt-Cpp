#include "RepoExplorerWidget.h"

#include <QAbstractItemView>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QModelIndex>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

/***************************************************************************/
/*                                                                         */
/*                         RepoExplorerWidget                              */
/*                                                                         */
/***************************************************************************/

RepoExplorerWidget::RepoExplorerWidget(QWidget *parent)
    : QWidget(parent)
{
    initializeUi();
}

/***************************************************************************/
/*                                                                         */
/*                        ~RepoExplorerWidget                              */
/*                                                                         */
/***************************************************************************/

RepoExplorerWidget::~RepoExplorerWidget()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
    updateRepositoryInfo();

    if (m_fileSystemModel == nullptr || m_treeView == nullptr) {
        return;
    }

    const QModelIndex rootIndex = m_fileSystemModel->setRootPath(m_repositoryPath);
    m_treeView->setRootIndex(rootIndex);

    if (rootIndex.isValid()) {
        m_treeView->expand(rootIndex);
        m_treeView->scrollTo(rootIndex);
        m_treeView->sortByColumn(0, Qt::AscendingOrder);
    }
}

QString RepoExplorerWidget::repositoryPath() const
{
    return m_repositoryPath;
}

void RepoExplorerWidget::refresh()
{
    if (m_repositoryPath.isEmpty() || m_fileSystemModel == nullptr || m_treeView == nullptr) {
        return;
    }

    QModelIndex rootIndex = m_fileSystemModel->setRootPath(QString());
    Q_UNUSED(rootIndex);

    rootIndex = m_fileSystemModel->setRootPath(m_repositoryPath);
    m_treeView->setRootIndex(rootIndex);

    if (rootIndex.isValid()) {
        m_treeView->expand(rootIndex);
        m_treeView->scrollTo(rootIndex);
    }

    updateRepositoryInfo();
    emit refreshRequested();
}

QString RepoExplorerWidget::selectedFilePath() const
{
    if (m_treeView == nullptr || m_fileSystemModel == nullptr) {
        return QString();
    }

    const QModelIndex currentIndex = m_treeView->currentIndex();
    if (!currentIndex.isValid()) {
        return QString();
    }

    const QString path = m_fileSystemModel->filePath(currentIndex);
    const QFileInfo info(path);

    if (!info.exists() || !info.isFile()) {
        return QString();
    }

    return path;
}

/***************************************************************************/
/*                                                                         */
/*                        UI Initialization                                */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::initializeUi()
{
    createWidgets();
    configureTreeView();
    createLayouts();
    setupConnections();
    updateRepositoryInfo();
}

/***************************************************************************/
/*                                                                         */
/*                         Create Widgets                                  */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);
    m_repositoryLabel->setWordWrap(true);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_refreshButton = new QPushButton(tr("Refresh"), this);

    m_treeView = new QTreeView(this);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setAnimated(true);
    m_treeView->setSortingEnabled(true);

    m_fileSystemModel = new QFileSystemModel(this);
    m_fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    m_fileSystemModel->setRootPath(QString());
}

/***************************************************************************/
/*                                                                         */
/*                       Configure TreeView                                */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::configureTreeView()
{
    if (m_treeView == nullptr || m_fileSystemModel == nullptr) {
        return;
    }

    m_treeView->setModel(m_fileSystemModel);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setExpandsOnDoubleClick(true);
    m_treeView->setAllColumnsShowFocus(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setUniformRowHeights(true);

    m_treeView->setColumnWidth(0, 260);
    m_treeView->setColumnWidth(1, 90);
    m_treeView->setColumnWidth(2, 120);
    m_treeView->setColumnWidth(3, 160);

    if (m_treeView->header() != nullptr) {
        m_treeView->header()->setStretchLastSection(true);
    }
}

/***************************************************************************/
/*                                                                         */
/*                         Create Layouts                                  */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::createLayouts()
{
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);
    headerLayout->addWidget(m_repositoryLabel, 1);
    headerLayout->addWidget(m_refreshButton, 0);

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);
    m_mainLayout->addLayout(headerLayout);
    m_mainLayout->addWidget(m_treeView, 1);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked, this, &RepoExplorerWidget::refresh);

    connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        if (!index.isValid() || m_fileSystemModel == nullptr) {
            return;
        }

        const QString filePath = m_fileSystemModel->filePath(index);
        const QFileInfo info(filePath);

        if (!info.exists() || !info.isFile()) {
            return;
        }

        emit fileActivated(filePath);
    });

    if (m_treeView->selectionModel() != nullptr) {
        connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged, this,
                [this](const QModelIndex& current, const QModelIndex&) {
                    if (!current.isValid() || m_fileSystemModel == nullptr) {
                        return;
                    }

                    const QString filePath = m_fileSystemModel->filePath(current);
                    const QFileInfo info(filePath);

                    if (!info.exists() || !info.isFile()) {
                        return;
                    }

                    emit selectionChanged(filePath);
                });
    }
}

/***************************************************************************/
/*                                                                         */
/*                       Update Repository Info                            */
/*                                                                         */
/***************************************************************************/

void RepoExplorerWidget::updateRepositoryInfo()
{
    if (m_repositoryLabel == nullptr) {
        return;
    }

    if (m_repositoryPath.isEmpty()) {
        m_repositoryLabel->setText(tr("Repository: -"));
        m_repositoryLabel->setToolTip(QString());
        return;
    }

    const QFileInfo info(m_repositoryPath);
    const QString repoName = info.fileName().isEmpty() ? m_repositoryPath : info.fileName();

    m_repositoryLabel->setText(tr("Repository: %1").arg(repoName));
    m_repositoryLabel->setToolTip(m_repositoryPath);
}
