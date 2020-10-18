// �û��˺ͷ������˷��������
// �������û���ģ�飬�����Ͳ�ѯ����
//1.���ַ�ʽ��������
//1-1 �û��˶�ȡ�����ļ�������Query���ݣ�����
//1-2 �û���ֻ������QueryNumber, �������˶�ȡ�����ļ����ɻ�����ѯ���� �û����Ǹ�������
#include "mpi.h"
#include "DataLoader.hpp"
#include "Repartitioner.hpp"
#include "HeliosConfig.hpp"
#include "../server/server2.hpp"
#include "client2.hpp"

int main(int argc, char* argv[]) {
	//1.��������
	for (int i = 0; i < argc; i++) {
		cout << "argv[" << i << "] = " << argv[i] << ",";
	}
	cout << endl;
	/////////////////////////
	int numberOfClients;
	int numberOfQueries;
	string current_nodeName;
	vector<boost::thread> clientThread_list;

	////////////////////////// Validate the number of command line parameters.
	if (argc < 3) errorReport("Usage: ./server <numberOfClients> <numberOfQueries>");
	numberOfClients = atoi(argv[1]);
	numberOfQueries = atoi(argv[2]);

	if (numberOfClients < 1) errorReport("At least one client is required!");
	if (numberOfQueries < 1) errorReport("At least one message needs to be sent!");

	////////////////////////////////
	MPI_Init(NULL, NULL);

	int32_t global_num_servers;
	MPI_Comm_size(MPI_COMM_WORLD, &global_num_servers);

	int32_t world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


	////////////////////////////////
	HeliosConfig config("../helios.cfg", global_num_servers);
	current_nodeName = config.host_names[(int)world_rank];
	assert(global_num_servers == config.global_num_servers);
	//shared by all threads
	//Memory* mem = new Memory(global_num_servers);

	////////////////////////////////
	//1.�û��˶�ȡ���ɲ�ѯ
	//add workload info
	vector<QueryBank> vec_workload = get_workload_config_info(&config); //ͨ�������ļ����ɲ�ѯ

	//������ѯ����
	for (int client_nbr = 1; client_nbr <= numberOfClients; client_nbr++)
		clientThread_list.push_back(boost::thread(boost::bind(&client_task2, current_nodeName,
			numberOfQueries, client_nbr, &config, vec_workload)));

	////////////////////////////////
	// Wait for terminating the clients and servers.
	for (int client_nbr = 1; client_nbr <= numberOfClients; client_nbr++)
		clientThread_list[client_nbr - 1].join();

	//��ӡ : �û�query task ���̽���
	cout << "client task thread end " << endl;
	//delete mem;
	////////////////////////////////
	MPI_Finalize();

	return 0;
}
