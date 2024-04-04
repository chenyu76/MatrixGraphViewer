#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H


//#include "qscrollarea.h"
#include <QDialog>
#include <QScrollArea>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

//#include <map>
#include <vector>


class VariableWidget : public QWidget {
    Q_OBJECT
public:
    VariableWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        QComboBox *comboBox = new QComboBox();
        comboBox->addItems(QStringList() << "定值" << "列" << "行"); // << "滑动条");
        QLineEdit *nameEdit = new QLineEdit();
        QLineEdit *rangeEdit = new QLineEdit();
        QPushButton *deleteButton = new QPushButton("删除");

        // 设置占位符文本
        nameEdit->setPlaceholderText("变量名");
        nameEdit->setObjectName("nameEdit");
        nameEdit->setMaximumWidth(80);
        rangeEdit->setPlaceholderText("值/范围");
        rangeEdit->setObjectName("rangeEdit");

        layout->addWidget(nameEdit);
        layout->addWidget(comboBox);
        layout->addWidget(rangeEdit);
        layout->addWidget(deleteButton);

        connect(deleteButton, &QPushButton::clicked, this, &VariableWidget::onDeleteClicked);
    }

signals:
    void deleteClicked(VariableWidget *widget);

private slots:
    void onDeleteClicked() {
        emit deleteClicked(this);
    }
};



struct VariableConfig {
    int type;       // 变量类型
    QStringList range; // 变量范围
};




class ConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr,
                const QJsonObject &configData = [](){
                        QJsonObject defaultState;
                        defaultState["ConfigNotSaved"] = true;
                        return defaultState;}()
                );
    virtual ~ConfigDialog();

    // 保存配置
    QJsonObject saveState() const;
    // 恢复配置
    void restoreState(const QJsonObject &state);
    // 保存配置到文件
    static bool saveConfigToFile(const QString &filePath, const QJsonObject &state);
    // 从文件加载配置，参数是文件路径和存到的地方
    static bool loadConfigFromFile(const QString &filePath, QJsonObject &state);


    std::map<QString, VariableConfig> getVariableConfigs() const;

    QString getImageName() const { return pictureName->text(); }
    QString getImagePath() const { return folderPathEdit->text(); }
    QString getXAxisLabel() const { return xAxisLabel->text(); }
    QString getYAxisLabel() const { return yAxisLabel->text(); }
    QString getSecXAxisLabel() const { return secXAxisLabel->text(); }
    QString getSecYAxisLabel() const { return secYAxisLabel->text(); }

    // 获得一个QString代表的格式，first是字符串，second表示这个字符串是否是变量，是变量的话会在之后的环节替换为值
    // 变量以 `' 括起来
    static std::vector<std::pair<QString, int>> getStrVarConfig(const QString&);

private slots:
    void selectImagePath() {
        QString directory = QFileDialog::getExistingDirectory(this, tr("选择图片文件夹"), QDir::homePath());
        if (!directory.isEmpty()) {
            folderPathEdit->setText(directory);
        }
    }

    void onAddVariableClicked() {
        VariableWidget *variable = new VariableWidget();
        connect(variable, &VariableWidget::deleteClicked, this, &ConfigDialog::onDeleteVariableClicked);
        variablesLayout->addWidget(variable);
        variableWidgets.append(variable);
    }

    void onDeleteVariableClicked(VariableWidget *widget) {
        variableWidgets.removeAll(widget);
        variablesLayout->removeWidget(widget);
        widget->deleteLater();
    }

private:
    QLineEdit *gridSizeEdit;
    QLineEdit *folderPathEdit;
    QLineEdit *pictureName;

    QLineEdit *xAxisLabel;
    QLineEdit *yAxisLabel;

    QLineEdit *secXAxisLabel;
    QLineEdit *secYAxisLabel;

    QVBoxLayout *variablesLayout;
    QList<VariableWidget *> variableWidgets;
};


#endif // CONFIGDIALOG_H
