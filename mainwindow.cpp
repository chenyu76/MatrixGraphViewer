#include "mainwindow.h"
#include "qapplication.h"
#include "configdialog.h"
#include "mousecontrolgraphview.h"
#include <QGraphicsScene>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QPixmap>
#include <QGraphicsPixmapItem>

#include <map>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar();
    setMenuBar(menuBar);

    // 创建“文件”菜单
    QMenu *fileMenu = menuBar->addMenu(tr("&文件"));
    // 创建配置
    QAction *configAction = menuBar->addAction(tr("配置"));
    connect(configAction, &QAction::triggered, this, &MainWindow::openConfigDialog);


    QAction *newAction = fileMenu->addAction(tr("新建"));
    connect(newAction, &QAction::triggered, this, [&](){
        // 需要实现先询问是否保存当前文件，再打开配置
        /*
         * ...
         */
        this->configData = [](){QJsonObject defaultState;defaultState["ConfigNotSaved"] = true; return defaultState;}();
        MainWindow::openConfigDialog();
    });

    QAction *openAction = fileMenu->addAction(tr("打开"));
    connect(openAction, &QAction::triggered, this, [&](){
        QString filter = "JSON Files (*.json) ;; All Files (*.*)";
        ConfigDialog::loadConfigFromFile(QFileDialog::getOpenFileName(this, "Select a file", "./", filter), configData);
        // 打开后需要 updateGraphicsScene 才符合直觉
        /*
         * ...
         */
    });

    QAction *saveAction = fileMenu->addAction(tr("保存为"));
    connect(saveAction, &QAction::triggered, this, [&](){
        /*
         * 现在这个更像另存为，保存应该可以记得文件路径
         */
        QString filter = "JSON Files (*.json) ;; All Files (*.*)";
        ConfigDialog::saveConfigToFile(QFileDialog::getSaveFileName(this, "Save file", "./", filter), configData);
    });

    fileMenu->addSeparator();

    // 创建"导出"子菜单
    QMenu *exportMenu = fileMenu->addMenu(tr("导出"));
    // 在"导出"子菜单中添加"导出为LaTeX"操作
    QAction *exportLatexAction = exportMenu->addAction(tr("导出为LaTeX"));
    //connect(exportLatexAction, &QAction::triggered, this, &YourClassName::exportAsLaTeX);
    connect(exportLatexAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("还没做好"),
                           tr("前面的区域，以后再来探索吧"));
    });


    // 添加“退出”动作到“文件”菜单
    QAction *exitAction = fileMenu->addAction(tr("退出"));
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);

    // 创建“关于”菜单
    QMenu *helpMenu = menuBar->addMenu(tr("&帮助"));

    // 添加“帮助”动作到“关于”菜单
    QAction *helpAction = helpMenu->addAction(tr("如何使用"));
    connect(helpAction, &QAction::triggered, this, [this]() {
        QString helpText = tr("这里是详细的帮助内容。"
                              "<p>您可以使用HTML标签来<b>格式化</b>文本，"
                              "例如使用<p><ul><li>项目符号</li>"
                              "<li>列表</li></ul>等等。</p>"
                              "<p>更多详细信息，请访问我们的<a href='http://example.com'>网站</a>。</p>");
        HelpDialog *dialog = new HelpDialog(helpText, this);
        dialog->exec(); // 显示对话框
    });

    // 添加“关于”动作到“关于”菜单
    QAction *aboutAction = helpMenu->addAction(tr("关于本程序"));
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("关于本程序"),
                           tr("这是一个使用Qt创建的示例应用程序。"));
    });

    // 添加默认的打开文件夹对话框
    //QPushButton *configButton = new QPushButton("没有打开文件夹，点击此处配置图片文件夹");
    //QGraphicsProxyWidget *proxyWidget = scene->addWidget(configButton);

    // 可以调整按钮的位置
    //proxyWidget->setPos(10, 10); // 例如，将按钮放在场景的左上角

    // 连接按钮的点击信号到打开配置对话框的槽
    //connect(configButton, &QPushButton::clicked, this, &MainWindow::openConfigDialog);


    view = new MouseControlGraphView(scene);

    // 添加一个测试椭圆到场景中
    //QGraphicsEllipseItem *ellipse = scene->addEllipse(10, 10, 100, 100);

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
}

