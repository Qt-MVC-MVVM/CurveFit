#include <QtDebug>
#include "Model.h"
#define precision 1e-13
#define STARTLNFUNCT 1e-4

Model::Model()
{
	sp_Function = std::make_shared<Function>();
	samplePoints = std::make_shared<Points>();
	real_xy_points = std::make_shared<Points>();
	map_to_img_xy = std::make_shared<Points>();
	range_x = std::make_shared<Point>();
	range_y = std::make_shared<Point>();
}
std::shared_ptr<Function> Model::getFunction()
{
	return sp_Function;
}

std::shared_ptr<Points> Model::getRealPoints()
{
	return real_xy_points;
}

std::shared_ptr<Point> Model::getRangeX()
{
	return range_x;
}

std::shared_ptr<Point> Model::getRangeY()
{
	return range_y;
}

bool Model::opcf_fit(Param_opcf& p)
{
	Type t = p.get_type();
	Points sp_points = p.get_points();
	(*sp_Function).set_type(t);


	/*send sample points to model*/
	samplePoints->clear();
	for (auto i = 0; i < sp_points.size(); i++)
	{
		Point point = sp_points[i];
		samplePoints->push_back(point);
	}


#ifndef NDEBUG
	qDebug() << "Int Create Function\n";
	qDebug() << "Type: " <<t;
	qDebug() << "\n point number" << sp_points.size();
	qDebug() << "\n";
	qDebug() << "Test if pass to model" << samplePoints->size();
#endif // !NDEBUG

	/*fuction fit*/

	if (t == LINEAR_FUNCTION)
	{
		double ave_x, ave_y, sum_xy, sum_qx;
		double a, b;
		int n;
		std::string func;
		ave_x = ave_y = sum_xy = sum_qx = 0.0;
		n = sp_points.size();
		for (int i = 0; i < n; i++) {
			ave_x += sp_points[i].getx();
			ave_y += sp_points[i].gety();
			sum_xy += sp_points[i].getx() * sp_points[i].gety();
			sum_qx += sp_points[i].getx() * sp_points[i].getx();
		}
		ave_x /= n;
		ave_y /= n;
		b = (sum_xy - n * ave_x * ave_y) / (sum_qx - n * ave_x * ave_x);
		a = ave_y - b * ave_x;
		if (b != 0) {
			func += std::to_string(b);
			func += "x";
		}
		if (a < 0) {
			func += '-';
			a *= -1;
			func += std::to_string(a);
			func += "\0";
		}
		else if (a > 0) {
			func += '+';
			func += std::to_string(a);
			func += '\0';
		}
		else if (a == 0) func += '\0';
		sp_Function->set_function(func);
	}
	else if (t == QUADRATIC_FUNCTION) {
		int n = sp_points.size();
		double a,b,c,m1,m2,m3,z1,z2,z3;
		double sumx = 0, sumx2 = 0, sumx3 = 0, sumx4 = 0, sumy = 0, sumxy = 0, sumx2y = 0;
		a = b = c = 0;
		for (int i = 0; i < n; i++) {
			sumx += sp_points[i].getx();
			sumx2 += sp_points[i].getx() * sp_points[i].getx();
			sumx3 += sp_points[i].getx() * sp_points[i].getx() * sp_points[i].getx();
			sumx4 += sp_points[i].getx() * sp_points[i].getx() * sp_points[i].getx() * sp_points[i].getx();
			sumy += sp_points[i].gety();
			sumxy += sp_points[i].getx() * sp_points[i].gety();
			sumx2y += sp_points[i].getx() * sp_points[i].getx() * sp_points[i].gety();
		}
		do { 
			m1 = a;a = (sumx2y - sumx3 * b - sumx2 * c) / sumx4; z1 = (a - m1) * (a - m1);
			m2 = b; b = (sumxy - sumx * c - sumx3 * a) / sumx2; z2 = (b - m2) * (b - m2);
			m3 = c; c = (sumy - sumx2 * a - sumx * b) / n; z3 = (c - m3) * (c - m3); 
		} while ((z1 > precision) || (z2 > precision) || (z3 > precision));
		std::string func;
		if (a != 0) {
			func += std::to_string(a);
			func += "x^2";
		}
		if (b != 0) {
			if (b < 0) {
				func += std::to_string(b);
				func += 'x';
			}
			else {
				func += '+';
				func += std::to_string(b);
				func += 'x';
			}
		}
		if (c != 0) {
			if (c < 0)func += std::to_string(c);
			else {
				func += '+';
				func += std::to_string(c);
			}
		}
		func += '\0';
		sp_Function->set_function(func);
	}
	else if (t == EXPONENTIAL_FUNCTION) {
		int n = sp_points.size();
		double a, b, Inb;
		double sumx = 0, sumIny = 0, sumx2 = 0, sumxIny = 0;
		for (int i = 0; i < n; i++) {
			sumx += sp_points[i].getx();
			sumIny += log(sp_points[i].gety());
			sumx2 += sp_points[i].getx() * sp_points[i].getx();
			sumxIny += sp_points[i].getx() * log(sp_points[i].gety());
		}
		a = (n * sumxIny - sumx * sumIny) / (n * sumx2 - sumx * sumx);
		Inb = (sumx2 * sumIny - sumxIny * sumx) / (n * sumx2 - sumx * sumx);
		b = exp(Inb);
		std::string func;
		if (b == 0)func += "0\0";
		else {
			func += std::to_string(b);
			func += 'e';
			if (a == 0)func += '\0';
			else {
				func += '^';
				func += std::to_string(a);
				func += "x\0";
			}
		}
		sp_Function->set_function(func);
	}
	else if (t == LN_FUNCTION) {
		int n = sp_points.size();
		double Ina, a, b;
		double sumyInx = 0, sumy = 0, sumInx = 0, sumInx2 = 0;
		std::string func;
		for (int i = 0; i < n; i++) {
			sumyInx += sp_points[i].gety() * log(sp_points[i].getx());
			sumy += sp_points[i].gety();
			sumInx += log(sp_points[i].getx());
			sumInx2 += log(sp_points[i].getx()) * log(sp_points[i].getx());
		}
		b = (n * sumyInx - sumy * sumInx) / (n * sumInx2 - sumInx * sumInx);
		Ina = (sumy * sumInx - b * sumInx * sumInx) / (n * b * sumInx);
		a = exp(Ina);
		func += std::to_string(b);
		func += "Ln";
		func += std::to_string(a);
		func += "x\0";
		sp_Function->set_function(func);


	}
	else if (t == NORMAL_FUNCTION) {

	}
	else {

	}

	/*get xy*/
	bool whether_get_real_points;
	whether_get_real_points = get_realXYPoints(t);
	range_x->setx((this->get_min_real_x()));
	range_x->sety((this->get_max_real_x()));
	range_y->setx((this->get_min_real_y()));
	range_y->sety((this->get_max_real_y()));

	/*map to x y in img*/



#ifndef NDEBUG
	qDebug() << "End of opcf_fit and the function is" << QString::fromStdString((*sp_Function).get_function()) << "\n";
	qDebug() << "And we have " << real_xy_points->size() << " points to be painted\n";
	qDebug() <<"Fire_OnPropertyChanged(Function) \n";
#endif // !NDEBUG

	//inform other section
	//
	Fire_OnPropertyChanged("Function");
	return true;
}

