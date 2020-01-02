# Huffman_code_zip
使用Huffman编码算法对文件进行压缩解压

# 代码清单
Huffman.h 实现Huffman 类，包括构建Huffman树和得到huffman编码的方法
zip&unzip.cpp 实现FileCompress类 ，包括调用Huffman类的方法实现文件压缩和解压

# 使用方法
在zip&unzip.cpp中的mian函数中调用压缩和解压算法

# 压缩支持文件
目前能实现对文本文件的无损压缩和解压。对于其他文件类型目前还支持不了。
后续考虑改进一下以支持多类型文件和文件夹压缩。