void MainWindow::openConfigDialog() {
    ConfigDialog dialog(this, configData);
    if (dialog.exec() == QDialog::Accepted) {
        //保存填的东西
        configData = dialog.saveState();
        /*
        int newGridSize = dialog.getGridSize().toInt();
        if(newGridSize <= 0){
            // 弹出一个对话框，提示输入的数不符合要求，设为默认值1
            QMessageBox::warning(this, tr("输入错误"), tr("输入的网格大小不符合要求。将使用默认值3。"));
            newGridSize = 3; // 设置为默认值
        }
        QString newImagePath = dialog.getImagePath();
        updateGraphicsScene(newGridSize, newImagePath);
        */
        //std::cout << "accepted" ;

        QString folderPath = dialog.getImagePath();
        std::vector<std::pair<QString, int>> imageNameConfig = dialog.getImageNameConfig();
        //std::cout << newImagePath.toStdString();

        std::map<QString, VariableConfig> varcfg = dialog.getVariableConfigs();

        QString xVar;
        QString yVar;
        QStringList fixedVar;
        QStringList sliderVar;
        // 获取各类变量的类型，放到不同变量中处理
        for(auto &i : varcfg){
            switch(i.second.type){
            case 1:
                xVar = i.first;
                break;
            case 2:
                yVar = i.first;
                break;
            case 0:
                fixedVar.append(i.first);
                break;
            case 3:
                sliderVar.append(i.first);
            }
        }

        // 输入一个变量映射表，将imageNameConfig 转成 QString的函数
        // map中第一个是变量名，第二个是值
        auto toName = [&](const std::map<QString, QString> &m){
            QString str = "";
            for(auto& v:imageNameConfig) {
                str.append(v.second && m.count(v.first) ? m.at(v.first) : v.first);
            }
            return str;
        };

        //获得图片路径矩阵
        int xLen = varcfg[xVar].range.length();
        int yLen = varcfg[yVar].range.length();
        std::vector<std::vector<QString>> nameMatrix(yLen, std::vector<QString>(xLen));

        for(int i = 0; i < yLen; i++) {
            for (int j = 0; j < xLen; j++) {
                nameMatrix[i][j] = toName([&](){
                    std::map<QString, QString> m;
                    for(auto& v:varcfg)
                        m[v.first] = v.second.range[v.first == xVar ? j : (v.first == yVar ? i : 0)];
                    return m;
                }());
            }
        }

        // 添加图片，先使用文本检查是否正确
        // 清除现有场景中的所有内容
        scene->clear();

        // 计算每个文本项的大小和位置（这里可以根据需要进行调整）
        const int textSize = 50; // 假定每个文本项的基础大小
        const int imageSize = 1000;
        const qreal imageSpacing = 1.1;

        QDir dir(folderPath);
        for (int i = 0; i < yLen; ++i) {
            for (int j = 0; j < xLen; ++j) {
                // 有图片
                if(QFile::exists(dir.absoluteFilePath(nameMatrix[i][j]))) {
                    QPixmap pixmap(dir.absoluteFilePath(nameMatrix[i][j]));
                    QGraphicsPixmapItem *item = scene->addPixmap(pixmap.scaled(imageSize, imageSize, Qt::KeepAspectRatio));
                    item->setPos(j * imageSize * imageSpacing, i * imageSize * imageSpacing);
                } //没有图片用文本代替
                else {
                    QGraphicsTextItem *textItem = new QGraphicsTextItem(nameMatrix[i][j]);
                    QFont font = textItem->font();
                    font.setPixelSize(textSize); // 设置文本大小
                    textItem->setFont(font);

                    // 设置文本项的位置
                    textItem->setPos(j * imageSize * imageSpacing, i * imageSize * imageSpacing);
                    scene->addItem(textItem);
                }

            }
        }

        // 或许可以在遥远的地方加一些看不见的东西，这样就可以拖动很远


        //将变量的范围转为标准数组

        //得到变量名的map, 用于从imageName文本中生成图片路径

        //找到x轴和y轴，然后for两下得到所有有效的图片名，传递给updateGraphicsScene

        // for (const auto &config : variableConfigs) {
        //     std::cout << "类型:" << config.type.toStdString()
        //               << "，名称:" << config.name.toStdString()
        //               << "，范围:" << config.range.toStdString() << std::endl;
        //      根据收集到的数据进行进一步处理...

        // }
    }
}

void MainWindow::updateGraphicsScene(int gridSize, const QString &folderPath) {
    // 清除现有场景中的所有内容
    scene->clear();

    // 设置新的网格大小和图片路径
    this->gridSize = gridSize;
    this->imagePath = folderPath;

    // 计算每张图片的大小（这里简单地使用固定值，你可以根据需要调整）
    const int imageSize = 1000;

    QDir dir(folderPath);
    QStringList images = dir.entryList(QStringList() << "*.jpg" << "*.png", QDir::Files);
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            int index = (i * gridSize + j) % images.size(); // 循环使用图片
            QPixmap pixmap(dir.absoluteFilePath(images[index]));
            QGraphicsPixmapItem *item = scene->addPixmap(pixmap.scaled(imageSize, imageSize, Qt::KeepAspectRatio));
            item->setPos(i * imageSize * 1.1, j * imageSize * 1.1);
        }
    }
}
