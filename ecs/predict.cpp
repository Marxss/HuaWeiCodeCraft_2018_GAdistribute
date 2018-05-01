#include "predict.h"
#include <cfloat>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctgmath>
#include <cmath>

#include "Flavor.h"
#include "Host.h"
#include "Problem.h"
#include "DataContainer.h"
#include "PredictResult.h"
#include "DistributeResult.h"
#include "GA_Distribute.h"
#include "Matrix.h"

using namespace std;


//��Ҫ��ɵĹ��������
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
    Problem problem(info);//��������
    problem.Problem_printf();
    DataContainer data_container(data,data_num,problem.v_flavors_info);//����ѵ������
    data_container.data_smooth3();
    data_container.DataPrintf();


    //TODO Ԥ��
    PredictResult predictResult(0,problem);//����Ԥ����
    predictResult.flavor_nums=problem.v_flavors_info;
    for(int i=0;i<problem.v_flavors_info.size();i++)
    {
        predictResult.flavor_nums[i].count=200;
        predictResult.totalnums+=200;
    }


    //TODO ����Ԥ��������������������Դ����
    GA_Distribute ga_distribute(100,100,0.3,0.15);//��ǰ���adjust82.644,����ǰ��adjust84.127
    DistributeResult distributeResultGA=ga_distribute.run(problem,predictResult);
    distributeResultGA.result_put_in_order();


    // ��Ҫ���������
    stringstream res;
    res<<predictResult.totalnums<<"\n";
    for(auto flavor:predictResult.flavor_nums)
        res<<flavor.name<<" "<<flavor.count<<"\n";
    if(distributeResultGA.normal_host_num>0){
        res<<"\nGeneral "<<distributeResultGA.normal_host_num<<"\n";
        for(int i=0;i<distributeResultGA.distribute_res_normal.size();i++)
        {
            res<<"General-"<<i+1<<" ";
            //res<<distributeResultGA.distribute_res_normal[i].compute_res_used_rate()<<" ";
            //if(distributeResultGA.distribute_res_normal[i].compute_res_used_rate()<0.9)return;
            for(auto flavor:distributeResultGA.distribute_res_normal[i].flavor_index)
            {
                res<<flavor.name<<" "<<flavor.count<<" ";
            }
            res<<"\n";
        }
    }
    if(distributeResultGA.mem_host_num>0){
        res<<"\nLarge-Memory "<<distributeResultGA.mem_host_num<<"\n";
        for(int i=0;i<distributeResultGA.distribute_res_mem.size();i++)
        {
            res<<"Large-Memory-"<<i+1<<" ";
            //res<<distributeResultGA.distribute_res_mem[i].compute_res_used_rate()<<" ";
            //if(distributeResultGA.distribute_res_mem[i].compute_res_used_rate()<0.9)return;
            for(auto flavor:distributeResultGA.distribute_res_mem[i].flavor_index)
            {
                res<<flavor.name<<" "<<flavor.count<<" ";
            }
            res<<"\n";
        }
    }

    if(distributeResultGA.cpu_host_num>0){
        res<<"\nHigh-Performance "<<distributeResultGA.cpu_host_num<<"\n";
        for(int i=0;i<distributeResultGA.distribute_res_cpu.size();i++)
        {
            res<<"High-Performance-"<<i+1<<" ";
            //res<<distributeResultGA.distribute_res_cpu[i].compute_res_used_rate()<<" ";
            //if(distributeResultGA.distribute_res_cpu[i].compute_res_used_rate()<0.9)return;
            for(auto flavor:distributeResultGA.distribute_res_cpu[i].flavor_index)
            {
                res<<flavor.name<<" "<<flavor.count<<" ";
            }
            res<<"\n";
        }
    }

    cout<<"********************distribute result file start**********************"<<endl;
    cout<<res.str().c_str();
    cout<<"********************distribute result file end**********************"<<endl;
    clock_t programTime = clock();
    cout << "Totle Time : " <<(double)programTime/ CLOCKS_PER_SEC << "s" << endl;

    double source_rate=distributeResultGA.compute_res_used_rate(predictResult);
    cout<<"source_rate="<<source_rate<<endl;
//    if(source_rate<0.99)
//        return;

    // ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
    write_result(res.str().c_str(), filename);
}