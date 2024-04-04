#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QJsonObject>
#include <configdialog.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void updateGraphicsScene(const ConfigDialog &);

private:
    QGraphicsView *view; // 添加一个QGraphicsView成员变量

    // 创建场景和视图
    QGraphicsScene *scene = new QGraphicsScene(this);

    int gridSize;
    QString imagePath;

    //存储配置文件
    QJsonObject configData = [](){
        QJsonObject defaultState;
        defaultState["ConfigNotSaved"] = true;
        return defaultState;
    }();

    static void openHelpDocument();

    // 保存配置，返回是否保存成功，参数为是否另存为
    bool saveConfig(bool as);
    // 显示是否保存对话框，如果点击取消返回0,否则1
    bool showSaveDialog();

    QString savedConfigPath = "";
    bool configSaved = true;

private slots:
    void openConfigDialog(); // 声明槽函数
};

class HelpDialog : public QDialog {
public:
    HelpDialog(const QString &text, QWidget *parent = nullptr) : QDialog(parent) {
        QTextBrowser *textBrowser = new QTextBrowser(this);
        textBrowser->setReadOnly(true);
        textBrowser->setHtml(text); // 使用HTML格式的字符串设置帮助文本，以便可以格式化文本

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(textBrowser);

        setLayout(layout);
        setWindowTitle(tr("使用手册")); // 设置对话框标题
        resize(600, 400); // 设置对话框大小
    }
};

#endif // MAINWINDOW_H
