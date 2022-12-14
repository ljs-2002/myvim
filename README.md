# myvim

## 实现功能

| 功能           | 描述                                                         |
| -------------- | ------------------------------------------------------------ |
| 正常的编辑功能 | 包括通过方向键移动光标改变插入位置，<br />在任意地方换行，在任意地方删除 |
| Tab缩进        | 按下Tab即可输入4个空格                                       |
| 保存文件       | 按`ctrl +S`可以保存文件                                      |
| 退出编辑器     | 按`ctrl + C`可以退出编辑器                                   |

## 待添加

| 功能       | 预期                                                         |状态|
| ---------- | :----------------------------------------------------------- | ---------- |
|自动补齐|输入`()`等符号时自动补齐另一半|已实现(1.1)|
|自动缩进|当输入`{`时下一行自动缩进4格，换行保持缩进|未实现|
| 命令模式   | 与vim类似的按`i`进入输入模式，<br />按`Esc`回到命令模式，在命令模式下<br />可以使用`hjkl`来移动光标，使用`:wq`来写如和退出 |部分实现(1.1)|
| 底部状态条 | 在底部固定显示总字数，当前光标所在行列等信息 |未实现|
| ... | ... |...|

## 使用方法

​	在当前目录下使用下列命令编译产生`myvim`。

```shell
make
```

​	在当前目录下使用下列命令开始编辑文件`file`

```shell 
./myvim file
```

​	按`Ctrl + S`可保存文件。按`Ctrl + C`可以退出编辑器（不保存）

### 1.1新增

​	当输入`(`、`[`、`‘`、`“`、`{`时将会自动补齐另一半括号或引号，并且将光标置于括号/引号中间。	

​	进入编辑器默认处于命令模式下，按`i`进入编辑模式。在命令模式下无法编辑文件，可以使用`hjkl`来移动光标，也可以使用回车、`Backspace`、方向键来移动光标。
