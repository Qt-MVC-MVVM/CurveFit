#include "Model.h"


Model::Model()
{
	sp_Fuction = std::make_shared<Function>();
	sp_points = std::make_shared<Points>();
}

std::shared_ptr<Function> Model::getFunction()
{
	return std::shared_ptr<Function>();
}

void Model::opcf_createFunction(Type t)
{
	if (t == LINEAR_FUNCTION)
	{

	}
	else if (t == QUADRATIC_FUNCTION) {

	}
	else if(t == EXPONENTIAL_FUNCTION){

	}
	else if (t == LN_FUNCTION) {

	}
	else if (t == NORMAL_FUNCTION) {
	
	}
	else {

	}
	//��֪����ģ�飬model�����Function�Ѿ��ı�
	Fire_OnPropertyChanged("Fuction");
}
