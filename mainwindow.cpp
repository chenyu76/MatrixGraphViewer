#include "mainwindow.h"
#include "configdialog.h"
#include "mousecontrolgraphview.h"
#include "qapplication.h"
#include <QAction>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>

#include <map>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  // 创建菜单栏
  QMenuBar *menuBar = new QMenuBar();
  setMenuBar(menuBar);

  // 创建“文件”菜单
  QMenu *fileMenu = menuBar->addMenu(tr("&文件"));
  // 创建配置
  QAction *configAction = fileMenu->addAction(tr("配置"));
  connect(configAction, &QAction::triggered, this,
          &MainWindow::openConfigDialog);

  fileMenu->addSeparator();

  QAction *newAction = fileMenu->addAction(tr("新建"));
  connect(newAction, &QAction::triggered, this, [&]() {
    // 需要实现先询问是否保存当前文件，再打开配置
    /*
     * ...
     */
    if(!configSaved)
        if(!showSaveDialog())
            return;

    savedConfigPath = "";
    this->configData = []() {
      QJsonObject defaultState;
      defaultState["ConfigNotSaved"] = true;
      return defaultState;
    }();
    MainWindow::openConfigDialog();
  });

  QAction *openAction = fileMenu->addAction(tr("打开"));
  connect(openAction, &QAction::triggered, this, [&]() {
    QString filter = "JSON Files (*.json) ;; All Files (*.*)";
    QString loadFP = QFileDialog::getOpenFileName(this, "Select a file", "./", filter);
    if(ConfigDialog::loadConfigFromFile(loadFP, configData)){
        // 打开后需要 updateGraphicsScene 才符合直觉
        /*
         * ...
         */
        // 简单处理为打开配置
        savedConfigPath = loadFP;
        MainWindow::openConfigDialog();
    }
    configSaved = false;
  });

  QAction *saveAction = fileMenu->addAction(tr("保存"));
  connect(saveAction, &QAction::triggered, this, &MainWindow::saveConfig);

  QAction *saveAsAction = fileMenu->addAction(tr("另存为"));
  connect(saveAsAction, &QAction::triggered, this, [&](){MainWindow::saveConfig(true);});

  fileMenu->addSeparator();

  // 创建"导出"子菜单
  QMenu *exportMenu = fileMenu->addMenu(tr("导出"));
  // 在"导出"子菜单中添加"导出为LaTeX"操作
  QAction *exportLatexAction = exportMenu->addAction(tr("导出为LaTeX"));
  // connect(exportLatexAction, &QAction::triggered, this,
  // &YourClassName::exportAsLaTeX);
  connect(exportLatexAction, &QAction::triggered, this, [this]() {
    QMessageBox::warning(this, tr("还没做好"), tr("前面的区域，以后再来探索吧"));
  });

  fileMenu->addSeparator();
  // 添加“退出”动作到“文件”菜单
  QAction *exitAction = fileMenu->addAction(tr("退出"));
  connect(exitAction, &QAction::triggered, this, &QApplication::quit);

  // 创建“关于”菜单
  QMenu *helpMenu = menuBar->addMenu(tr("&帮助"));

  // 添加“帮助”动作到“关于”菜单
  QAction *helpAction = helpMenu->addAction(tr("如何使用"));
  connect(helpAction, &QAction::triggered, this, MainWindow::openHelpDocument);

  // 添加“关于”动作到“关于”菜单
  QAction *aboutAction = helpMenu->addAction(tr("关于本程序"));
  connect(aboutAction, &QAction::triggered, this, [this]() {
    QMessageBox::about(this, tr("About"),
                       tr("Matrix Graph Viewer By Chen Yu"));
  });

    // 初始提示字符串
    QGraphicsTextItem *textItem = new QGraphicsTextItem(tr("点击“配置”以开始"));
    QFont font = textItem->font();
    font.setPixelSize(30); // 设置文本大小
    textItem->setFont(font);
    textItem->setPos(0,0);
    scene->addItem(textItem);

  view = new MouseControlGraphView(scene);

  // 设置视图支持缩放
  view->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
  view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  view->setDragMode(QGraphicsView::ScrollHandDrag);
  // 设置尺寸
  view->setMinimumSize(800, 600);

  // 应用缩放因子
  double scaleFactor = 1.5; // 示例缩放因子
  view->scale(scaleFactor, scaleFactor);

  // 将view设置为MainWindow的中心部件
  this->setCentralWidget(view);


  // 创建按钮
  QPushButton *button = new QPushButton("配置", view);
  button->setIcon(QIcon(":/icon/config.svg"));
  connect(button, &QPushButton::clicked, this, &MainWindow::openConfigDialog);

}

