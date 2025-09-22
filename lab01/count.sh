#!/bin/bash

#统计文件数
count_files=$(find . -maxdepth 1 -type f | wc -l)

#统计子目录数
count_dirs=$(find . -maxdepth 1 -mindepth 1 -type d | wc -l)

#统计文件总行数
count_lines=$(find . -maxdepth 1 -type f -exec wc -l {} + | tail -n 1 | awk '{print $1}')

echo "当前目录文件总数：$count_files"
echo "当前目录子目录总数：$count_dirs"
echo "当前目录文件内容总行数：$count_lines"
