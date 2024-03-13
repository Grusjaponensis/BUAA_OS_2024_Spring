# sed命令
 1. 替换文本
`sed 's/old_text/new_text/' input_file`

 2. 删除行
 `sed '/pattern/d' input_file`

 3. 显示文件指定的行
 `sed -n 'p' input_file`

 4. 插入和追加行
 `sed '1i\new_line' input_file` 在第一行前插入新行
 `sed '1a\new_line' input_file` 在第一个行后追加行

 5. 全局替换(替换每一行的对应内容)
 `sed 's/old_text/new_text/g input_file` 

# find命令
1. **基本语法**：
   ```bash
   find [path] [options] [expression]
   ```
   - `path`：指定要搜索的路径，默认为当前目录。
   - `options`：控制搜索的行为，如 `-type`、`-name` 等。
   - `expression`：定义搜索条件。

2. **常见选项**：
   - `-name pattern`：按文件名模式匹配搜索文件。
   - `-type type`：按文件类型搜索，如 `f`（普通文件）、`d`（目录）、`l`（符号链接）等。
   - `-size [+|-]size[c]`：按文件大小搜索，可以使用 `+` 或 `-` 来指定大小范围，`c` 表示以字节为单位。
   - `-mtime [+|-]n`：按文件的修改时间搜索，`+n` 表示在 `n` 天之前修改过的文件，`-n` 表示在 `n` 天内修改过的文件。
   - `-exec command {} \;`：对搜索到的文件执行指定的命令。
   - `-print`：将搜索到的文件名输出到标准输出。

3. **常见用法示例**：
   - 在当前目录下搜索名为 `file.txt` 的文件：
     ```bash
     find . -name "file.txt"
     ```
   - 搜索文件大小超过 1MB 的文件：
     ```bash
     find . -size +1M
     ```
   - 搜索修改时间在 7 天以内的文件：
     ```bash
     find . -mtime -7
     ```
   - 执行对搜索到的文件执行 `ls -l` 命令：
     ```bash
     find . -exec ls -l {} \;
     ```

# grep命令
1. **基本语法**：
   ```bash
   grep [options] pattern [file ...]
   ```
   - `pattern`：要搜索的模式，可以是简单字符串或者正则表达式。
   - `file`：指定要搜索的文件，如果省略，则从标准输入中读取。

2. **常见选项**：
   - `-i`：忽略大小写。
   - `-r`：递归搜索目录中的文件。
   - `-n`：显示匹配行的行号。
   - `-v`：显示不匹配的行。
   - `-w`：只匹配整个单词。
   - `-e pattern`：指定多个模式。
   - `-E`：使用扩展的正则表达式（支持更多元字符）。
   - `-A num`：显示匹配行及其后 num 行。
   - `-B num`：显示匹配行及其前 num 行。
   - `-C num`：显示匹配行及其前后各 num 行。

3. **常见用法示例**：
   - 在文件中搜索匹配的文本行：
     ```bash
     grep pattern file.txt
     ```
   - 在目录中递归搜索匹配的文件：
     ```bash
     grep -r pattern directory
     ```
   - 忽略大小写搜索匹配的文本行：
     ```bash
     grep -i pattern file.txt
     ```
   - 显示匹配行及其行号：
     ```bash
     grep -n pattern file.txt
     ```
   - 显示不匹配的文本行：
     ```bash
     grep -v pattern file.txt
     ```
   - 使用扩展的正则表达式搜索：
     ```bash
     grep -E 'pattern1|pattern2' file.txt
     ```
