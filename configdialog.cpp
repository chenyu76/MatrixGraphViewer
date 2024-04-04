#include "configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent, const QJsonObject &status) : QDialog(parent) {

    this->setWindowTitle(tr("配置")); // 设置窗口标题
    this->resize(600, 400);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 文件夹选择部分
    QHBoxLayout *folderLayout = new QHBoxLayout();
    folderPathEdit = new QLineEdit();
    QPushButton *selectFolderButton = new QPushButton(tr("选择"));
    folderLayout->addWidget(new QLabel(tr("文件夹路径:")));
    folderLayout->addWidget(folderPathEdit);
    folderLayout->addWidget(selectFolderButton);

    // 图片名称规范
    QHBoxLayout *pictureLayout = new QHBoxLayout();
    pictureName = new QLineEdit();
    pictureLayout->addWidget(new QLabel(tr("图片名称格式：")));
    pictureLayout->addWidget(pictureName);

    // 横纵坐标名称规范
    QHBoxLayout *AxisLayout = new QHBoxLayout();
    xAxisLabel = new QLineEdit();
    yAxisLabel = new QLineEdit();
    AxisLayout->addWidget(new QLabel(tr("列标签")));
    AxisLayout->addWidget(xAxisLabel);
    AxisLayout->addWidget(new QLabel(tr("行标签")));
    AxisLayout->addWidget(yAxisLabel);

    QHBoxLayout *SecAxisLayout = new QHBoxLayout();
    secXAxisLabel = new QLineEdit();
    secYAxisLabel = new QLineEdit();
    SecAxisLayout->addWidget(new QLabel(tr("次级列标签")));
    SecAxisLayout->addWidget(secXAxisLabel);
    SecAxisLayout->addWidget(new QLabel(tr("次级行标签")));
    SecAxisLayout->addWidget(secYAxisLabel);

    // 添加变量部分
    QPushButton *addVariableButton = new QPushButton(tr("添加变量"));
    variablesLayout = new QVBoxLayout();
    QScrollArea *scrollArea = new QScrollArea();
    QWidget *container = new QWidget();
    container->setLayout(variablesLayout);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);

    mainLayout->addLayout(folderLayout);
    mainLayout->addLayout(pictureLayout);
    mainLayout->addLayout(AxisLayout);
    mainLayout->addLayout(SecAxisLayout);
    mainLayout->addWidget(addVariableButton);
    mainLayout->addWidget(scrollArea);

    //取消与应用按钮
    QHBoxLayout *acceptedLayout = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("取消"));
    acceptedLayout->addWidget(cancelButton);
    QPushButton *applyButton = new QPushButton(tr("应用"));
    acceptedLayout->addWidget(applyButton);
    mainLayout->addLayout(acceptedLayout);

    //连接信号
    connect(selectFolderButton, &QPushButton::clicked, this, [this]() {
        QString folder = QFileDialog::getExistingDirectory(this, tr("选择图片文件夹"));
        if (!folder.isEmpty()) {
            this->folderPathEdit->setText(folder);
        }
    });
    connect(addVariableButton, &QPushButton::clicked, this, &ConfigDialog::onAddVariableClicked);
    connect(applyButton, &QPushButton::clicked, this, &ConfigDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &ConfigDialog::close);

    if(!status.contains("ConfigNotSaved"))
        restoreState(status);
}

ConfigDialog::~ConfigDialog() {

}

std::map<QString, VariableConfig> ConfigDialog::getVariableConfigs() const {
    std::map<QString, VariableConfig> configs;
    for (auto *widget : variableWidgets) {
        VariableConfig config;
        QComboBox *comboBox = widget->findChild<QComboBox*>(); // 假设你有方式访问或存储这些控件的引用
        QLineEdit *nameEdit = widget->findChild<QLineEdit*>("nameEdit"); // 你可能需要设置对象名
        QLineEdit *rangeEdit = widget->findChild<QLineEdit*>("rangeEdit");

        if (comboBox && nameEdit && rangeEdit) {
            // 将用户输入的范围变为一个QStringList
            // 处理 xx-xx 形式，有一个 - 就认为是这个格式
            QString range = rangeEdit->text();
            QStringList ranges;
            if(range.contains("~")) {
                ranges = range.split("~");
                int max = ranges[1].toInt();
                int min = ranges[0].toInt();
                if (max < min)
                    std::swap(max,min);
                ranges.resize(max - min + 1);
                for(int i = 0; i < max - min + 1; i++)
                    ranges[i] = QString::number(i+min);
            }
            else if(range.contains(",")) {
                // 处理 xx,xx,xx, ...形式（有一个,就认为是这个格式
                ranges = range.split(",");
            }
            else {
                // 处理 xx 形式，不属于上面的格式就认为是这个格式（真随意
                ranges.append(range);
            }
            configs[nameEdit->text()] = {comboBox->currentIndex(), ranges};
        }
    }
    return configs;
}