void MainWindow::openConfigDialog() {
  ConfigDialog dialog(this, configData);
  if (dialog.exec() == QDialog::Accepted) {
    // 保存填的东西
    configData = dialog.saveState();

    updateGraphicsScene(dialog);
  }
}

void MainWindow::updateGraphicsScene(const ConfigDialog & dialog) {

    QString folderPath = dialog.getImagePath();
    std::vector<std::pair<QString, int>> imageNameConfig =
        ConfigDialog::getStrVarConfig(dialog.getImageName());
    std::vector<std::vector<std::pair<QString, int>>> xNameConfig = {
        ConfigDialog::getStrVarConfig(dialog.getXAxisLabel()),
        ConfigDialog::getStrVarConfig(dialog.getSecXAxisLabel())
    };
    std::vector<std::vector<std::pair<QString, int>>> yNameConfig = {
        ConfigDialog::getStrVarConfig(dialog.getYAxisLabel()),
        ConfigDialog::getStrVarConfig(dialog.getSecYAxisLabel())
    };

    std::map<QString, VariableConfig> varcfg = dialog.getVariableConfigs();

    QString xVar = "xDefault114514";
    QString yVar = "yDefault1919810";
    QStringList fixedVar;
    // QStringList sliderVar;

    // 获取各类变量的类型，放到不同变量中处理
    for (auto &i : varcfg) {
        switch (i.second.type) {
        case 1:
            xVar = i.first;
            break;
        case 2:
            yVar = i.first;
            break;
        case 0:
            fixedVar.append(i.first);
            break;
            // case 3:
            //     sliderVar.append(i.first);
        }
    }
    // 没有横纵坐标的默认行为
    if(xVar == "xDefault114514")
        varcfg[xVar] = {0, QStringList("好臭的变量名，不允许使用")};
    if(yVar == "yDefault1919810")
        varcfg[yVar] = {0, QStringList("42")};

    // 获得图片路径矩阵 以及x轴坐标和y轴坐标
    int xLen = varcfg[xVar].range.length();
    int yLen = varcfg[yVar].range.length();
    std::vector<std::vector<QString>> nameMatrix(yLen,
                                                 std::vector<QString>(xLen));

    //注意这两层和nameMatrix不同，这里是第一层是上方的标签和下方的标签，第二层是标签内容
    std::vector<std::vector<QString>> xName(2, std::vector<QString>(xLen));
    std::vector<std::vector<QString>> yName(2, std::vector<QString>(yLen));

    //arg: 带参数字符串, 变量映射表, x 位置， y 位置
    //转成 QString的函数
    auto toName = [&](const std::vector<std::pair<QString, int>> &cfgs,
                      const int &i, const int &j = 0) {
        QString str = "";
        for (auto &v : cfgs)
            str.append(
                (v.second && varcfg.count(v.first))
                    ? (varcfg.at(v.first)
                           .range[v.first == xVar ? i : (v.first == yVar ? j : 0)])
                    : v.first);
        return str;
    };

    // 创建标签向量，名称矩阵
    for (int j = 0; j < yLen; j++)
        for (int i = 0; i < xLen; i++)
            nameMatrix[j][i] = toName(imageNameConfig, i, j);
    for (int index = 0; index <= 1; index ++) {
        for (int i = 0; i < xLen; i++)
            xName[index][i] = toName(xNameConfig[index], i);
        for (int j = 0; j < yLen; j++)
            yName[index][j] = toName(yNameConfig[index], 0, j);
    }


    // 添加图片，先使用文本检查是否正确
    // 清除现有场景中的所有内容
    scene->clear();

    // 计算每个文本项的大小和位置（这里可以根据需要进行调整）
    const int textSize = 100; // 假定每个文本项的基础大小
    const int imageSize = 1000;
    const qreal imageSpacing = 1.1;

    QDir dir(folderPath);
    for (int j = 0; j < yLen; ++j) {
        for (int i = 0; i < xLen; ++i) {
            // 有图片
            if (QFile::exists(dir.absoluteFilePath(nameMatrix[j][i]))) {
                QPixmap pixmap(dir.absoluteFilePath(nameMatrix[j][i]));
                QGraphicsPixmapItem *item = scene->addPixmap(
                    pixmap.scaled(imageSize, imageSize, Qt::KeepAspectRatio));
                item->setPos(i * imageSize * imageSpacing,
                             j * imageSize * imageSpacing);
            }
            // 没有图片用文本代替
            // else {
            //     QGraphicsTextItem *textItem = new QGraphicsTextItem(nameMatrix[j][i]);
            //     QFont font = textItem->font();
            //     font.setPixelSize(textSize / 2); // 设置文本大小
            //     textItem->setFont(font);

            //     // 设置文本项的位置
            //     textItem->setPos(i * imageSize * imageSpacing,
            //                      j * imageSize * imageSpacing);
            //     scene->addItem(textItem);
            // }
        }
    }

    int totH = yLen * imageSize * imageSpacing - imageSize * (imageSpacing - 1);
    int totW = xLen * imageSize * imageSpacing - imageSize * (imageSpacing - 1);
    // 添加 xy 轴标签
    for(int index = 0; index <= 1; index++) {
        for (int i = 0; i < xLen; ++i) {
            QGraphicsTextItem *textItem = new QGraphicsTextItem(xName[index][i]);
            QFont font = textItem->font();
            font.setPixelSize(textSize); // 设置文本大小
            textItem->setFont(font);
            // 计算并设置文本居中对齐（假设已知文本项和目标区域的大小）
            QRectF bounds = textItem->boundingRect();
            textItem->setPos(i * imageSize * imageSpacing + (imageSize - bounds.width()) / 2,
                             (index ? totH: -bounds.height()));
            scene->addItem(textItem);
        }
        for (int i = 0; i < yLen; ++i) {
            QGraphicsTextItem *textItem = new QGraphicsTextItem(yName[index][i]);
            QFont font = textItem->font();
            font.setPixelSize(textSize); // 设置文本大小
            textItem->setFont(font);
            // 设置文本项的位置
            textItem->setPos((index ? totW : 0), i * imageSize * imageSpacing);
            // 旋转文本项90度
            textItem->setRotation(-90);
            // 计算并设置文本居中对齐（假设已知文本项和目标区域的大小）
            QRectF bounds = textItem->boundingRect();
            textItem->setPos(textItem->x() + (index ? 0:-bounds.height()),
                             textItem->y() + (imageSize + bounds.width()) / 2);
            scene->addItem(textItem);
        }
    }

    // 可以在遥远的地方加一些看不见的东西，这样就可以拖动很远
    for(int i:{-5000, totW+5000}) {
        for(int j:{-5000, totH+5000}) {
            QGraphicsTextItem *textItem = new QGraphicsTextItem(" ");
            textItem->setPos(i,j);
            scene->addItem(textItem);
        }
    }
}