bool Model::get_realXYPoints(Type t)
{
	double x;
	double y;
	double start_x;
	double end_x;
	double length;
	double step;
	start_x = get_min_sample_x();
	end_x = get_max_sample_x();
	length = end_x - start_x;
#ifndef NDEBUG
	qDebug() << "In get_realXYPoints(Type):\n" << "X Range of sample Points  " << start_x << "-" << end_x<<"\n";
	qDebug()<<"Function: "<< QString::fromStdString(sp_Function->get_function());
#endif // !NDEBUG
	switch (t)
	{
	case LINEAR_FUNCTION: {
		start_x = start_x - length;
		end_x = end_x + length;
		step = (end_x - start_x) / POINTSNUMBER;
	}
		break;
	case QUADRATIC_FUNCTION: {
		start_x = start_x - length;
		end_x = end_x + length;
		step = (end_x - start_x) / POINTSNUMBER;
	}
		break;
	case EXPONENTIAL_FUNCTION: {
		start_x = start_x - length;
		end_x = end_x + length;
		step = (end_x - start_x) / POINTSNUMBER;
	}
		break;
	case LN_FUNCTION: {
		start_x = start_x - length;
		end_x = end_x + length;
		if (start_x <= 0) start_x = STARTLNFUNCT;
		step = (end_x - start_x) / POINTSNUMBER;
	}
		break;
	case NORMAL_FUNCTION: {
		step = (end_x - start_x) / POINTSNUMBER;
	}
		break;
	default: {
		step = (end_x - start_x) / POINTSNUMBER;
	}
		break;
	}

	real_xy_points->clear();
	x = start_x;
	y = sp_Function->get_y(x);
	for (int i = 0; i < POINTSNUMBER; i++) {
		Point t;
		t.setx(x);
		t.sety(y);
#ifndef NDEBUG
		qDebug() << "In get_realXYPoints(Type):\n" << "x  " << x << "y " << y << "\n";
#endif // !NDEBUG
		real_xy_points->push_back(t);
		x += step;
		y = sp_Function->get_y(x);
	}
	return true;
}

