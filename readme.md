<h1>Matrix Graph Viewer 帮助文档</h1>

view pictures with naming conventions in grid format.

以网格形式批量查看有命名规范的图片。

<h2>变量命名规范</h2>

本程序支持设定两个变量作为行和列，以矩阵的方式展示不同图片。

在配置中的文本输入框中使用 ` ' 将字符括起来后，` ' 内的字符将被视作变量。

<h3>变量支持的值/范围格式</h3>

<ul>
<li>a~b : a,b需为整数，变量将会以1为步长遍历区间[a, b]</li>
<li>a1,a2,a3, ... : a可为任意字符串，变量会遍历以 , 间隔的每一个值</li>
<li>a : 一个定值，可以在变量选择为“定值”时使用</li>
</ul>

<h2>使用方法</h2>

<ol>
<li>点击“配置”来打开配置窗口，</li>
<li>选择文件夹</li>
<li>输入图片名称、行列标签，你可以使用变量来遍历图片</li>
<li>在配置窗口的变量列表处点击“添加变量”，输入你的变量，选定其作为定值、行或是列，设置变量的范围</li>
<li>点击应用，查看图片，可使用滚轮缩放，鼠标左键拖动</li>
<li>可以在“文件”菜单中保存此次的配置文件，以便下次使用</li>
</ol>


<h2>示例配置</h2>

可复制到新文本文档中保存，使用本程序打开

<pre>
{
    "folderPath": "/home/yuchen/Documents/Qt/Hermite_Wronskian_plot_2",
    "pictureName": "`m',`N',`k',`l'.png",
    "secXAxisLabel": "Re(x)",
    "secYAxisLabel": "Im(x)",
    "variables": [
        {
            "name": "m",
            "range": "5",
            "type": 0
        },
        {
            "name": "N",
            "range": "4",
            "type": 0
        },
        {
            "name": "k",
            "range": "3~10",
            "type": 2
        },
        {
            "name": "l",
            "range": "2,3,4,5,6,7,8,9",
            "type": 1
        }
    ],
    "xAxisLabel": "l=`l'",
    "yAxisLabel": "k=`k'"
}
<code>


<h1>TODO List</h1>

<ul>
<li>x~y 格式支持自定义步长，计划以x~y,d的格式</li>
<li>将图片导出为LateX</li>
<li>优化“打开”“保存”“新建”的逻辑</li>
<li>在主界面可直接调整“定值”变量的值</li>
<li>添加图标（进度：80%，图标做好但不知道怎么加上去）</li>
<li>完善关于界面</li>
<li>完善帮助文档</li>
<li>多语言支持</li>
<li>复选框：给每张图片增加外框</li>
<li>可使用\` \'转义字符</li>
</ul>