void MainWindow::openHelpDocument() {
  // 使用QTextBrowser或QTextEdit读取并显示帮助文档
  QTextBrowser *textBrowser = new QTextBrowser;
  QFile file(":/help/readme.md");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    textBrowser->setHtml(file.readAll());
    file.close();
  }

  // 设置窗口标题和大小
  textBrowser->setWindowTitle(tr("Help Document"));
  textBrowser->resize(600, 400);

  // 显示文档
  textBrowser->show();
}


bool MainWindow::saveConfig(bool as = false) {
    QString filter = "JSON Files (*.json) ;; All Files (*.*)";

    if(as || savedConfigPath.isEmpty()){
        QString s = QFileDialog::getSaveFileName(this, "Save file", "./", filter);
        if(s.isEmpty())
            return configSaved=0;
        savedConfigPath = s.endsWith(".json") ? s : s + ".json";
    }

    configSaved = ConfigDialog::saveConfigToFile(savedConfigPath, configData);
    if(!configSaved)
        QMessageBox::warning(this, tr("保存失败"), tr("请检查保存的位置是否可写入"));
    return configSaved;
}

bool MainWindow::showSaveDialog() {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("保存文件"));  // 设置对话框标题
    msgBox.setText(tr("你想保存更改吗？")); // 设置显示的文本
    msgBox.setIcon(QMessageBox::Question); // 设置对话框图标为问号

    // 添加按钮并自定义它们的角色
    QPushButton *saveButton = msgBox.addButton(tr("保存"), QMessageBox::AcceptRole);
    QPushButton *dontSaveButton = msgBox.addButton(tr("不保存"), QMessageBox::RejectRole);
    msgBox.addButton(tr("取消"), QMessageBox::DestructiveRole);

    // 显示对话框并等待用户响应
    msgBox.exec();

    // 根据用户的选择执行相应的操作
    if (msgBox.clickedButton() == saveButton) {
        while(!saveConfig());
        return true;
    } else if (msgBox.clickedButton() == dontSaveButton) {
        return true;
    }
    return false;
}