std::vector<std::pair<QString, int>> ConfigDialog::getStrVarConfig(const QString& name){
    std::vector<std::pair<QString, int>> nameCfg;
    int is_var = 0;
    int is_var_pre = 0;
    int i = -1;
    for(int e = 0; e < name.length(); e++) {
        if(name[e] == QChar('`'))
            is_var = 1;
        if(name[e] == QChar('\''))
            is_var = 0;

        // 进入变量或字符串范围
        if( is_var != is_var_pre ) {
            nameCfg.push_back(std::pair(name.mid(i+1,e - i - 1), is_var_pre));
            i = e;
        }
        is_var_pre = is_var;
    }
    // 处理末尾字符
    nameCfg.push_back(std::pair(name.right(name.length() - i - 1), is_var_pre));

    return nameCfg;
}


QJsonObject ConfigDialog::saveState() const {
    QJsonObject state;
    state["folderPath"] = folderPathEdit->text();
    state["pictureName"] = pictureName->text();
    state["xAxisLabel"] = xAxisLabel->text();
    state["yAxisLabel"] = yAxisLabel->text();
    state["secXAxisLabel"] = secXAxisLabel->text();
    state["secYAxisLabel"] = secYAxisLabel->text();

    QJsonArray variablesArray;
    for (auto *widget : variableWidgets) {
        QJsonObject variableObject;
        QComboBox *comboBox = widget->findChild<QComboBox*>();
        QLineEdit *nameEdit = widget->findChild<QLineEdit*>("nameEdit");
        QLineEdit *rangeEdit = widget->findChild<QLineEdit*>("rangeEdit");
        if (comboBox && nameEdit && rangeEdit) {
            variableObject["type"] = comboBox->currentIndex();
            variableObject["name"] = nameEdit->text();
            variableObject["range"] = rangeEdit->text();
            variablesArray.append(variableObject);
        }
    }
    state["variables"] = variablesArray;
    return state;
}

void ConfigDialog::restoreState(const QJsonObject &state) {
    folderPathEdit->setText(state["folderPath"].toString());
    pictureName->setText(state["pictureName"].toString());
    xAxisLabel->setText(state["xAxisLabel"].toString());
    yAxisLabel->setText(state["yAxisLabel"].toString());
    secXAxisLabel->setText(state["secXAxisLabel"].toString());
    secYAxisLabel->setText(state["secYAxisLabel"].toString());

    QJsonArray variablesArray = state["variables"].toArray();
    for (auto variableValue : variablesArray) {
        QJsonObject variableObject = variableValue.toObject();
        VariableWidget *variable = new VariableWidget();
        QComboBox *comboBox = variable->findChild<QComboBox*>();
        QLineEdit *nameEdit = variable->findChild<QLineEdit*>("nameEdit");
        QLineEdit *rangeEdit = variable->findChild<QLineEdit*>("rangeEdit");
        if (comboBox && nameEdit && rangeEdit) {
            comboBox->setCurrentIndex(variableObject["type"].toInt());
            nameEdit->setText(variableObject["name"].toString());
            rangeEdit->setText(variableObject["range"].toString());
            variableWidgets.append(variable);
            variablesLayout->addWidget(variable);
        }
    }
}

// 保存配置到文件
bool ConfigDialog::saveConfigToFile(const QString &filePath, const QJsonObject &state) {
    QFile saveFile(filePath);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument saveDoc(state);
    saveFile.write(saveDoc.toJson());
    return true;
}

// 从文件加载配置
bool ConfigDialog::loadConfigFromFile(const QString &filePath, QJsonObject &state) {
    QFile loadFile(filePath);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    state = loadDoc.object();
    return true;
}
