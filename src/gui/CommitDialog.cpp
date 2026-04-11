#include "CommitDialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

/***************************************************************************/
/*                                                                         */
/*                            CommitDialog                                 */
/*                                                                         */
/***************************************************************************/

CommitDialog::CommitDialog(QWidget *parent)
    : QDialog(parent)
{
    initializeUi();
}

/***************************************************************************/
/*                                                                         */
/*                           ~CommitDialog                                 */
/*                                                                         */
/***************************************************************************/

CommitDialog::~CommitDialog()
{
}

/***************************************************************************/
/*                                                                         */
/*                              Public API                                 */
/*                                                                         */
/***************************************************************************/

void CommitDialog::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
    updateRepositoryInfo();
}

QString CommitDialog::repositoryPath() const
{
    return m_repositoryPath;
}

QString CommitDialog::commitMessage() const
{
    const QString summary = (m_summaryLineEdit != nullptr)
    ? m_summaryLineEdit->text().trimmed()
    : QString();

    const QString description = (m_descriptionTextEdit != nullptr)
                                    ? m_descriptionTextEdit->toPlainText().trimmed()
                                    : QString();

    if (description.isEmpty()) {
        return summary;
    }

    return summary + QStringLiteral("\n\n") + description;
}

void CommitDialog::clearForm()
{
    if (m_summaryLineEdit != nullptr) {
        m_summaryLineEdit->clear();
    }

    if (m_descriptionTextEdit != nullptr) {
        m_descriptionTextEdit->clear();
    }

    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                         UI Initialization                               */
/*                                                                         */
/***************************************************************************/

void CommitDialog::initializeUi()
{
    createWidgets();
    createLayouts();
    setupConnections();
    updateRepositoryInfo();
    updateUiState();

    setWindowTitle(tr("Create Commit"));
    setModal(true);
    resize(600, 400);
}

/***************************************************************************/
/*                                                                         */
/*                           Create Widgets                                */
/*                                                                         */
/***************************************************************************/

void CommitDialog::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_summaryLabel = new QLabel(tr("Summary"), this);
    m_descriptionLabel = new QLabel(tr("Description"), this);

    m_summaryLineEdit = new QLineEdit(this);
    m_summaryLineEdit->setPlaceholderText(tr("Enter commit summary"));

    m_descriptionTextEdit = new QTextEdit(this);
    m_descriptionTextEdit->setPlaceholderText(tr("Enter optional commit description"));

    m_commitButton = new QPushButton(tr("Commit"), this);
    m_cancelButton = new QPushButton(tr("Cancel"), this);

    m_commitButton->setDefault(true);
}

/***************************************************************************/
/*                                                                         */
/*                           Create Layouts                                */
/*                                                                         */
/***************************************************************************/

void CommitDialog::createLayouts()
{
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_commitButton);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    m_mainLayout->setSpacing(10);

    m_mainLayout->addWidget(m_repositoryLabel);
    m_mainLayout->addSpacing(6);
    m_mainLayout->addWidget(m_summaryLabel);
    m_mainLayout->addWidget(m_summaryLineEdit);
    m_mainLayout->addWidget(m_descriptionLabel);
    m_mainLayout->addWidget(m_descriptionTextEdit, 1);
    m_mainLayout->addLayout(buttonLayout);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void CommitDialog::setupConnections()
{
    connect(m_cancelButton, &QPushButton::clicked, this, &CommitDialog::reject);
    connect(m_commitButton, &QPushButton::clicked, this, &CommitDialog::accept);

    connect(m_summaryLineEdit, &QLineEdit::textChanged, this, [this]() {
        updateUiState();
    });

    connect(m_descriptionTextEdit, &QTextEdit::textChanged, this, [this]() {
        updateUiState();
    });
}

/***************************************************************************/
/*                                                                         */
/*                         Update Repository Info                          */
/*                                                                         */
/***************************************************************************/

void CommitDialog::updateRepositoryInfo()
{
    if (m_repositoryLabel == nullptr) {
        return;
    }

    if (m_repositoryPath.isEmpty()) {
        m_repositoryLabel->setText(tr("Repository: -"));
        m_repositoryLabel->setToolTip(QString());
    } else {
        m_repositoryLabel->setText(tr("Repository: %1").arg(m_repositoryPath));
        m_repositoryLabel->setToolTip(m_repositoryPath);
    }
}

/***************************************************************************/
/*                                                                         */
/*                           Update UI State                               */
/*                                                                         */
/***************************************************************************/

void CommitDialog::updateUiState()
{
    if (m_commitButton == nullptr || m_summaryLineEdit == nullptr) {
        return;
    }

    m_commitButton->setEnabled(!m_summaryLineEdit->text().trimmed().isEmpty());
}
