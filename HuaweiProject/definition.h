#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

//����������
struct SeverType
{
	//�������ͺ�
	string name;
	//ÿ���ڵ�CPU�������ܺ���/2��
	unsigned int cores;
	//ÿ���ڵ��ڴ棨���ڴ�/2��
	unsigned int memory;
	//Ӳ���ɱ�
	unsigned int price;
	//ÿ���ܺĳɱ�
	unsigned int costPerDay;
};

//���������
struct VmwareType
{
	//������ͺ�
	string name;
	//ռ��CPU����
	unsigned int cores;
	//ռ���ڴ�
	unsigned int memory;
	//�Ƿ�˫�ڵ㲿��
	bool isDouble;
};

//��������
struct RequestType
{
	//�Ƿ�������������Ϊɾ����
	bool isAdd;
	//������ͺ�
	string name;
	//�����ID
	unsigned int ID;
};

