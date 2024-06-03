/*
防止头文件被重复包含的预处理器指令。
*/
#ifndef __PAGERANK_H_
#define __PAGERANK_H_

#include<iostream>
#include<fstream>

#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

typedef long double long_d;

class PageRank
{
private:
    /*
    beta为PageRank算法中的 teleport parameter
    */
    double beta;

    /*
    */
    double epsilon;

    /*
    分块数量
    */
   int block_num;

    /*
    graph存储图网络 FromNodeID -> ToNodeID
    page_rank存储每个节点的PageRank值
    page_rank_sorted存储根据value排序后的数据
    */
    unordered_map<int, unordered_set<int>> graph;
    unordered_map<int, long_d> page_rank;
    vector<pair<int, long_d>> page_rank_sorted;

    /*
    输入文件和输出文件
    */
   char* input_file;
   char* output_file;
   char* output_file_all;

   //节点数量
   int node_count=0;

public:
    PageRank(double beta, double epsilon,int block_num, char *input_file, char *output_file, char* output_file_all);
    ~PageRank();

    void load_data();//加载 input_file 文件
    void rank_base();// 基本的 PageRank 算法（不含 优化稀疏矩阵 和 实现分块计算）
    void load_data_2();
    void rank_block();
    long_d compute_L1(unordered_map<int, long_d> old_rank, unordered_map<int, long_d> new_rank);// 计算 L1 误差值
    void mysort();// 排序
    void write_to_result();// 将排序结果写入 output_file 和 output_file_all
};

#endif