bool Model::get_mappedXYPoints()
{
	return false;
}

double Model::get_min_real_x()
{
	double min = ((*real_xy_points)[0]).getx();
	for (auto i = 1; i < real_xy_points->size(); i++) {
		if (min > ((*real_xy_points)[i]).getx())
		{
			min = ((*real_xy_points)[i]).getx();
		}
	}
	return min;
}

double Model::get_max_real_x()
{
	double max = ((*real_xy_points)[0]).getx();
	for (auto i = 1; i < real_xy_points->size(); i++) {
		if (max < ((*real_xy_points)[i]).getx())
		{
			max = ((*real_xy_points)[i]).getx();
		}
	}
	return max;
}

double Model::get_min_real_y()
{
#ifndef NDEBUG
	qDebug() << "In get_min_real_y()\n";
#endif // !NDEBUG
	double min = ((*real_xy_points)[0]).gety();
	for (auto i = 1; i < real_xy_points->size(); i++) {
		if (min > ((*real_xy_points)[i]).gety())
		{
			min = ((*real_xy_points)[i]).gety();
		}
	}
#ifndef NDEBUG
	qDebug() << "min"<<min<<"\n";
#endif // !NDEBUG
	return min;
}

double Model::get_max_real_y()
{
	double max = ((*real_xy_points)[0]).gety();
	for (auto i = 1; i < real_xy_points->size(); i++) {
		if (max < ((*real_xy_points)[i]).gety())
		{
			max = ((*real_xy_points)[i]).gety();
		}
	}
	return max;
}

double Model::get_min_sample_x()
{
	double min = (*samplePoints)[0].getx();
	for (auto i = 1; i < samplePoints->size(); i++) {
		if (min > (*samplePoints)[i].getx())
		{
			min = (*samplePoints)[i].getx();
		}
	}
	return min;
}

double Model::get_max_sample_x()
{
	double max = (*samplePoints)[0].getx();
	for (auto i = 1; i < samplePoints->size(); i++) {
		if (max < (*samplePoints)[i].getx())
		{
			max = (*samplePoints)[i].getx();
		}
	}
	return max;
}