#include "PageRank.h"
#include <mutex>
#include <sstream>
#include <iostream>
#include <fstream>


PageRank::PageRank(double beta, double epsilon,int block_num, char *input_file, char *output_file, char *output_file_all){
    this->beta = beta;
	this->epsilon = epsilon;
    this->input_file = input_file;
    this->output_file = output_file;
	this->output_file_all = output_file_all;
	this->block_num = block_num;
    //this->load_data();
	this->load_data_2();
}

PageRank::~PageRank(){
}

void PageRank::load_data()
{
	/*
	加载文件中的数据
	*/
    ifstream inf;

	inf.open(this->input_file, ios::in);

	int FromNodeID,ToNodeID;
	// 使用文件读取操作的成功与否作为循环的终止条件
	while (inf >> FromNodeID >> ToNodeID)//Data.txt 共有 135737 行
	{
		/*
		Data.txt
		一共 135737 行；
		最小节点为 1；
		最大节点为 8297；
		*/
		this->graph[FromNodeID].insert(ToNodeID);
		this->graph[ToNodeID];//若为空则会初始化；若不为空没有影响
	}
	this->node_count = this->graph.size();

	// cout<<this->node_count;

	//输出测试数据
	// for(int i=1;i<=10;i++){
	// 	cout<<"FromNodeID: "<<i<<endl;
	// 	cout<<"ToNodeID: ";
	// 	unordered_set<int> tmp=this->graph[i];
	// 	for(auto it=tmp.begin();it!=tmp.end();it++){
	// 		cout<<*it<<" ";
	// 	}
	// 	cout<<endl;
	// }

	inf.close();
}

long_d PageRank::compute_L1(unordered_map<int, long_d> old_rank, unordered_map<int, long_d> new_rank){
	//计算L1值
	long_d L1=0.0;
	for(int i=1;i<=this->node_count;i++){
		L1 += abs(old_rank[i]-new_rank[i]);
	}
	return L1;
}

void PageRank::rank_base(){
	// r^(old)
	for(int i=1;i<=this->node_count;i++){
		//初始化节点的PageRank值
		this->page_rank[i]=1.0/this->node_count;
	}
	// r^(new)
	unordered_map<int, long_d> new_rank=this->page_rank;

	long_d L1 = 1.0;//计算 r^(old) 和 r^(new) 的L1差值
	long_d S = 0.0;//计算 r^(new) 的和

	int round=1;//记录迭代轮数

	// 重复计算直到收敛（差值和小于epsilon）
	while(L1 > this->epsilon){
		// 初始化
		for(auto [key,value]:new_rank){
			new_rank[key] = 0.0;
		}

		// 解决 spider traps
		for(int j=1;j<=this->node_count;j++){
			// 计算r`^(new)_j
			unordered_set<int> from_j = graph[j];// 节点 j 的所有出度节点
			int out_degree = from_j.size();

			for(unordered_set<int>::iterator it=from_j.begin();it!=from_j.end();it++){
				// 更新 *it 节点
				new_rank[*it] += this->beta*(this->page_rank[j]*1.0/from_j.size());
			}
		}
		// 解决 dead ends
		// 计算 new_rank的和
		S = 0.0;
		for(auto [key,value]:new_rank){
			S += value;
		}
		// 更新
		for(int j=1;j<=this->node_count;j++){
			new_rank[j] = new_rank[j] + (1.0 - S)/(1.0 * this->node_count);
		}
		L1 = compute_L1(this->page_rank, new_rank);
		this->page_rank = new_rank;
		// cout<<L1<<" "<<S<<endl;
		cout<<"已经迭代了 "<<round++<<" 轮..."<<"现在的 L1 距离为 "<<L1<<endl;
	}

}


void PageRank::load_data_2()
{
	int max_node_num=0;
	int block_num=this->block_num;
		/*
	加载文件中的数据
	*/
    ifstream inf;

	inf.open(this->input_file, ios::in);

	int FromNodeID,ToNodeID;
	
	while (inf >> FromNodeID >> ToNodeID)//Data.txt 共有 135737 行
	{
		max_node_num=std::max(std::max(FromNodeID,ToNodeID),max_node_num);
	}
	inf.close();

	//每块的数量

	int step = max_node_num/block_num;

	//每块的名字

	auto belong_file = [&](int node_num)
	{
		int file_num=node_num/step+1;
		file_num = std::min(file_num,block_num);
		return file_num;
	};

	std::ofstream out;

	//每块输出一个文件，文件中对该块存一个表,每行为node：to_node1,to_node2......

	for(int i=1;i<=block_num;i++){
		this->graph.clear();
		inf.open(input_file);
		while(inf>>FromNodeID>>ToNodeID)
		{
			if(belong_file(FromNodeID)==i)
			{
				this->graph[FromNodeID].insert(ToNodeID);
			}
			if(belong_file(ToNodeID)==i)
			{
				this->graph[ToNodeID];
			}

		}
		this->node_count+=this->graph.size();
		out.open("block"+std::to_string(i));
		for(auto &edge:this->graph)
		{
			out<<edge.first;
			for(auto &ToNodeID:edge.second)
			{
				out<<" "<<ToNodeID;
			}
			out<<"\n";
		}
		inf.close();
		out.close();
		for(auto &[FromNodeID,ToNodeID]:this->graph)
		{
			this->page_rank[FromNodeID]=1.0/max_node_num;
		}
	}

}

void PageRank::rank_block()
{
	// r^(old)在load_data里赋值

	// r^(new)
	unordered_map<int, long_d> new_rank=this->page_rank;

	long_d L1 = 1.0;//计算 r^(old) 和 r^(new) 的L1差值
	

	int round=1;//记录迭代轮数

	while(L1>this->epsilon)
	{
		for(auto &[key,value]:page_rank)
		{
			new_rank[key]=0;
		}

		int from_node,to_node;
		ifstream input;
		string line;
		int num=0;
		for(int file_name=1;file_name<=block_num;file_name++)
		{
			this->graph.clear();
			input.open("block"+std::to_string(file_name));
			while(getline(input,line))
			{
				std::istringstream ss(line);
				ss>>from_node;
				this->graph[from_node];
				while (ss>>to_node)
				{
					this->graph[from_node].insert(to_node);
				}
			}

			input.close();
			
			//r^new
			for(auto &[from_node,to_nodes]:this->graph)
			{
				int out_degree=to_nodes.size();
				for(auto to_node:to_nodes)
				{
					new_rank[to_node]+=beta*(page_rank[from_node]/out_degree);
				}
			}
			
		}
		long_d sum=0,inc=0;
		for(auto &[node,rank]:new_rank)
		{
			sum+=rank;
		}
		inc=(1-sum)/node_count;
		for(auto &[node,rank]:new_rank)
		{
			rank+=inc;
		}

		L1=compute_L1(this->page_rank,new_rank);
		this->page_rank=new_rank;
		cout<<"已经迭代了"<<round++<<" 轮..."<<"现在的L1距离为"<<L1<<endl;
	}
}

void PageRank::mysort(){
	// map 转为 vector 类型
	// page_rank_sorted 的类型为 vector< pair<int, long_d> >
	for(auto x:this->page_rank){
		page_rank_sorted.push_back(x);
	}

	//从大到小排序，重写 compare 函数
	sort(page_rank_sorted.begin(),
		page_rank_sorted.end(),
		[](auto p1, auto p2){return p1.second == p2.second ? p1.first < p2.first : p1.second > p2.second; });
}

void PageRank::write_to_result(){
	// 将前 100 写入 this->output_file
	int rank = 1;
	ofstream outf;
	outf.open(this->output_file, ios::out);

    for (auto [key, value]:this->page_rank_sorted)
    {
        if (rank > 100)
            break;
        outf << key << " " << value << std::endl;
        rank++;
    }
    outf.close();


	// 将 全部 写入 this->output_file_all
	outf.open(this->output_file_all, ios::out);
    for (auto [key, value]:this->page_rank_sorted)
    {
        outf << key << " " << value << std::endl;
    }
    outf.close();
}